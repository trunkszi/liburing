// expected-error: MPOL requires NUMA
#include <uring/futex2/futex_flags.hpp>
constexpr auto kBad = uring::futex2::Futex2Flags<uring::futex2::FutexScope::kShared,
                                                 static_cast<uring::futex2::FutexLocality>(uring::abi::kFutex2Mpol)>();
