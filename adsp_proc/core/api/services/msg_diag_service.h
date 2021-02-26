#ifndef MSG_DIAG_SERVICE_H
#define MSG_DIAG_SERVICE_H

/*!
  @ingroup diag_message_service
  @file msg_diag_service.h 
  @brief
  All the declarations and definitions necessary to support the reporting 
  of messages for debugging. 
  @note
  Please do NOT include this file directly. Clients 
  need to include msg.h to use Diagnostic Debug Message Service.
  
  @par Some implementation details of Diag Debug Messages:
  The diagnostic extended message packet is defined to be processed as 
  efficiently as possible in the caller's context.  Therefore, the packet 
  is divided into the following logical blocks that are aligned without 
  declaring the structure as PACK:

  
@par
   1. Header
@par
   2. Constant variable length data (format string and filename string, etc).
@par
   3. Arguments ("Variable" variable-length data)
@par
   The data is delivered in this order in the packet to simplify the runtime 
   processing of each message.  All constant data is handled in DIAG task 
   context, leaving the caller's task to only process variable data at runtime.
@par
   The phone will never process the constant data directly, except to copy 
   format and filename strings. The above holds true for all MSG 2.0 macros, 
   except the SPRINTF macros.
*/

/*
Copyright (c) 2008-2014 Qualcomm Technologies, Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*==========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/services/msg_diag_service.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/21/14   xy      Replace qurt_island.h with qurt.h
07/11/14   xy      Implemented UMSG() macro for uimage mode
09/27/13   xy      Removed code under FEATURE_WINCE  
03/02/13   sg      Support for QShrink 2.0
05/25/11   vs      Added msg_status
05/26/10   mad     Doxygenated
04/08/10   vs      Pulled in optimizations for ERR macro
04/02/10   sg      Cleaning up the msg.h inclusion. Moved masks to msg_mask.h
01/22/10   sg      Added Qshrink support 
12/03/09   ps      Fixed compiler warnings & added F3trace support(SW).
09/30/09   as      Ported msg_ts and fixed WINCE featurization 
09/29/09   JV      Removed features FEATURE_ERR_EXTENDED_STORE and
                   FEATURE_SAVE_DEBUG_TRACE from this file
09/08/09   as      Removed support for FEATURE_MSG_IFACE_VIOLATION
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/17/09   mad     Merged API headers for commonality.
07/17/09   mad     Un-featurized do_save member of struct msg_const_type
07/17/09   mad     Moved msg_hdr_type and msg_desc_type to this file,removed
                   inclusion of msg_pkt_defs.h
07/17/09   mad     Included customer.h conditionally, included qw.h
05/15/09   mad     Un-featurized do_save member of struct msg_const_type
02/20/09   mad     Added macro MSG_TS and function msg_send_ts(). These let
                   clients pass in a timestamp.(Primarily for use by RETAILMSG
                   or other native-OS Debug message mechanisms that have their
                   own timestamps)
10/23/08   mad     Created from msg.h. Separated out OS-specific re-directions
                   of MSG macros into separate header files.
===========================================================================*/


#include "customer.h"


#include "comdef.h"            /* Definitions for byte, word, etc. */
#include "qw.h"                /* for qword */
#include "msg_qsr.h"
#include "msgcfg.h"
#include "qurt.h" /*for umsg*/

/*!
@ingroup diag_message_service
@name Sub-system command-codes for command-code 125
Command-code 125 configures the extended debug message service. It contains 
subcommands that are outlined below.          
*/
/*@{*/ /* start Sub-system command-codes for command-code 125 */
#define MSG_EXT_SUBCMD_MIN               (1) /*!< Place-holder for first sub-command for command-code 125 */
#define MSG_EXT_SUBCMD_GET_RANGES        (1) /*!< query the SSID range of the mobile */
#define MSG_EXT_SUBCMD_GET_BUILD_MASK    (2) /*!< query the build mask for a specific SSID */
#define MSG_EXT_SUBCMD_GET_RT_MASK       (3) /*!< query the runtime mask for a specific SSID */
#define MSG_EXT_SUBCMD_SET_RT_MASK       (4) /*!< set the runtime mask for a specific SSID */
#define MSG_EXT_SUBCMD_SET_ALL_RT_MASKS  (5) /*!< set the runtime masks for ALL SSIDs */
#define MSG_EXT_SUBCMD_MAX               (5) /*!< Place-holder for last sub-command for command-code 125 */
/*@}*/  /* end sub-system command-codes for command-code 125 */

