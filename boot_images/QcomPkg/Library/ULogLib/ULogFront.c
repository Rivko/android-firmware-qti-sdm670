/*============================================================================
@file ULogFront.c

This file implements the front end interfaces to ULog.  

Copyright (c) 2009-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "ULogBuildFeatures.h"
#include "stdarg.h"
#include "DALSys.h"
#include "ULog.h"
#include "ULogFront.h"
#include "ULogCoreInternal.h"
#include "ULogInlines.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "CoreString.h"
#include "DALStdDef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DEFEATURE_ULOG_FRONT_REALTIME
/**
 * <!-- ULogFront_RealTimeInit -->
 *
 * @brief Create a new log in the RealTime format.
 *
 * Create a new RealTime log, See ULogCore_LogCreate for details.
 *
 * @param h : A ULog handle is returned at this pointer. 
 * @param name : The log name (24 chars max, including NULL).
 * @param logBufSize : The size in bytes of the log buffer. A request of size 0 
 *                     will leave the buffer unallocated. 
 * @param logBufMemType : The memory configuration. Use ULOG_MEMORY_LOCAL.
 * @param logLockType : The type of locking required.  ULOG_LOCK_OS or ULOG_LOCK_NONE
 *
 * @return DAL_SUCCESS if the create completed successfully.  The log will have been
 *        enabled and will be ready to use. 
 *        ULOG_ERR_INITINCOMPLETE if ULOG_MEMORY_LOCAL_MANUAL is used.  
 *        ULOG_ERR_INITINCOMPLETE if a buffersize of 0 is used.  
 *        The log can be enabled later after ULogCore_MemoryAssign sets up all the parts.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeInit( ULogHandle * h,
                                   const char * name,
                                   uint32 logBufSize,
                                   uint32 logBufMemType,
                                   ULOG_LOCK_TYPE logLockType )
{
  return(ULogCore_LogCreate(h, name, logBufSize, logBufMemType, logLockType, ULOG_INTERFACE_REALTIME));
}


/**
 * <!-- ULogFront_RealTimeStaticInit -->
 *
 * @brief Create a new log in the RealTime Static format.
 *
 * Static logs are different than normal ULogs.  Messages are written to
 * specified locations in the log. This guarantees messages will not 
 * scroll out of the log the way they do on and ordinary ULog. 
 *
 * @param h : Log handle to be filled in.
 * @param logName : Ascii log name (must be less than 24 chars).
 * @param logMessageSlots : How many slots are needed for 
 *                        messages. Max is 9. //sga -- this
 *                        comment is confusing as the test in
 *                        the function indicate that this param
 *                        must be > 1170??
 * @param memoryType : Not used currently. Use ULOG_MEMORY_LOCAL. 
 * @param lockType : ULOG_LOCK_OS or ULOG_LOCK_NONE
 *
 * @return DAL_SUCCESS if the create completed successfully.
 */
ULogResult ULogFront_RealTimeStaticInit( ULogHandle * h,
                                         const char * name,
                                         uint32 logMessageSlots,
                                         uint32 logBufMemType,
                                         ULOG_LOCK_TYPE logLockType )
{
  ULogResult result;
  uint32 i;
  uint32 bufferNeeded;
  uint32 bufferRequestAmt;

  //A 64K log could fit about 1170 messages.
  if ( (logMessageSlots == 0) || (logMessageSlots > 1170) )
  {
    return ULOG_ERR_NOTSUPPORTED;  
  }
  
  //creating an ordinary ULog big enough for up to the desired messages.
  bufferNeeded = logMessageSlots * ULOGSTATIC_MESSAGE_SIZE;

  bufferRequestAmt = 128;  //nothing smaller than 128 bytes

  //find the right sized bufferRequestAmt for a power-of-2 Ulog
  if ( bufferNeeded > bufferRequestAmt )
  {
    for ( i = 7; i <= 16; i++ )
    {
      if ( ((uint32)(1 << i)) > bufferNeeded )
      {
        bufferRequestAmt = (1 << i);  //found the nearest power of two log size that will work for the desired slots.
        break;
      }
    }
  }

  result = ULogCore_LogCreate(h, name, bufferRequestAmt, logBufMemType, logLockType, ULOG_INTERFACE_REALTIME);
  
  if (result != DAL_SUCCESS)
  {
    return result;
  }
 
  //set the number of slots in the log stucture.
  (*(ULOG_TYPE**)h)->staticMsgSlots = logMessageSlots;

  //turn on log rewind at the completion of reading
  (*(ULOG_TYPE**)h)->attributes |= ULOG_ATTRIBUTE_READ_AUTOREWIND;

  //initialize all the slots to unused
  for(i=0; i<logMessageSlots; i++)
  {
    ULOG_RT_STATIC_PRINTF_0(*h, i, "unused");
  }
  return result;
}



