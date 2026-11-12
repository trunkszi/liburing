// expected-error: FUTEX2_MPOL requires FUTEX2_NUMA
#include <uring/ops/futex.hpp>
uring::abi::FutexWaitv waiter;
auto bad = uring::ops::PrepareFutexWaitv<uring::abi::kFutex2SizeU32 | uring::abi::kFutex2Mpol>(&waiter, 1U);
