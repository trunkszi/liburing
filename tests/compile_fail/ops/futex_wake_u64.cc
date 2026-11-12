// expected-error: Linux kernel futex2 currently only implements FUTEX2_SIZE_U32
#include <uring/ops/futex.hpp>
std::uint64_t word;
auto bad = uring::ops::PrepareFutexWake(&word, 1U);
