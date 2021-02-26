#ifndef MSG_QSR_H
#define MSG_QSR_H
/*
 QSHRINK SUPPORTED = 20
*/

/* 
   Qshrink parser will search for the above line in this file and only then 
   it will modify source code based on the new parameter processing feature
   Otherwise it will modify source code as before. 
*/

/*!
  @ingroup diag_message_service
  @file msg_qsr.h 
  @brief
  All the declarations and definitions necessary to support the optimized f3 
  messages (a.k.a. QSHRINK messages) for errors and debugging.

  This file includes support for the extended capabilities as well as the 
  legacy messaging scheme.
  @note
  Please do NOT include this file directly. Clients 
  need to include msg.h to use Diagnostic Debug Message Service.
  
  @par 
  The Qshrink message service is designed to optimize memory use in debug 
  messages. Qshrink extended messages do not contain format strings and file 
  names as compared to extended debug messages (Command_code 121/0x79). The 
  debug strings associated with debug messages are hashed and stored locally 
  to reduce their memory foot-print. The hash value is sent as a message field 
  to the tool to print the actual debug string.
  The packet with CMD_CODE 146/0x92 is generated by the DMSS with no 
  associated request packet. The service is configured using the Extended 
  Configuration Request/Response message (command 125)
*/

/*
Copyright (c) 2010-2013, 2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/msg_qsr.h#1 $

when       who     what, where, why
--------   ---   ----------------------------------------------------------
10/09/17   is      Featurize Qshrink attribute
03/02/13   sg      Support for QShrink 2.0
03/31/11   hm      Changes in QSR_MSG_XXX macros to support Parameter Processing
06/10/10   mad     Doxygenated.
06/08/10   vs      Fixed compile error in QSR_MSG_ERR_LOG
05/20/10   vs      Changed QSR_ERR_LOG to QSR_MSG_ERR_LOG
04/08/10   vs      Changes to include memory optimizations for ERR macro
04/01/10   sg      Removed the undefines for QSR_MSG_LOW,MEDIUM,HIGH,ERROR,
                   FATAL
10/11/09   vg      8200 Warning Clean Up
20/05/09   ps      Created new file for qsr feature based on msg.h

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"            /* Definitions for byte, word, etc. */
#include "msg_pkt_defs.h"
#include "msgcfg.h" 


/*---------------------------------------------------------------------------
  The extended message packet is defined to be processed as efficiently as
  possible in the caller's context.  Therefore, the packet is divided into
  logical blocks that are aligned w/out declaring the structure as PACK.
  
  A header, static constant block, and argument list are defined to minimize
  the work done by the caller's task.
   
   1. Header
   2. Constant variable length data (format string and filename string, etc).
   3. Arguments ("Variable" variable-length data)
   
  The data is delivered in this order in the packet to simplify the runtime 
  processing of each message.  All constant data is handled in DIAG task 
  context, leaving the caller's task to only process variable data at runtime.
   
  The phone will never process the constant data directly, except to copy 
  format and filename strings. 
---------------------------------------------------------------------------*/

/*!
@ingroup diag_message_service
  All constant information stored for an optimized debug message.
   
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
  uint32 msg_hash;       /*!< message hash value */
}
msg_qsr_const_type;

/*!
@ingroup diag_message_service
  All constant information stored for an optimized debug message, for use by 
  error service.
   
  The values for the fields of this structure are known at compile time. 
  So this is to be defined as a "static " in the MACRO, so it ends up
  being defined and initialized at compile time for each and every message 
  in the software. This minimizes the amount of work to do during run time.
  
  So this structure is to be used in the "caller's" context. "Caller" is the
  client of the Message Services.
*/
typedef struct
{
  msg_qsr_const_type qsr_const_blk; /*!< contains mask, line number, subsystem-id and hash value. */
  const char *fname;                /*!<file name */
} err_msg_qsr_const_type;

/*!
Bit flag for qsr_flag to check if msg contains a QSR hash.
*/
#define QSR_BITFLAG_IS_HASH (0x00000001)

