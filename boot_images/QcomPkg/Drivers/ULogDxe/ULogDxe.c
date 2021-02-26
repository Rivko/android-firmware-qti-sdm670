/** @file ULogDxe.c

  This file implements ULog EFI protocol interface.

  Copyright (c) 2013-2015, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/30/15   sj/rli   Added EFI_ULogCore_ReadSessionComplete API
 05/22/15   rli      Added ULogFile config API
 11/24/14   sj       Using memset to initialize
 10/02/14   vpopuri  Merge with ULOGv5, code cleanup
 08/14/14   vpopuri  Added UEFI File logging interface
 03/25/14   rli      Fix compiler warning
 03/27/13   rli      Initial revision.

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIULog.h>
#include "DALSys.h"
#include "ULogFile.h"


/*=========================================================================
      Functions
==========================================================================*/

/*=========================================================================
      ULog
==========================================================================*/


/**
 * <!-- EFI_ULogCore_Init -->
 *
 * @brief Initialize the ULog System.
 *
 * It is not strictly necessary to call this function since it
 * will be executed any time an attempt is made to connect to, or
 * create a log.
 *
 * @param retvalue : DAL_SUCCESS if the initialization is successful.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Init( ULogResult *retvalue)
{
    *retvalue = ULogCore_Init (  );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_LogCreate -->
 *
 * @brief  - Create a new log.
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
 * @param retvalue : DAL_SUCCESS if the create completed successfully.  The log will have been
 *        enabled and will be ready to use. 
 *        ULOG_ERR_INITINCOMPLETE if ULOG_MEMORY_LOCAL_MANUAL is used.  
 *        ULOG_ERR_INITINCOMPLETE if a buffersize of 0 is used.  
 *        The log can be enabled later after ULogCore_MemoryAssign sets up all the parts.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_LogCreate
(
    ULogResult *retvalue,
    ULogHandle * h,
    const char *  logName,
    uint32  bufferSize,
    uint32  memoryType,
    ULOG_LOCK_TYPE  lockType,
    ULOG_INTERFACE_TYPE  interfaceType
)
{
    *retvalue = ULogCore_LogCreate ( h, logName, bufferSize, memoryType, lockType, interfaceType );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_HeaderSet -->
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
 * @param h: A ULog handle.
 * @param headerText: The string to store in the log.
 *
 * @param retvalue : DAL_SUCCESS if the header was successfully stored.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_HeaderSet
(
    ULogResult *retvalue,
    ULogHandle h,
    char * headerText
)
{
    *retvalue = ULogCore_HeaderSet ( h, headerText );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_AttributeSet -->
 *
 * @brief Set the log attributes
 * 
 * OR in the provided attribute bits into the logs attribute mask.
 *
 * @param h : The log to set attributes for
 * @param attribute : The attributes to set
 *
 * @param retvalue : A handle to the named log
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_AttributeSet
(
    ULogHandle h,
    uint32 attribute
)
{
    ULogCore_AttributeSet ( h, attribute );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_MemoryAssign -->
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
 *                   Note that ALL buffers must be word aligned.
 * @param bufferSize : The size in bytes of the buffer.
 *
 * @param retvalue : DAL_SUCCESS if the memory was assigned successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_MemoryAssign
(
    ULogResult *retvalue,
    ULogHandle h,
    ULOG_VALUE_ID id,
    void * bufferPtr,
    uint32  bufferSize
)
{
    *retvalue = ULogCore_MemoryAssign ( h, id, bufferPtr, bufferSize );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_Enable -->
 *
 * @brief Enable a log that is already defined.
 *
 * Enable a log.  A log can need manual enabling if it has manual
 * allocation, has been disabbed, or buffer memory was not assigned.
 *
 * @param h : A ULog handle.
 *
 * @param retvalue : DAL_SUCCESS if the log was enabled successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Enable
(
    ULogResult *retvalue,
    ULogHandle h
)
{
    *retvalue = ULogCore_Enable ( h );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_IsEnabled -->
 *
 * @brief Check if a log is enabled.
 *
 * @param h : A ULog handle.
 * @param core : A vtable to fill in with the appropriate
 *              interface functions for the current log
 *
 * @param retvalue : TRUE if the log is enabled.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_IsEnabled
(
    DALBOOL *retvalue,
    ULogHandle h,
    ULOG_CORE_VTABLE ** core
)
{
    *retvalue = ULogCore_IsEnabled ( h, core );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_Query -->
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
 * @param retvalue : DAL_SUCCESS if the buffer id was known, and supported.
 *         Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Query
(
    ULogResult *retvalue,
    ULogHandle h,
    ULOG_VALUE_ID id,
    uint32 * value
)
{
    *retvalue = ULogCore_Query ( h, id, value );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_Read -->
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
 * @param retvalue : DAL_SUCCESS if the read completed.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Read
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32  outputSize,
    char *  outputMem,
    uint32 *  outputCount
)
{
    *retvalue = ULogCore_Read ( h, outputSize, outputMem, outputCount );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_ReadEx -->
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
 *                             the buffer.
 *
 * @param retvalue : DAL_SUCCESS if the read completed.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_ReadEx
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32  outputSize,
    char *  outputMem,
    uint32 *  outputCount,
    uint32 outputMessageLimit
)
{
    *retvalue = ULogCore_ReadEx ( h, outputSize, outputMem, outputCount, outputMessageLimit );
    return EFI_SUCCESS;
}



/**
 *  <!-- EFI_ULogCore_ReadSessionComplete -->
 *
 * @brief Indicate that any previous read sessions are complete. No overrun detection needed 
 * on the very next read of this log. Also make sure rewind logs are rewound.
 *
 * This function is needed so that reading a log multiple times doesn't consider the
 * read jump at the start of reading to be an overrun.  Use this safely at the start
 * or end of your read session.
 *
 * @param h : A ULog handle of the log about to be read or just completed being read.
 * @param retvalue: DAL_SUCCESS if the read completed.
 * @return EFI_SUCCESS
 */

