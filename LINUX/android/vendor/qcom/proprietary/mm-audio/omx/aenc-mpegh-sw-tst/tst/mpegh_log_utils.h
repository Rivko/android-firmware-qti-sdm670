/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   mpegh_log_utils.h           TYPE: C-header file              *]
[*                                                                           *]
[* DESCRIPTION: Utilities related to logging                                 *]
[*****************************************************************************/

#ifndef _MPEGH_DECODER_LOG_UTILS_H_
#define _MPEGH_DECODER_LOG_UTILS_H_
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#define MPEGH_LOG_DISPLAY_FILE_AND_LINE
#define MPEGH_LOG_DISPLAY_LOG_LEVEL_STR

/* LOG "LEVELS" */
#ifndef INFO_LOG_ENABLED
#define INFO_LOG_ENABLED 0
#endif
#ifndef ERROR_LOG_ENABLED
#define ERROR_LOG_ENABLED 0
#endif
#ifndef WARN_LOG_ENABLED
#define WARN_LOG_ENABLED 0
#endif

#ifndef DEBUG_LOG_ENABLED
#define DEBUG_LOG_ENABLED 0
#endif
#ifndef VERBOSE_LOG_ENABLED
#define VERBOSE_LOG_ENABLED 0
#endif

#ifndef VERY_VERBOSE_LOG_ENABLED
#define VERY_VERBOSE_LOG_ENABLED 0
#endif

#define UTILS_LOG_ENABLED 1

#ifndef HEX_DUMP_ENABLED
#define HEX_DUMP_ENABLED 0
#endif

#ifndef DEBUG_HEX_DUMP_BITBUFFER
#define DEBUG_HEX_DUMP_BITBUFFER 0
#endif

#define __LINE_NUMBER__ __LINE__



#ifdef __GNUC__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#elif WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#undef MPEGH_LOG_DISPLAY_FILE_AND_LINE
#endif

#ifdef MPEGH_LOG_DISPLAY_FILE_AND_LINE
#define LOCATION_FORMAT "%s::%d:\t"
#else
#define __FILENAME__ ("\0")
#undef __LINE_NUMBER__
#define __LINE_NUMBER__ ("\0")
#define LOCATION_FORMAT "%s%s"
#endif

#ifdef MPEGH_LOG_DISPLAY_LOG_LEVEL_STR
#define _VBS "VBS  "
#define _VVB "VVB  "
#define _DBG "DBG  "
#define _INF "INF  "
#define _WRN "WRN  "
#define _ERR "ERR  "
#define _UTILS "UTILS"
#else
#define _VBS ""
#define _VVB ""
#define _DBG ""
#define _INF ""
#define _WRN ""
#define _ERR ""
#define _UTILS ""
#endif
/* NOTE: NEED C99 compiler. */
/* NOTE: The compiler will ALWAYS see this code. The reason for this is
 * is for the log messages not to loose sync with the code.
 */

#define _MPEGH_LOG_IMPL(is_enabled, log_level_str, fmt, ...) \
      do{ if(is_enabled) {\
            const char *_p_filename = __FILENAME__;\
            if(_p_filename != NULL) {\
                  fprintf(stderr, log_level_str LOCATION_FORMAT fmt "\n", _p_filename, __LINE_NUMBER__, ## __VA_ARGS__);\
           }else {\
                  fprintf(stderr, log_level_str fmt "\n",  ## __VA_ARGS__);\
          }\
      }\
      } while (0)

 #define MPEGH_LOG_VERY_VERBOSE(fmt, ...) \
             do { _MPEGH_LOG_IMPL(VERY_VERBOSE_LOG_ENABLED, _VVB, fmt, ## __VA_ARGS__); } while (0)

 #define MPEGH_LOG_VERBOSE(fmt, ...) \
             do { _MPEGH_LOG_IMPL(VERBOSE_LOG_ENABLED, _VBS, fmt, ## __VA_ARGS__); } while (0)


#define MPEGH_LOG_DEBUG(fmt, ...) \
            do { _MPEGH_LOG_IMPL(DEBUG_LOG_ENABLED, _DBG, fmt, ## __VA_ARGS__); } while (0)


#define MPEGH_LOG_INFO(fmt, ...) \
do { _MPEGH_LOG_IMPL(INFO_LOG_ENABLED, _INF, fmt, ## __VA_ARGS__); } while (0)


#define MPEGH_LOG_WARN(fmt, ...) \
do { _MPEGH_LOG_IMPL(WARN_LOG_ENABLED, _WRN, fmt, ## __VA_ARGS__); } while (0)


#define MPEGH_LOG_ERROR(fmt, ...) \
do { _MPEGH_LOG_IMPL(ERROR_LOG_ENABLED, _ERR, fmt, ## __VA_ARGS__); } while (0)


#define MPEGH_RETURN_WITH_ERROR_LOG(ret_val, fmt, ...) \
                        do { MPEGH_LOG_ERROR(fmt, ##__VA_ARGS__); \
                          return ret_val; } while (0)

#define MPEGH_LOG_UTILS(fmt, ...) \
      do { if (UTILS_LOG_ENABLED) fprintf(stderr, "[UTILS] "fmt "\n", ## __VA_ARGS__); } while (0)

#ifndef HEX_DUMP_ENABLED
  void mpegh_hex_dump(const char *tag, uint8_t* data, uint64_t buffer_size, uint64_t buffer_offset, uint64_t dump_size) {}
#else
  void mpegh_hex_dump(const char *tag, uint8_t* data, uint64_t buffer_size, uint64_t buffer_offset, uint64_t dump_size);
#endif

#define MPEGH_HEX_DUMP(tag, data, buffer_size, buffer_offset, dump_size) \
                        do { if(HEX_DUMP_ENABLED){ \
                              mpegh_hex_dump(tag, data, buffer_size, buffer_offset, dump_size);\
                            }\
                        }while(0)





#endif
