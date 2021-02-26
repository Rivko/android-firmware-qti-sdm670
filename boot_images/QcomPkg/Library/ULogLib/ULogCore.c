/*============================================================================
@file ULogCoreInterface.c

Core interface for ULog.

Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "ULogBuildFeatures.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFEATURE_ULOG_CORE
#include "DALSys.h"
#include "ULog.h"
#include "CoreVerify.h"
#include "CorePool.h"
#include "CoreMutex.h"
#include "MemBarrier.h"
#include "ULogCoreInternal.h"
#include "ULogInlines.h"
#include "ULogFront.h"
#include "CoreTime.h"
#include "CoreString.h"
#ifndef DEFEATURE_ULOG_STM
#include "tracer.h"
#endif

/** The global pointer to the linked list of ULogs */
LOG_CONTEXT ulogContext;

//Function tables for the different ways to write out the log data.
ULogResult ULogCore_Write(ULogHandle h, uint32 firstMsgCount, const char * firstMsgContent, uint32 secondMsgCount, const char * secondMsgContent, ULOG_INTERFACE_TYPE interfaceType);
DALBOOL ULogCore_MultipartMsgBegin (ULogHandle h);
void ULogCore_MultipartMsgEnd (ULogHandle h);

static ULOG_CORE_VTABLE CoreVtableNormal =
{
  ULogCore_Write,
  ULogCore_MultipartMsgBegin,
  ULogCore_MultipartMsgEnd
};


#ifndef DEFEATURE_ULOG_TEE
ULogResult ULogCore_TeeWrite (ULogHandle h, uint32 firstMsgCount, const char * firstMsgContent, uint32 secondMsgCount, const char * secondMsgContent, ULOG_INTERFACE_TYPE interfaceType);
DALBOOL ULogCore_TeeMultipartMsgBegin (ULogHandle h);
void ULogCore_TeeMultipartMsgEnd (ULogHandle h);

static ULOG_CORE_VTABLE CoreVtableTee =
{
  ULogCore_TeeWrite,
  ULogCore_TeeMultipartMsgBegin,
  ULogCore_TeeMultipartMsgEnd
};


static CorePoolConfigType ULogTeePoolConfig =
{
  sizeof(ULOG_TEE_TYPE), // Element size
  4,                     // Number of elements in the initial allocation
  4                      // Number of elements in the subsequent allocations
};

static CorePoolConfigType ULogTeeLinkPoolConfig =
{
  sizeof(ULOG_TEE_LINK_TYPE), // Element size
  4,                         // Number of elements in the initial allocation
  4                          // Number of elements in the subsequent allocations
};

#endif /* DEFEATURE_ULOG_TEE */


#ifndef DEFEATURE_ULOG_STM
ULogResult ULogCore_STMWrite (ULogHandle h, uint32 firstMsgCount, const char * firstMsgContent, uint32 secondMsgCount, const char * secondMsgContent, ULOG_INTERFACE_TYPE interfaceType);
DALBOOL ULogCore_STMMultipartMsgBegin (ULogHandle h);
void ULogCore_STMMultipartMsgEnd (ULogHandle h);

static ULOG_CORE_VTABLE CoreVtableSTM =
{
  ULogCore_STMWrite,
  ULogCore_STMMultipartMsgBegin,
  ULogCore_STMMultipartMsgEnd
};
#endif /* DEFEATURE_ULOG_STM */


/**
 * <!-- ULogCore_Init -->
 *
 * @brief Initialize the ULog System.
 *
 * It is not strictly necessary to call this function since it
 * will be executed any time an attempt is made to connect to, or
 * create a log.
 *
 * @return DAL_SUCCESS if the initialization is successful.
 */
ULogResult ULogCore_Init(void)
{
  if (ulogContext.gLockHandle == NULL)
  {
    ulogContext.gLockHandle 
      = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT);
  }
  return DAL_SUCCESS;
}


/**
 * <!-- ULogCore_LogCreate -->
 *
 * @brief Create a new log.
 * 
 * Create a new log, and add it to the logs managed by the ULog
 * subsystem.
 * 
 * @param h : A ULog handle is returned at this pointer. 
 * @param logName : The log name (24 chars max, including NULL).
 * @param bufferSize : The size in bytes of the log buffer. A request of size 0 
 *                     will leave the buffer unallocated. 
 * @param memoryType : The memory configuration.
 * @param lockType : The type of locking required.
 * @param interfaceType : This field is a legacy option. There is only a single
 *                   ULOG type. Any value is ok for this param and will be
 *                   ignored. The RAW and REALTIME types are only used
 *                   internally to indicate if a write has a timestamp
 *                   included.
 *
 * @return DAL_SUCCESS if the create completed successfully.  The log will have been
 *        enabled and will be ready to use. 
 *        ULOG_ERR_INITINCOMPLETE if ULOG_MEMORY_LOCAL_MANUAL is used.  
 *        ULOG_ERR_INITINCOMPLETE if a buffersize of 0 is used.  
 *        The log can be enabled later after ULogCore_MemoryAssign sets up all the parts.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_LogCreate(ULogHandle * h, const char *  logName,
                              uint32 bufferSize, uint32 memoryType,
                               ULOG_LOCK_TYPE lockType,
                               ULOG_INTERFACE_TYPE interfaceType )
{
  uint32 maxMemory;
  void * memPtr;
  DALResult result = DAL_SUCCESS;
  ULOG_TYPE * log;

  // Make sure ULog is initialized before continuing.
  ULogCore_Init();

  //If the h pointer value is clearly invalid, return an error.
  if (h==NULL)
  {
    return ULOG_ERR_INVALIDPARAMETER;
  }

  //The shared memory config was added a long time ago with the intent
  //of someday supporting shared logs with RPM or other processors.
  //It's no longer supported.
  if (memoryType & ULOG_MEMORY_CONFIG_SHARED) //sga -- since this is no longer supported, remove all instances and simplify read/write cores to use same basic implementation that micro ulog uses
  {
    return ULOG_ERR_NOTSUPPORTED;
  }

  // Spin locks are unsupported at this time.
  if (lockType == ULOG_LOCK_SPIN) //sga -- do we have plans to implement spin locks?? if not lets remove these references, if so lets add spin locks to the ulog v5 requirements
  {
    return ULOG_ERR_NOTSUPPORTED;
  }

  if (((memoryType & ULOG_MEMORY_USAGE_WRITEABLE) == 0) &&   // If the log is NOT writeable
       ((memoryType & ULOG_ALLOC_MANUAL) == 0))              // Then it must be manually allocated.
  {
    return(ULOG_ERR_INVALIDPARAMETER);
  }

  // Connect creates the ULOG_TYPE log structure if it doesn't exist already.
  result = ULogCore_Connect((ULogHandle*)&log, logName);
  if (result != DAL_SUCCESS)
  {
    return(result);
  }

  // Grab the global lock so we only create one log at a time.
  Core_MutexLock(ulogContext.gLockHandle);

  // Check if the log already exists in a completed state.
  if (log->logStatus & ULOG_STATUS_DEFINED)
  {
    // Not setting any error bits in the log because we don't consider trying to create
    // a log that already exists a serious error.
    Core_MutexUnlock(ulogContext.gLockHandle);
    *h = (ULogHandle *)log;
    return(ULOG_ERR_ALREADYCREATED);
  }

  // If execution is here, an incomplete top level ULOG_TYPE, has been created,
  // it's name has been set, and the log is in the linked list of ulogs.
  // There are several mallocs and setup steps remaining.  If there are
  // any failures we will try to clean up memory and set the log's status to
  // ULOG_STATUS_MALLOC_FAIL or ULOG_STATUS_SETUP_FAIL. The log will not
  // have it's "defined" bit or "enabled" bit set, so no real operations will
  // be done with the log.  The log->logstatus error flags will hopefully help
  // debug the log's creation problem.


  // Initialize and copy input parameters to log header.
  log->version = ULOG_TYPE_LOG_HEADER_VERSION;
  log->level = 0xFFFFFFFF;
  log->bufMemType = memoryType;
  log->lockType = lockType;
  log->defineBufSize = bufferSize;
  log->stmTracerClientHandle = NULL;
  log->transport = ULOG_TRANSPORT_RAM;

#ifdef TARGET_UEFI
  log->OpenFile = NULL;
  log->MessagesSinceFlush = 0;
  log->MaxLogFileSize = (UINTN)0;
  log->LogHasOverlapped = FALSE;
  log->config = NULL;
#endif

#ifdef ULOG_64BIT_TIME
  log->feature_flags1 |= ULOG_FF1_64_BIT_TS;
#else
  log->feature_flags1 &= ~ULOG_FF1_64_BIT_TS;
#endif

#ifdef ULOG_64BIT_PROC
  log->feature_flags1 |= ULOG_FF1_64_BIT_PROC; 
#else
  log->feature_flags1 &= ~ULOG_FF1_64_BIT_PROC; 
#endif

  // Allocate buffer memory
  if ((memoryType & ULOG_ALLOC_LOCAL))
  {
    // Determine how much memory to allocate for the buffer
    maxMemory = log->requestedSize<log->defineBufSize?log->defineBufSize:log->requestedSize;

    
    //=== Log Buffer ===
    //if a zero memory log is requested we don't allocate a buffer, however we
    //do continue on to allocate the other needed allocations.
    if (maxMemory != 0)
    {
      //make sure the log size is a power of 2 and allocate it
      log->bufSize = ULogInternal_ConvertSize(maxMemory);
      if (ULogInternal_Malloc((ULOG_ALLOC_TYPE)log->bufMemType, &memPtr, log->bufSize)!=DAL_SUCCESS)
      {
        //set error bit(s) in the status flag to document why the create failed
        log->logStatus = ULOG_STATUS_MALLOC_FAIL;
        Core_MutexUnlock(ulogContext.gLockHandle);
        return(ULOG_ERR_MALLOC);
      }
      if (ULogCore_MemoryAssign(log, ULOG_VALUE_BUFFER, memPtr, log->bufSize)!=DAL_SUCCESS)
      {
        //failed to assign the memory.  Free the memory we earlier allocated.
        ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, &memPtr);
        //set error bit(s) in the status flag to document why the create failed
        log->logStatus = ULOG_STATUS_SETUP_FAIL;
        Core_MutexUnlock(ulogContext.gLockHandle);
        return(ULOG_ERR_ASSIGN);
      }
    }

    //=== Reader Core ===
    if (ULogInternal_Malloc((ULOG_ALLOC_TYPE)log->bufMemType, &memPtr, sizeof(ULOG_READER_CORE_TYPE))!=DAL_SUCCESS)
    {
      //free the previously allocated buffer
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->buffer);
      //set error bit(s) in the status flag to document why the create failed
      log->logStatus =  ULOG_STATUS_MALLOC_FAIL;
      Core_MutexUnlock(ulogContext.gLockHandle);
      return(ULOG_ERR_MALLOC);
    }
    if (ULogCore_MemoryAssign(log, ULOG_VALUE_READCORE, memPtr, sizeof(ULOG_READER_CORE_TYPE))!=DAL_SUCCESS){
      //free the previously allocated buffer
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->buffer);
      //free the memory we just allocated
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, memPtr);
      //set error bit(s) in the status flag to document why the create failed
      log->logStatus = ULOG_STATUS_SETUP_FAIL;
      Core_MutexUnlock(ulogContext.gLockHandle);
      return(ULOG_ERR_ASSIGN);
    }

    //=== Writer Core ===
    if (ULogInternal_Malloc((ULOG_ALLOC_TYPE)log->bufMemType, &memPtr, sizeof(ULOG_WRITER_CORE_TYPE))!=DAL_SUCCESS)
    {
      //free the previously allocated buffer
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->buffer);
      //free the previously allocated reader core
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->readerCore);
      //set error bit(s) in the status flag to document why the create failed
      log->logStatus = ULOG_STATUS_MALLOC_FAIL;
      Core_MutexUnlock(ulogContext.gLockHandle);
      return(ULOG_ERR_MALLOC);
    }
    if (ULogCore_MemoryAssign(log, ULOG_VALUE_WRITECORE, memPtr, sizeof(ULOG_WRITER_CORE_TYPE))!=DAL_SUCCESS)
    {
      //free the previously allocated buffer
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->buffer);
      //free the previously allocated reader core
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->readerCore);
      //free the memory we just allocated
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, memPtr);
      //set error bit(s) in the status flag to document why the create failed
      log->logStatus = ULOG_STATUS_SETUP_FAIL;
      Core_MutexUnlock(ulogContext.gLockHandle);
      return(ULOG_ERR_ASSIGN);
    }
  }

  //=== Lock ===
  switch (lockType)
  {
    default:
    case ULOG_LOCK_NONE:
      log->lockHandle = NULL;
      break;

    case ULOG_LOCK_OS:
      log->lockHandle = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT);
      if (log->lockHandle == NULL){
        //set error bit(s) in the status flag to document why the create failed
        log->logStatus = ULOG_STATUS_SETUP_FAIL | ULOG_STATUS_MALLOC_FAIL;
        Core_MutexUnlock(ulogContext.gLockHandle);
        return(ULOG_ERR_MISC);
      }
      break;

    case ULOG_LOCK_SPIN:
      // "Spin Locks are not yet supported."
      //set informational error bit(s) in the status flag
      log->logStatus = ULOG_STATUS_SETUP_FAIL;
      Core_MutexUnlock(ulogContext.gLockHandle);
      return(ULOG_ERR_MISC);

    case ULOG_LOCK_INT:
      log->lockHandle = Core_MutexCreateEx(CORE_MUTEXTYPE_INTLOCK,
                                           CORE_MUTEXATTR_DEFAULT);
      if (log->lockHandle == NULL){
        //set error bit(s) in the status flag to document why the create failed
        log->logStatus = ULOG_STATUS_SETUP_FAIL | ULOG_STATUS_MALLOC_FAIL;
        Core_MutexUnlock(ulogContext.gLockHandle);
        return(ULOG_ERR_MISC);
      }
      break;
  }
  

  // Set the shared header, this pointer points to this log.  If sharing headers across processors 
  // is ever supported in the future it will point to the shared header.  For now it may be useful 
  // as a sanity check to see if debug symbols are valid. If this value in a log doesn't point to 
  // itself then either the wrong ELF symbols are loaded, or maybe the memory is invalid. 
  memPtr = (void *)log;
  if (ULogCore_MemoryAssign(log, ULOG_VALUE_SHARED_HEADER, memPtr, sizeof(ULOG_TYPE))!=DAL_SUCCESS)
  {
    //free the previously allocated buffer
    ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->buffer);
    //free the previously allocated reader core
    ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->readerCore);
    //free the previously allocated writer core
    ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->writerCore);
    //set error bit(s) in the status flag to document why the create failed
    log->logStatus = ULOG_STATUS_SETUP_FAIL;
    Core_MutexUnlock(ulogContext.gLockHandle);
    return(ULOG_ERR_ASSIGN);
  }

  log->logStatus |= ULOG_STATUS_DEFINED;

  // If ULOG_ALLOC_LOCAL is set in the memoryType, everything will be setup at
  // this point and the log will be enabled.  ULogCore_Enable will return DAL_SUCCESS
  // If memory pointers in the log are still NULL because ULOG_ALLOC_MANUAL was
  // selected, ULogCore_Enable will return ULOG_ERR_INITINCOMPLETE, and the log
  // can be enabled after everything is setup.
  result = ULogCore_Enable((ULogHandle *)log);

  *h = (ULogHandle *)log;

#ifdef ULOG_PUBLISHLOG_SUPPORT
  /* Publish the newly created log into the QRS for others to use. */
  ULogInternal_PublishLog(logName);