static EFI_STATUS EFI_ULogCore_ReadSessionComplete
(
  ULogResult *retvalue,
  ULogHandle h
)
{
    *retvalue = ULogCore_ReadSessionComplete( h);
    return EFI_SUCCESS;
}


/**
 * <!-- EFI_ULogCore_Allocate -->
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
 * @param retvalue : DAL_SUCCESS if the memory was allocated successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Allocate
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 bufferSize
)
{
    *retvalue = ULogCore_Allocate ( h, bufferSize );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_Disable -->
 *
 * @brief Disable a log. Drop message writes until re-enabled.
 *
 * @param h : A ULog handle.
 *
 * @param retvalue : DAL_SUCCESS if the log was disabled successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Disable
(
    ULogResult *retvalue,
    ULogHandle h
)
{
    *retvalue = ULogCore_Disable ( h );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_List -->
 *
 * @brief List of all logs available in the system, output as a text string.
 *
 * @param registeredCount : The number of logs that are registered.
 * @param nameArraySize : The size of the array that receives log names.
 * @param namesReadCount : The number of log names that were actually placed
 *                         in the nameArray.
 * @param nameArray : The buffer to receive log name data.
 *
 * @param retvalue : DAL_SUCCESS if the list was successfull. Errors are defined in
 *        the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_List
(
    ULogResult *retvalue,
    uint32 *  registeredCount,
    uint32  nameArraySize,
    uint32 *  namesReadCount,
    char *  nameArray
)
{
    *retvalue = ULogCore_List ( registeredCount, nameArraySize, namesReadCount, nameArray );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_ListEx -->
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
 *                        in the nameArray.
 * @param nameArray : The buffer to receive log name data.
 *
 * @param retvalue : DAL_SUCCESS if the list was successfull. Errors are defined in
 *        the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_ListEx
(
    ULogResult *retvalue,
    uint32 offsetCount,
    uint32 *  registeredCount,
    uint32  nameArraySize,
    uint32 *  namesReadCount,
    char *  nameArray
)
{
    *retvalue = ULogCore_ListEx ( offsetCount, registeredCount, nameArraySize, namesReadCount, nameArray );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_MsgFormat -->
 *
 * @brief Format an individual message retrieved with the ULogCore_Read function.
 *
 * Format and output an individual message read with the
 * ULogCore_Read function.
 *
 * @param h : A ULog handle to the log we are formatting.
 * @param msg : The input message.
 * @param msgString : The output string.
 * @param msgStringSize : The size of the output string
 * @param msgConsumed : Number of bytes consumed from the original msg buffer.
 *
 * @param retvalue : DAL_SUCCESS if the format was successful. Errors are
 *        defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_MsgFormat
(
    ULogResult *retvalue,
    ULogHandle h,
    char *  msg,
    char *  msgString,
    uint32 msgStringSize,
    uint32 * msgConsumed
)
{
    *retvalue = ULogCore_MsgFormat ( h, msg, msgString, msgStringSize, msgConsumed );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_HeaderRead -->
 * 
 * @brief Read the header from the supplied log.
 * 
 * @param h : A handle of the log to access.
 * @param statusString : The output string.
 * @param statusStringSize : The size of the output string
 * 
 * @param retvalue : DAL_SUCCESS if the format was successful. Errors are
 *         defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_HeaderRead
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 headerReadOffset,
    char * headerString,
    uint32 headerStringSize,
    uint32 * headerActualLength
)
{
    *retvalue = ULogCore_HeaderRead ( h, headerReadOffset, headerString, headerStringSize, headerActualLength );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_Connect -->
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
 * @param retvalue : DAL_SUCCESS if the connect was successful. Errors are
 *        defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_Connect
(
    ULogResult *retvalue,
    ULogHandle * h,
    const char * logName
)
{
    *retvalue = ULogCore_Connect ( h, logName );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_LogLevelSet -->
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
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_LogLevelSet
(
    ULogHandle h,
    uint32 level
)
{
    ULogCore_LogLevelSet ( h, level );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_LogLevelGet -->
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
 * @param retvalue : The current log value.  0 if the log handle is NULL.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_LogLevelGet
(
    uint32 *retvalue,
    ULogHandle h
)
{
    *retvalue = ULogCore_LogLevelGet ( h );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_TeeAdd -->
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
 * @param retvalue : A handle to the grouped logs
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_TeeAdd
(
    ULogHandle *retvalue,
    ULogHandle h1,
    ULogHandle h2
)
{
    *retvalue = ULogCore_TeeAdd ( h1, h2 );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_TeeRemove -->
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
 * @param retvalue : A handle to the resulting group of logs
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_TeeRemove
(
    ULogHandle *retvalue,
    ULogHandle h1,
    ULogHandle h2
)
{
    *retvalue = ULogCore_TeeRemove ( h1, h2 );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_HandleGet -->
 *
 * @brief Get a log handle by its name.
 *
 * @param logName: Name of a ULog to search for.
 *
 * @param retvalue : A handle to the named log
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_HandleGet
(
    ULogHandle *retvalue,
    char *logName
)
{
    *retvalue = ULogCore_HandleGet ( logName );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_IsLogPresent -->
 *
 * @brief Check if a named log is present in a log set
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 * @param logName : name of a ULog
 *
 * @param retvalue : TRUE if the named log is present in the log(s) indicated by the handle
 *        h, and FALSE otherwise
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_IsLogPresent
(
    DALBOOL *retvalue,
    ULogHandle h,
    char *logName
)
{
    *retvalue = ULogCore_IsLogPresent ( h, logName );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_SetTransportToRAM -->
 *
 * @brief Write log messages to the circular RAM buffer.
 * 
 * Writing messages to the circular RAM buffer is the default behavior for logs. 
 *
 * @param h : A ULog handle
 *
 * @param retvalue : DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_SetTransportToRAM
(
    ULogResult *retvalue,
    ULogHandle h
)
{
    *retvalue = ULogCore_SetTransportToRAM ( h );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_SetTransportToStm -->
 *
 * @brief Write log messages to the QDSS/STM transport
 *
 * @param h : A ULog handle
 * @param protocol_num : A QDSS protocol number (0 to 127) is assigned to the 
 *                      ULog handle and this protocol number can be used to
 *                      match logs to the handle when reading the QDSS output.
 *
 * @param retvalue : DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_SetTransportToStm
(
    ULogResult *retvalue,
    ULogHandle h,
    unsigned char protocol_num
)
{
    *retvalue = ULogCore_SetTransportToStm ( h, protocol_num );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_SetTransportToStmAscii -->
 *
 * @brief Write log messages as ASCII to the QDSS/STM transport
 *
 * @param h : A ULog handle
 * @param protocol_num : A QDSS protocol number (0 to 127) is assigned to the
 *                      ULog handle and can be used to identify log output when
 *                      reading the QDSS output.
 *
 * @param retvalue : DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_SetTransportToStmAscii
(
    ULogResult *retvalue,
    ULogHandle h,
    unsigned char protocol_num
)
{
    *retvalue = ULogCore_SetTransportToStmAscii ( h, protocol_num );
        return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogCore_SetTransportToAlt -->
 *
 * @brief Write log messages using the provided handlers
 *
 * @param h : A ULog handle
 * @param newTansportVTable : A Write, MultipartStart and MultipartEnd function table.
 *
 * @param retvalue : DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogCore_SetTransportToAlt
(
    ULogResult *retvalue,
    ULogHandle h,
    ULOG_CORE_VTABLE* newTansportVTable
)
{
    *retvalue = ULogCore_SetTransportToAlt ( h, newTansportVTable );
    return EFI_SUCCESS;
}

/*=========================================================================
      ULogFront
==========================================================================*/

