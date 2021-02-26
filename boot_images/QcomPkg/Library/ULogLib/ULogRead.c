/*============================================================================
@file ULogRead.c

Log Read interface for ULog.

Copyright (c) 2009-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "ULogBuildFeatures.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFEATURE_ULOG_READ
#include "DALSys.h"
#include "CoreVerify.h"
#include "MemBarrier.h"
#include "ULog.h"
#include "ULogCoreInternal.h"
#include "CoreString.h"


/**
 * <!-- ReadIsValid -->
 *
 * @brief Is the read pointer somewhere between the current beginning and end of the log? 
 *
 * @param log : Pointer to the ULog to check.
 *
 * @return TRUE (1) if the read pointer is valid, FALSE (0) if not. 
 */
static uint32 ReadIsValid(ULOG_TYPE * log)
{
  CORE_MEM_BARRIER();

  // Return false if the read pointer required update.
  //if (((int32)log->readerCore->read - (int32)log->writerCore->readWriter) >= (int32)0)
  if ((int32)(log->readerCore->read - log->writerCore->readWriter) >=  0)
  {
    // The read index is ahead of the write index, so we're ok.
    return(TRUE);
  }
  log->readerCore->read = log->writerCore->readWriter;
  return(FALSE);
}


/**
 * <!-- ULogCore_Read -->
 *
 * @brief Read data from the log buffer.
 * 
 * Read data from the log buffer.  This data is unformatted. Call
 * ULogCore_MsgFormat to format each message.  Only full messages
 * are placed in the output buffer, thus it is incorrect to use
 * outputCount != outputSize as an indicator that the log buffer
 * is empty.
 *
 * An empty log buffer will yield a read of only status, and an
 * outputCount = 4.
 * 
 * @param h : A ULog handle of the log to read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 *
 * @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_Read( ULogHandle h,
                          uint32 outputSize, 
                          char* outputMem,
                          uint32* outputCount )
{
  return(ULogCore_ReadEx(h, outputSize, outputMem, outputCount, 0));
}


/**
 * <!-- ULogCore_ReadEx -->
 *
 * @brief Read data from the log buffer.
 *
 * Read data from the log buffer.  See ULogCore_Read for
 * description.  Allows additional control of limited number of
 * messages read.
 *
 * @param h : A ULog handle of the log to read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 * @param outputMessageLimit : The maximum number of messages to read from
 *                     the buffer.
 *
 * @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_ReadEx( ULogHandle h,
                            uint32 outputSize,
                            char *outputMem,
                            uint32* outputCount,
                            uint32 outputMessageLimit)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 msgLength;
  char * copyPtr;
  char * statusLocation = outputMem;
  uint32 statusValue;
  uint32 overrunDetected = 0;
  uint32 outputMessagesRemaining = outputMessageLimit;

  //Begin with lots of checks to make sure the log and function arguments are reasonable for log reading to begin. 
  if (log == NULL)
  {
    if (outputCount != NULL)
    {
      *outputCount = 0;
    }
    return(ULOG_ERR_INVALIDPARAMETER);
  }

  if (outputCount == NULL)   //no valid memory location to put the our copied bytes count. 
  {
    return(ULOG_ERR_INVALIDPARAMETER);
  }

#ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log))
  {
    *outputCount = 0;
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif

  if ((log->logStatus & ULOG_STATUS_DEFINED) == 0)
  {
    // The log is not yet defined.  Return no data, but don't error either.
    *outputCount = 0;
    return(DAL_SUCCESS);
  }
  
  // Check for possible error conditions.
  if ((outputMem == NULL) ||    //no valid memory location to put the data 
           (outputSize < 4) ||       //insufficient space to copy out the data
           ((log->bufMemType & ULOG_MEMORY_USAGE_READABLE) == 0))
  {
    *outputCount = 0;
    return(ULOG_ERR_INVALIDPARAMETER);
  }

  // One more check for a valid log to read
  if (log->sharedHeader == NULL || log->buffer == NULL)
  {
    *outputCount = 0;
    return(DAL_SUCCESS);
  }

  //Static logs jump the write pointer back and forth, this is the only case where we lock a log to read it. 
  if (log->staticMsgSlots!=0){
    ULogInternal_LockGet(log);
  }

    // Skip the location for the status word.
    *outputCount = 4;
    outputMem+=4;

    //first read from the log, let the log catch up before giving overrun notificaiton
    if (!(log->logStatus & ULOG_STATUS_LOG_SW_READ_STARTED))
    {
      ReadIsValid(log);
      log->logStatus |= ULOG_STATUS_LOG_SW_READ_STARTED;
    }


  while ((log->readerCore->read != log->writerCore->write) &&                 /* not caught up with the write pointer*/
         (*outputCount < outputSize) &&                                       /* there is space left in the buffer to copy messages into */
         ((outputMessageLimit == 0) || (outputMessagesRemaining != 0)) &&     /* haven't read the number of desired messages yet*/
         overrunDetected == 0)                                                /* we couldn't read fast enough, the log moved under our feet.*/
    {
      copyPtr = outputMem;

      // Read the next message length
    core_memcpy(&msgLength, 4, &log->buffer[log->readerCore->read & log->bufSizeMask], 4);
      msgLength = (msgLength & ULOG_LENGTH_MASK) >> ULOG_LENGTH_SHIFT;
      msgLength = ULOGINTERNAL_PADCOUNT(msgLength);

      // Make sure the current message is still valid.  If so,
      // then msgBytes should be valid, and we can read the
      // message data.
      // 
      if (ReadIsValid(log))
      {
        if (msgLength == 4)
        {
          // The next message isn't written yet.  We're done.
          break;
        }
        else if ((*outputCount + msgLength) <= outputSize)
        {
          uint32 i;
          // This message will fit into the output buffer.  Copy it.
          // Don't move the outBuf pointer until we are sure the
          // message data is good.
          //
          for(i = 0; i < msgLength; i++)
          {
            *copyPtr++ = log->buffer[(log->readerCore->read+i) & log->bufSizeMask];
          }
        }
        else if (msgLength > outputSize)
        {
          // The entire output buffer doesn't have enough memory to hold the
          // one message.  Throw the message away, or it will clog the pipeline.
          // 
          log->readerCore->read += msgLength;
          if (log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)
          {
            log->writerCore->usageData |= ULOG_ERR_LARGEMSGOUT;
          }
          break;
        }
        else
        {
          // We have written all we can to the output buffer.
          break;
        }
        if (ReadIsValid(log))
        {
          // The full message is valid.  Update all the pointers
          // and byte counts as appropriate.
          // 
          // Advance the read record in the log to account for what we've just read out.
          log->readerCore->read += msgLength;
          // Update the number of bytes we've copied so far into the caller's count.
          *outputCount += msgLength;
          // Advance the starting point for then next pass through this loop
          outputMem = copyPtr;   
          // The caller requested a number of messages to be read out.  Subtract 1 for the message we just completed. 
          outputMessagesRemaining--;
          //if we made it here, than we successfully processed a message for this read 
          //meaning that the previous message is no longer an overrun, hence clear the flag
          //indicating the previous read was an overrun 
          if (log->logStatus & ULOG_STATUS_LAST_MSG_WAS_OVERRUN)
          {
            log->logStatus &= ~ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
          }
        }
        else
        {
          //invalid read pointer, this indactes that we may need to send an overrun notification
          if (log->logStatus & ULOG_STATUS_LAST_MSG_WAS_OVERRUN)
          {
            //the message right before this read was also an overrun,
            //so we won't send an additonal notification, instead we 
            //use this detection to catch the read pointer up to the readWriter pointer (begining of valid messages)
            log->logStatus &= ~ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
          }
          else if (log->logStatus & ULOG_STATUS_LOG_SW_READ_STARTED)
          {
            //an overrun was detected and the last msg wasn't an overrun and this
            //isnt the first read from the log, break out of the read and send the notification
            overrunDetected = 1;
          }
        }
      }
      else
      {
        //invalid read pointer, this indactes that we may need to send an overrun notification   
        if (log->logStatus & ULOG_STATUS_LAST_MSG_WAS_OVERRUN)
        {
          //the message right before this read was also an overrun,
          //so we won't send an additonal notification, instead we 
          //use this detection to catch the read pointer up to the readWriter pointer (begining of valid messages)
          log->logStatus &= ~ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
        }
        else if (log->logStatus & ULOG_STATUS_LOG_SW_READ_STARTED)
        {
          //an overrun was detected and the last msg wasn't an overrun and this
          //isnt the first read from the log, break out of the read and send the notification
          overrunDetected = 1;
        }
      }
    }

    // Rewind the log if we have just read and found no data.
    // The only thing in the log is the status.
    if ((*outputCount == 4) &&
        (log->attributes & ULOG_ATTRIBUTE_READ_AUTOREWIND))
    {
      log->readerCore->read = log->writerCore->readWriter;
      log->readerCore->readFlags |= ULOG_RD_FLAG_REWIND;
    }

    // Write the status here.
    statusValue = log->writerCore->usageData;
  core_memcpy(statusLocation, sizeof(statusLocation), &statusValue, 4);
    if (log->bufMemType & ULOG_MEMORY_USAGE_WRITEABLE)
    {
      // Note, there is a window of time in which an error could
      // take place that would be cleared by this operation.
      log->writerCore->usageData = 0;
    }

  //Static logs jump the write pointer back and forth, this is the only case where we lock a log to read it. 
  if (log->staticMsgSlots!=0){
    ULogInternal_LockRelease(log);
  }


  if(overrunDetected == 1)
  {
    log->logStatus |= ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
    return(ULOG_ERR_OVERRUN);
  }

    return(DAL_SUCCESS);
  }