#endif

  log->altTimestampSrcFn = NULL;
  Core_MutexUnlock(ulogContext.gLockHandle);
  return(result);
}

/**
 * <!-- ULogCore_SetLockType -->
 *
 * @brief Switches the lock type for a log.
 *
 *
 * @param h : A ULog handle.
 * @param lockType : The desired lock type for the log
 * 
  */
void ULogCore_SetLockType( ULogHandle h, ULOG_LOCK_TYPE lockType )
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;


  //verify a new lock type is being requested
  if ( lockType != log->lockType )
  {
    log->lockType = lockType;
    switch ( lockType )
    {
      case ULOG_LOCK_NONE:
        break;
      case ULOG_LOCK_INT:
        if ( !log->lockHandle )
        {
          CORE_VERIFY_PTR( log->lockHandle =
                           Core_MutexCreateEx( CORE_MUTEXTYPE_INTLOCK,
                                               CORE_MUTEXATTR_DEFAULT ) );
        }
        break;
      case ULOG_LOCK_OS :
        if ( !log->lockHandle )
        {
          CORE_VERIFY_PTR( log->lockHandle =
                           Core_MutexCreate( CORE_MUTEXATTR_DEFAULT ) );
        }
        break;
      case ULOG_LOCK_SPIN:
      default:
        //bad lock type
        CORE_VERIFY( 0 );

    }

    ULOG_RT_PRINTF_1( h, "**** Switched lock to %d ****",  lockType );
  }
}

/**
 * <!-- ULogCore_HeaderSet -->
 *
 * @brief Set the header for the log.
 *
 * Apply the supplied string to a given log header.  The header
 * is stored with a log, and will not be overwritten unless this
 * function is called again.
 *
 * There are no real limitations to this string.  The system
 * determines the length of the supplied string, allocates that
 * amount of memory, and copies the string to it.
 *
 * @param h : A ULog handle.
 * @param headerText : The string to store in the log.
 *
 * @return DAL_SUCCESS if the header was successfully stored.
 */
ULogResult ULogCore_HeaderSet(ULogHandle h, char * headerText)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return (DAL_ERROR);
  }
#ifndef DEFEATURE_ULOG_TEE
  if (!HANDLE_IS_A_TEE(log))
#endif
  {
    size_t headerTextLength;

    ULogInternal_LockGet(log);

    // Remove any existing log header
    if (log->header != NULL)
    {
      DALSYS_Free(log->header);
    }
    headerTextLength = strlen(headerText)+1;
    DALSYS_Malloc(headerTextLength, (void **)&log->header);
    if (log->header != NULL)
    {
      core_strlcpy(log->header, headerText, headerTextLength);
    }
    else
    {
      ULogInternal_LockRelease(log);
      return (DAL_ERROR);
    }
    ULogInternal_LockRelease(log);
  }

  return(DAL_SUCCESS);
}


/**
 * <!-- ULogCore_AttributeSet -->
 *
 * @brief Set the log attributes
 * 
 * OR in the provided attribute bits into the logs attribute mask.
 *
 * @param h : The log to set attributes for
 * @param attribute : The attributes to set
 *
 * @return A handle to the named log
 */
void ULogCore_AttributeSet(ULogHandle h, uint32 attribute)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  if (log != NULL)
  {
  #ifndef DEFEATURE_ULOG_TEE
    if (!HANDLE_IS_A_TEE(log))
  #endif
    {
      log->attributes |= attribute;
	  return;
	}
	return;
  }
}


