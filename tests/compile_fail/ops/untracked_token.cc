// expected-error: untracked operations require kNullToken
#include <uring/ops/core.hpp>
constexpr auto kBad = uring::ops::PreparePadNop().WithToken(uring::MakeToken(1, 0, 7, 1));
