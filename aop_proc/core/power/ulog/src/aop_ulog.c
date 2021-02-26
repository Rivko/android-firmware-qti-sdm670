/*============================================================================
  rpm_ULog.c

  Public-facing/support RPM_ULog functions.

  Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/
/* $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ulog/src/aop_ulog.c#1 $ */
#include "aop_ulog.h"


//global pointer to the ULogs.  The compiler initializes globals to zero. 
RPM_LOG_CONTEXT rpm_ulogContext;




/*---------------------------------------------------------------------------------------
                                  INLINE FUNCTIONS
---------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------
  rpm_ULogInternal_UseFastRam
  Checks to see if fast RAM writes can be used, or if VTable writes must be
  used instead.
---------------------------------------------------------------------------------------*/
static RPM_ULOG_INLINE rpm_ULogResult rpm_ULogInternal_UseFastRam(RPM_ULOG_TYPE * log)
{
    if (log->transport!=RPM_ULOG_TRANSPORT_RAM){
      return RPM_ULOG_ERROR;
    } 
    return RPM_ULOG_SUCCESS;
}




/*----------------------------------------------------------------------------
                       REGULAR (NON-INLINE) FUNCTIONS
----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 rpm_ULog_ConvertSize
 Ulog sizes are always a power of 2. 64 bytes is the minimum. 
----------------------------------------------------------------------------*/
uint32 rpm_ULog_ConvertSize(uint32 size)
{
  uint32 output;

  if (size < RPM_ULOG_MINIMUM_LOG_SIZE)
  {
    // Require logs to be at least 64 bytes long.
    size = RPM_ULOG_MINIMUM_LOG_SIZE;
  }

  // Ensure the size is an even power of 2.  
  output = 0;
  while(size != 1)
  {
    size >>= 1;
    output++;
  }
  output = 1 << output;

  return(output);
}