/*!
@ingroup diag_message_service
    This is the structure that is stored by the caller's task in qsr_msg_send().
    The DIAG task will expand the constant data into the final packet before
    sending to the external device.
*/
typedef struct
{
  msg_hdr_type hdr; /*!< Header, defined in msg_pkt_defs.h */
  const msg_qsr_const_type* qsr_const_data_ptr; /*!< contains mask, line number, subsystem-id and hash value. */
  uint32 qsr_flag; /*!< Bit 0: True, if hash value is passed, False, if string pointer is passed */
  uint32 args[1];   /*!< starting address of arguments */
}
msg_qsr_store_type;

/*!
@ingroup diag_message_service
@name XX_QSR_MSG_CONST macros
The purpose of these macros is to define the constant part of the message
that can be initialized at compile time and stored in ROM. msg_qsr_const_type 
is constructed from the Subsystem-ID (@a xx_ss_id), Mask (@a xx_ss_mask) 
and hash value (@a hash).The "static" limits the scope to the file the 
macro is called from.
@note 
Please do not call these directly, these are to be used internally,
by the different externalised MSG_ macros, which in-turn get text-substituted 
with QSR_MSG_ macros and the corresponding hash.
@note
XX_QSR_MSG_CONST_FMT_VAR can be removed. (!!!!!!!)
*/
/*@{*/ /* start group XX_QSR_MSG_CONST macros */         
/*!
Constructs a msg_qsr_const_type from the SSID, mask and message hash.
*/
  #define XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash) \
    static const msg_qsr_const_type xx_msg_qsr_const = { \
      {__LINE__, (xx_ss_id), (xx_ss_mask)}, hash}
/*!
Constructs a msg_qsr_const_type from the SSID, mask and message hash.
*/
  #define XX_QSR_MSG_CONST_FMT_VAR(xx_ss_id, xx_ss_mask,hash) \
    const msg_qsr_const_type xx_msg_qsr_const = { \
      {__LINE__, (xx_ss_id), (xx_ss_mask)}, hash}
/*!
Constructs an err_msg_qsr_const_type from the SSID, mask, message hash and file-name.
*/
  #define XX_ERR_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash) \
    static const err_msg_qsr_const_type xx_err_msg_qsr_const = { \
      {{__LINE__, (xx_ss_id), (xx_ss_mask)}, hash}, msg_file}
/*@}*/ /* end group XX_QSR_MSG_CONST macros */

/*!
@ingroup diag_message_service
@name QSR MSG 2.0 macros
These are the optimized message macros that support messages with 0-9 integer 
arguments.Build-time mask-check is done against @a xx_ss_mask before macro expansion.
Defining the macro in a do{}while() limits the scope and guarantees the 
uniqueness of the variable name defined by the XX_QSR_MSG_CONST macros.
@a xx_ss_id specifies the Sub-system ID of the client, @a xx_fmt is the 
format-string that gets replaced with the hash value and @a xx_arg1 etc 
specify the integer arguments.
*/
/*@{*/ /*Start group QSR MSG 2.0 macros */

typedef struct
{
  msg_desc_type desc; /*!< ss_mask, line, ss_id */
  const char * msg;   /*!< filename + format string - This field may be changed to a hash value when the 
                           QSHRINK20 script runs on the linker output */
}
msg_v2_const_type;

typedef struct
{
  msg_v2_const_type msg_v2_const_blk;     /*!< contains mask, line number, subsystem-id and hash value. */
  const char *fname;                      /*!<file name */
} err_msg_v2_const_type;

#ifdef __cplusplus
  #define SECT_EXT __FILE__
#else
  #define SECT_EXT
#endif


  
#ifdef FEATURE_DISABLE_DIAG_QSHRINK
  /*!
    Disables Qshrink 2.0 from Diag
  */
  #define QSR_ATTR