/**
 * <!-- ULogCore_MemoryAssign -->
 *
 * @brief Assign memory to specific log buffers.
 * 
 * When a log is created with the allocation type
 * ULOG_ALLOC_MANUAL, the buffers are and must be manually
 * assigned with this function.  All supplied buffers must be word
 * aligned.  If a buffer is not word aligned, this function will
 * fail.
 * 
 * @param h : A ULog handle.
 * @param id : Which buffer to assign.
 * @param bufferPtr : A pointer to the buffer memory to assign.
 *                    Note that ALL buffers must be word aligned.
 * @param bufferSize : The size in bytes of the buffer.
 *
 * @return DAL_SUCCESS if the memory was assigned successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_MemoryAssign(ULogHandle h, ULOG_VALUE_ID id,
                                  void * bufferPtr, uint32 bufferSize)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 realSize;
  DALResult result = DAL_ERROR;

  if (log == NULL)
  {
    return(DAL_ERROR);
  }

  #ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log)){
      return(ULOG_ERR_INVALIDHANDLE);
  }
  #endif

  if (((DALMemAddr)bufferPtr & 0x03) != 0)  // If the ptr is not word aligned...
  {
    return(DAL_ERROR);
  }

  switch(id)
  {
    case ULOG_VALUE_BUFFER:
      // Only assign memory if we don't already have some
      // assigned.
      if (log->buffer == NULL)
      {
        realSize = ULogInternal_ConvertSize(bufferSize);
        if (realSize > bufferSize)
        {
          // Make sure we were given enough memory.
          result = DAL_ERROR;
          break;
        }
        log->buffer = (char *)bufferPtr;
        log->bufSize = realSize;
        log->bufSizeMask = realSize - 1;
        result = DAL_SUCCESS;
      }
      break;

    case ULOG_VALUE_SHARED_HEADER:
      if ((log->sharedHeader == NULL) &&
          (bufferSize >= sizeof(ULOG_TYPE)))
      {
        log->sharedHeader = (ULOG_TYPE*)bufferPtr;
        result = DAL_SUCCESS;
      }
      break;

    case ULOG_VALUE_READCORE:
      if ((log->readerCore == NULL) &&
          (bufferSize >= sizeof(ULOG_READER_CORE_TYPE)))
      {
        log->readerCore = (ULOG_READER_CORE_TYPE*)bufferPtr;
        result = DAL_SUCCESS;
      }
      break;

    case ULOG_VALUE_WRITECORE:
      if ((log->writerCore == NULL) &&
          (bufferSize >= sizeof(ULOG_WRITER_CORE_TYPE)))
      {
        log->writerCore = (ULOG_WRITER_CORE_TYPE *)bufferPtr;
        result = DAL_SUCCESS;
      }
      break;

    case ULOG_VALUE_LOCKCORE:
      // No longer supported
      result = DAL_SUCCESS;
      break;

    default:
      result = DAL_ERROR;
      break;
  }
  return(result);
}


/**
 * <!-- ULogCore_Enable -->
 *
 * @brief Enable a log that is already defined.
 *
 * Enable a log.  A log can need manual enabling if it has manual
 * allocation, has been disabled, or buffer memory was not assigned.
 *
 * @param h : Handle of the log to be enabled.
 *
 * @return DAL_SUCCESS if the log was enabled successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Enable(ULogHandle h)
{
  DALResult result = DAL_ERROR;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return(DAL_ERROR);
  }

#ifndef DEFEATURE_ULOG_TEE
  if (!HANDLE_IS_A_TEE(log))
#endif
  {
    Core_MutexLock(ulogContext.gLockHandle);

    if ((log->sharedHeader == NULL) ||
        ((log->logStatus & ULOG_STATUS_DEFINED) == 0) ||
        (log->readerCore == NULL) ||
        (log->writerCore == NULL) ||
        (log->buffer == NULL))
    {
      result = ULOG_ERR_INITINCOMPLETE;
    }
    else if (log->sharedHeader->logStatus & ULOG_STATUS_ENABLED)
    {
      result = ULOG_ERR_ALREADYENABLED;
    }
    else if (log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)
    {
      // Lock the log.
      if (log->lockHandle != NULL)
      {
        ULogInternal_LockGet(log);
      }

      // Clear out the core logging variables before we
      // enable the log.
      memset(log->readerCore, 0, sizeof(ULOG_READER_CORE_TYPE));
      memset(log->writerCore, 0, sizeof(ULOG_WRITER_CORE_TYPE));

      log->sharedHeader->logStatus |= ULOG_STATUS_ENABLED;

      // Unlock the log.
      if (log->lockHandle != NULL)
      {
        ULogInternal_LockRelease(log);
      }
      result = DAL_SUCCESS;
    }
    else
    {
      result = ULOG_ERR_READONLY;
    }

    Core_MutexUnlock(ulogContext.gLockHandle);
  }

  return (result);
}


/**
 * <!-- ULogCore_IsEnabled -->
 *
 * @brief Check if a log is enabled, and provide a vtable. 
 *
 * @param h : A ULog handle.
 * @param core : A vtable to be filled in with the appropriate
 *               interface functions for the current log
 *
 * @return TRUE if the log is enabled. Return the correct vtable in the core pointer.
 */
DALBOOL ULogCore_IsEnabled(ULogHandle h, ULOG_CORE_VTABLE ** core)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return (FALSE);
  }

#ifndef DEFEATURE_ULOG_TEE
  // Is this log a tee
  if (HANDLE_IS_A_TEE(log))
  {
    ULOG_TEE_LINK_TYPE * link;
    ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)log;
    if (core!=NULL)
    {
      *core = &CoreVtableTee;
    }
    link = tee->link;
    while (link != NULL)
    {
      if ((link->log != NULL) &&
          (link->log->sharedHeader != NULL) &&
          (link->log->sharedHeader->logStatus & ULOG_STATUS_ENABLED))
      {
        tee->firstEnabled = link;
        return(TRUE);
      }
      link = link->next;
    }
    // None of the logs in the Tee are enabled.  
    return FALSE;
  }
  else
#endif /* DEFEATURE_ULOG_TEE */
  {
    switch (log->transport){
      case ULOG_TRANSPORT_RAM:
        if (core!=NULL)
        {
          *core = &CoreVtableNormal;
        }
        break;

      case ULOG_TRANSPORT_STM:
#ifndef DEFEATURE_ULOG_STM
        if (core!=NULL)
        {
          *core = &CoreVtableSTM;
        }
#endif
        break;

      case ULOG_TRANSPORT_ALTERNATE:
        if (core!=NULL)
        {
          *core = log->altTransportVTable;
        }
        break;

      default:
        if (core!=NULL)
        {
          *core = &CoreVtableNormal;
        }
        break;
    }

    // We already check that log is != NULL
    if ((log->sharedHeader != NULL) &&
        (log->sharedHeader->logStatus & ULOG_STATUS_ENABLED))
    {
      return(TRUE);
    }
  }
  return (FALSE);
}


/**
 * <!-- ULogCore_Query -->
 * 
 * @brief Query information about a specific log or the logging system.
 * 
 * This function will return the expected sizes of internal log
 * buffers, in order to manually allocate the buffers.  Use a NULL
 * handle for these values.
 *
 * @param h : A ULog handle.
 * @param id : A log buffer ID.
 * @param value : The returned number of bytes needed by the
 *               selected buffer.
 *
 * @return DAL_SUCCESS if the buffer id was known, and supported.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Query(ULogHandle h, ULOG_VALUE_ID id, uint32 * value)
{
  DALResult result = DAL_SUCCESS;

  if (id < ULOG_VALUE_LOG_BASE)
  {
    switch(id)
    {
      case ULOG_VALUE_BUFFER:
        // We cannot specify the buffer size.  Return 0.
        *value = 0;
        result = DAL_ERROR;
        break;

      case ULOG_VALUE_SHARED_HEADER:
        *value = sizeof(ULOG_TYPE);
        break;

      case ULOG_VALUE_READCORE:
        *value = sizeof(ULOG_READER_CORE_TYPE);
        break;

      case ULOG_VALUE_WRITECORE:
        *value = sizeof(ULOG_WRITER_CORE_TYPE);
        break;

      case ULOG_VALUE_LOCKCORE:
        *value = 0;
        break;

      default:
        result = DAL_ERROR;
        break;
    }
  }
  else
  {
    ULOG_TYPE * log = (ULOG_TYPE *)h;

    switch(id)
    {
      case ULOG_VALUE_LOG_READFLAGS:
        if ((log != NULL) &&
            (log->readerCore != NULL))
        {
          *value = log->readerCore->readFlags;
          log->readerCore->readFlags = 0;
        }
        else
        {
          result = DAL_ERROR;
        }
        break;

      default:
        result = DAL_ERROR;
        break;
    }
  }

  return(result);
}


/**
 * <!-- ULogCore_LogLevelSet -->
 *
 * @brief Set the log level.
 *
 * ULog does not filter messages, but this function is
 * intended to facilitate filtering without requiring it. This
 * simply sets a parameter in a log that can be used to filter log
 * messages, along with the provided macros.
 *
 * @param h : A ULog handle.
 * @param level : The new level for the log.
 */
void ULogCore_LogLevelSet(ULogHandle h, uint32 level)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log != NULL)
  {
#ifndef DEFEATURE_ULOG_TEE
    if (!HANDLE_IS_A_TEE(log))
#endif
    {
      // Compare this level to the level in the log.
      //
      if (log != NULL)
      {
        log->level = level;
      }
    }
  }
}


/**
 * <!-- ULogCore_LogLevelGet -->
 *
 * @brief Get the log level.
 *
 * ULog does not filter messages, but this function is
 * intended to facilitate filtering without requiring it. This
 * simply returns a parameter in a log that can be used to filter
 * log messages, along with the provided macros.
 *
 * @param h : A ULog handle.
 *
 * @return The current log value.  0 if the log handle is NULL.
 */
uint32 ULogCore_LogLevelGet(ULogHandle h)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log != NULL)
  {
  #ifndef DEFEATURE_ULOG_TEE
    if (!HANDLE_IS_A_TEE(log))
  #endif
    {
      if (log != NULL)
      {
        return(log->level);
      }
    }
  }
  return(0);
}


/**
 * <!-- ULogCore_Connect -->
 *
 * @brief Connect a log handle to a log defined elsewhere.
 *
 * Connect a log handle to a log that has been, or will be defined
 * elsewhere.  A successful return allows logging through the
 * handle, although no data will be written to a physical log
 * until the log is defined with the ULogCore_Create function.
 *
 * @param h : A ULog handle.
 * @param logName : The name of the log to connect with.
 *
 * @return DAL_SUCCESS if the connect was successful. Errors are
 *        defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Connect(ULogHandle * h,  const char * logName)
{
  ULOG_TYPE * tempLog;

  //If the h pointer value is clearly invalid, return an error.
  if (h==NULL)
  {
    return(DAL_ERROR);
  }

  if(DAL_SUCCESS != ULogInternal_NameVerify(logName))
  {
    return ULOG_ERR_INVALIDNAME;
  }

  Core_MutexLock(ulogContext.gLockHandle);

  tempLog = (ULOG_TYPE *)ULogCore_HandleGet((char*)logName);

  // Create a new log if one with this name doesn't already exist.
  if (tempLog == NULL)
  {
    if ((DALSYS_Malloc(sizeof(ULOG_TYPE), (void **)&tempLog) == DAL_ERROR) ||
        (tempLog == NULL))
    {
      return(DAL_ERROR);
    }

    //zeroing the ULOG_TYPE ensures everything, including the log->logStatus.
    //log->logStatus == 0 indicates the log is not enabled and not considered
    //defined yet.
    memset(tempLog, 0, sizeof(ULOG_TYPE));

    // Store the log name
    core_strlcpy(tempLog->name, logName, ULOG_MAX_NAME_SIZE+1);
	
    // Link the log into the list
    tempLog->next = ulogContext.logHead;
    ulogContext.logHead = tempLog;
  }

  Core_MutexUnlock(ulogContext.gLockHandle);

  *h = tempLog;

  return DAL_SUCCESS;
}


/**
 * <!-- ULogCore_HandleGet -->
 *
 * @brief Get a log handle by its name.
 *
 * @param logName : Name of a ULog to search for.
 *
 * @return A handle to the named log
 */
