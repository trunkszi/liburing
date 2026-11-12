// expected-error: subsystem operations cannot use IOSQE_IO_DRAIN
#include <uring/ops/core.hpp>
constexpr auto kBad = uring::ops::PrepareNop().WithSqeFlags<uring::abi::kSqeIoDrain>();
