/*========================================================================
Copyright (c) 2010-2011, 2013-2014, 2016 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/**
@file qurt_elite_diag.h

@brief This file contains a utility for generating diagnostic messages.
 This file defines macros for printing debug messages on the target or
 in simulation.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The weakgroup description below displays in the PDF.
===========================================================================*/

/** @weakgroup weakf_qurt_elite_diag_intro
The diagnostics utility includes the qurt_elite_hexdump() function, which is
used to generate diagnostic messages. The utility includes macros for
printing debug messages on a target or in simulation.
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/inc/qurt_elite_diag.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8.
01/08/16   sw       (Tech Pubs) General edits in Doxygen comments.
09/05/14   sw       (Tech Pubs) Merged Doxygen comments from 2.4.
03/25/13   sw       (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo      (Tech Pubs) Edited doxygen comments and markup.
02/04/10   mwc      Created file.
03/09/10   brs      Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_DIAG_H
#define QURT_ELITE_DIAG_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "mmdefs.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


//***************************************************************************
// Debug message features
//***************************************************************************
// change the following to be variables for runtime (TBD)
// hence defined in lower case

/** @addtogroup qurt_elite_diag
@{ */

/** Enable (1) or disable (0) the debug message. */
#define qurt_elite_debugmsg_enable    1




#include "msg.h"
#include "msgcfg.h"

/* TODO: request our own ssids by emailing asw.diag.request
   currently all ssid are mapped to MSG_SSID_QDSP6
 */
// common priorities

/** @name Legacy debug priority messages
@{ */

#define DBG_LOW_PRIO   MSG_LEGACY_LOW    /**< Low priority debug message. */
#define DBG_MED_PRIO   MSG_LEGACY_MED    /**< Medium priority debug message. */
#define DBG_HIGH_PRIO  MSG_LEGACY_HIGH   /**< High priority debug message. */
#define DBG_ERROR_PRIO MSG_LEGACY_ERROR  /**< Error priority debug message. */
#define DBG_FATAL_PRIO MSG_LEGACY_FATAL  /**< Fatal priority debug message. */

/** @} */ /* end_namegroup Legacy debug priority messages */

#if defined(SIM)

/** Variable for setting the lowest priority debug message. */
/* set using adsp_file_logging.cfg. variable is defined & inited in EliteLoggingUtilsForSim.cpp.*/
extern uint8_t qurt_elite_debugmsg_lowest_prio;

/*=====================================================*/
/* Original implementation cut&paste from AudCmnUtil.h */
/*=====================================================*/

/*
DBG_MSG for debugging. Currently compile time as we are supposed
to have a single build. Prints the file and line number and the
message. Message is printf format and takes a variable number of
arguments.
*/
#include "hexagon_sim_timer.h"

#undef  MSG_SSID_QDSP6
#undef MSG_SSID_APR_ADSP
#define MSG_SSID_QDSP6  0
#define MSG_SSID_APR_ADSP  1

//if __FILENAME macro is not available, just leave that field blank
#ifndef __FILENAME__
#define __FILENAME__ ""
#endif

// assert is for priority == MSG_LEGACY_FATAL (i.e on FATAL)
// disable warning "conditional expression is constant" for qurt_elite_debugmsg_enable
#if (__QDSP6_ARCH__ <= 3)
#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (qurt_elite_debugmsg_enable && (priority >= qurt_elite_debugmsg_lowest_prio)) \
    { \
      uint64_t cycles = hexagon_sim_read_pcycles(); \
      int tms = ((cycles >> 10)*1790)>>20; \
      int tsec = ((cycles >> 10)*1833)>>30; \
      int tmin = ((cycles >> 10)*31)>>30; \
      printf("%dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
      if (MSG_LEGACY_FATAL == priority) QURT_ELITE_ASSERT(0);\
    } \
  } while(0) \

#elif (__QDSP6_ARCH__>=4)

#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (qurt_elite_debugmsg_enable && (priority >= qurt_elite_debugmsg_lowest_prio)) \
    { \
      uint64_t cycles = hexagon_sim_read_pcycles(); \
      int tms = ((cycles >> 10)*2148)>>20; \
      int tsec = ((cycles >> 10)*2200)>>30; \
      int tmin = ((cycles >> 10)*37)>>30; \
      printf("%dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
      if (MSG_LEGACY_FATAL == priority) QURT_ELITE_ASSERT(0);\
    } \
  } while(0) \

#else
#error "Unknown Q6 Arch Version"
#endif
#undef MSG
#undef MSG_1
#undef MSG_2
#undef MSG_3
#undef MSG_4
#undef MSG_5
#undef MSG_6
#undef MSG_7
#undef MSG_8
#undef MSG_9
#undef MSG_FATAL
#define MSG(xx_ss_id, xx_ss_mask, xx_fmt)                       DBG_MSG(0,xx_ss_mask,xx_fmt)
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1)            DBG_MSG(0,xx_ss_mask,xx_fmt,xx_arg1)
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)   DBG_MSG(0,xx_ss_mask,xx_fmt,xx_arg1,xx_arg2)
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8)


#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
    DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)


#define MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_QDSP6, DBG_FATAL_PRIO, x_fmt, a, b, c)

#undef MSG_SPRINTF
#undef MSG_SPRINTF_1
#undef MSG_SPRINTF_2
#undef MSG_SPRINTF_3
#undef MSG_SPRINTF_4
#undef MSG_SPRINTF_5
#undef MSG_SPRINTF_6
#undef MSG_SPRINTF_7
#undef MSG_SPRINTF_8
#undef MSG_SPRINTF_9

#define MSG_SPRINTF MSG
#define MSG_SPRINTF_1 MSG_1
#define MSG_SPRINTF_2 MSG_2
#define MSG_SPRINTF_3 MSG_3
#define MSG_SPRINTF_4 MSG_4
#define MSG_SPRINTF_5 MSG_5
#define MSG_SPRINTF_6 MSG_6
#define MSG_SPRINTF_7 MSG_7
#define MSG_SPRINTF_8 MSG_8
#define MSG_SPRINTF_9 MSG_9

#endif /* SIMULATION */

/* Original macro cut&paste from AudCmnUtil.h, leave it outside SURF for kernel debugging */

/** Macro that prints to the QuRT buffer for debugging purposes. */
#define PRINT()   printf("%s %d\n",__FILENAME__, __LINE__);

#if defined DEBUG || defined SIM
   /** QuRT_Elite version of assert. */
   #define QURT_ELITE_ASSERT(x) assert(x)
#else
   /** QuRT_Elite version of assert. */
   /*
   If the definition of QURT_ELITE_ASSERT for on target builds is set to blank, the compiler simply removes any code inside the QURT_ELITE_ASSERT macro.
   This may cause an unused variable error. We can fix this by doing:

   #define QURT_ELITE_ASSERT(x) (x)

   However, this will trigger another warning from the compiler saying that this statement does not have any effect. This warning can be suppressed by doing:

   #define QURT_ELITE_ASSERT(x) (void)(x)

   This works, but may cause the compiler to insert code that evaluates x in some cases. We want the compiler to not generate any code at all.
   The C++ standard states that contents of sizeof() are evaluated at compile time and never at run time. So we can use that property and do the following:

   #define QURT_ELITE_ASSERT(x) (void)sizeof(x)

   This eliminates the compiler warnings and ensures that no code is generated.
   */
   #define QURT_ELITE_ASSERT(x) (void)sizeof(x)
#endif

/** @} */ /* end_addtogroup qurt_elite_diag */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_DIAG_H