/*!
@ingroup diag_message_service
  All constant information stored for a message.
   
  The values for the fields of this structure are known at compile time. 
  So this is to be defined as a "static " in the MACRO, so it ends up
  being defined and initialized at compile time for each and every message 
  in the software. This minimizes the amount of work to do during run time.
  
  So this structure is to be used in the "caller's" context. "Caller" is the
  client of the Message Services.
*/

typedef struct
{
  msg_desc_type desc;   /*!< contains mask, line number and subsystem-id. defined in msg_pkt_defs.h */
  const char* fmt;      /*!< Printf style format string */
  const char* fname;    /*!< Pointer to source file name */
}
msg_const_type;

/*!
@ingroup diag_message_service
This is the structure that is stored by the caller's task in msg_send ().
The DIAG task will expand the constant data into the final packet before
sending to the external device.
*/
typedef struct
{
  msg_hdr_type hdr; /*!< Header, defined in msg_pkt_defs.h */
  const msg_const_type* const_data_ptr; /*!< desc, fmt-string, file-name */
  uint32 args[1]; /*!< starting address of arguments */
}
msg_ext_store_type;


/*!
@ingroup diag_message_service
@name xx_msg_const macros
The purpose of these macros is to define the constant part of the message
that can be initialized at compile time and stored in ROM. msg_const_type 
is constructed from the Subsystem-ID (@a xx_ss_id), Mask (@a xx_ss_mask) 
and format-string (@a xx_fmt).The "static" limits the scope to the file the 
macro is called from.
@note 
Please do not call these directly, these are to be used internally,
by the different externalised MSG_ macros.
@note
XX_MSG_CONST_FMT_VAR is used when format string @a xx_fmt is passed as a variable at
runtime instead of a literal.
*/
/*@{*/ /* start group xx_msg_const macros */
/*!
Constructs a msg_const_type from the SSID, mask and format-string
*/
#define XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const msg_const_type xx_msg_const = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, (xx_fmt), msg_file}

/*!
Constructs a msg_const_type from the SSID, mask and format-string
*/
#define XX_MSG_CONST_SAVE(xx_ss_id, xx_ss_mask, xx_fmt) XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt)

/*!
XX_MSG_CONST_FMT_VAR is used when format string @a xx_fmt is passed as a variable at
runtime instead of a literal.
*/
#define XX_MSG_CONST_FMT_VAR(xx_ss_id, xx_ss_mask, xx_fmt) \
    const msg_const_type xx_msg_const = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, (xx_fmt), msg_file}
/*@}*/ /* end group xx_msg_const macros */


/*!
@ingroup diag_message_service
@name MSG2.0 macros
These are the message macros that support messages with 0-9 integer arguments.
Build-time mask-check is done against @a xx_ss_mask before macro expansion.
Defining the macro in a do{}while() limits the scope and guarantees the 
uniqueness of the variable name defined by the xx_msg_v2_const macros.
@a xx_ss_id specifies the Sub-system ID of the client, @a xx_fmt is the 
format-string and @a xx_arg1 etc specify the integer arguments.
*/
/*@{*/ /*Start group MSG2.0 macros */
/*!
This is the macro for messages with no integer parameters but only a text string.
*/
#define MSG(xx_ss_id, xx_ss_mask, xx_fmt) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send (&xx_msg_v2_const); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This macro is used if clients want to pass in a time-stamp, @a xx_ts.
In this case, Diagnostic service does NOT introduce timestamp into the message,
but uses the timestamp provided by the client.
*/
#define MSG_TS(xx_ss_id, xx_ss_mask, xx_fmt, xx_ts) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_ts (&xx_msg_const, xx_ts); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)

