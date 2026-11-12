// expected-error: Linux kernel futex2 currently only implements FUTEX2_SIZE_U32
#include <uring/ops/futex.hpp>
std::uint8_t word;
auto bad = uring::ops::PrepareFutexWait(&word, 1U);