/**
 * <!-- ULogFront_RealTimeVprintf -->
 * 
 * @brief Log data in the vprintf format.
 *
 * Log printf data to a RealTime log.  The printf formating is not
 * executed until the log is pulled.  This makes a very performant call,
 * but also means all strings must be present when the log is read.
 *
 * @note This function serves identical purpose to ULogFront_RealTimePrintf,
 *       except it takes a va_list argument instead of being variadic.
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ap : A va_list of the parameters being printed.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeVprintf( ULogHandle h,
                                      uint32 dataCount,
                                      const char * formatStr,
                                      va_list ap )
{
  uint32 total_size;
  uint32 dataArray[15];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32* wordPtr;
  uint32 i;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;

  if ( (dataCount > 10) || (h == NULL) )
  {
    return(DAL_ERROR);
  }

  // msgSize is len/fmt(4) + format string + TS + data
  total_size = 4 + ULOG_FORMAT_STRING_SIZE + ULOG_TIMESTAMP_SIZE + dataCount * 4;

  switch ( ULogInternal_FastFmtSizeTS( h, total_size, ULOG_REALTIME_SUBTYPE_PRINTF ) )
  {
    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg.

      //pointer to the format string
      wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
#ifndef ULOG_64BIT_PROC
      *wordPtr = (uint32)formatStr;
      log->writerCore->write+=4;
#else
      *wordPtr = (uint32)((uint64)formatStr);
      log->writerCore->write += 4;
      wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
      *wordPtr = (uint32)((uint64)formatStr>>32);
      log->writerCore->write+=4;
#endif

      //any arguments
      for(i = 0; i < dataCount; i++)
      {
        wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        *wordPtr = (uint32)va_arg(ap, uint32);
        log->writerCore->write+=4;
      }

      //fill in the size
      wordPtr = (uint32*)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

      ULogInternal_LockRelease(log);

      return (DAL_SUCCESS);


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes.
      //Use the virtualized core->write to handle Tee, STM, or Alternate writes.
      if(ULogCore_IsEnabled(h, &core) == TRUE)
      {
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_GetTeeLocks( h );
        }
        *dataPtr = ULOG_REALTIME_SUBTYPE_PRINTF;
#ifdef ULOG_64BIT_PROC //set format pointer size indication bit
        *dataPtr++ |= ULOG_FORMAT_STR_SIZE_BIT;
#else
        *dataPtr++ &= ~ULOG_FORMAT_STR_SIZE_BIT;
#endif

#ifdef ULOG_64BIT_TIME
        if(log->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(log))
        {
            timestamp = log->altTimestampSrcFn();
        }
        else
        {
        timestamp = CoreTimetick_Get64();
        }

        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp>>32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif

#ifndef ULOG_64BIT_PROC
        *dataPtr++ = (uint32)formatStr;
#else
        *dataPtr++ = (uint32)((uint64)formatStr);
        *dataPtr++ = (uint32)((uint64)formatStr >> 32);
#endif

        for(i = 0; i < dataCount; i++)
        {
          *dataPtr++ = va_arg(ap, uint32);
        }

        retval = core->write (h, total_size, (char *)dataArray,  0, NULL, ULOG_INTERFACE_REALTIME);
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_ReleaseTeeLocks( h );
        }
      }
      return retval;

    case ULOG_FAST_TO_RAM_FAIL:
      //the log is disabled, or some argument is wrong.
        return(DAL_ERROR);
  }

  return(DAL_ERROR);
}


/**
 * <!-- ULogFront_RealTimePrintf -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function does the most work for ULog logging.  It's been optimized to 
 * write directly to the buffer if possible. 
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not including
 *                    the formatStr).  Limited to 10 parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimePrintf(ULogHandle h, uint32 dataCount, const char * formatStr, ...)
{
  uint32 total_size;
  uint32 dataArray[14];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32* wordPtr;
  uint32 i;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;
  va_list    ap;


  if ( (dataCount > 10) || (h == NULL) )
  {
    return(DAL_ERROR);
  }

  // msgSize is len/fmt(4) + format string + TS + data
  total_size = 4 + ULOG_FORMAT_STRING_SIZE + ULOG_TIMESTAMP_SIZE + dataCount * 4;

  switch ( ULogInternal_FastFmtSizeTS( h, total_size, ULOG_REALTIME_SUBTYPE_PRINTF ) )
  {

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg.
      va_start( ap, formatStr );

      //pointer to the format string
      wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
#ifndef ULOG_64BIT_PROC
      *wordPtr = (uint32)formatStr;
      log->writerCore->write+=4;
#else
      *wordPtr = (uint32)((uint64)formatStr);
      log->writerCore->write += 4;
          wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
          *wordPtr = (uint32)((uint64)formatStr>>32);
          log->writerCore->write+=4;
#endif

      //any arguments
      for(i = 0; i < dataCount; i++)
      {
        wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        *wordPtr = (uint32)va_arg(ap, uint32);
        log->writerCore->write+=4;
      }
      va_end(ap);

      //fill in the size
      wordPtr = (uint32*)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

      ULogInternal_LockRelease(log);
      return (DAL_SUCCESS);


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes.
      //Use the virtualized core->write to handle Tee, STM or Alternate writes.
      va_start(ap, formatStr);

      if(ULogCore_IsEnabled(h, &core) == TRUE)
      {
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_GetTeeLocks( h );
        }
        *dataPtr = ULOG_REALTIME_SUBTYPE_PRINTF;
#ifdef ULOG_64BIT_PROC //set format pointer size indication bit
        *dataPtr++ |= ULOG_FORMAT_STR_SIZE_BIT;
#else
        *dataPtr++ &= ~ULOG_FORMAT_STR_SIZE_BIT;
#endif
#ifdef ULOG_64BIT_TIME
        if(log->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(log))
        {
          timestamp = log->altTimestampSrcFn();
        }
        else
        {
          timestamp = CoreTimetick_Get64();
        }  
    
        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp>>32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif

#ifndef ULOG_64BIT_PROC
        *dataPtr++ = (uint32)formatStr;
#else
        *dataPtr++ = (uint32)((uint64)formatStr);
        *dataPtr++ = (uint32)((uint64)formatStr >> 32);
#endif

        for ( i = 0; i < dataCount; i++ )
        {
          *dataPtr++ = va_arg( ap, uint32 );
        }

        retval = core->write( h, total_size, (char *)dataArray,  0, NULL, ULOG_INTERFACE_REALTIME );

        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_ReleaseTeeLocks( h );
        }
      }
      va_end( ap );
      return retval;


    case ULOG_FAST_TO_RAM_FAIL:
      //the log is disabled, or some argument is wrong.
      return ( DAL_ERROR );
  }

  return ( DAL_ERROR );
}


/**
 * <!-- ULogFront_RealTimePrintf_Ex -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function does the most work for ULog logging.  It's been optimized to 
 * write directly to the buffer if possible. 
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not including
 *                    the formatStr).  Limited to 10 parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimePrintf_Ex( ULogHandle h, uint32 dataCount, uint32 paramBitField, const char * formatStr, ... )
{
  uint32 total_size;
  uint32 dataArray[14];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 * wordPtr;
  uint32 i;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;
  va_list    ap;
  uint64 tempValue;
  uint32 num64bitParams;

  num64bitParams = ULogInternal_CountBitsSet( paramBitField );

  if ( (dataCount > 10) || (h == NULL) || (num64bitParams > 10) )
  {
    return ( DAL_ERROR );
  }

  // msgSize is len/fmt(4) + format string + TS + data
  total_size = 4 + ULOG_FORMAT_STRING_SIZE + ULOG_TIMESTAMP_SIZE + (dataCount - num64bitParams) * 4 + num64bitParams * 8;

  switch ( ULogInternal_FastFmtSizeTS( h, total_size, ULOG_REALTIME_SUBTYPE_PRINTF ) )
  {

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg.
      va_start( ap, formatStr );

      //pointer to the format string
      wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
#ifndef ULOG_64BIT_PROC
      *wordPtr = (uint32)formatStr;
      log->writerCore->write += 4;
#else
      *wordPtr = (uint32)((uint64)formatStr);
      log->writerCore->write += 4;
      wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
      *wordPtr = (uint32)((uint64)formatStr >> 32);
      log->writerCore->write += 4;
#endif

      //any arguments
      for ( i = 0; i < dataCount; i++ )
      {
        wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        if ( paramBitField & (1 << i) ) //64-bit parameter
        {
          tempValue = (uint64)va_arg( ap, uint64 );
          *wordPtr = (uint32)((uint64)tempValue);
          log->writerCore->write += 4;
          wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
          *wordPtr = (uint32)((uint64)tempValue >> 32);
          log->writerCore->write += 4;
        }
        else //32-bit parameter
        {
          *wordPtr = (uint32)va_arg( ap, uint32 );
          log->writerCore->write += 4;
        }

      }
      va_end( ap );

      //fill in the size/param size bitfield
      wordPtr = (uint32 *)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount) << ULOG_LENGTH_SHIFT) & ULOG_LENGTH_MASK;
      *wordPtr |= (paramBitField << ULOG_ARG_BITFIELD_SHIFT) & ULOG_ARG_BITFIELD_MASK;

      ULogInternal_LockRelease( log );
      return ( DAL_SUCCESS );


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes.
      //Use the virtualized core->write to handle Tee, STM or Alternate writes.
      va_start( ap, formatStr );

      if ( ULogCore_IsEnabled( h, &core ) == TRUE )
      {
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_GetTeeLocks( h );
        }
        *dataPtr = ULOG_REALTIME_SUBTYPE_PRINTF;
#ifdef ULOG_64BIT_PROC //set format pointer size indication bit
        *dataPtr++ |= ULOG_FORMAT_STR_SIZE_BIT;
#else
        *dataPtr++ &= ~ULOG_FORMAT_STR_SIZE_BIT;
#endif
#ifdef ULOG_64BIT_TIME
        if ( log->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE( log ) )
        {
          timestamp = log->altTimestampSrcFn();
        }
        else
        {
          timestamp = CoreTimetick_Get64();
        }

        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp >> 32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif

#ifndef ULOG_64BIT_PROC
        *dataPtr++ = (uint32)formatStr;
#else
        *dataPtr++ = (uint32)((uint64)formatStr);
        *dataPtr++ = (uint32)((uint64)formatStr >> 32);
#endif

        //any arguments
        for ( i = 0; i < dataCount; i++ )
        {
           if ( paramBitField & (1 << i) ) //64-bit parameter
           {
             tempValue = (uint64)va_arg( ap, uint64 );
             *dataPtr++ = (uint32)((uint64)tempValue);
             *dataPtr++ = (uint32)((uint64)tempValue >> 32);
           }
           else //32-bit parameter
           {
             *dataPtr++ = va_arg( ap, uint32 );
           }
        }

        retval = core->write( h, total_size, (char *)dataArray,  0, NULL, ULOG_INTERFACE_REALTIME );

        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_ReleaseTeeLocks( h );
        }
      }
      va_end(ap);
      return retval;


      case ULOG_FAST_TO_RAM_FAIL:
      //the log is disabled, or some argument is wrong.
        return(DAL_ERROR);
  }

  return(DAL_ERROR);
}


/**
 * <!-- ULogFront_RealStaticTimePrintf -->
 *
 * @brief Log data in the printf format to the specified static log slot.
 *
 * Do not use this API.  Use the ULOG_RT_STATIC_PRINTF_n macros.
 *
 * @param h : A ULog handle.
 * @param msgSlot : The message slot to be written.
 * @param dataCount : The number of parameters being printed (Not including
 *                    the formatStr).  Must be 10 parameters for static logs. 
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeStaticPrintf(ULogHandle h, uint32 msgSlot, uint32 dataCount, const char * formatStr, ...)
{
  va_list args;
  ULogResult retval = DAL_ERROR;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if ( (dataCount != 10) || (h == NULL) )
  {
    return(DAL_ERROR);
  }

  //don't try to write to a slot larger than the number of defined slots.
  CORE_VERIFY(msgSlot < log->staticMsgSlots);

  ULogInternal_LockGet(log);

  //move the write pointer to the correct slot to place the message.
  log->writerCore->write = msgSlot * ULOGSTATIC_MESSAGE_SIZE;

  va_start(args, formatStr);
  
  //StaticPrintfs always uses 10 args.
  retval = ULogFront_RealTimeVprintf(h, 10, formatStr, args);
  
  va_end(args);

  //move the write pointer to the end of the log
  log->writerCore->write = log->staticMsgSlots * ULOGSTATIC_MESSAGE_SIZE;

  ULogInternal_LockRelease(log);
 
  return retval;
}


/**
 * <!-- ULogFront_RealTimeData -->
 *
 * @brief Log uint32 data to a RealTime log.
 *
 * Data is provided as variatic arguments to this function.
 *  
 * NOTE: On a 64-bit system, string pointers will be treated as 
 * two uint32 values. If the dataCount passed into 
 * this function does not account for this, parsing issues may 
 * occur 
 *  
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being logged.  Can
 *                  be a maximum of 10.
 * @param ... : The parameters to log.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeData(ULogHandle h, uint32 dataCount, ...)
{
  uint32 total_size;
  uint32 dataArray[14];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  va_list ap;
  uint32 i;
  uint32* wordPtr;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
 
  if ((dataCount > 10) ||
      (dataCount == 0) ||
      (h == NULL))
  {
    return(DAL_ERROR);
  }

  //size/fmt + TS + data
  total_size = 4 + ULOG_TIMESTAMP_SIZE + dataCount*4; 

  switch ( ULogInternal_FastFmtSizeTS( h, total_size, ULOG_REALTIME_SUBTYPE_WORDDATA ) )
  {

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg.
      va_start(ap, dataCount);
      for(i = 0; i < dataCount; i++)
      {
        wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        *wordPtr = (uint32)va_arg(ap, uint32);
        log->writerCore->write+=4;
      }
      va_end(ap);

      //fill in the size
      wordPtr = (uint32*)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount)<<ULOG_LENGTH_SHIFT)& ULOG_LENGTH_MASK;

      ULogInternal_LockRelease(log);
      return (DAL_SUCCESS);


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes.
      if(ULogCore_IsEnabled(h, &core) == TRUE)   
      {
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_GetTeeLocks( h );
        }
        *dataPtr++ = ULOG_REALTIME_SUBTYPE_WORDDATA; 
#ifdef ULOG_64BIT_TIME
        if(((ULOG_TYPE*)h)->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(((ULOG_TYPE*)h)))
        {
            timestamp = ((ULOG_TYPE*)h)->altTimestampSrcFn();
        }
        else
        {
          timestamp = CoreTimetick_Get64();
        }
        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp>>32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif
        va_start(ap, dataCount);
        for(i = 0; i < dataCount; i++)
        {
          *dataPtr++ = va_arg(ap, uint32);
        }
        va_end(ap);

        retval = core->write (h, total_size, (char *)dataArray, 0, NULL, ULOG_INTERFACE_REALTIME);
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_ReleaseTeeLocks( h );
        }
      }
      return retval;


      case ULOG_FAST_TO_RAM_FAIL:
      //the log is disabled, or some argument is wrong.
        return(DAL_ERROR);
  }

  return(DAL_ERROR);
}


/**
 * <!-- ULogFront_RealTimeCharArray -->
 *
 * @brief Log byte data to a RealTime log.  
 *
 * core->write will use memcpy on the passed in array so we're not doing the 
 * ULogInternal_FastFmtSizeTS type optimizations we do in other functions. 
 * 
 * @param h : A ULog handle.
 * @param byteCount : the number of bytes to log.
 * @param byteData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeCharArray( ULogHandle h,
                                        uint32 byteCount,
                                        char * byteData )
{
  uint32 logData[3];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;


  if ((byteCount == 0) ||
      (byteCount > (1020-ULOG_TIMESTAMP_SIZE)) ||
       (h == NULL) )
  {
    return(DAL_ERROR);
  }

  if((byteData != NULL) && (ULogCore_IsEnabled(h, &core) == TRUE))
  {
    logData[0] = ULOG_REALTIME_SUBTYPE_BYTEDATA;

#ifdef ULOG_64BIT_TIME
    if(((ULOG_TYPE*)h)->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(((ULOG_TYPE*)h)))
    {
      timestamp = ((ULOG_TYPE*)h)->altTimestampSrcFn();
    }
    else
    {
      timestamp = CoreTimetick_Get64();
    }
    logData[1] = (uint32)timestamp;
    logData[2] = (uint32)((uint64)timestamp>>32);
#else
    logData[1] = CoreTimetick_Get();
#endif
    //len/format, and time, and then bytes
    retval = core->write(h, 4 + ULOG_TIMESTAMP_SIZE , (char *)logData,  byteCount, byteData, ULOG_INTERFACE_REALTIME);

  }
  return retval;
}


/**
 * <!-- ULogFront_RealTimeString -->
 *
 * @brief Log string data to a RealTime log.
 *
 * This function copies the full string to the log.
 * core->write will use memcpy on the passed in array so we're not doing the 
 * ULogInternal_FastFmtSizeTS type optimizations we do in other functions. 
 *
 * @param h : A ULog handle.
 * @param string : The string to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeString(ULogHandle h, char * cStr)
{
  uint32 logData[3];
  size_t length;
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;

  if ( (cStr == NULL) || (h == NULL) )
  {
    return(DAL_ERROR);
  }

  length = strlen( cStr );  //Note: we don't include the NULL termination in the stored string.

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    logData[0] = ULOG_REALTIME_SUBTYPE_STRINGDATA;
    
#ifdef ULOG_64BIT_TIME
    if(((ULOG_TYPE*)h)->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(((ULOG_TYPE*)h)))
    {
      timestamp = ((ULOG_TYPE*)h)->altTimestampSrcFn();
    }
    else
    {
      timestamp = CoreTimetick_Get64();
    }

    logData[1] = (uint32)timestamp;
    logData[2] = (uint32)((uint64)timestamp>>32);
#else
    logData[1] = CoreTimetick_Get();
#endif

    retval = core->write (h, 4 + ULOG_TIMESTAMP_SIZE, (char *)logData, length, cStr, ULOG_INTERFACE_REALTIME);
  }

  return retval;
}


/**
 * <!-- ULogFront_RealTimeWordBaseFcn -->
 *
 * @brief A generic function to write word data into the log.
 *
 * core->write will use memcpy on the passed in array so we're not doing the 
 * ULogInternal_FastFmtSizeTS type optimizations we do in other functions. 
 *
 */
