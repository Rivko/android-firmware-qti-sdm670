/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

#ifndef __TEETEST_PUBLIC_H
#define __TEETEST_PUBLIC_H
#include <stddef.h>
#include <stdint.h>

/******************************************************************************
 SMC ID MACRO Definitions
******************************************************************************/

/** Trusted OS Call IDs */
#define TZ_OWNER_TEE_HLOS                     51
#define TZ_OWNER_TEE                          52

/** Trusted OS: call groups */
#define TEE_SVC_EXECUTIVE_EXT                 250

#define TEE_SYSCALL_CREATE_SMC_ID(o, s, f) \
  ((uint32_t)((((o & 0x3f) << 24 ) | (s & 0xff) << 8) | (f & 0xff)))

/** Helper macro for checking if an owning entity is of type Trusted OS */
#define IS_OWNER_TRUSTED_OS(owner_id) \
  (((owner_id >= 50) && (owner_id <= ((63))))?TRUE:FALSE)

/** Helper macro to extract the Owning entity from SMC ID. */
#define TEE_SYSCALL_OWNER_ID(r0)   ((r0 & TEE_MASK_BITS(29,24)) >> 24)

/******************************************************************************
 SMC ID Definitions
******************************************************************************/
#define TEE_IPC_INBOUND_SMC_ID \
  TEE_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TEE, TEE_SVC_EXECUTIVE_EXT, 0x01)

#define TEE_IPC_RETURN_FROM_INBOUND_SMC_ID \
  TEE_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TEE, TEE_SVC_EXECUTIVE_EXT, 0x03)

#define TEE_IPC_OUTBOUND_SMC_ID \
  TEE_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TEE, TEE_SVC_EXECUTIVE_EXT, 0x04)


/******************************************************************************
 SMC Param ID Definitions
******************************************************************************/
/** A parameter of type value */
#define TEE_PARAM_TYPE_VAL              0x0
/** A parameter of type buffer read-only */
#define TEE_PARAM_TYPE_BUF_RO           0x1
/** A parameter of type buffer read-write */
#define TEE_PARAM_TYPE_BUF_RW           0x2

#define TEE_MASK_BITS(h,l)     ((0xffffffff >> (32 - ((h - l) + 1))) << l)
#define TEE_PARAM_NARGS_MASK  TEE_MASK_BITS(3,0)
#define TEE_PARAM_TYPE_MASK   TEE_MASK_BITS(1,0)

