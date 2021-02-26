/** 
  @file  EFIULog.h
  @brief ULOG Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 09/30/15   sj/rli  Added API EFI_ULOGCORE_READSESSIONCOMPLETE 
 05/22/15   rli     Added ULogFile config API
 10/02/14   vpopuri Merged ULOGv5
 08/14/14   vpopuri Added UEFI File Logging interface
 11/20/13   llg     (Tech Pubs) Edited function syntax; otherwise, Doxygen will not run
 07/01/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 03/27/13   rli     Initial revision.

=============================================================================*/
#ifndef __EFIULog_H__
#define __EFIULog_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include <Library/ULog.h>
#include <Library/ULogFront.h>
#include <Library/ULogUEFI.h>

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/
/** @ingroup efi_ulog_constants
  Protocol version.
*/
#define EFI_ULOG_PROTOCOL_REVISION 0x0000000000010005

/*  Protocol GUID definition */
/** @ingroup efi_ulog_protocol */
#define EFI_ULOG_PROTOCOL_GUID \
  { 0x93a824b1, 0xf77c, 0x4880, { 0xad, 0x95, 0xe7, 0xbc, 0x63, 0x4f, 0x90, 0x51 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/*
  External reference to the ULog Protocol GUID.
 */
extern EFI_GUID gEfiULogProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* EFI_ULOGCORE_INIT */
/** @ingroup efi_ulogcore_init
  @par Summary
  Initializes the ULog subsystem.
  @par
  It is not strictly necessary to call this function because it
  is executed any time an attempt is made to connect to a log or
  create a log.

  @param[out] retvalue  DAL_SUCCESS if the initialization is successful.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_INIT)( ULogResult *retvalue);

/* EFI_ULOGCORE_LOGCREATE */
/** @ingroup efi_ulogcore_logcreate
  @par Summary
  Creates a new log and adds it to the logs managed by the ULog subsystem.
  
  @param[in]  h              A ULog handle is returned at this pointer. 
  @param[in]  logName        Log name; 24 characters maximum, including NULL.
  @param[in]  bufferSize     Size, in bytes, of the log buffer. A request
                             of size 0 leaves the buffer unallocated. 
  @param[in]  memoryType     Memory configuration.
  @param[in]  lockType       Type of locking required.
  @param[in]  interfaceType  This field is a legacy option. There is only a
                             single Ulog type. Any value can be used for this
                             parameter because it is ignored. The raw and
                             real-time types are only used internally to
                             indicate if a write has a timestamp included.
  @param[out] retvalue       DAL_SUCCESS if the create completed successfully;
                             the log is enabled and is ready to use. \n
                             \vertspace{4}
                             ULOG_ERR_INITINCOMPLETE if 
                             ULOG_MEMORY_LOCAL_MANUAL memory type was used. \n
                             \vertspace{4}
                             ULOG_ERR_INITINCOMPLETE if a buffer size of 0 was
                             used. \n
                             \vertspace{4}
                             The log can be enabled later after
                             EFI_ULOGCORE_MEMORYASSIGN() sets up all the
                             parts. \n
                             \vertspace{4}
                             See #ULOG_ERRORS for descriptions of all error
                             codes.
  
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_LOGCREATE)(
    ULogResult *retvalue,
    ULogHandle * h,
    const char *  logName,
    uint32  bufferSize,
    uint32  memoryType,
    ULOG_LOCK_TYPE  lockType,
    ULOG_INTERFACE_TYPE  interfaceType
);

/* EFI_ULOGCORE_HEADERSET */
/** @ingroup efi_ulogcore_headerset
  @par Summary
  Sets the header for the log.

  @param[in]  h           ULog handle.
  @param[in]  headerText  String to store in the log.
  @param[out] retvalue    DAL_SUCCESS if the header was successfully stored.
  
  @par Description
  This function applies the supplied string to a specified log header. The
  header is stored with a log and is not overwritten unless this function
  is called again.
  @par
  There are no real limitations to this string. The system
  determines the length of the supplied string, allocates that
  amount of memory, and copies the string to it.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_HEADERSET)(
    ULogResult *retvalue,
    ULogHandle h,
    char * headerText
);

/* EFI_ULOGCORE_ATTRIBUTESET */
/** @ingroup efi_ulogcore_attributeset
  @par Summary
  Sets the log attributes.
  @par
  This function performs an OR operation in the provided attribute bits into
  the logs attribute mask.

  @param[in]  h          Log for which to set the attributes.
  @param[in]  attribute  Attributes to be set.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_ATTRIBUTESET)(
    ULogHandle h,
    uint32 attribute
);

/* EFI_ULOGCORE_MEMORYASSIGN */
/** @ingroup efi_ulogcore_memoryassign
  @par Summary
  Assigns memory to specific log buffers.
  
  @param[in]  h           ULog handle.
  @param[in]  id          Which buffer to assign.
  @param[in]  bufferPtr   Pointer to the buffer memory to assign.
                          Note that <i>all</i> buffers must be word aligned.
  @param[in]  bufferSize  Size, in bytes, of the buffer.
  @param[out] retvalue    DAL_SUCCESS if the memory was assigned
                          successfully. \n
                          See #ULOG_ERRORS for descriptions of all error
                          codes.
  
  @par Description
  When a log is created with the allocation type
  ULOG_ALLOC_MANUAL, the buffers are and must be manually
  assigned with this function. All supplied buffers must be word
  aligned. If a buffer is not word aligned, this function fails.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_MEMORYASSIGN)(
    ULogResult *retvalue,
    ULogHandle h,
    ULOG_VALUE_ID id,
    void * bufferPtr,
    uint32  bufferSize
);

/* EFI_ULOGCORE_ENABLE */
/** @ingroup efi_ulogcore_enable
  @par Summary
  Enables a log that is already defined.
  @par
  A log needs manual enabling if it has manual allocation, has been
  disabled, or if buffer memory was not assigned.

  @param[in]  h         ULog handle.
  @param[out] retvalue  DAL_SUCCESS if the log was enabled successfully. \n
                        See #ULOG_ERRORS for descriptions of all error
                        codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_ENABLE)(
    ULogResult *retvalue,
    ULogHandle h
);

/* EFI_ULOGCORE_ISENABLED */
/** @ingroup efi_ulogcore_isenabled
  @par Summary
  Checks if a log is enabled.

  @param[in]  h         ULog handle.
  @param[in]  core      A vtable to be filled in with the appropriate
                        interface functions for the current log.
  @param[out] retvalue  TRUE if the log is enabled; otherwise, FALSE.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_ISENABLED)(
    DALBOOL *retvalue,
    ULogHandle h,
    ULOG_CORE_VTABLE ** core
);

/* EFI_ULOGCORE_QUERY */
/** @ingroup efi_ulogcore_query
  @par Summary
  Queries information about a specific log or the logging system.
  @par
  This function returns the expected sizes of internal log
  buffers to manually allocate the buffers. A NULL handle is used
  for these values.

  @param[in]  h         ULog handle.
  @param[in]  id        Log buffer ID.
  @param[in]  value     The returned number of bytes needed by the
                        selected buffer.
  @param[out] retvalue  DAL_SUCCESS if the buffer ID is known and supported. \n
                        See #ULOG_ERRORS for descriptions of all error
                        codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_QUERY)(
    ULogResult *retvalue,
    ULogHandle h,
    ULOG_VALUE_ID id,
    uint32 * value
);

/* EFI_ULOGCORE_READ */
/** @ingroup efi_ulogcore_read
  @par Summary
  Reads data from the log buffer.
  
  @param[in]  h            ULog handle of the log to read.
  @param[in]  outputSize   Size of the output buffer.
  @param[in]  outputMem    Pointer to the output buffer.
  @param[in]  outputCount  Amount of data placed in the output buffer.
  @param[out] retvalue     DAL_SUCCESS if the read completed.
  
  @par Description
  When the data is read from the log buffer, it is unformatted.
  EFI_ULOGCORE_MSGFORMAT() is used to format each message. Only full
  messages are placed in the output buffer; it is incorrect to use
  outputCount != outputSize as an indicator that the log buffer
  is empty.
  @par
  An empty log buffer yields only a status read and an outputCount of 4.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_READ)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32  outputSize,
    char *  outputMem,
    uint32 *  outputCount
);

/* EFI_ULOGCORE_READEX */
/** @ingroup efi_ulogcore_read_ex
  @par Summary
  Reads data from the log buffer and allows control of the number of 
  messages to read.

  @param[in]  h                   ULog handle of the log to read.
  @param[in]  outputSize          Size of the output buffer.
  @param[in]  outputMem           Pointer to the output buffer.
  @param[in]  outputCount         Amount of data placed in the output
                                  buffer.
  @param[in]  outputMessageLimit  Maximum number of messages to read from
                                  the buffer.
  @param[out] retvalue            DAL_SUCCESS if the read completed.

  @par Description
  See EFI_ULOGCORE_READ() for description details.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_READEX)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32  outputSize,
    char *  outputMem,
    uint32 *  outputCount,
    uint32 outputMessageLimit
);

/* EFI_ULOGCORE_ALLOCATE */
/** @ingroup efi_ulogcore_allocate
  @par Summary
  Allocates memory to the log buffer.

  @param[in]  h           ULog handle of the log to read.
  @param[in]  bufferSize  Size of the circular buffer to allocate.
  @param[out] retvalue    DAL_SUCCESS if the memory was allocated successfully. \n
                          See #ULOG_ERRORS for descriptions of all error
                          codes.

  @par Description
  When a log is created with no output buffer, this function can
  be used to allocate that memory. It is intended for use from
  applications enabling and reading logs from the system.
  @par
  This function only works for logs created with a ULOG_ALLOC_LOCAL
  allocation type.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_ALLOCATE)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 bufferSize
);

/* EFI_ULOGCORE_DISABLE */
/** @ingroup efi_ulogcore_disable
  @par Summary
  Disables a log. 
  @par
  Message writes are dropped until the log is re-enabled.

  @param[in]  h         ULog handle.
  @param[out] retvalue  DAL_SUCCESS if the log was disabled successfully. \n
                        See #ULOG_ERRORS for descriptions of all error
                        codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_DISABLE)(
    ULogResult *retvalue,
    ULogHandle h
);

/* EFI_ULOGCORE_LIST */
/** @ingroup efi_ulogcore_list
  @par Summary
  Creates a list of all available logs in the system and outputs the list
  as a text string.
  
  @param[in]  registeredCount  Number of logs that are registered.
  @param[in]  nameArraySize    Size of the array that receives the log names.
  @param[in]  namesReadCount   Number of log names that were actually
                               placed in nameArray.
  @param[in]  nameArray        Buffer to receive the log name data.
  @param[out] retvalue         DAL_SUCCESS if the list was successful. \n
                               See #ULOG_ERRORS for descriptions of all error
                               codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_LIST)(
    ULogResult *retvalue,
    uint32 *  registeredCount,
    uint32  nameArraySize,
    uint32 *  namesReadCount,
    char *  nameArray
);

/* EFI_ULOGCORE_LISTEX */
/** @ingroup efi_ulogcore_list_ex
  @par Summary
  Creates a list of all available logs in the system and outputs the list 
  as a text string.
  @par
  This function allows for an offset into the list by the specified amount.
  
  @param[in]  offsetCount      Number of log names to skip before 
                               returning log name data.
  @param[in]  registeredCount  Number of logs that are registered.
  @param[in]  nameArraySize    Size of the array that receives the log names.
  @param[in]  namesReadCount   Number of log names that were actually 
                               placed in nameArray.
  @param[in]  nameArray        Buffer to receive the log name data.
  @param[out] retvalue         DAL_SUCCESS if the list was successful. \n
                               See #ULOG_ERRORS for descriptions of all error
                               codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_LISTEX)(
    ULogResult *retvalue,
    uint32 offsetCount,
    uint32 *  registeredCount,
    uint32  nameArraySize,
    uint32 *  namesReadCount,
    char *  nameArray
);

/* EFI_ULOGCORE_MSGFORMAT */
/** @ingroup efi_ulogcore_msgformat
  @par Summary
  Formats and outputs an individual message retrieved via 
  EFI_ULOGCORE_READ().

  @param[in]  h              ULog handle to the log to be formatted.
  @param[in]  msg            Input message.
  @param[in]  msgString      Output string.
  @param[in]  msgStringSize  Size of the output string.
  @param[in]  msgConsumed    Number of bytes consumed from the original
                             message buffer.
  @param[out] retvalue       DAL_SUCCESS if the format was successful. \n
                             See #ULOG_ERRORS for descriptions of all error
                             codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_MSGFORMAT)(
    ULogResult *retvalue,
    ULogHandle h,
    char *  msg,
    char *  msgString,
    uint32 msgStringSize,
    uint32 * msgConsumed
);

/* EFI_ULOGCORE_HEADERREAD */
/** @ingroup efi_ulogcore_header_read
  @par Summary
  Reads the header from the supplied log.
  
  @param[in]  h                   Handle of the log to access.
  @param[in]  headerReadOffset    Offset of the read.
  @param[out] headerString        Output string.
  @param[in]  headerStringSize    Size of the output string.
  @param[out] headerActualLength  Actual length of the header string.
  @param[out] retvalue            DAL_SUCCESS if the format was successful. \n
                                  See #ULOG_ERRORS for descriptions of all
                                  error codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_HEADERREAD)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 headerReadOffset,
    char * headerString,
    uint32 headerStringSize,
    uint32 * headerActualLength
);

/* EFI_ULOGCORE_CONNECT */
/** @ingroup efi_ulogcore_connect
  @par Summary
  Connects a log handle to a log defined elsewhere.

  @param[in]  h         ULog handle.
  @param[in]  logName   Name of the log with which to connect.
  @param[out] retvalue  DAL_SUCCESS if the connect was successful. \n
                        See #ULOG_ERRORS for descriptions of all error
                        codes.

  @par Description
  This function connects a log handle to a log that has been, or will be,
  defined elsewhere. A successful return allows logging through the
  handle, although no data is written to a physical log
  until the log is defined with the EFI_ULOGCORE_LOGCREATE() function.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_CONNECT)(
    ULogResult *retvalue,
    ULogHandle * h,
    const char * logName
);

/* EFI_ULOGCORE_LOGLEVELSET */
/** @ingroup efi_ulogcore_loglevelset
  @par Summary
  Sets the log level.

  @param[in]  h      ULog handle.
  @param[in]  level  New level for the log.

  @par Description
  The ULog subsystem does not filter messages, but this function is
  intended to facilitate filtering without requiring it. The function
  simply sets a parameter in a log that can be used to filter log
  messages along with provided macros.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_LOGLEVELSET)(
    ULogHandle h,
    uint32 level
);

/* EFI_ULOGCORE_LOGLEVELGET */
/** @ingroup efi_ulogcore_loglevelget
  @par Summary
  Gets the log level.

  @param[in]  h         ULog handle.
  @param[out] retvalue  Current log value or 0 if the log handle is NULL.

  @par Description
  The ULog subsystem does not filter messages, but this function is
  intended to facilitate filtering without requiring it. The function
  simply returns a parameter in a log that can be used to filter
  messages along with provided macros.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_LOGLEVELGET)(
    uint32 *retvalue,
    ULogHandle h
);

/* EFI_ULOGCORE_TEEADD */
/** @ingroup efi_ulogcore_tee_add
  @par Summary
  Groups handles together.
  @par
  This function groups handles together so that any logging information
  is copied to multiple logs.

  @note1hang The first handle may point to an updated object after a call to
             this function, whereas the second handle does not.

  @param[in]  h1        ULog handle, which may be a handle to a single log
                        or a group of logs.
  @param[in]  h2        ULog handle, which may be a handle to a single log
                        or a group of logs to be added to h1.
  @param[out] retvalue  Handle to the grouped logs.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_TEEADD)(
    ULogHandle *retvalue,
    ULogHandle h1,
    ULogHandle h2
);

/* EFI_ULOGCORE_TEEREMOVE */
/** @ingroup efi_ulogcore_tee_remove
  @par Summary
  Removes one or more handles.
   
  @note1hang The first handle may point to an updated object after a call to
             this function, whereas the second handle does not.

  @param[in]  h1        ULog handle, which may be a handle to a single log
                        or a group of logs.
  @param[in]  h2        ULog handle, which may be a handle to a single log
                        or a group of logs to be removed from h1.
  @param[out] retvalue  Handle to the resulting group of logs.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_TEEREMOVE)(
    ULogHandle *retvalue,
    ULogHandle h1,
    ULogHandle h2
);

/* EFI_ULOGCORE_HANDLEGET */
/** @ingroup efi_ulogcore_handleget
  @par Summary
  Gets a log handle by its name.

  @param[in]  logName   Name of a ULog for which to search.
  @param[out] retvalue  Handle to the named log.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_HANDLEGET)(
    ULogHandle *retvalue,
    char *logName
);

/* EFI_ULOGCORE_ISLOGPRESENT */
/** @ingroup efi_ulogcore_islog_present
  @par Summary
  Checks if a named log is present in a log set.

  @param[in]  h         ULog handle, which may be a handle to a single log
                        or a group of logs.
  @param[in]  logName   Name of a ULog.
  @param[out] retvalue  TRUE if the named log is present in the log or logs
                        indicated by the handle h; otherwise, FALSE.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_ISLOGPRESENT)(
    DALBOOL *retvalue,
    ULogHandle h,
    char *logName
);

/* EFI_ULOGCORE_SETTRANSPORTTORAM */
/** @ingroup efi_ulogcore_settransport_to_ram
  @par Summary
  Writes log messages to the circular RAM buffer; this is the default
  behavior for logs. 

  @param[in]  h         ULog handle.
  @param[out] retvalue  DAL_SUCCESS if the transport is changed; 
                        otherwise, DAL_ERROR.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_SETTRANSPORTTORAM)(
    ULogResult *retvalue,
    ULogHandle h
);

/* EFI_ULOGCORE_SETTRANSPORTTOSTM */
/** @ingroup efi_ulogcore_settransport_to_stm
  @par Summary
  Writes log messages to the Qualcomm Debug Subsystem/System Trace Macrocell 
  (QDSS/STM) transport.

  @param[in]  h             ULog handle.
  @param[in]  protocol_num  QDSS protocol number (0 to 127) assigned to the
                            ULog handle; this number can be used to match
                            logs to the handle when reading the QDSS output.
  @param[out] retvalue      DAL_SUCCESS if the transport is changed; 
                            otherwise, DAL_ERROR.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_SETTRANSPORTTOSTM)(
    ULogResult *retvalue,
    ULogHandle h,
    unsigned char protocol_num
);

/* EFI_ULOGCORE_SETTRANSPORTTOSTMASCII */
/** @ingroup efi_ulogcore_settransport_to_stmascii
  @par Summary
  Writes log messages as ASCII to the QDSS/STM transport.

  @param[in]  h             ULog handle.
  @param[in]  protocol_num  QDSS protocol number (0 to 127) assigned to the
                            ULog handle; this number can be used to identify
                            log output when reading the QDSS output.
  @param[out] retvalue      DAL_SUCCESS if the transport is changed; 
                            otherwise, DAL_ERROR.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_SETTRANSPORTTOSTMASCII)(
    ULogResult *retvalue,
    ULogHandle h,
    unsigned char protocol_num
);

/* EFI_ULOGCORE_SETTRANSPORTTOALT */
/** @ingroup efi_ulogcore_settransport_to_alt
  @par Summary
  Writes log messages using the provided handles.

  @param[in]  h                  ULog handle.
  @param[in]  newTansportVTable  A Write, MultipartStart, and MultipartEnd
                                 function table.
  @param[out] retvalue           DAL_SUCCESS if the transport is changed
                                 otherwise, DAL_ERROR.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGCORE_SETTRANSPORTTOALT)(
    ULogResult *retvalue,
    ULogHandle h,
    ULOG_CORE_VTABLE* newTansportVTable
);

/* EFI_ULOGFRONT_RAWINIT */
/** @ingroup efi_ulogfront_rawinit
  @par Summary
  Creates a new log in the raw format.

  @param[in]  h              A ULog handle is returned at this pointer. 
  @param[in]  name           Log name; 24 characters maximum, including NULL.
  @param[in]  logBufSize     Size, in bytes, of the log buffer. A request
                             of size 0 leaves the buffer unallocated. 
  @param[in]  logBufMemType  Memory configuration.
  @param[in]  logLockType    Type of locking required.
  @param[out] retvalue       DAL_SUCCESS if the create completed successfully;
                             the log is enabled and is ready to use. \n
                             \vertspace{4}
                             ULOG_ERR_INITINCOMPLETE if 
                             ULOG_MEMORY_LOCAL_MANUAL memory type was used. \n
                             \vertspace{4}
                             ULOG_ERR_INITINCOMPLETE if a buffer size of 0 was
                             used. \n
                             \vertspace{4}
                             The log can be enabled later after
                             EFI_ULOGCORE_MEMORYASSIGN() sets up all the
                             parts. \n
                             \vertspace{4}
                             See #ULOG_ERRORS for descriptions of all error
                             codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_RAWINIT)(
    ULogResult *retvalue,
    ULogHandle * h,
    const char * name,
    uint32 logBufSize,
    uint32 logBufMemType,
    ULOG_LOCK_TYPE logLockType
);

/* EFI_ULOGFRONT_RAWLOG */
/** @ingroup efi_ulogfront_rawlog
  @par Summary
  Logs data to a raw log.

  @param[in]  h          ULog handle.
  @param[in]  dataArray  Pointer to the data to be logged.
  @param[in]  dataCount  Amount of data to be logged.
  @param[out] retvalue   DAL_SUCCESS if the message was logged successfully. \n
                         See #ULOG_ERRORS for descriptions of all error
                         codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_RAWLOG)(
    ULogResult *retvalue,
    ULogHandle h,
    const char * dataArray,
    uint32 dataCount
);

/* EFI_ULOGFRONT_REALTIMEINIT */
/** @ingroup efi_ulogfront_realtime_init
  @par Summary
  Creates a new log in the real-time format.

  @param[in]  h              A ULog handle is returned at this pointer. 
  @param[in]  name           Log name; 24 characters maximum, including NULL.
  @param[in]  logBufSize     Size, in bytes, of the log buffer. A request
                             of size 0 leaves the buffer unallocated. 
  @param[in]  logBufMemType  Memory configuration.
  @param[in]  logLockType    Type of locking required.
  @param[out] retvalue       DAL_SUCCESS if the create completed successfully;
                             the log is enabled and is ready to use. \n
                             \vertspace{4}
                             ULOG_ERR_INITINCOMPLETE if 
                             ULOG_MEMORY_LOCAL_MANUAL memory type was used. \n
                             \vertspace{4}
                             ULOG_ERR_INITINCOMPLETE if a buffer size of 0 was
                             used. \n
                             \vertspace{4}
                             The log can be enabled later after
                             EFI_ULOGCORE_MEMORYASSIGN() sets up all the
                             parts. \n
                             \vertspace{4}
                             See #ULOG_ERRORS for descriptions of all error
                             codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEINIT)(
    ULogResult *retvalue,
    ULogHandle * h,
    const char * name,
    uint32 logBufSize,
    uint32 logBufMemType,
    ULOG_LOCK_TYPE logLockType
);

/* EFI_ULOGFRONT_REALTIMEVPRINTF */
/** @ingroup efi_ulogfront_realtime_vprintf
  @par Summary
  Logs data in the vprintf format.

  @param[in]  h          ULog handle.
  @param[in]  dataCount  Number of parameters being printed (not
                         including formatStr); limited to ten
                         parameters.
  @param[in]  formatStr  Format string for the printf.
  @param[in]  ap         A va_list of the parameters being printed.
  @param[out] retvalue   DAL_SUCCESS if the message was logged successfully. \n
                         See #ULOG_ERRORS for descriptions of all error
                         codes.

  @par Description
  This function logs printf data to a real-time log. The printf format
  is not executed until the log is pulled. This makes a better performing
  call, but also means all strings must be present when the log is read.

  @note1hang This function serves the same purpose as real-time
             print format, except it takes a va_list argument
             instead of being variadic.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEVPRINTF)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 dataCount,
    const char * formatStr,
    va_list ap
);

/* EFI_ULOGFRONT_REALTIMECHARARRAY */
/** @ingroup efi_ulogfront_realtime_char_array
  @par Summary
  Logs byte data to a real-time log.
  
  @param[in]  h          ULog handle.
  @param[in]  byteCount  Number of bytes to be logged.
  @param[in]  byteData   Pointer to the data to be logged.
  @param[out] retvalue   DAL_SUCCESS if the message was logged successfully. \n
                         See #ULOG_ERRORS for descriptions of all error
                         codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMECHARARRAY)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 byteCount,
    char * byteData
);

/* EFI_ULOGFRONT_REALTIMESTRING */
/** @ingroup efi_ulogfront_realtime_string
  @par Summary
  Logs string data to a real-time log.
  @par
  This function copies the full string to the log.

  @param[in]  h         ULog handle.
  @param[in]  cStr      String to be logged.
  @param[out] retvalue  DAL_SUCCESS if the message was logged successfully. \n
                        See #ULOG_ERRORS for descriptions of all error
                        codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMESTRING)(
    ULogResult *retvalue,
    ULogHandle h,
    char * cStr
);

/* EFI_ULOGFRONT_REALTIMEWORDARRAY */
/** @ingroup efi_ulogfront_realtime_word_array
  @par Summary
  Logs word data to a real-time log.

  @param[in]  h          ULog handle.
  @param[in]  wordCount  Number of words to be logged.
  @param[in]  wordData   Pointer to the data to be logged.
  @param[out] retvalue   DAL_SUCCESS if the message was logged successfully. \n
                         See #ULOG_ERRORS for descriptions of all error
                         codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEWORDARRAY)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 wordCount,
    const uint32 * wordData
);

/* EFI_ULOGFRONT_REALTIMECSV */
/** @ingroup efi_ulogfront_realtime_csv
  @par Summary
  Logs word data to a real-time log; output is a CSV-compatible format.

  @param[in]  h          ULog handle.
  @param[in]  wordCount  Number of words to be logged.
  @param[in]  wordData   Pointer to the data to be logged.
  @param[out] retvalue   DAL_SUCCESS if the message was logged successfully. \n
                         See #ULOG_ERRORS for descriptions of all error
                         codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMECSV)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 wordCount,
    const uint32 * wordData
);

/* EFI_ULOGFRONT_REALTIMEVECTOR */
/* TechPubs note: in the @param below, %x, %d, %i, and %u require a 
   backslash escape (\%x) to format correctly in the document. */
/** @ingroup efi_ulogfront_realtime_vector
  @par Summary
  Logs vector data to a real-time log.
  @par
  The output is a vector with the provided format. No newlines are inserted.

  @param[in]  h               ULog handle.
  @param[in]  formatStr       Format string with a single \%x, \%d, \%i, 
                              or \%u.
  @param[in]  entryByteCount  1-, 2-, or 4-byte size data.
  @param[in]  vectorLength    How many elements to store. 
  @param[in]  vector          Pointer to the array of elements.
  @param[out] retvalue        DAL_SUCCESS if the message was logged
                              successfully. \n
                              See #ULOG_ERRORS for descriptions of all error
                              codes.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEVECTOR)(
    ULogResult *retvalue,
    ULogHandle h,
    const char * formatStr,
    unsigned short entryByteCount,
    unsigned short vectorLength,
    const void * vector
);

/* EFI_ULOGFRONT_REALTIMEMULTIPARTMSGBEGIN */
/* TechPubs note: in the Description below, %m requires a 
   backslash escape (\%m) to format correctly in the document. */
/** @ingroup efi_ulogfront_realtime_multipart_msg_begin
  @par Summary
  Begins a multipart message.

  @param[in]  h         ULog handle, which may be a handle to a single log
                        or a group of logs.
  @param[out] retvalue  TRUE if the multipart message was successfully started;
                        otherwise, FALSE and the multipart message is not to be
                        continued further.

  @par Description
  The log is locked for the duration of the multipart message (MPM). To
  free the lock, call EFI_ULOGFRONT_REALTIMEMULTIPARTMSGEND(). The first
  message of an MPM is always a real-time print format with \%m specifiers
  for the parts to add in the subsequent logging before calling
  %EFI_ULOGFRONT_REALTIMEMULTIPARTMSGEND().

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEMULTIPARTMSGBEGIN)(
    DALBOOL *retvalue,
    ULogHandle h
);

/* EFI_ULOGFRONT_REALTIMEMULTIPARTMSGEND */
/** @ingroup efi_ulogfront_realtime_multipart_msg_end
  @par Summary
  Ends a multipart message.

  @param[in]  h  ULog handle, which may be a handle to a single log
                 or a group of logs.

  @par Description
  The log is locked for the duration of the multipart message (MPM).
  This function frees the lock, ends the MPM, and completes the MPM
  bookkeeping.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
 */
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEMULTIPARTMSGEND)( ULogHandle h);

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
 * @param _FileName:  Name of the file to open, will be truncated if
 *                    size > ULOG_MAX_NAME_SIZE
 * @param FileSize:  Maximum log file size in bytes
 * @return       EFI_SUCCESS
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFILE_OPEN)
(
    ULogResult *retvalue,
    ULogHandle * h,
    CONST CHAR8 *fileName,
    UINT32 FileSize
);

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
typedef EFI_STATUS (EFIAPI *EFI_ULOGFILE_SETCONFIG)
(
    ULogResult *retvalue,
    ULogHandle *h,
    CONST ULOG_CONFIG_TYPE *settings
);

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
typedef EFI_STATUS (EFIAPI *EFI_ULOGFILE_GETCONFIG)
(
    ULogResult *retvalue,
    ULogHandle *h,
    ULOG_CONFIG_TYPE *settings
);