ULogResult ULogFront_RealTimeWordBaseFcn( ULogHandle h,
                                          uint32 wordCount,
                                          const uint32 * wordData,
                                          uint32 subformatId )
{
  uint32 logData[3];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;

  if ((wordData==NULL)  ||
      (wordCount == 0)  ||
      (wordCount > 200) || /*Setting a large max value here.*/
      (h==NULL))
  {
    return(DAL_ERROR);
  }

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    logData[0] = subformatId;
#ifdef ULOG_64BIT_TIME
    if(((ULOG_TYPE*)h)->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(((ULOG_TYPE*)h)))
    {
      timestamp = ((ULOG_TYPE*)h)->altTimestampSrcFn();
    }
    else
    {
      timestamp = CoreTimetick_Get64();
    }

    logData[1] = (uint32)timestamp;
    logData[2] = (uint32)((uint64)timestamp>>32);
#else
    logData[1] = CoreTimetick_Get();
#endif
    retval = core->write (h, 4 + ULOG_TIMESTAMP_SIZE, (char *)logData, wordCount*4, (char *)wordData, ULOG_INTERFACE_REALTIME);
  }

  return retval;
}


/**
 * <!-- ULogFront_RealTimeWordArray -->
 *
 * @brief Log word data to a RealTime log.
 *
 * @param h : A ULog handle.
 * @param wordCount : The number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeWordArray( ULogHandle h,
                                        uint32 wordCount,
                                        const uint32 * wordData )
{
  return(ULogFront_RealTimeWordBaseFcn(h, wordCount, wordData, ULOG_REALTIME_SUBTYPE_WORDDATA));
}


/**
 * <!-- ULogFront_RealTimeCsv -->
 *
 * @brief Log word data to a RealTime log. Output will be a CSV compatible format.
 *
 * @param h  : A ULog handle.
 * @param wordCount : the number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeCsv( ULogHandle h,
                                  uint32 wordCount,
                                  const uint32 * wordData )
{
  return(ULogFront_RealTimeWordBaseFcn(h, wordCount, wordData, ULOG_REALTIME_SUBTYPE_CSVDATA));
}


/**
 * <!-- ULogFront_RealTimeVector -->
 *
 * @brief Log vector data to a RealTime log.
 *
 * Output will be a vector with the provided format. No newlines will be inserted
 *
 * @param h : A ULog handle.
 * @param formatStr : A format string with a single %x, %d, %i, or %u 
 * @param entryByteCount : 1, 2 or 4 byte size data 
 * @param vectorLength :  How man elements to store. 
 * @param vector : Pointer to the array of elements.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeVector( ULogHandle h,
                                     const char * formatStr,
                                     unsigned short entryByteCount,
                                     unsigned short vectorLength,
                                     const void * vector )
{
  ULogResult retval = DAL_ERROR;
  uint32 logData[8];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  ULOG_CORE_VTABLE * core;

  if ((vector == NULL)     ||
      (formatStr ==  NULL) ||
      (vectorLength == 0)   || 
      (vectorLength > 200) || /*Setting a large max value here.*/
      (h==NULL))
  {
    return(DAL_ERROR);
  }
  
  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    // We only handle these byte entry values.
    if ((entryByteCount == 4) ||
        (entryByteCount == 1) ||
        (entryByteCount == 2))
    {
      uint32 i = 0;
      logData[i++] = ULOG_REALTIME_SUBTYPE_VECTOR;
#ifdef ULOG_64BIT_TIME

      if ( ((ULOG_TYPE *)h)->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE( ((ULOG_TYPE *)h) ) )
      {
        timestamp = ((ULOG_TYPE *)h)->altTimestampSrcFn();
      }
      else
      {
        timestamp = CoreTimetick_Get64();
      }

      logData[i++] = (uint32)timestamp;
      logData[i++] = (uint32)((uint64)timestamp>>32);
#ifndef ULOG_64BIT_PROC
      logData[i++] = (uint32)formatStr;
#else
      logData[i++] = (uint32)((uint64)formatStr);
      logData[i++] = (uint32)((uint64)formatStr >> 32);
#endif
      logData[i++] = entryByteCount << 16 | vectorLength;
#else
      logData[i++] = CoreTimetick_Get();
#ifndef ULOG_64BIT_PROC
      logData[i++] = (uint32)formatStr;
#else
      logData[i++] = (uint32)((uint64)formatStr);
      logData[i++] = (uint32)((uint64)formatStr >> 32);
#endif
      logData[i++] = entryByteCount << 16 | vectorLength;
#endif
      retval = core->write( h,  8 + ULOG_FORMAT_STRING_SIZE + ULOG_TIMESTAMP_SIZE, (char *)logData, vectorLength * entryByteCount, (char *)vector, ULOG_INTERFACE_REALTIME );
    }
  }
  return retval;
}


