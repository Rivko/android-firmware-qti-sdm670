/*============================================================================
  micro_diagbuffer_client_read.c

  The read function translates client log buffer to local address
  using mapping function set by the client before reading.

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1/services/diag/micro_diagbuffer/src/micro_diagbuffer_client_read.c#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   sp     Created new file to support microULog read functionality.
===========================================================================*/

#include <stringl/stringl.h>        /* For memscpy */
#include "MemBarrier.h"
#include "osal.h"
#include "micro_diagbuffer_i.h"
#include "micro_diagbuffer_client_read.h"

/**
 * <!-- micro_diagbuffer_is_read_valid -->
 *
 * @brief Is the read pointer somewhere between the current beginning and end of the log? 
 *
 * @param log : Pointer to the microUlog buffer to check.
 *
 * @return TRUE (1) if the read pointer is valid, FALSE (0) if not. 
 */

static uint32 micro_diagbuffer_is_read_valid( micro_ULogHandle log )
{
  if ( log )
  { 
    /* Return false if the read pointer required update. */
    if ( (int32)(log->read - log->readWriter) >= 0 )
    {
      /* The read index is ahead of the write index, so we're ok. */
      return ( TRUE );
    }
    log->read = log->readWriter;
  }
  return ( FALSE );
}

/**
 * <!-- micro_diagbuffer_NameGet -->
 *
 * @brief Get the log name by its handle.
 *
 * @param Handle : Handle of a microULog
 * @param logname : Pointer to copy the log name to
 * @return MICRO_ULOG_SUCCESS if handle is valid
 */

micro_ULogResult micro_diagbuffer_NameGet(micro_ULogHandle handle, char* logname)
{
  micro_ULogResult result = MICRO_ULOG_ERR_INVALIDPARAMETER;

  if (handle && logname)
  {
    strlcpy(logname, handle->name, MICRO_ULOG_MAX_NAME_SIZE + 1);
    result = MICRO_ULOG_SUCCESS;
  }
  return result;
}

/**
 * <!-- micro_diagbuffer_Read_vaddr-->
 *
 * @brief Read data from the microUlog buffer
 *
 * Read data from the microUlog buffer. This data is unformatted. 
 * Call micro_diagbuffer_FormatMsg_vaddr to format each message. Only
 * full messages are placed in the output buffer, thus it is incorrect 
 * to use outputCount != outputSize as an indicator that the log 
 * buffer is empty.
 *
 *
 * @param h : A microULog handle of the log to read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 * @param outputMessageLimit : The maximum number of messages to read from
 *                     the buffer.
 * @param mapping_fprt : mapping function set by client for address translation
 *
 * @return MICRO_ULOG_SUCCESS if the read completed.
 */

