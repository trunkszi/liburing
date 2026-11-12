#include <uring/abi/proofs.hpp>
#include <uring/sys/syscall.hpp>
uring::abi::FlatCqe AbiCqeEcho(uring::abi::FlatCqe value) noexcept asm("abi_cqe_echo");
uring::abi::FlatCqe AbiCqeEcho(uring::abi::FlatCqe value) noexcept { return value; }
// NOLINTNEXTLINE(readability-identifier-naming) -- Stable symbols consumed by codegen gates.
extern "C" std::uint64_t abi_sqe_offset(const uring::abi::FlatSqe* __restrict sqe) noexcept { return sqe->off(); }
// NOLINTNEXTLINE(readability-identifier-naming) -- Stable symbols consumed by codegen gates.
extern "C" void abi_cmd_store(uring::abi::FlatSqe* __restrict sqe, std::uint32_t command) noexcept { sqe->cmd_op(command); }
extern "C" std::int64_t syscall_pid_probe() noexcept { return uring::sys::Syscall6(uring::sys::kSysGetpid); }

extern "C" [[noreturn, gnu::cold, gnu::noinline]] void invariant_panic_probe() noexcept {
  uring::sys::Panic("invariant probe");
}