/**
 * <!-- ULogFront_RealTimeMultipartMsgBegin -->
 *
 * @brief Begin a multipart message.
 *
 * The log will be locked for the duration of the multipart 
 * message. Each of the messages following the call to 
 * ULogFront_RealTimeMultipartMsgBegin will be have the contents
 * of that message appended to the end of the previous message. 
 * Calling ULogFront_RealTimeMultipartMsgEnd will free the logs 
 * lock and messages after this call will no longer be appended 
 * to the multipart message, they will appear as normal ulog 
 * messages 
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 *
 * @return TRUE if the mpm was successfully started. False if it wasn't and
 *         the multipart message should not be continued further
 */
DALBOOL ULogFront_RealTimeMultipartMsgBegin(ULogHandle h)
{
  ULOG_CORE_VTABLE * core;
  ULOG_TYPE * log = (ULOG_TYPE *)h;

  if (h==NULL)
  {
    return(FALSE);
  }

#ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log))
  {
    ULOG_TEE_LINK_TYPE * link;
    ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)log;
    link = tee->link;
    while (link != NULL)
    {
      if ((link->log != NULL) &&
          (link->log->sharedHeader != NULL))
      {
        ULogInternal_LockGet(link->log);      
      }
      link = link->next;
    }
  }
#endif

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    return(core->multipartMsgBegin(h));
  }

  return(FALSE);
}