/* EFI_ULOGFRONT_REALTIMEVPRINTF_EX */
/** @ingroup efi_ulogfront_realtime_vprintf
  @par Summary
  Logs data in the vprintf format().

  @param[in]  h          ULog handle.
  @param[in]  dataCount  Number of parameters being printed (not
                         including formatStr); limited to ten
                         parameters.
  @param[in]  formatStr  Format string for the printf.
  @param[in]  ap         A va_list of the parameters being printed.
  @param[out] retvalue   DAL_SUCCESS if the message was logged successfully. \n
                         See #ULOG_ERRORS for descriptions of all error
                         codes.

  @par Description
  This function logs printf data to a real-time log. The printf format
  is not executed until the log is pulled. This makes a better performing
  call, but also means all strings must be present when the log is read.

  @note1hang This function serves the same purpose as real-time
             print format, except it takes a va_list argument
             instead of being variadic.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef EFI_STATUS (EFIAPI* EFI_ULOGFRONT_REALTIMEVPRINTF_EX)(
    ULogResult *retvalue,
    ULogHandle h,
    uint32 dataCount,
    uint32 paramBitField,
    const char * formatStr,
    va_list ap
);

/**
 *  <!-- EFI_ULOGCORE_READSESSIONCOMPLETE -->
 *
 * @brief Indicate that any previous read sessions are complete. No overrun detection needed 
 * on the very next read of this log. Also make sure rewind logs are rewound.
 *
 * This function is needed so that reading a log multiple times doesn't consider the
 * read jump at the start of reading to be an overrun.  Use this safely at the start
 * or end of your read session.
 *
 * @param h : A ULog handle of the log about to be read or just completed being read.
 * @return
 * EFI_SUCCESS -- Function completed successfully. \n
 * Other values -- Failure.
 */

