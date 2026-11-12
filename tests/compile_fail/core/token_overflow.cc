// expected-error: field value overflow
#include <uring/core/user_data_token.hpp>
constexpr auto kBad = uring::UserDataToken{}.with_const<^^uring::TokenSpec::slot, 0x400000U>();
