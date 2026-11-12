// expected-error: (constraints not satisfied|no matching function)
#include <uring/core/payload_codec.hpp>
struct InvalidPayload {
  std::int32_t signed_value;
};
struct TooWide {
  std::uint32_t first;
  std::uint32_t second;
};
static_assert(uring::PayloadEncodable<TooWide>, "payload exceeds 56 bits");
int main() { return static_cast<int>(uring::EncodePayload(InvalidPayload{})); }
