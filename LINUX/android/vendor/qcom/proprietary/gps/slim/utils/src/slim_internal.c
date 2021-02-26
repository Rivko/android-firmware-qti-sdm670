/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
/**
@file
@brief SLIM processor specific utils implementation.

@ingroup slim_APSS
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <libslimcommon/slim_internal.h>

//! @addtogroup slim_APSS
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
@brief Returns current processor type.

Function returns current processor type.

@return Current processor type.
*/
slim_ProcessorEnumType slim_CurrentProcessor
(
  void
)
{
  return SLIM_PROCESSOR_APSS;
}

/**
@brief Size bounded memory copy.

Function copies bytes from the source buffer to the destination buffer.

@param dst       Destination buffer.
@param dst_size  Size of the destination buffer in bytes.
@param src       Source buffer.
@param src_size  Number of bytes to copy from source buffer.
@return The number of bytes copied to the destination buffer.
*/
size_t slim_Memscpy
(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t src_size
)
{
  return memscpy(dst, dst_size, src, src_size);
}

/**
@brief Creates a mutex which can be associated with a resource.

Function creates a mutex which can be associated with a resource. Mutexes are
used to allow only one thread to enter the critical section of code that is
accessing shared data.

@param pz_Mutex Pointer to the mutex structure
@return TRUE if initialized with no errors, otherwise FALSE.
*/
boolean slim_MutexInit
(
  slim_MutexStructType *pz_Mutex
)
{
  return os_MutexInit(pz_Mutex, MUTEX_DATA_ONLY_CONTEXT);
}

/**
@brief Locks the resource associated with pz_Mutex if it is not already locked.

Function locks the resource associated with pz_Mutex if it is not already
locked.

@param pz_Mutex Pointer to the mutex structure
*/
void slim_MutexLock
(
  slim_MutexStructType *pz_Mutex
)
{
  os_MutexLock(pz_Mutex);
}

/**
@brief Unlocks the resource associated with pz_Mutex.

Function unlocks the resource associated with pz_Mutex.

@param pz_Mutex Pointer to the mutex structure
*/
void slim_MutexUnlock
(
  slim_MutexStructType *pz_Mutex
)
{
  os_MutexUnlock(pz_Mutex);
}

/**
@brief Processor-specific DIAG initialization.

Function initializes DIAG module for target SLIM processor.

@return eSLIM_INIT_SUCCESS if initialization succeeds. Otherwise error code.
*/
slim_InitErrorEnumType slim_InitDiag()
{
  return eSLIM_INIT_SUCCESS;
}

//! @}
