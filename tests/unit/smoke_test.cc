#include <cstdint>
#include <limits>
#include <meta>
#include <uarch/primitives.hpp>
#include <uring/uring.hpp>

struct SmokeRecord {
  std::uint64_t first;
  std::uint64_t second;
};
static_assert(uarch::NoPadding<SmokeRecord> && uarch::RegisterPassable<SmokeRecord>);
static_assert(uarch::GprPair<SmokeRecord>);
constexpr auto kMembers =
    std::define_static_array(std::meta::nonstatic_data_members_of(^^SmokeRecord, std::meta::access_context::current()));
static_assert(kMembers.size() == 2);

struct SmokeBits {
  using Word = std::uint64_t;
  std::uint32_t slot[[= uarch::bits::Field{0, 32}]];
  std::uint32_t generation[[= uarch::bits::Field{32, 32}]];
};
using SmokeWord = uarch::BitWord<SmokeBits>;
static_assert(uarch::NoPadding<SmokeWord> && uarch::RegisterPassable<SmokeWord>);

constexpr SmokeWord Pack(std::uint32_t slot, std::uint32_t generation) noexcept {
  SmokeWord word{};
  word = word.with<^^SmokeBits::slot>(slot);
  return word.with<^^SmokeBits::generation>(generation);
}
static_assert(Pack(7, 3).raw() == 0x0000000300000007ULL);
static_assert(Pack(7, 3).wrapping_add_top_field<^^SmokeBits::generation>(1U).raw() == 0x0000000400000007ULL);

static_assert(std::is_same_v<uring::DirectFile, uring::io::DirectFile>);
static_assert(std::is_same_v<uring::prelude::OwnedFd, uring::sys::UniqueFd>);
static_assert(std::is_same_v<uring::prelude::BufferRing, uring::resources::ProvidedBufferRing>);
static_assert(std::is_same_v<uring::prelude::ScriptedRing, uring::ring::ScriptedRing>);
static_assert(std::is_same_v<uring::FutexWord32, uring::futex2::FutexWord32>);
static_assert(std::is_same_v<uring::FutexNumaWord32, uring::futex2::FutexNumaWord32>);
static_assert(std::is_same_v<uring::ops::FutexNumaWord32, uring::futex2::FutexNumaWord32>);
static_assert(std::is_same_v<uring::prelude::FutexWord32, uring::futex2::FutexWord32>);
static_assert(std::is_same_v<uring::prelude::FutexNumaWord32, uring::futex2::FutexNumaWord32>);
static_assert(std::is_same_v<uring::SocketDispatcher<int, int>, uring::io::SocketDispatcher<int, int>>);
static_assert(std::is_same_v<uring::prelude::SocketDispatcher<int, int>, uring::io::SocketDispatcher<int, int>>);
int main() {
#if defined(URING_UBSAN_FAILURE_CONTROL)
  volatile int input = std::numeric_limits<int>::max();
  return input + 1;
#else
  volatile std::uint32_t input = 7;
  return Pack(input, 3).raw() == 0x0000000300000007ULL ? 0 : 1;
#endif
}