ULogHandle ULogCore_HandleGet(char *logName)
{
  ULOG_TYPE * tempLog;

  if(DAL_SUCCESS != ULogInternal_NameVerify(logName))
  {
    // Invalide log name.
    return NULL;
  }

  Core_MutexLock(ulogContext.gLockHandle);

  // Search for an existing log of this type.
  tempLog = ulogContext.logHead;
  while (tempLog != NULL)
  {
    if (strncmp(tempLog->name, logName, ULOG_MAX_NAME_SIZE + 1) == 0)
    {
      // We have a match.
      Core_MutexUnlock(ulogContext.gLockHandle);
      return (ULogHandle) tempLog;
    }
    tempLog = tempLog->next;
  }

  Core_MutexUnlock(ulogContext.gLockHandle);
  return NULL;
}


/**
 * <!-- ULogCore_Write -->
 *
 * @brief The general log write function that takes message content and copies it into the log.
 *
 * @param h : A ULog handle.
 * @param firstMsgCount : how many bytes of data in the first array of content
 * @param firstMsgContent : The data to put into the log. 
 * @param secondMsgCount : how many bytes of data in the second array of content
 * @param secondMsgContent : The remaining data to put into the log. 
 * @param interfaceType : Not used, this is a legacy argument. 
 *
 * @return DAL_SUCCESS if no problems encountered. 
 */
ULogResult ULogCore_Write(ULogHandle h, uint32 firstMsgCount,
                           const char* firstMsgContent,
                           uint32 secondMsgCount,
                           const char* secondMsgContent, 
                           ULOG_INTERFACE_TYPE interfaceType )
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 msgCount = firstMsgCount + secondMsgCount;
  uint32 requestingSize = ULOGINTERNAL_PADCOUNT(msgCount);
  uint32* wordPtr;

  if ((log == NULL) ||
      (log->sharedHeader == NULL) ||
      !(log->sharedHeader->logStatus & ULOG_STATUS_ENABLED) ||
      !(log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE) ||
      (msgCount > ULOG_MAX_MSG_SIZE)
     )
  {
    return(DAL_ERROR);
  }

  ULogInternal_LockGet(log);

  // If the message is too large for the log buffer, just drop it.
  if (requestingSize >= log->bufSize)
  {
    log->writerCore->usageData |= ULOG_ERR_LARGEMSG;
    ULogInternal_LockRelease(log);
    return(DAL_ERROR);
  }

  // Prepare for the transaction
  log->transactionWriteCount = 0;
  log->transactionWriteStart = log->writerCore->write;

  //check the write and readWriter for available space, free space if needed.
  if (ULogInternal_FreeSpaceForNewMsg(log, requestingSize)==FALSE){
    ULogInternal_LockRelease(log);
    return(DAL_ERROR);
  }

  // The readWriter pointer must be correct in memory before we
  // write to the buffer so the reader does not incorrectly read
  // a message as it is being overwritten.
  //
  // The message length must also be set to zero before proceeding
  // to stop a read of a bogus message.
  CORE_MEM_BARRIER();

  ULogInternal_WriteData(log, firstMsgCount, firstMsgContent);
  if (secondMsgCount>0){
   ULogInternal_WriteData(log, secondMsgCount, secondMsgContent);
  }

  // The message data must be valid in the log before we set the
  // message length to a non-zero value.
  CORE_MEM_BARRIER();

  //write the final size for this message into the buffer.
  wordPtr = (uint32 *)&log->buffer[log->transactionWriteStart & log->bufSizeMask];
  *wordPtr |= (log->transactionWriteCount<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

  ULogInternal_LockRelease(log);

  return(DAL_SUCCESS);
}


#ifndef DEFEATURE_ULOG_TEE
/**
 * <!-- ULogCore_TeeWrite -->
 * 
 * @brief The Tee version of the write function.
 *
 * @param h : A ULog handle.
 * @param firstMsgCount : how many bytes of data in the first array of content
 * @param firstMsgContent : The data to put into the log. 
 * @param secondMsgCount : how many bytes of data in the second array of content
 * @param secondMsgContent : The remaining data to put into the log. 
 * @param interfaceType : Not used, this is a legacy argument. 
 *
 * @return DAL_SUCCESS if no problems encountered. 
 */
ULogResult ULogCore_TeeWrite(ULogHandle h,
                           uint32 firstMsgCount,
                           const char * firstMsgContent,
                           uint32 secondMsgCount,
                           const char * secondMsgContent,
                           ULOG_INTERFACE_TYPE interfaceType)
{
  ULOG_TEE_LINK_TYPE * link;
  ULOG_CORE_VTABLE * core;
  ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)h;
  link = tee->firstEnabled;
  while (link != NULL)
  {
    //Use the core function table to call the correct write function.
    if(ULogCore_IsEnabled(link->log, &core) == TRUE)
    {
      core->write(link->log, firstMsgCount, firstMsgContent, secondMsgCount, secondMsgContent, interfaceType);
    }
    link = link->next;
  }
  return(DAL_SUCCESS);
}
#endif /* DEFEATURE_ULOG_TEE */


#ifndef DEFEATURE_ULOG_STM
/**
 * <!-- ULogCore_STMWrite -->
 *
 * @brief The STM version of the write function.
 *
 * @param h : A ULog handle.
 * @param firstMsgCount : how many bytes of data in the first array of content
 * @param firstMsgContent : The data to put into the log. 
 * @param secondMsgCount : how many bytes of data in the second array of content
 * @param secondMsgContent : The remaining data to put into the log. 
 * @param interfaceType : Not used, this is a legacy argument. 
 *
 * @return DAL_SUCCESS if no problems encountered. 
 */
ULogResult ULogCore_STMWrite(ULogHandle h,
                           uint32 firstMsgCount,
                           const char * firstMsgContent,
                           uint32 secondMsgCount,
                           const char * secondMsgContent,
                           ULOG_INTERFACE_TYPE interfaceType)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (interfaceType==ULOG_INTERFACE_RAW){
    if (firstMsgCount < 4) {
      // There should be at least a format word.
      // If the data size is smaller than it should be, something is wrong,
      // return an error.
      return DAL_ERROR;
    }
  } else {
    if (firstMsgCount < (4 + ULOG_TIMESTAMP_SIZE)) {
      // There should be at least a format and a timestamp.
      // If the data size is smaller than it should be, something is wrong,
      // return an error.
      return DAL_ERROR;
    }
  }

  if (log->protocol_num > 127){
#ifndef DEFEATURE_ULOG_MSG_FORMAT
    //ASCII QDSS Output
    char print_buffer[256];
    ULOG_READ_TO_ASCII_DATA parse_data;
    print_buffer[0] = 0; //NULL terminate

    ULogInternal_LockGet(log);

    //fill in the information needed to parse the message
    parse_data.msgpart1=(char*)firstMsgContent;
    parse_data.msgpart2=(char*)secondMsgContent;
    parse_data.part1_size=firstMsgCount;
    parse_data.part2_size=secondMsgCount;
    parse_data.outputsz=print_buffer;
    parse_data.outputsz_size=256;
    parse_data.outOffset = 0;

    //parse it
    ULogParsers_FormatParseData(&parse_data, 0);

    //send out the ascii
    tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle), TRACER_OPT_GUARANTEE|TRACER_OPT_TIMESTAMP, 0, print_buffer);

    ULogInternal_LockRelease(log);
#endif /* DEFEATURE_ULOG_MSG_FORMAT */

  } else {
    //Binary QDSS Output
    ULogInternal_LockGet(log);

	if (interfaceType!=ULOG_INTERFACE_RAW){
#ifndef ULOG_64BIT_TIME
    //The expectation is that almost all ULOG over QDSS logs will use 64bit TS,
    //however, if the system uses 32 bit timestamps we or in a 0x100 to the
    //format string.
    (*(unsigned int *) firstMsgContent) |= ULOG_QDSS_32TS_FLAG;
#endif /* ULOG_64BIT_TIME */
    }
	
    //put the message size into the format/len word
    (*(unsigned int *) firstMsgContent) |= (((firstMsgCount + secondMsgCount)<<ULOG_LENGTH_SHIFT) & ULOG_LENGTH_MASK);

    tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle), TRACER_OPT_GUARANTEE|TRACER_OPT_TIMESTAMP, firstMsgCount, (const char*)firstMsgContent);
    if (secondMsgCount!=0)
    {
      tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle), TRACER_OPT_GUARANTEE, secondMsgCount, (const char*)secondMsgContent);
    }

    ULogInternal_LockRelease(log);
  }
  return(DAL_SUCCESS);
}
#endif /* DEFEATURE_ULOG_STM */


/**
 * <!-- ULogCore_MultipartMsgBegin -->
 * 
 * @brief Lock the log, set bookkeeping pointers, and write initial data in a new MPM msg.
 *
 * @param h : A ULog handle.
 *
 * @return TRUE if log is locked and MPM can continue. 
 */
