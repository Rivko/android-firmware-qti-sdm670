#ifndef MSG_PKT_DEFS_H
#define MSG_PKT_DEFS_H

/*!
  @ingroup diag_message_service
  @file msg_pkt_defs.h 
  @brief
  Diagnostic Debug Message Service Packet Definitions and structures

  @par OVERVIEW:
			   The debug message service provides printf()-style debugging
			   with build-time and run-time granularity.  This granularity
			   is achieved by assigning various technology areas to a 
			   subsystem within the service.  Each subsystem defines up to
			   32 categories (denoted by a 32-bit mask) by which debug 
			   messages may be identified.  The external device may 
			   configure each subsystem individually using a 32-bit mask
			   for each subsystem.

  
  @par TERMINOLOGY: 
               Subsystem ID (SSID):
               Unique identifer given to each subsystem within the target.
               
               @par Subsystem Mask:
               32-bit value with each bit denotes a category assigned by
               the technology area assigned to the subsystem ID.
               '1' denotes the category is enabled.
               '0' denotes the category is disabled.
               This mask is specified with the definition of each message.
               
               @par Build Mask:
               A 32-bit mask specified at compile-time to determine which
               messages are to be compiled in for each subsystem.  This
               build-time granularity is available to enable build managers
               to compile out certain categories of messages to save ROM.
               During compilation, a bitwise AND ('&') is performed between
               the Build Mask and the Subsystem Mask of each message.  If
               the '&' operation is non-zero, the message is compiled in.
               
               @par Run-Time Mask (RT Mask):
               A 32-bit mask specified at by the external device at run-time
               to configure messages for a subsystem.  At run-time, the 
               message's Subsystem Mask the subsystem's RT Mask are compared
               with a bitwise AND ('&').  If the result if non-zero, the 
               message service attempts to send the message.
               
               @par Dropped Message:
               If insuffient resources exist to send a message that is 
               enabled, the message is dropped.  The next successful message
               sent will contain a count of the number of messages dropped 
               since the last successful message.

   
          @note
		  These packet definitions are part of an externalized 
          diagnostic command interface, defined in 80-V1294-1 
		  (CDMA Dual-Mode Subscriber Station Serial Data Interface Control 
		  Document). These definitions must not be changed.
*/
/*
Copyright (c) 2002, 2007, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*/

/* <EJECT> */

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1.c4/api/services/msg_pkt_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/27/10   mad     Doxygenated.
01/23/09   sg      Moved msg_desc_type, msg_hdr_type here
05/15/09   mad     Moved msg_desc_type, msg_hdr_type to msg.h. Included msg.h.
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/23/07   as      Enabled pragma pack support for WINCE targets
11/04/04   as      Added GW field for WCDMA phones in msg_ts_type
07/23/02   lad     Updated to reflect requirements changes and final 
                   implementation.
03/22/02   igt     Created file.
===========================================================================*/
#include "comdef.h"
#include "qw.h"

/* --------------------------------------------------------------------------
   Definitions and Declarations
   ----------------------------------------------------------------------- */


/*!
@ingroup diag_message_service
Structure that defines request packet sent by external device to query the range
of SSIDs supported by the mobile.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_GET_RANGES (1)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command to query SSID range */
}
msg_get_ssid_ranges_req_type;

/*!
@ingroup diag_message_service
Structure that defines response to SSID range request query. 
Response for:
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_GET_RANGES (1)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint8 status;			/*!< Status of operation */
  uint8 rsvd;			/*!< Unused */

  uint32 range_cnt;		/*!< number of Subsystem ID ranges that follow, in ssid_ranges[] */

  struct
  {
    uint16 ssid_first;		/*!< First Subsystem ID for range */
    uint16 ssid_last;		/*!< Last Subsystem ID for range */
  }
  ssid_ranges[1];           /*!< SSID range array has range_cnt number of members */
}
msg_get_ssid_ranges_rsp_type;

