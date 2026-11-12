// expected-error: ChainShape forbids OpKind::kMultishot members in IOSQE_IO_LINK chains
#include <uring/ring/submission_queue.hpp>

using InvalidChain = uring::ring::ChainShape<uring::ring::OneShot64Op, uring::ring::Multishot64Op>;
static_assert(InvalidChain::kMemberCount == 2);
