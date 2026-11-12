#define IOSQE_FIXED_FILE_BIT (0)
#define IOSQE_IO_DRAIN_BIT (IOSQE_FIXED_FILE_BIT + 1)
#define IOSQE_IO_LINK_BIT (IOSQE_IO_DRAIN_BIT + 1)
#define IOSQE_IO_HARDLINK_BIT (IOSQE_IO_LINK_BIT + 1)
#define IOSQE_ASYNC_BIT (IOSQE_IO_HARDLINK_BIT + 1)
#define IOSQE_BUFFER_SELECT_BIT (IOSQE_ASYNC_BIT + 1)
#define IOSQE_CQE_SKIP_SUCCESS_BIT (IOSQE_BUFFER_SELECT_BIT + 1)
#define IORING_OP_NOP (0)
#define IORING_OP_READV (IORING_OP_NOP + 1)
#define IORING_OP_WRITEV (IORING_OP_READV + 1)
#define IORING_OP_FSYNC (IORING_OP_WRITEV + 1)
#define IORING_OP_READ_FIXED (IORING_OP_FSYNC + 1)
#define IORING_OP_WRITE_FIXED (IORING_OP_READ_FIXED + 1)
#define IORING_OP_POLL_ADD (IORING_OP_WRITE_FIXED + 1)
#define IORING_OP_POLL_REMOVE (IORING_OP_POLL_ADD + 1)
#define IORING_OP_SYNC_FILE_RANGE (IORING_OP_POLL_REMOVE + 1)
#define IORING_OP_SENDMSG (IORING_OP_SYNC_FILE_RANGE + 1)
#define IORING_OP_RECVMSG (IORING_OP_SENDMSG + 1)
#define IORING_OP_TIMEOUT (IORING_OP_RECVMSG + 1)
#define IORING_OP_TIMEOUT_REMOVE (IORING_OP_TIMEOUT + 1)
#define IORING_OP_ACCEPT (IORING_OP_TIMEOUT_REMOVE + 1)
#define IORING_OP_ASYNC_CANCEL (IORING_OP_ACCEPT + 1)
#define IORING_OP_LINK_TIMEOUT (IORING_OP_ASYNC_CANCEL + 1)
#define IORING_OP_CONNECT (IORING_OP_LINK_TIMEOUT + 1)
#define IORING_OP_FALLOCATE (IORING_OP_CONNECT + 1)
#define IORING_OP_OPENAT (IORING_OP_FALLOCATE + 1)
#define IORING_OP_CLOSE (IORING_OP_OPENAT + 1)
#define IORING_OP_FILES_UPDATE (IORING_OP_CLOSE + 1)
#define IORING_OP_STATX (IORING_OP_FILES_UPDATE + 1)
#define IORING_OP_READ (IORING_OP_STATX + 1)
#define IORING_OP_WRITE (IORING_OP_READ + 1)
#define IORING_OP_FADVISE (IORING_OP_WRITE + 1)
#define IORING_OP_MADVISE (IORING_OP_FADVISE + 1)
#define IORING_OP_SEND (IORING_OP_MADVISE + 1)
#define IORING_OP_RECV (IORING_OP_SEND + 1)
#define IORING_OP_OPENAT2 (IORING_OP_RECV + 1)
#define IORING_OP_EPOLL_CTL (IORING_OP_OPENAT2 + 1)
#define IORING_OP_SPLICE (IORING_OP_EPOLL_CTL + 1)
#define IORING_OP_PROVIDE_BUFFERS (IORING_OP_SPLICE + 1)
#define IORING_OP_REMOVE_BUFFERS (IORING_OP_PROVIDE_BUFFERS + 1)
#define IORING_OP_TEE (IORING_OP_REMOVE_BUFFERS + 1)
#define IORING_OP_SHUTDOWN (IORING_OP_TEE + 1)
#define IORING_OP_RENAMEAT (IORING_OP_SHUTDOWN + 1)
#define IORING_OP_UNLINKAT (IORING_OP_RENAMEAT + 1)
#define IORING_OP_MKDIRAT (IORING_OP_UNLINKAT + 1)
#define IORING_OP_SYMLINKAT (IORING_OP_MKDIRAT + 1)
#define IORING_OP_LINKAT (IORING_OP_SYMLINKAT + 1)
#define IORING_OP_MSG_RING (IORING_OP_LINKAT + 1)
#define IORING_OP_FSETXATTR (IORING_OP_MSG_RING + 1)
#define IORING_OP_SETXATTR (IORING_OP_FSETXATTR + 1)
#define IORING_OP_FGETXATTR (IORING_OP_SETXATTR + 1)
#define IORING_OP_GETXATTR (IORING_OP_FGETXATTR + 1)
#define IORING_OP_SOCKET (IORING_OP_GETXATTR + 1)
#define IORING_OP_URING_CMD (IORING_OP_SOCKET + 1)
#define IORING_OP_SEND_ZC (IORING_OP_URING_CMD + 1)
#define IORING_OP_SENDMSG_ZC (IORING_OP_SEND_ZC + 1)
#define IORING_OP_READ_MULTISHOT (IORING_OP_SENDMSG_ZC + 1)
#define IORING_OP_WAITID (IORING_OP_READ_MULTISHOT + 1)
#define IORING_OP_FUTEX_WAIT (IORING_OP_WAITID + 1)
#define IORING_OP_FUTEX_WAKE (IORING_OP_FUTEX_WAIT + 1)
#define IORING_OP_FUTEX_WAITV (IORING_OP_FUTEX_WAKE + 1)
#define IORING_OP_FIXED_FD_INSTALL (IORING_OP_FUTEX_WAITV + 1)
#define IORING_OP_FTRUNCATE (IORING_OP_FIXED_FD_INSTALL + 1)
#define IORING_OP_BIND (IORING_OP_FTRUNCATE + 1)
#define IORING_OP_LISTEN (IORING_OP_BIND + 1)
#define IORING_OP_RECV_ZC (IORING_OP_LISTEN + 1)
#define IORING_OP_EPOLL_WAIT (IORING_OP_RECV_ZC + 1)
#define IORING_OP_READV_FIXED (IORING_OP_EPOLL_WAIT + 1)
#define IORING_OP_WRITEV_FIXED (IORING_OP_READV_FIXED + 1)
#define IORING_OP_PIPE (IORING_OP_WRITEV_FIXED + 1)
#define IORING_OP_NOP128 (IORING_OP_PIPE + 1)
#define IORING_OP_URING_CMD128 (IORING_OP_NOP128 + 1)
#define IORING_OP_LAST (IORING_OP_URING_CMD128 + 1)
#define IORING_MSG_DATA (0)
#define IORING_MSG_SEND_FD (IORING_MSG_DATA + 1)
#define IORING_REGISTER_BUFFERS (0)
#define IORING_UNREGISTER_BUFFERS (1)
#define IORING_REGISTER_FILES (2)
#define IORING_UNREGISTER_FILES (3)
#define IORING_REGISTER_EVENTFD (4)
#define IORING_UNREGISTER_EVENTFD (5)
#define IORING_REGISTER_FILES_UPDATE (6)
#define IORING_REGISTER_EVENTFD_ASYNC (7)
#define IORING_REGISTER_PROBE (8)
#define IORING_REGISTER_PERSONALITY (9)
#define IORING_UNREGISTER_PERSONALITY (10)
#define IORING_REGISTER_RESTRICTIONS (11)
#define IORING_REGISTER_ENABLE_RINGS (12)
#define IORING_REGISTER_FILES2 (13)
#define IORING_REGISTER_FILES_UPDATE2 (14)
#define IORING_REGISTER_BUFFERS2 (15)
#define IORING_REGISTER_BUFFERS_UPDATE (16)
#define IORING_REGISTER_IOWQ_AFF (17)
#define IORING_UNREGISTER_IOWQ_AFF (18)
#define IORING_REGISTER_IOWQ_MAX_WORKERS (19)
#define IORING_REGISTER_RING_FDS (20)
#define IORING_UNREGISTER_RING_FDS (21)
#define IORING_REGISTER_PBUF_RING (22)
#define IORING_UNREGISTER_PBUF_RING (23)
#define IORING_REGISTER_SYNC_CANCEL (24)
#define IORING_REGISTER_FILE_ALLOC_RANGE (25)
#define IORING_REGISTER_PBUF_STATUS (26)
#define IORING_REGISTER_NAPI (27)
#define IORING_UNREGISTER_NAPI (28)
#define IORING_REGISTER_CLOCK (29)
#define IORING_REGISTER_CLONE_BUFFERS (30)
#define IORING_REGISTER_SEND_MSG_RING (31)
#define IORING_REGISTER_ZCRX_IFQ (32)
#define IORING_REGISTER_RESIZE_RINGS (33)
#define IORING_REGISTER_MEM_REGION (34)
#define IORING_REGISTER_QUERY (35)
#define IORING_REGISTER_ZCRX_CTRL (36)
#define IORING_REGISTER_BPF_FILTER (37)
#define IORING_REGISTER_LAST (IORING_REGISTER_BPF_FILTER + 1)
#define IORING_REGISTER_USE_REGISTERED_RING (1U << 31)
#define IORING_MEM_REGION_TYPE_USER (1)
#define IORING_MEM_REGION_REG_WAIT_ARG (1)
#define IORING_REGISTER_SRC_REGISTERED ((1U << 0))
#define IORING_REGISTER_DST_REPLACE ((1U << 1))
#define IOU_PBUF_RING_MMAP (1)
#define IOU_PBUF_RING_INC (2)
#define IO_URING_NAPI_REGISTER_OP (0)
#define IO_URING_NAPI_STATIC_ADD_ID (1)
#define IO_URING_NAPI_STATIC_DEL_ID (2)
#define IO_URING_NAPI_TRACKING_DYNAMIC (0)
#define IO_URING_NAPI_TRACKING_STATIC (1)
#define IO_URING_NAPI_TRACKING_INACTIVE (255)
#define IORING_RESTRICTION_REGISTER_OP (0)
#define IORING_RESTRICTION_SQE_OP (1)
#define IORING_RESTRICTION_SQE_FLAGS_ALLOWED (2)
#define IORING_RESTRICTION_SQE_FLAGS_REQUIRED (3)
#define IORING_RESTRICTION_LAST (IORING_RESTRICTION_SQE_FLAGS_REQUIRED + 1)
#define IORING_REG_WAIT_TS ((1U << 0))
#define IORING_ZCRX_AREA_DMABUF (1)
#define IORING_RW_ATTR_FLAG_PI ((1U << 0))
#define IORING_FILE_INDEX_ALLOC ((~0U))
#define IOSQE_FIXED_FILE ((1U << IOSQE_FIXED_FILE_BIT))
#define IOSQE_IO_DRAIN ((1U << IOSQE_IO_DRAIN_BIT))
#define IOSQE_IO_LINK ((1U << IOSQE_IO_LINK_BIT))
#define IOSQE_IO_HARDLINK ((1U << IOSQE_IO_HARDLINK_BIT))
#define IOSQE_ASYNC ((1U << IOSQE_ASYNC_BIT))
#define IOSQE_BUFFER_SELECT ((1U << IOSQE_BUFFER_SELECT_BIT))
#define IOSQE_CQE_SKIP_SUCCESS ((1U << IOSQE_CQE_SKIP_SUCCESS_BIT))
#define IORING_SETUP_IOPOLL ((1U << 0))
#define IORING_SETUP_SQPOLL ((1U << 1))
#define IORING_SETUP_SQ_AFF ((1U << 2))
#define IORING_SETUP_CQSIZE ((1U << 3))
#define IORING_SETUP_CLAMP ((1U << 4))
#define IORING_SETUP_ATTACH_WQ ((1U << 5))
#define IORING_SETUP_R_DISABLED ((1U << 6))
#define IORING_SETUP_SUBMIT_ALL ((1U << 7))
#define IORING_SETUP_COOP_TASKRUN ((1U << 8))
#define IORING_SETUP_TASKRUN_FLAG ((1U << 9))
#define IORING_SETUP_SQE128 ((1U << 10))
#define IORING_SETUP_CQE32 ((1U << 11))
#define IORING_SETUP_SINGLE_ISSUER ((1U << 12))
#define IORING_SETUP_DEFER_TASKRUN ((1U << 13))
#define IORING_SETUP_NO_MMAP ((1U << 14))
#define IORING_SETUP_REGISTERED_FD_ONLY ((1U << 15))
#define IORING_SETUP_NO_SQARRAY ((1U << 16))
#define IORING_SETUP_HYBRID_IOPOLL ((1U << 17))
#define IORING_SETUP_CQE_MIXED ((1U << 18))
#define IORING_SETUP_SQE_MIXED ((1U << 19))
#define IORING_SETUP_SQ_REWIND ((1U << 20))
#define IORING_URING_CMD_FIXED ((1U << 0))
#define IORING_URING_CMD_MULTISHOT ((1U << 1))
#define IORING_URING_CMD_MASK ((IORING_URING_CMD_FIXED | IORING_URING_CMD_MULTISHOT))
#define IORING_FSYNC_DATASYNC ((1U << 0))
#define IORING_TIMEOUT_ABS ((1U << 0))
#define IORING_TIMEOUT_UPDATE ((1U << 1))
#define IORING_TIMEOUT_BOOTTIME ((1U << 2))
#define IORING_TIMEOUT_REALTIME ((1U << 3))
#define IORING_LINK_TIMEOUT_UPDATE ((1U << 4))
#define IORING_TIMEOUT_ETIME_SUCCESS ((1U << 5))
#define IORING_TIMEOUT_MULTISHOT ((1U << 6))
#define IORING_TIMEOUT_CLOCK_MASK ((IORING_TIMEOUT_BOOTTIME | IORING_TIMEOUT_REALTIME))
#define IORING_TIMEOUT_UPDATE_MASK ((IORING_TIMEOUT_UPDATE | IORING_LINK_TIMEOUT_UPDATE))
#define IORING_POLL_ADD_MULTI ((1U << 0))
#define IORING_POLL_UPDATE_EVENTS ((1U << 1))
#define IORING_POLL_UPDATE_USER_DATA ((1U << 2))
#define IORING_POLL_ADD_LEVEL ((1U << 3))
#define IORING_ASYNC_CANCEL_ALL ((1U << 0))
#define IORING_ASYNC_CANCEL_FD ((1U << 1))
#define IORING_ASYNC_CANCEL_ANY ((1U << 2))
#define IORING_ASYNC_CANCEL_FD_FIXED ((1U << 3))
#define IORING_ASYNC_CANCEL_USERDATA ((1U << 4))
#define IORING_ASYNC_CANCEL_OP ((1U << 5))
#define IORING_RECVSEND_POLL_FIRST ((1U << 0))
#define IORING_RECV_MULTISHOT ((1U << 1))
#define IORING_RECVSEND_FIXED_BUF ((1U << 2))
#define IORING_SEND_ZC_REPORT_USAGE ((1U << 3))
#define IORING_RECVSEND_BUNDLE ((1U << 4))
#define IORING_SEND_VECTORIZED ((1U << 5))
#define IORING_NOTIF_USAGE_ZC_COPIED ((1U << 31))
#define IORING_ACCEPT_MULTISHOT ((1U << 0))
#define IORING_ACCEPT_DONTWAIT ((1U << 1))
#define IORING_ACCEPT_POLL_FIRST ((1U << 2))
#define IORING_MSG_RING_CQE_SKIP ((1U << 0))
#define IORING_MSG_RING_FLAGS_PASS ((1U << 1))
#define IORING_FIXED_FD_NO_CLOEXEC ((1U << 0))
#define IORING_NOP_INJECT_RESULT ((1U << 0))
#define IORING_NOP_FILE ((1U << 1))
#define IORING_NOP_FIXED_FILE ((1U << 2))
#define IORING_NOP_FIXED_BUFFER ((1U << 3))
#define IORING_NOP_TW ((1U << 4))
#define IORING_NOP_CQE32 ((1U << 5))
#define IORING_CQE_F_BUFFER ((1U << 0))
#define IORING_CQE_F_MORE ((1U << 1))
#define IORING_CQE_F_SOCK_NONEMPTY ((1U << 2))
#define IORING_CQE_F_NOTIF ((1U << 3))
#define IORING_CQE_F_BUF_MORE ((1U << 4))
#define IORING_CQE_F_SKIP ((1U << 5))
#define IORING_CQE_F_32 ((1U << 15))
#define IORING_CQE_BUFFER_SHIFT (16)
#define IORING_OFF_SQ_RING (0ULL)
#define IORING_OFF_CQ_RING (0x8000000ULL)
#define IORING_OFF_SQES (0x10000000ULL)
#define IORING_OFF_PBUF_RING (0x80000000ULL)
#define IORING_OFF_PBUF_SHIFT (16)
#define IORING_OFF_MMAP_MASK (0xf8000000ULL)
#define IORING_SQ_NEED_WAKEUP ((1U << 0))
#define IORING_SQ_CQ_OVERFLOW ((1U << 1))
#define IORING_SQ_TASKRUN ((1U << 2))
#define IORING_CQ_EVENTFD_DISABLED ((1U << 0))
#define IORING_ENTER_GETEVENTS ((1U << 0))
#define IORING_ENTER_SQ_WAKEUP ((1U << 1))
#define IORING_ENTER_SQ_WAIT ((1U << 2))
#define IORING_ENTER_EXT_ARG ((1U << 3))
#define IORING_ENTER_REGISTERED_RING ((1U << 4))
#define IORING_ENTER_ABS_TIMER ((1U << 5))
#define IORING_ENTER_EXT_ARG_REG ((1U << 6))
#define IORING_ENTER_NO_IOWAIT ((1U << 7))
#define IORING_FEAT_SINGLE_MMAP ((1U << 0))
#define IORING_FEAT_NODROP ((1U << 1))
#define IORING_FEAT_SUBMIT_STABLE ((1U << 2))
#define IORING_FEAT_RW_CUR_POS ((1U << 3))
#define IORING_FEAT_CUR_PERSONALITY ((1U << 4))
#define IORING_FEAT_FAST_POLL ((1U << 5))
#define IORING_FEAT_POLL_32BITS ((1U << 6))
#define IORING_FEAT_SQPOLL_NONFIXED ((1U << 7))
#define IORING_FEAT_EXT_ARG ((1U << 8))
#define IORING_FEAT_NATIVE_WORKERS ((1U << 9))
#define IORING_FEAT_RSRC_TAGS ((1U << 10))
#define IORING_FEAT_CQE_SKIP ((1U << 11))
#define IORING_FEAT_LINKED_FILE ((1U << 12))
#define IORING_FEAT_REG_REG_RING ((1U << 13))
#define IORING_FEAT_RECVSEND_BUNDLE ((1U << 14))
#define IORING_FEAT_MIN_TIMEOUT ((1U << 15))
#define IORING_FEAT_RW_ATTR ((1U << 16))
#define IORING_FEAT_NO_IOWAIT ((1U << 17))
#define IORING_RSRC_REGISTER_SPARSE ((1U << 0))
#define IORING_REGISTER_FILES_SKIP ((-2))
#define IO_URING_OP_SUPPORTED ((1U << 0))
#define IORING_TIMESTAMP_HW_SHIFT (16)
#define IORING_TIMESTAMP_TYPE_SHIFT ((IORING_TIMESTAMP_HW_SHIFT + 1))
#define IORING_CQE_F_TSTAMP_HW (((std::uint32_t)1 << IORING_TIMESTAMP_HW_SHIFT))
#define IORING_ZCRX_AREA_SHIFT (48)
#define IORING_ZCRX_AREA_MASK ((~(((std::uint64_t)1 << IORING_ZCRX_AREA_SHIFT) - 1)))
#include <uring/abi/uapi_io_uring.hpp>
static_assert(uring::abi::kSqeFixedFileBit == IOSQE_FIXED_FILE_BIT);
static_assert(uring::abi::kSqeIoDrainBit == IOSQE_IO_DRAIN_BIT);
static_assert(uring::abi::kSqeIoLinkBit == IOSQE_IO_LINK_BIT);
static_assert(uring::abi::kSqeIoHardlinkBit == IOSQE_IO_HARDLINK_BIT);
static_assert(uring::abi::kSqeAsyncBit == IOSQE_ASYNC_BIT);
static_assert(uring::abi::kSqeBufferSelectBit == IOSQE_BUFFER_SELECT_BIT);
static_assert(uring::abi::kSqeCqeSkipSuccessBit == IOSQE_CQE_SKIP_SUCCESS_BIT);
static_assert(uring::abi::kOpNop == IORING_OP_NOP);
static_assert(uring::abi::kOpReadv == IORING_OP_READV);
static_assert(uring::abi::kOpWritev == IORING_OP_WRITEV);
static_assert(uring::abi::kOpFsync == IORING_OP_FSYNC);
static_assert(uring::abi::kOpReadFixed == IORING_OP_READ_FIXED);
static_assert(uring::abi::kOpWriteFixed == IORING_OP_WRITE_FIXED);
static_assert(uring::abi::kOpPollAdd == IORING_OP_POLL_ADD);
static_assert(uring::abi::kOpPollRemove == IORING_OP_POLL_REMOVE);
static_assert(uring::abi::kOpSyncFileRange == IORING_OP_SYNC_FILE_RANGE);
static_assert(uring::abi::kOpSendmsg == IORING_OP_SENDMSG);
static_assert(uring::abi::kOpRecvmsg == IORING_OP_RECVMSG);
static_assert(uring::abi::kOpTimeout == IORING_OP_TIMEOUT);
static_assert(uring::abi::kOpTimeoutRemove == IORING_OP_TIMEOUT_REMOVE);
static_assert(uring::abi::kOpAccept == IORING_OP_ACCEPT);
static_assert(uring::abi::kOpAsyncCancel == IORING_OP_ASYNC_CANCEL);
static_assert(uring::abi::kOpLinkTimeout == IORING_OP_LINK_TIMEOUT);
static_assert(uring::abi::kOpConnect == IORING_OP_CONNECT);
static_assert(uring::abi::kOpFallocate == IORING_OP_FALLOCATE);
static_assert(uring::abi::kOpOpenat == IORING_OP_OPENAT);
static_assert(uring::abi::kOpClose == IORING_OP_CLOSE);
static_assert(uring::abi::kOpFilesUpdate == IORING_OP_FILES_UPDATE);
static_assert(uring::abi::kOpStatx == IORING_OP_STATX);
static_assert(uring::abi::kOpRead == IORING_OP_READ);
static_assert(uring::abi::kOpWrite == IORING_OP_WRITE);
static_assert(uring::abi::kOpFadvise == IORING_OP_FADVISE);
static_assert(uring::abi::kOpMadvise == IORING_OP_MADVISE);
static_assert(uring::abi::kOpSend == IORING_OP_SEND);
static_assert(uring::abi::kOpRecv == IORING_OP_RECV);
static_assert(uring::abi::kOpOpenat2 == IORING_OP_OPENAT2);
static_assert(uring::abi::kOpEpollCtl == IORING_OP_EPOLL_CTL);
static_assert(uring::abi::kOpSplice == IORING_OP_SPLICE);
static_assert(uring::abi::kOpProvideBuffers == IORING_OP_PROVIDE_BUFFERS);
static_assert(uring::abi::kOpRemoveBuffers == IORING_OP_REMOVE_BUFFERS);
static_assert(uring::abi::kOpTee == IORING_OP_TEE);
static_assert(uring::abi::kOpShutdown == IORING_OP_SHUTDOWN);
static_assert(uring::abi::kOpRenameat == IORING_OP_RENAMEAT);
static_assert(uring::abi::kOpUnlinkat == IORING_OP_UNLINKAT);
static_assert(uring::abi::kOpMkdirat == IORING_OP_MKDIRAT);
static_assert(uring::abi::kOpSymlinkat == IORING_OP_SYMLINKAT);
static_assert(uring::abi::kOpLinkat == IORING_OP_LINKAT);
static_assert(uring::abi::kOpMsgRing == IORING_OP_MSG_RING);
static_assert(uring::abi::kOpFsetxattr == IORING_OP_FSETXATTR);
static_assert(uring::abi::kOpSetxattr == IORING_OP_SETXATTR);
static_assert(uring::abi::kOpFgetxattr == IORING_OP_FGETXATTR);
static_assert(uring::abi::kOpGetxattr == IORING_OP_GETXATTR);
static_assert(uring::abi::kOpSocket == IORING_OP_SOCKET);
static_assert(uring::abi::kOpUringCmd == IORING_OP_URING_CMD);
static_assert(uring::abi::kOpSendZc == IORING_OP_SEND_ZC);
static_assert(uring::abi::kOpSendmsgZc == IORING_OP_SENDMSG_ZC);
static_assert(uring::abi::kOpReadMultishot == IORING_OP_READ_MULTISHOT);
static_assert(uring::abi::kOpWaitid == IORING_OP_WAITID);
static_assert(uring::abi::kOpFutexWait == IORING_OP_FUTEX_WAIT);
static_assert(uring::abi::kOpFutexWake == IORING_OP_FUTEX_WAKE);
static_assert(uring::abi::kOpFutexWaitv == IORING_OP_FUTEX_WAITV);
static_assert(uring::abi::kOpFixedFdInstall == IORING_OP_FIXED_FD_INSTALL);
static_assert(uring::abi::kOpFtruncate == IORING_OP_FTRUNCATE);
static_assert(uring::abi::kOpBind == IORING_OP_BIND);
static_assert(uring::abi::kOpListen == IORING_OP_LISTEN);
static_assert(uring::abi::kOpRecvZc == IORING_OP_RECV_ZC);
static_assert(uring::abi::kOpEpollWait == IORING_OP_EPOLL_WAIT);
static_assert(uring::abi::kOpReadvFixed == IORING_OP_READV_FIXED);
static_assert(uring::abi::kOpWritevFixed == IORING_OP_WRITEV_FIXED);
static_assert(uring::abi::kOpPipe == IORING_OP_PIPE);
static_assert(uring::abi::kOpNop128 == IORING_OP_NOP128);
static_assert(uring::abi::kOpUringCmd128 == IORING_OP_URING_CMD128);
static_assert(uring::abi::kOpLast == IORING_OP_LAST);
static_assert(uring::abi::kMsgData == IORING_MSG_DATA);
static_assert(uring::abi::kMsgSendFd == IORING_MSG_SEND_FD);
static_assert(uring::abi::kRegisterBuffers == IORING_REGISTER_BUFFERS);
static_assert(uring::abi::kUnregisterBuffers == IORING_UNREGISTER_BUFFERS);
static_assert(uring::abi::kRegisterFiles == IORING_REGISTER_FILES);
static_assert(uring::abi::kUnregisterFiles == IORING_UNREGISTER_FILES);
static_assert(uring::abi::kRegisterEventfd == IORING_REGISTER_EVENTFD);
static_assert(uring::abi::kUnregisterEventfd == IORING_UNREGISTER_EVENTFD);
static_assert(uring::abi::kRegisterFilesUpdate == IORING_REGISTER_FILES_UPDATE);
static_assert(uring::abi::kRegisterEventfdAsync == IORING_REGISTER_EVENTFD_ASYNC);
static_assert(uring::abi::kRegisterProbe == IORING_REGISTER_PROBE);
static_assert(uring::abi::kRegisterPersonality == IORING_REGISTER_PERSONALITY);
static_assert(uring::abi::kUnregisterPersonality == IORING_UNREGISTER_PERSONALITY);
static_assert(uring::abi::kRegisterRestrictions == IORING_REGISTER_RESTRICTIONS);
static_assert(uring::abi::kRegisterEnableRings == IORING_REGISTER_ENABLE_RINGS);
static_assert(uring::abi::kRegisterFiles2 == IORING_REGISTER_FILES2);
static_assert(uring::abi::kRegisterFilesUpdate2 == IORING_REGISTER_FILES_UPDATE2);
static_assert(uring::abi::kRegisterBuffers2 == IORING_REGISTER_BUFFERS2);
static_assert(uring::abi::kRegisterBuffersUpdate == IORING_REGISTER_BUFFERS_UPDATE);
static_assert(uring::abi::kRegisterIoWqAff == IORING_REGISTER_IOWQ_AFF);
static_assert(uring::abi::kUnregisterIoWqAff == IORING_UNREGISTER_IOWQ_AFF);
static_assert(uring::abi::kRegisterIoWqMaxWorkers == IORING_REGISTER_IOWQ_MAX_WORKERS);
static_assert(uring::abi::kRegisterRingFds == IORING_REGISTER_RING_FDS);
static_assert(uring::abi::kUnregisterRingFds == IORING_UNREGISTER_RING_FDS);
static_assert(uring::abi::kRegisterPbufRing == IORING_REGISTER_PBUF_RING);
static_assert(uring::abi::kUnregisterPbufRing == IORING_UNREGISTER_PBUF_RING);
static_assert(uring::abi::kRegisterSyncCancel == IORING_REGISTER_SYNC_CANCEL);
static_assert(uring::abi::kRegisterFileAllocRange == IORING_REGISTER_FILE_ALLOC_RANGE);
static_assert(uring::abi::kRegisterPbufStatus == IORING_REGISTER_PBUF_STATUS);
static_assert(uring::abi::kRegisterNapi == IORING_REGISTER_NAPI);
static_assert(uring::abi::kUnregisterNapi == IORING_UNREGISTER_NAPI);
static_assert(uring::abi::kRegisterClock == IORING_REGISTER_CLOCK);
static_assert(uring::abi::kRegisterCloneBuffers == IORING_REGISTER_CLONE_BUFFERS);
static_assert(uring::abi::kRegisterSendMsgRing == IORING_REGISTER_SEND_MSG_RING);
static_assert(uring::abi::kRegisterZcrxIfq == IORING_REGISTER_ZCRX_IFQ);
static_assert(uring::abi::kRegisterResizeRings == IORING_REGISTER_RESIZE_RINGS);
static_assert(uring::abi::kRegisterMemRegion == IORING_REGISTER_MEM_REGION);
static_assert(uring::abi::kRegisterQuery == IORING_REGISTER_QUERY);
static_assert(uring::abi::kRegisterZcrxCtrl == IORING_REGISTER_ZCRX_CTRL);
static_assert(uring::abi::kRegisterBpfFilter == IORING_REGISTER_BPF_FILTER);
static_assert(uring::abi::kRegisterLast == IORING_REGISTER_LAST);
static_assert(uring::abi::kRegisterUseRegisteredRing == IORING_REGISTER_USE_REGISTERED_RING);
static_assert(uring::abi::kMemRegionTypeUser == IORING_MEM_REGION_TYPE_USER);
static_assert(uring::abi::kMemRegionRegWaitArg == IORING_MEM_REGION_REG_WAIT_ARG);
static_assert(uring::abi::kRegisterSrcRegistered == IORING_REGISTER_SRC_REGISTERED);
static_assert(uring::abi::kRegisterDstReplace == IORING_REGISTER_DST_REPLACE);
static_assert(uring::abi::kPbufRingMmap == IOU_PBUF_RING_MMAP);
static_assert(uring::abi::kPbufRingInc == IOU_PBUF_RING_INC);
static_assert(uring::abi::kIoUringNapiRegisterOp == IO_URING_NAPI_REGISTER_OP);
static_assert(uring::abi::kIoUringNapiStaticAddId == IO_URING_NAPI_STATIC_ADD_ID);
static_assert(uring::abi::kIoUringNapiStaticDelId == IO_URING_NAPI_STATIC_DEL_ID);
static_assert(uring::abi::kIoUringNapiTrackingDynamic == IO_URING_NAPI_TRACKING_DYNAMIC);
static_assert(uring::abi::kIoUringNapiTrackingStatic == IO_URING_NAPI_TRACKING_STATIC);
static_assert(uring::abi::kIoUringNapiTrackingInactive == IO_URING_NAPI_TRACKING_INACTIVE);
static_assert(uring::abi::kRestrictionRegisterOp == IORING_RESTRICTION_REGISTER_OP);
static_assert(uring::abi::kRestrictionSqeOp == IORING_RESTRICTION_SQE_OP);
static_assert(uring::abi::kRestrictionSqeFlagsAllowed == IORING_RESTRICTION_SQE_FLAGS_ALLOWED);
static_assert(uring::abi::kRestrictionSqeFlagsRequired == IORING_RESTRICTION_SQE_FLAGS_REQUIRED);
static_assert(uring::abi::kRestrictionLast == IORING_RESTRICTION_LAST);
static_assert(uring::abi::kRegWaitTs == IORING_REG_WAIT_TS);
static_assert(uring::abi::kZcrxAreaDmabuf == IORING_ZCRX_AREA_DMABUF);
static_assert(uring::abi::kRwAttrFlagPi == IORING_RW_ATTR_FLAG_PI);
static_assert(uring::abi::kFileIndexAlloc == IORING_FILE_INDEX_ALLOC);
static_assert(uring::abi::kSqeFixedFile == IOSQE_FIXED_FILE);
static_assert(uring::abi::kSqeIoDrain == IOSQE_IO_DRAIN);
static_assert(uring::abi::kSqeIoLink == IOSQE_IO_LINK);
static_assert(uring::abi::kSqeIoHardlink == IOSQE_IO_HARDLINK);
static_assert(uring::abi::kSqeAsync == IOSQE_ASYNC);
static_assert(uring::abi::kSqeBufferSelect == IOSQE_BUFFER_SELECT);
static_assert(uring::abi::kSqeCqeSkipSuccess == IOSQE_CQE_SKIP_SUCCESS);
static_assert(uring::abi::kSetupIoPoll == IORING_SETUP_IOPOLL);
static_assert(uring::abi::kSetupSqPoll == IORING_SETUP_SQPOLL);
static_assert(uring::abi::kSetupSqAff == IORING_SETUP_SQ_AFF);
static_assert(uring::abi::kSetupCqSize == IORING_SETUP_CQSIZE);
static_assert(uring::abi::kSetupClamp == IORING_SETUP_CLAMP);
static_assert(uring::abi::kSetupAttachWq == IORING_SETUP_ATTACH_WQ);
static_assert(uring::abi::kSetupRDisabled == IORING_SETUP_R_DISABLED);
static_assert(uring::abi::kSetupSubmitAll == IORING_SETUP_SUBMIT_ALL);
static_assert(uring::abi::kSetupCoopTaskrun == IORING_SETUP_COOP_TASKRUN);
static_assert(uring::abi::kSetupTaskrunFlag == IORING_SETUP_TASKRUN_FLAG);
static_assert(uring::abi::kSetupSqe128 == IORING_SETUP_SQE128);
static_assert(uring::abi::kSetupCqe32 == IORING_SETUP_CQE32);
static_assert(uring::abi::kSetupSingleIssuer == IORING_SETUP_SINGLE_ISSUER);
static_assert(uring::abi::kSetupDeferTaskrun == IORING_SETUP_DEFER_TASKRUN);
static_assert(uring::abi::kSetupNoMmap == IORING_SETUP_NO_MMAP);
static_assert(uring::abi::kSetupRegisteredFdOnly == IORING_SETUP_REGISTERED_FD_ONLY);
static_assert(uring::abi::kSetupNoSqArray == IORING_SETUP_NO_SQARRAY);
static_assert(uring::abi::kSetupHybridIoPoll == IORING_SETUP_HYBRID_IOPOLL);
static_assert(uring::abi::kSetupCqeMixed == IORING_SETUP_CQE_MIXED);
static_assert(uring::abi::kSetupSqeMixed == IORING_SETUP_SQE_MIXED);
static_assert(uring::abi::kSetupSqRewind == IORING_SETUP_SQ_REWIND);
static_assert(uring::abi::kUringCmdFixed == IORING_URING_CMD_FIXED);
static_assert(uring::abi::kUringCmdMultishot == IORING_URING_CMD_MULTISHOT);
static_assert(uring::abi::kUringCmdMask == IORING_URING_CMD_MASK);
static_assert(uring::abi::kFsyncDatasync == IORING_FSYNC_DATASYNC);
static_assert(uring::abi::kTimeoutAbs == IORING_TIMEOUT_ABS);
static_assert(uring::abi::kTimeoutUpdate == IORING_TIMEOUT_UPDATE);
static_assert(uring::abi::kTimeoutBoottime == IORING_TIMEOUT_BOOTTIME);
static_assert(uring::abi::kTimeoutRealtime == IORING_TIMEOUT_REALTIME);
static_assert(uring::abi::kLinkTimeoutUpdate == IORING_LINK_TIMEOUT_UPDATE);
static_assert(uring::abi::kTimeoutEtimeSuccess == IORING_TIMEOUT_ETIME_SUCCESS);
static_assert(uring::abi::kTimeoutMultishot == IORING_TIMEOUT_MULTISHOT);
static_assert(uring::abi::kTimeoutClockMask == IORING_TIMEOUT_CLOCK_MASK);
static_assert(uring::abi::kTimeoutUpdateMask == IORING_TIMEOUT_UPDATE_MASK);
static_assert(uring::abi::kPollAddMulti == IORING_POLL_ADD_MULTI);
static_assert(uring::abi::kPollUpdateEvents == IORING_POLL_UPDATE_EVENTS);
static_assert(uring::abi::kPollUpdateUserData == IORING_POLL_UPDATE_USER_DATA);
static_assert(uring::abi::kPollAddLevel == IORING_POLL_ADD_LEVEL);
static_assert(uring::abi::kAsyncCancelAll == IORING_ASYNC_CANCEL_ALL);
static_assert(uring::abi::kAsyncCancelFd == IORING_ASYNC_CANCEL_FD);
static_assert(uring::abi::kAsyncCancelAny == IORING_ASYNC_CANCEL_ANY);
static_assert(uring::abi::kAsyncCancelFdFixed == IORING_ASYNC_CANCEL_FD_FIXED);
static_assert(uring::abi::kAsyncCancelUserdata == IORING_ASYNC_CANCEL_USERDATA);
static_assert(uring::abi::kAsyncCancelOp == IORING_ASYNC_CANCEL_OP);
static_assert(uring::abi::kRecvsendPollFirst == IORING_RECVSEND_POLL_FIRST);
static_assert(uring::abi::kRecvMultishot == IORING_RECV_MULTISHOT);
static_assert(uring::abi::kRecvsendFixedBuf == IORING_RECVSEND_FIXED_BUF);
static_assert(uring::abi::kSendZcReportUsage == IORING_SEND_ZC_REPORT_USAGE);
static_assert(uring::abi::kRecvsendBundle == IORING_RECVSEND_BUNDLE);
static_assert(uring::abi::kSendVectorized == IORING_SEND_VECTORIZED);
static_assert(uring::abi::kNotifUsageZcCopied == IORING_NOTIF_USAGE_ZC_COPIED);
static_assert(uring::abi::kAcceptMultishot == IORING_ACCEPT_MULTISHOT);
static_assert(uring::abi::kAcceptDontwait == IORING_ACCEPT_DONTWAIT);
static_assert(uring::abi::kAcceptPollFirst == IORING_ACCEPT_POLL_FIRST);
static_assert(uring::abi::kMsgRingCqeSkip == IORING_MSG_RING_CQE_SKIP);
static_assert(uring::abi::kMsgRingFlagsPass == IORING_MSG_RING_FLAGS_PASS);
static_assert(uring::abi::kFixedFdNoCloexec == IORING_FIXED_FD_NO_CLOEXEC);
static_assert(uring::abi::kNopInjectResult == IORING_NOP_INJECT_RESULT);
static_assert(uring::abi::kNopFile == IORING_NOP_FILE);
static_assert(uring::abi::kNopFixedFile == IORING_NOP_FIXED_FILE);
static_assert(uring::abi::kNopFixedBuffer == IORING_NOP_FIXED_BUFFER);
static_assert(uring::abi::kNopTw == IORING_NOP_TW);
static_assert(uring::abi::kNopCqe32 == IORING_NOP_CQE32);
static_assert(uring::abi::kCqeBuffer == IORING_CQE_F_BUFFER);
static_assert(uring::abi::kCqeMore == IORING_CQE_F_MORE);
static_assert(uring::abi::kCqeSockNonempty == IORING_CQE_F_SOCK_NONEMPTY);
static_assert(uring::abi::kCqeNotif == IORING_CQE_F_NOTIF);
static_assert(uring::abi::kCqeBufMore == IORING_CQE_F_BUF_MORE);
static_assert(uring::abi::kCqeSkip == IORING_CQE_F_SKIP);
static_assert(uring::abi::kCqe32 == IORING_CQE_F_32);
static_assert(uring::abi::kCqeBufferShift == IORING_CQE_BUFFER_SHIFT);
static_assert(uring::abi::kOffSqRing == IORING_OFF_SQ_RING);
static_assert(uring::abi::kOffCqRing == IORING_OFF_CQ_RING);
static_assert(uring::abi::kOffSqes == IORING_OFF_SQES);
static_assert(uring::abi::kOffPbufRing == IORING_OFF_PBUF_RING);
static_assert(uring::abi::kOffPbufShift == IORING_OFF_PBUF_SHIFT);
static_assert(uring::abi::kOffMmapMask == IORING_OFF_MMAP_MASK);
static_assert(uring::abi::kSqNeedWakeup == IORING_SQ_NEED_WAKEUP);
static_assert(uring::abi::kSqCqOverflow == IORING_SQ_CQ_OVERFLOW);
static_assert(uring::abi::kSqTaskrun == IORING_SQ_TASKRUN);
static_assert(uring::abi::kCqEventfdDisabled == IORING_CQ_EVENTFD_DISABLED);
static_assert(uring::abi::kEnterGetevents == IORING_ENTER_GETEVENTS);
static_assert(uring::abi::kEnterSqWakeup == IORING_ENTER_SQ_WAKEUP);
static_assert(uring::abi::kEnterSqWait == IORING_ENTER_SQ_WAIT);
static_assert(uring::abi::kEnterExtArg == IORING_ENTER_EXT_ARG);
static_assert(uring::abi::kEnterRegisteredRing == IORING_ENTER_REGISTERED_RING);
static_assert(uring::abi::kEnterAbsTimer == IORING_ENTER_ABS_TIMER);
static_assert(uring::abi::kEnterExtArgReg == IORING_ENTER_EXT_ARG_REG);
static_assert(uring::abi::kEnterNoIowait == IORING_ENTER_NO_IOWAIT);
static_assert(uring::abi::kFeatSingleMmap == IORING_FEAT_SINGLE_MMAP);
static_assert(uring::abi::kFeatNoDrop == IORING_FEAT_NODROP);
static_assert(uring::abi::kFeatSubmitStable == IORING_FEAT_SUBMIT_STABLE);
static_assert(uring::abi::kFeatRwCurPos == IORING_FEAT_RW_CUR_POS);
static_assert(uring::abi::kFeatCurPersonality == IORING_FEAT_CUR_PERSONALITY);
static_assert(uring::abi::kFeatFastPoll == IORING_FEAT_FAST_POLL);
static_assert(uring::abi::kFeatPoll32Bits == IORING_FEAT_POLL_32BITS);
static_assert(uring::abi::kFeatSqPollNonfixed == IORING_FEAT_SQPOLL_NONFIXED);
static_assert(uring::abi::kFeatExtArg == IORING_FEAT_EXT_ARG);
static_assert(uring::abi::kFeatNativeWorkers == IORING_FEAT_NATIVE_WORKERS);
static_assert(uring::abi::kFeatRsrcTags == IORING_FEAT_RSRC_TAGS);
static_assert(uring::abi::kFeatCqeSkip == IORING_FEAT_CQE_SKIP);
static_assert(uring::abi::kFeatLinkedFile == IORING_FEAT_LINKED_FILE);
static_assert(uring::abi::kFeatRegRegRing == IORING_FEAT_REG_REG_RING);
static_assert(uring::abi::kFeatRecvsendBundle == IORING_FEAT_RECVSEND_BUNDLE);
static_assert(uring::abi::kFeatMinTimeout == IORING_FEAT_MIN_TIMEOUT);
static_assert(uring::abi::kFeatRwAttr == IORING_FEAT_RW_ATTR);
static_assert(uring::abi::kFeatNoIowait == IORING_FEAT_NO_IOWAIT);
static_assert(uring::abi::kRsrcRegisterSparse == IORING_RSRC_REGISTER_SPARSE);
static_assert(uring::abi::kRegisterFilesSkip == IORING_REGISTER_FILES_SKIP);
static_assert(uring::abi::kIoUringOpSupported == IO_URING_OP_SUPPORTED);
static_assert(uring::abi::kTimestampHwShift == IORING_TIMESTAMP_HW_SHIFT);
static_assert(uring::abi::kTimestampTypeShift == IORING_TIMESTAMP_TYPE_SHIFT);
static_assert(uring::abi::kCqeTstampHw == IORING_CQE_F_TSTAMP_HW);
static_assert(uring::abi::kZcrxAreaShift == IORING_ZCRX_AREA_SHIFT);
static_assert(uring::abi::kZcrxAreaMask == IORING_ZCRX_AREA_MASK);
int main() {}
