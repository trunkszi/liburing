// expected-error: subsystem IDs must be unique and nonreserved
#include <uring/core/dispatcher.hpp>
struct Sink {
  static constexpr std::uint8_t kSubsystemId = 1;
  static consteval uring::caps::RequiredCaps RequiredCaps() noexcept { return {}; }
  void OnCompletions(std::span<const uring::Delivery>) noexcept {}
};
int main() {
  Sink a;
  Sink b;
  uring::StaticDispatcher dispatcher{a, b};
}
