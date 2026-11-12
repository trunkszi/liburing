#include <uring/core/completion.hpp>
#include <uring/core/op_header.hpp>

extern "C" std::uint64_t t2_generation_delta(uring::UserDataToken token, uring::OpHeader header) noexcept {
  return (token.raw() ^ header.raw()) >> 32;
}
uring::OperationKey t2_key_pair(uring::OperationKey key) noexcept asm("t2_key_pair");
uring::OperationKey t2_key_pair(uring::OperationKey key) noexcept { return key; }
uring::Completion t2_completion_pair(uring::Completion completion) noexcept asm("t2_completion_pair");
uring::Completion t2_completion_pair(uring::Completion completion) noexcept { return completion; }
extern "C" uring::OpState t2_fsm_next(uring::OpKind kind, uring::OpState state, uring::CqeClass event) noexcept {
  return uring::kNext[static_cast<unsigned>(kind)][static_cast<unsigned>(state)][static_cast<unsigned>(event)];
}
uring::OpHeader t2_advance(uring::OpHeader header, uring::CqeClass event) noexcept asm("t2_advance");
uring::OpHeader t2_advance(uring::OpHeader header, uring::CqeClass event) noexcept { return uring::Advance(header, event); }