/*!
@ingroup diag_message_service
Structure that defines request packet sent by external device to query the 
masks (both build and run-time) corresponding to a range of SSIDs.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_GET_BUILD_MASK (2)
sub-command MSG_EXT_SUBCMD_GET_RT_MASK (3)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint16 ssid_start;	/*!< Start of subsystem ID range */
  uint16 ssid_end;      /*!< End of subsystem ID range */
}
msg_get_mask_req_type;

/*!
@ingroup diag_message_service
Structure that defines response to request packet sent by external device 
to query the masks(both build and run-time) corresponding to a range of SSIDs.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_GET_BUILD_MASK (2)
sub-command MSG_EXT_SUBCMD_GET_RT_MASK (3)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint16 ssid_start;	/*!< Start of subsystem ID range */
  uint16 ssid_end;      /*!< End of subsystem ID range */

  uint8 status;			/*!< Status of operation */
  uint8 pad;       		/*!< unused */

  uint32 bld_mask[1];	/*!< Array of buildtime (or run-time, depending on sub-command)
                           masks for subsystems in range.  Array size is:
                           'ssid_end - ssid_start + 1' */
}
msg_get_mask_rsp_type;

/*!
@ingroup diag_message_service
Structure that defines the request packet sent by external device 
to set the run-time masks corresponding to a range of SSIDs.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_SET_RT_MASK (4)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint16 ssid_start;	/*!< Start of subsystem ID range */
  uint16 ssid_end;      /*!< End of subsystem ID range */
  uint16 pad;           /*!< unused */
  uint32 rt_mask[1];	/*!< Array of runtime masks for subsystems in 
                        range.  Array size is: 'ssid_end - ssid_start + 1' */
}
msg_set_rt_mask_req_type;

/*!
@ingroup diag_message_service
Structure that defines response to the request packet sent by external device 
to set the run-time masks corresponding to a range of SSIDs.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_SET_RT_MASK (4)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint16 ssid_start;	/*!< Start of subsystem ID range */
  uint16 ssid_end;      /*!< End of subsystem ID range */
  uint8 status;			/*!< Status of operation */
  uint8 pad;            /*!< unused */
  uint32 rt_mask[1];	/*!< Array of runtime masks for subsystems in 
                        range.  Array size is: 'ssid_end - ssid_start + 1' */
}
msg_set_rt_mask_rsp_type;

/*!
@ingroup diag_message_service
Structure that defines the request packet sent by external device 
to set the run-time masks for ALL SSIDs.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_SET_ALL_RT_MASKS (5)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint16 rsvd;			/*!< unused */
  uint32 rt_mask;		/*!< Run-time mask to set in all Subsystems */
}
msg_set_all_masks_req_type;

/*!
@ingroup diag_message_service
Structure that defines response to the request packet sent by external device 
to set the run-time masks for ALL SSIDs.
@par
command-code DIAG_EXT_MSG_CONFIG_F (125)
@par
sub-command MSG_EXT_SUBCMD_SET_ALL_RT_MASKS (5)
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 sub_cmd;		/*!< Message Sub-command */
  uint8 status;			/*!< Status of operation */
  uint8 rsvd;           /*!< unused */
  uint32 rt_mask;		/*!< New Run-time Mask for all SSIDs */
}
msg_set_all_masks_rsp_type;

/*!
@ingroup diag_message_service
  This structure is stored in ROM and is copied blindly by the phone.   
  The values for the fields of this structure are known at compile time. 
  So this is to be defined as a "static const" in the MACRO, so it ends up
  being defined and initialized at compile time for each and every message 
  in the software. This minimizes the amount of work to do during run time.
  
  So this structure is to be used in the "caller's" context. "Caller" is the
  client of the Message Services.
*/
typedef struct
{
  uint16 line;			/*!< Line number in source file */
  uint16 ss_id;			/*!< Subsystem ID               */
  uint32 ss_mask;		/*!< Subsystem Mask             */
}
msg_desc_type;

