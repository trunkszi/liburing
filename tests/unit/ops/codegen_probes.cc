#include <uring/ops/fs.hpp>
#include <uring/ops/futex.hpp>
#include <uring/ops/net.hpp>
#include <uring/ops/uring_cmd.hpp>
extern "C" void OpsEncodeNop(uring::abi::FlatSqe* destination, uring::UserDataToken token) noexcept {
  uring::ops::PrepareNop().WithToken(token).Encode(*destination);
}
extern "C" void OpsEncodeRead(uring::abi::FlatSqe* destination, std::int32_t fd, uring::ops::UserAddress buffer,
                              std::uint32_t length, std::uint64_t offset, uring::UserDataToken token) noexcept {
  uring::ops::PrepareRead(fd, buffer, length, offset).WithToken(token).Encode(*destination);
}
extern "C" void OpsEncodeFutex(uring::abi::FlatSqe* destination, std::uint32_t* word, std::uint32_t value) noexcept {
  uring::ops::PrepareFutexWait(word, value).Encode(*destination);
}
extern "C" void OpsEncodeMultishot(uring::abi::FlatSqe* destination, std::int32_t fd, std::uint16_t group) noexcept {
  uring::ops::PrepareRecvMultishot(fd, uring::ops::UserAddress{}, 0, 0, group).Encode(*destination);
}
extern "C" void OpsEncodeCommand128(uring::abi::FlatSqe128* destination, std::int32_t fd, std::uint32_t command,
                                    const std::array<std::byte, 80>* payload) noexcept {
  uring::ops::PrepareUringCmd128(fd, command, *payload).Encode(*destination);
}
extern "C" std::uint64_t OpsAddressValue(uring::ops::UserAddress address) noexcept { return address.Raw(); }
