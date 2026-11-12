// expected-error: tracked operations require CQEs
#include <uring/ops/uring_cmd.hpp>
constexpr std::array<std::byte, 80> kPayload{};
constexpr auto kBad = uring::ops::PrepareUringCmd128(3, 4, kPayload).WithSqeFlags<uring::abi::kSqeCqeSkipSuccess>();
