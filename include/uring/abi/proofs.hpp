#pragma once
#include <array>
#include <cstddef>
#include <meta>
#include <uarch/primitives.hpp>
#include <uring/abi/uapi_futex.hpp>
#include <uring/abi/uapi_io_uring.hpp>
namespace uring::abi {
consteval bool CheckLayouts() {
  static_assert(uarch::assert_layout<AbsoluteDeadline, 24, 8, 0, 16, 20>());
  static_assert(uarch::assert_layout<FlatSqe, 64, 8, 0, 1, 2, 4, 8, 16, 24, 28, 32, 40, 42, 44, 48, 56>());
  static_assert(uarch::assert_layout<FlatSqe128, 128, 8, 0, 64>());
  static_assert(uarch::assert_layout<FlatCqe, 16, 8, 0, 8, 12>());
  static_assert(uarch::assert_layout<FlatCqe32, 32, 8, 0, 16>());
  static_assert(uarch::assert_layout<IoSqringOffsets, 40, 8, 0, 4, 8, 12, 16, 20, 24, 28, 32>());
  static_assert(uarch::assert_layout<IoCqringOffsets, 40, 8, 0, 4, 8, 12, 16, 20, 24, 28, 32>());
  static_assert(uarch::assert_layout<IoUringParams, 120, 8, 0, 4, 8, 12, 16, 20, 24, 28, 40, 80>());
  static_assert(uarch::assert_layout<IoUringRsrcRegister, 32, 8, 0, 4, 8, 16, 24>());
  static_assert(uarch::assert_layout<IoUringRsrcUpdate2, 32, 8, 0, 4, 8, 16, 24, 28>());
  static_assert(uarch::assert_layout<IoUringRsrcUpdate, 16, 8, 0, 4, 8>());
  static_assert(uarch::assert_layout<IoUringBuf, 16, 8, 0, 8, 12, 14>());
  static_assert(uarch::assert_layout<IoUringBufRing, 16, 8, 0, 8, 12, 14>());
  static_assert(uarch::assert_layout<IoUringBufReg, 40, 8, 0, 8, 12, 14, 16, 20>());
  static_assert(uarch::assert_layout<IoUringGeteventsArg, 24, 8, 0, 8, 12, 16>());
  static_assert(uarch::assert_layout<IoUringSyncCancelReg, 64, 8, 0, 8, 12, 16, 32, 33, 40>());
  static_assert(uarch::assert_layout<IoUringProbeOp, 8, 4, 0, 1, 2, 4>());
  static_assert(uarch::assert_layout<IoUringProbe, 16, 4, 0, 1, 2, 4>());
  static_assert(uarch::assert_layout<IoUringZcrxRqe, 16, 8, 0, 8, 12>());
  static_assert(uarch::GprPair<IoUringZcrxRqe>);
  static_assert(uarch::assert_layout<IoUringZcrxOffsets, 32, 8, 0, 4, 8, 12, 16>());
  static_assert(uarch::assert_layout<IoUringZcrxIfqReg, 96, 8, 0, 4, 8, 12, 16, 24, 32, 64, 68, 72>());
  static_assert(uarch::assert_layout<IoUringRegionDesc, 64, 8, 0, 8, 16, 20, 24, 32>());
  static_assert(uarch::assert_layout<IoUringMemRegionReg, 32, 8, 0, 8, 16>());
  static_assert(uarch::assert_layout<IoUringZcrxAreaReg, 48, 8, 0, 8, 16, 24, 28, 32>());
  static_assert(uarch::assert_layout<IoUringNapi, 16, 4, 0, 4, 5, 6, 8, 12>());
  static_assert(uarch::assert_layout<IoUringFileIndexRange, 16, 8, 0, 4, 8>());
  static_assert(uarch::assert_layout<IoUringCloneBuffers, 32, 4, 0, 4, 8, 12, 16, 20>());
  static_assert(uarch::assert_layout<IoUringClockRegister, 16, 4, 0, 4>());
  static_assert(uarch::assert_layout<IoUringRestriction, 16, 4, 0, 2, 3, 4>());
  static_assert(uarch::assert_layout<IoUringBufStatus, 40, 4, 0, 4, 8>());
  static_assert(uarch::assert_layout<IoUringQueryHdr, 40, 8, 0, 8, 16, 20, 24, 28>());
  static_assert(uarch::assert_layout<IoUringQueryOpcode, 48, 8, 0, 4, 8, 16, 24, 32, 40, 44>());
  static_assert(uarch::assert_layout<FutexWaitv, 24, 8, 0, 8, 16, 20>());
  static_assert(uarch::assert_layout<KernelTimespec, 16, 8, 0, 8>());
  static_assert(uarch::RegisterPassable<FlatCqe> && uarch::GprPair<FlatCqe>);
  static_assert(uarch::RegisterPassable<IoUringBuf> && uarch::GprPair<IoUringBuf>);
  static_assert(uarch::RegisterPassable<KernelTimespec> && uarch::GprPair<KernelTimespec>);
  return true;
}
static_assert(CheckLayouts());
}  // namespace uring::abi
