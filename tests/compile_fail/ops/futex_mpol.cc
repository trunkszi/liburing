// expected-error: FUTEX2_MPOL requires FUTEX2_NUMA
#include <uring/ops/futex.hpp>
std::uint32_t word;
auto bad = uring::ops::PrepareFutexWait<uring::abi::kFutex2SizeU32 | uring::abi::kFutex2Mpol>(&word, 1U);
