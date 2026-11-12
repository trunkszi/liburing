#include <sys/socket.h>

#include <atomic>
#include <cstring>
#include <uring/sys/mmap_region.hpp>

#include "../../consteval/ops/encoding_cases.hpp"
namespace {
using namespace uring;
using namespace uring::ops;
constexpr bool ChainStampPreserved() {
  using namespace uring;
  using namespace uring::ops;
  const auto token = MakeToken(5, 0, 7, 1);
  const auto member = MakeToken(5, 1, 7, 1);
  abi::FlatSqe sqes[2]{};
  sqes[0].flags = abi::kSqeIoLink;
  sqes[0].user_data = token.raw();
  sqes[1].user_data = member.raw();
  PrepareSend(3, 0x1000, 16).WithToken(token).Encode(sqes[0]);
  PrepareLinkTimeout(nullptr).WithToken(token).Encode(sqes[1]);
  if ((sqes[0].flags & abi::kSqeIoLink) == 0 || sqes[1].user_data != member.raw()) return false;
  PrepareLinkTimeout(nullptr).Encode(sqes[1]);
  if (sqes[1].user_data != member.raw()) return false;
  sqes[0].flags = abi::kSqeIoHardlink;
  const auto own = MakeToken(9, 0, 8, 2);
  PrepareNop().WithToken(own).Encode(sqes[0]);
  if (sqes[0].flags != abi::kSqeIoHardlink || sqes[0].user_data != own.raw()) return false;
  abi::FlatSqe128 wide{};
  wide.sqe.flags = abi::kSqeIoLink;
  wide.sqe.user_data = member.raw();
  PrepareNop128().WithToken(token).Encode(wide);
  if (wide.sqe.flags != abi::kSqeIoLink || wide.sqe.user_data != member.raw()) return false;
  std::array<std::byte, 80> payload{};
  payload[79] = std::byte{0x5a};
  PrepareUringCmd128(7, 9, payload).WithToken(token).Encode(wide);
  if (wide.sqe.flags != abi::kSqeIoLink || wide.sqe.user_data != member.raw() || wide.extension[7] != 0x5a00000000000000ULL)
    return false;
  PrepareUringCmd128(7, 9, payload).Encode(wide);
  return wide.sqe.flags == abi::kSqeIoLink && wide.sqe.user_data == member.raw();
}
constexpr bool DirectFileSlots() {
  using namespace uring;
  using namespace uring::ops;
  for (const auto slot : std::array<std::uint32_t, 4>{0, 3, 0xfffffffeU, abi::kFileIndexAlloc}) {
    const auto expected = slot == abi::kFileIndexAlloc ? abi::kFileIndexAlloc : slot + 1U;
    abi::FlatSqe actual{};
    PrepareOpenatDirect(-100, 0x1000, 0, 0, slot).Encode(actual);
    if (actual.file_index() != expected) return false;
    PrepareSocketDirect(2, 1, 0, slot).Encode(actual);
    if (actual.file_index() != expected) return false;
    PrepareAcceptDirect(3, nullptr, nullptr, slot).Encode(actual);
    if (actual.file_index() != expected) return false;
    PrepareMsgRingSendFd(4, 5, slot, 6).Encode(actual);
    if (actual.file_index() != expected) return false;
  }
  return true;
}
class NativeRing {
  sys::UniqueFd fd_;
  sys::RingMappings mappings_;
  abi::IoUringParams params_;
  NativeRing(sys::UniqueFd fd, sys::RingMappings mappings, abi::IoUringParams params) noexcept
      : fd_(std::move(fd)), mappings_(std::move(mappings)), params_(params) {}
  static std::atomic_ref<std::uint32_t> Cursor(std::span<std::byte> memory, std::uint32_t offset) noexcept {
    return std::atomic_ref<std::uint32_t>(*reinterpret_cast<std::uint32_t*>(memory.data() + offset));
  }

