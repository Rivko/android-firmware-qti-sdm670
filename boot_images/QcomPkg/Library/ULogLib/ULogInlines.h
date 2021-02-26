/*============================================================================
@file ULogInlines.h

Inlines for some commonly used routines that should be need to be fast
and/or abstracted out to make the code easier to work with. 

Copyright (c) 2009-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef ULOG_INLINES_H
#define ULOG_INLINES_H

#include "ULogBuildFeatures.h"
#include "ULogCoreInternal.h"
#include "MemBarrier.h"
#include "CoreTime.h"
#include "CoreString.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * <!-- ULogFreeSpaceForNewMsg -->
 *
 * Checks whether the message we're about to write looks reasonable and makes 
 * space for our incoming message is necessary. 
 */
static ULOG_INLINE DALBOOL ULogInternal_FreeSpaceForNewMsg( ULOG_TYPE * log, unsigned int requestingSize )
{
  uint32 oldMsgSize;
  uint32 bufferNeeded;
  uint32 freeAttempts=0; 

  //Impossible to make space if the request amount is bigger than the log.
  if ( requestingSize > log->bufSize )
  {
    log->writerCore->usageData |= ULOG_ERR_LARGEMSG;
    return FALSE;
  }

  // Determine if there is enough free memory for the new message.
  // If not, remove old messages until there is enough room.
  //
  // In order to add the message::
  // Current message data in buffer = write - readWriter
  // current message data + new message <= Buffer Size
  bufferNeeded = (log->writerCore->write - log->writerCore->readWriter) + requestingSize;
  
  while (bufferNeeded > log->bufSize)
  {
    // Detect a wrap condition. We're about to move the readWriter
    // forwards to free up space. This will overwrite the oldest logs
    // that have not been read.
    // 99.99% of the time SW on the device isn't reading the logs so read==0,
    // and it's a 0 = 0 check that indicates the log has wrapped for the
    // first time.
    if (log->readerCore->read == log->writerCore->readWriter)
    {
      log->writerCore->usageData |= ULOG_LOG_WRAPPED;
    }

    // Remove the oldest message to free memory for the new message.
    oldMsgSize = ((*((uint32 *)&log->buffer[log->writerCore->readWriter & log->bufSizeMask]) & ULOG_LENGTH_MASK) >> ULOG_LENGTH_SHIFT);
    if (oldMsgSize == 0)
    {
      // The msg size is 0 length, which means we're not done writing that
      // message.  If this happens because some thread has died, this is going
      // to be unrecoverable. If this happens because a MPM has caught up to
      // it's start, the ULogFront_RealTimeMultipartMsgEnd of the MPM will
      // fill in this missing size later, and then normal messages can continue.
      // This throw away behavior may cause some unusual MPMs where some
      // messages are missing, but that would be a MPM that is really too large.
      return(FALSE);
    }
    else
    {
      oldMsgSize = ULOGINTERNAL_PADCOUNT(oldMsgSize);
      log->writerCore->readWriter += oldMsgSize;
      bufferNeeded -= oldMsgSize;

      // We've seen in memory corruption cases where this loop gets stuck
      // because there are corrupt logs and these pointers get corrupted.
      // If the log is ever in an impossible state.  Reset it.
      if ( (log->writerCore->write - log->writerCore->readWriter) > log->bufSize )
      {
        ULogCore_ResetLog((ULogHandle)log);
        bufferNeeded = requestingSize;
      }

      //Because making space for new logs relies on the integrity of
      //previous logs, there can be problems with this loops in rare
      //memory corruption cases.  This loop cannot run more than the
      //times needed to fit the largest ULOG_MAX_MSG_SIZE message.
      //There's no log msg smaller than 8 bytes so
      //ULOG_MAX_MSG_SIZE/8, so any attempt to make space more than
      //ULOG_MAX_MSG_SIZE/8 times should cause a log reset.
      freeAttempts++;

      if ((freeAttempts>ULOG_MAX_MSG_SIZE/8) ||
          (oldMsgSize > ULOG_MAX_MSG_SIZE) || 
           (oldMsgSize < 8) )
      {
        ULogCore_ResetLog((ULogHandle)log);
        bufferNeeded = requestingSize;
      }


    }
  }
  return TRUE;
}


