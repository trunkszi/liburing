// expected-error: tracked operations require CQEs
#include <uring/ops/core.hpp>
constexpr auto kBad =
    uring::ops::PrepareNop().WithToken(uring::MakeToken(1, 0, 7, 1)).WithSqeFlags<uring::abi::kSqeCqeSkipSuccess>();