typedef EFI_STATUS (EFIAPI *EFI_ULOGCORE_READSESSIONCOMPLETE)(
  ULogResult *retvalue,
  ULogHandle h
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_ulog_protocol
  @par Summary
  Universal Log (ULog) Driver Protocol interface.

  @par Parameters
  @inputprotoparams{ulog_proto_params.tex}
*/
typedef struct _EFI_ULOG_PROTOCOL {
  UINT64                                   Revision;
//ULog.c
  EFI_ULOGCORE_INIT                        ULogCore_Init;
  EFI_ULOGCORE_LOGCREATE                   ULogCore_LogCreate;
  EFI_ULOGCORE_HEADERSET                   ULogCore_HeaderSet;
  EFI_ULOGCORE_ATTRIBUTESET                ULogCore_AttributeSet;
  EFI_ULOGCORE_MEMORYASSIGN                ULogCore_MemoryAssign;
  EFI_ULOGCORE_ENABLE                      ULogCore_Enable;
  EFI_ULOGCORE_ISENABLED                   ULogCore_IsEnabled;
  EFI_ULOGCORE_QUERY                       ULogCore_Query;
  EFI_ULOGCORE_READ                        ULogCore_Read;
  EFI_ULOGCORE_READEX                      ULogCore_ReadEx;
  EFI_ULOGCORE_ALLOCATE                    ULogCore_Allocate;
  EFI_ULOGCORE_DISABLE                     ULogCore_Disable;
  EFI_ULOGCORE_LIST                        ULogCore_List;
  EFI_ULOGCORE_LISTEX                      ULogCore_ListEx;
  EFI_ULOGCORE_MSGFORMAT                   ULogCore_MsgFormat;
  EFI_ULOGCORE_HEADERREAD                  ULogCore_HeaderRead;
  EFI_ULOGCORE_CONNECT                     ULogCore_Connect;
  EFI_ULOGCORE_LOGLEVELSET                 ULogCore_LogLevelSet;
  EFI_ULOGCORE_LOGLEVELGET                 ULogCore_LogLevelGet;
  EFI_ULOGCORE_TEEADD                      ULogCore_TeeAdd;
  EFI_ULOGCORE_TEEREMOVE                   ULogCore_TeeRemove;
  EFI_ULOGCORE_HANDLEGET                   ULogCore_HandleGet;
  EFI_ULOGCORE_ISLOGPRESENT                ULogCore_IsLogPresent;
  EFI_ULOGCORE_SETTRANSPORTTORAM           ULogCore_SetTransportToRAM;
  EFI_ULOGCORE_SETTRANSPORTTOSTM           ULogCore_SetTransportToStm;
  EFI_ULOGCORE_SETTRANSPORTTOSTMASCII      ULogCore_SetTransportToStmAscii;
  EFI_ULOGCORE_SETTRANSPORTTOALT           ULogCore_SetTransportToAlt;
//ULogFront.c
  EFI_ULOGFRONT_RAWINIT                    ULogFront_RawInit;
  EFI_ULOGFRONT_RAWLOG                     ULogFront_RawLog;
  EFI_ULOGFRONT_REALTIMEINIT               ULogFront_RealTimeInit;
  EFI_ULOGFRONT_REALTIMEVPRINTF            ULogFront_RealTimeVprintf;
  EFI_ULOGFRONT_REALTIMECHARARRAY          ULogFront_RealTimeCharArray;
  EFI_ULOGFRONT_REALTIMESTRING             ULogFront_RealTimeString;
  EFI_ULOGFRONT_REALTIMEWORDARRAY          ULogFront_RealTimeWordArray;
  EFI_ULOGFRONT_REALTIMECSV                ULogFront_RealTimeCsv;
  EFI_ULOGFRONT_REALTIMEVECTOR             ULogFront_RealTimeVector;
  EFI_ULOGFRONT_REALTIMEMULTIPARTMSGBEGIN  ULogFront_RealTimeMultipartMsgBegin;
  EFI_ULOGFRONT_REALTIMEMULTIPARTMSGEND    ULogFront_RealTimeMultipartMsgEnd;
  EFI_ULOGFRONT_REALTIMEVPRINTF_EX         ULogFront_RealTimeVprintf_Ex;
//ULogFile.c  
  EFI_ULOGFILE_OPEN                        ULogFile_Open;
  EFI_ULOGFILE_SETCONFIG                   ULogFile_SetConfig;
  EFI_ULOGFILE_GETCONFIG                   ULogFile_GetConfig;
//ULogRead.c
  EFI_ULOGCORE_READSESSIONCOMPLETE         ULogCore_ReadSessionComplete;

 }EFI_ULOG_PROTOCOL;

#endif /* EFIULog_H */
