#include <uring/core/proactor.hpp>
using namespace uring;
struct Sink {
  static constexpr std::uint8_t kSubsystemId = 7;
  std::size_t count{};
  [[nodiscard]] static consteval caps::RequiredCaps RequiredCaps() noexcept { return {}; }
  void OnCompletions(std::span<const Delivery> batch) noexcept { count += batch.size(); }
};
int ResizeControl() {
  ScriptedProactor engine{std::in_place, 8U, 16U};
  Sink sink;
  SingleDispatcher dispatcher{sink};
  if (!engine.ArmOneShot(7, ops::PrepareNop())) return 20;
  ControlCommand command{{kNullToken, engine.Instance()}, 16, ControlKind::kResize, 0};
  if (!engine.EnqueueControl(19, command)) return 21;
  if (!engine.Drive(DriveMode::kPoll, dispatcher)) return 22;
  if (engine.Backend().Sq().RingEntries() != 8 || engine.InFlight() != 0) return 23;
  if (!engine.Drive(DriveMode::kPoll, dispatcher)) return 24;
  if (engine.Backend().Sq().RingEntries() != 16) return 25;
  if (!engine.ArmOneShot(7, ops::PrepareNop()) || !engine.Drive(DriveMode::kPoll, dispatcher)) return 26;
  return engine.InFlight() == 0 && sink.count == 2 ? 0 : 27;
}
int DeferredCancellationTerminals() {
  ScriptedProactor engine{std::in_place, 4U, 16U};
  for (int i = 0; i < 4; ++i)
    if (!engine.Arm(7, ops::PrepareNop())) return 30;
  auto chain = engine.ArmChain(7, ops::PrepareNop(), ops::PrepareNop(), ops::PrepareNop());
  auto once = engine.Arm(7, ops::PrepareNop());
  if (!chain || !once || !engine.Cancel(*chain) || !engine.Cancel(*once) || !engine.Cancel(*chain)) return 31;
  const bool retained = engine.Contains(*chain) && engine.Contains(*once) && engine.InFlight() == 6;
  unsigned cancelled = 0, members = 0;
  bool invalid = false;
  auto dispatcher = MakeFnDispatcher([&](const Delivery& d) noexcept {
    if (d.completion.result != 0) {
      invalid |= d.completion.result != -static_cast<int>(UringErrorCode::kCanceled);
      ++cancelled;
      if (Slot(d.completion.token) == Slot(*chain)) members |= 1U << Member(d.completion.token);
    }
  });
  if (!engine.Tick(dispatcher) || !engine.Tick(dispatcher) || !engine.Shutdown()) return 32;
  return retained && !invalid && cancelled == 4 && members == 7 && engine.InFlight() == 0 ? 0 : 33;
}

int ControlAdmissionAndDetach() {
  ScriptedProactor engine{std::in_place, 8U, 16U};
  Sink sink;
  SingleDispatcher dispatcher{sink};
  engine.BindDispatcher(dispatcher);
  ControlCommand detach{{kNullToken, engine.Instance()}, 7, ControlKind::kDetach, 0};
  if (engine.EnqueueControl(0, detach).error() != UringErrorCode::kInvalid ||
      engine.EnqueueControl(ScriptedProactor::kCapacity, detach).error() != UringErrorCode::kInvalid)
    return 40;
  ControlCommand invalid = detach;
  invalid.argument = 255;
  if (engine.EnqueueControl(1, invalid).error() != UringErrorCode::kInvalid) return 41;
  if (!engine.EnqueueControl(1, detach) || engine.EnqueueControl(1, detach).error() != UringErrorCode::kBusy) return 42;
  if (!engine.Tick(dispatcher)) return 43;
  auto armed = engine.Arm(7, ops::PrepareNop());
  if (!armed || !engine.Tick(dispatcher) || !engine.Shutdown()) return 44;
  ScriptedProactor dynamic_engine{std::in_place, 8U, 16U};
  DynamicDispatcher dynamic;
  if (!dynamic.Register(Describe(sink))) return 45;
  detach.key.ring_instance = dynamic_engine.Instance();
  if (!dynamic_engine.EnqueueControl(1, detach) || !dynamic_engine.Tick(dynamic) || !dynamic.Register(Describe(sink)))
    return 46;
  if (!dynamic_engine.Arm(7, ops::PrepareNop()) || !dynamic_engine.Tick(dynamic) || !dynamic_engine.Shutdown()) return 47;
  return 0;
}
int IoPollDetach() {
  auto engine = Proactor::Builder().Entries(2).Mode(ProactorMode::IoPoll()).BuildScripted();
  if (!engine) return 50;
  engine->Backend().SetAutoCompleteNops(false);
  auto a = engine->Arm(7, ops::PrepareNop());
  auto b = engine->Arm(7, ops::PrepareNop());
  auto deferred = engine->Arm(7, ops::PrepareNop());
  if (!a || !b || !deferred) return 51;
  ControlCommand detach{{kNullToken, engine->Instance()}, 7, ControlKind::kDetach, 0};
  if (!engine->EnqueueControl(1, detach) || engine->DeferredCount() != 0) return 52;
  unsigned cancelled = 0;
  auto dispatcher = MakeFnDispatcher([&](const Delivery& d) noexcept {
    if (d.completion.token == *deferred && d.completion.result == -125) ++cancelled;
  });
  if (!engine->Tick(dispatcher)) return 53;
  engine->Backend().ScriptCqe(*a, 0);
  engine->Backend().ScriptCqe(*b, 0);
  if (!engine->Tick(dispatcher) || !engine->Tick(dispatcher) || !engine->Shutdown()) return 54;
  return cancelled == 1 ? 0 : 55;
}

