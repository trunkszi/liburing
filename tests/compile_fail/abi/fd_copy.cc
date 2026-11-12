// expected-error: fd ownership is unique
#include <uring/sys/unique_fd.hpp>
void Copy(const uring::sys::UniqueFd& fd) { auto copy = fd; }
