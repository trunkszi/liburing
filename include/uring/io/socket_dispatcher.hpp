/** @file @brief Borrowed socket routing with owned callbacks and complete batch error propagation. */
#pragma once
#include <optional>
#include <span>
#include <type_traits>
#include <uring/core/completion.hpp>
#include <uring/error/uring_error.hpp>
#include <uring/sys/syscall.hpp>
#include <utility>
namespace uring::sys {
// Linux releases the descriptor even when close returns EINTR, so a single attempt is enough.
[[nodiscard]] inline Result<void> Close(std::int32_t fd) noexcept {
  if (fd < 0) return std::unexpected(UringErrorCode::kBadFd);
  const auto closed = ToResult(Syscall6(kSysClose, static_cast<std::uint64_t>(fd)));
  if (!closed) return std::unexpected(closed.error());
  return {};
}
}  // namespace uring::sys
namespace uring::io {
template <class Sockets, class Emit>
class SocketDispatcher {
  Sockets* sockets_;
  Emit emit_;

 public:
  SocketDispatcher(Sockets& sockets, Emit emit) noexcept : sockets_(&sockets), emit_(std::move(emit)) {}
  [[nodiscard]] bool CanRoute(std::uint8_t id) const noexcept { return id == sockets_->Config().subsystem_id; }
  [[nodiscard]] bool IsRoutable(std::uint8_t id) const noexcept { return CanRoute(id); }
  [[nodiscard]] Result<void> DispatchBatch(std::span<const Delivery> batch) noexcept {
    std::optional<UringErrorCode> error;
    for (const auto& delivery : batch) {
      if (!CanRoute(Subsystem(delivery.completion.token))) {
        if (!error) error = UringErrorCode::kNoEntry;
        continue;
      }
      sockets_->TranslateDelivery(delivery, [&](typename Sockets::Event&& event) noexcept {
        if constexpr (std::is_void_v<std::invoke_result_t<Emit&, typename Sockets::Event>>)
          emit_(std::move(event));
        else {
          auto result = emit_(std::move(event));
          if (!result && !error) error = result.error();
        }
      });
    }
    if (error) return std::unexpected(*error);
    return {};
  }
};
}  // namespace uring::io