#else
  /*!
    QShrink attribute flag to move QSR strings into QSR_STR region of scatter load file
  */
  #define QSR_ATTR __attribute__((section ("QSR_STR.fmt.rodata." SECT_EXT),aligned (4))) 
#endif

  /*!
    Constructs a msg_v2_const_type from the SSID, mask and format-string
  */
  #define XX_MSG_V2_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const char QSR_string_ptr[] QSR_ATTR  \
        = msg_file ":" xx_fmt ; \
    static const msg_v2_const_type xx_msg_v2_const = { \
      {__LINE__, (xx_ss_id), (xx_ss_mask)}, \
          (char*)&QSR_string_ptr }
    
  /*!
    Constructs a err_msg_v2_const_type from the SSID, mask and format-string
  */
  #define XX_ERR_MSG_V2_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const char QSR_string_ptr[] QSR_ATTR  \
       = msg_file ":" xx_fmt ; \
    static const err_msg_v2_const_type xx_err_msg_v2_const = { \
      {{__LINE__, (xx_ss_id), (xx_ss_mask)}, \
          (char*)&QSR_string_ptr} , msg_file}
    

/*! 
  This is the optimized macro for parameter processing.
*/
#define QSR_MSG_N(hash,NbPar,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) \
    { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      switch (NbPar)                                           \
      {                                                          \
        case 3:                                                   \
        qsr_msg_send_3( &xx_msg_qsr_const,(uint32) (xx_arg1), (uint32) (xx_arg2), \
                                    (uint32) (xx_arg3)); \
        break;                                                    \
        case 2:                                                   \
        qsr_msg_send_2( &xx_msg_qsr_const,(uint32) (xx_arg1), (uint32) (xx_arg2)); \
        break;                                                    \
        case 1:                                                   \
        qsr_msg_send_1( &xx_msg_qsr_const,(uint32) (xx_arg1));         \
        break;                                                    \
        case 0:                                                   \
        qsr_msg_send (&xx_msg_qsr_const);                          \
        break;                                                    \
        default:                                               \
        qsr_msg_send_3( &xx_msg_qsr_const,(uint32) (xx_arg1), (uint32) (xx_arg2), \
                                    (uint32) (xx_arg3)); \
        break;                                                 \
      }                   \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)  \
  /* lint +e717 */
/*! 
This is the macro for optimized messages with no integer parameters but only a 
text string.
*/
#define QSR_MSG(hash, xx_ss_id, xx_ss_mask, xx_fmt) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send (&xx_msg_qsr_const); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This is the macro for optimized messages with 1 integer parameter.
*/
#define QSR_MSG_1(hash, xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_1 (&xx_msg_qsr_const, (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This is the macro for optimized messages with 2 integer parameters.
*/
#define QSR_MSG_2(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_2 (&xx_msg_qsr_const,(uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
This is the macro for optimized messages with 3 integer parameters.
*/
#define QSR_MSG_3(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_3( &xx_msg_qsr_const,(uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for optimized messages with 4 parameters. In this case the 
  function called needs to have more than 4 parameters so it is going to be a slow 
  function call.  So for this case the  qsr_msg_send_var() uses var arg list 
  supported by the compiler.
*/
#define QSR_MSG_4(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var (&xx_msg_qsr_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for optimized messages with 5 parameters. qsr_msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define QSR_MSG_5(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for optimized messages with 6 parameters. qsr_msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define QSR_MSG_6(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var (&xx_msg_qsr_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for optimized messages with 7 parameters. qsr_msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define QSR_MSG_7(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)                                                             \
                       /* lint +e717 */

/*!
  This is the macro for optimized messages with 8 parameters. qsr_msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define QSR_MSG_8(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for optimized messages with 9 parameters. qsr_msg_send_var() 
  uses var arg list supported by the compiler.
*/
#define QSR_MSG_9(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */


// TODO: MSG_EXPENSIVE: on target string expansion (name?)

/*---------------------------------------------------------------------------
  The following MACROs are for LEGACY diagnostic messages support.  
---------------------------------------------------------------------------*/