/*!
Macro for messages with 1 integer parameter.
*/
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_1 (&xx_msg_v2_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*! 
Macro for messages with 2 integer parameters.
*/
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_2 (&xx_msg_v2_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                      /* lint +e717 */

/*!
This is the macro for messages with 3 integer parameters.
*/
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_3 (&xx_msg_v2_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
Macro for messages with 3 integer parameters??
*/
#define MSG_3_SAVE(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, do_sav) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_3 (&xx_msg_v2_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This is the macro for messages with 4 integer parameters. In this case the 
function called needs to have more than 4 parameters so it is going to be a 
slow function call.  So for this case the  msg_send_var() uses var arg list 
supported by the compiler.
*/
#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var (&xx_msg_v2_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 5 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var(&xx_msg_v2_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 6 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var (&xx_msg_v2_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 7 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var (&xx_msg_v2_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)                                                             \
                       /* lint +e717 */

/*!
  This is the macro for messages with 8 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var (&xx_msg_v2_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */


/*!
  This is the macro for messages with 9 integer parameters. msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_v2_send_var (&xx_msg_v2_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/*@}*/ /* End group MSG2.0 macros */


/*!
@ingroup diag_message_service
@name MSG_SPRINTF_* macros
These macros are used for sprintf-style messages. Please use discretion while 
using these macros, because the string expansion happens in the client-context.
As opposed to the MSG_* macros that allow only integer arguments in addition
to a format_string, these macros allow float and string arguments, with the 
added cost.
@par
msg_sprintf() uses var arg list supported by the compiler. These macros are 
to be used when xx_fmt is passed as a literal.
*/
/*@{*/ /* starting group MSG_SPRINTF_* macros */
/*!
  This is the macro for sprintf-style messages with 1 parameter.msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/*!
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/*!
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/*!
  This is the macro for sprintf messages with 6 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 7 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for sprintf messages with 8 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
*/
#define MSG_SPRINTF_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7), \
               (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */


/*!
  This is the macro for sprintf messages with 3 parameters. This differs from 
  MSG_SPRINTF_3, in that here xx_fmt can be passed as a variable, instead of
  as a literal.
*/
#define MSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST_FMT_VAR (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/*@}*/ /* End group MSG_SPRINTF_* macros */

/*!
@ingroup diag_message_service
@name UMSG macros
These are the message macros that support messages with 0-9 integer arguments 
for uimage mode. They will check whether it is in uImage mode or not, and if 
it is in uImage, do nothing. And if it is in normal mode, then do the normal 
thing that the other MSG macros do. 
*/
/*@{*/ /*Start group UMSG macros */
/*!
Macro for Uimage messages with no integer parameters but only a text string.
*/
#define UMSG(xx_ss_id, xx_ss_mask, xx_fmt) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send (&xx_msg_v2_const); \
        /*lint +e571 */ \
      } \
	}\
    /*lint -e717 */ \
  } while (0)\


/*!
This macro is used if clients want to pass in a time-stamp, @a xx_ts.
In this case, Diagnostic service does NOT introduce timestamp into the message,
but uses the timestamp provided by the client.
*/
#define UMSG_TS(xx_ss_id, xx_ss_mask, xx_fmt, xx_ts) \
  do { \
    if (!qurt_island_get_status()){\
   /*In normal mode,use MSG_TS()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_send_ts (&xx_msg_const, xx_ts); \
        /*lint +e571 */ \
      }\
	}\
    /*lint -e717 */ \
  } while (0) \


/*!
Macro for Uimage messages with 1 integer parameter.
*/
#define UMSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_1()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
        /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_1 (&xx_msg_v2_const, (uint32) (xx_arg1)); \
        /*lint +e571 */ \
      }\
    } \
    /*lint -e717 */ \
  } while (0) \
  



/*!
Macro for Uimage messages with 2 integer parameter.
*/
#define UMSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_2()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_2 (&xx_msg_v2_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
        /*lint +e571 */ \
      }\
	} \
    /*lint -e717 */ \
  } while (0) \
  
/*!
Macro for Uimage messages with 3 integer parameter.
*/
#define UMSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
	if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_3()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_3 (&xx_msg_v2_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \
  
/*!
Macro for Uimage messages with 4 integer parameter.
*/
#define UMSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_4()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \
  
/*!
Macro for Uimage messages with 5 integer parameter.
*/
#define UMSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_5()*/ \
	  /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_var(&xx_msg_v2_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \

/*!
Macro for Uimage messages with 6 integer parameter.
*/
#define UMSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_6()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
        /*lint +e571 */ \
      } \
	}\
    /*lint -e717 */ \
  } while (0) \
  
/*!
Macro for Uimage messages with 7 integer parameter.
*/
#define UMSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_7()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
        /*lint +e571 */ \
      } \
	}\
    /*lint -e717 */ \
  } while (0) \

/*!
Macro for Uimage messages with 8 integer parameter.
*/
#define UMSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_8()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \

