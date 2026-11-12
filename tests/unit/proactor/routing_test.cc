#include <array>
#include <uring/core/proactor.hpp>
#include <uring/ops/net.hpp>
using namespace uring;
template <std::uint8_t Id>
struct Sink {
  static constexpr auto kSubsystemId = Id;
  std::size_t count{};
  std::size_t calls{};
  [[nodiscard]] static consteval caps::RequiredCaps RequiredCaps() noexcept { return {}; }
  void OnCompletions(std::span<const Delivery> deliveries) noexcept {
    ++calls;
    count += deliveries.size();
  }
};
template <class Dispatcher>
int CheckRuns(Dispatcher& dispatcher, Sink<1>& sink) {
  std::array<Delivery, 6> batch{};
  constexpr std::array<std::uint8_t, 6> ids{9, 1, 1, 9, 1, 9};
  for (std::size_t i = 0; i < ids.size(); ++i) batch[i].completion.token = MakeToken(1, 0, ids[i], 1);
  auto result = dispatcher.DispatchBatch(batch);
  if (result || result.error() != UringErrorCode::kNoEntry || sink.count != 3 || sink.calls != 2 ||
      dispatcher.UnroutedCount() != 3)
    return 1;
  if (!dispatcher.CanRoute(1) || dispatcher.CanRoute(9)) return 2;
  return 0;
}
enum class Routes : std::uint8_t { kFirst = 1, kSecond = 2 };
int ErgonomicRouting() {
  ScriptedProactor engine{std::in_place, 16U, 32U};
  auto handle = engine.Handle(Routes::kFirst);
  if (handle.Id() != 1) return 10;
  auto token = handle.ArmOneShot(ops::PrepareNop());
  auto key = engine.ArmKey(Routes::kSecond, ops::PrepareNop());
  auto chain = handle.ArmChainKey(ops::PrepareNop(), ops::PrepareNop());
  auto chained = engine.ArmChain(Routes::kSecond, ops::PrepareNop(), ops::PrepareNop());
  if (!token || !key || !chain || !chained || engine.KeyFor(*token).ring_instance != engine.Instance()) return 11;
  if (engine.Handle(Routes::kSecond).Cancel(*token).error() != UringErrorCode::kInvalidToken) return 12;
  unsigned calls = 0;
  auto dispatcher = MakeRouteDispatcher<Routes>([&](Routes, const Delivery&) noexcept { ++calls; });
  if (!engine.Tick(dispatcher) || calls != 6 || engine.InFlight() != 0) return 13;
  auto stray = engine.Arm(9, ops::PrepareNop());
  if (stray || stray.error() != UringErrorCode::kNoEntry) return 14;
  if (!engine.QuiesceSubsystem(Routes::kSecond)) return 15;
  auto stopped = engine.Arm(Routes::kSecond, ops::PrepareNop());
  if (stopped || stopped.error() != UringErrorCode::kShuttingDown) return 16;
  return engine.Shutdown() ? 0 : 17;
}
int EnumOperationKinds() {
  auto engine = Proactor::Builder().Entries(16).Geometry(RingMode::kMixed64_128_16_32).BuildScripted();
  if (!engine) return 20;
  auto handle = engine->Handle(Routes::kFirst);
  auto wide = handle.Arm128(ops::PrepareNop128());
  auto multi = engine->ArmMultishot(Routes::kSecond, ops::PreparePollMultishot(1, 0));
  auto zero = engine->ArmZeroCopy(Routes::kFirst, ops::PrepareSendZc(1, nullptr, 0));
  if (!wide || !multi || !zero) return 21;
  engine->Backend().ScriptCqe(*multi, 0);
  engine->Backend().ScriptCqe(*zero, 0);
  auto dispatcher = MakeRouteDispatcher<Routes>([](Routes, const Delivery&) noexcept {});
  if (!engine->Tick(dispatcher) || !engine->ShutdownWith(dispatcher)) return 22;
  return 0;
}
int main() {
  if (auto error = EnumOperationKinds()) return error;
  if (auto error = ErgonomicRouting()) return error;
  Sink<1> a;
  SingleDispatcher single{a};
  if (CheckRuns(single, a)) return 1;
  Sink<1> b;
  StaticDispatcher statically_bound{b};
  if (CheckRuns(statically_bound, b)) return 2;
  Sink<1> c;
  DynamicDispatcher dynamically_bound;
  if (!dynamically_bound.Register(Describe(c)) || CheckRuns(dynamically_bound, c)) return 3;
  ScriptedProactor engine{std::in_place, 8U, 16U};
  auto stray = engine.Arm(9, ops::PrepareNop());
  auto valid = engine.Arm(1, ops::PrepareNop());
  if (!stray || !valid) return 4;
  engine.BindDispatcher(statically_bound);
  if (engine.Arm(9, ops::PrepareNop()).error() != UringErrorCode::kNoEntry) return 5;
  auto drove = engine.Drive(DriveMode::kPoll, statically_bound);
  if (drove || drove.error() != UringErrorCode::kNoEntry || b.count != 4 || engine.InFlight() != 0) return 6;
  return engine.DrainBlocking(statically_bound) ? 0 : 7;
}