/**
 * <!-- ULogInternal_ReadyForMessage -->
 *
 * Checks whether the message we're about to write looks reasonable and makes 
 * space for our incoming message is necessary. 
 */
static ULOG_INLINE DALBOOL ULogInternal_ReadyForMessage(ULOG_TYPE * log, unsigned int msgCount)
{
  uint32 requestingSize = ULOGINTERNAL_PADCOUNT(msgCount);

  if ((log->sharedHeader == NULL) ||
      ((log->sharedHeader->logStatus & ULOG_STATUS_ENABLED)!=ULOG_STATUS_ENABLED) ||
      ((log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)!=ULOG_MEMORY_USAGE_WRITEABLE) ||
      (requestingSize > ULOG_MAX_MSG_SIZE)
     )
  {
    return(FALSE);
  }
  
  if ( ULogInternal_FreeSpaceForNewMsg( log, requestingSize ) == FALSE )
  {
    return(FALSE);
  }

  // The readWriter pointer must be correct in memory before we
  // write to the buffer so the reader does not incorrectly read
  // a message as it is being overwritten.
  //
  // The message length must also be set to zero before proceeding
  // to stop a read of a bogus message.
  CORE_MEM_BARRIER();

  return TRUE;
}


/**
 * <!-- ULogInternal_UseFastRam -->
 *
 * The fastest logging occurs when we write directly to the log buffer.  This 
 * can only be done to normal RAM logs.  Tee'd or QDSS STM logs use a function 
 * pointer table to call a generic write.  This function returns TRUE if this
 * is a normal RAM log and we can use the fast writes directly to the buffer.
 */
#if  !defined (DEFEATURE_ULOG_TEE) || !defined (DEFEATURE_ULOG_STM)
  //If the other write methods are disabled no need to check whether we can do
  //fast writes direct to the buffer.  Keeps the lock if we can write do the fast ram writes next.
static ULOG_INLINE DALBOOL ULogInternal_UseFastRam(ULOG_TYPE * log)
{

#ifndef DEFEATURE_ULOG_TEE
    // Is this log a tee
    if (HANDLE_IS_A_TEE(log))
    {
      return FALSE;
    }
#endif 

  if ( log->transport != ULOG_TRANSPORT_RAM )
  {
      return FALSE;
  }

    return TRUE;
}

#endif


/**
 * <!-- ULogInternal_FastFmtSizeTS -->
 * //ULogInternal_PrepareMessageFmtSizeTS 
 *
 * All of the realtime frontend APIs that we want to use the faster writes 
 * directly to RAM need to do the same initial operations.  
 *  - See if the fast writes to ram can be done,
 *  - Get the lock
 *  - Check the size of the log, and make space if needed. 
 *  - Return one of three possible outcomes:
 *    1) Error, 2) Success 3) Can't use fast RAM writes on this log. 
 */
static ULOG_INLINE int ULogInternal_FastFmtSizeTS( ULogHandle h, uint32 size, uint32 msg_format )
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32* wordPtr;
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  // See if this log is an ordinary RAM log.
  // We don't need to do this check if other log types are off.
#if  !defined (DEFEATURE_ULOG_TEE) || !defined (DEFEATURE_ULOG_STM)
  //See if this is an ordinary RAM ULog.  We can do our fastest writes if it is.
  if (ULogInternal_UseFastRam(log))
