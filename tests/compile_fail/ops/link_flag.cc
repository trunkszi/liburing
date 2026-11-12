// expected-error: linked chains must use ChainShape
#include <uring/ops/uring_cmd.hpp>
#ifdef T4_HARDLINK
constexpr auto kFlag = uring::abi::kSqeIoHardlink;
#else
constexpr auto kFlag = uring::abi::kSqeIoLink;
#endif
#ifdef T4_WIDE
constexpr auto kBad = uring::ops::UringCmd128{}.WithSqeFlags<kFlag>();
#else
constexpr auto kBad = uring::ops::PrepareNop().WithSqeFlags<kFlag>();
#endif
