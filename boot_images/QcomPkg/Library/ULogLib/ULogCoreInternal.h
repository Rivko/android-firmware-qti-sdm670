/*============================================================================
@file ULogCoreInternal.h

Core interface for ULog.

Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef ULOGCOREINTERNAL_H
#define ULOGCOREINTERNAL_H

#include "DALSys.h"
#include "CorePool.h"
#include "CoreMutex.h"
#include "ULog.h"
#ifdef TARGET_UEFI
  #include "ULogUEFI.h"
  #include <Library/EfiFileLib.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  // Read Index.  Maintained by the reader.  Points to the
  // index where the next log message will be read.  Also used
  // by the writer to determine if a log overflow (wrap) has occurred.
  volatile uint32 read;

  // Read Status Flags.  These can be updated for anything related to the
  // read path.  The bits are defined in ULOG_READ_FLAGS.
  volatile uint32 readFlags;

} ULOG_READER_CORE_TYPE;


typedef struct
{
  // Write Index.  Maintained by the writer, points to the
  // index where the next log message will start.
  volatile uint32 write;

  // Trailing index.  Maintained by the writer, points to the
  // first valid message in the log.  As messages are added to
  // the log, old messages become invalid.  This pointer is maintained
  // to let the reader know where it can start reading.
  volatile uint32 readWriter;

  // Status information,  maintained by the writer, a bitfield
  // made up of ULOG_WRITER_STATUS_TYPE values.
  uint32 usageData;

} ULOG_WRITER_CORE_TYPE;


// Round up to the nearest word.
#define ULOGINTERNAL_PADCOUNT(x) ((x + 3) & 0xFFFFFFFC)


typedef struct 
{
  union
  {
    float fData;
    unsigned int uiData[2];
  } u;
} FloatConverter;


// Multipart messages introduce challenges for streaming environments.
// In this environment the %m data won't be available so we won't try to read it.
typedef enum{
   ULOG_PARSE_DATA_STREAM = 0,
   ULOG_PARSE_READ_DATA
} ULOG_PARSE_ENVIRONMENT;


// ULOG_READ_TO_ASCII_DATA makes passing around the needed pointers and data easier.
// During a normal log read, only msgpart1 will be used, however, these decode
// functions can also be used to convert the ULogCore_Write immediately to ASCII.
// ULogCore_Write can take up to two arrays of data to be written, so we do a
// a little extra work here so we can handle data from both sources.
typedef struct
{
  char* msgpart1;
  char* msgpart2;
  uint32 part1_size;
  uint32 part2_size;
  char * outputsz;              //This pointer is frequently moved forward
  unsigned int outputsz_size;  //and the size updated accordingly.
  unsigned int outOffset;      //ouputsz[outOffset] points to the end of the string.
  ULOG_PARSE_ENVIRONMENT environment;    //ULOG_PARSE_DATA_REALTIME,  ULOG_PARSE_READ_DATA
} ULOG_READ_TO_ASCII_DATA;


#define ULOG_MINIMUM_LOG_SIZE 64

#define MIN_RAW_MSG_LEN 5
#define ULOG_MAX_MSG_SIZE 1024

#define ULOG_TYPE_HEADER_VERSION_MASK  0x80000000 //sga not used
#define ULOG_TYPE_LOG_HEADER_VERSION   0x00000005
#define ULOG_TYPE_TEE_HEADER_VERSION   0x80000001

// the length is the top 16 bits of the first word.
#define ULOG_LENGTH_MASK   0xFFFF0000
#define ULOG_LENGTH_SHIFT  16

// the argument size bitfield are the 10 bits to the right of the 16 length bits
#define ULOG_ARG_BITFIELD_MASK   0x0000FFC0
#define ULOG_ARG_BITFIELD_SHIFT  6 //sga verify that this is actually required

// the 1st bit to the right of the argument size bitfield
// indicates if the format string is 4 or 8 bytes
#define ULOG_FORMAT_STR_SIZE_MASK   0x00000020
#define ULOG_FORMAT_STR_SIZE_BIT    ( 1 << 5 ) 

//the 4 bits left of the format string size indicator are the format of the msg
#define ULOG_FORMAT_MASK         0x0000000F //sga verify this is used


// Defines for the logStatus word in the header (ULOG_STRUCT) defined below.
#define ULOG_STATUS_DEFINED      (0x1 << 0)    // Set if the log is defined
#define ULOG_STATUS_ENABLED      (0x1 << 1)    // Set if the log is enabled
#define ULOG_STATUS_MPM_ACTIVE   (0x1 << 2)    // Set if the log is currently handling a multi-part message
#define ULOG_STATUS_MALLOC_FAIL  (0x1 << 3)    // Set if a malloc failed during log creation or use
#define ULOG_STATUS_SETUP_FAIL   (0x1 << 4)    // Set if an initialization of the log failed
#define ULOG_STATUS_LOG_SW_READ_STARTED  (0x1 << 5)    // Set the first time the log is read
#define ULOG_STATUS_LAST_MSG_WAS_OVERRUN (0x1 << 7)    // Set when an overrun occurs during reading

#define ULOG_TRANSPORT_RAM  0
#define ULOG_TRANSPORT_STM  1
#define ULOG_TRANSPORT_ALTERNATE 2


// ULog will try to write directly to the ram buffer in ULogInternal_FastFmtSizeTS
// to save time. These return codes indicate if the write could be done.
#define ULOG_FAST_TO_RAM_SUCCESS  0
#define ULOG_FAST_TO_RAM_UNAVAIL  1
#define ULOG_FAST_TO_RAM_FAIL     2


#ifndef DEFEATURE_ULOG_TEE
#define HANDLE_IS_A_TEE(log)   ((log)->version & 0x80000000)
#endif

#if defined(__cplusplus)
#define ULOG_INLINE inline
#else
#define ULOG_INLINE __inline
#endif

#ifdef ULOG_64BIT_TIME
  #define ULOG_TIMESTAMP_SIZE 8
#else
  #define ULOG_TIMESTAMP_SIZE 4
#endif

#ifdef ULOG_64BIT_PROC
#define ULOG_FORMAT_STRING_SIZE 8 
#else
#define ULOG_FORMAT_STRING_SIZE 4
#endif


//for static ULog, only 10 argument printfs are put in static logs.
// static msg size = msg/fmt + TS (4/8) + format str size (4/8)
// + 10 4byte parameters (10*4)
#define ULOGSTATIC_MESSAGE_SIZE (4 + ULOG_TIMESTAMP_SIZE + ULOG_FORMAT_STRING_SIZE + 40)

typedef struct ULOG_STRUCT
{
  struct ULOG_STRUCT * next; // Link to the next log in the system.

  uint32 version;            // Version of the ULog binary format.  Needed to ensure the
                             // ULog parsing tools are appropriate for the log formatting.

  char name[ULOG_MAX_NAME_SIZE+1]; // The name of the log.

  uint32 logStatus;          // See ULOG STATUS defines above.

  char * buffer;             // The physical log buffer pointer.
  uint32 bufSize;            // The size of the log buffer.
  uint32 bufSizeMask;        // To speed writes, the log buffer is always
                             // powers of two in length.  This value is
                             // bufSize - 1, allowing us to simply mask it with
                             // the read and write indices to yield the actual
                             // memory pointer.

  uint32 defineBufSize;      // The amount of memory requested through the Create
                             // function.

  uint32 requestedSize;      // Amount of memory requested through the Allocate
                             // function.  This allows the Allocate function to
                             // be called before the create function.  The memory
                             // allocated will be the greater of the two numbers.

  uint32 bufMemType;
  ULOG_LOCK_TYPE lockType;

  CoreMutex * lockHandle;              // Handle to a lock used by the OS Lock/Int lock type.

  struct ULOG_STRUCT * sharedHeader;   // Location of shared header, intended
                                       // to support ULOG_MEMORY_CONFIG_SHARED
                                       // but not currently used.  Always points
                                       // to "this" log.
  ULOG_READER_CORE_TYPE * readerCore;  // Mapped at creation time to provide a
  ULOG_WRITER_CORE_TYPE * writerCore;  // consistent method for accessing the log
                                       // core parameters.

 uint32 transactionWriteCount;                  // Number of bytes written
 uint32 transactionWriteStart;                  // Starting point of the Write
 
  uint32 level;                                 // The level of this log.  This value
                                                // is not used beyond the get and set
                                                // functions. It is simply a storage
                                                // point.  This variable is initialized
                                                // to 0xFFFFFFFF so all log messages
                                                // will be allowed.

  char * header;                                // Optional header information.  Points to a
                                                // string that can be written by the log
                                                // owner, and read by the reader.

  uint32 attributes;                            // Log attributes that can be set with ULogCore_AttributeSet
                                                // See the "ULOG_ATTRIBUTE_" defines in ULog.h

  uint32 mpmLengthLocation;                     // The location of the length word in multi-part message.

  unsigned char transport;                      // RAM, STM, or ALT
  unsigned char protocol_num;                   // STM protocol number 0-127 are used for binary logs, 128-255 are ASCII logs
  unsigned char feature_flags1;                 // currently using one bit for 64 vs 32 bit timestamps and one bit for 32/64-bit pointers
  unsigned char resetCount;                     // Used to count log resets. Should be rare.

  void *stmTracerClientHandle;                

  ULOG_CORE_VTABLE* altTransportVTable;

  ULOG_ALT_TS_SRC altTimestampSrcFn;            // Location of alternate timestamp source, if NULL default is used

  uint32 staticMsgSlots;                        // for static logs, how many allowable messages in buffer

#ifdef TARGET_UEFI
  EFI_OPEN_FILE *OpenFile;                      // Used for ALT transport, points to the open log file.
  UINT32 MessagesSinceFlush;                    // Used for delayed flushing
  UINTN MaxLogFileSize;                         // Maximum log file size
  BOOLEAN LogHasOverlapped;                     // Used to print the indicator, if needed
  ULOG_CONFIG_TYPE *config;                     // Used to modify various log-specific settings
  EFI_OPEN_FILE *OpenFileMetaData;              // Metadata file handle corresponding to "OpenFile"
#endif

} ULOG_TYPE;

//Feature Flags 1 Bits
#define ULOG_FF1_64_BIT_TS   (0x1 << 0) // Timstamp size is Bit 0 of feature flags 1
#define ULOG_FF1_64_BIT_PROC (0x1 << 1) // 32/64-bit proc is Bit 1 of feature flags 1

#define ULOG_FF1_TS_SIZE_MASK 0x1 // sga -- not used, can't imagine when it would ever be required to use this, any objections to removing?

/*
 ULOG_TEE_TYPE is the top level tee structure passed around as a log handle.
 ULOG_TEE_LINK_TYPEs form a linked list each holding a pointer to the log. 
*/
typedef struct ULOG_TEE_LINK_STRUCT
{
  struct ULOG_TEE_LINK_STRUCT * next;
  ULOG_TYPE * log;
}ULOG_TEE_LINK_TYPE;


