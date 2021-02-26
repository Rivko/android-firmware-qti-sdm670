#ifndef COMMON_SERVICE_01_H
#define COMMON_SERVICE_01_H
/**
  @file common_v01.h

  @brief This is the public header file which defines the common service Data structures.

  This header file defines the types and structures that were defined in
  common. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2006-2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/chre.dsp/1.2.0/chre_app_testproxy/inc/common_v01.h#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Fri Jul  8 2016 (Spin 0)
   From IDL File: common_v01.idl */

/** @defgroup common_qmi_consts Constant values defined in the IDL */
/** @defgroup common_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup common_qmi_enums Enumerated types used in QMI messages */
/** @defgroup common_qmi_messages Structures sent as QMI messages */
/** @defgroup common_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup common_qmi_accessor Accessor for QMI service object */
/** @defgroup common_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup common_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define COMMON_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define COMMON_V01_IDL_MINOR_VERS 0x0B
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define COMMON_V01_IDL_TOOL_VERS 0x06

/**
    @}
  */


/** @addtogroup common_qmi_consts
    @{
  */

/**  */
#define QMI_FALSE_V01 0x00
#define QMI_TRUE_V01 0x01
#define QMI_GET_SUPPORTED_MESSAGES_ARRAY_MAX_V01 8192
#define QMI_GET_SUPPORTED_FIELDS_ARRAY_MAX_V01 32
/**
    @}
  */

/** @addtogroup common_qmi_enums
    @{
  */