/*!
@name Legacy MSG macros for QSHRINK
@ingroup diag_message_service
The following MACROs are for LEGACY (a.k.a MSG 1.0) diagnostic messages support 
for optimized debug messages.
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
@note
These macros come under "Legacy" subsystem, and do not allow 
filtering by specific functional sub-systems.

@param[in]  x_fmt Format string for message (printf style). Note, this parameter
must be a string literal (e.g. "Tuned to %lx"), not a variable. This is because
 the value is compiled into ROM, and can clearly not be a dynamic data type.
@param[in] a     1st parameter for format string
@param[in] b     2nd parameter for format string
@param[in] c     3rd parameter for format string

@dependencies 
  msg_init() must be called prior to this macro referencing msg_put().
  These macros, are multiple C expressions, and cannot be used as if it
  is a single expression.

@sideeffects
  On the target hardware, the hash values are placed in ROM.
*/
/*@{*/ /* start group Legacy MSG macros for QSHRINK */
#define QSR_MSG_FATAL(hash,x_fmt, a, b, c) \
    QSR_MSG_3 (hash,MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define QSR_MSG_ERROR(hash, x_fmt, a, b, c) \
    QSR_MSG_3 (hash, MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define QSR_MSG_HIGH(hash, x_fmt, a, b, c) \
    QSR_MSG_3 (hash, MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define QSR_MSG_MED(hash,x_fmt, a, b, c) \
    QSR_MSG_3 (hash,MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define QSR_MSG_LOW(hash,x_fmt, a, b, c) \
    QSR_MSG_3 (hash,MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)
/*@}*/ /* end group Legacy MSG macros for QSHRINK */

/*@{*/ /* start group Legacy MSG macros with Parameter Preocessing for QSHRINK */
#define QSR_MSG_FATAL_N(hash,NbPar,x_fmt, a, b, c) \
    QSR_MSG_N (hash,NbPar,MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define QSR_MSG_ERROR_N(hash,NbPar, x_fmt, a, b, c) \
    QSR_MSG_N (hash,NbPar, MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define QSR_MSG_HIGH_N(hash,NbPar, x_fmt, a, b, c) \
    QSR_MSG_N (hash,NbPar, MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define QSR_MSG_MED_N(hash,NbPar,x_fmt, a, b, c) \
    QSR_MSG_N (hash,NbPar,MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define QSR_MSG_LOW_N(hash,NbPar,x_fmt, a, b, c) \
    QSR_MSG_N (hash,NbPar,MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)
/*@}*/ /* end group Legacy MSG macros with Parameter Preocessing for QSHRINK */

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
   This will build a new style optimized diagnostic message with no parameters. 

   @warning
   Do not call directly; use macro MSG that gets text-replaced to QSR_MSG

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different XX_QSR_MSG_CONST macros

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   qsr_msg_send_1, qsr_msg_send_2, qsr_msg_send_3, qsr_msg_send_var
*/
  void qsr_msg_send ( const msg_qsr_const_type * xx_msg_const_ptr);


/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with no parameters. 

   @warning
   Do not call directly; use macro MSG()

   @param[in] *xx_msg_v2_const static constant constructed using 
   the different XX_MSG_V2_CONST macros

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_v2_send_1, msg_v2_send_2, msg_v2_send_3, msg_v2_send_var
*/
  void msg_v2_send ( const msg_v2_const_type * xx_msg_const_ptr);


/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with 1 parameter. 

   @warning
   Do not call directly; use macro MSG_1 that gets text-replaced to QSR_MSG_1

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different XX_QSR_MSG_CONST macros
   @param[in]  xx_arg1           integer parameter 1 for the optimized message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   qsr_msg_send, qsr_msg_send_2, qsr_msg_send_3, qsr_msg_send_var
*/
  void qsr_msg_send_1 (const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with 1 parameter. 

   @warning
   Do not call directly; use macro MSG_1()

   @param[in] *xx_msg_v2_const  static constant constructed using 
   the different XX_MSG_V2_CONST macros
   @param[in]  xx_arg1           integer parameter 1 for the optimized message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_v2_send, msg_v2_send_2, msg_v2_send_3, msg_v2_send_var
*/
  void msg_v2_send_1 (const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1);


/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with 2 parameters. 

   @warning
   Do not call directly; use macro MSG_2 that gets text-replaced to QSR_MSG_2

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different XX_QSR_MSG_CONST macros
   @param[in]  xx_arg1           integer parameter 1 for the optimized message
   @param[in]  xx_arg2           integer parameter 2 for the optimized message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   qsr_msg_send, qsr_msg_send_1, qsr_msg_send_3, qsr_msg_send_var
*/
  void qsr_msg_send_2 ( const msg_qsr_const_type * xx_msg_const_ptr,uint32 xx_arg1,
    uint32 xx_arg2);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with 2 parameters. 

   @warning
   Do not call directly; use macro MSG_2()

   @param[in] *xx_msg_v2_const  static constant constructed using 
   the different XX_MSG_V2_CONST macros
   @param[in]  xx_arg1           integer parameter 1 for the optimized message
   @param[in]  xx_arg2           integer parameter 2 for the optimized message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_v2_send, msg_v2_send_1, msg_v2_send_3, msg_v2_send_var
*/
  void msg_v2_send_2 ( const msg_v2_const_type * xx_msg_const_ptr,uint32 xx_arg1,
    uint32 xx_arg2);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with 3 parameters. 

   @warning
   Do not call directly; use macro MSG_3 that gets text-replaced to QSR_MSG_3

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different XX_QSR_MSG_CONST macros
   @param[in]  xx_arg1           integer parameter 1 for the optimized message
   @param[in]  xx_arg2           integer parameter 2 for the optimized message
   @param[in]  xx_arg3           integer parameter 3 for the optimized message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   qsr_msg_send, qsr_msg_send_1, qsr_msg_send_3, qsr_msg_send_var
*/
  void qsr_msg_send_3 ( const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with 3 parameters. 

   @warning
   Do not call directly; use macro MSG_3()

   @param[in] *xx_msg_v2_const  static constant constructed using 
   the different XX_MSG_V2_CONST macros
   @param[in]  xx_arg1           integer parameter 1 for the optimized message
   @param[in]  xx_arg2           integer parameter 2 for the optimized message
   @param[in]  xx_arg3           integer parameter 3 for the optimized message

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_v2_send, msg_v2_send_1, msg_v2_send_2, msg_v2_send_var
*/
  void msg_v2_send_3 ( const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);


/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with variable number 
   of (4 to 9) parameters.  

   @warning
   Do not call directly; use macro MSG_4 ..MSG_9, that get text-replaced to 
   QSR_MSG_4 .. QSR_MSG_9

   @param[in] *xx_msg_const_ptr            static constant constructed using 
   the different XX_QSR_MSG_CONST macros
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
  void qsr_msg_send_var ( const msg_qsr_const_type * xx_msg_const_ptr, uint32 num_args, ...);

/*!
@ingroup diag_message_service
   @brief
   This will build a new style optimized diagnostic message with variable number 
   of (4 to 9) parameters.  

   @warning
   Do not call directly; use macros MSG_4(), ..MSG_9()

   @param[in] *xx_msg_v2_const            static constant constructed using 
   the different XX_MSG_V2_CONST macros
   @param[in] num_args                     number of parameters

   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
   msg_v2_send, msg_v2_send_1, msg_v2_send_2, msg_v2_send_3
*/
  void msg_v2_send_var ( const msg_v2_const_type * xx_msg_const_ptr, uint32 num_args, ...);


/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using qsr_msg_send_3, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro that gets text-replaced 
   to QSR_MSG_ERR_LOG.

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
void msg_qsrerrlog_3 (const err_msg_qsr_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using qsr_msg_send_2, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro that gets text-replaced 
   to QSR_MSG_ERR_LOG.

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
void msg_qsrerrlog_2 (const err_msg_qsr_const_type* const_blk, uint32 code1, uint32 code2);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using qsr_msg_send_1, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro that gets text-replaced 
   to QSR_MSG_ERR_LOG.

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
void msg_qsrerrlog_1 (const err_msg_qsr_const_type* const_blk, uint32 code1);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using qsr_msg_send, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro that gets text-replaced 
   to QSR_MSG_ERR_LOG.

   @param[in] *const_blk          static constant constructed using 
   the different xx_err_msg_v2_const macros
   
   @dependencies
   None

   @return
   None

   @sideeffects
   None

   @sa
   None
*/
void msg_qsrerrlog_0 (const err_msg_qsr_const_type* const_blk);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using msg_v2_send_3, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro

   @param[in] *const_blk          static constant constructed using 
   the different xx_err_msg_v2_const macros
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
void msg_v2_errlog_3 (const err_msg_v2_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using msg_v2_send_2, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro

   @param[in] *const_blk          static constant constructed using 
   the different xx_err_msg_v2_const macros
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
void msg_v2_errlog_2 (const err_msg_v2_const_type* const_blk, uint32 code1, uint32 code2);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using msg_v2_send_1, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro

   @param[in] *const_blk          static constant constructed using 
   the different xx_err_msg_v2_const macros
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
void msg_v2_errlog_1 (const err_msg_v2_const_type* const_blk, uint32 code1);

/*!
@ingroup diag_message_service
   @brief
   Sends the desired optimized debug message using msg_v2_send, in addition 
   to invoking err_put_log. This reduces one function call for top level macro.

   @warning
   Do not call directly, use MSG_ERR_LOG macro

   @param[in] *const_blk          static constant constructed using 
   the different xx_err_msg_v2_const macros
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
void msg_v2_errlog_0 (const err_msg_v2_const_type* const_blk);


/*!
@name QSR_MSG_ERR_LOG macro
@ingroup diag_message_service
This macro is currently used by Error services. 
*/
/*!
This constructs the err_msg_qsr_const_type and calls the correct msg_qsrerrlog_* 
functions, based on the number of arguments to the string, given by @a NbPar.
@a hash is the message hash, @a xx_fmt is the format string and @a xx_arg1, 
@a xx_arg2 and @a xx_arg3 are the integer arguments to the string. 
This macro results in the string being sent to diagnostic buffer and into 
the Error-services buffer.
*/
/*@{*/ /* start group QSR_MSG_ERR_LOG macro */
#define QSR_MSG_ERR_LOG(hash, NbPar, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
do \
{  \
 /*lint -e506 -e774*/ \
 if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_MSG_SSID_DFLT)) \
 { \
 /*lint +e506 +e774*/ \
  XX_ERR_QSR_MSG_CONST (MSG_SSID_DFLT, MSG_LEGACY_ERROR, hash); \
   /*lint -e571 */ \
  switch (NbPar)                                           \
  {                                                          \
    case 3:                                                   \
    msg_qsrerrlog_3 (&xx_err_msg_qsr_const, (uint32)xx_arg1,(uint32) xx_arg2,(uint32) xx_arg3);     \
    break;                                                    \
    case 2:                                                   \
    msg_qsrerrlog_2 (&xx_err_msg_qsr_const,(uint32) xx_arg1, (uint32)xx_arg2); \
    break;                                                    \
    case 1:                                                   \
    msg_qsrerrlog_1 (&xx_err_msg_qsr_const, (uint32)xx_arg1);         \
    break;                                                    \
    case 0:                                                   \
    msg_qsrerrlog_0 (&xx_err_msg_qsr_const);                          \
    break;                                                    \
     default:                                               \
     break;                                                 \
  }                   \
      /*lint +e571 */ \
 } \
  /*lint -e717 */ \
} while (0) \
/* lint +e717 */
/*@}*/ /* end group QSR_MSG_ERR_LOG macro */

#ifdef __cplusplus
}
#endif

#endif  /* MSG_QSR_H */