int main() {
  if (auto result = ControlAdmissionAndDetach()) return result;
  if (auto result = IoPollDetach()) return result;
  if (auto result = DeferredCancellationTerminals()) return result;
  ScriptedProactor engine{std::in_place, 2U, 16U};
  engine.Backend().SetAutoCompleteNops(false);
  engine.Backend().SetConsumeSqOnSubmit(false);
  Sink sink;
  DynamicDispatcher dispatcher;
  if (!dispatcher.Register(Describe(sink))) return 1;
  auto a = engine.ArmOneShot(7, ops::PrepareNop());
  auto b = engine.ArmOneShot(7, ops::PrepareNop());
  auto deferred = engine.ArmOneShot(7, ops::PrepareNop());
  if (!a || !b || !deferred) return 2;
  if (!engine.Cancel(*deferred) || !engine.Contains(*deferred) || engine.DeferredCount() != 0) return 3;
  if (!engine.BeginDrain() || !engine.Drive(DriveMode::kPoll, dispatcher)) return 4;
  if (engine.Stage() != DrainStage::kDraining || engine.InFlight() != 2) return 5;
  if (engine.Backend().LastRegisterOpcode() != abi::kRegisterSyncCancel) return 6;
  engine.Backend().ScriptCqe(*a, -125);
  engine.Backend().ScriptCqe(*b, -125);
  if (!engine.Drive(DriveMode::kPoll, dispatcher) || engine.Stage() != DrainStage::kQuiescent) return 7;
  if (!engine.Close()) return 8;

  ScriptedProactor second{std::in_place, 8U, 16U};
  second.Backend().SetAutoCompleteNops(false);
  auto token = second.ArmOneShot(7, ops::PrepareNop());
  if (!token) return 9;
  if (second.Cancel(OperationKey{*token, engine.Instance()}).error() != UringErrorCode::kInvalidToken) return 10;
  ControlCommand detach{{*token, second.Instance()}, 7, ControlKind::kDetach, 0};
  if (!second.EnqueueControl(17, detach)) return 11;
  if (!second.Drive(DriveMode::kPoll, dispatcher)) return 12;
  const bool issued_cancel = second.Backend().Sq().Tail() == 2;
  second.Backend().ScriptCqe(*token, -125);
  if (issued_cancel) second.Backend().ScriptCqe(second.InternalCancelToken(), 0);
  if (!second.Drive(DriveMode::kPoll, dispatcher) || !second.Drive(DriveMode::kPoll, dispatcher)) return 13;
  if (!issued_cancel) return 14;
  Delivery delivery{{*token, second.Instance()}, {*token, 0, 0}};
  if (dispatcher.DispatchBatch({&delivery, 1}).error() != UringErrorCode::kNoEntry) return 15;
  return ResizeControl();
}
