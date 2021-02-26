#ifndef HAP_DIAG_H
#define HAP_DIAG_H
/*==============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/
#include "HAP_farf.h"

#if !defined(SIM)
// Map diag mask to HAP_mask
#ifndef MSG_LEGACY_LOW
#define MSG_LEGACY_LOW      0x01
#endif
#ifndef MSG_LEGACY_MED
#define MSG_LEGACY_MED      0x02
#endif
#ifndef MSG_LEGACY_HIGH
#define MSG_LEGACY_HIGH     0x04
#endif
#ifndef MSG_LEGACY_ERROR
#define MSG_LEGACY_ERROR    0x08
#endif
#ifndef MSG_LEGACY_FATAL
#define MSG_LEGACY_FATAL    0x10
#endif

//map elite to Diag mask
#define DBG_LOW_PRIO   MSG_LEGACY_LOW    /**< Low priority debug message. */
#define DBG_MED_PRIO   MSG_LEGACY_MED    /**< Medium priority debug message. */
#define DBG_HIGH_PRIO  MSG_LEGACY_HIGH   /**< High priority debug message. */
#define DBG_ERROR_PRIO MSG_LEGACY_ERROR  /**< Error priority debug message. */
#define DBG_FATAL_PRIO MSG_LEGACY_FATAL  /**< Fatal priority debug message. */

static inline uint16 diag_to_farf_mask_map(uint16 xx_ss_mask) {
    switch(xx_ss_mask) {
        case MSG_LEGACY_LOW:
            return HAP_LEVEL_LOW;
        case MSG_LEGACY_MED:
            return HAP_LEVEL_MEDIUM;
        case MSG_LEGACY_HIGH:
            return HAP_LEVEL_HIGH;
        case MSG_LEGACY_ERROR:
            return HAP_LEVEL_ERROR;
        case MSG_LEGACY_FATAL:
            return HAP_LEVEL_FATAL;
        default:
            return HAP_LEVEL_ERROR;
    }
}

#define MSG_FARF(xx_ss_mask, xx_fmt...) \
    do { \
        if(0 == (HAP_debug_v2)) { \
            _HAP_debug_v2( \
                diag_to_farf_mask_map(xx_ss_mask), \
                __FILENAME__, \
                __LINE__, \
                xx_fmt); \
        } else { \
            HAP_debug_v2( \
                diag_to_farf_mask_map(xx_ss_mask), \
                __FILENAME__, \
                __LINE__, \
                xx_fmt); \
        } \
    } while (0)

// MSG APIs
#define MSG(xx_ss_id, xx_ss_mask, xx_fmt) \
                    MSG_FARF(xx_ss_mask,xx_fmt)
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt,...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)

// MSG_sprintf APIs
#define MSG_SPRINTF_1(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_2(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_3(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_4(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_5(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_6(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_7(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)
#define MSG_SPRINTF_8(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)

#define MSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
                    MSG_FARF(xx_ss_mask,xx_fmt,##__VA_ARGS__)

#else /** SIM */
/*=====================================================*/
/* Original implementation copy&paste from qurt_elite_diag.h 
   However QURT_ELITE_ASSERT part is not copied. */
/*=====================================================*/

/** Enable or disable the debug message. */
#define shared_lib_api_debugmsg_enable    1

/* min loggig level. variable is defined & inited in EliteLoggingUtilsForSim.cpp.*/
extern uint8_t shared_lib_api_debugmsg_lowest_prio;

/*
DBG_MSG for debugging. Currently compile time as we are supposed
to have a single build. Prints the file and line number and the
message. Message is printf format and takes a variable number of
arguments.
*/
#include "hexagon_sim_timer.h"
/** @name Common Debug Priorities
@{ */

#define DBG_FATAL_PRIO 16
#define DBG_ERROR_PRIO 8
#define DBG_HIGH_PRIO  4
#define DBG_MED_PRIO   2
#define DBG_LOW_PRIO   1

/** @} */ /* end_namegroup Common Debug Priorities */
#undef  MSG_SSID_QDSP6
#undef MSG_SSID_APR_ADSP
#define MSG_SSID_QDSP6  0
#define MSG_SSID_APR_ADSP  1

//if __FILENAME macro is not available, just leave that field blank
#ifndef __FILENAME__
#define __FILENAME__ ""
#endif

// assert is for priority == 0 (i.e on ERROR)
// disable warning "conditional expression is constant" for shared_lib_api_debugmsg_enable
#if (__QDSP6_ARCH__ <= 3)
#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (shared_lib_api_debugmsg_enable && (priority >= shared_lib_api_debugmsg_lowest_prio)) \
    { \
      uint64_t cycles = hexagon_sim_read_pcycles(); \
      int tms = ((cycles >> 10)*1790)>>20; \
      int tsec = ((cycles >> 10)*1833)>>30; \
      int tmin = ((cycles >> 10)*31)>>30; \
      printf("%dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
    } \
  } while(0) \

#elif (__QDSP6_ARCH__>=4)

#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (shared_lib_api_debugmsg_enable && (priority >= shared_lib_api_debugmsg_lowest_prio)) \
    { \
      uint64_t cycles = hexagon_sim_read_pcycles(); \
      int tms = ((cycles >> 10)*2148)>>20; \
      int tsec = ((cycles >> 10)*2200)>>30; \
      int tmin = ((cycles >> 10)*37)>>30; \
      printf("%dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
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

#endif /** SIM */            
         
#endif // HAP_DIAG_H