#endif
  {
    ULogInternal_LockGet(log);

    if(ULogInternal_ReadyForMessage((ULOG_TYPE *)h, size)==FALSE)
    {
      ULogInternal_LockRelease(log);
      return(ULOG_FAST_TO_RAM_FAIL);
    }

    // len/fmt word
    wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
    //save this important index and size to use at the end of the write
    log->transactionWriteStart = log->writerCore->write;
    log->transactionWriteCount = size;
    *wordPtr = (msg_format & ULOG_FORMAT_MASK);  //keep the size zero until the msg is completely written.
                                                 //fill in the format string size bit
#ifdef ULOG_64BIT_PROC
    *wordPtr |= ULOG_FORMAT_STR_SIZE_BIT;
#else
    *wordPtr &= ~ULOG_FORMAT_STR_SIZE_BIT;
#endif
    log->writerCore->write+=4;

    //Timestamp
#ifdef ULOG_64BIT_TIME
	if(log->altTimestampSrcFn != NULL)
	{
	  timestamp = log->altTimestampSrcFn();
	}
	else
	{
      timestamp = CoreTimetick_Get64();
	}

    wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
    *wordPtr = (uint32)timestamp;
    log->writerCore->write+=4;

    wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
    *wordPtr = (uint32)((uint64)timestamp>>32);
    log->writerCore->write+=4;

#else
    wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
    *wordPtr = CoreTimetick_Get();
    log->writerCore->write+=4;
#endif
    return ULOG_FAST_TO_RAM_SUCCESS;
  }

  return ULOG_FAST_TO_RAM_UNAVAIL;
}


/**
 * <!-- ULogInternal_WriteData -->
 */
static ULOG_INLINE void ULogInternal_WriteData(ULOG_TYPE * log, uint32 msgCount, const char * msgContent)
{
  unsigned int padding=0;
  unsigned int bytesToEnd, copy1Count, copy2Count;

  log->transactionWriteCount += msgCount;

  bytesToEnd = log->bufSize - (log->writerCore->write & log->bufSizeMask);

  if (msgCount <= bytesToEnd)
  {
    core_memcpy( &(log->buffer[log->writerCore->write & log->bufSizeMask]), msgCount, msgContent, msgCount );
  }
  else
  {
    copy1Count = bytesToEnd;
    copy2Count = msgCount - bytesToEnd;
    core_memcpy( &log->buffer[log->writerCore->write & log->bufSizeMask], copy1Count, msgContent, copy1Count );
    core_memcpy( &log->buffer[0], copy2Count, &msgContent[copy1Count], copy2Count );
  }
  
  //make sure the write pointer ends on a word boundary
  if ((msgCount & 0x03) != 0)
  {
     padding = 4 - (msgCount & 0x03);
  }

  // Increment the write pointer
  log->writerCore->write += msgCount + padding;

  return;
}

/**
 * <!-- ULogInternal_GetTeeLocks -->
 * Inline function to Lock a ULog Tee.
 */
static ULOG_INLINE void ULogInternal_GetTeeLocks( ULogHandle h )
{
  ULOG_TEE_TYPE * teeLog = (ULOG_TEE_TYPE *)h;
  ULOG_TEE_LINK_TYPE * link = teeLog->firstEnabled;
  while ( link != NULL )
  {
    ULogInternal_LockGet( link->log );
    link = link->next;
  }
  return;
}

/**
 * <!-- _ULogInternal_ReleaseTeeLock -->
 * Inline function invoked recursively to unlock logs in a ULog Tee.
 * Recursion is neccessary to ensure locks are unlocked in a LIFO manner.
 */
static ULOG_INLINE void _ULogInternal_ReleaseTeeLock(ULOG_TEE_LINK_TYPE *link)
  {
  if (link == NULL) return;

  _ULogInternal_ReleaseTeeLock(link->next);
    ULogInternal_LockRelease( link->log );
}

/**
 * <!-- ULogInternal_ReleaseTeeLocks -->
 * Inline function to Release a ULog Tee.
 */
static ULOG_INLINE void ULogInternal_ReleaseTeeLocks( ULogHandle h )
{
  ULOG_TEE_TYPE * teeLog = (ULOG_TEE_TYPE *)h;
  _ULogInternal_ReleaseTeeLock(teeLog->firstEnabled);
}

/**
 * <!-- ULogInternal_CountBitsSet -->
 *
 * @brief Calculate the number of set bits in a 32-bit param
 *
 * @param bitField : The value used to determine the number of bits set
 *
 * @return The number of bits set in the input parameter
 *
 *
 */
static ULOG_INLINE uint32 ULogInternal_CountBitsSet( uint32 bitField )
{
  uint32 numSet = 0;
  while (bitField)
  {
    bitField &= (bitField-1);
    numSet++;
  }
  return numSet;
}

#ifdef __cplusplus
}
#endif

#endif /* ULOG_INLINES_H */
