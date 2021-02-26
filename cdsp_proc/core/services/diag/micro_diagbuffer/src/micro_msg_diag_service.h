#ifndef MICRO_MSG_DIAG_SERVICE_H
#define MICRO_MSG_DIAG_SERVICE_H

/*!
  @ingroup diag_message_service
  @file msg_diag_service_micro.h 
  @brief
  All the declarations and definitions necessary to support the reporting 
  of messages for debugging in uImage mode. 
  @note

  
  @par Some implementation details of Diag Debug Messages:
  The diagnostic extended message packet is defined to be processed as 
  efficiently as possible in the caller's context.  Therefore, the packet 
  is divided into the following logical blocks that are aligned without 
  declaring the structure as PACK:
  These APIs will be called by clients when they want to generate a new debug message. 
  The APIs will check to see if uImage mode is active or not when they are called.
  If active, the API will write the message into the uImage diag_buffer using 
  the diag_buffer interface. A dedicated diag_buffer will be set aside for uImage logging.
  This buffer will act as a circular buffer and will not be drained while in uImage mode.
  Upon exiting uImage mode, Diag task will drain out the uImage diag_buffer as part of 
  it’s regular draining process.
  
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
Copyright (c) 2017 Qualcomm Technologies, Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*==========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/micro_msg_diag_service.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
04/11/16   ph      Added support for QShrink 4.0
11/09/15   ph      Optimizations in rt maks for uimage.
08/12/15   xy      Verified DIAG uimage APIs in island mode
06/15/15   xy      Added new DIAG APIs for uImage mode
05/12/15   xy      Added support for DIAG uImage APIs
05/12/15   xy      Created file
===========================================================================*/
#include "comdef.h"		       /* Definitions for byte, word, etc. */
#include "msgcfg.h"
//#include "msg_diag_service_os_tmp.h"    /*for XX_MSG_CONST*/
#include "msg_diag_service.h"    /*for XX_MSG_CONST*/

typedef struct
{
  uint16 ssid_first;      /* Start of range of supported SSIDs */
  uint16 ssid_last;       /* Last SSID in range */
  /* Array of (ssid_last - ssid_first + 1) masks */
  uint32* rt_mask_array;
}
micro_msg_mask_type;


#define diag_island_mode_check() qurt_island_get_status() /*this function is used to check if we are in island mode*/

#define XX_MICRO_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const msg_const_type xx_msg_const  = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, (xx_fmt), msg_file}
/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with no parameters. 

   @warning
   Do not call directly; use macro MICRO_MSG

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros
   @param[in] version            Specifies the version - currently, there are two versions 
                                 0 - same as msg_v3_send_var() (for backwards compatibility)
                                 1 - The outgoing msg header will have the support to
                                 include ID field to differentiate messages
                                 coming from different instances of the processor.
   
   @param[in] *parameter         Based on the version passed in, this pointer is interpreted
                                 as follows-
                                 version       pointer
                                 0             NULL
                                 1             uint32 * . The uint32 value this pointer points to,
        				       is the ID of the instance generating the msg.
 
 
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa
*/
  void _micro_msg_send (const msg_const_type * xx_msg_const_ptr);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 1 parameter. 

   @warning
   Do not call directly; use macro MICRO_MSG_1.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
   the different xx_msg_const macros

   @param[in] version            Specifies the version - currently, there are two versions 
                                 0 - same as msg_v3_send_var() (for backwards compatibility)
                                 1 - The outgoing msg header will have the support to
                                 include ID field to differentiate messages
                                 coming from different instances of the processor.
   
   @param[in] *parameter         Based on the version passed in, this pointer is interpreted
                                 as follows-
                                 version       pointer
                                 0             NULL
                                 1             uint32 * . The uint32 value this pointer points to,
                                               is the ID of the instance generating the msg.
   @param[in] xx_arg1            Integer parameter for the message
 
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa

*/
  void _micro_msg_send_1 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 2 parameters. 

   @warning
   Do not call directly; use macro MICRO_MSG_2.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
                                 the different xx_msg_const macros
   @param[in] version            Specifies the version - currently, there are two versions 
                                 0 - same as msg_v3_send_var() (for backwards compatibility)
                                 1 - The outgoing msg header will have the support to
                                 include ID field to differentiate messages
                                 coming from different instances of the processor.
   
   @param[in] *parameter         Based on the version passed in, this pointer is interpreted
                                 as follows-
                                 version       pointer
                                 0             NULL
                                 1             uint32 * . The uint32 value this pointer points to,
                                               is the ID of the instance generating the msg.
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

*/  
  void _micro_msg_send_2 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2);

