#pragma once
#include <cstddef>
#include <cstdint>
#include <meta>
#include <type_traits>
#include <uring/error/uring_error.hpp>
namespace uring {
class[[= uarch::layout::register_leaf]] OperationPayload;
class OperationPayload {
  std::uint64_t raw_{};

 public:
  static constexpr std::uint64_t kMaxRaw = 0x00ffffffffffffffULL;
  [[nodiscard]] static constexpr Result<OperationPayload> New(std::uint64_t raw) noexcept {
    if (raw > kMaxRaw) return std::unexpected(UringErrorCode::kInvalid);
    return NewUnchecked(raw);
  }
  [[nodiscard]] static constexpr OperationPayload NewUnchecked(std::uint64_t raw) noexcept {
    OperationPayload result;
    result.raw_ = raw;
    return result;
  }
  [[nodiscard]] constexpr std::uint64_t Raw() const noexcept { return raw_; }
};
static_assert(uarch::NoPadding<OperationPayload> && uarch::RegisterLeaf<OperationPayload> &&
              uarch::RegisterPassable<OperationPayload>);
namespace detail {
template <class T>
[[nodiscard]] consteval unsigned ScalarPayloadBits() {
  if constexpr (!std::is_same_v<T, std::remove_cv_t<T>>) return 0;
  if constexpr (std::is_same_v<T, bool>)
    return 8;
  else if constexpr (std::is_same_v<T, std::uint8_t> || std::is_same_v<T, std::uint16_t> || std::is_same_v<T, std::uint32_t> ||
                     std::is_same_v<T, std::uint64_t>)
    return sizeof(T) * 8;
  else if constexpr (std::is_enum_v<T>) {
    using Underlying = std::underlying_type_t<T>;
    static constexpr auto enumerators = std::define_static_array(std::meta::enumerators_of(^^T));
    if constexpr (std::is_same_v<Underlying, bool> || enumerators.empty())
      return 0;
    else
      return ScalarPayloadBits<Underlying>();
  } else
    return 0;
}
template <class T>
[[nodiscard]] constexpr Result<T> DecodeScalarPayload(std::uint64_t raw) noexcept {
  if constexpr (std::is_same_v<T, bool>) {
    if (raw > 1) return std::unexpected(UringErrorCode::kInvalid);
    return raw != 0;
  } else if constexpr (std::is_enum_v<T>) {
    template for (constexpr auto enumerator : std::define_static_array(std::meta::enumerators_of(^^T))) {
      constexpr auto candidate = static_cast<std::uint64_t>([:enumerator:]);
      if (raw == candidate) return static_cast<T>(candidate);
    }
    return std::unexpected(UringErrorCode::kInvalid);
  } else
    return static_cast<T>(raw);
}
}  // namespace detail
template <class T>
[[nodiscard]] consteval unsigned PayloadBits() {
  if constexpr (constexpr auto scalar_bits = detail::ScalarPayloadBits<T>(); scalar_bits != 0)
    return scalar_bits <= 56 ? scalar_bits : 0;
  else if constexpr (!std::is_class_v<T> || !std::is_standard_layout_v<T> || !std::is_aggregate_v<T>)
    return 0;
  else {
    if (!std::meta::bases_of(^^T, std::meta::access_context::current()).empty()) return 0;
    unsigned bits = 0;
    bool valid = true;
    template for (constexpr auto field :
                  std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()))) {
      if constexpr (std::meta::is_bit_field(field))
        valid = false;
      else {
        using Field = [:std::meta::type_of(field):];
        constexpr auto bits_used = detail::ScalarPayloadBits<Field>();
        valid = valid && bits_used != 0;
        bits += bits_used;
      }
    }
    return valid && bits <= 56 ? bits : 0;
  }
}
template <class T>
concept PayloadEncodable = PayloadBits<T>() > 0;
template <PayloadEncodable T>
[[nodiscard]] constexpr std::uint64_t EncodePayload(const T& value) noexcept {
  if constexpr (detail::ScalarPayloadBits<T>() != 0)
    return static_cast<std::uint64_t>(value);
  else {
    std::uint64_t raw = 0;
    template for (constexpr auto field :
                  std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()))) {
      using Field = [:std::meta::type_of(field):];
      raw = (raw << detail::ScalarPayloadBits<Field>()) | static_cast<std::uint64_t>(value.[:field:]);
    }
    return raw;
  }
}
template <PayloadEncodable T>
[[nodiscard]] constexpr Result<T> DecodePayload(std::uint64_t raw) noexcept {
  constexpr unsigned bits_used = PayloadBits<T>();
  if ((raw >> bits_used) != 0) return std::unexpected(UringErrorCode::kInvalid);
  if constexpr (detail::ScalarPayloadBits<T>() != 0)
    return detail::DecodeScalarPayload<T>(raw);
  else {
    T value{};
    unsigned remaining = bits_used;
    template for (constexpr auto field :
                  std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()))) {
      using Field = [:std::meta::type_of(field):];
      constexpr unsigned field_bits = detail::ScalarPayloadBits<Field>();
      remaining -= field_bits;
      const auto field_raw = (raw >> remaining) & ((std::uint64_t{1} << field_bits) - 1);
      auto decoded = detail::DecodeScalarPayload<Field>(field_raw);
      if (!decoded) return std::unexpected(decoded.error());
      value.[:field:] = *decoded;
    }
    return value;
  }
}
}  // namespace uring
