#include <array>
#include <concepts>
#include <uarch/core/flow/delegate.hpp>
#include <uring/core/dispatcher.hpp>
using namespace uring;
static_assert(std::same_as<CompletionDelegate, uarch::Callback<void(std::span<const Delivery>)>>);
static_assert(sizeof(CompletionDelegate) == 16 && alignof(CompletionDelegate) == 8);
static_assert(uarch::NoPadding<CompletionDelegate> && uarch::GprPair<CompletionDelegate>);
struct Log {
  std::array<int, 8> order{};
  std::size_t count{};
};
template <std::uint8_t Id>
struct Sink {
  static constexpr auto kSubsystemId = Id;
  Log* log;
  [[nodiscard]] static consteval caps::RequiredCaps RequiredCaps() noexcept { return caps::RequiredCaps::For<abi::kOpNop>(); }
  constexpr void OnCompletions(std::span<const Delivery> batch) noexcept {
    log->order[log->count++] = Id * 100 + static_cast<int>(batch.size());
  }
};
constexpr bool DescribedAtCompileTime() {
  Log log{};
  Sink<1> receiver{&log};
  const auto descriptor = Describe(receiver);
  std::array<Delivery, 1> batch{};
  descriptor.completion(batch);
  return descriptor.subsystem_id == 1 && descriptor.required_caps.opcodes[0].test(abi::kOpNop) && log.count == 1 &&
         log.order[0] == 101;
}
static_assert(DescribedAtCompileTime());
namespace {
std::size_t free_count = 0;
void FreeBatch(std::span<const Delivery> batch) noexcept { free_count += batch.size(); }
}  // namespace
int main() {
  Log log;
  Sink<1> a{&log};
  Sink<2> b{&log};
  StaticDispatcher dispatch{a, b};
  std::array<Delivery, 5> deliveries{};
  constexpr std::array<std::uint8_t, 5> ids{1, 1, 2, 1, 2};
  for (std::size_t i = 0; i < ids.size(); ++i) deliveries[i].completion.token = MakeToken(1, 0, ids[i], 1);
  if (!dispatch.DispatchBatch(deliveries)) return 1;
  if (log.count != 4 || log.order[0] != 102 || log.order[1] != 201 || log.order[2] != 101 || log.order[3] != 201) return 2;
  DynamicDispatcher dynamic;
  if (!dynamic.Register(Describe(a)) || !dynamic.Register(Describe(b))) return 3;
  if (dynamic.Register(Describe(a)).error() != UringErrorCode::kBusy) return 4;
  log.count = 0;
  if (!dynamic.DispatchBatch(deliveries) || log.count != 4) return 5;
  if (!dynamic.Detach(2)) return 6;
  if (dynamic.DispatchBatch(deliveries).error() != UringErrorCode::kNoEntry) return 7;
  SubsystemDescriptor stateless{
      .subsystem_id = 3, .completion = CompletionDelegate::from_raw([](void*, std::span<const Delivery>) noexcept {}, nullptr)};
  if (!dynamic.Register(stateless)) return 9;
  if (!dynamic.CanRoute(1) || dynamic.CanRoute(2) || !dynamic.RequiredCaps().opcodes[0].test(abi::kOpNop)) return 10;
  if (!dynamic.Detach(1) || dynamic.RequiredCaps().opcodes[0].test(abi::kOpNop)) return 11;
  if (!dynamic.Register({.subsystem_id = 1, .completion = uarch::bind<&Sink<1>::OnCompletions>(a)})) return 12;
  if (!dynamic.Register({.subsystem_id = 2, .completion = uarch::bind<&FreeBatch>()})) return 13;
  log.count = 0;
  if (!dynamic.DispatchBatch(deliveries) || log.count != 2 || log.order[0] != 102 || log.order[1] != 101 || free_count != 2)
    return 14;
  if (dynamic.Register({.subsystem_id = 4}).error() != UringErrorCode::kInvalid) return 15;
  if (dynamic.Detach(4).error() != UringErrorCode::kNoEntry) return 16;
  SingleDispatcher single{a};
  log.count = 0;
  if (!single.DispatchBatch(std::span<const Delivery>{deliveries.data(), 2}) || log.order[0] != 102) return 8;
  constexpr auto required = decltype(dispatch)::RequiredCaps();
  static_assert(required.opcodes[0].test(abi::kOpNop));
  static_assert(plugins::Subsystem<Sink<1>>);
  static_assert(!UniqueSubsystemIds<Sink<1>, Sink<1>>());
  return 0;
}