DALBOOL ULogCore_MultipartMsgBegin(ULogHandle h)
{
  uint32 logData[3];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  //size/fmt + TS
  uint32 msgCount = 4 + ULOG_TIMESTAMP_SIZE;  //depends on the timestamp size. size/fmt + TS

   if (log == NULL)
   {
      return(FALSE);
   }

  // Get the log lock for the MPM
  // Insert a start of MPM message into the log.
  // Keep the length of that message as 0, but
  // move the pointers forward.
  // Log is again modified when the mpm ends.
  ULogInternal_LockGet(log);

  if ((log->sharedHeader == NULL) ||
      !(log->sharedHeader->logStatus & ULOG_STATUS_ENABLED) ||
      !(log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)
     )
  {
    //release the MPM lock
    ULogInternal_LockRelease(log);
    return(FALSE);
  }

  // If the message is too large for the log buffer, just drop it.
  if (msgCount >= log->bufSize)
  {
    log->writerCore->usageData |= ULOG_ERR_LARGEMSG;
    //release the MPM lock
    ULogInternal_LockRelease(log);
    return(FALSE);
  }

  // Prepare for the transaction
  log->transactionWriteCount = 0;
  log->transactionWriteStart = log->writerCore->write;

  //check the write and readWriter for available space, free space if needed.
  if (ULogInternal_FreeSpaceForNewMsg(log, msgCount)==FALSE){
    ULogInternal_LockRelease(log);
    return(FALSE);
  }

  // The readWriter pointer must be correct in memory before we
  // write to the buffer so the reader does not incorrectly read
  // a message as it is being overwritten.
  //
  // The message length must also be set to zero before proceeding
  // to stop a read of a bogus message.
  CORE_MEM_BARRIER();

  log->logStatus |= ULOG_STATUS_MPM_ACTIVE;
  log->mpmLengthLocation = log->transactionWriteStart;

  logData[0] = ULOG_REALTIME_SUBTYPE_MULTIPART;      // Subformat ID for MPM  (size is zero at start of MPM)

#ifdef ULOG_64BIT_TIME

  if(log->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(log))
  {
	  timestamp = log->altTimestampSrcFn();
  }
  else
  {
	timestamp = CoreTimetick_Get64();
  }
 
  logData[1] = (uint32)timestamp;
  logData[2] = (uint32)((uint64)timestamp>>32);
  ULogInternal_WriteData(log, 12, (char *)logData);
#else
  logData[1] = CoreTimetick_Get();
  ULogInternal_WriteData(log, 8, (char *)logData);
#endif

  CORE_MEM_BARRIER();

  // Retain the MPM lock until ULogCore_MultipartMsgEnd is called.
  return(TRUE);
}



#ifndef DEFEATURE_ULOG_TEE
/**
 * <!-- ULogCore_TeeMultipartMsgBegin -->
 *
 * @brief Walk the list of logs beginning an MPM on each one. 
 *
 * @param h : A Tee ULog handle.
 *
 * @return Always returns TRUE. 
 */
DALBOOL ULogCore_TeeMultipartMsgBegin(ULogHandle h)
{
  ULOG_TEE_LINK_TYPE * link;
  ULOG_CORE_VTABLE * core;
  ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)h;
  link = tee->firstEnabled;

  while (link != NULL)
  {
    //Use the core function table to call the correct MultipartMsgBegin function.
    if(ULogCore_IsEnabled(link->log, &core) == TRUE)
    {
      core->multipartMsgBegin(link->log);
    }
    link = link->next;
  }
  return TRUE;
}
#endif  /* DEFEATURE_ULOG_TEE */



#ifndef DEFEATURE_ULOG_STM
/**
 * <!-- ULogCore_STMMultipartMsgBegin -->
 *
 * @brief Send out a packet indicating that a MPM is beginning.
 *
 * @param h : A ULog handle.
 *
 * @return Always returns TRUE. 
 */
DALBOOL ULogCore_STMMultipartMsgBegin(ULogHandle h)
{
  unsigned int value;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if ((log->logStatus & ULOG_STATUS_MPM_ACTIVE) != ULOG_STATUS_MPM_ACTIVE)
  {
    log->logStatus |= ULOG_STATUS_MPM_ACTIVE;
    //get the MPM lock, this is released at the end of the MPM
    ULogInternal_LockGet(log);

    if (log->protocol_num > 127){
      //ASCII QDSS Output
      tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle),
                     TRACER_OPT_GUARANTEE|TRACER_OPT_TIMESTAMP,
                     0,
                     "MPM Begin");
    } else {
      //Binary QDSS Output
      value = ULOG_REALTIME_SUBTYPE_MULTIPART | (((4)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK);
      tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle),
                     TRACER_OPT_GUARANTEE|TRACER_OPT_TIMESTAMP,
                     4,
                     (const char*)&value);
    }
    return(TRUE);
  }
  return(FALSE);
}
#endif /* DEFEATURE_ULOG_STM */


/**
 * <!-- ULogCore_MultipartMsgEnd -->
 *
 * @brief Write the final size into the beginning of the message and release the lock. 
 *
 * @param h : A ULog handle.
 */
void ULogCore_MultipartMsgEnd(ULogHandle h)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 *wordPtr;

  // Update the mpm start message to include all log messages
  // between the start and now.
  if (log->logStatus & ULOG_STATUS_MPM_ACTIVE)
  {
    uint32 length = log->writerCore->write - log->mpmLengthLocation;
    wordPtr = (uint32 *)&log->buffer[log->mpmLengthLocation & log->bufSizeMask];
    *wordPtr |= ((length << ULOG_LENGTH_SHIFT) & ULOG_LENGTH_MASK);
    log->logStatus &= ~ULOG_STATUS_MPM_ACTIVE;
    ULogInternal_LockRelease(log);  //release the MPM lock
  }
}


#ifndef DEFEATURE_ULOG_TEE
/**
 * <!-- ULogCore_TeeMultipartMsgEnd -->
 *
 * @brief Call MultipartMsgEnd on all the logs in a Tee. 
 *
 * @param h : A ULog handle.
 */
void ULogCore_TeeMultipartMsgEnd(ULogHandle h)
{
  ULOG_TEE_LINK_TYPE * link;
  ULOG_CORE_VTABLE * core;
  ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)h;
  link = tee->firstEnabled;
  while (link != NULL)
  {
    //Use the core function table to call the correct MultipartMsgEnd function.
    if(ULogCore_IsEnabled(link->log, &core) == TRUE)
    {
      core->multipartMsgEnd(link->log);
    }
    link = link->next;
  }
}
#endif /* DEFEATURE_ULOG_TEE */



#ifndef DEFEATURE_ULOG_STM
/**
 * <!-- ULogCore_STMMultipartMsgEnd -->
 *
 * @brief Send out a packet indicating a MPM is ending. 
 *
 * @param h : A ULog handle.
 */
void ULogCore_STMMultipartMsgEnd(ULogHandle h)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  unsigned int value;

  if (log->logStatus & ULOG_STATUS_MPM_ACTIVE)
  {
    log->logStatus &= ~ULOG_STATUS_MPM_ACTIVE;
    if (log->protocol_num > 127){
      //ASCII QDSS Output
      tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle), TRACER_OPT_GUARANTEE|TRACER_OPT_TIMESTAMP, 0, "MPM End");
      ULogInternal_LockRelease(log);
    } else {
      //Binary QDSS Output
      value = ULOG_REALTIME_SUBTYPE_MULTIPART_STREAM_END | (((4)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK);
      tracer_data((tracer_client_handle_t)(log->stmTracerClientHandle),
                     TRACER_OPT_GUARANTEE|TRACER_OPT_TIMESTAMP,
                     4,
                     (const char*)&value);
      ULogInternal_LockRelease(log);
    }
  }
}
#endif /* DEFEATURE_ULOG_STM */


/**
 * <!-- ULogCore_ResetLog -->
 *
 * @brief Reset the log and record that a reset has occurred.
 *
 * There should be VERY few practical reasons why production code should use
 * this function to empty out a ulog on a running device.  In the case of
 * a memory corrupted log this function is used.  It is also used for some ULog
 * test cases.
 * This function assumes the memory buffer, enabled/disabled status, and lock
 * state are all valid.
 *
 * @param h : The ulog handle of the log to reset. 
 *
 * @return DAL_SUCCESS after the log has been reset. 
 */
ULogResult ULogCore_ResetLog(ULogHandle h)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return DAL_ERROR;
  }

#ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log)){
     return DAL_ERROR;
  }
#endif

  ULogInternal_LockGet(log);

	//Set all the indexes back to zero and clear flags.
  log->writerCore->write=0;
  log->writerCore->readWriter=0;
  log->writerCore->usageData |= ULOG_ERR_RESET;
  log->readerCore->read=0;
  log->readerCore->readFlags=0;

  //Increment the resetCount so it's possible to see how many times reset has happened.
  log->resetCount = log->resetCount + 1;

  ULogInternal_LockRelease(log);

  return DAL_SUCCESS;
}


/**
 * <!-- ULogInternal_LockGet -->
 *
 * @brief Use the appropriate type of lock for the log. 
 *
 * @param log : Pointer to the ULog to lock.
 */
void ULogInternal_LockGet(ULOG_TYPE * log)
{
  switch(log->lockType)
  {
    case ULOG_LOCK_NONE:
      // Do nothing in this case.
      break;

    case ULOG_LOCK_OS:
    case ULOG_LOCK_INT:
      Core_MutexLock(log->lockHandle);
      break;
      
    case ULOG_LOCK_SPIN:
      // Unsupported at this time.
      break;

    case ULOG_LOCK_UINT32:
      // This enum value is never intended for actual use, but some compilers
      // will spit a warning if it not listed in the switch statement.
      break;
  }
}


/**
 * <!-- ULogInternal_LockRelease -->
 *
 * @brief Use the appropriate type of unlock for the log. 
 *
 * @param log : Pointer to a ULog.
 */
void ULogInternal_LockRelease(ULOG_TYPE * log)
{
  switch(log->lockType)
  {
    case ULOG_LOCK_NONE:
      // Do nothing in this case.
      break;

    case ULOG_LOCK_OS:
    case ULOG_LOCK_INT:
      Core_MutexUnlock(log->lockHandle);
      break;

    case ULOG_LOCK_SPIN:
      // Unsupported at this time.
      break;

    case ULOG_LOCK_UINT32:
      // This enum value is never intended for actual use, but some compilers
      // will spit a warning if it not listed in the switch statement.
      break;
  }
}


/**
 * <!-- ULogInternal_ConvertSize -->
 *
 * @brief Calculate/check the size is a power of 2.
 * 
 * Ulog sizes are always a power of 2 rounding down if necessary. 
 * ULOG_MINIMUM_LOG_SIZE bytes is the minimum allowed.
 *
 * @param The size in bytes to check and modify if needed.
 */
uint32 ULogInternal_ConvertSize(uint32 size)
{
  uint32 output;

  if (size < ULOG_MINIMUM_LOG_SIZE)
  {
    // Require logs to be at least 64 bytes long.
    size = ULOG_MINIMUM_LOG_SIZE;
  }

  output = 0;
  while(size != 1)
  {
    size >>= 1;
    output++;
  }
  output = 1 << output;

  return(output);
}


/**
 * <!-- ULogInternal_Malloc -->
 *
 * @brief Call the appropriate type of malloc for the provided type. 
 *
 * @param type : The ULOG_ALLOC_TYPE to determine how to allocate.
 * @param mem :  Pointer to a pointer to return the allocated memory. 
 * @param size : How much to allocate. 
 *
 * @return DAL_SUCCESS if the allocation succeeded. 
 */