/*----------------------------------------------------------------------------
 rpm_ULog_CreateNew
 Assigns from the given memory space for all of the components of a rpm_ULog: the
  ReaderCore, WriterCore, top-level RPM_ULOG_STRUCT, and the message buffer. The
  message buffer is sized as large as possible but must be a power of 2 and at minimum
  64 bytes.
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_CreateNew(rpm_ULogHandle * h,
                                      const char * logName,
                                      char * mem,
                                      uint32 * effectiveMessageBufferSize,
                                      uint32 memSize)
{
  rpm_ULogResult result;
  char * tempMemPtr = mem;
  uint32 tempMemPtrSize = memSize;
  uint32 realBufferSize;
  uint32 remainingSpace = memSize - sizeof(RPM_ULOG_TYPE);

  if (mem == NULL || h == NULL){
    return RPM_ULOG_ERROR;
  }

  // verify that there is enough space for at least a minimum-sized buffer
  if (remainingSpace < 64){
    return RPM_ULOG_ERR_BUFFER_TOO_SMALL;
  }

  // round down to the next power of two
  realBufferSize = rpm_ULog_ConvertSize(remainingSpace);

  // zero it out, to have a nice clean structure. 
  memset((void*)mem, 0, memSize);

  // assign the RPM_ULOG_TYPE
  if (RPM_ULOG_SUCCESS != (result = rpm_ULog_MemoryAssignLog(h, (void *)mem, memSize, logName)))
  {
    return result;
  }
  tempMemPtr += sizeof(RPM_ULOG_TYPE);
  tempMemPtrSize -= sizeof(RPM_ULOG_TYPE);
  // do word-alignment just in case
  if ((uint32)tempMemPtr & 0x03){
    tempMemPtr = (char*)(((uint32)tempMemPtr + 4) & (~0x03));
    tempMemPtrSize = tempMemPtrSize & (~0x03);
  }

  // assign the message buffer
  if (RPM_ULOG_SUCCESS != (result = rpm_ULog_MemoryAssign(*h, RPM_ULOG_VALUE_BUFFER, tempMemPtr, realBufferSize)))
  {
    return result;
  }
  tempMemPtr += realBufferSize;
  tempMemPtrSize -= realBufferSize;
  // this will automatically be word-aligned as buffers are a power of 2, greater than 64 bytes

  // Enable the log and finish up
  rpm_ULog_Enable(*h);

  if (effectiveMessageBufferSize != NULL) {
    *effectiveMessageBufferSize = realBufferSize;
  }

  return RPM_ULOG_SUCCESS;
}



/*---------------------------------------------------------------------------------------
  rpm_ULog_MemoryAssign
  Assigns given memory to a ULog component, such as the message buffer, ReaderCore, etc
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_MemoryAssign(rpm_ULogHandle h,
                                         RPM_ULOG_VALUE_ID id,
                                         void * bufferPtr,
                                         uint32 bufferSize)
{
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;
  rpm_ULogResult result = RPM_ULOG_ERROR;
  uint32 realSize;
  if (h == NULL)
  {
    return RPM_ULOG_ERROR;
  }

  if (((uint32)bufferPtr & 0x03) != 0)  // If the ptr is not word aligned...
  {
    return(RPM_ULOG_ERROR);
  }

  switch(id)
  {
    case RPM_ULOG_VALUE_BUFFER:
      // Only assign memory if we don't already have some assigned
      if (log->buffer == NULL)
      {
        realSize = rpm_ULog_ConvertSize(bufferSize);
          
        // Make sure we were given enough memory.
        if (realSize > bufferSize)
        {
          result = RPM_ULOG_ERROR;
          break;
        }
        log->buffer = (char *)bufferPtr;
        log->bufSize = realSize;
        log->bufSizeMask = realSize - 1;
        result = RPM_ULOG_SUCCESS;
      }
      break;

    default:
      result = RPM_ULOG_ERROR;
      break;
  }

  // Flag the log as defined after it's buffer is assigned.
  if (log->buffer != NULL)
  {
    log->logStatus |= RPM_ULOG_STATUS_DEFINED;
  }

  return(result);
}



/*---------------------------------------------------------------------------------------
  rpm_ULog_MemoryAssignLog
  Creates a new top-level rpm_ULog structure in the specified memory.
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_MemoryAssignLog(rpm_ULogHandle * h,
                                            void * bufferPtr,
                                            uint32 bufferSize,
                                            const char * logName)
{
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)bufferPtr;

  if (h == NULL){
    return RPM_ULOG_ERROR;
  }
  
  // ensure the buffer is big enough
  if ( sizeof(RPM_ULOG_TYPE) > bufferSize )
  {
    return RPM_ULOG_ERR_BUFFER_TOO_SMALL;
  }

  // zero this structure's space out
  memset(bufferPtr, 0, sizeof(RPM_ULOG_TYPE));

  // ensure the name is not too big to fit into the buffer, then set the log name
  if((strlen(logName) > RPM_ULOG_MAX_NAME_SIZE) || (strlen(logName) == 0))
  {
    return RPM_ULOG_ERR_INVALIDNAME;
  }
  RPM_ULOG_STRLCPY(log->name, logName, RPM_ULOG_MAX_NAME_SIZE+1);
  
  // Link the log into the list
  log->next = rpm_ulogContext.logHead;
  rpm_ulogContext.logHead = log;
  
  // set flags (see ULogCore_LogCreate in ULCoreInterface.c)
  // Copy input parameters to log header.
  log->version               = RPM_ULOG_TYPE_LOG_HEADER_VERSION;
  log->stmPort               = 0;
  log->transport             = RPM_ULOG_TRANSPORT_RAM;
  log->feature_flags1        = log->feature_flags1 | RPM_ULOG_FF1_64_BIT_TS; 

  *h = log;
  
  return RPM_ULOG_SUCCESS;
}






/*---------------------------------------------------------------------------------------
  rpm_ULog_ResetLog
  Should be used only rarely in anything but test code. "Resets" a log by moving the read
   and write pointers back to the beginning of the circular RAM buffer.
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_ResetLog(rpm_ULogHandle h)
{
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;

  if (log == NULL)
  {
    return RPM_ULOG_ERROR;
  }
  
  //Set all the indexes back to zero and clear flags. 
  log->write=0;
  log->readWriter=0;
  log->read=0;
  log->readFlags=0;

  //set a the reset bit in the usageData variable.
  log->usageData=RPM_ULOG_ERR_RESET;
  //Increment the resetCount so users will know a bit about how often this has happened.
  log->resetCount = log->resetCount + 1;  
    
  return RPM_ULOG_SUCCESS;
}



/*----------------------------------------------------------------------------
 rpm_ULog_Enable 
 Enable and reset the log. 
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_Enable (rpm_ULogHandle h)
{
  rpm_ULogResult result = RPM_ULOG_ERROR;
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;

  if (log == NULL)
  {
    return RPM_ULOG_ERROR;
  }

  if (((log->logStatus & RPM_ULOG_STATUS_DEFINED) == 0) || 
      (log->buffer == NULL))
  {
    result = RPM_ULOG_ERR_INITINCOMPLETE;  
  }
  else if (log->logStatus & RPM_ULOG_STATUS_ENABLED)
  {
    result = RPM_ULOG_ERR_ALREADYENABLED;  
  }
  else
  {
    log->logStatus |= RPM_ULOG_STATUS_ENABLED;

    result = RPM_ULOG_SUCCESS;
  }
  return (result);
}



/*----------------------------------------------------------------------------
 rpm_ULog_Disable
 Disable the log, preventing further writes until it is enabled again
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_Disable(rpm_ULogHandle h)
{
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;
  rpm_ULogResult result = RPM_ULOG_ERROR;

  if (log == NULL)
  {
    return(RPM_ULOG_ERROR);
  }

  if (log->logStatus & RPM_ULOG_STATUS_ENABLED)
  {
    log->logStatus &= ~RPM_ULOG_STATUS_ENABLED;

    result = RPM_ULOG_SUCCESS;
  }

  return (result);
}





/*----------------------------------------------------------------------------
rpm_ULog_SetTransportToRAM
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_SetTransportToRAM(rpm_ULogHandle h)
{

  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;

  if(h == NULL)
  {
    return RPM_ULOG_ERROR;
  }

  log->transport = RPM_ULOG_TRANSPORT_RAM;

  return RPM_ULOG_SUCCESS;
}


/*----------------------------------------------------------------------------
rpm_ULog_SetTransportToSTM
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_SetTransportToSTM(rpm_ULogHandle h)
{
  rpm_ULogResult result = RPM_ULOG_ERROR;
  
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;

  if(h == NULL)
  {
    return RPM_ULOG_ERROR;
  }
  
  if (log->transport != RPM_ULOG_TRANSPORT_STM)
  {

      log->transport = RPM_ULOG_TRANSPORT_STM;
      result = RPM_ULOG_SUCCESS;

  }
  return result;
}


/*----------------------------------------------------------------------------
rpm_ULog_SetTransportToAlt
Take a pointer to a ULOG_CORE_VTABLE and use it for future writes.
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_SetTransportToAlt(rpm_ULogHandle h,
                                              RPM_ULOG_CORE_VTABLE* newTansportVTable)
{
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;

  if(h == NULL)
  {
    return RPM_ULOG_ERROR;
  }

  log->altTransportVTable = newTansportVTable;

  log->transport = RPM_ULOG_TRANSPORT_ALTERNATE;
  
  return RPM_ULOG_SUCCESS;
}


/*----------------------------------------------------------------------------
 RPM_ULog_Fast7WordWrite
 Writes 7 words to the specified log in a format compatible with Raw ULogs.
  This CANNOT be mixed with RealTime ULog writes to the same log.
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_Fast7WordWrite(rpm_ULogHandle h,
                                           uint32 message[7])
{
  RPM_ULOG_TYPE * log = (RPM_ULOG_TYPE *)h;
  uint32 * buffer;
  uint32 bufSize;
  uint32 bufSizeMask;
  uint32 msgSize;
  uint32 msgTotalSize; // size + message
  uint32 size_idx;
  uint32 bufferNeeded;

  if (h == NULL){
    return RPM_ULOG_ERROR;
  }
  // check if current transport set to RAM
  if (rpm_ULogInternal_UseFastRam(log) == RPM_ULOG_SUCCESS)
  {
    buffer = (uint32*)log->buffer;
    bufSize = log->bufSize;
    bufSizeMask = log->bufSizeMask;
    msgSize = 7 * sizeof(uint32);
    msgTotalSize = 4 + msgSize; // size/format + data

    bufferNeeded = log->write - log->readWriter + msgTotalSize;

    if(bufferNeeded > bufSize)
    {
      if(log->read == log->readWriter)
      {
        //indicate that the log has wrapped.  
        log->usageData |= RPM_ULOG_LOG_WRAPPED;
      }
      // Advance the readWriter pointer forward by 1 message. 
      // Note how this is a speed optimization from normal ULogs.  Since all 
      // messages are 8 words long, erasing the old message doesn't require any
      // reading old messages and advancing the readWriter by the old message size.
      // We simply jump readWriter forward 8 words. This is faster, but exactly
      // why this Fast7 API can't be used with the other APIs in the same log. 
      log->readWriter += msgTotalSize;
    }


    // 'zeroing' size to an appropriate word index and incrementing write pointer
    size_idx = (log->write & bufSizeMask) >> 2;
    buffer[size_idx] = 0;
    log->write += 4;

    // write message out to buffer (except for size)
    memcpy(&buffer[size_idx+1], message, msgSize);
    log->write += msgSize;

    // finally, store size and format
    buffer[size_idx] = (msgTotalSize << 16) | RPM_ULOG_SUBTYPE_RESERVED_FOR_RAW;
  }
  
  
  return RPM_ULOG_SUCCESS;
}