/**
 * <!-- ULogFront_RealTimeMultipartMsgEnd -->
 *
 * @brief End a multipart message
 *
 * Write the size into the start of the MPM and release the MPM lock 
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 */
void ULogFront_RealTimeMultipartMsgEnd(ULogHandle h)
{
  ULOG_CORE_VTABLE * core;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  
  if (h==NULL)
  {
    return;
  }

  if(ULogCore_IsEnabled(h, &core) == TRUE)
  {
    core->multipartMsgEnd(h);
  }

#ifndef DEFEATURE_ULOG_TEE
  if (HANDLE_IS_A_TEE(log))
  {
    ULOG_TEE_LINK_TYPE * link;
    ULOG_TEE_TYPE * tee = (ULOG_TEE_TYPE *)log;
    link = tee->link;
    while (link != NULL)
    {
      if ((link->log != NULL) &&
          (link->log->sharedHeader != NULL))
      {
        ULogInternal_LockRelease(link->log);      
      }
      link = link->next;
    }
  } 
#endif
}
#endif /* DEFEATURE_ULOG_FRONT_REALTIME */


/**
 * <!-- ULogFront_RawInit -->
 *
 * @brief Create a new log in the RAW format.
 * 
 * Create a new raw log, See ULogCore_LogCreate for details.
 * 
 * @param h : See ULogCore_LogCreate.
 * @param name : See ULogCore_LogCreate.
 * @param logBufSize : See ULogCore_LogCreate.
 * @param logBufMemType : See ULogCore_LogCreate.
 * @param logLockType : See ULogCore_LogCreate.
 *
 * @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RawInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType)
{
    return(ULogCore_LogCreate(h, name, logBufSize, 
                              logBufMemType, 
                              logLockType, 
                              ULOG_INTERFACE_RAW));
}


/**
 * <!-- ULogFront_RawLog -->
 *
 * @brief Log data to a raw log.
 *
 * @param h : A ULog handle.
 * @param dataArray : A pointer to the data to log.
 * @param dataCount : The amount of data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RawLog( ULogHandle h,
                             const char * dataArray,
                             uint32 dataCount )
{
  uint32 total_size;
  uint32* wordPtr;

  unsigned int padding=0;
  unsigned int bytesToEnd, copy1Count, copy2Count;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  ULOG_CORE_VTABLE * core;

  if ((dataArray == NULL) ||
      (h == NULL))
  {
    return(DAL_ERROR);
  }

  //len/fmt word (4 bytes) + data(at least 1 byte)
  total_size = 4 + dataCount;

  if ( total_size < MIN_RAW_MSG_LEN )
  {
    return(DAL_ERROR);
  }

  if ( ULogInternal_UseFastRam( log ) )
  {
    
    ULogInternal_LockGet(log);

    if(ULogInternal_ReadyForMessage(log, total_size) == FALSE)
    {
      ULogInternal_LockRelease(log);
      return(DAL_ERROR);
    }

    // len/fmt word  (Raw logs set format to 0)
    wordPtr = (uint32*)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
    //save this important index and size to use at the end of the write
    log->transactionWriteStart = log->writerCore->write;
    *wordPtr = 0;  //keep the size zero until the msg is completely written.

    //advance the write pointer past the fmt/size word we just wrote
    log->writerCore->write+=4;

    //memcpy the rest of the bytes into the buffer.
    bytesToEnd = log->bufSize - (log->writerCore->write & log->bufSizeMask);

    if (dataCount <= bytesToEnd)
    {
      core_memcpy( &(log->buffer[log->writerCore->write & log->bufSizeMask]),
                   dataCount, dataArray, dataCount );
    }
    else
    {
      copy1Count = bytesToEnd;
      copy2Count = dataCount - bytesToEnd;
      core_memcpy( &log->buffer[log->writerCore->write & log->bufSizeMask],
                   copy1Count, dataArray, copy1Count );
      core_memcpy( &log->buffer[0], copy2Count, &dataArray[copy1Count], copy2Count );
    }
  
    //make sure the write pointer ends on a word boundary
    if ((dataCount & 0x03) != 0)
    {
       padding = 4 - (dataCount & 0x03);
    }

    // Increment the write pointer
    log->writerCore->write += dataCount + padding;
    
    //write the final size for this message into the buffer.
    wordPtr = (uint32 *)&log->buffer[log->transactionWriteStart & log->bufSizeMask];
    *wordPtr |= (total_size << ULOG_LENGTH_SHIFT) & ULOG_LENGTH_MASK;

    ULogInternal_LockRelease(log);
    return(DAL_SUCCESS);
  }
  else
  {
    //it's an STM or other type of write.  Use the generic write function.
    padding = 0;  //the format is zero, the write will fill in the size.
    if(ULogCore_IsEnabled(h, &core) == TRUE)
    {
      return(core->write(h, 4, (char*)&padding, dataCount, dataArray, ULOG_INTERFACE_RAW));
    }
  }

  return(DAL_ERROR);
}

#ifdef TARGET_UEFI //sync with ULogFront_RealTimePrintf_Ex 
/**
 * <!-- ULogFront_RealTimeVprintf_Ex -->
 *
 * @brief Log data in the printf format.
 *
 * Log printf data to a RealTime log.  The printf formating is
 * not executed until the log is read. This makes a very performant call,
 * but also means all strings must be present when the log is read.
 * This function does the most work for ULog logging.  It's been optimized to 
 * write directly to the buffer if possible. 
 *
 * @param h : A ULog handle.
 * @param dataCount : the number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ap : A va_list of the parameters being printed.
 * 
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
  */
