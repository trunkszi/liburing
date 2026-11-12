// expected-error: currently only implements FUTEX2_SIZE_U32
#include <uring/futex2/futex_flags.hpp>
void Unsupported(uring::futex2::FutexWord64& word) { static_cast<void>(uring::futex2::MakeRef(word)); }
