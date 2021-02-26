/*============================================================================
  @file ULogFront.c

  Frontend interfaces for ULog Core.  Simplified access to the logging mechanism.

  Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 05/22/15   rli     Merged with UEFI.3.1 (config API, global protocol)
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIULog.h>

extern EFI_ULOG_PROTOCOL * gULogProtocol;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_RAW
 * -------------------------------------------------------------------------*/

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
ULogResult ULogFront_RawInit(ULogHandle * h,  const char * name, uint32 logBufSize, uint32 logBufMemType, ULOG_LOCK_TYPE logLockType)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol == NULL)
    {
        efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
    }
    if(efistatus == EFI_SUCCESS)
    {
        gULogProtocol->ULogFront_RawInit( &retvalue, h, name, logBufSize, logBufMemType, logLockType );
    }
    return retvalue;
}

/**
 * <!-- ULogFront_RawLog -->
 *
 * @brief Log data to a raw log.
 *
 * @param h  : A ULog handle.
 * @param dataArray : A pointer to the data to log.
 * @param dataCount : The amount of data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RawLog(ULogHandle h,  const char * dataArray, uint32 dataCount)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RawLog( &retvalue, h, dataArray, dataCount );
    }
    return retvalue;
}

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_REALTIME
 * -------------------------------------------------------------------------*/

/**
 * <!-- ULogFront_RealTimeInit -->
 *
 * @brief Create a new log in the RealTime format.
 *
 * Create a new RealTime log, See ULogCore_LogCreate for details.
 *
 * @param h : See ULogCore_LogCreate.
 * @param logName : See ULogCore_LogCreate.
 * @param bufferSize : See ULogCore_LogCreate.
 * @param memoryType : See ULogCore_LogCreate.
 * @param lockType : See ULogCore_LogCreate.
 *
 * @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RealTimeInit(ULogHandle * h,  const char * name, uint32 logBufSize, uint32 logBufMemType, ULOG_LOCK_TYPE logLockType)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol == NULL)
    {
        efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
    }
    if(efistatus == EFI_SUCCESS)
    {
        gULogProtocol->ULogFront_RealTimeInit( &retvalue, h, name, logBufSize, logBufMemType, logLockType );
    }
    return retvalue;
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
 * @param dataArray : The number of parameters being printed (Not
 *                    including the formatStr).  Limited to 10
 *                    parameters.
 * @param formatStr : The format string for the printf.
 * @param ... : The parameters to print.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimePrintf(ULogHandle h, 
                                    uint32 dataCount, 
                                    const char * formatStr,
                                    ...)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    va_list    ap;

    if (gULogProtocol != NULL)
    {
        va_start(ap, formatStr);
        gULogProtocol->ULogFront_RealTimeVprintf( &retvalue, h, dataCount, formatStr, ap );
        va_end(ap);
    }
    return retvalue;
}


/**
 * <!-- ULogFront_RealTimeVprintf -->
 * 
 * @brief Log data in the vprintf format.
 *
 * Log printf data to a RealTime log.  The printf formativ is not
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
ULogResult ULogFront_RealTimeVprintf(ULogHandle h,  uint32 dataCount, const char * formatStr, va_list ap)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeVprintf( &retvalue, h, dataCount, formatStr, ap );
    }
    return retvalue;
}

/**
 * <!-- ULogFront_RealTimeCharArray -->
 *
 * @brief Log byte data to a RealTime log.  
 * 
 * @param h : A ULog handle.
 * @param byteCount : the number of bytes to log.
 * @param byteData : A pointer to the data to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCharArray(ULogHandle h,  uint32 byteCount, char * byteData)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeCharArray( &retvalue, h, byteCount, byteData );
    }
    return retvalue;
}


/**
 * <!-- ULogFront_RealTimeString -->
 *
 * @brief Log string data to a RealTime log.
 *
 * This function copies the full string to the log.
 *
 * @param h : A ULog handle.
 * @param string : The string to log.
 *
 * @return DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogFront_RealTimeString(ULogHandle h, char * cStr)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeString( &retvalue, h, cStr );
    }
    return retvalue;
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
ULogResult ULogFront_RealTimeWordArray(ULogHandle h,  uint32 wordCount, const uint32 * wordData)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeWordArray( &retvalue, h, wordCount, wordData );
    }
    return retvalue;
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
ULogResult ULogFront_RealTimeCsv(ULogHandle h,  uint32 wordCount, const uint32 * wordData)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeCsv( &retvalue, h, wordCount, wordData );
    }
    return retvalue;
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
ULogResult ULogFront_RealTimeVector(ULogHandle h,  const char * formatStr, unsigned short entryByteCount, unsigned short vectorLength, const void * vector)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeVector( &retvalue, h, formatStr, entryByteCount, vectorLength, vector );
    }
    return retvalue;
}


/**
 * <!-- ULogFront_RealTimeMultipartMsgBegin -->
 *
 * @brief Begin a multipart message.
 *
 * The log will be locked for the duration of the multipart message.  To
 * free the lock, call ULogFront_RealTimeMultipartMsgEnd. The first message of a 
 * MPM is always a ULogFront_RealTimePrintf with %m specifiers for the parts to 
 * add in the subsequent logging before calling ULogFront_RealTimeMultipartMsgEnd.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 *
 * @return TRUE if the mpm was successfully started. False if it wasn't and
 *         the multipart message should not be continued further
 */