/**
 * <!-- EFI_ULogFront_RawInit -->
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
 * @param retvalue : See ULogCore_LogCreate.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RawInit
(
    ULogResult *retvalue,
    ULogHandle * h,
    const char * name,
    uint32 logBufSize,
    uint32 logBufMemType,
    ULOG_LOCK_TYPE logLockType
)
{
    *retvalue = ULogFront_RawInit ( h, name, logBufSize, logBufMemType, logLockType );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RawLog -->
 *
 * @brief Log data to a raw log.
 *
 * @param h  : A ULog handle.
 * @param dataArray : A pointer to the data to log.
 * @param dataCount : The amount of data to log.
 *
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RawLog
(
    ULogResult *retvalue,
    ULogHandle h,
    const char * dataArray,
    uint32 dataCount
)
{
    *retvalue = ULogFront_RawLog ( h, dataArray, dataCount );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeInit -->
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
 * @param retvalue : See ULogCore_LogCreate.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeInit
(
    ULogResult *retvalue,
    ULogHandle * h,
    const char * name,
    uint32 logBufSize,
    uint32 logBufMemType,
    ULOG_LOCK_TYPE logLockType
)
{
    *retvalue = ULogFront_RealTimeInit ( h, name, logBufSize, logBufMemType, logLockType );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeVprintf -->
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
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeVprintf
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 dataCount,
    const char * formatStr,
    va_list ap
)
{
    *retvalue = ULogFront_RealTimeVprintf ( h, dataCount, formatStr, ap );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeCharArray -->
 *
 * @brief Log byte data to a RealTime log.  
 * 
 * @param h : A ULog handle.
 * @param byteCount : the number of bytes to log.
 * @param byteData : A pointer to the data to log.
 *
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeCharArray
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 byteCount,
    char * byteData
)
{
    *retvalue = ULogFront_RealTimeCharArray ( h, byteCount, byteData );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeString -->
 *
 * @brief Log string data to a RealTime log.
 *
 * This function copies the full string to the log.
 *
 * @param h : A ULog handle.
 * @param string : The string to log.
 *
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogFront_RealTimeString
(
    ULogResult *retvalue,
    ULogHandle h,
    char * cStr
)
{
    *retvalue = ULogFront_RealTimeString ( h, cStr );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeWordArray -->
 *
 * @brief Log word data to a RealTime log.
 *
 * @param h : A ULog handle.
 * @param wordCount : The number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeWordArray
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 wordCount,
    const uint32 * wordData
)
{
    *retvalue = ULogFront_RealTimeWordArray ( h, wordCount, wordData );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeCsv -->
 *
 * @brief Log word data to a RealTime log. Output will be a CSV compatible format.
 *
 * @param h  : A ULog handle.
 * @param wordCount : the number of words to log.
 * @param wordData : A pointer to the data to log.
 *
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *         Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeCsv
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 wordCount,
    const uint32 * wordData
)
{
    *retvalue = ULogFront_RealTimeCsv ( h, wordCount, wordData );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeVector -->
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
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeVector
(
    ULogResult *retvalue,
    ULogHandle h,
    const char * formatStr,
    unsigned short entryByteCount,
    unsigned short vectorLength,
    const void * vector
)
{
    *retvalue = ULogFront_RealTimeVector ( h, formatStr, entryByteCount, vectorLength, vector );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeMultipartMsgBegin -->
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
 * @param retvalue : TRUE if the mpm was successfully started. False if it wasn't and
 *         the multipart message should not be continued further
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogFront_RealTimeMultipartMsgBegin
(
    DALBOOL *retvalue,
    ULogHandle h
)
{
    *retvalue = ULogFront_RealTimeMultipartMsgBegin ( h );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeMultipartMsgEnd -->
 *
 * @brief End a multipart message
 *
 * The log will be locked for the duration of the multipart message.  
 * This function frees the lock, ends the MPM message and completes the MPM bookeeping.
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_ULogFront_RealTimeMultipartMsgEnd( ULogHandle h)
{
    ULogFront_RealTimeMultipartMsgEnd ( h );
    return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFile_Open -->
 *
 * Open a ULog file on disk, creating it if it doesn't exist
 * The file name is ULOGFILE_DIR<log_name>.ULOGFILE_EXTENSION
 *
 * The handle for the log file is stored in the ULog entry to
 * improve performance.
 *
 * @param retvalue: DAL_SUCCESS upon success, either DAL_ERROR or the
 *          DAL_SUCCESS if no errors occur
 *          DAL_ERROR for all other errors
 * @param h: File handle for the log
 * @param fileName:  Name of the file to open, will be truncated if
 *                    size > ULOG_MAX_NAME_SIZE
 * @param logBuf_size:  Maximum log file size in bytes
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFile_Open ( ULogResult *retvalue,  ULogHandle * h,
                                      CONST CHAR8 *fileName, UINT32 logBuf_size)
{
  //create a ulog
  *retvalue = ULogFile_Open( h, fileName, logBuf_size );
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFile_SetConfig -->
 * @brief Modify the file logging settings
 *
 * @param h: Pointer to the ULog handle to modify
 * @param settings: The new settings to apply
 *
 * @return EFI_SUCCESS if all input settings are valid
 *         ULOG_ERR_XX otherwise
 */
