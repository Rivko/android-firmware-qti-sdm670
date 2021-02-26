/*============================================================================
  @file ULog_wrapper.c

  Map Main interface for ULog to ULogDxe API.

  Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 09/30/15   sj/rli  Added ULogCore_ReadSessionComplete
 05/22/15   rli     Added global var for protocol to improve performance
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

EFI_ULOG_PROTOCOL * gULogProtocol = NULL;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

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
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol == NULL)
        {
          efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
        }
        if(efistatus == EFI_SUCCESS)
        {
            gULogProtocol->ULogCore_Init( &retvalue );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_LogCreate -->
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
 * @return DAL_SUCCESS if the create completed successfully.  The log will have been
 *        enabled and will be ready to use. 
 *        ULOG_ERR_INITINCOMPLETE if ULOG_MEMORY_LOCAL_MANUAL is used.  
 *        ULOG_ERR_INITINCOMPLETE if a buffersize of 0 is used.  
 *        The log can be enabled later after ULogCore_MemoryAssign sets up all the parts.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_LogCreate(ULogHandle * h, const char *  logName, uint32  bufferSize, uint32  memoryType, ULOG_LOCK_TYPE  lockType, ULOG_INTERFACE_TYPE  interfaceType)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol == NULL)
        {
          efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
        }
        if(efistatus == EFI_SUCCESS)
        {
            gULogProtocol->ULogCore_LogCreate( &retvalue, h, logName, bufferSize, memoryType, lockType, interfaceType );
        }
    return retvalue;
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
 * @param h: A ULog handle.
 * @param headerText: The string to store in the log.
 *
 * @return DAL_SUCCESS if the header was successfully stored.
 */
ULogResult ULogCore_HeaderSet(ULogHandle h, char * headerText)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_HeaderSet( &retvalue, h, headerText );
        }
    return retvalue;
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
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_AttributeSet( h, attribute );
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
 *                   Note that ALL buffers must be word aligned.
 * @param bufferSize : The size in bytes of the buffer.
 *
 * @return DAL_SUCCESS if the memory was assigned successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_MemoryAssign(ULogHandle h, ULOG_VALUE_ID id, void * bufferPtr, uint32  bufferSize)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_MemoryAssign( &retvalue, h, id, bufferPtr, bufferSize );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_Enable -->
 *
 * @brief Enable a log that is already defined.
 *
 * Enable a log.  A log can need manual enabling if it has manual
 * allocation, has been disabbed, or buffer memory was not assigned.
 *
 * @param h : A ULog handle.
 *
 * @return DAL_SUCCESS if the log was enabled successfully.
 *        Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Enable(ULogHandle h)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_Enable( &retvalue, h );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_IsEnabled -->
 *
 * @brief Check if a log is enabled.
 *
 * @param h : A ULog handle.
 * @param core : A vtable to fill in with the appropriate
 *              interface functions for the current log
 *
 * @return TRUE if the log is enabled.
 */
DALBOOL ULogCore_IsEnabled(ULogHandle h, ULOG_CORE_VTABLE ** core)
{
    DALBOOL  retvalue = 0;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_IsEnabled( &retvalue, h, core );
        }
    return retvalue;
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
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_Query( &retvalue, h, id, value );
        }
    return retvalue;
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
ULogResult ULogCore_Read(ULogHandle h, uint32  outputSize, char *  outputMem, uint32 *  outputCount)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_Read( &retvalue, h, outputSize, outputMem, outputCount );
        }
    return retvalue;
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
  
  ULogResult  retvalue = ULOG_ERR_MISC;
  if (gULogProtocol != NULL)
  {
      gULogProtocol->ULogCore_ReadSessionComplete( &retvalue, h);
  }
  return retvalue;
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
 *                             the buffer.
 *
 * @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_ReadEx(ULogHandle h, uint32  outputSize, char *  outputMem, uint32 *  outputCount, uint32 outputMessageLimit)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_ReadEx( &retvalue, h, outputSize, outputMem, outputCount, outputMessageLimit );
        }
    return retvalue;
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
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_Allocate( &retvalue, h, bufferSize );
        }
    return retvalue;
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
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_Disable( &retvalue, h );
        }
    return retvalue;
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
 * @return DAL_SUCCESS if the list was successfull. Errors are defined in
 *        the ULOG_ERRORS enum.
 */
ULogResult ULogCore_List(uint32 *  registeredCount, uint32  nameArraySize, uint32 *  namesReadCount, char *  nameArray)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol == NULL)
        {
          efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
        }
        if(efistatus == EFI_SUCCESS)
        {
            gULogProtocol->ULogCore_List( &retvalue, registeredCount, nameArraySize, namesReadCount, nameArray );
        }
    return retvalue;
}


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
 *                        in the nameArray.
 * @param nameArray : The buffer to receive log name data.
 *
 * @return DAL_SUCCESS if the list was successfull. Errors are defined in
 *        the ULOG_ERRORS enum.
 */
