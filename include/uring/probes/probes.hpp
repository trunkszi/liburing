/** @file @brief Native C-linkage leaves expose dispatch and encoding for binary audits. */
#pragma once
#include <uring/core/dispatcher.hpp>
#include <uring/core/op_header.hpp>
#include <uring/ring/completion_queue.hpp>
extern "C" {
std::uint64_t proactor_token_pack_probe(std::uint32_t slot, std::uint8_t subsystem, std::uint32_t generation) noexcept;
std::uint64_t proactor_fsm_step_probe(std::uint64_t header, std::uint32_t flags) noexcept;
void proactor_sqe_encode_probe(uring::abi::FlatSqe* destination, std::int32_t fd, std::uint64_t address,
                               std::uint32_t length) noexcept;
std::size_t proactor_cq_reap_probe(uring::ring::CqCursor* cursor, uring::Completion* output, std::size_t size) noexcept;
std::uint64_t proactor_static_dispatch_probe(const uring::Delivery* deliveries, std::size_t size) noexcept;
}