/*!
Macro for Uimage messages with 9 integer parameter.
*/
#define UMSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_9()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
        /*lint +e571 */ \
      } \
	}\
    /*lint -e717 */ \
  } while (0) \

/*!
  This is the macro for sprintf messages with 1 parameters in uimage mode.
*/
#define UMSG_SPRINTF_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_SPRINT_1()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0)\
  
/*!
  This is the macro for sprintf messages with 2 parameters in uimage mode.
*/
#define UMSG_SPRINTF_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
   do { \
     if (!qurt_island_get_status()){\
     /*In normal mode,use MSG_SPRINTF_2()*/ \
     /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \
  
/*!
  This is the macro for sprintf messages with 3 parameters in uimage mode.
*/
#define UMSG_SPRINTF_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_SPRINTF_3()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \
  
/*!
  This is the macro for sprintf messages with 4 parameters in uimage mode.
*/
#define UMSG_SPRINTF_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 ); \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use MSG_SPRINTF_4()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
        /*lint +e571 */ \
      } \
	}\
  /*lint -e717 */ \
  } while (0) \
  
/*!
  This is the macro for sprintf messages with 5 parameters in uimage mode.
*/
#define UMSG_SPRINTF_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 ); \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use UMSG_SPRINTF_5()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
        /*lint +e571 */ \
      }\
	} \
    /*lint -e717 */ \
  } while (0) \


/*!
  This is the macro for sprintf messages with 6 parameters in uimage mode.
*/
#define UMSG_SPRINTF_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 ); \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use UMSG_SPRINTF_6()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6)); \
        /*lint +e571 */ \
      }\
	} \
    /*lint -e717 */ \
  } while (0)  \

/*!
  This is the macro for sprintf messages with 7 parameters in uimage mode.
*/
#define UMSG_SPRINTF_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 ); \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use UMSG_SPRINTF_7()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7)); \
        /*lint +e571 */ \
      } \
	}\
    /*lint -e717 */ \
  } while (0) \

/*!
  This is the macro for sprintf messages with 8 parameters in uimage mode.
*/
#define UMSG_SPRINTF_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 ); \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use UMSG_SPRINTF_8()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7), \
               (uint32)(xx_arg8)); \
        /*lint +e571 */ \
      } \
	}\
    /*lint -e717 */ \
  } while (0) \

/*!
  This is the macro for sprintf messages with 3 parameters in uimage mode. This differs from 
  UMSG_SPRINTF_3, in that here xx_fmt can be passed as a variable, instead of
  as a literal.
*/
#define UMSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    if (!qurt_island_get_status()){\
    /*In normal mode,use UMSG_SPRINTF_8()*/ \
      /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
        /*lint +e506 +e774*/ \
        XX_MSG_CONST_FMT_VAR (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
        /*lint +e571 */ \
    } \
      }\
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/*@}*/ /* End group UMSG macros */

/*---------------------------------------------------------------------------
  The following MACROs are for LEGACY diagnostic messages support.  
---------------------------------------------------------------------------*/

/*!
@name Legacy MSG macros
@ingroup diag_message_service
The following MACROs are for LEGACY (a.k.a MSG 1.0) diagnostic messages support.
The message levels are controlled by selecting the proper macro:
@par
MSG_FATAL  for  fatal
@par
MSG_ERROR for  error
@par
MSG_HIGH  for  high
@par
MSG_MED  for   medium
and
@par
MSG_LOW for  low.
@par
These macros come under "Legacy" subsystem, and do not allow 
filtering by specific functional sub-systems. The further use of these is 
discouraged. Clients are recommended to use the MSG 2.0 macros that allow 
them to specify a sub-system to group their messages.

@param[in]  x_fmt Format string for message (printf style). Note, this parameter
must be a string literal (e.g. "Tuned to %lx"), not a variable. This is because
 the value is compiled into ROM, and can clearly not be a dynamic data type.
@param[in] a     1st parameter for format string
@param[in] b     2nd parameter for format string
@param[in] c     3rd parameter for format string

@dependencies 
  msg_init() must be called prior to these macros.
  This macro, is multiple C expressions, and cannot be used as if it
  is a single expression.

@sideeffects
  On the target hardware, the file and format strings are placed in ROM.
*/
/*@{*/ /* start group Legacy MSG macros */
#define MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define MSG_ERROR(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define MSG_HIGH(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define MSG_MED(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define MSG_LOW(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)

#define MSG_FATAL_NO_SAVE(x_fmt, a, b, c) MSG_FATAL (x_fmt, a, b, c)

