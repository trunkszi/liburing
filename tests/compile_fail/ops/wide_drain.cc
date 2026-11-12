// expected-error: subsystem operations cannot use IOSQE_IO_DRAIN
#include <uring/ops/uring_cmd.hpp>
constexpr std::array<std::byte, 80> kPayload{};
constexpr auto kBad = uring::ops::PrepareUringCmd128(3, 4, kPayload).WithSqeFlags<uring::abi::kSqeIoDrain>();