typedef enum {
  QMI_RESULT_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_RESULT_SUCCESS_V01 = 0, 
  QMI_RESULT_FAILURE_V01 = 1, 
  QMI_RESULT_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_result_type_v01;
/**
    @}
  */

/** @addtogroup common_qmi_enums
    @{
  */
typedef enum {
  QMI_ERROR_TYPE_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  QMI_ERR_NONE_V01 = 0x0000, 
  QMI_ERR_MALFORMED_MSG_V01 = 0x0001, 
  QMI_ERR_NO_MEMORY_V01 = 0x0002, 
  QMI_ERR_INTERNAL_V01 = 0x0003, 
  QMI_ERR_ABORTED_V01 = 0x0004, 
  QMI_ERR_CLIENT_IDS_EXHAUSTED_V01 = 0x0005, 
  QMI_ERR_UNABORTABLE_TRANSACTION_V01 = 0x0006, 
  QMI_ERR_INVALID_CLIENT_ID_V01 = 0x0007, 
  QMI_ERR_NO_THRESHOLDS_V01 = 0x0008, 
  QMI_ERR_INVALID_HANDLE_V01 = 0x0009, 
  QMI_ERR_INVALID_PROFILE_V01 = 0x000A, 
  QMI_ERR_INVALID_PINID_V01 = 0x000B, 
  QMI_ERR_INCORRECT_PIN_V01 = 0x000C, 
  QMI_ERR_NO_NETWORK_FOUND_V01 = 0x000D, 
  QMI_ERR_CALL_FAILED_V01 = 0x000E, 
  QMI_ERR_OUT_OF_CALL_V01 = 0x000F, 
  QMI_ERR_NOT_PROVISIONED_V01 = 0x0010, 
  QMI_ERR_MISSING_ARG_V01 = 0x0011, 
  QMI_ERR_ARG_TOO_LONG_V01 = 0x0013, 
  QMI_ERR_INVALID_TX_ID_V01 = 0x0016, 
  QMI_ERR_DEVICE_IN_USE_V01 = 0x0017, 
  QMI_ERR_OP_NETWORK_UNSUPPORTED_V01 = 0x0018, 
  QMI_ERR_OP_DEVICE_UNSUPPORTED_V01 = 0x0019, 
  QMI_ERR_NO_EFFECT_V01 = 0x001A, 
  QMI_ERR_NO_FREE_PROFILE_V01 = 0x001B, 
  QMI_ERR_INVALID_PDP_TYPE_V01 = 0x001C, 
  QMI_ERR_INVALID_TECH_PREF_V01 = 0x001D, 
  QMI_ERR_INVALID_PROFILE_TYPE_V01 = 0x001E, 
  QMI_ERR_INVALID_SERVICE_TYPE_V01 = 0x001F, 
  QMI_ERR_INVALID_REGISTER_ACTION_V01 = 0x0020, 
  QMI_ERR_INVALID_PS_ATTACH_ACTION_V01 = 0x0021, 
  QMI_ERR_AUTHENTICATION_FAILED_V01 = 0x0022, 
  QMI_ERR_PIN_BLOCKED_V01 = 0x0023, 
  QMI_ERR_PIN_PERM_BLOCKED_V01 = 0x0024, 
  QMI_ERR_SIM_NOT_INITIALIZED_V01 = 0x0025, 
  QMI_ERR_MAX_QOS_REQUESTS_IN_USE_V01 = 0x0026, 
  QMI_ERR_INCORRECT_FLOW_FILTER_V01 = 0x0027, 
  QMI_ERR_NETWORK_QOS_UNAWARE_V01 = 0x0028, 
  QMI_ERR_INVALID_ID_V01 = 0x0029, 
  QMI_ERR_INVALID_QOS_ID_V01 = 0x0029, 
  QMI_ERR_REQUESTED_NUM_UNSUPPORTED_V01 = 0x002A, 
  QMI_ERR_INTERFACE_NOT_FOUND_V01 = 0x002B, 
  QMI_ERR_FLOW_SUSPENDED_V01 = 0x002C, 
  QMI_ERR_INVALID_DATA_FORMAT_V01 = 0x002D, 
  QMI_ERR_GENERAL_V01 = 0x002E, 
  QMI_ERR_UNKNOWN_V01 = 0x002F, 
  QMI_ERR_INVALID_ARG_V01 = 0x0030, 
  QMI_ERR_INVALID_INDEX_V01 = 0x0031, 
  QMI_ERR_NO_ENTRY_V01 = 0x0032, 
  QMI_ERR_DEVICE_STORAGE_FULL_V01 = 0x0033, 
  QMI_ERR_DEVICE_NOT_READY_V01 = 0x0034, 
  QMI_ERR_NETWORK_NOT_READY_V01 = 0x0035, 
  QMI_ERR_CAUSE_CODE_V01 = 0x0036, 
  QMI_ERR_MESSAGE_NOT_SENT_V01 = 0x0037, 
  QMI_ERR_MESSAGE_DELIVERY_FAILURE_V01 = 0x0038, 
  QMI_ERR_INVALID_MESSAGE_ID_V01 = 0x0039, 
  QMI_ERR_ENCODING_V01 = 0x003A, 
  QMI_ERR_AUTHENTICATION_LOCK_V01 = 0x003B, 
  QMI_ERR_INVALID_TRANSITION_V01 = 0x003C, 
  QMI_ERR_NOT_A_MCAST_IFACE_V01 = 0x003D, 
  QMI_ERR_MAX_MCAST_REQUESTS_IN_USE_V01 = 0x003E, 
  QMI_ERR_INVALID_MCAST_HANDLE_V01 = 0x003F, 
  QMI_ERR_INVALID_IP_FAMILY_PREF_V01 = 0x0040, 
  QMI_ERR_SESSION_INACTIVE_V01 = 0x0041, 
  QMI_ERR_SESSION_INVALID_V01 = 0x0042, 
  QMI_ERR_SESSION_OWNERSHIP_V01 = 0x0043, 
  QMI_ERR_INSUFFICIENT_RESOURCES_V01 = 0x0044, 
  QMI_ERR_DISABLED_V01 = 0x0045, 
  QMI_ERR_INVALID_OPERATION_V01 = 0x0046, 
  QMI_ERR_INVALID_QMI_CMD_V01 = 0x0047, 
  QMI_ERR_TPDU_TYPE_V01 = 0x0048, 
  QMI_ERR_SMSC_ADDR_V01 = 0x0049, 
  QMI_ERR_INFO_UNAVAILABLE_V01 = 0x004A, 
  QMI_ERR_SEGMENT_TOO_LONG_V01 = 0x004B, 
  QMI_ERR_SEGMENT_ORDER_V01 = 0x004C, 
  QMI_ERR_BUNDLING_NOT_SUPPORTED_V01 = 0x004D, 
  QMI_ERR_OP_PARTIAL_FAILURE_V01 = 0x004E, 
  QMI_ERR_POLICY_MISMATCH_V01 = 0x004F, 
  QMI_ERR_SIM_FILE_NOT_FOUND_V01 = 0x0050, 
  QMI_ERR_EXTENDED_INTERNAL_V01 = 0x0051, 
  QMI_ERR_ACCESS_DENIED_V01 = 0x0052, 
  QMI_ERR_HARDWARE_RESTRICTED_V01 = 0x0053, 
  QMI_ERR_ACK_NOT_SENT_V01 = 0x0054, 
  QMI_ERR_INJECT_TIMEOUT_V01 = 0x0055, 
  QMI_ERR_INCOMPATIBLE_STATE_V01 = 0x005A, 
  QMI_ERR_FDN_RESTRICT_V01 = 0x005B, 
  QMI_ERR_SUPS_FAILURE_CAUSE_V01 = 0x005C, 
  QMI_ERR_NO_RADIO_V01 = 0x005D, 
  QMI_ERR_NOT_SUPPORTED_V01 = 0x005E, 
  QMI_ERR_NO_SUBSCRIPTION_V01 = 0x005F, 
  QMI_ERR_CARD_CALL_CONTROL_FAILED_V01 = 0x0060, 
  QMI_ERR_NETWORK_ABORTED_V01 = 0x0061, 
  QMI_ERR_MSG_BLOCKED_V01 = 0x0062, 
  QMI_ERR_INVALID_SESSION_TYPE_V01 = 0x0064, 
  QMI_ERR_INVALID_PB_TYPE_V01 = 0x0065, 
  QMI_ERR_NO_SIM_V01 = 0x0066, 
  QMI_ERR_PB_NOT_READY_V01 = 0x0067, 
  QMI_ERR_PIN_RESTRICTION_V01 = 0x0068, 
  QMI_ERR_PIN2_RESTRICTION_V01 = 0x0069, 
  QMI_ERR_PUK_RESTRICTION_V01 = 0x006A, 
  QMI_ERR_PUK2_RESTRICTION_V01 = 0x006B, 
  QMI_ERR_PB_ACCESS_RESTRICTED_V01 = 0x006C, 
  QMI_ERR_PB_DELETE_IN_PROG_V01 = 0x006D, 
  QMI_ERR_PB_TEXT_TOO_LONG_V01 = 0x006E, 
  QMI_ERR_PB_NUMBER_TOO_LONG_V01 = 0x006F, 
  QMI_ERR_PB_HIDDEN_KEY_RESTRICTION_V01 = 0x0070, 
  QMI_ERR_PB_NOT_AVAILABLE_V01 = 0x0071, 
  QMI_ERR_DEVICE_MEMORY_ERROR_V01 = 0x0072, 
  QMI_ERR_NO_PERMISSION_V01 = 0x0073, 
  QMI_ERR_TOO_SOON_V01 = 0x0074, 
  QMI_ERR_TIME_NOT_ACQUIRED_V01 = 0x0075, 
  QMI_ERR_OP_IN_PROGRESS_V01 = 0x0076, 
  QMI_ERR_EPERM_V01 = 0x101, /**<  Operation not permitted  */
  QMI_ERR_ENOENT_V01 = 0x102, /**<  No such file or directory  */
  QMI_ERR_ESRCH_V01 = 0x103, /**<  No such process  */
  QMI_ERR_EINTR_V01 = 0x104, /**<  Interrupted system call  */
  QMI_ERR_EIO_V01 = 0x105, /**<  I/O error  */
  QMI_ERR_ENXIO_V01 = 0x106, /**<  No such device or address  */
  QMI_ERR_E2BIG_V01 = 0x107, /**<  Argument list too long  */
  QMI_ERR_ENOEXEC_V01 = 0x108, /**<  Exec format error  */
  QMI_ERR_EBADF_V01 = 0x109, /**<  Bad file number  */
  QMI_ERR_ECHILD_V01 = 0x10A, /**<  No child processes  */
  QMI_ERR_EAGAIN_V01 = 0x10B, /**<  Try again  */
  QMI_ERR_ENOMEM_V01 = 0x10C, /**<  Out of memory  */
  QMI_ERR_EACCES_V01 = 0x10D, /**<  Permission denied  */
  QMI_ERR_EFAULT_V01 = 0x10E, /**<  Bad address  */
  QMI_ERR_ENOTBLK_V01 = 0x10F, /**<  Block device required  */
  QMI_ERR_EBUSY_V01 = 0x110, /**<  Device or resource busy  */
  QMI_ERR_EEXIST_V01 = 0x111, /**<  File exists  */
  QMI_ERR_EXDEV_V01 = 0x112, /**<  Cross-device link  */
  QMI_ERR_ENODEV_V01 = 0x113, /**<  No such device  */
  QMI_ERR_ENOTDIR_V01 = 0x114, /**<  Not a directory  */
  QMI_ERR_EISDIR_V01 = 0x115, /**<  Is a directory  */
  QMI_ERR_EINVAL_V01 = 0x116, /**<  Invalid argument  */
  QMI_ERR_ENFILE_V01 = 0x117, /**<  File table overflow  */
  QMI_ERR_EMFILE_V01 = 0x118, /**<  Too many open files  */
  QMI_ERR_ENOTTY_V01 = 0x119, /**<  Not a typewriter  */
  QMI_ERR_ETXTBSY_V01 = 0x11A, /**<  Text file busy  */
  QMI_ERR_EFBIG_V01 = 0x11B, /**<  File too large  */
  QMI_ERR_ENOSPC_V01 = 0x11C, /**<  No space left on device  */
  QMI_ERR_ESPIPE_V01 = 0x11D, /**<  Illegal seek  */
  QMI_ERR_EROFS_V01 = 0x11E, /**<  Read-only file system  */
  QMI_ERR_EMLINK_V01 = 0x11F, /**<  Too many links  */
  QMI_ERR_EPIPE_V01 = 0x120, /**<  Broken pipe  */
  QMI_ERR_EDOM_V01 = 0x121, /**<  Math argument out of domain of func  */
  QMI_ERR_ERANGE_V01 = 0x122, /**<  Math result not representable  */
  QMI_ERR_EDEADLK_V01 = 0x123, /**<  Resource deadlock would occur  */
  QMI_ERR_ENAMETOOLONG_V01 = 0x124, /**<  File name too long  */
  QMI_ERR_ENOLCK_V01 = 0x125, /**<  No record locks available  */
  QMI_ERR_ENOSYS_V01 = 0x126, /**<  Function not implemented  */
  QMI_ERR_ENOTEMPTY_V01 = 0x127, /**<  Directory not empty  */
  QMI_ERR_ELOOP_V01 = 0x128, /**<  Too many symbolic links encountered  */
  QMI_ERR_EWOULDBLOCK_V01 = 0x10B, /**<  Operation would block  */
  QMI_ERR_ENOMSG_V01 = 0x12A, /**<  No message of desired type  */
  QMI_ERR_EIDRM_V01 = 0x12B, /**<  Identifier removed  */
  QMI_ERR_ECHRNG_V01 = 0x12C, /**<  Channel number out of range  */
  QMI_ERR_EL2NSYNC_V01 = 0x12D, /**<  Level 2 not synchronized  */
  QMI_ERR_EL3HLT_V01 = 0x12E, /**<  Level 3 halted  */
  QMI_ERR_EL3RST_V01 = 0x12F, /**<  Level 3 reset  */
  QMI_ERR_ELNRNG_V01 = 0x130, /**<  Link number out of range  */
  QMI_ERR_EUNATCH_V01 = 0x131, /**<  Protocol driver not attached  */
  QMI_ERR_ENOCSI_V01 = 0x132, /**<  No CSI structure available  */
  QMI_ERR_EL2HLT_V01 = 0x133, /**<  Level 2 halted  */
  QMI_ERR_EBADE_V01 = 0x134, /**<  Invalid exchange  */
  QMI_ERR_EBADR_V01 = 0x135, /**<  Invalid request descriptor  */
  QMI_ERR_EXFULL_V01 = 0x136, /**<  Exchange full  */
  QMI_ERR_ENOANO_V01 = 0x137, /**<  No anode  */
  QMI_ERR_EBADRQC_V01 = 0x138, /**<  Invalid request code  */
  QMI_ERR_EBADSLT_V01 = 0x139, /**<  Invalid slot  */
  QMI_ERR_EDEADLOCK_V01 = 0x123, /**<  Resource deadlock would occur  */
  QMI_ERR_EBFONT_V01 = 0x13B, /**<  Bad font file format  */
  QMI_ERR_ENOSTR_V01 = 0x13C, /**<  Device not a stream  */
  QMI_ERR_ENODATA_V01 = 0x13D, /**<  No data available  */
  QMI_ERR_ETIME_V01 = 0x13E, /**<  Timer expired  */
  QMI_ERR_ENOSR_V01 = 0x13F, /**<  Out of streams resources  */
  QMI_ERR_ENONET_V01 = 0x140, /**<  Machine is not on the network  */
  QMI_ERR_ENOPKG_V01 = 0x141, /**<  Package not installed  */
  QMI_ERR_EREMOTE_V01 = 0x142, /**<  Object is remote  */
  QMI_ERR_ENOLINK_V01 = 0x143, /**<  Link has been severed  */
  QMI_ERR_EADV_V01 = 0x144, /**<  Advertise error  */
  QMI_ERR_ESRMNT_V01 = 0x145, /**<  Srmount error  */
  QMI_ERR_ECOMM_V01 = 0x146, /**<  Communication error on send  */
  QMI_ERR_EPROTO_V01 = 0x147, /**<  Protocol error  */
  QMI_ERR_EMULTIHOP_V01 = 0x148, /**<  Multihop attempted  */
  QMI_ERR_EDOTDOT_V01 = 0x149, /**<  RFS specific error  */
  QMI_ERR_EBADMSG_V01 = 0x14A, /**<  Not a data message  */
  QMI_ERR_EOVERFLOW_V01 = 0x14B, /**<  Value too large for defined data type  */
  QMI_ERR_ENOTUNIQ_V01 = 0x14C, /**<  Name not unique on network  */
  QMI_ERR_EBADFD_V01 = 0x14D, /**<  File descriptor in bad state  */
  QMI_ERR_EREMCHG_V01 = 0x14E, /**<  Remote address changed  */
  QMI_ERR_ELIBACC_V01 = 0x14F, /**<  Can not access a needed shared library  */
  QMI_ERR_ELIBBAD_V01 = 0x150, /**<  Accessing a corrupted shared library  */
  QMI_ERR_ELIBSCN_V01 = 0x151, /**<  .lib section in a.out corrupted  */
  QMI_ERR_ELIBMAX_V01 = 0x152, /**<  Attempting to link in too many shared libraries  */
  QMI_ERR_ELIBEXEC_V01 = 0x153, /**<  Cannot exec a shared library directly  */
  QMI_ERR_EILSEQ_V01 = 0x154, /**<  Illegal byte sequence  */
  QMI_ERR_ERESTART_V01 = 0x155, /**<  Interrupted system call should be restarted  */
  QMI_ERR_ESTRPIPE_V01 = 0x156, /**<  Streams pipe error  */
  QMI_ERR_EUSERS_V01 = 0x157, /**<  Too many users  */
  QMI_ERR_ENOTSOCK_V01 = 0x158, /**<  Socket operation on non-socket  */
  QMI_ERR_EDESTADDRREQ_V01 = 0x159, /**<  Destination address required  */
  QMI_ERR_EMSGSIZE_V01 = 0x15A, /**<  Message too long  */
  QMI_ERR_EPROTOTYPE_V01 = 0x15B, /**<  Protocol wrong type for socket  */
  QMI_ERR_ENOPROTOOPT_V01 = 0x15C, /**<  Protocol not available  */
  QMI_ERR_EPROTONOSUPPORT_V01 = 0x15D, /**<  Protocol not supported  */
  QMI_ERR_ESOCKTNOSUPPORT_V01 = 0x15E, /**<  Socket type not supported  */
  QMI_ERR_EOPNOTSUPP_V01 = 0x15F, /**<  Operation not supported on transport endpoint  */
  QMI_ERR_EPFNOSUPPORT_V01 = 0x160, /**<  Protocol family not supported  */
  QMI_ERR_EAFNOSUPPORT_V01 = 0x161, /**<  Address family not supported by protocol  */
  QMI_ERR_EADDRINUSE_V01 = 0x162, /**<  Address already in use  */
  QMI_ERR_EADDRNOTAVAIL_V01 = 0x163, /**<  Cannot assign requested address  */
  QMI_ERR_ENETDOWN_V01 = 0x164, /**<  Network is down  */
  QMI_ERR_ENETUNREACH_V01 = 0x165, /**<  Network is unreachable  */
  QMI_ERR_ENETRESET_V01 = 0x166, /**<  Network dropped connection because of reset  */
  QMI_ERR_ECONNABORTED_V01 = 0x167, /**<  Software caused connection abort  */
  QMI_ERR_ECONNRESET_V01 = 0x168, /**<  Connection reset by peer  */
  QMI_ERR_ENOBUFS_V01 = 0x169, /**<  No buffer space available  */
  QMI_ERR_EISCONN_V01 = 0x16A, /**<  Transport endpoint is already connected  */
  QMI_ERR_ENOTCONN_V01 = 0x16B, /**<  Transport endpoint is not connected  */
  QMI_ERR_ESHUTDOWN_V01 = 0x16C, /**<  Cannot send after transport endpoint shutdown  */
  QMI_ERR_ETOOMANYREFS_V01 = 0x16D, /**<  Too many references: cannot splice  */
  QMI_ERR_ETIMEDOUT_V01 = 0x16E, /**<  Connection timed out  */
  QMI_ERR_ECONNREFUSED_V01 = 0x16F, /**<  Connection refused  */
  QMI_ERR_EHOSTDOWN_V01 = 0x170, /**<  Host is down  */
  QMI_ERR_EHOSTUNREACH_V01 = 0x171, /**<  No route to host  */
  QMI_ERR_EALREADY_V01 = 0x172, /**<  Operation already in progress  */
  QMI_ERR_EINPROGRESS_V01 = 0x173, /**<  Operation now in progress  */
  QMI_ERR_ESTALE_V01 = 0x174, /**<  Stale NFS file handle  */
  QMI_ERR_EUCLEAN_V01 = 0x175, /**<  Structure needs cleaning  */
  QMI_ERR_ENOTNAM_V01 = 0x176, /**<  Not a XENIX named type file  */
  QMI_ERR_ENAVAIL_V01 = 0x177, /**<  No XENIX semaphores available  */
  QMI_ERR_EISNAM_V01 = 0x178, /**<  Is a named type file  */
  QMI_ERR_EREMOTEIO_V01 = 0x179, /**<  Remote I/O error  */
  QMI_ERR_EDQUOT_V01 = 0x17A, /**<  Quota exceeded  */
  QMI_ERR_ENOMEDIUM_V01 = 0x17B, /**<  No medium found  */
  QMI_ERR_EMEDIUMTYPE_V01 = 0x17C, /**<  Wrong medium type  */
  QMI_ERR_ECANCELED_V01 = 0x17D, /**<  Operation Canceled  */
  QMI_ERR_ENOKEY_V01 = 0x17E, /**<  Required key not available  */
  QMI_ERR_EKEYEXPIRED_V01 = 0x17F, /**<  Key has expired  */
  QMI_ERR_EKEYREVOKED_V01 = 0x180, /**<  Key has been revoked  */
  QMI_ERR_EKEYREJECTED_V01 = 0x181, /**<  Key was rejected by service  */
  QMI_ERR_EOWNERDEAD_V01 = 0x182, /**<  Owner died  */
  QMI_ERR_ENOTRECOVERABLE_V01 = 0x183, /**<  State not recoverable  */
  QMI_ERROR_TYPE_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmi_error_type_v01;
/**
    @}
  */

/** @addtogroup common_qmi_aggregates
    @{
  */
typedef struct {

  qmi_result_type_v01 result;
  /**<   Result code
                            - QMI_RESULT_SUCCESS
                            - QMI_RESULT_FAILURE
                          */

  qmi_error_type_v01 error;
  /**<   Error code. Possible error code values are 
                               described in the error codes section of each 
                               message definition.
                          */
}qmi_response_type_v01;  /* Type */
/**
    @}
  */

/** @addtogroup common_qmi_messages
    @{
  */
/** Request Message; Queries the set of messages implemented by the currently 
               running software.  */
typedef struct {
  /* This element is a placeholder to prevent the declaration of
     an empty struct.  DO NOT USE THIS FIELD UNDER ANY CIRCUMSTANCE */
  char __placeholder;
}qmi_get_supported_msgs_req_v01;

  /* Message */
/**
    @}
  */

/** @addtogroup common_qmi_messages
    @{
  */
/** Response Message; Queries the set of messages implemented by the currently 
               running software.  */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
   */

  /* Optional */
  /*  List of Supported Messages */
  uint8_t supported_msgs_valid;  /**< Must be set to true if supported_msgs is being passed */
  uint32_t supported_msgs_len;  /**< Must be set to # of elements in supported_msgs */
  uint8_t supported_msgs[QMI_GET_SUPPORTED_MESSAGES_ARRAY_MAX_V01];
  /**<   This array of uint8 is a bitmask where each bit represents a message ID, 
       i.e., starting with the LSB, bit 0 represents message ID 0, bit 1 
       represents message ID 1, etc. 
       
       \vspace{3pt}
       The bit is set to 1 if the message is supported; otherwise, it is set
       to zero. 
       
       \vspace{3pt}
       For example, if a service supports exactly four messages with 
       IDs 0, 1, 30, and 31 (decimal), the array (in hexadecimal) is 
       4 bytes [03 00 00 c0].
  */
}qmi_get_supported_msgs_resp_v01;  /* Message */
/**
    @}
  */

/** @addtogroup common_qmi_messages
    @{
  */
/** Request Message; Queries the fields supported for a single command as implemented 
               by the currently running software. */
typedef struct {

  /* Mandatory */
  /*  Service Message ID */
  uint16_t msg_id;
  /**<   ID of the command for which the supported fields are requested. */
}qmi_get_supported_fields_req_v01;  /* Message */
/**
    @}
  */

/** @addtogroup common_qmi_messages
    @{
  */
/** Response Message; Queries the fields supported for a single command as implemented 
               by the currently running software. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members:
     - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
     - qmi_error_type  -- Error code. Possible error code values are described 
                          in the error codes section of each message definition.
   */

  /* Optional */
  /*  List of Supported Request Fields */
  uint8_t request_fields_valid;  /**< Must be set to true if request_fields is being passed */
  uint32_t request_fields_len;  /**< Must be set to # of elements in request_fields */
  uint8_t request_fields[QMI_GET_SUPPORTED_FIELDS_ARRAY_MAX_V01];
  /**<   This field describes which optional field IDs are supported in the QMI 
       request. The array of uint8 is a bitmask where each bit represents a 
       field (TLV) ID. Because fields 0 to 15 (decimal) are mandatory by 
       definition, the first bit represents field ID 16. Starting with the 
       LSB, bit 0 represents field ID 16, bit 1 represents field ID 17, etc. 
       
       \vspace{3pt}
       The bit is set to 1 if the field ID is supported; otherwise, it is set 
       to zero. 
       
       \vspace{3pt}
       For example, if a service supports exactly four fields with 
       IDs 16, 17, 30, and 31 (decimal), the array (in hexadecimal) is 
       2 bytes [03 c0].
  */

  /* Optional */
  /*  List of Supported Response Fields */
  uint8_t response_fields_valid;  /**< Must be set to true if response_fields is being passed */
  uint32_t response_fields_len;  /**< Must be set to # of elements in response_fields */
  uint8_t response_fields[QMI_GET_SUPPORTED_FIELDS_ARRAY_MAX_V01];
  /**<   This field describes which optional field IDs are supported in the QMI 
       response. Its format is the same as request_fields.
  */

  /* Optional */
  /*  List of Supported Indication Fields */
  uint8_t indication_fields_valid;  /**< Must be set to true if indication_fields is being passed */
  uint32_t indication_fields_len;  /**< Must be set to # of elements in indication_fields */
  uint8_t indication_fields[QMI_GET_SUPPORTED_FIELDS_ARRAY_MAX_V01];
  /**<   This field describes which optional field IDs are supported in the QMI 
       indication. Its format is the same as request_fields.
  */
}qmi_get_supported_fields_resp_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 

/*Extern Definition of Type Table Object*/
/*THIS IS AN INTERNAL OBJECT AND SHOULD ONLY*/
/*BE ACCESSED BY AUTOGENERATED FILES*/
extern const qmi_idl_type_table_object common_qmi_idl_type_table_object_v01;


#ifdef __cplusplus
}
#endif
#endif

