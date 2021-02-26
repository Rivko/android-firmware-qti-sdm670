/*============================================================================
  rpm_ULog.h

  Public-facing RPM_ULog structs, enums, and function headers

  Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
  QUALCOMM Proprietary.  Export of this technology or software is regulated 
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/
/* $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/power/aop_ulog.h#1 $ */
#ifndef RPM_ULOG_H
#define RPM_ULOG_H
#include "aop_ulogtargetcustom.h"
#include "tracer_event_ids.h"
#include <string.h>
#include <stdarg.h>
//Logs are passed around and used by their handle. 
typedef void * rpm_ULogHandle;

/* -----------------------------------------------------------------------
** RPM_ULog Constants
** ----------------------------------------------------------------------- */
// Maximum length of log name stored within the RPM_ULOG_TYPE struct
#define RPM_ULOG_MAX_NAME_SIZE 23

// Defines for the logStatus word in the header (ULOG_STRUCT) defined below.
#define RPM_ULOG_STATUS_DEFINED      (0x01 << 0)    // Set if the log is defined
#define RPM_ULOG_STATUS_ENABLED      (0x01 << 1)    // Set if the log is enabled

#define RPM_ULOG_TRANSPORT_RAM  0
#define RPM_ULOG_TRANSPORT_STM  1
#define RPM_ULOG_TRANSPORT_ALTERNATE 2

#define RPM_ULOG_TYPE_LOG_HEADER_VERSION   0x00001000
#define RPM_ULOG_TIMESTAMP_SIZE      8

//Feature Flags 1 Bits
//Timstamp size is Bit 0 of feature flags 1
#define RPM_ULOG_FF1_TS_SIZE_MASK 0x1
#define RPM_ULOG_FF1_32_BIT_TS 0
#define RPM_ULOG_FF1_64_BIT_TS 1

//the length is the top 16 bits of the first word written in msgs.
#define RPM_ULOG_LENGTH_MASK  0xFFFF0000 
#define RPM_ULOG_LENGTH_SHIFT 16

//individual RPM ULog messages are limited to 128 bytes or less
#define RPM_ULOG_MAX_MSG_SIZE 128

//Unless a log is not defined (isn't really a log yet) the minimum size is 64 bytes. 
#define RPM_ULOG_MINIMUM_LOG_SIZE 64

// Data subtypes stored in log message headers
typedef enum
{
  RPM_ULOG_SUBTYPE_RESERVED_FOR_RAW = 0,
  RPM_ULOG_SUBTYPE_REALTIME_PRINTF,
  RPM_ULOG_SUBTYPE_REALTIME_BYTEDATA,
  RPM_ULOG_SUBTYPE_REALTIME_STRINGDATA,
  RPM_ULOG_SUBTYPE_REALTIME_WORDDATA,
  RPM_ULOG_SUBTYPE_REALTIME_CSVDATA,
  RPM_ULOG_SUBTYPE_REALTIME_VECTOR,
  RPM_ULOG_SUBTYPE_REALTIME_MULTIPART,
  RPM_ULOG_SUBTYPE_REALTIME_MULTIPART_STREAM_END,
  RPM_ULOG_SUBTYPE_REALTIME_TOKENIZED_STRING,
  RPM_ULOG_SUBTYPE_RESERVED1,
  RPM_ULOG_SUBTYPE_RESERVED2,
  RPM_ULOG_SUBTYPE_RESERVED3,
  RPM_ULOG_SUBTYPE_RAW8,
  RPM_ULOG_SUBTYPE_RAW16,
  RPM_ULOG_SUBTYPE_RAW32,
  
  RPM_ULOG_SUBTYPE_REALTIME_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} RPM_ULOG_REALTIME_SUBTYPES;     


