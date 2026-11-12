/** @file @brief Plugins borrow ordered deliveries only for the duration of their callback. */
#pragma once
#include <concepts>
#include <span>
#include <uarch/core/flow/delegate.hpp>
#include <uring/caps/required_caps.hpp>
#include <uring/core/completion.hpp>
namespace uring::plugins {
template <class S>
concept Subsystem = requires {
  typename std::integral_constant<std::uint8_t, S::kSubsystemId>;
  { S::RequiredCaps() } noexcept -> std::same_as<caps::RequiredCaps>;
} && (requires(S& subsystem, std::span<const Delivery> deliveries) {
  { subsystem.OnCompletions(deliveries) } noexcept -> std::same_as<void>;
} || requires(S& subsystem, const Delivery& delivery) {
  { subsystem.OnCompletion(delivery) } noexcept -> std::same_as<void>;
});
template <Subsystem S>
constexpr void Deliver(S& subsystem, std::span<const Delivery> deliveries) noexcept {
  if constexpr (requires { subsystem.OnCompletions(deliveries); })
    subsystem.OnCompletions(deliveries);
  else
    for (const auto& delivery : deliveries) subsystem.OnCompletion(delivery);
}
}  // namespace uring::plugins
namespace uring {
using CompletionDelegate = uarch::Callback<void(std::span<const Delivery>)>;
static_assert(uarch::NoPadding<CompletionDelegate> && uarch::GprPair<CompletionDelegate>);
struct SubsystemDescriptor {
  std::uint8_t subsystem_id{};
  caps::RequiredCaps required_caps{};
  CompletionDelegate completion{};
};
template <plugins::Subsystem S>
[[nodiscard]] constexpr SubsystemDescriptor Describe(S& subsystem) noexcept {
  return {
      .subsystem_id = S::kSubsystemId,
      .required_caps = S::RequiredCaps(),
      .completion = CompletionDelegate::from_raw(
          [](void* context, std::span<const Delivery> batch) noexcept { plugins::Deliver(*static_cast<S*>(context), batch); },
          &subsystem)};
}
[[nodiscard]] constexpr caps::RequiredCaps MergeCaps(caps::RequiredCaps a, const caps::RequiredCaps& b) noexcept {
  a.setup_flags |= b.setup_flags;
  a.feature_flags |= b.feature_flags;
  for (std::size_t i = 0; i < a.opcodes.size(); ++i) a.opcodes[i] |= b.opcodes[i];
  a.required_futex |= b.required_futex;
  a.require_query |= b.require_query;
  return a;
}
/** A callback capability exposes staging without giving plugins a ring-enter API. */
template <class Engine>
class SubsystemHandle {
  Engine* engine_;
  std::uint8_t id_;
  [[maybe_unused]] std::array<std::uint8_t, 7> reserved_{};

 public:
  constexpr SubsystemHandle(Engine& engine, std::uint8_t id) noexcept : engine_(&engine), id_(id) {}
  template <class Op>
  [[nodiscard]] Result<UserDataToken> Arm(const Op& op) const noexcept {
    return engine_->Arm(id_, op);
  }
  template <class... Ops>
  [[nodiscard]] Result<UserDataToken> ArmChain(const Ops&... ops) const noexcept {
    return engine_->ArmChain(id_, ops...);
  }
  [[nodiscard]] constexpr std::uint8_t Id() const noexcept { return id_; }
  template <class Op>
  [[nodiscard]] Result<UserDataToken> ArmOneShot(const Op& op) const noexcept {
    return engine_->ArmOneShot(id_, op);
  }
  template <class Op>
  [[nodiscard]] Result<UserDataToken> ArmMultishot(const Op& op) const noexcept {
    return engine_->ArmMultishot(id_, op);
  }
  template <class Op>
  [[nodiscard]] Result<UserDataToken> ArmZeroCopy(const Op& op) const noexcept {
    return engine_->ArmZeroCopy(id_, op);
  }
  template <class Op>
  [[nodiscard]] Result<UserDataToken> Arm128(const Op& op) const noexcept {
    return engine_->Arm128(id_, op);
  }
  template <class Op>
  [[nodiscard]] Result<OperationKey> ArmKey(const Op& op) const noexcept {
    return engine_->ArmKey(id_, op);
  }
  template <class... Ops>
  [[nodiscard]] Result<OperationKey> ArmChainKey(const Ops&... ops) const noexcept {
    return engine_->ArmChainKey(id_, ops...);
  }
  [[nodiscard]] Result<void> Cancel(UserDataToken token) const noexcept {
    if (Subsystem(token) != id_) return std::unexpected(UringErrorCode::kInvalidToken);
    return engine_->Cancel(token);
  }
  [[nodiscard]] Result<void> Cancel(OperationKey key) const noexcept {
    if (Subsystem(key.token) != id_) return std::unexpected(UringErrorCode::kInvalidToken);
    return engine_->Cancel(key);
  }
};
}  // namespace uring
