////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef IS_DBG_UTIL_H
#define IS_DBG_UTIL_H

#include <stdio.h>
#include <inttypes.h>

#define IS_LOG_LEVEL_INFO      (5)
#define IS_LOG_LEVEL_LOW       (4)
#define IS_LOG_LEVEL_HIGH      (3)
#define IS_LOG_LEVEL_ERROR     (2)
#define IS_LOG_LEVEL_NONE      (1)


#ifndef IS_LOG_LEVEL
#define IS_LOG_LEVEL IS_LOG_LEVEL_ERROR
#endif /*IS_LOG_LEVEL */

#if defined(_ANDROID_)

#undef LOG_TAG
#define LOG_TAG "IS_ALGO"

#include <android/log.h>

/** ERROR debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_ERROR)
#define IS_ERR(...)    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define IS_ERR(...)  ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_ERROR) */

/** HIGH debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_HIGH)
#define IS_HIGH(...)    __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#else
#define IS_HIGH(...)  ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_HIGH) */

/** LOW debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_LOW)
#define IS_LOW(...)    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)
#else
#define IS_LOW(...)  ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_LOW) */

/** INFO debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_INFO)
#define IS_INFO( ...)    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, ##__VA_ARGS__)
#else
#define IS_INFO(...)   ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_INFO) */

#else

#define LOG_LEVEL_INFO_LABLE      "[INFO]"
#define LOG_LEVEL_LOW_LABLE       "[LOW] "
#define LOG_LEVEL_HIGH_LABLE      "[HIGH]"
#define LOG_LEVEL_ERROR_LABLE     "[ERR] "

/** printf format label */
#define IS_FPRINTF_MSG(_file,_label,_fmt,...)               \
do {                                                        \
    fprintf(_file,"%s",_label);                             \
    fprintf(_file,"%s(%d): ", __FUNCTION__, __LINE__);      \
    fprintf(_file,_fmt, ##__VA_ARGS__);                     \
    fprintf(_file,"\n");                                    \
} while(0,0)

/** ERROR debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_ERROR)
#define IS_ERR(fmt, ...)    IS_FPRINTF_MSG(stderr, LOG_LEVEL_ERROR_LABLE, fmt, ##__VA_ARGS__)
#else
#define IS_ERR(fmt, ...)  ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_ERROR) */

/** HIGH debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_HIGH)
#define IS_HIGH(fmt, ...)    IS_FPRINTF_MSG(stderr, LOG_LEVEL_HIGH_LABLE, fmt, ##__VA_ARGS__)
#else
#define IS_HIGH(fmt, ...)  ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_HIGH) */

/** LOW debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_LOW)
#define IS_LOW(fmt, ...)    IS_FPRINTF_MSG(stderr, LOG_LEVEL_LOW_LABLE, fmt, ##__VA_ARGS__)
#else
#define IS_LOW(fmt, ...)  ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_LOW) */

/** INFO debug macro */
#if (IS_LOG_LEVEL >= IS_LOG_LEVEL_INFO)
#define IS_INFO(fmt, ...)    IS_FPRINTF_MSG(stderr, LOG_LEVEL_INFO_LABLE, fmt, ##__VA_ARGS__)
#else
#define IS_INFO(fmt, ...)   ((void)(0))
#endif /* (IS_LOG_LEVEL >= IS_LOG_LEVEL_INFO) */

#endif /* _ANDROID_ */

#ifndef  IS_COMPILE_TIME_ASSERT
#define _IS_PASTE_STR(a,b) a##b
#define _IS_COMPILE_TIME_ASSERT(expr, id)   typedef char _IS_PASTE_STR(CASSERT_FAILED_, id)[(expr)?1:-1]
#define IS_COMPILE_TIME_ASSERT(expr) _IS_COMPILE_TIME_ASSERT(expr, __LINE__)
#endif /* !IS_COMPILE_TIME_ASSERT */

#ifndef IS_ASSERT
#include  <assert.h>
#if !defined(_ANDROID_)
#define IS_ASSERT(c)    { if (!(c)) {IS_ERR("Assertion failed: %s, file %s, line %d\n", #c, __FILE__, __LINE__ ); __debugbreak();  } }
#else
#ifndef NDEBUG
#define IS_ASSERT(c)    { if (!(c)) {IS_ERR("Assertion failed: %s, file %s, line %d\n", #c, __FILE__, __LINE__ ); assert(false);} }
#else
#define IS_ASSERT(c)    { assert(c); }
#endif /* NDEBUG */
#endif /* !_ANDROID_ */
#endif /* IS_ASSERT */


/*********************************************************************************************************************/
/*                        DEBUG API                                                                                  */
/*********************************************************************************************************************/
#include "is_interface.h"

void eis2_debug_set_output(uint32_t frameIdx, is_output_type *is_output);
void eis3_debug_set_output(uint32_t frameIdx, is_output_type *is_output);

void eis_debug_print_init_info(
    const char* file_name,
    const is_init_data_common* init_common,
    const is_init_data_sensor *init_sensors,
    uint32_t num_sensors);

void eis2_debug_print_process_input(
    const char* file_name,
    const is_input_t *is_input);

void eis_debug_print_process_output(
    const char* file_name,
    const is_output_type *is_output);

void eis3_debug_print_process_input(
    const char* file_name,
    const is_input_t *is_input);

#endif /* !IS_DBG_UTIL_H */