/**
 *  <!-- ULogCore_ReadSessionComplete -->
 *
 * @brief Indicate that any previous read sessions are complete. No overrun detection needed 
 * on the very next read of this log. Also make sure rewind logs are rewound.
 *
 * This function is needed so that reading a log multiple times doesn't consider the
 * read jump at the start of reading to be an overrun.  Use this safely at the start
 * or end of your read session.
 *
 * @param h : A ULog handle of the log about to be read or just completed being read.
 *
 * @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_ReadSessionComplete(ULogHandle h)
  {
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return(ULOG_ERR_INVALIDPARAMETER);
  }
#ifndef DEFEATURE_ULOG_TEE
  else if (HANDLE_IS_A_TEE(log))
  {
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif
  else if ((log->logStatus & ULOG_STATUS_DEFINED) == 0)
  {
    // The log is not yet defined. Nothign to do, but no error either.
    return(DAL_SUCCESS);
  }

  //clear the READ_STARTED bit so the next jumping the read pointer won't be flagged as an overrun while reading.
  log->logStatus &= ~ULOG_STATUS_LOG_SW_READ_STARTED;

  //clear the last message was an overrun flag. 
  log->logStatus &= ~ULOG_STATUS_LAST_MSG_WAS_OVERRUN;

  //if the log is a rewind log, reset the read pointer.
  if (log->attributes & ULOG_ATTRIBUTE_READ_AUTOREWIND)
  {
    log->readerCore->read = log->writerCore->readWriter;
  }
  return(DAL_SUCCESS);
}


/**
 * <!-- ULogCore_HeaderRead -->
 * 
 * @brief Read the header from the supplied log.
 * 
 * @param h : A handle of the log to access.
 * @param statusString : The output string.
 * @param statusStringSize : The size of the output string
 * 
 * @return DAL_SUCCESS if the format was successful. Errors are
 *         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_HeaderRead(ULogHandle h,  
                               uint32 headerReadOffset,
                               char * headerString, 
                               uint32 headerStringSize,
                               uint32 * headerActualLength)
{
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (log == NULL)
  {
    return (DAL_ERROR);
  }

  if ((headerStringSize == 0) ||
      (headerActualLength == NULL))
  {
    return(DAL_ERROR);
  }

#ifndef DEFEATURE_ULOG_TEE
  else if (HANDLE_IS_A_TEE(log))
  {
    return(ULOG_ERR_INVALIDHANDLE);
  }
#endif
  else if (log->header == NULL)
  {
    *headerActualLength = 0;
    headerString[0] = 0;
  }
  else
  {
    if (headerReadOffset < strlen(log->header))
    {
      core_strlcpy(headerString, &log->header[headerReadOffset], headerStringSize);
    }
    else
    {
      headerString[0] = 0;
    }
    *headerActualLength = strlen(headerString);
  }

  return(DAL_SUCCESS);
}

#endif // DEFEATURE_ULOG_READ

#ifdef __cplusplus
}
#endif
