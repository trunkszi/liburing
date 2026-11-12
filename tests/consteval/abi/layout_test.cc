#include <uring/abi/proofs.hpp>

static_assert(uring::abi::CheckLayouts());
static_assert(uring::abi::kOpLast == 65);
static_assert(uring::abi::kRegisterLast == 38);
int main() { return 0; }