#define MSG_ERROR_NO_SAVE(x_fmt, a, b, c) MSG_ERROR (x_fmt, a, b, c)

#define MSG_HIGH_NO_SAVE(x_fmt, a, b, c)  MSG_HIGH (x_fmt, a, b, c)

#define MSG_MED_NO_SAVE(x_fmt, a, b, c)   MSG_MED (x_fmt, a, b, c)

#define MSG_LOW_NO_SAVE(x_fmt, a, b, c)   MSG_LOW (x_fmt, a, b, c)
/*@}*/  /* End group Legacy MSG macros */

/*@{*/ /* start group Legacy UMSG macros */
#define UMSG_FATAL(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define UMSG_ERROR(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define UMSG_HIGH(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define UMSG_MED(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define UMSG_LOW(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)

#define UMSG_FATAL_NO_SAVE(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_FATAL (x_fmt, a, b, c)

#define UMSG_ERROR_NO_SAVE(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_ERROR (x_fmt, a, b, c)

#define UMSG_HIGH_NO_SAVE(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
     MSG_HIGH (x_fmt, a, b, c)

#define UMSG_MED_NO_SAVE(x_fmt, a, b, c) \
  if(!qurt_island_get_status())\
    MSG_MED (x_fmt, a, b, c)

#define UMSG_LOW_NO_SAVE(x_fmt, a, b, c)  \
  if(!qurt_island_get_status())\
    MSG_LOW (x_fmt, a, b, c)
/*@}*/  /* End group Legacy MSG macros */


/*! 
@cond DOXYGEN_BLOAT
*/
/*
Legacy stop-gap solutions to the need for string transport in MSG services.
note: Please do not use these, these are defined to nothing.
*/
#define MSG_STR_ERROR(x_fmt, s, a, b, c)
#define MSG_STR_HIGH(x_fmt, s, a, b, c)
#define MSG_STR_MED(x_fmt, s, a, b, c)
#define MSG_STR_LOW(x_fmt, s, a, b, c)
/*! 
@endcond
*/

/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*!
   @ingroup diag_message_service
   @brief 
   Initialize the diagnostic debug message reporting service.

   @dependencies 
   Can only be called by one task. Currently this is called by TMC.

   @sideeffects
   Sets the counts to 0, and throws away any buffered messages. 

   @return
   None
   
   @sa 
   None
*/
void msg_init(void);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with no parameters. 

   @warning
   Do not call directly; use macro MSG

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send_1, msg_send_2, msg_send_3, msg_send_var
*/
void msg_send(const msg_const_type* xx_msg_const_ptr);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 1 parameter. 

   @warning
   Do not call directly; use macro MSG_1.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter for the message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_2, msg_send_3, msg_send_var
*/
void msg_send_1(const msg_const_type* xx_msg_const_ptr, uint32 xx_arg1);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 2 parameters. 

   @warning
   Do not call directly; use macro MSG_2.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter 1 for the message
   @param[in] xx_arg2            integer parameter 2 for the message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_1, msg_send_3, msg_send_var
*/
void msg_send_2(const msg_const_type* xx_msg_const_ptr, uint32 xx_arg1,
                uint32 xx_arg2);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 3 parameters. 

   @warning
   Do not call directly; use macro MSG_3.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter 1 for the message
   @param[in] xx_arg2            integer parameter 2 for the message
   @param[in] xx_arg3            integer parameter 3 for the message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_1, msg_send_2, msg_send_var
*/
void msg_send_3(const msg_const_type* xx_msg_const_ptr, uint32 xx_arg1,
                uint32 xx_arg2, uint32 xx_arg3);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with variable number of(4 to 9) 
   parameters.  

   @warning
   Do not call directly; use macro MSG_4 ..MSG_9.

   @param[in] *xx_msg_const_ptr            static constant constructed using 
   the different xx_msg_const macros
   @param[in] num_args                     number of parameters

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_send, msg_send_1, msg_send_2, msg_send_3
*/
void msg_send_var(const msg_const_type* xx_msg_const_ptr, uint32 num_args,
                  ...);

/*!
@ingroup diag_message_service
  @brief
  This will build a message sprintf diagnostic Message with variable number
  (1 to 8) of parameters.

  @warning
  Do not call directly; use macro MSG_SPRINTF_ (1..8) macros 

  @param[in] *const_blk          static constant constructed using 
  the different xx_msg_const macros
   
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   None
*/
void msg_sprintf(const msg_const_type* const_blk, ...);