ULogResult ULogInternal_Malloc(ULOG_ALLOC_TYPE type, void ** mem, uint32 size)
{
  ULogResult result = DAL_SUCCESS;

  switch(type & ULOG_ALLOC_TYPE_MASK)
  {
    case ULOG_ALLOC_MANUAL:
      // Do not allocate memory in this case!
      *mem = 0;
      result = DAL_ERROR;
      break;

    case ULOG_ALLOC_LOCAL:
      result = DALSYS_Malloc(size, mem);
      break;

    default:
      *mem = 0;
      result = DAL_ERROR;
      break;
  }
  return(result);
}


/**
 * <!-- ULogInternal_NameVerify -->
 *
 * @brief Verify the name is a valid log name.
 *
 * @param logName : String pointer to check. 
 *
 * @return DAL_SUCCESS if the name is reasonable. 
 */
ULogResult ULogInternal_NameVerify(const char *logName)
{
  unsigned int name_size;
  name_size = strlen(logName);

  if ((name_size > ULOG_MAX_NAME_SIZE) || (name_size==0))
  {
    // The string is invalid.
    return ULOG_ERR_INVALIDNAME;
  }

  return DAL_SUCCESS;
}


/**
 * <!-- ULogCore_SetTimestampSrcFn -->
 *
 * @brief Allows the source of the timestamp to be altered by the user
 *
 * This function will reset the timestamp source to its default if the 
 * user passes in NULL for the 2nd parameter
 *
 * @param h : A ULog handle.
 * @param altULogTimeStampFn : Fn pointer to the users timestamp source fn
 *
 * @return DAL_SUCCESS if the Timestamp function was correctly set
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_SetTimestampSrcFn(ULogHandle h, ULOG_ALT_TS_SRC altULogTimeStampFn)
{
	//return error if NULL or Tee handle is passed in
	if(h == NULL || HANDLE_IS_A_TEE((ULOG_TYPE*)h))
	{
		return ULOG_ERR_INVALIDHANDLE;
	}

	ULogInternal_LockGet(((ULOG_TYPE*)h));
	((ULOG_TYPE*)h)->altTimestampSrcFn = altULogTimeStampFn;
	ULogInternal_LockRelease(((ULOG_TYPE*)h));

	return DAL_SUCCESS;
}


/**
 * <!-- ULogCore_SetTransportToRAM -->
 *
 * @brief Write log messages to the circular RAM buffer.
 * 
 * Writing messages to the circular RAM buffer is the default behavior for logs. 
 *
 * @param h : A ULog handle
 *
 * @return DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 */
ULogResult ULogCore_SetTransportToRAM(ULogHandle h){

  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if(log==NULL){
    return ULOG_ERR_INVALIDHANDLE;
  }

  ULogInternal_LockGet(log);
#ifndef DEFEATURE_ULOG_STM
  //deregister if leaving STM mode
  if (log->transport == ULOG_TRANSPORT_STM){
    if (log->stmTracerClientHandle!=NULL){
      tracer_unregister((tracer_client_handle_t*)&(log->stmTracerClientHandle));
      log->stmTracerClientHandle = NULL;
    }
  }
#endif

  log->transport = ULOG_TRANSPORT_RAM;
  ULogInternal_LockRelease(log);

  return DAL_SUCCESS;
}


/**
 * ULogCore_SetTransportToStm
 *
 * @brief Write log messages to the QDSS/STM transport
 *
 * @param h: A ULog handle
 * @param protocol_num: A QDSS protocol number (0 to 127) is assigned to the 
 *                     ULog handle and this protocol number can be used to
 *                     match logs to the handle when reading the QDSS output.
 *
 * @return DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 */
ULogResult ULogCore_SetTransportToStm( ULogHandle h,
                                       unsigned char protocol_num )
{
  DALResult result = ULOG_ERR_NOTSUPPORTED;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if(log==NULL){
    return ULOG_ERR_INVALIDHANDLE;
  }

  //ensure a valid protocol
  if (protocol_num > 127){
     return ULOG_ERR_INVALIDPARAMETER;
  }

#ifndef DEFEATURE_ULOG_STM
  //if it's already enabled, return an error
  if ((log->transport == ULOG_TRANSPORT_STM) && (log->protocol_num <= 127)){
     return ULOG_ERR_ALREADYENABLED;
  }

  //if it's already enabled, but it's in the other QDSS ascii mode
  if ((log->transport == ULOG_TRANSPORT_STM) && (log->protocol_num > 127)){
    //the log is currently doing the ascii QDSS and a binary format is being requested
    //unregister the current handle and continue on to register the new handle
    if (log->stmTracerClientHandle!=NULL){
      tracer_unregister((tracer_client_handle_t*)&(log->stmTracerClientHandle));
      log->stmTracerClientHandle = NULL;
    }
  }


  //If this log doesn't have a tracer handle, get one.
  if (log->stmTracerClientHandle==NULL){
    if (TRACER_RETURN_SUCCESS == tracer_register((tracer_client_handle_t*)&(log->stmTracerClientHandle), TRACER_ENTITY_ULOG, protocol_num)){
      log->transport = ULOG_TRANSPORT_STM;
      log->protocol_num = protocol_num;
      result = DAL_SUCCESS;
    } else {
      result = DAL_ERROR;
    }
  }
#endif

  return result;
}


/**
 * <!-- ULogCore_SetTransportToStmAscii -->
 *
 * @brief Write log messages as ASCII to the QDSS/STM transport
 *
 * Ascii QDSS is recorded in the log header as a protocol number > 127, however,
 * to keep the API simple and consistent with the other ULogCore_SetTransportToStm
 * API we take numbers 0 to 127 as an input and add 128 to it ourselves.
 *
 * @param h : A ULog handle
 * @param protocol_num : A QDSS protocol number (0 to 127) is assigned to the
 *                       ULog handle and can be used to identify log output when
 *                       reading the QDSS output.
 *
 * @return DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 */
ULogResult ULogCore_SetTransportToStmAscii( ULogHandle h,
                                            unsigned char protocol_num )
{
  DALResult result = ULOG_ERR_NOTSUPPORTED;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if(log==NULL){
    return ULOG_ERR_INVALIDHANDLE;
  }

  if (protocol_num > 127){
     return ULOG_ERR_INVALIDPARAMETER;
  }

#ifndef DEFEATURE_ULOG_STM

#ifndef DEFEATURE_ULOG_MSG_FORMAT

  //if it's already enabled, return an error
  if ((log->transport == ULOG_TRANSPORT_STM) && (log->protocol_num > 127)){
     return ULOG_ERR_ALREADYENABLED;
  }

  if ((log->transport == ULOG_TRANSPORT_STM) && (log->protocol_num <= 127)){
    //the log is currently doing the binary QDSS and the ascii format is being requested
    //unregister the current handle and continue on to register the new handle
    if (log->stmTracerClientHandle!=NULL){
      tracer_unregister((tracer_client_handle_t*)&(log->stmTracerClientHandle));
      log->stmTracerClientHandle = NULL;
    }
  }


  //If this log doesn't have a tracer handle, get one.
  if (log->stmTracerClientHandle==NULL){
    if (TRACER_RETURN_SUCCESS == tracer_register((tracer_client_handle_t*)&(log->stmTracerClientHandle), TRACER_ENTITY_ULOG, (protocol_num + 128))){
	    log->transport = ULOG_TRANSPORT_STM;
      log->protocol_num = protocol_num + 128;   //128 to 255 are ascii logs
      result = DAL_SUCCESS;
    } else {
      result = DAL_ERROR;
    }
  }
#endif

#endif

  return result;
}


/**
 * <!-- ULogCore_SetTransportToAlt -->
 *
 * @brief Write log messages using the provided handlers
 *
 * @param h : A ULog handle
 * @param newTansportVTable : A Write, MultipartStart and MultipartEnd function table.
 *
 * @return DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 */
ULogResult ULogCore_SetTransportToAlt( ULogHandle h,
                                       ULOG_CORE_VTABLE* newTansportVTable)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if((log==NULL) || (newTansportVTable==NULL)){
    return DAL_ERROR;
  }

  ULogInternal_LockGet(log);

#ifndef DEFEATURE_ULOG_STM
  //deregister if leaving STM mode
  if (log->transport == ULOG_TRANSPORT_STM){
    if (log->stmTracerClientHandle!=NULL){
      tracer_unregister((tracer_client_handle_t*)&(log->stmTracerClientHandle));
      log->stmTracerClientHandle = NULL;
    }
  }
#endif

  log->altTransportVTable = newTansportVTable;

  log->transport = ULOG_TRANSPORT_ALTERNATE;

  ULogInternal_LockRelease(log);

  return DAL_SUCCESS;
}
#endif //DEFEATURE_ULOG_CORE


#ifndef DEFEATURE_ULOG_LIST
/**
 * <!-- ULogCore_ListEx -->
 *
 * @brief List the available logs.
 *
 * Create a list of all logs available in the system, and output
 * it as a text string.  Offset into the list by the provided
 * amount.
 * 
 * @param offsetCount : The number of log names to skip before returning log name data.
 * @param registeredCount : The number of logs that are registered.
 * @param nameArraySize : The size of the array that receives log names.
 * @param namesReadCount : The number of log names that were actually placed
 *                         in the nameArray.
 * @param nameArray : The buffer to receive log name data.
 *
 * @return DAL_SUCCESS if the list was successful. Errors are defined in
 *        the ULOG_ERRORS enum.
 */