typedef struct ULOG_TEE_STRUCT
{
  struct ULOG_TEE_STRUCT * next;  // Link to the next tee

  uint32 version;                 // Version of this header.  Needed for logs that
                                  // exist across processors.  Must be >= 0x80000000
                                  // for a tee.

  ULOG_TEE_LINK_TYPE * link;      // Link to the logs associated with this tee.
  ULOG_TEE_LINK_TYPE * firstEnabled;      // Pointer used when log handles in a tee are iterated
  uint32 linkSize;                // Number of items in this link list.
  CoreMutex * lockHandle;        // Handle to a lock.
} ULOG_TEE_TYPE;


typedef struct 
{
  ULOG_TYPE * logHead;          // Pointer to a list of available logs.
  CoreMutex * gLockHandle;      // Handle to global lock.
  CorePoolType * teePool;       // Memory pool for log tee headers.
  CorePoolType * teeLinkPool;   // Memory pool for the link elements in a log tee.
} LOG_CONTEXT;


extern LOG_CONTEXT ulogContext;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
void ULogInternal_LockGet(ULOG_TYPE * log);
void ULogInternal_LockRelease(ULOG_TYPE * log);
uint32 ULogInternal_ConvertSize(uint32 size);
ULogResult ULogInternal_Malloc(ULOG_ALLOC_TYPE type, void ** mem, uint32 size);
ULogResult ULogInternal_NameVerify (const char *logName);
ULogResult ULogCore_ResetLog(ULogHandle h);
ULogResult ULogParsers_FormatMsg(ULOG_TYPE * log, char *  message, char * fmtMessage, uint32 fmtMessageBufSize, uint32 * bytesConsumed);
ULogResult ULogParsers_FormatParseData(ULOG_READ_TO_ASCII_DATA* parse_data, ULOG_READ_TO_ASCII_DATA* mpm);
ULogResult ULogInternal_FreeMem(ULOG_ALLOC_TYPE type, void * mem);

/**
 * <!-- ULogInternal_PublishLog -->
 *
 * Used to publish the named log to the Rendezvous server ( QRS ) if it is
 * used on the QNX platform.
 * On Non-Qnx platform this function has a NULL implementation.
 */
void ULogInternal_PublishLog(const char * logName);


/**
 * <!-- ULogInternal_InitPublisher -->
 *
 * Initializes the target specific publishing mechanism (such as QRS).
 */
void* ULogInternal_InitPublisher(void);

#ifdef __cplusplus
}
#endif

#endif // ULOGCOREINTERNAL_H

