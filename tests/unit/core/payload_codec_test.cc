#include <uring/core/payload_codec.hpp>
#include <uring/core/proactor.hpp>
#include <uring/core/route_dispatcher.hpp>
using namespace uring;
struct Header {
  std::uint8_t kind;
  std::uint16_t flags;
  std::uint32_t len;
};
struct Small {
  std::uint8_t first;
  std::uint8_t second;
};
static_assert(DecodePayload<Small>(0x0102)->first == 1);
static_assert(!DecodePayload<Small>(0x00ab000000000102ULL));
struct Signed {
  std::int8_t value;
};
struct TooWide {
  std::uint64_t value;
};
static_assert(PayloadEncodable<Header> && !PayloadEncodable<Signed> && !PayloadEncodable<TooWide>);
struct BoolField {
  bool value;
};
struct FloatField {
  float value;
};
struct PointerField {
  void* value;
};
static_assert(PayloadEncodable<BoolField> && !PayloadEncodable<FloatField> && !PayloadEncodable<PointerField>);
constexpr Header kHeader{0xff, 0xffff, 0xffffffff};
static_assert(EncodePayload(Header{0x12, 0x3456, 0x789abcde}) == 0x00123456789abcdeULL);
static_assert(EncodePayload(kHeader) == 0x00ffffffffffffffULL);
static_assert(DecodePayload<Header>(EncodePayload(kHeader))->len == 0xffffffffU);
static_assert(!DecodePayload<Header>(0x0100000000000000ULL));
enum class Routes : std::uint8_t { kFile = 1, kTimer = 2 };
enum class BadRoutes : std::uint8_t { kReserved = 0 };
enum class Alias : std::uint8_t { kOne = 1, kAlias = 1 };
static_assert(SubsystemsEnum<Routes> && !SubsystemsEnum<BadRoutes> && !SubsystemsEnum<Alias>);
static_assert(RouteFromSubsystemId<Routes>(2) == Routes::kTimer && !RouteFromSubsystemId<Routes>(9));
int main() {
  auto engine = Proactor::Builder().Entries(8).BuildScripted();
  if (!engine) return 1;
  unsigned state = 0;
  auto route = MakeRouteDispatcher<Routes>(state, [](Routes, unsigned& count, const Delivery&) noexcept { ++count; });
  if (!engine->Arm(1, ops::PrepareNop()) || !engine->Arm(2, ops::PrepareNop())) return 2;
  if (!engine->Tick(route) || state != 2) return 3;
  auto fn = MakeFnDispatcher([&](const Delivery&) noexcept -> Result<void> {
    ++state;
    return {};
  });
  if (!engine->Arm(1, ops::PrepareNop()) || !engine->Tick(fn) || state != 3) return 4;
  if (!engine->Shutdown()) return 5;
  unsigned routed = 0;
  auto isolate = MakeRouteDispatcher<Routes>(routed, [](Routes, unsigned& value, const Delivery&) noexcept { ++value; });
  std::array<Delivery, 3> mixed{};
  mixed[0].completion.token = MakeToken(1, 0, 9, 1);
  mixed[1].completion.token = MakeToken(2, 0, 1, 1);
  mixed[2].completion.token = MakeToken(3, 0, 2, 1);
  auto isolated = isolate.DispatchBatch(mixed);
  if (isolated || isolated.error() != UringErrorCode::kNoEntry || routed != 2 || isolate.UnroutedCount() != 1) return 6;
  auto failure =
      MakeRouteDispatcher<Routes>(routed, [](Routes route, unsigned& value, const Delivery&) noexcept -> Result<void> {
        if (route == Routes::kFile) return std::unexpected(UringErrorCode::kBusy);
        ++value;
        return {};
      });
  auto failed = failure.DispatchBatch(std::span<const Delivery>{mixed}.subspan(1));
  if (failed || failed.error() != UringErrorCode::kBusy || routed != 3) return 7;
  return OperationPayload::New(OperationPayload::kMaxRaw + 1) ? 8 : 0;
}

enum class PayloadMode : std::uint16_t { kNone = 0, kRead = 3, kAlias = 3, kWrite = 511 };
enum PlainMode : std::uint8_t { kPlain = 7 };
enum class SignedMode : std::int8_t { kNegative = -1 };
struct ExtendedPayload {
  bool ready;
  PayloadMode mode;
  std::uint8_t count;
};
static_assert(PayloadEncodable<bool> && PayloadEncodable<std::uint8_t> && PayloadEncodable<std::uint16_t> &&
              PayloadEncodable<std::uint32_t>);
static_assert(PayloadEncodable<PayloadMode> && PayloadEncodable<PlainMode> && !PayloadEncodable<SignedMode>);
static_assert(!PayloadEncodable<std::uint64_t> && !PayloadEncodable<std::int32_t>);
static_assert(EncodePayload(true) == 1 && DecodePayload<bool>(1) == true && !DecodePayload<bool>(2));
static_assert(DecodePayload<std::uint8_t>(255) == 255 && !DecodePayload<std::uint8_t>(256));
static_assert(EncodePayload(PayloadMode::kWrite) == 511 && DecodePayload<PayloadMode>(511) == PayloadMode::kWrite);
static_assert(!DecodePayload<PayloadMode>(4) && !DecodePayload<PayloadMode>(65536));
static_assert(DecodePayload<PlainMode>(7) == kPlain && !DecodePayload<PlainMode>(8));
static_assert(PayloadBits<ExtendedPayload>() == 32);
constexpr auto kExtended = EncodePayload(ExtendedPayload{true, PayloadMode::kRead, 9});
static_assert(kExtended == 0x01000309 && DecodePayload<ExtendedPayload>(kExtended)->ready);
static_assert(!DecodePayload<ExtendedPayload>(0x02000309) && !DecodePayload<ExtendedPayload>(0x01000409));

enum class BoolEnum : bool { kFalse, kTrue };
enum class WideEnum : std::uint64_t { kOne = 1 };
static_assert(!PayloadEncodable<BoolEnum> && !PayloadEncodable<WideEnum>);
static_assert(DecodePayload<bool>(0) == false && !DecodePayload<bool>(256));
static_assert(EncodePayload(std::uint32_t{0xffffffff}) == 0xffffffffULL);
static_assert(DecodePayload<std::uint32_t>(0xffffffffULL) == 0xffffffffU);
static_assert(!DecodePayload<std::uint32_t>(0x100000000ULL));
struct ConstField {
  const std::uint8_t value;
};
struct BitsField {
  unsigned value : 1;
};
static_assert(!PayloadEncodable<ConstField> && !PayloadEncodable<BitsField>);

enum class EmptyPayloadEnum : std::uint8_t {};
struct EmptyEnumField {
  EmptyPayloadEnum value;
};
static_assert(!PayloadEncodable<EmptyPayloadEnum> && !PayloadEncodable<EmptyEnumField>);