DALBOOL ULogFront_RealTimeMultipartMsgBegin (ULogHandle h)
{
    DALBOOL  retvalue = 0;
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeMultipartMsgBegin( &retvalue, h );
    }
    return retvalue;
}


/**
 * <!-- ULogFront_RealTimeMultipartMsgEnd -->
 *
 * @brief End a multipart message
 *
 * The log will be locked for the duration of the multipart message.  
 * This function frees the lock, ends the MPM message and completes the MPM bookeeping.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 */
void ULogFront_RealTimeMultipartMsgEnd (ULogHandle h)
{
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogFront_RealTimeMultipartMsgEnd( h );
    }
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
ULogResult ULogFront_RealTimePrintf_Ex(ULogHandle h, 
                                    uint32 dataCount, 
                                    uint32 paramBitField,
                                    const char * formatStr,
                                    ...)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
    va_list    ap;

    if (gULogProtocol != NULL)
    {
        va_start(ap, formatStr);
        gULogProtocol->ULogFront_RealTimeVprintf_Ex( &retvalue, h, dataCount, paramBitField, formatStr, ap );
        va_end(ap);
    }
    return retvalue;
}

/**
 * <!-- ULogFile_Open -->
 *
 * Open a ULog file on disk, creating it if it doesn't exist
 * The file name is ULOGFILE_DIR<log_name>.ULOGFILE_EXTENSION
 *
 * The handle for the log file is stored in the ULog entry to
 * improve performance.
 * @param h: File handle for the log
 * @param fileName:  Name of the file to open, will be truncated if
 *                    size > ULOG_MAX_NAME_SIZE
 * @param fileSize:  Maximum log file size in bytes
 * @return DAL_SUCCESS upon success, either DAL_ERROR or the
 *         Appropriate ULOG_ERROR error on failure.
 *
 */
ULogResult ULogFile_Open( ULogHandle * h, const char *fileName, uint32 fileSize )
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
    if (gULogProtocol == NULL)
    {
        efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
    }
    if(efistatus == EFI_SUCCESS)
    {
        gULogProtocol->ULogFile_Open( &retvalue, h, fileName, fileSize);
    }
    return retvalue;
}

/**
 * <!-- ULogFile_SetConfig -->
 * @brief Modify the file logging settings
 *
 * @param h: Pointer to the ULog handle to modify
 * @param settings: The new settings to apply
 *
 * @return EFI_SUCCESS if all input settings are valid
 *         ULOG_ERR_XX otherwise
 */
ULogResult ULogFile_SetConfig(ULogHandle *h, CONST ULOG_CONFIG_TYPE *settings){
    ULogResult retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
      gULogProtocol->ULogFile_SetConfig( &retvalue, h, settings );
    }
    return retvalue;
}

/**
 * <!-- ULogFile_GetConfig -->
 * @brief Return the file logging settings
 *
 * @param h: Pointer to the handle for the ULog entry in question
 * @param settings: Pointer to the caller-alloc'd buffer in which to return the settings
 *
 * @return EFI_SUCCESS if settings could be copied successfully
 *         ULOG_ERR_XX otherwise
 */
ULogResult ULogFile_GetConfig(ULogHandle *h, ULOG_CONFIG_TYPE *settings){
    ULogResult retvalue = ULOG_ERR_MISC;
    if (gULogProtocol != NULL)
    {
      gULogProtocol->ULogFile_GetConfig( &retvalue, h, settings );
    }
    return retvalue;
}
