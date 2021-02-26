/*============================================================================
  micro_diagbuffer_read.c

  micro_diagbuffer read functions.

  Copyright (c) 2015-2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/micro_diagbuffer_read.c#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/17   sp      Add support for microULog debug capability 
09/13/16   rs      Removed unused variables/functions to avoid compilation warnings
08/25/15   vk      Fixed KW warnings
08/16/15   vk      file created for micro diagbuffer drain implementation

===========================================================================*/
#ifdef DIAG_UIMAGE_MODE

#include <stddef.h>                 /* For size_t */

#include "micro_diagbuffer_i.h"
#include "micro_diagbuffer_read.h"  /* For 	micro_diagbuffer_read() */

/* Round up to the nearest word. */
#define MICRO_DIAGBUFFER_INTERNAL_PADCOUNT(x) ((x + 3) & 0xFFFFFFFC)

/**
 * <!-- micro_diagbuffer_internal_is_read_valid -->
 *
 * @brief Is the read pointer somewhere between the current beginning and end of the log? 
 *
 * @param log : Pointer to the micro diagbuffer to check.
 *
 * @return TRUE (1) if the read pointer is valid, FALSE (0) if not. 
 */
static uint32 micro_diagbuffer_internal_is_read_valid( MICRO_DIAGBUFFER_TYPE *log )
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
 * <!-- micro_diagbuffer_read -->
 *
 * @brief Read data from the micro diagbuffer
 *
 * Read data from the micro diagbuffer. This data is unformatted. 
 * Call ULogCore_MsgFormat to format each message. Only full 
 * messages are placed in the output buffer, thus it is incorrect 
 * to use outputCount != outputSize as an indicator that the log 
 * buffer is empty.
 *
 *
 * @param h : A micro diagbuffer handle of the buffer read.
 * @param outputSize : Size of the output buffer.
 * @param outputMem : A pointer to the output buffer.
 * @param outputCount : The amount of data placed in the output buffer.
 * @param outputMessageLimit : The maximum number of messages to read from
 *                     the buffer.
 *
 * @return MICRO_DIAGBUFFER_SUCCESS if the read completed.
 */
micro_diagbuffer_result micro_diagbuffer_read( micro_diagbuffer_handle h,
                                               uint32 outputSize,
                                               uint8 *outputMem,
                                               uint32 *outputCount,
                                               uint32 outputMessageLimit )
{
    MICRO_DIAGBUFFER_TYPE *log = (MICRO_DIAGBUFFER_TYPE *)h;
    uint32 bufSizeMask = 0; 
    uint32 msgLength;
    uint8 *copyPtr;
    uint32 outputMessagesRemaining = outputMessageLimit;

    /* Begin with lots of checks to make sure the log and function arguments are reasonable for log reading to begin. */
    if ( log == NULL )
    {
      if ( outputCount != NULL )
      {
        *outputCount = 0;
      }
      return ( MICRO_DIAGBUFFER_ERR_INVALIDPARAMETER );
    }

    bufSizeMask = MICRO_DIAGBUFFER_BUFSIZE_MASK(log);

    if ( outputCount == NULL )   /* no valid memory location to put the our copied bytes count. */
    {
      return ( MICRO_DIAGBUFFER_ERR_INVALIDPARAMETER );
    }

    if ( (log->logStatus & MICRO_DIAGBUFFER_STATUS_DEFINED) == 0 )
    {
      /* The log is not yet defined.  Return no data, but don't error either. */
      *outputCount = 0;
      return ( MICRO_DIAGBUFFER_SUCCESS );
    }

    /* Check for possible error conditions. */
    if ( (outputMem == NULL) ||    /* no valid memory location to put the data */
         (outputSize < 4) )         /* insufficient space to copy out the data */
    {
      *outputCount = 0;
      return ( MICRO_DIAGBUFFER_ERR_INVALIDPARAMETER );
    }

    micro_diagbuffer_internal_is_read_valid(log);

    while ( (log->read != log->write) &&                                        /* not caught up with the write pointer*/
            (*outputCount < outputSize) &&                                      /* there is space left in the buffer to copy messages into */
            ((outputMessageLimit == 0) || (outputMessagesRemaining != 0)) )      /* haven't read the number of desired messages yet*/
    {
      copyPtr = outputMem;

      /* Read the next message length */
      micro_diagbuffer_internal_memcpy(&msgLength, 4, &log->buffer[log->read & bufSizeMask], 4);
      msgLength = (msgLength & MICRO_ULOG_LENGTH_MASK) >> MICRO_ULOG_LENGTH_SHIFT;
      msgLength = MICRO_DIAGBUFFER_INTERNAL_PADCOUNT(msgLength);

      /* Make sure the current message is still valid.  If so,
         then msgBytes should be valid, and we can read the
         message data.
      */
      if ( micro_diagbuffer_internal_is_read_valid(log) )   //if the read isn't valid the log moved while we were trying to read it.
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
            *copyPtr++ = log->buffer[(log->read + i) & bufSizeMask];
          }
        }
        else if ( msgLength > outputSize )
        {
          /* The entire output buffer doesn't have enough memory to hold the
             one message.  Throw the message away, or it will clog the pipeline.
          */
          log->read += msgLength;
          break;
        }
        else
        {
          /* We have written all we can to the output buffer. */
          break;
        }
        if ( micro_diagbuffer_internal_is_read_valid(log) )   //check one more time that the log hasn't moved while we read it.
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
        }
      }
    }
    return ( MICRO_DIAGBUFFER_SUCCESS );
}

#endif /* DIAG_UIMAGE_MODE */
