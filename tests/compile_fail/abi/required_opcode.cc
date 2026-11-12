// expected-error: required opcode outside UAPI
#include <uring/caps/required_caps.hpp>
constexpr auto kBad = uring::caps::RequiredCaps::For<65>();