ULogResult ULogCore_ListEx(uint32 offsetCount, uint32 *  registeredCount, uint32  nameArraySize, uint32 *  namesReadCount, char *  nameArray)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol == NULL)
        {
          efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
        }
        if(efistatus == EFI_SUCCESS)
        {
            gULogProtocol->ULogCore_ListEx( &retvalue, offsetCount, registeredCount, nameArraySize, namesReadCount, nameArray );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_MsgFormat -->
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
 * @return DAL_SUCCESS if the format was successful. Errors are
 *        defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_MsgFormat(ULogHandle h, char *  msg, char *  msgString, uint32 msgStringSize, uint32 * msgConsumed)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_MsgFormat( &retvalue, h, msg, msgString, msgStringSize, msgConsumed );
        }
    return retvalue;
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
ULogResult ULogCore_HeaderRead(ULogHandle h, uint32 headerReadOffset, char * headerString, uint32 headerStringSize, uint32 * headerActualLength)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_HeaderRead( &retvalue, h, headerReadOffset, headerString, headerStringSize, headerActualLength );
        }
    return retvalue;
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
ULogResult ULogCore_Connect(ULogHandle * h, const char * logName)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol == NULL)
        {
          efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
        }
        if(efistatus == EFI_SUCCESS)
        {
            gULogProtocol->ULogCore_Connect( &retvalue, h, logName );
        }
    return retvalue;
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
    if (gULogProtocol != NULL)
    {
        gULogProtocol->ULogCore_LogLevelSet( h, level );
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
    uint32  retvalue = 0;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_LogLevelGet( &retvalue, h );
        }
    return retvalue;
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
    ULogHandle  retvalue = NULL;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_TeeAdd( &retvalue, h1, h2 );
        }
    return retvalue;
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
    ULogHandle  retvalue = NULL;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_TeeRemove( &retvalue, h1, h2 );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_HandleGet -->
 *
 * @brief Get a log handle by its name.
 *
 * @param logName: Name of a ULog to search for.
 *
 * @return A handle to the named log
 */
ULogHandle ULogCore_HandleGet(char *logName)
{
    EFI_STATUS efistatus = EFI_SUCCESS;
    ULogHandle  retvalue = NULL;
        if (gULogProtocol == NULL)
        {
          efistatus = gBS->LocateProtocol(&gEfiULogProtocolGuid, NULL, (void**)&gULogProtocol);
        }
        if(efistatus == EFI_SUCCESS)
        {
            gULogProtocol->ULogCore_HandleGet( &retvalue, logName );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_IsLogPresent -->
 *
 * @brief Check if a named log is present in a log set
 *
 * @param h : A ULog handle, which may be a handle to a single or a group of logs
 * @param logName : name of a ULog
 *
 * @return TRUE if the named log is present in the log(s) indicated by the handle
 *        h, and FALSE otherwise
 */
DALBOOL ULogCore_IsLogPresent(ULogHandle h, char *logName)
{
    DALBOOL  retvalue = 0;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_IsLogPresent( &retvalue, h, logName );
        }
    return retvalue;
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
ULogResult ULogCore_SetTransportToRAM(ULogHandle h)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_SetTransportToRAM( &retvalue, h );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_SetTransportToStm -->
 *
 * @brief Write log messages to the QDSS/STM transport
 *
 * @param h : A ULog handle
 * @param protocol_num : A QDSS protocol number (0 to 127) is assigned to the 
 *                      ULog handle and this protocol number can be used to
 *                      match logs to the handle when reading the QDSS output.
 *
 * @return DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 */
ULogResult ULogCore_SetTransportToStm(ULogHandle h, unsigned char protocol_num)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_SetTransportToStm( &retvalue, h, protocol_num );
        }
    return retvalue;
}


/**
 * <!-- ULogCore_SetTransportToStmAscii -->
 *
 * @brief Write log messages as ASCII to the QDSS/STM transport
 *
 * @param h : A ULog handle
 * @param protocol_num : A QDSS protocol number (0 to 127) is assigned to the
 *                      ULog handle and can be used to identify log output when
 *                      reading the QDSS output.
 *
 * @return DAL_SUCCESS if the transport is changed, DAL_ERROR if it isn't
 */
ULogResult ULogCore_SetTransportToStmAscii(ULogHandle h, unsigned char protocol_num)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_SetTransportToStmAscii( &retvalue, h, protocol_num );
        }
    return retvalue;
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
ULogResult ULogCore_SetTransportToAlt(ULogHandle h, ULOG_CORE_VTABLE* newTansportVTable)
{
    ULogResult  retvalue = ULOG_ERR_MISC;
        if (gULogProtocol != NULL)
        {
            gULogProtocol->ULogCore_SetTransportToAlt( &retvalue, h, newTansportVTable );
        }
    return retvalue;
}