static EFI_STATUS EFI_ULogFile_SetConfig(ULogResult *retvalue, ULogHandle *h, CONST ULOG_CONFIG_TYPE *settings){
  *retvalue = ULogFile_SetConfig(h, settings);
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFile_GetConfig -->
 * @brief Return the file logging settings
 *
 * @param h: Pointer to the handle for the ULog entry in questin
 * @param settings: Pointer to the caller-alloc'd buffer in which to return the settings
 *
 * @return EFI_SUCCESS if settings could be copied successfully
 *         ULOG_ERR_XX otherwise
 */
static EFI_STATUS EFI_ULogFile_GetConfig(ULogResult *retvalue, ULogHandle *h, ULOG_CONFIG_TYPE *settings){
  *retvalue = ULogFile_GetConfig(h, settings);
  return EFI_SUCCESS;
}

/**
 * <!-- EFI_ULogFront_RealTimeVprintf_Ex -->
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
 * @param retvalue : DAL_SUCCESS if the message was logged successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 * @return       EFI_SUCCESS
*/
static EFI_STATUS EFI_ULogFront_RealTimeVprintf_Ex
(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 dataCount,
    uint32 paramBitField,
    const char * formatStr,
    va_list ap
)
{
    *retvalue = ULogFront_RealTimeVprintf_Ex ( h, dataCount, paramBitField, formatStr, ap );
    return EFI_SUCCESS;
}

/*=========================================================================
      Data Declarations
==========================================================================*/
static EFI_ULOG_PROTOCOL ULogProtocol =
{
    EFI_ULOG_PROTOCOL_REVISION,
//ULog.c
EFI_ULogCore_Init,
EFI_ULogCore_LogCreate,
EFI_ULogCore_HeaderSet,
EFI_ULogCore_AttributeSet,
EFI_ULogCore_MemoryAssign,
EFI_ULogCore_Enable,
EFI_ULogCore_IsEnabled,
EFI_ULogCore_Query,
EFI_ULogCore_Read,
EFI_ULogCore_ReadEx,
EFI_ULogCore_Allocate,
EFI_ULogCore_Disable,
EFI_ULogCore_List,
EFI_ULogCore_ListEx,
EFI_ULogCore_MsgFormat,
EFI_ULogCore_HeaderRead,
EFI_ULogCore_Connect,
EFI_ULogCore_LogLevelSet,
EFI_ULogCore_LogLevelGet,
EFI_ULogCore_TeeAdd,
EFI_ULogCore_TeeRemove,
EFI_ULogCore_HandleGet,
EFI_ULogCore_IsLogPresent,
EFI_ULogCore_SetTransportToRAM,
EFI_ULogCore_SetTransportToStm,
EFI_ULogCore_SetTransportToStmAscii,
EFI_ULogCore_SetTransportToAlt,
//ULogFront.c
EFI_ULogFront_RawInit,
EFI_ULogFront_RawLog,
EFI_ULogFront_RealTimeInit,
EFI_ULogFront_RealTimeVprintf,
EFI_ULogFront_RealTimeCharArray,
EFI_ULogFront_RealTimeString,
EFI_ULogFront_RealTimeWordArray,
EFI_ULogFront_RealTimeCsv,
EFI_ULogFront_RealTimeVector,
EFI_ULogFront_RealTimeMultipartMsgBegin,
EFI_ULogFront_RealTimeMultipartMsgEnd,
EFI_ULogFront_RealTimeVprintf_Ex,
//ULogFile.c
EFI_ULogFile_Open,
EFI_ULogFile_SetConfig,
EFI_ULogFile_GetConfig,
EFI_ULogCore_ReadSessionComplete
};

EFI_STATUS EFIAPI ULogDxeEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status;
  DALSYSConfig syscfg;

  memset(&syscfg, 0, sizeof(syscfg));

  /* Initialize DALSYS */
  DALSYS_InitMod(&syscfg);

  /* ********************************* */

  status = gBS->InstallMultipleProtocolInterfaces(
                &handle,
                &gEfiULogProtocolGuid,
                (void **)&ULogProtocol, NULL );

  return status;
}