// Values that can be OR'ed into the log's usageData variable. 
typedef enum
{
  RPM_ULOG_LOG_WRAPPED = 0x0001,   // Not an error condition. Just an 
                                     // indicator that the log has wrapped
                                     // at least once.

  RPM_ULOG_ERR_LARGEMSG = 0x0002,  // A message too large for the buffer 
                                     // was logged, and therefore was dropped.
                                     // This indicates the message was too 
                                     // large for the entire buffer.

  RPM_ULOG_ERR_LARGEMSGOUT = 0x0004, // A message too large for the output 
                                       // buffer was dropped because it could
                                       // not fit.

  RPM_ULOG_ERR_RESET = 0x0008,           // The log has been reset at least once.

  RPM_ULOG_ERR_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} RPM_ULOG_STATUS_TYPE; 






// Error Codes that can be returned from RPM_ULog functions.  
typedef enum
{
  RPM_ULOG_ERROR = -1,
  RPM_ULOG_SUCCESS,
  RPM_ULOG_ERR_BUFFER_TOO_SMALL,
  RPM_ULOG_ERR_INVALIDNAME,
  RPM_ULOG_ERR_ALREADYCREATED,
  RPM_ULOG_ERR_ALREADYENABLED,
  RPM_ULOG_ERR_INVALIDHANDLE,
  RPM_ULOG_ERR_INITINCOMPLETE,
  RPM_ULOG_FAST_TO_RAM_UNAVAIL,
  RPM_ULOG_FAST_TO_RAM_FAIL,
  RPM_ULOG_FAST_TO_RAM_SUCCESS,
} rpm_ULogResult;


// These flags are used with the rpm_ULog_MemoryAssign function to indicate which 
// memory block the function should assign. The SHARED_HEADER value is depricated and 
// no longer used. 
typedef enum
{
  RPM_ULOG_VALUE_BUFFER,
  RPM_ULOG_VALUE_SHARED_HEADER,
} RPM_ULOG_VALUE_ID;


//This macro returns the amount of memory needed for a RPM_ULog with a desiredBufSize
//message buffer size. desiredBufSize must be a power of 2, and greater than 64 in order
//for the result to be accurate. This expression accounts for 32-bit word alignment.
#define RPM_ULOG_MEM_NEEDED(desiredBufSize) (((sizeof(RPM_ULOG_TYPE) + 3) & 0xFFFFFFFC) + desiredBufSize)

#define RPM_ULOGINTERNAL_PADCOUNT(x) ((x + 3) & 0xFFFFFFFC)   

//The function pointers here allow additional and custom log write implementations, and for these
//to be changed during runtime as needed without modification to the rest of the ULog logic.
typedef struct
{
  rpm_ULogResult (*write) (rpm_ULogHandle h, uint32 firstMsgCount, const char * firstMsgContent,
                             uint32 secondMsgCount, const char * secondMsgContent);
  int (*multipartMsgBegin) (rpm_ULogHandle h);   /* important to leave these in for
                                                      compatibility's sake */
  void (*multipartMsgEnd) (rpm_ULogHandle h);
} RPM_ULOG_CORE_VTABLE;