ULogResult ULogCore_ListEx(uint32 offsetCount, uint32 *  registeredCount,
                           uint32  nameArraySize,  uint32 *  namesReadCount,
                           char *  nameArray)
{
  ULOG_TYPE * log;
  uint32 outputBytes;
  uint32 curr_name_length;
  DALBOOL nameArray_is_full;

  //check the input pointers aren't NULL and the char buffer isn't zero.
  if ((registeredCount==NULL) || (namesReadCount==NULL) ||
                   (nameArray==NULL) || (nameArraySize==0))
  {
    return ULOG_ERR_INVALIDPARAMETER;
  }

  *registeredCount = 0;
  *namesReadCount = 0;
  outputBytes = 0;
  nameArray_is_full = FALSE;
  nameArray[0] = 0;  // Terminate the string in case we return without adding names.

  log = ulogContext.logHead;

  // Skip the number of logs requested skipped.
  while ((offsetCount) && (log != NULL))
  {
    (*registeredCount)++;
    log = log->next;
    offsetCount--;
  }

  // Traverse the remaining log list, copying as many log names as possible
  // into the output nameArray. If there isn't any more space in nameArray
  // we still traverse to the end of the linked list to count the total
  // number of known logs into registeredCount.
  while (log != NULL)
  {
    //Count the number of logs
    (*registeredCount)++;

    //If we haven't filled up the available space in the nameArray, try to add to it.
    if (nameArray_is_full == FALSE)
    {
      curr_name_length = strlen(log->name) + 1;

      //If there's space, add the name, if a name won't fit then we won't try to add any more
      if (nameArraySize >= (outputBytes + curr_name_length))
      {
        core_strlcpy(&(nameArray[outputBytes]), log->name, curr_name_length);
        outputBytes += curr_name_length;
        (*namesReadCount)++;
      } else {
        //Typically the nameArray provided would be at least ULOG_MAX_NAME_SIZE+1
        //so this error should not occur, but if a "too small" nameArray is provided
        //and not even a single log name can be copied into it, return an error.
        if (*namesReadCount==0){
          return ULOG_ERR_INSUFFICIENT_BUFFER;
        } else {
          //We put in at least one name, but couldn't fit in the next one.
          //Consider the nameArray full so we don't try to incorrectly
          //squeeze in later names.
          nameArray_is_full = TRUE;
        }
      }
    }

    log = log->next;
  }

   return DAL_SUCCESS;
}


/**
 * <!-- ULogCore_List -->
 *
 * @brief List of all logs available in the system, output as a text string.
 *
 * @param registeredCount : The number of logs that are registered.
 * @param nameArraySize : The size of the array that receives log names.
 * @param namesReadCount : The number of log names that were actually placed
 *                         in the nameArray.
 * @param nameArray : The buffer to receive log name data.
 *
 * @return DAL_SUCCESS if the list was successful. Errors are defined in
 *        the ULOG_ERRORS enum.
 */
ULogResult ULogCore_List( uint32* registeredCount, uint32 nameArraySize,
                          uint32* namesReadCount, char* nameArray )
{
  return (ULogCore_ListEx(0, registeredCount, nameArraySize, namesReadCount, nameArray));
}

#endif // DEFEATURE_ULOG_LIST



#ifndef DEFEATURE_ULOG_ALLOCATE

/**
 * <!-- ULogResult ULogInternal_FreeMem -->
 */
ULogResult ULogInternal_FreeMem(ULOG_ALLOC_TYPE type, void * mem)
{
  DALResult result = DAL_SUCCESS;

  if (mem == NULL) {
    return (DAL_ERROR);
  }

  switch(type & ULOG_ALLOC_TYPE_MASK)
  {
    case ULOG_ALLOC_MANUAL:
      // Didn't allocate this.  Can't free it.
      result = DAL_ERROR;
      break;

    case ULOG_ALLOC_LOCAL:
      result = DALSYS_Free(mem);
      break;

    default:
      result = DAL_ERROR;
      break;
  }
  return(result);
}


/**
 * <!-- ULogCore_Allocate -->
 *
 * @brief Allocate memory to the log buffer.
 *
 * When a log is created with no output buffer, this function can
 * be used to allocate that memory.  It is intended for use from
 * applications enabling and reading logs from the system.
 *
 * This only works for logs created with ULOG_ALLOC_LOCAL
 * allocation type.
 *
 * @param h : A ULog handle of the log to read.
 * @param bufferSize : Size of the circular buffer to allocate.
 *
 * @return DAL_SUCCESS if the memory was allocated successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Allocate(ULogHandle h, uint32 bufferSize)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  DALResult result = DAL_ERROR;
  uint32 adjustedSize;
  uint32 oldEnableState;

  if (log == NULL)
  {
    return(DAL_ERROR);
  }
#ifndef DEFEATURE_ULOG_TEE
  else if (HANDLE_IS_A_TEE(log))
  {
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif
  // We can only allocate memory if certain conditions are met.
  //
  else if ((log->bufMemType & ULOG_ALLOC_MANUAL) ||
      ((log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE) == 0) ||
      (log->bufMemType & ULOG_MEMORY_CONFIG_SHARED))
  {
    // Don't allocate...
    return(result);
  }

  Core_MutexLock(ulogContext.gLockHandle);

  adjustedSize = ULogInternal_ConvertSize(bufferSize);

  // Store the requested size in case the log is not yet defined.
  log->requestedSize = adjustedSize;

  // Only allocate memory if:
  // 1. The log exists
  // 2. There is a lock
  // 3. Memory size is not already allocated

  if (((log->lockHandle != NULL) || (log->buffer == NULL)) &&
      (log->bufSize != adjustedSize) &&
      (log->logStatus & ULOG_STATUS_DEFINED))
  {
    oldEnableState = log->sharedHeader->logStatus & ULOG_STATUS_ENABLED;
    ULogCore_Disable(h);

    // Lock the log.
    if (log->lockHandle != NULL)
    {
      ULogInternal_LockGet(log);
    }

    // No need to disable the log since we have it locked.
    // Remove the old buffer.
    if (log->buffer)
    {
      ULogInternal_FreeMem((ULOG_ALLOC_TYPE)log->bufMemType, log->buffer);
      log->buffer = NULL;
    }

    // Assign the new buffer.  We know the old one is NULL at this point.
    log->bufSizeMask = 0;
    log->bufSize = adjustedSize;
    if (log->bufSize)
    {
      log->bufSizeMask = log->bufSize - 1;
      if (ULogInternal_Malloc((ULOG_ALLOC_TYPE)log->bufMemType, (void **)&log->buffer, log->bufSize)!=DAL_SUCCESS)
      {
        ULogCore_Disable(h);
        log->logStatus |= ULOG_ERR_MALLOC;
      }
    }

    if (oldEnableState)
    {
      ULogCore_Enable(h);
    }

    // Clear out the core logging variables before we
    // enable the log.
    //
    memset(log->readerCore, 0, sizeof(ULOG_READER_CORE_TYPE));
    memset(log->writerCore, 0, sizeof(ULOG_WRITER_CORE_TYPE));

    // Unlock the log.
    if (log->lockHandle != NULL)
    {
      ULogInternal_LockRelease(log);
    }

    result = DAL_SUCCESS;
  }

  Core_MutexUnlock(ulogContext.gLockHandle);

  return (result);
}


/**
 * <!-- ULogCore_Disable -->
 *
 * @brief Disable a log. Drop message writes until re-enabled.
 *
 * @param h : A ULog handle.
 *
 * @return DAL_SUCCESS if the log was disabled successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Disable(ULogHandle h)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  DALResult result = DAL_ERROR;

  if (log == NULL)
  {
    return(DAL_ERROR);
  }
#ifndef DEFEATURE_ULOG_TEE
  else if (HANDLE_IS_A_TEE(log))
  {
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif

  Core_MutexLock(ulogContext.gLockHandle);

  if ((log->sharedHeader != NULL) &&
      (log->sharedHeader->logStatus & ULOG_STATUS_ENABLED) &&
      (log->sharedHeader->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE))
  {
    // Lock the log.
    if (log->lockHandle != NULL)
    {
      ULogInternal_LockGet(log);
    }

    log->sharedHeader->logStatus &= ~ULOG_STATUS_ENABLED;

    // We could free the buffer if it was allocated at this point,
    // but think it would cause more problems that it would solve.
    // It is not expected that these logs will be enabled and disabled
    // in a production environment, unless it was in a diagnostic
    // manner.

    // Unlock the log.
    if (log->lockHandle != NULL)
    {
      ULogInternal_LockRelease(log);
    }
    result = DAL_SUCCESS;
  }

  Core_MutexUnlock(ulogContext.gLockHandle);
  return (result);
}

#endif // DEFEATURE_ULOG_ALLOCATE


#ifndef DEFEATURE_ULOG_TEE
/**
 * <!-- ULogCore_TeeHandleSearch -->
 *
 * @brief Search a log handle in a tee, if it is in the tee, return TRUE
 *
 * @param tee : Pointer to ULOG_TEE_TYPE to be searched. 
 * @param log : Pointer to a ULog.
 */
static DALBOOL ULogCore_TeeHandleSearch(ULOG_TEE_TYPE *tee, ULOG_TYPE *log)
{
  ULOG_TEE_LINK_TYPE *link;

  if((NULL == tee) || (NULL == log))
  {
    return FALSE;
  }
  link = tee->link;

  while(NULL != link)
  {
    if(link->log == log)
    {
      return TRUE;
    }
    link = link->next;
  }

  return FALSE;
}


/**
 * <!-- ULogCore_IsLogPresent -->
 *
 * @brief Check if a named log is present in a log set
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 * @param logName : Ascii name of a ULog
 *
 * @return TRUE if the named log is present in the log(s) indicated by the handle
 *        h, and FALSE otherwise
 */
