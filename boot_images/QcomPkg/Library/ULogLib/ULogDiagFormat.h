/*============================================================================
@file ULogDiagFormat.h

Interface descriptions and formatting for the ULog Diag protocol.

Copyright (c) 2010-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef ULOGDIAGFORMAT_H
#define ULOGDIAGFORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
   Subsystem command codes
============================================================================*/
typedef enum
{
  ULOGDIAG_VERSION = 0,
  ULOGDIAG_CONNECT,
  ULOGDIAG_ENABLE,
  ULOGDIAG_ALLOCATE,
  ULOGDIAG_LIST,
  ULOGDIAG_READ_FORMAT,
  ULOGDIAG_HEADER_READ,
  ULOGDIAG_DISABLE,
  ULOGDIAG_CONNECT_EX,
  ULOGDIAG_SETTRANSPORT_RAM,
  ULOGDIAG_SETTRANSPORT_STM,
  ULOGDIAG_SETTRANSPORT_STMASCII,
  ULOGDIAG_PLUGIN,
  ULOGDIAG_MAX_CMD,
  ULOGDIAG_FORCE_2_BYTES = 0x7fff,
} ULOGDIAG_CommandsType;


// Define the processor IDs supported by the system.
//
#define ULOGDIAG_PROCESSOR_APPS      0x0100
#define ULOGDIAG_PROCESSOR_MODEM     0x0200
#define ULOGDIAG_PROCESSOR_MODEMCTRL 0x0300
#define ULOGDIAG_PROCESSOR_RPM       0x0400
#define ULOGDIAG_PROCESSOR_LPA       0x0500
#define ULOGDIAG_PROCESSOR_WCN       0x0600


/*============================================================================
 *
 * ULog Message Header
 *
 *============================================================================*/
typedef struct
{
  char cmdCode;
  char subsysId;
  unsigned short int subsysCmdCode;
} ulogdiag_msg_header;


/*============================================================================
 * Version
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
} ulogdiag_cmd_type_version;


// Version response
typedef struct
{
  ulogdiag_msg_header header;
  unsigned long int majorVersion;
  unsigned long int minorVersion;
} ulogdiag_cmd_rsp_type_version;


/*============================================================================
 * Connect
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  char logName[24];
} ulogdiag_cmd_type_connect;


// Connect response
typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
  signed int result;
} ulogdiag_cmd_rsp_type_connect;


/*============================================================================
 * Connect_Ex
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  // Name of the log to connect.
  char logName[24];
  // Input process name 
  char processName[24];
} ulogdiag_cmd_type_connect_ex;


// Connect response
typedef struct
{
  ulogdiag_msg_header header;
  // The actual log handle data
  void * handle;
  // Output process name
  char processName[24];

  signed int result;
} ulogdiag_cmd_rsp_type_connect_ex;


/*============================================================================
 * Enable
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
} ulogdiag_cmd_type_enable;


// Enable response
typedef struct
{
  ulogdiag_msg_header header;
  signed int result;
} ulogdiag_cmd_rsp_type_enable;


/*============================================================================
 * Allocate
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
  unsigned long int bufferSize;
} ulogdiag_cmd_type_allocate;


// Allocate response
typedef struct
{
  ulogdiag_msg_header header;
  signed int result;
} ulogdiag_cmd_rsp_type_allocate;


/*============================================================================
 * List
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  signed int logListReadOffset;
} ulogdiag_cmd_type_list;


// List response
typedef struct
{
  ulogdiag_msg_header header;
  unsigned long int result;
  unsigned long int registeredCount;
  unsigned long int offsetCount;
  unsigned long int namesReadCount;  
  char nameArray[1024];
} ulogdiag_cmd_rsp_type_list;


/*============================================================================
 * Read and Format
 *===========================================================================*/

// Values for the outputFormat entry of ulogdiag_cmd_type_read
#define ULOGDIAG_FORMAT_ASCII    0
#define ULOGDIAG_FORMAT_BINARY   1
// the following two definitions are used for the outputFormat fields 
// of the diag cmd read request response. They indicate the ususal format 
// field and indicate that the last message in the read was invalid
#define ULOGDIAG_FORMAT_ASCII_OVERRUN_DETECTED     2
#define ULOGDIAG_FORMAT_BINARY_OVERRUN_DETECTED    3

typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
  unsigned long int outputFormat;
} ulogdiag_cmd_type_read;


// Read and Format response
typedef struct
{
  ulogdiag_msg_header header;
  unsigned long int outputFormat;
  unsigned long int outputLength;
  char response[1024];
} ulogdiag_cmd_rsp_type_read;


/*============================================================================
 * Header Read
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
  unsigned long int headerReadOffset;
} ulogdiag_cmd_type_header_read;


// Header Read response
typedef struct
{
  ulogdiag_msg_header header;
  unsigned long int result;
  unsigned long int headerLength;
  char headerData[1024];
} ulogdiag_cmd_rsp_type_header_read;


/*============================================================================
 * Disable
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
} ulogdiag_cmd_type_disable;


// Enable response
typedef struct
{
  ulogdiag_msg_header header;
  signed int result;
} ulogdiag_cmd_rsp_type_disable;


/*============================================================================
 * Set Transport
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  void * handle;
  char  protocol_num;
}ulogdiag_cmd_type_settransport;


//set transport response
typedef struct
{
  ulogdiag_msg_header header;
  signed int result;
}ulogdiag_cmd_rsp_type_settransport;



/*============================================================================
 * Plugin
 *===========================================================================*/
typedef struct
{
  ulogdiag_msg_header header;
  unsigned long int plugin_id;
  unsigned long int msg_to_plugin;
}ulogdiag_cmd_type_plugin;


//set transport response
typedef struct
{
  ulogdiag_msg_header header;
  signed int result;
}ulogdiag_cmd_rsp_type_plugin_cmd;

#ifdef __cplusplus
}
#endif


#endif /*  ULOGDIAGFORMAT_H */