// Top-level RPM_ULog structure
typedef struct RPM_ULOG_STRUCT
{
  struct RPM_ULOG_STRUCT * next;  // Link to the next log in the system.

  uint32 version;    // Version of this header.  Needed for logs that
                     // exist across processors.
 
  char name[RPM_ULOG_MAX_NAME_SIZE+1];     // The name of the log.  
  uint32 logStatus;  // See defines above.  Contains status information on the log

  char * buffer;       // The physical log buffer pointer.
  uint32 bufSize;      // The size of the log buffer.
  uint32 bufSizeMask;  // To speed writes, the log buffer is always
                       // powers of two in length.  This value is
                       // bufSize - 1, allowing us to simply mask it with 
                       // the read and write indicies to yield the actual
                       // memory pointer.

  // Read Index.  Maintained by the reader.  Points to the
  // index where the next log message will be read.  Is used
  // by the writer to determine if a log overflow has 
  // occurred.
  volatile uint32 read;

  // Read Status Flags.  These can be updated for anything related to the
  // read path.  The bits are defined in ULOG_READ_FLAGS.
  volatile uint32 readFlags;
  
  // Write Index.  Maintained by the writer, points to the
  // index where the next log message will start.
  volatile uint32 write;

  // Trailing index.  Maintained by the writer, points to the
  // first valid message in the log.  As messages are added to 
  // the log, messages become invalid.  This pointer is maintained
  // to let the reader know where it can start reading.
  volatile uint32 readWriter;

  // Status information.  Maintained by the writer, is a bitfield
  // made up of ULOG_ERR_TYPE errors.
  uint32 usageData;

  uint32 transactionWriteCount;                 // Number of bytes written
  uint32 transactionWriteStart;                 // Starting point of the Write

   //32 bits broken up into 4 chars for various indicators.
  unsigned char transport;                      //RAM, STM, or ALT
  unsigned char protocol_num;                   //STM protocol number 0-127 are used for binary logs, 128-255 are ASCII logs
  unsigned char feature_flags1;                 //currently using one bit for 64 vs 32 bit timestamps 
  unsigned char resetCount;                     //Used to count log resets. Should be rare. 

  uint32 stmPort; 
	
  RPM_ULOG_CORE_VTABLE* altTransportVTable;

} RPM_ULOG_TYPE;


// Log context- linked list of all logs in the system
typedef struct 
{
  RPM_ULOG_TYPE * logHead;          // Pointer to a list of available logs.
} RPM_LOG_CONTEXT;



/*****************************************************************************************
*                                 Function Declarations                                  *
*****************************************************************************************/