ULogResult ULogFront_RealTimeVprintf_Ex( ULogHandle h,
                                      uint32 dataCount,
                                      uint32 paramBitField,
                                      const char * formatStr,
                                      va_list ap )
{
  uint32 total_size;
  uint32 dataArray[14];
#ifdef ULOG_64BIT_TIME
  uint64 timestamp;
#endif
  uint32 * dataPtr = dataArray;
  ULOG_TYPE * log = (ULOG_TYPE *)h;
  uint32 * wordPtr;
  uint32 i;
  ULogResult retval = DAL_ERROR;
  ULOG_CORE_VTABLE * core;
  uint64 tempValue;
  uint32 num64bitParams;

  num64bitParams = ULogInternal_CountBitsSet( paramBitField );

  if ( (dataCount > 10) || (h == NULL) || (num64bitParams > 10) )
  {
    return ( DAL_ERROR );
  }

  // msgSize is len/fmt(4) + format string + TS + data
  total_size = 4 + ULOG_FORMAT_STRING_SIZE + ULOG_TIMESTAMP_SIZE + (dataCount - num64bitParams) * 4 + num64bitParams * 8;

  switch ( ULogInternal_FastFmtSizeTS( h, total_size, ULOG_REALTIME_SUBTYPE_PRINTF ) )
  {

    case ULOG_FAST_TO_RAM_SUCCESS:
      //Success.  We have the lock, and will release it after we finish this msg.

      //pointer to the format string
      wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
#ifndef ULOG_64BIT_PROC
      *wordPtr = (uint32)formatStr;
      log->writerCore->write += 4;
#else
      *wordPtr = (uint32)((uint64)formatStr);
      log->writerCore->write += 4;
      wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
      *wordPtr = (uint32)((uint64)formatStr >> 32);
      log->writerCore->write += 4;
#endif

      //any arguments
      for ( i = 0; i < dataCount; i++ )
      {
        wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
        if ( paramBitField & (1 << i) ) //64-bit parameter
        {
          tempValue = (uint64)va_arg( ap, uint64 );
          *wordPtr = (uint32)((uint64)tempValue);
          log->writerCore->write += 4;
          wordPtr = (uint32 *)&(log->buffer[log->writerCore->write & log->bufSizeMask]);
          *wordPtr = (uint32)((uint64)tempValue >> 32);
          log->writerCore->write += 4;
        }
        else //32-bit parameter
        {
          *wordPtr = (uint32)va_arg( ap, uint32 );
          log->writerCore->write += 4;
        }

      }

      //fill in the size/param size bitfield
      wordPtr = (uint32 *)&(log->buffer[log->transactionWriteStart & log->bufSizeMask]);
      *wordPtr |= ((log->transactionWriteCount) << ULOG_LENGTH_SHIFT) & ULOG_LENGTH_MASK;
      *wordPtr |= (paramBitField << ULOG_ARG_BITFIELD_SHIFT) & ULOG_ARG_BITFIELD_MASK;

      ULogInternal_LockRelease( log );
      return ( DAL_SUCCESS );


    case ULOG_FAST_TO_RAM_UNAVAIL:
      //It's not a normal RAM log, fill in the array and call the generic writes.
      //Use the virtualized core->write to handle Tee, STM or Alternate writes.

      if ( ULogCore_IsEnabled( h, &core ) == TRUE )
      {
        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_GetTeeLocks( h );
        }
        *dataPtr = ULOG_REALTIME_SUBTYPE_PRINTF;
#ifdef ULOG_64BIT_PROC //set format pointer size indication bit
        *dataPtr++ |= ULOG_FORMAT_STR_SIZE_BIT;
#else
        *dataPtr++ &= ~ULOG_FORMAT_STR_SIZE_BIT;
#endif
#ifdef ULOG_64BIT_TIME
        if ( log->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE( log ) )
        {
          timestamp = log->altTimestampSrcFn();
        }
        else
        {
          timestamp = CoreTimetick_Get64();
        }

        *dataPtr++ = (uint32)timestamp;
        *dataPtr++ = (uint32)((uint64)timestamp >> 32);
#else
        *dataPtr++ = CoreTimetick_Get();
#endif

#ifndef ULOG_64BIT_PROC
        *dataPtr++ = (uint32)formatStr;
#else
        *dataPtr++ = (uint32)((uint64)formatStr);
        *dataPtr++ = (uint32)((uint64)formatStr >> 32);
#endif
        //any arguments
        for ( i = 0; i < dataCount; i++ )
        {
           if ( paramBitField & (1 << i) ) //64-bit parameter
           {
             tempValue = (uint64)va_arg( ap, uint64 );
             *dataPtr++ = (uint32)((uint64)tempValue);
             *dataPtr++ = (uint32)((uint64)tempValue >> 32);
           }
           else //32-bit parameter
           {
             *dataPtr++ = va_arg( ap, uint32 );
           }
        }

        retval = core->write( h, total_size, (char *)dataArray,  0, NULL, ULOG_INTERFACE_REALTIME );

        if ( HANDLE_IS_A_TEE( log ) )
        {
          ULogInternal_ReleaseTeeLocks( h );
        }
      }
      return retval;


    case ULOG_FAST_TO_RAM_FAIL:
      //the log is disabled, or some argument is wrong.
      return ( DAL_ERROR );
  }

  return ( DAL_ERROR );
}
#endif //TARGET_UEFI

#ifdef __cplusplus
}
#endif
