#ifndef QURT_ERROR_H
#define QURT_ERROR_H

/**
  @file qurt_error.h 
  @brief Error results- QURT defines a set of standard symbols for the error result values. This file lists the
  symbols and their corresponding values.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc..
 ======================================================================*/

/** @addtogroup chapter_error
@{ */

/*=====================================================================
Constants and macros
======================================================================*/
#define QURT_EOK                             0  /**< Operation was successfully performed. */
#define QURT_EVAL                            1  /**< Wrong values for the parameters. The specified page does not exist. */
#define QURT_EMEM                            2  /**< Not enough memory to perform the operation.*/

#define QURT_EINVALID                        4  /**< Invalid argument value; invalid key. */ 
/** @cond  */
#define QURT_EUNKNOWN                        6  /**< Defined but never used in QuRT. */
#define QURT_ENOMSGS                         7  /**< Message queue is empty. */ 
/** @endcond */
#define QURT_EFAILED                        12  /**< Operation failed. */ 
/** @cond RTOS_user_guide */
#define QURT_ENOTALLOWED                    13  /**< Operation not allowed. */
/** @endcond */
/** @} */ /* end_addtogroup chapter_error */
#define QURT_EDUPCLSID                      14  /**< Duplicate class ID. */
#define QURT_ENOREGISTERED                  20  /**< No registered interrupts.*/ 




/** @cond RTOS_user_guide_supplement */
/** @addtogroup chapter_error
@{ */
#define QURT_EISDB                          21  /**< Power collapse failed due to ISDB being enabled. */
#define QURT_ESTM                           22  /**< Power collapse failed in a Single-threaded mode check. */
/** @} */ /* end_addtogroup chapter_error */
/** @endcond */


/** @addtogroup chapter_error
@{ */
/** @cond RTOS_user_guide */
#define QURT_ETLSAVAIL                      23  /**< No free TLS key is available. */
#define QURT_ETLSENTRY                      24  /**< TLS key is not already free. */ 
/** @endcond */
#define QURT_EINT                           26  /**< Invalid interrupt number (not registered). */  
/** @cond RTOS_user_guide */
#define QURT_ESIG                           27  /**< Invalid signal bitmask (cannot set more than one signal at a time). */
/** @cond */
#define QURT_EHEAP                          28  /**< No heap space is available. */
#define QURT_EMEMMAP                        29  /**< Physical address layout is not supported by the kernel. */
/** @endcond */
#define QURT_ENOTHREAD                      30  /**< Thread no longer exists. */
/** @cond */
#define QURT_EL2CACHE                       31  /**< L2cachable is not supported in kernel invalidate/cleaninv. */
/** @endcond */
#define QURT_EALIGN                         32  /**< Not aligned. */
#define QURT_EDEREGISTERED                  33  /**< Interrupt is already deregistered.*/
/** @endcond */
/** @} */ /* end_addtogroup chapter_error */


/** @cond RTOS_user_guide_supplement */
/** @addtogroup chapter_error
@{ */
#define QURT_ETLBCREATESIZE                 34  /**< TLB create error -- Incorrect size.*/
#define QURT_ETLBCREATEUNALIGNED            35  /**< TLB create error -- Unaligned address.*/
/** @} */ /* end_addtogroup chapter_error */
/** @endcond */

#define QURT_EPRIVILEGE                     36  /**< Caller does not have enough privilege for this operation.*/
/** @cond RTOS_user_guide */
/** @addtogroup chapter_error
@{ */
#define QURT_ECANCEL                        37  /**< A cancellable request was cancelled due to the associated process being asked to exit.*/
/** @} */ /* end_addtogroup chapter_error */
/** @endcond */

/** @cond RTOS_user_guide_supplement */
/** @addtogroup chapter_error
@{ */
#define QURT_EISLANDTRAP                    38  /**< Unsupported TRAP is called in Island mode.*/ 
/** @} */ /* end_addtogroup chapter_error */
/** @endcond */

#define QURT_ERMUTEXUNLOCKNONHOLDER         39  /**< Rmutex unlock by a non-holder.*/
#define QURT_ERMUTEXUNLOCKFATAL             40  /**< Rmutex unlock error, all except the non-holder error.*/
#define QURT_EMUTEXUNLOCKNONHOLDER          41  /**< Mutex unlock by a non-holder.*/
#define QURT_EMUTEXUNLOCKFATAL              42  /**< Mutex unlock error, all except the non-holder error.*/
#define QURT_EINVALIDPOWERCOLLAPSE          43  /**< Invalid power collapse mode requested. */ 

#define QURT_EISLANDUSEREXIT                44  /**< User call has resulted in island exit.*/
#define QURT_ENOISLANDENTRY                 45  /**< Island mode had not yet been entered.*/
#define QURT_EISLANDINVALIDINT              46  /**< Island mode has been exited due to an invalid island interrupt.*/
/** @addtogroup chapter_error
@{ */
#define QURT_EFATAL                         -1  /**< Fatal error. */ 
/** @} */ /* end_addtogroup chapter_error */


/* Error codes to distinguish from multiple error.
 * SSR and BADAVA are inconclusive without vector no.
 */
/* cause - error type - 8-bits*/ 
#define QURT_EXCEPT_PRECISE             1 /**< */
#define QURT_EXCEPT_NMI                 2 /**< */
#define QURT_EXCEPT_TLBMISS             3 /**< */
#define QURT_EXCEPT_RSVD_VECTOR         4 /**< */
#define QURT_EXCEPT_ASSERT              5 /**< */
#define QURT_EXCEPT_BADTRAP             6 /**< */
#define QURT_EXCEPT_TRAP1               7 /**< */
#define QURT_EXCEPT_EXIT                8 /**< */
#define QURT_EXCEPT_TLBMISS_X           10 /**< */
#define QURT_EXCEPT_STOPPED             11 /**< */
#define QURT_EXCEPT_FATAL_EXIT          12 /**< */
#define QURT_EXCEPT_INVALID_INT         13 /**< */
#define QURT_EXCEPT_FLOATING_POINT      14 /**< */
#define QURT_EXCEPT_DBG_SINGLE_STEP     15 /**< */
#define QURT_EXCEPT_TLBMISS_RW_ISLAND   16 /**< */
#define QURT_EXCEPT_TLBMISS_X_ISLAND    17 /**< */

#define QURT_ECODE_UPPER_LIBC         (0 << 16)  /**< Upper 16-bits is 0 for libc */
#define QURT_ECODE_UPPER_QURT         (0 << 16)  /**< Upper 16-bits is 0 for QuRT */
#define QURT_ECODE_UPPER_ERR_SERVICES (2 << 16)  /**< Upper 16-bits is 2 for error service */
/** @addtogroup chapter_error
@{ */
/** @cond RTOS_user_guide */
/* Enable floating point exceptions */
#define QURT_FP_EXCEPTION_ALL        0x1F << 25 /**< */
#define QURT_FP_EXCEPTION_INEXACT    0x1 << 29 /**< */
#define QURT_FP_EXCEPTION_UNDERFLOW  0x1 << 28 /**< */
#define QURT_FP_EXCEPTION_OVERFLOW   0x1 << 27 /**< */
#define QURT_FP_EXCEPTION_DIVIDE0    0x1 << 26 /**< */
#define QURT_FP_EXCEPTION_INVALID    0x1 << 25 /**< */

/** @endcond */
/** @} */ /* end_addtogroup chapter_error */

#endif /* QURT_ERROR_H */