 public:
  NativeRing(NativeRing&&) noexcept = default;
  [[nodiscard]] static Result<NativeRing> Open() noexcept {
    abi::IoUringParams params{};
    auto raw = ToResult(sys::SysIoUringSetup(8, &params));
    if (!raw) return std::unexpected(raw.error());
    auto fd = sys::UniqueFd::Adopt(static_cast<std::int32_t>(*raw));
    if (!fd) return std::unexpected(fd.error());
    auto mappings = sys::RingMappings::Map(fd->get(), params);
    if (!mappings) return std::unexpected(mappings.error());
    return NativeRing{std::move(*fd), std::move(*mappings), params};
  }
  bool RegisterSparseFiles() noexcept {
    abi::IoUringRsrcRegister registration{.nr = 4, .flags = abi::kRsrcRegisterSparse};
    return sys::SysIoUringRegister(fd_.get(), abi::kRegisterFiles2, &registration, sizeof(registration)) == 0;
  }
  template <OpDescriptor Op>
  Result<std::int32_t> Run(const Op& op) noexcept {
    static_assert(Op::kSqeSlots == 1 && !Op::kUntracked);
    const auto tail = Cursor(mappings_.sq(), params_.sq_off.tail).load(std::memory_order_relaxed);
    const auto index = tail & (params_.sq_entries - 1);
    const auto token = MakeToken(1, 0, 7, tail + 1);
    abi::FlatSqe sqe{};
    op.WithToken(token).Encode(sqe);
    std::memcpy(mappings_.sqes().data() + static_cast<std::size_t>(index) * 64, &sqe, sizeof(sqe));
    auto* array = reinterpret_cast<std::uint32_t*>(mappings_.sq().data() + params_.sq_off.array);
    array[index] = index;
    Cursor(mappings_.sq(), params_.sq_off.tail).store(tail + 1, std::memory_order_release);
    const auto submitted = ToResult(sys::SysIoUringEnter(fd_.get(), 1, 1, abi::kEnterGetevents));
    if (!submitted) return std::unexpected(submitted.error());
    if (*submitted != 1) return std::unexpected(UringErrorCode::kProtocolViolation);
    auto head = Cursor(mappings_.cq(), params_.cq_off.head).load(std::memory_order_relaxed);
    if (head == Cursor(mappings_.cq(), params_.cq_off.tail).load(std::memory_order_acquire))
      return std::unexpected(UringErrorCode::kProtocolViolation);
    abi::FlatCqe cqe{};
    std::memcpy(&cqe,
                mappings_.cq().data() + params_.cq_off.cqes + static_cast<std::size_t>(head & (params_.cq_entries - 1)) * 16,
                sizeof(cqe));
    Cursor(mappings_.cq(), params_.cq_off.head).store(head + 1, std::memory_order_release);
    if (cqe.user_data != token.raw()) return std::unexpected(UringErrorCode::kInvalidToken);
    return cqe.res;
  }
};
bool KernelSmoke() {
  const char name[] = "typed-ops";
  const std::array<char, 9> input{'u', 'r', 'i', 'n', 'g', '-', 'o', 'p', 's'};
  std::array<char, 9> output{};
  std::uint32_t word = 0;
  abi::FutexWaitv waiter{.val = 1, .uaddr = sys::Address(&word), .flags = abi::kFutex2SizeU32};
  auto memfd = ToResult(sys::Syscall6(sys::kSysMemfdCreate, sys::Address(name), 1));
  if (!memfd) return false;
  auto file = sys::UniqueFd::Adopt(static_cast<std::int32_t>(*memfd));
  auto ring = NativeRing::Open();
  if (!file || !ring) return false;
  auto nop = ring->Run(PrepareNop());
  auto written = ring->Run(PrepareWrite(file->get(), input.data(), 9, 0));
  auto read = ring->Run(PrepareRead(file->get(), output.data(), 9, 0));
  auto mismatch = ring->Run(PrepareFutexWait(&word, 1));
  auto wake = ring->Run(PrepareFutexWake(&word, 0));
  auto waitv = ring->Run(PrepareFutexWaitv(&waiter, 1));
  return nop && *nop == 0 && written && *written == 9 && read && *read == 9 && input == output && mismatch &&
         *mismatch == -11 && wake && *wake == 0 && waitv && *waitv == -11;
}
bool KernelDirectFile() {
  auto ring = NativeRing::Open();
  if (!ring || !ring->RegisterSparseFiles()) return false;
  const auto opened = ring->Run(PrepareOpenatDirect(-100, "/dev/null", 0, 0, 0));
  return opened && *opened == 0;
}
bool WidePayload() {
  volatile std::uint8_t seed = 13;
  std::array<std::byte, 80> payload{};
  for (std::size_t i = 0; i < payload.size(); ++i) payload[i] = static_cast<std::byte>(i + seed);
  abi::FlatSqe128 encoded{};
  const auto token = MakeToken(2, 0, 7, 9);
  PrepareUringCmd128(19, 0x11223344, payload, abi::kUringCmdFixed).WithToken(token).Encode(encoded);
  return encoded.sqe.user_data == token.raw() && encoded.sqe.cmd_op() == 0x11223344 &&
         std::memcmp(encoded.command().data(), payload.data(), 80) == 0;
}
}  // namespace
bool ErgonomicBuffersAndFutex() {
  std::array<std::byte, 4> bytes{};
  std::array<std::uint8_t, 4> u8{};
  abi::FlatSqe sqe{};
  auto check = [&](const auto& op, std::uint64_t address, std::uint32_t flags) {
    op.Encode(sqe);
    return sqe.addr() == address && sqe.len == 4 && sqe.op_flags() == flags;
  };
  auto read = PrepareRead(3, std::span{bytes}, 9, 7);
  auto read_u8 = PrepareRead(3, std::span{u8});
  auto write = PrepareWrite(3, std::span<const std::byte>{bytes});
  auto write_u8 = PrepareWrite(3, std::span<const std::uint8_t>{u8});
  auto recv = PrepareRecv(3, std::span{bytes});
  auto recv_u8 = PrepareRecv(3, std::span{u8});
  auto send = PrepareSend(3, std::span<const std::byte>{bytes}, MSG_NOSIGNAL);
  auto send_zc = PrepareSendZc(3, std::span<const std::uint8_t>{u8}, MSG_NOSIGNAL);
  if (!read || !read_u8 || !write || !write_u8 || !recv || !recv_u8 || !send || !send_zc) return false;
  if (!check(*read, sys::Address(bytes.data()), 7) || !check(*read_u8, sys::Address(u8.data()), 0) ||
      !check(*write, sys::Address(bytes.data()), 0) || !check(*write_u8, sys::Address(u8.data()), 0) ||
      !check(*recv, sys::Address(bytes.data()), 0) || !check(*recv_u8, sys::Address(u8.data()), 0) ||
      !check(*send, sys::Address(bytes.data()), MSG_NOSIGNAL) || !check(*send_zc, sys::Address(u8.data()), MSG_NOSIGNAL))
    return false;
  std::byte one{};
  auto max_read = PrepareRead(3, std::span<std::byte>{&one, static_cast<std::size_t>(UINT32_MAX)});
  auto over_read = PrepareRead(3, std::span<std::byte>{&one, static_cast<std::size_t>(UINT32_MAX) + 1U});
  auto max_send = PrepareSend(3, std::span<const std::byte>{&one, static_cast<std::size_t>(UINT32_MAX)});
  auto over_send = PrepareSend(3, std::span<const std::byte>{&one, static_cast<std::size_t>(UINT32_MAX) + 1U});
  if (!max_read || over_read || over_read.error() != UringErrorCode::kOverflow) return false;
  if (!max_send || over_send || over_send.error() != UringErrorCode::kOverflow) return false;
  max_read->Encode(sqe);
  if (sqe.len != UINT32_MAX || sqe.addr() != sys::Address(&one)) return false;
  futex2::FutexWord32 word;
  futex2::FutexNumaWord32 numa;
  PrepareFutexWait(word, 7, 9).Encode(sqe);
  if (sqe.addr() != sys::Address(&word.value) || sqe.off() != 7 || sqe.addr3() != 9) return false;
  PrepareFutexWake(numa, 2).Encode(sqe);
  if (sqe.fd != static_cast<int>(abi::kFutex2SizeU32 | abi::kFutex2Numa)) return false;
  PrepareFutexWake(word, 4, 3).Encode(sqe);
  if (sqe.fd != static_cast<int>(abi::kFutex2SizeU32) || sqe.off() != 4 || sqe.addr3() != 3) return false;
  PrepareFutexWait(numa, 4).Encode(sqe);
  if (sqe.addr() != sys::Address(&numa.value)) return false;
  auto numa_ref = futex2::MakeNumaRef(numa);
  if (!numa_ref || PrepareFutexWait(*numa_ref, 0)) return false;
  auto numa_wait = PrepareFutexWait<abi::kFutex2SizeU32 | abi::kFutex2Numa>(*numa_ref, 8);
  if (!numa_wait) return false;
  numa_wait->Encode(sqe);
  if (sqe.fd != static_cast<int>(numa_ref->flags) || sqe.off() != 8) return false;
  auto ref = futex2::MakeRef(word);
  if (!ref) return false;
  auto wait = PrepareFutexWait(*ref, 3);
  auto wake = PrepareFutexWake(*ref, 2);
  if (!wait || !wake || PrepareFutexWait(futex2::FutexRef{0, abi::kFutex2SizeU32, 0}, 0)) return false;
  if (PrepareFutexWait(*ref, 3, 0) || PrepareFutexWake(*ref, 2, 0)) return false;
  if (PrepareFutexWait(*ref, 3, 0).error() != UringErrorCode::kInvalid) return false;
  if (PrepareFutexWake(*ref, 2, 0).error() != UringErrorCode::kInvalid) return false;
  wait->Encode(sqe);
  return sqe.addr() == ref->uaddr;
}
int main() {
  if (!ErgonomicBuffersAndFutex()) return 2;
  return DirectFileSlots() && KernelDirectFile() && ChainStampPreserved() && ops_test::AllEncodings() && ops_test::Variants() &&
                 WidePayload() && KernelSmoke()
             ? 0
             : 1;
}
