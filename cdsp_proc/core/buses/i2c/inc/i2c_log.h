/** 
    @file  i2c_log.h
    @brief I2C logging interface
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_LOG_H__
#define __I2C_LOG_H__

// limit arguments to 10
#define I2C_LOG_NUM_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) (N - 1)
#define I2C_LOG(level, msg, args...) I2C_LOG_##level(msg, ##args)

#ifdef I2C_LOG_ENABLE_DATA
#define I2C_LOG_LEVEL_DATA(...) i2c_log(I2C_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define I2C_LOG_LEVEL_DATA(...)
#endif

#ifdef I2C_LOG_ENABLE_ERROR
#define I2C_LOG_LEVEL_ERROR(...) i2c_log(I2C_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define I2C_LOG_LEVEL_ERROR(...)
#endif

#ifdef I2C_LOG_ENABLE_INFO
#define I2C_LOG_LEVEL_INFO(...) i2c_log(I2C_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define I2C_LOG_LEVEL_INFO(...)
#endif

#ifdef I2C_LOG_ENABLE_REGS
#define I2C_LOG_LEVEL_REGS(...) i2c_log(I2C_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define I2C_LOG_LEVEL_REGS(...)
#endif

#ifdef I2C_LOG_ENABLE_PERF
#define I2C_LOG_LEVEL_PERF(...)
#else
#define I2C_LOG_LEVEL_PERF(...)
#endif

#ifdef I2C_LOG_ENABLE_VERBOSE
#define I2C_LOG_LEVEL_VERBOSE(...) i2c_log(I2C_LOG_NUM_ARGS(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1), __VA_ARGS__)
#else
#define I2C_LOG_LEVEL_VERBOSE(...)
#endif

boolean i2c_log_init(void);
void i2c_log (uint32 data_count, const char *format_str, ...);

#endif
