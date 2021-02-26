#ifndef MSG_MASK_H
#define MSG_MASK_H
/*!
  @ingroup diag_message_service
  @file msg_mask.h 
  @brief
  Diagnostic Debug Message Service header file for mask definitions.
  
  @details
  This file includes support for the masks for MSG 2.0 macros (MSG_1 etc)
  as well as the legacy debug messaging scheme (MSG_HIGH etc).
*/

/*
Copyright (c) 2010-2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/*==========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/msg_mask.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/27/13   xy      Removed code under FEATURE_WINCE   
04/12/13   sr      Support to disable F3 msgs by target team  
02/02/13   sg      Replaced __FILE__ with __FILENAME__
05/26/10   mad     Doxygenated
04/02/10   sg      Created Initial Version of File
04/13/10   vs      Moved msg_file from diag_msg_service.h
===========================================================================*/
/*!
@name DIAG_MSG_DISABLE_LVL_MASK 
@ingroup diag_message_service
This mask turns off msg levels from 1 to 32. 
@value if this is set to 0, all levels are allowed 
if this is set to 0xFFFFFFFF, then all the levels are disabled and 
none of the F3 msgs will be reported  
*/
#ifndef DIAG_MSG_DISABLE_LVL_MASK
#define DIAG_MSG_DISABLE_LVL_MASK 0
#endif

/*!
@name MSG_MASK definitions for MSG 2.0 macros
@ingroup diag_message_service
These are the masks that are used to identify a message as belonging to
group "i" of a particular Subsystem. This allows the user to selectively
turn groups of messages within a Subsystem to ON or OFF. The legacy
messages will continue to be supported through the legacy masks, MSG_LEGACY_*.
*/
/*@{*/ /* start group MSG_MASK definitions for MSG 2.0 macros */
#define MSG_MASK_0   (0x00000001)
#define MSG_MASK_1   (0x00000002)
#define MSG_MASK_2   (0x00000004)
#define MSG_MASK_3   (0x00000008)
#define MSG_MASK_4   (0x00000010)
#define MSG_MASK_5   (0x00000020)
#define MSG_MASK_6   (0x00000040)
#define MSG_MASK_7   (0x00000080)
#define MSG_MASK_8   (0x00000100)
#define MSG_MASK_9   (0x00000200)
#define MSG_MASK_10  (0x00000400)
#define MSG_MASK_11  (0x00000800)
#define MSG_MASK_12  (0x00001000)
#define MSG_MASK_13  (0x00002000)
#define MSG_MASK_14  (0x00004000)
#define MSG_MASK_15  (0x00008000)
#define MSG_MASK_16  (0x00010000)
#define MSG_MASK_17  (0x00020000)
#define MSG_MASK_18  (0x00040000)
#define MSG_MASK_19  (0x00080000)
#define MSG_MASK_20  (0x00100000)
#define MSG_MASK_21  (0x00200000)
#define MSG_MASK_22  (0x00400000)
#define MSG_MASK_23  (0x00800000)
#define MSG_MASK_24  (0x01000000)
#define MSG_MASK_25  (0x02000000)
#define MSG_MASK_26  (0x04000000)
#define MSG_MASK_27  (0x08000000)
#define MSG_MASK_28  (0x10000000)
#define MSG_MASK_29  (0x20000000)
#define MSG_MASK_30  (0x40000000)
#define MSG_MASK_31  (0x80000000)
/*@}*/ /* end group MSG_MASK definitions for MSG 2.0 macros */

/*!
@name MSG_MASK definitions for MSG 2.0 macros
@ingroup diag_message_service
These are the masks that are used to identify a message as belonging to
group "i" of a particular Subsystem. This allows the user to selectively
turn groups of messages within a Subsystem to ON or OFF. The legacy
messages will continue to be supported through the legacy masks, MSG_LEGACY_*.
*/
/*@{*/ /* start group MSG_MASK definitions for MSG 2.0 macros */

#define MSG_INT_MASK_0   ((0x00000001) & (~DIAG_MSG_DISABLE_LVL_MASK))
#define MSG_INT_MASK_1   ((0x00000002) & (~DIAG_MSG_DISABLE_LVL_MASK))
#define MSG_INT_MASK_2   ((0x00000004) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_3   ((0x00000008) & (~DIAG_MSG_DISABLE_LVL_MASK))
#define MSG_INT_MASK_4   ((0x00000010) & (~DIAG_MSG_DISABLE_LVL_MASK))
#define MSG_INT_MASK_5   ((0x00000020) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_6   ((0x00000040) & (~DIAG_MSG_DISABLE_LVL_MASK))
#define MSG_INT_MASK_7   ((0x00000080) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_8   ((0x00000100) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_9   ((0x00000200) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_10  ((0x00000400) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_11  ((0x00000800) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_12  ((0x00001000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_13  ((0x00002000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_14  ((0x00004000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_15  ((0x00008000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_16  ((0x00010000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_17  ((0x00020000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_18  ((0x00040000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_19  ((0x00080000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_20  ((0x00100000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_21  ((0x00200000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_22  ((0x00400000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_23  ((0x00800000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_24  ((0x01000000) & (~DIAG_MSG_DISABLE_LVL_MASK))
#define MSG_INT_MASK_25  ((0x02000000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_26  ((0x04000000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_27  ((0x08000000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_28  ((0x10000000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_29  ((0x20000000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_30  ((0x40000000) & (~DIAG_MSG_DISABLE_LVL_MASK)) 
#define MSG_INT_MASK_31  ((0x80000000) & (~DIAG_MSG_DISABLE_LVL_MASK))