DALBOOL ULogCore_IsLogPresent(ULogHandle h, char *logName)
{
  ULogHandle logHdl;

  if(NULL == h)
  {
    return FALSE;
  }
  if(NULL == (logHdl = ULogCore_HandleGet(logName)))
  {
    return FALSE;
  }

  // this is a log handle
  if(!HANDLE_IS_A_TEE(((ULOG_TYPE *)h)))
  {
    if(h == logHdl)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  // this is a tee handle
  else
  {
    return ULogCore_TeeHandleSearch((ULOG_TEE_TYPE *)h, (ULOG_TYPE *)logHdl);
  }
}


/**
 * <!-- ULogCore_TeeLinkAdd -->
 *
 * Create a TEE_LINK_TYPE to hold the pointer to the log, put it into the
 * tee's linked list. We order the linked list by the log's memory address so
 * that everyone locks logs in the same order and deadlocks in multipart messages
 * are prevented.
 */
static ULogHandle ULogCore_TeeLinkAdd(ULOG_TEE_TYPE *tee, ULOG_TYPE *newlog)
{
  ULOG_TEE_LINK_TYPE *newlink;
  ULOG_TEE_LINK_TYPE *previouslink;
  ULOG_TEE_LINK_TYPE *templink;

  if((tee == NULL) || (newlog == NULL))
  {
    return tee;
  }

  //create a new tee link
  CORE_VERIFY_PTR(newlink = (ULOG_TEE_LINK_TYPE *)Core_PoolAlloc(ulogContext.teeLinkPool));

  //fill in the new tee link's log pointer to point to the log we want to add
  newlink->log = newlog;

  // Order the linked list by memory address.

  //The simple case, no logs in the list yet.
  if (tee->link == NULL){
    //put the current start of the list (NULL) into the new link's next pointer
    newlink->next = tee->link;
    //make the new tee link the new start of the list
    tee->link = newlink;
    //update the number of links in the tee
    tee->linkSize++;
    return tee;
  }

  // The new log's address is less than the log in the first link of the linked
  // list. The new link gets placed on the start of the list.
  if((DALMemAddr)(newlink->log) < (DALMemAddr)(tee->link->log)){
    //put the current start of the list into the new link's next pointer
    newlink->next = tee->link;
    //make the new tee link the new start of the list
    tee->link = newlink;
    //update the number of links in the tee
    tee->linkSize++;
    return tee;
  }

  //walk the list and if we find our new log is less than the next one, insert it in the list
  templink = tee->link->next;
  previouslink = tee->link;
  while (templink != NULL)
  {
    if((DALMemAddr)newlink->log < (DALMemAddr)templink->log){
      //our new log's address is less than the templink's, so we insert before templink
      newlink->next = templink;
      previouslink->next = newlink;
      //update the number of links in the tee
      tee->linkSize++;
      return tee;
    }
    previouslink = templink;
    templink = templink->next;
  }

  //got to the end of the list, that means the new log should be placed at the end of the list.
  newlink->next = NULL;
  previouslink->next = newlink;
  //update the number of links in the tee
  tee->linkSize++;
  return tee;
}


/**
 * <!-- ULogCore_TeeAdd -->
 *
 * @brief Group handles together.
 *
 * Group handles together so that any logging information will be copied to
 * multiple logs.
 *
 * Note: the first handle may points to an updated object after a call to
 * this function, whereas the second one will not.
 *
 * @param h1 : A ULog handle, which may be one to a single or a group of logs
 * @param h2 : A ULog handle, which may be one to a single or a group of logs
 *             to be added to h1
 *
 * @return A handle to the grouped logs
 */
ULogHandle ULogCore_TeeAdd(ULogHandle h1, ULogHandle h2)
{
  uint32 versionFlag1, versionFlag2;
  ULOG_TEE_TYPE *teeLog1, *teeLog2;
  ULOG_TEE_LINK_TYPE *link;

  if(NULL == h1)
  {
    return h2;
  }
  if(NULL == h2)
  {
    return h1;
  }

  // Initialize the memory pool for tee handles and links
  if(NULL == ulogContext.teePool)
  {
    DALSYS_Malloc(sizeof(CorePoolType), (void **)&ulogContext.teePool);
    CORE_VERIFY(ulogContext.teePool);
    Core_PoolConfig(ulogContext.teePool, &ULogTeePoolConfig);

    DALSYS_Malloc(sizeof(CorePoolType), (void **)&ulogContext.teeLinkPool);
    CORE_VERIFY(ulogContext.teeLinkPool);
    Core_PoolConfig(ulogContext.teeLinkPool, &ULogTeeLinkPoolConfig);
  }

  versionFlag1 = HANDLE_IS_A_TEE((ULOG_TYPE *)h1);
  versionFlag2 = HANDLE_IS_A_TEE((ULOG_TYPE *)h2);

  // The first is a log handle
  if(0 == versionFlag1)
  {
    // The second is also a log handle and same as the first
    if((0 == versionFlag2) && (h1 == h2))
    {
      return h1;
    }

    // Create a new tee
    CORE_VERIFY_PTR(teeLog1 = (ULOG_TEE_TYPE *)Core_PoolAlloc(ulogContext.teePool));
    memset(teeLog1, 0, sizeof(ULOG_TEE_TYPE));
    teeLog1->version = ULOG_TYPE_TEE_HEADER_VERSION;
    teeLog1->lockHandle = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT);

    // Add the first handle to the new tee
    teeLog1 = (ULOG_TEE_TYPE*)ULogCore_TeeLinkAdd(teeLog1, (ULOG_TYPE*)h1);
  }
  // The first is a tee handle
  else
  {
    teeLog1 = (ULOG_TEE_TYPE * )h1;
  }

  // The second handle is a log one
  if(0 == versionFlag2)
  {
    Core_MutexLock(teeLog1->lockHandle);
    if(FALSE == ULogCore_TeeHandleSearch(teeLog1, (ULOG_TYPE*)h2))
    {
      teeLog1 = (ULOG_TEE_TYPE*)ULogCore_TeeLinkAdd(teeLog1, (ULOG_TYPE*)h2);
    }
    Core_MutexUnlock(teeLog1->lockHandle);
  }
  // The second handle is a tee one
  else
  {
    teeLog2 = (ULOG_TEE_TYPE * )h2;
    Core_MutexLock(teeLog1->lockHandle);
    Core_MutexLock(teeLog2->lockHandle);
    link = teeLog2->link;
    while(NULL != link)
    {
      // If the handle (link2->log) is not already in the first tee, link it
      if(FALSE == ULogCore_TeeHandleSearch(teeLog1, link->log))
      {
        ULogCore_TeeLinkAdd(teeLog1, link->log);
      }
      link = link->next;
    }
    Core_MutexUnlock(teeLog2->lockHandle);
    Core_MutexUnlock(teeLog1->lockHandle);
  }

  return teeLog1;
}


/**
 * <!-- ULogCore_TeeLinkRemove -->
 */
static ULogHandle ULogCore_TeeLinkRemove(ULOG_TEE_TYPE *tee, ULOG_TYPE *log)
{
  ULOG_TEE_LINK_TYPE *link1, *link2;

  if((NULL == tee) || (NULL == tee->link) || (NULL == log))
  {
    return tee;
  }
  if(tee->link->log == log)
  {
    // If the head link matches, update it to the next link in the list
    link1 = tee->link;
    tee->link = link1->next;
    tee->linkSize --;
    Core_PoolFree(ulogContext.teeLinkPool, link1);
  }
  else
  {
    link2 = tee->link;
    link1 = link2->next;

    while(NULL != link1)
    {
      if(link1->log == log)
      { // Remove the first occurrence - there should be only one.
        link2->next = link1->next;
        Core_PoolFree(ulogContext.teeLinkPool, link1);
        tee->linkSize --;
        break;
      }
      link1 = link1->next;
      link2 = link2->next;
    }
  }

  return tee;
}


/**
 * <!-- ULogCore_TeeRemove -->
 *
 * @brief Remove some handle(s)
 *
 * Note: the first handle may points to an updated object after a call to
 * this function, whereas the second one will not.
 *
 * @param h1 : A ULog handle, which may be one to a single or a group of logs
 * @param h2 : A ULog handle, which may be one to a single or a group of logs
 *             to be removed from h1
 *
 * @return A handle to the resulting group of logs
 */
ULogHandle ULogCore_TeeRemove(ULogHandle h1, ULogHandle h2)
{
  uint32 versionFlag1, versionFlag2;
  ULOG_TEE_LINK_TYPE *link;
  ULOG_TYPE *log;
  ULOG_TEE_TYPE *teeLog1, *teeLog2;

  if((NULL == h1) || (NULL == h2))
  {
    return h1;
  }

  // Since users are supposed to call TeeAdd before this function for a tee to
  // exist, the tee pool should already be initialized in TeeAdd call.
  // We should not return if it is not initialized, as users may remove a log
  // handle from itself. This is a valid operation, and TeeRemove will return
  // a NULL handle.

  versionFlag1 = HANDLE_IS_A_TEE((ULOG_TYPE *)h1);
  versionFlag2 = HANDLE_IS_A_TEE((ULOG_TYPE *)h2);

  // The first handle is not a tee
  if(0 == versionFlag1)
  {
    // The second handle is a log one, and same as the first
    if((0 == versionFlag2) && (h1 == h2))
    {
      return NULL;
    }
    // The second handle is a tee, and contains the first
    if(0 != versionFlag2)
    {
      DALBOOL result;
      teeLog2 = (ULOG_TEE_TYPE *)h2;
      Core_MutexLock(teeLog2->lockHandle);
      result = ULogCore_TeeHandleSearch(teeLog2, (ULOG_TYPE *)h1);
      Core_MutexUnlock(teeLog2->lockHandle);
      if(TRUE == result)
      {
        return NULL;
      }
    }
    // The second handle is different from or does not contain the first
    return h1;
  }

  // The first handle is a tee
  teeLog1 = (ULOG_TEE_TYPE *)h1;
  Core_MutexLock(teeLog1->lockHandle);

  if(0 == versionFlag2)
  {
    // The second handle is a log
    teeLog1 = (ULOG_TEE_TYPE*)ULogCore_TeeLinkRemove(teeLog1, (ULOG_TYPE *)h2);
  }
  else
  {
    // The second handle is a tee
    teeLog2 = (ULOG_TEE_TYPE * )h2;

    Core_MutexLock(teeLog2->lockHandle);
    link = teeLog2->link;
    while((NULL != teeLog1->link) && (NULL != link))
    {
      CORE_VERIFY_PTR(log = link->log);
      // We may remove a tee from itself, so update the link before it gets removed.
      link = link->next;
      teeLog1 = (ULOG_TEE_TYPE*)ULogCore_TeeLinkRemove(teeLog1, log);
    }
    Core_MutexUnlock(teeLog2->lockHandle);
  }

  // If tee1 is empty
  if(NULL == teeLog1->link)
  {
    Core_MutexUnlock(teeLog1->lockHandle);
    Core_MutexDestroy(teeLog1->lockHandle);
    Core_PoolFree(ulogContext.teePool, teeLog1);
    return NULL;
  }

  // If there is only one log left in tee1
  if(NULL == teeLog1->link->next)
  {
    log = teeLog1->link->log;
    Core_MutexUnlock(teeLog1->lockHandle);
    Core_MutexDestroy(teeLog1->lockHandle);
    Core_PoolFree(ulogContext.teeLinkPool, teeLog1->link);
    Core_PoolFree(ulogContext.teePool, teeLog1);
    return log;
  }

  Core_MutexUnlock(teeLog1->lockHandle);
  return teeLog1;
}

#endif // DEFEATURE_ULOG_TEE

#ifdef __cplusplus
}
#endif
