#ifndef TZBSP_SYSCALL_H
#define TZBSP_SYSCALL_H

/**
@file tzbsp_syscall.h
@brief Provide internal system calls functions for the SYSCALL API
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsee/include/tzbsp_syscall.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/20/10   cap      Initial version.

===========================================================================*/
#include <stdbool.h>
#include "tz_syscall_pub.h"
#include "tzbsp_syscall_priv.h"

struct tzbsp_regs_s;

/*===========================================================================

                              DEFINES

===========================================================================*/

/** Maxmimum number of system call arguments. */
#define TZBSP_SYSCALL_MAX_ARG                     10

#define TZBSP_SYSCALL_INDIRECT_PARAMS_MAX         7

#define TZBSP_SYSCALL_PARAM_TYPE_VAL              0x0
#define TZBSP_SYSCALL_PARAM_TYPE_BUF_RO           0x1
#define TZBSP_SYSCALL_PARAM_TYPE_BUF_RW           0x2
#define TZBSP_SYSCALL_PARAM_TYPE_BUF_VALIDATION   0x3

#define TZBSP_SYSCALL_AARCH32                     0x0
#define TZBSP_SYSCALL_AARCH64                     0x1

#define TZBSP_SYSCALL_IRQS_ENABLED                0x0
#define TZBSP_SYSCALL_IRQS_DISABLED               0x1

#define IS_INVALID_SEC_RSP_PTR(rsp_ptr) (NULL == rsp_ptr)

/* Syscalls marked with RSP flag will be passed a ptr to
 * this struct.
 *
 * A syscall cannot send more than three 4-byte values back to HLOS via registers.
 * A syscall cannot return with more than 6 4-byte values back to TEE via registers.
 */
typedef struct tzbsp_smc_rsp_s {
  uintnt rsp[6];              //Values to return to HLOS or TEE
  uintnt return_to_tee;       //Indication to post an smc after the current smc has been handled and return to tee
} tzbsp_smc_rsp_t;

/** Context information for ns and secure*/
typedef struct tzbsp_regs_s
{
   uintnt          reg[12];
   tzbsp_smc_rsp_t *rsp;
} tzbsp_regs_t;


#define TZBSP_SET_RSP(rsp_ptr, rsp0, rsp1, rsp2) \
  do { \
    if (!rsp_ptr) break; \
    ((tzbsp_smc_rsp_t *)rsp_ptr)->rsp[0] = (uintnt) rsp0; \
    ((tzbsp_smc_rsp_t *)rsp_ptr)->rsp[1] = (uintnt) rsp1; \
    ((tzbsp_smc_rsp_t *)rsp_ptr)->rsp[2] = (uintnt) rsp2; \
  } while (0)

/*
 * Helper macros for setting tzbsp_smc_rsp_t*
 */
#define TZBSP_SET_RSP1(rsp_ptr, rsp0) \
  TZBSP_SET_RSP(rsp_ptr, rsp0, 0, 0)
#define TZBSP_SET_RSP2(rsp_ptr, rsp0, rsp1) \
  TZBSP_SET_RSP(rsp_ptr, rsp0, rsp1, 0)
#define TZBSP_SET_RSP3(rsp_ptr, rsp0, rsp1, rsp2) \
  TZBSP_SET_RSP(rsp_ptr, rsp0, rsp1, rsp2)

#define TZBSP_SET_RSPI(rsp_ptr, idx, rspval) \
  do { \
    if (!rsp_ptr || idx>=3 || idx<0) break; \
    rsp_ptr->rsp[idx] = (uint32)rspval; \
  } while (0)

#define TZBSP_DEFINE_SYSCALL(mink_service, smc_id, param_id, flags, func) \
   extern int func ();\
   static tzbsp_syscall_entry_t tz_syscall_entry_ ##func \
   __attribute__((section("_tz_syscall"),used)) = \
  { mink_service, smc_id, param_id, flags, func }

#define TZBSP_DEFINE_WEAK_SYSCALL(mink_service, smc_id, param_id, flags, func) \
   extern int func () __attribute__((weak));\
   static tzbsp_syscall_entry_t tz_syscall_entry_ ##func \
   __attribute__((section("_tz_syscall"),used)) = \
  { mink_service, smc_id, param_id, flags, func}

extern const void * _tz_syscall$$Base;
extern const void * _tz_syscall$$Limit;
extern const void * _tztest_syscall$$Base;
extern const void * _tztest_syscall$$Limit;

#ifndef TZBSP_SYSCALL_FUNC_T
#define TZBSP_SYSCALL_FUNC_T
typedef int (*tzbsp_syscall_func_t) ();
#endif

typedef struct tzbsp_syscall_entry_s
{
  uint32 mink_service_id;       ///< identify which module to route the syscall to
  uint32 smc_id;                ///< owner id and command id of the function to be called
  uint32 param_id;              ///< # of args and their types defined by function definition
  uint32 flags;                 ///< flags for special syscall handling
  tzbsp_syscall_func_t func;    ///< Syscall function

} tzbsp_syscall_entry_t;

/** Syscall definition flags */
#define TZ_SC_FLAG_RSP                0x00000001 /* Syscall populates rsp          */
#define TZ_SC_FLAG_ENABLE_INT         0x00000002 /* Enable IRQs during syscall     */
#define TZ_SC_FLAG_NON_COPY_RETURN    0x00000004 /* Appsbl mode switch flag        */
#define TZ_SC_FLAG_LEGACY_API         0x00000008 /* Legacy API bypasses validation */
#define TZ_SC_FLAG_O_RSP_ON_ERR       0x00000010 /* Overwrite Response on error    */
#define TZ_SC_FLAG_USE_CRYPTO         0x00000020
#define TZ_SC_FLAG_FLUSH_RW_BUF       0x00000040
#define TZ_SC_FLAG_TEE_API            0x00000080 /* TEE API bypasses params id use */
//#define TZ_SC_FLAG_HYP_SMC_CALL     0x00000100 /* Do not use anymore             */
#define TZ_SC_FLAG_MC_CALL            0x00000200 /* Syscall can be handled in idle 
                                                    thread by multiple cores 
													concurrently                   */

/* Make no distinction between Register Class and Control Class
   syscalls */
#define TZ_SC_FLAG_SHARED_SYSCALL     TZ_SC_FLAG_O_RSP_ON_ERR

/** Sycall Client ID */
#define ARM_NS_AVMID_HYP      0    // defined in ARM SMC Spec for W7-client guest  tracking

/**
 * Acquire SMC lock
 *
 * @returns None.
 */
void tzbsp_acquire_smc_lock(void);

/**
 * Release SMC lock
 *
 * @returns None.
 */
void tzbsp_release_smc_lock(void);

/**
 * Parses non-secure world SMC calls 
 *
 * @param [in] regs          The non-secure saved registers
 * @param [out] is_mc_call   Return TRUE if current syscall is of 
 *                           TZ_SC_FLAG_MC_CALL type. FALSE otherwise.
 */
int tzbsp_syscall(tzbsp_regs_t * regs, bool * is_mc_call);

#endif