/*!
@name MSG_MASK definitions for legacy MSG macros
@ingroup diag_message_service
These masks are to be used for support of all legacy messages in the sw.
The user does not need to remember the names as they will be embedded in 
the appropriate macros.
*/
/*@{*/ /* start group MSG_MASK definitions for legacy MSG macros */
/*!
  If the message mask is MSG_LEGACY_LOW client would only see the message if 
  run time mask for low  is turned on
*/
#define MSG_LEGACY_LOW      MSG_MASK_0

/*!
  If the message mask is MSG_LEGACY_MED client would only see the message if 
  run time mask for med  is turned on for the subsystem id 
*/
#define MSG_LEGACY_MED      MSG_MASK_1

/*!
  If the message mask is MSG_LEGACY_HIGH client would only see the message if 
  run time mask for high  is turned on for the subsystem id 
*/
#define MSG_LEGACY_HIGH     MSG_MASK_2

/*!
  If the message mask is MSG_LEGACY_ERROR client would only see the message if 
  run time mask for error is turned on for the subsystem id 
*/
#define MSG_LEGACY_ERROR    MSG_MASK_3

/*!
  If the message mask is MSG_LEGACY_FATAL client would only see the message if 
  run time mask for fatal is turned on for the subsystem id 
*/
#define MSG_LEGACY_FATAL    MSG_MASK_4
/*@}*/ /* end group MSG_MASK definitions for legacy MSG macros */


/*!
@name Mask settings for build-time masks for Diag Debug message macros
@ingroup diag_message_service
MSG_LVL_FATAL , MSG_LVL_ERROR , MSG_LVL_HIGH, MSG_LVL_MED , MSG_LVL_LOW 
masks are to be used only as build time masks, as used in file msgtgt.h.
*/
/*@{*/ /* start group Mask settings for build-time masks for Diag Debug message macros */
/*!
  If the build mask for a sub-system is MSG_LVL_FATAL, the messages for
  that sub-system will be compiled in only if fatal is used as mask in the 
  macro.
*/
#define MSG_LVL_FATAL   (MSG_INT_MASK_4)

/*!
  If the build mask for a sub-system is MSG_LVL_ERROR, the messages for
  that sub-system will be compiled in only if error or above is used as 
  mask in the macro.
*/
#define MSG_LVL_ERROR   (MSG_INT_MASK_3 | MSG_LVL_FATAL) 

/*!
  If the build mask for a sub-system is MSG_LVL_HIGH, the messages for
  that sub-system will be compiled in only if high or above is used as 
  mask in the macro.
*/
#define MSG_LVL_HIGH    (MSG_INT_MASK_2 | MSG_LVL_ERROR)

/*!
  If the build mask for a sub-system is MSG_LVL_MED, the messages for
  that sub-system will be compiled in only if medium or above is used as 
  mask in the macro.
*/
#define MSG_LVL_MED     (MSG_INT_MASK_1 | MSG_LVL_HIGH)

/*!
  If the build mask for a sub-system is MSG_LVL_LOW, the messages for
  that sub-system will be compiled in only if low or above is used as 
  mask in the macro.
*/
#define MSG_LVL_LOW     (MSG_INT_MASK_0 | MSG_LVL_MED)

/*!
  If the build mask for a sub-system is MSG_LVL_NONE, the messages for
  that sub-system will be compiled out from the build.
*/
#define MSG_LVL_NONE    0
/*@}*/ /* end group Mask settings for build-time masks for Diag Debug message macros */


/*!
@name FILE definition
@ingroup diag_message_service
   If MSG_FILE is defined, use that as the filename, and allocate a single
   character string to contain it.  Note that this string is shared with the
   Error Services, to conserve ROM and RAM.

   With ADS1.1 and later, multiple uses of __FILE__ or __MODULE__ within the
   same file do not cause multiple literal strings to be stored in ROM. So in
   builds that use the more recent versions of ADS, it is not necessary to
   define the static variable msg_file. Note that __MODULE__ gets replaced
   with the filename portion of the full pathname of the file. 
   @note
   If this symbol is activated, the file is being compiled with legacy
   message service defines.  Older compilers may need the static const
   variable to avoid multiple copies of the filename string in ROM. 
*/
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 110000)
/* Check for ARM version first.  Defining static const char msg_file[] will
   generate a compiler warning if the file doesn't actually use a message. */

#define msg_file __MODULE__

#elif defined (MSG_FILE)

static const char msg_file[] = MSG_FILE;
#elif defined (MY_GCC_FILE)

#define msg_file MY_GCC_FILE

#elif defined (__GNUC__) 
#define msg_file __FILENAME__

#else

static const char msg_file[] = __FILENAME__;

#endif

#endif /* MSG_MASK_H */