/*---------------------------------------------------------------------------------------
  rpm_ULog_CreateNew
  Assigns from the given memory space all of the components of a RPM_ULog: 
   the top-level RPM_ULog structure and the message buffer. 
   The message buffer is sized as large as possible but must be a power of 2 and a minimum 
   64 bytes. CreateNew uses rpm_ULog_MemoryAssignLog and rpm_ULog_MemoryAssign internally
   to set up each of these components. Alternatively, the user can call these two functions
   manually and follow with a call to rpm_ULog_Enable. 
  Returns: RPM_ULOG_SUCCESS on success
           RPM_ULOG_ERR_BUFFER_TOO_SMALL if the provided memory cannot provide at least
            64 bytes for a message buffer, in addition to the other components.
           RPM_ULOG_ERROR on other errors
  Parameters: h: will be returned as a handle to the new RPM_ULog
              logName: a string to name the log (max 23 characters plus null terminator)
              mem: pointer to the memory to be used for the ULog header and log buffer.  
              effectiveMessageBufferSize: returns the actual size of the new message
               buffer. Optionally, pass NULL to prevent returning this value.
              memSize: the size of memory pointed to by 'mem' 
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_CreateNew(rpm_ULogHandle * h,
                                      const char * logName,
                                      char * mem,
                                      uint32 * effectiveMessageBufferSize,
                                      uint32 memSize);


/*---------------------------------------------------------------------------------------
  rpm_ULog_MemoryAssignLog
  Creates a new top-level RPM_ULog structure in the specified memory.
  Returns: RPM_ULOG_SUCCESS on success
           RPM_ULOG_ERR_ALREADYCREATED if the specified memory contains an active top-
            level log structure
           RPM_ULOG_ERR_BUFFER_TOO_SMALL if the memory is too small to fit the struct
           RPM_ULOG_ERR_INVALIDNAME if the logName is too long
  Parameters: h: Pointer to a log handle, which will be returned as a handle to the new
               top-level struct
              bufferPtr: pointer to the memory to assign to the top-level structure
              bufferSize: size of the memory pointed to by bufferPtr
              logName: the name to give to the log
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_MemoryAssignLog(rpm_ULogHandle * h,
                                            void * bufferPtr,
                                            uint32 bufferSize,
                                            const char * logName);



/*---------------------------------------------------------------------------------------
  rpm_ULog_MemoryAssign
  Assigns given memory to a ULog component, such as the message buffer, ReaderCore, etc
  Returns: RPM_ULOG_SUCCESS on success, RPM_ULOG_INVALID_HANDLE for a NULL h,
           RPM_ULOG_ERROR on other errors
  Parameters: h: The handle of the RPM_ULog for which to allocate memory
              id: the RPM_ULOG_VALUE_ID code for the type of memory being assigned
              bufferPtr: pointer to the memory to use
              bufferSize: the number of bytes available in the buffer
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_MemoryAssign(rpm_ULogHandle h,
                                         RPM_ULOG_VALUE_ID id,
                                         void * bufferPtr,
                                         uint32 bufferSize);


/*---------------------------------------------------------------------------------------
  rpm_ULog_Enable
  Enable and reset the log. Only needed if using rpm_ULog_MemoryAssign to manually
   assign log memory components.
  Returns: RPM_ULOG_SUCCESS on success,
           RPM_ULOG_ERR_INITINCOMPLETE if other components have not been initialized
           RPM_ULOG_ERR_ALREADYENABLED if the log has already been enabled
           RPM_ULOG_ERROR on other errors
  Parameters: h: The handle of the RPM_ULog to enable
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_Enable(rpm_ULogHandle h);



/*---------------------------------------------------------------------------------------
  rpm_ULog_Disable
  Disable the log, preventing further writes until it is enabled again
  Returns: RPM_ULOG_SUCCESS on success,
           RPM_ULOG_ERROR on errors
  Parameters: h: The handle of the RPM_ULog to disable
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_Disable(rpm_ULogHandle h);



/*---------------------------------------------------------------------------------------
  rpm_ULog_ResetLog
  Should be used only rarely in anything but test code. "Resets" a log by moving the read
   and write pointers back to the beginning of the circular RAM buffer.
  Returns: RPM_ULOG_SUCCESS on success
           RPM_ULOG_ERROR if h is NULL.
  Parameters: h: the RPM_ULog to reset
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_ResetLog(rpm_ULogHandle h);



/*---------------------------------------------------------------------------------------
  rpm_ULog_ConvertSize
  RPM_ULog message buffers must be at least 64 bytes in size and also a power of 2.
   This function returns the next biggest power of two for a given input (and at least
   64). Inputs of 32 and 127 would return 64, 1000 would return 512, etc.
  Returns: The next biggest power of 2, and at least 64.
  Parameters: size: the size to convert to the next biggest power of 2
---------------------------------------------------------------------------------------*/
uint32 rpm_ULog_ConvertSize(uint32 size);



/*---------------------------------------------------------------------------------------
 rpm_ULog_SetTransportToRAM
 Write log messages to the circular RAM buffer
 Returns: RPM_ULOG_SUCCESS if the transport is changed
          RPM_ULOR_ERROR if it isn't 
 Parameters: h : A ULog handle
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_SetTransportToRAM(rpm_ULogHandle h);



/*---------------------------------------------------------------------------------------
 rpm_ULog_SetTransportToSTM
 Write log messages to the QDSS/STM transport
 Returns: RPM_ULOG_SUCCESS if the transport is changed
          RPM_ULOR_ERROR if it isn't 
 Parameters: h : A ULog handle
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_SetTransportToSTM(rpm_ULogHandle h);



/*---------------------------------------------------------------------------------------
 rpm_ULog_SetTransportToAlt
 Write log messages using the provided handlers
 Returns: RPM_ULOG_SUCCESS if the transport is changed
          RPM_ULOR_ERROR if it isn't 
 Parameters: h : A ULog handle
             newTansportVTable : A function table including the write function
---------------------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_SetTransportToAlt(rpm_ULogHandle h,
                                              RPM_ULOG_CORE_VTABLE * newTansportVTable);





/*----------------------------------------------------------------------------
 rpm_ULog_Fast7WordWrite
 Writes 7 words to the specified log in a format compatible with Raw ULogs.
  This function works on a log that ONLY contains Fast7 messages to speed optimize 
  way space is freed when the log wraps.  It CANNOT be mixed with other types of 
  ULog writes to the same log. Doing so, would corrupt messages in that log. 
 Returns: RPM_ULOG_SUCCESS on a successful write
          RPM_ULOG_ERROR on errors
 Parameters: h : A ULog handle
             message : seven data words to write to the log
----------------------------------------------------------------------------*/
rpm_ULogResult rpm_ULog_Fast7WordWrite(rpm_ULogHandle h,
                                           uint32 message[7]);