#define TEE_CREATE_PARAM_ID_0 0
#define TEE_CREATE_PARAM_ID_1(p1) \
  _TEE_CREATE_PARAM_ID(1, p1, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_2(p1, p2) \
  _TEE_CREATE_PARAM_ID(2, p1, p2, 0, 0, 0, 0, 0, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_3(p1, p2, p3) \
  _TEE_CREATE_PARAM_ID(3, p1, p2, p3, 0, 0, 0, 0, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_4(p1, p2, p3, p4) \
  _TEE_CREATE_PARAM_ID(4, p1, p2, p3, p4, 0, 0, 0, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_5(p1, p2, p3, p4, p5) \
  _TEE_CREATE_PARAM_ID(5, p1, p2, p3, p4, p5, 0, 0, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_6(p1, p2, p3, p4, p5, p6) \
  _TEE_CREATE_PARAM_ID(6, p1, p2, p3, p4, p5, p6, 0, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_7(p1, p2, p3, p4, p5, p6, p7) \
  _TEE_CREATE_PARAM_ID(7, p1, p2, p3, p4, p5, p6, p7, 0, 0, 0)
#define TEE_CREATE_PARAM_ID_8(p1, p2, p3, p4, p5, p6, p7, p8) \
  _TEE_CREATE_PARAM_ID(8, p1, p2, p3, p4, p5, p6, p7, p8, 0, 0)
#define TEE_CREATE_PARAM_ID_9(p1, p2, p3, p4, p5, p6, p7, p8, p9) \
  _TEE_CREATE_PARAM_ID(9, p1, p2, p3, p4, p5, p6, p7, p8, p9, 0)
#define TEE_CREATE_PARAM_ID_10(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
  _TEE_CREATE_PARAM_ID(10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

#define _TEE_CREATE_PARAM_ID(nargs, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
  ((nargs&TEE_PARAM_NARGS_MASK)+ \
  ((p1&TEE_PARAM_TYPE_MASK)<<4)+ \
  ((p2&TEE_PARAM_TYPE_MASK)<<6)+ \
  ((p3&TEE_PARAM_TYPE_MASK)<<8)+ \
  ((p4&TEE_PARAM_TYPE_MASK)<<10)+ \
  ((p5&TEE_PARAM_TYPE_MASK)<<12)+ \
  ((p6&TEE_PARAM_TYPE_MASK)<<14)+ \
  ((p7&TEE_PARAM_TYPE_MASK)<<16)+ \
  ((p8&TEE_PARAM_TYPE_MASK)<<18)+ \
  ((p9&TEE_PARAM_TYPE_MASK)<<20)+ \
  ((p10&TEE_PARAM_TYPE_MASK)<<22))

#define TEE_IPC_OUTBOUND_SMC_PARAM_ID \
  TEE_CREATE_PARAM_ID_4( \
  TEE_PARAM_TYPE_VAL, TEE_PARAM_TYPE_VAL, \
  TEE_PARAM_TYPE_VAL, TEE_PARAM_TYPE_VAL )

/** Helper macro to extract the number of arguments from Param ID */
#define TEE_SYSCALL_NUM_ARGS(r1)   (r1 & TEE_MASK_BITS(3,0))

#define TEE_SYSCALL_MAX_ARG                        10


/******************************************************************************
Structure of TEE boot information
******************************************************************************/
typedef struct tee_memblock_s
{
  uint64_t start;
  uint64_t end;
  uint64_t attributes;
} tee_memblock_t, *tee_memblock_ptr;

typedef struct tee_boot_info_s
{
  uint64_t         size;
  tee_memblock_t ddr_mem;
  tee_memblock_t ipc_mem;
} tee_boot_info_t, *tee_boot_info_ptr;

typedef uint32_t (*tee_syscall_func_t) ();

typedef struct tee_syscall_entry_s
{
  uint32_t mink_service_id;       ///< identify which module to route the syscall to
  uint32_t smc_id;                ///< owner id and command id of the function to be called
  uint32_t param_id;              ///< # of args and their types defined by function definition
  uint32_t flags;                 ///< flags for special syscall handling
  tee_syscall_func_t func;        ///< Syscall function

} tee_syscall_entry_;

/**
  SMC Interface Error Codes Enumaration
*/
typedef enum
{
  SMC_INTERRUPTED                   =  1,   /** SMC call was interrupted                */
  SMC_SUCCESS                       =  0,   /** Success, requested syscall was called   */
  SMC_ERR_UNKNOWN_SMC_ID            = -1L,  /** ARM-defined error of 0xFFFFFFFF         */
  SMC_ERR_SYSCALL_FAILED            = -2L,  /** Requested syscall returned failure      */
  SMC_ERR_SYSCALL_NOT_AVAILABLE     = -3L,  /** No function associated with syscall def */
  SMC_ERR_RESERVED_FIELD_MBZ        = -5L,  /** SMC ID err: reserved field must be zero */
  SMC_ERR_NUM_ARGS_MISMATCH         = -6L,  /** Num args does not match func definition */
  SMC_ERR_INDIRECT_PARAM_PTR_NOT_NS = -7L,  /** Ind. param ptr doesnt point to ns mem   */
  SMC_ERR_BUF_LEN_NOT_FOUND         = -8L,  /** No buffer len following buffer ptr      */
  SMC_ERR_BUF_PTR_NOT_NS            = -9L,  /** Buffer ptr does not point to ns mem     */
  SMC_ERR_NO_MEMORY                 = -10L, /** Heap is out of space                    */
  SMC_ERR_PARAM_TYPE_MISMATCH       = -11L, /** Parameter type does not match func definition */
  SMC_ERR_PARAM_ID_MISMATCH         = -12L, /* Incorrect Param ID from NS world        */
  SMC_ERR_YOU_BROKE_IT              = -13L, /** How could you?  (unused) */
  SMC_ERR_BUSY                      = -14L, /** TZ is busy waiting for listener rsp     */
  SMC_ERR_NOT_FOUND_IN_HYP          = -15L, /** The SMC call is not served in HYP       */
  SMC_ERR_TEE_SERVICE_NOT_FOUND     = -16L, /** The SMC call is from a TEE but format is bad */


  /* ARMv8 SMC spec: Must return 0xFFFFFFFF for these errors */
  SMC_ERR_SYSCALL_NOT_SUPPORTED     = SMC_ERR_UNKNOWN_SMC_ID,
  SMC_ERR_AARCH64_NOT_SUPPORTED     = SMC_ERR_UNKNOWN_SMC_ID,

  SMC_ERR_FAILURE                   = -100L /** Internal failure                */
} SmcErrnoType;

/*******************************************************************************
Structure of a TEE_IPC_RETURN_FROM_INBOUND_ID
---------------------------------------------
Register   Description

x0/r0      SMC ID
x1/r1      Error code

Structure of a TEE_IPC_RETURN_FROM_OUTBOUND_ID
----------------------------------------------
Register   Description

x0/r0      SMC ID
x1/r1      Error code

Error code as defined in table below (from _object.h).

// Generic error codes
#define Object_OK                  0   // non-specific success code
#define Object_ERROR               1   // non-specific error
#define Object_ERROR_INVALID       2   // unsupported/unrecognized request
#define Object_ERROR_USERBASE     10   // start of user-defined error range

// Transport layer error codes
#define Object_ERROR_DEFUNCT     -90   // object no longer exists
#define Object_ERROR_ABORT       -91   // calling thread must exit
#define Object_ERROR_BADOBJ      -92   // invalid object context
#define Object_ERROR_NOSLOTS     -93   // caller's object table full
#define Object_ERROR_MAXARGS     -94   // too many args
#define Object_ERROR_MAXDATA     -95   // buffers too large
#define Object_ERROR_UNAVAIL     -96   // the request could not be processed
#define Object_ERROR_KMEM        -97   // kernel out of memory
#define Object_ERROR_REMOTE      -98   // local method sent to remote object

Structure of TEE_IPC_OUTBOUND_SMC_ID
------------------------------------
Register   Description

x0/r0      SMC ID
x1/r1      SMC PARAMS ID
x2/r2      Object ID
x3/r3      Object method
x4/r4      Packed object count (k)
x5/r5      Pointer to object arg

Structure of TEE_IPC_INBOUND_SMC_ID
--------------------------------------------------------------
Register   Description

x0/r0      SMC ID
x1/r1      Object ID
x2/r2      Object method
x3/r3      Packed object count (k)
x4/r4      Pointer to object arg

arg[0]  object[0].ptr
arg[0]  object[0].len
arg[1]  object[0].ptr
arg[1]  object[0].len
arg[2]  object[0].ptr
arg[2]  object[0].len

etc according to value of k

******************************************************************************/
#endif