micro_ULogResult micro_diagbuffer_Read_vaddr( micro_ULogHandle h,
                                               uint32 outputSize,
                                               char *outputMem,
                                               uint32 *outputCount,
                                                           uint32 outputMessageLimit,
                                                           vaddr_fptr mapping_fptr)
{
  micro_ULogHandle log = (micro_ULogHandle)h;
  uint32 bufSizeMask = 0; 
  uint32 msgLength;
  char * copyPtr;
  uint32 overrunDetected = 0;
  uint32 outputMessagesRemaining = outputMessageLimit;
  char * buffer = NULL;

  /* Begin with lots of checks to make sure the log and function arguments are reasonable for log reading to begin. */
  if ( log == NULL )
  {
    if ( outputCount != NULL )
    {
      *outputCount = 0;
    }
    return ( MICRO_ULOG_ERR_INVALIDPARAMETER );
  }

  bufSizeMask = MICRO_ULOG_BUFSIZE_MASK(log);

  if ( outputCount == NULL )   /* no valid memory location to put the our copied bytes count. */
  {
    return ( MICRO_ULOG_ERR_INVALIDPARAMETER );
  }

  if ( (log->logStatus & MICRO_ULOG_STATUS_DEFINED) == 0 )
  {
    /* The log is not yet defined.  Return no data, but don't error either. */
    *outputCount = 0;
    return ( MICRO_ULOG_SUCCESS );
  }

  /* Check for possible error conditions. */
  if ( (outputMem == NULL) ||    /* no valid memory location to put the data */
       (outputSize < 4) )         /* insufficient space to copy out the data */
  {
    *outputCount = 0;
    return ( MICRO_ULOG_ERR_INVALIDPARAMETER );
  }

  if (!(log->logStatus & MICRO_ULOG_STATUS_LOG_SW_READ_STARTED))
  {
    micro_diagbuffer_is_read_valid(log);
    log->logStatus |= MICRO_ULOG_STATUS_LOG_SW_READ_STARTED;
  }

  if (mapping_fptr)
  {
    /* Convert log buffer to local address using vaddr mapping function */
    buffer = (char *)mapping_fptr((uint32)log->buffer);
  }
  else
  {
    buffer = log->buffer;
  }
  while ( (log->read != log->write) &&                                        /* not caught up with the write pointer*/
          (*outputCount < outputSize) &&                                      /* there is space left in the buffer to copy messages into */
          ((outputMessageLimit == 0) || (outputMessagesRemaining != 0)) &&     /* haven't read the number of desired messages yet*/
          overrunDetected == 0 )                                               /* we couldn't read fast enough, the log moved under our feet.*/
  {
    copyPtr = outputMem;

    /* Read the next message length */
    memscpy(&msgLength, 4, &buffer[log->read & bufSizeMask], 4);
    msgLength = (msgLength & MICRO_ULOG_LENGTH_MASK) >> MICRO_ULOG_LENGTH_SHIFT;
    msgLength = MICRO_ULOG_PADCOUNT(msgLength);

    /* Make sure the current message is still valid.  If so,
       then msgBytes should be valid, and we can read the
       message data.
    */
    if ( micro_diagbuffer_is_read_valid(log) )   //if the read isn't valid the log moved while we were trying to read it.
    {
      if ( msgLength == 0 )
      {
        /* The next message isn't written yet.  We're done. */
        break;
      }
      else if ( (*outputCount + msgLength) <= outputSize )
      {
        uint32 i;

        /* This message will fit into the output buffer.  Copy it.
           Don't move the outBuf pointer until we are sure the
           message data is good.
        */
        for ( i = 0; i < msgLength; i++ )
        {
          *copyPtr++ = buffer[(log->read + i) & bufSizeMask];
        }
      }
      else if ( msgLength > outputSize )
      {
        /* The entire output buffer doesn't have enough memory to hold the
           one message.  Throw the message away, or it will clog the pipeline.
        */
        log->read += msgLength;
        log->usageData |= MICRO_ULOG_ERR_LARGEMSGOUT;
        break;
      }
      else
      {
        /* We have written all we can to the output buffer. */
        break;
      }
      if ( micro_diagbuffer_is_read_valid(log) )//check one more time that the log hasn't moved while we read it.
      {
        /* The full message is valid.  Update all the pointers
           and byte counts as appropriate.
        */
        /* Advance the read record in the log to account for what we've just read out. */
        log->read += msgLength;
        /* Update the number of bytes we've copied so far into the caller's count. */
        *outputCount += msgLength;
        /* Advance the starting point for then next pass through this loop */
        outputMem = copyPtr;
        /* The caller requested a number of messages to be read out.  Subtract 1 for the message we just completed. */
        outputMessagesRemaining--;
        //if we made it here, than we successfully processed a message for this read 
        //meaning that the previous message is no longer an overrun, hence clear the flag
        //indicating the previous read was an overrun 
        if (log->logStatus & MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN)
        {
          log->logStatus &= ~MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
        }
      }
      else
      {
        //invalid read pointer, this indactes that we may need to send an overrun notification
        if (log->logStatus & MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN)
        {
          //the message right before this read was also an overrun,
          //so we won't send an additonal notification, instead we 
          //use this detection to catch the read pointer up to the readWriter pointer (begining of valid messages)
          log->logStatus &= ~MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
        }
        else if (log->logStatus & MICRO_ULOG_STATUS_LOG_SW_READ_STARTED)
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
      if (log->logStatus & MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN)
      {
        //the message right before this read was also an overrun,
        //so we won't send an additonal notification, instead we 
        //use this detection to catch the read pointer up to the readWriter pointer (begining of valid messages)
        log->logStatus &= ~MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
      }
      else if (log->logStatus & MICRO_ULOG_STATUS_LOG_SW_READ_STARTED)
      {
        //an overrun was detected and the last msg wasn't an overrun and this
        //isnt the first read from the log, break out of the read and send the notification
        overrunDetected = 1;
      }
    }
  }

  if(overrunDetected == 1)
  {
    log->logStatus |= MICRO_ULOG_STATUS_LAST_MSG_WAS_OVERRUN;
    return(MICRO_ULOG_ERR_OVERRUN);
  }

  return ( MICRO_ULOG_SUCCESS );
}

/**
 * <!-- micro_diagbuffer_ReadAndFormat_vaddr -->
 *
 * @param handle : Pointer to microULog handle to read from
 * @param format_req : Format of payload to generate (ASCII)
 * @param format_rsp : Format of payload generated
                       (will contain OVERRUN information if needed)
 * @param buf_size : size of payload_buf
 * @param payload_buf : pointer to payload buffer where output will be written
 * @param bytes_written: set to number of bytes written to payload_buf
 *                       upon exitting this function. A value of 0 can
 *                       indicate an empty microULog, or that a rewind has occurred.
 * @param mapping_fptr: mapping function set by client for address translation
 *
 * @return MICRO_ULOG_SUCCESS if successful. (bytes_written value of 0 with MICRO_ULOG_SUCCESS indicates an empty microULog)
 *         Error are definded in the micro_ULogResult enum.
 *         MICRO_ULOG_ERROR in this case represents that the microULog was rewound while trying to read from it.
 */

micro_ULogResult micro_diagbuffer_ReadAndFormat_vaddr(micro_ULogHandle handle, int format_req,
                                                             unsigned long int *format_rsp, int buf_size,
                                                             char * payload_buf, unsigned long int *bytes_written, vaddr_fptr mapping_fptr)
{
  micro_ULogResult rval = MICRO_ULOG_ERR_INVALIDPARAMETER;
  
  if((handle==NULL) || (bytes_written==NULL) || (format_rsp==NULL))
  {
    return MICRO_ULOG_ERR_INVALIDPARAMETER;
  }

  if(format_req == MICROULOG_DIAG_FORMAT_ASCII)
  {
    char * stringStart = (char *)payload_buf;
    uint32 outputMsgIndex = 0;
    uint32 msgConsumed;
    uint32 oneMsgBuffer[32];

    // Null terminate the output buffer.
    *bytes_written = 0;
    payload_buf[0] = 0;

    // While the log has messages in it, or the output buffer
    // is not full, continue reading from the log
    while (1)
    {
      *format_rsp = MICROULOG_DIAG_FORMAT_ASCII;
      uint32 readOutputLength = 0;

      if(MICRO_ULOG_ERR_OVERRUN == 
          micro_diagbuffer_Read_vaddr (handle,
          sizeof(oneMsgBuffer),
          (char *)oneMsgBuffer,
          &readOutputLength,
          1,
          mapping_fptr))
      {
        *format_rsp = MICROULOG_DIAG_FORMAT_ASCII_OVERRUN_DETECTED;
      }

      // Read one message from the log
      if (readOutputLength)
      {
        // Convert it and put it in the output buffer
 
        micro_diagbuffer_FormatMsg_vaddr(handle,
          (char *)&oneMsgBuffer,
          &stringStart[outputMsgIndex], 
          buf_size - outputMsgIndex, 
          &msgConsumed,
          mapping_fptr);

        // Now how full is our output buffer
        outputMsgIndex = strlen(stringStart);

        *bytes_written = outputMsgIndex;

        // If we have filled enough of the output buffer,
        // call it good and return.
        if ((buf_size - outputMsgIndex) < 255)
        {
          rval = MICRO_ULOG_SUCCESS;
          break;
        }
      }
      else
      {
        // The log has no data.
        rval = MICRO_ULOG_SUCCESS;
        break;
      }
    } // while loop
  }

  return rval;
}