/*!
   @ingroup diag_message_service
   @brief
   This will build a new style diagnostic Message with 3 parameters. 

   @warning
   Do not call directly; use macro MICRO_MSG_3.

   @param[in] *xx_msg_const_ptr  static constant constructed using 
                                 the different xx_msg_const macros
   @param[in] version            Specifies the version - currently, there are two versions 
                                 0 - same as msg_v3_send_var() (for backwards compatibility)
                                 1 - The outgoing msg header will have the support to
                                 include ID field to differentiate messages
                                 coming from different instances of the processor.
   
   @param[in] *parameter         Based on the version passed in, this pointer is interpreted
                                 as follows-
                                 version       pointer
                                 0             NULL
                                 1             uint32 * . The uint32 value this pointer points to,
                                               is the ID of the instance generating the msg.
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

*/
  void _micro_msg_send_3 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
                                 uint32 xx_arg2, uint32 xx_arg3);

/*!
@ingroup diag_message_service
   @brief
   This will build a diagnostic Message with variable number of(4 to 9) 
   parameters.  

   @warning
   Do not call directly; use macro MICRO_MSG_4 ..MICRO_MSG_9.

   @param[in] *xx_msg_const_ptr        static constant constructed using 
                                       the different xx_msg_const macros
   @param[in] num_args                 number of parameters
   @param[in] version                  Specifies the version - currently, there are two versions 
                                       0 - same as msg_v3_send_var() (for backwards compatibility)
                                       1 - The outgoing msg header will have the support to
                                           include ID field to differentiate messages
                                           coming from different instances of the processor.
   
   @param[in] *parameter               Based on the version passed in, this pointer is interpreted
                                       as follows-
                                       version       pointer
                                       0             NULL
                                       1             uint32 * . The uint32 value this pointer points to,
                                                     is the ID of the instance generating the msg.

   @param[in] num_args                number of parameters
 
   @dependencies 
   msg_init() must have been called previously.  A free buffer must
   be available or the message will be ignored (never buffered).

   @sideeffects
   None

   @return
   None

   @sa

*/
  void _micro_msg_send_var (const msg_const_type * xx_msg_const_ptr, 
                                  uint32 num_args, ...);

/*!
  @ingroup diag_message_service
  @name MSG2.0 macros
  These are the message macros for micro image mode that support messages with 
  0-9 integer arguments. Build-time mask-check is done against @a xx_ss_mask before 
  macro expansion. Defining the macro in a do{}while() limits the scope and guarantees the 
  uniqueness of the variable name defined by the xx_msg_v3_const macros.
  @a xx_ss_id specifies the Sub-system ID of the client, @a xx_fmt is the 
  format-string, @a xx_version specifies the version of the msg, @a xx_parameter is the 
  parameter passed by the user and @a xx_arg1 etc specify the integer arguments.
*/

/*@{*/ /*Start group MSG2.0 macros */

/*!
  This is the macro for messages with no integer parameters but only a text string.
  It takes parameters to differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG(xx_ss_id, xx_ss_mask, xx_fmt) \
  do {\
    /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        if (diag_island_mode_check()){\
          XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
          _micro_msg_send(&xx_msg_const); \
         /*lint +e571 */ \
        } \
	    else{\
          XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
          msg_v2_send(&xx_msg_v2_const); \
         /*lint +e571 */ \
       } \
      }\
  /*lint -e717 */ \
  }while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 1 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_1(&xx_msg_v2_const, (uint32) (xx_arg1)); \
         /*lint +e571 */ \
      } \
	} \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 2 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
        /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_2 (&xx_msg_const,(uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        msg_v2_send_2 (&xx_msg_v2_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
         /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                      /* lint +e717 */

/*!
  This is the macro for messages with 3 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
      if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
      /*lint +e506 +e774*/ \
        if (diag_island_mode_check()){\
          XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
          _micro_msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
         /*lint +e571 */ \
        } \
        else{\
          XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
          msg_v2_send_3 (&xx_msg_v2_const,(uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 4 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4)\
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_var (&xx_msg_const,(uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 5 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_var(&xx_msg_const,(uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
        msg_v2_send_var(&xx_msg_v2_const,(uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 6 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_var (&xx_msg_const,(uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const,(uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 7 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7)\
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0)    \
                       /* lint +e717 */

/*!
  This is the macro for messages with 8 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8)\
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*!
  This is the macro for messages with 9 integer parameters. It takes parameters to 
  differentiate F3s coming from different instances of the processor
*/
#define MICRO_MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9)\
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      if (diag_island_mode_check()){\
        XX_MICRO_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
        /*lint -e571 */ \
        _micro_msg_send_var (&xx_msg_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
      } \
      else{\
        XX_MSG_V2_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
          /*lint -e571 */ \
        msg_v2_send_var (&xx_msg_v2_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
          /*lint +e571 */ \
      } \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
#endif /* MSG_DIAG_SERVICE_MICRO_H */