/*!
@ingroup diag_message_service
   @brief
   This will build a message with no parameter, and allow client to pass in 
   own timestamp.

   @warning
   Limited usage: only for Native Debug messages mechanisms like WM RETAILMSG 
   etc that need their timestamps preserved. Do not call directly, use 
   MSG_TS macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] timestamp           Client timestamp
   
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   None
*/
void msg_send_ts(const msg_const_type* const_blk, uint64 timestamp);

/*!
@ingroup diag_message_service
   @brief
   Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
   macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] xx_arg1            integer parameter 1
   @param[in] xx_arg2            integer parameter 2
   @param[in] xx_arg3            integer parameter 3
   @param[in] *msg               Message to be stored


   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg);
/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send_3, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] code1              integer parameter 1
   @param[in] code2              integer parameter 2
   @param[in] code3              integer parameter 3
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_3(const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send_2, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] code1              integer parameter 1
   @param[in] code2              integer parameter 2
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_2(const msg_const_type* const_blk, uint32 code1, uint32 code2);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send_1, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   @param[in] code1              integer parameter 1
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_1(const msg_const_type* const_blk, uint32 code1);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired f3 message using msg_send, in addition to invoking 
   err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro.

   @param[in] *const_blk          static constant constructed using 
   the different xx_msg_const macros
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_errlog_0(const msg_const_type* const_blk);

/*!
@name MSG_ERR_LOG macro
@ingroup diag_message_service
This macro is currently used by Error services. 
*/
/*!
This constructs the msg_const_type and calls the correct msg_v2_err_log_* functions,
based on the number of arguments to the string, given by @a NbPar.
@a xx_fmt is the format string and @a xx_arg1, @a xx_arg2 and @a xx_arg3 are 
the integer arguments to the string. This macro results in the string being 
sent to diagnostic buffer and into the Error-services buffer.
*/
/*@{*/ /* start group MSG_ERR_LOG macro */
#define MSG_ERR_LOG(NbPar, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
do                                                             \
{                                                              \
 /*lint -e506 -e774*/                                          \
 if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_MSG_SSID_DFLT))        \
 {                                                             \
   /*lint +e506 +e774*/                                        \
   XX_ERR_MSG_V2_CONST (MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt);     \
   /*lint -e571 */                                             \
   switch (NbPar)                                         \
   {                                                      \
     case 3:                                               \
       msg_v2_errlog_3 (&xx_err_msg_v2_const, (uint32)(xx_arg1), (uint32)(xx_arg2), (uint32)(xx_arg3)); \
     break;                                                \
     case 2:                                               \
       msg_v2_errlog_2 (&xx_err_msg_v2_const, (uint32)(xx_arg1), (uint32)(xx_arg2));                       \
     break;                                                \
     case 1:                                               \
       msg_v2_errlog_1 (&xx_err_msg_v2_const, (uint32)(xx_arg1));                \
     break;                                                \
     case 0:                                               \
       msg_v2_errlog_0 (&xx_err_msg_v2_const);                                   \
     break;                                                \
     default:                                               \
	 break;                                                 \
   }   \
    /*lint +e571 */                                                                         \
 }                                                                                          \
  /*lint -e717 */                                                                           \
} while (0)                                                                                 \
/* lint +e717 */
/*@}*/ /* end group MSG_ERR_LOG macro */


#ifdef __cplusplus
}
#endif

#define FEATURE_DIAG_MSG_STATUS

/*!
@ingroup diag_message_service
   @brief
    returns if mask is valid or not

   @warning
    This API is to be used infrequently; in almost all cases clients 
    should use the existing MSG macros without calling this API. 
    Clients should use this API sparingly and only in cases where 
    there is a need to avoid heavy computation that would be wasted 
    if the mask is disabled and the MSG not sent. The client may 
    check multiple masks at once for a given SSID by ORing the 
    individual bit masks together into the ss_mask parameter

   @param[in] ss_id           SSID
   @param[in] ss_mask         sub-system mask.
            
   @dependencies
   None

   @return
   boolean : TRUE : if mask is enabled
             FALSE : if mask is disabled.

   @sideeffects
   None

   @sa
    log_status
*/

boolean msg_status(uint16 ss_id, uint32 ss_mask);


#endif /* MSG_DIAG_SERVICE_H */
