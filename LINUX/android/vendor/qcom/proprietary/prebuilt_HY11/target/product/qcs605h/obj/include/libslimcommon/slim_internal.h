#ifndef SLIM_INTERNAL_H
#define SLIM_INTERNAL_H
/**
  @file slim_internal.h
  @brief SLIM processor specific utils declarations.

  @ingroup slim_Core
*/
/*============================================================================
               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/8.5/gnss/slim/common/osal/inc/slim_internal.h#1 $ */

/* Define to include ped alignment in engineering build.
   This flag will be removed altogether once the QMI_SNS-API is available in modem.
*/
#undef FEATURE_GNSS_SLIM_SSC_PED_ALIGNMENT

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_internal_api.h"
#include "slim_processor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
@addtogroup slim_Core
@{
@name Processor-Specific Declarations
@{
*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
@brief Initialize critical section

Platform-specific initialization of reentrant mutex or it's equivalent.

@param[in] pz_Mutex Mutex to initialize.
*/
#define SLIM_INIT_CRIT_SECTION(pz_Mutex)  slim_MutexInit(pz_Mutex)
/**
@brief Enter critical section

Platform-specific locking of reentrant mutex or it's equivalent.

@param[in] pz_Mutex Mutex to lock.
*/
#define SLIM_ENTER_CRIT_SECTION(pz_Mutex) slim_MutexLock(pz_Mutex)
/**
@brief Leave critical section

Platform-specific unlocking of reentrant mutex or it's equivalent.

@param[in] pz_Mutex Mutex to unlock.
*/
#define SLIM_LEAVE_CRIT_SECTION(pz_Mutex) slim_MutexUnlock(pz_Mutex)
/**
@brief Macro to tell compiler to ignore unused variable

This macro forces compiler to ignore unused variable. The usage scenario is
restricted for interface functions implementation.

@param[in] var Variable literal
 */
#define SLIM_UNUSED(var) ((void)var)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
@brief Enum for processors supported by SLIM.
*/
typedef enum
{
  SLIM_PROCESSOR_MPSS  = 0, /**< Modem processor */
  SLIM_PROCESSOR_APSS,      /**< Application processor */

  //! @cond
  SLIM_PROCESSOR_MAX = 2147483647 /* To force a 32 bit signed enum. */
  //! @endcond
} slim_ProcessorEnumType;

/**
@brief Enum for SLIM initialization error codes

@sa slim_InitDiag
*/
typedef enum
{
  eSLIM_INIT_SUCCESS                   = 0,
  /**< No error - success */
  eSLIM_INIT_ERROR_ALREADY_RUNNING     = -1,
  /**< Initialization has been already done. */
  eSLIM_INIT_ERROR_FAILURE             = -2,
  /**< Initialization failed. */

  //! @cond
  eSLIM_INIT_ERROR_MIN = -2147483647 /* Force 32bit */
  //! @endcond
} slim_InitErrorEnumType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Returns current processor type.

Function returns current processor type.

@return Current processor type.
*/
slim_ProcessorEnumType slim_CurrentProcessor
(
  void
);

/**
@brief Size bounded memory copy.

Function copies bytes from the source buffer to the destination buffer.

@param[out] dst      Destination buffer.
@param[in]  dst_size Size of the destination buffer in bytes.
@param[in]  src      Source buffer.
@param[in]  src_size Number of bytes to copy from source buffer.
@return The number of bytes copied to the destination buffer.
*/
size_t slim_Memscpy
(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t src_size
);

/**
@brief Creates a mutex which can be associated with a resource.

Function creates a mutex which can be associated with a resource. Mutexes are
used to allow only one thread to enter the critical section of code that is
accessing shared data.

@param[out] pz_Mutex Pointer to the mutex structure
@return TRUE if initialized with no errors, otherwise FALSE.
*/
boolean slim_MutexInit
(
  slim_MutexStructType *pz_Mutex
);

/**
@brief Locks the resource associated with pz_Mutex if it is not already locked.

Function locks the resource associated with pz_Mutex if it is not already
locked.

@param[in] pz_Mutex Pointer to the mutex structure
*/
void slim_MutexLock
(
  slim_MutexStructType *pz_Mutex
);

/**
@brief Unlocks the resource associated with pz_Mutex.

Function unlocks the resource associated with pz_Mutex.

@param[in] pz_Mutex Pointer to the mutex structure
*/
void slim_MutexUnlock
(
  slim_MutexStructType *pz_Mutex
);

/**
@brief Returns the timetick count in milliseconds.

Function returns the current timetick count in milliseconds.

@return Current timetick count in milliseconds.
*/
uint64 slim_TimeTickGetMilliseconds
(
  void
);

/**
@brief Processor-specific DIAG initialization.

Function initializes DIAG module for target SLIM processor.

@return eSLIM_INIT_SUCCESS if initialization succeeds. Otherwise error code.
*/
slim_InitErrorEnumType slim_InitDiag
(
  void
);

/**
@}
@}
*/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_INTERNAL_H */