/*!
@ingroup diag_message_service
  This is the message HEADER type. It contains the beginning fields of the 
  packet and is of fixed length.  These fields are filled by the calling task.
*/
typedef struct
{
  uint8 cmd_code;		/*!< Command code */
  uint8 ts_type;		/*!< Time stamp type */
  uint8 num_args;		/*!< Number of arguments in message */
  uint8 drop_cnt;		/*!< number of messages dropped since last successful message */
  qword ts;			    /*!< Time stamp */
}
msg_hdr_type;

/*!
@ingroup diag_message_service
  This is the enumeration type for all  different timestamps supported.
  At this time timestamps are 8 bytes but that may change in the future
*/
typedef enum
{
  MSG_TS_TYPE_CDMA_FULL = 0,
  MSG_TS_TYPE_WIN32,
  MSG_TS_TYPE_GW          /*!< WCDMA and GSM mode */
}
msg_ts_type;

/*!
@ingroup diag_message_service
This structure defines the Debug message packet :command-code 125, DIAG_MSG_EXT_F.
Provides debug messages with NULL terminated filename and format fields, a 
variable number of 32-bit parameters, an originator-specific timestamp 
format and filterability via subsystem ids and masks.
@par
For simplicity, only 'long' arguments are supported.  This packet
allows for N arguments, though the macros support a finite number.
@note
This is the structure that is used to represent the final structure that is 
sent to the external device.  'msg_ext_store_type' is expanded to this 
structure in DIAG task context at the time it is sent to the communication 
layer.
*/
typedef struct
{
  msg_hdr_type hdr;     /*!< Header */
  msg_desc_type desc;   /*!< line number, SSID, mask */
  uint32 args[1];		/*!< Array of long args, specified by
				        'hdr.num_args' */
						/*!< followed by NULL terminated format and file strings */
}
msg_ext_type;


/*!
@ingroup diag_message_service
@name Constants for file-name and format-string lengths for packet 31.
These constants define the maximum string-length values for command-code 31
(DIAG_MSG_F). For details, please see 80-V1294-1.
*/
/*@{*/ /* start group Constants for file-name and format-string lengths for packet 31. */
#define MSG_FILE_LEN (12+1)	       /*!< Allows full DOS file name + NULL */
#define MSG_FMT_LEN  40		       /*!< Max len of format string + NULL */
/*@}*/ /* end group Constants for file-name and format-string lengths for packet 31. */



/*!
@ingroup diag_message_service
This structure defines the packet with command-code 31 (DIAG_MSG_F).
This is sent by the external device to request a buffered debug message.
The structure is dictated by document: 80-V1294-1.
*/
typedef PACK(struct)
{
  byte cmd_code;        /*!< Command-code, this is set to 31 by external device */
  word msg_level;		/*!< Minimum message severity level */
}
msg_legacy_req_type;

/*!
@ingroup diag_message_service
This structure defines the response for the packet with command-code 31 
(DIAG_MSG_F), as defined in 80-V1294-1.
*/
typedef PACK(struct)
{
  byte cmd_code;          /*!< command-code, 31 */
  word qty;		          /*!< number of msg packets in the msg buffer
				            including the msg packet returned in this
				            response  -- if this field is 0, no msg
				            packet is included in this response */
  dword drop_cnt;		  /*!< number of msgs dropped by error services  */
  dword total_msgs;		  /*!< total number of msgs that have occured    */
  byte level;			  /*!< Severity level / Priority of this message */
  char file[MSG_FILE_LEN];/*!< Holds source file name */
  word line;			  /*!< Line number in source file  */
  char fmt[MSG_FMT_LEN];  /*!< printf style format string  */
  dword code1;			  /*!< first parameter to go with the format string */
  dword code2;            /*!< second parameter to go with the format string */
  dword code3;            /*!< third parameter to go with the format string */
  qword time;			  /*!< Time at which message was generated */
}
msg_legacy_rsp_type;



#endif		   /* MSG_PKT_DEFS_H */