#ifndef SWEVENT_API
#define SWEVENT_API


/*----------------------------------------------------------------------------
swevent_to_ram 
Changes the ulog transport method to internal RAM 
Calls swevent_init if not called by clients 
Returns: RPM_ULOG_SUCCESS on a successful change
          RPM_ULOG_ERROR on errors 
----------------------------------------------------------------------------*/
rpm_ULogResult swevent_to_ram(void);

/*----------------------------------------------------------------------------
swevent_to_qdss 
Changes logging transport to STM. Calls swevent init for the first call. 
Returns: RPM_ULOG_SUCCESS on a successful change
          RPM_ULOG_ERROR on errors
----------------------------------------------------------------------------*/
rpm_ULogResult swevent_to_qdss(void);


/*----------------------------------------------------------------------------
aop_log_event
 Writes 5 uint32 values to log in a format compatible with Raw ULogs.
 Compatible with fast7 messages only and can be used with fast7 logs.
 Attempt to use other write methods might corrupt other messages.
 Returns: RPM_ULOG_SUCCESS on a successful write
          RPM_ULOG_ERROR on errors
 Parameters: id : A id to define event with. See tracer_event_ids.h 
             message : seven data words to write to the log
----------------------------------------------------------------------------*/
rpm_ULogResult aop_log_event(uint32 id, uint32 d0, uint32 d1, uint32 d2, uint32 d3);

/*----------------------------------------------------------------------------
ddr_mgr_log_event
 Writes 5 uint32 values to log in a format compatible with Raw ULogs.
 Compatible with fast7 messages only and can be used with fast7 logs.
 Attempt to use other write methods might corrupt other messages.
 Returns: RPM_ULOG_SUCCESS on a successful write
          RPM_ULOG_ERROR on errors
 Parameters: id : A id to define event with. See tracer_event_ids.h 
             message : seven data words to write to the log
----------------------------------------------------------------------------*/
rpm_ULogResult ddr_mgr_log_event(uint32 id, uint32 d0, uint32 d1, uint32 d2, uint32 d3);

#define EXTEND_ARGS(...) \
    EXTEND_ARGS_(0, ##__VA_ARGS__, FILLER_ARGS())

#define EXTEND_ARGS_(...)\
    SELECT_ARGS(__VA_ARGS__)

#define SELECT_ARGS(_ignore, _1, _2, _3, _4, ...) (uint32)_1, (uint32)_2, (uint32)_3, (uint32)_4
#define FILLER_ARGS() 0, 0, 0, 0


/*---------------------------------------------------------------------------- 
SWEVENT API 
 Use this API to log sw events. See tracer_event_ids.h to add custom id, default
 logging to RAM. Use swevent_to_qdss to switch to QDSS logging.
----------------------------------------------------------------------------*/
#define SWEVENT(id, ...) aop_log_event((id), EXTEND_ARGS(__VA_ARGS__))

/*---------------------------------------------------------------------------- 
DDR MGR SWEVENT API 
 Use this API to log ddr sw events. See tracer_event_ids.h to add custom id, default
 logging to RAM. Use swevent_to_qdss to switch to QDSS logging.
----------------------------------------------------------------------------*/
#define DDR_MGR_SWEVENT(id, ...) ddr_mgr_log_event((id), EXTEND_ARGS(__VA_ARGS__))
#define DDR_SWEVENTS
#endif /*SWEVENT_API*/
#endif   /* RPM_ULOG_H */
