#ifndef __PDC_TARGET_H__
#define __PDC_TARGET_H__
/*============================================================================
  @file PdcTarget.h

                Copyright (c) 2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Library/QcomLib.h>
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALFramework.h"
#include "comdef.h"
#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define PDC_ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define PDC_DISABLE_INIT 0x1

#define PDC_IS_MODE_SET(_x, _m) (((_x) & (_m))?TRUE:FALSE)
#define PDC_IS_IDX_SET(_x, _idx) (((_x) & (0x1<<_idx))?TRUE:FALSE)

/* Logging */
#define LOG_LVL_CRIT 0
#define LOG_LVL_ERR  1
#define LOG_LVL_INFO 2
#define LOG_LVL_DBG  3
#define PDC_LOG_VAR  log_lvl

#define PDC_GET_COUNT_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,COUNT,...) COUNT
//#define PDC_DO_ULOG(...) ULogFront_RealTimePrintf(PDC_ULOG_HANDLE, PDC_GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0), __VA_ARGS__)

#define PDC_DO_ULOG(...) \
  do{ \
  ULogFront_RealTimePrintf_Ex(PDC_ULOG_HANDLE, PDC_GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0), \
                              (0x1 << (PDC_GET_COUNT_MACRO(__VA_ARGS__,10,9,8,7,6,5,4,3,2,1,0)))-1, \
                              __VA_ARGS__); \
  } while (0);

#define PDC_LOG(_lvl, _f, args...) \
  do {\
    if (_lvl <= PDC_LOG_VAR)\
    {\
      if (PDC_ULOG_INIT_STATE)\
      {\
        PDC_DO_ULOG(_f, ##args);\
      }\
    }\
  }while (0);

#define PDC_LOG_CRIT(_f, args...) PDC_LOG(LOG_LVL_CRIT, "Crit: "_f, ##args)
#define PDC_LOG_ERR(_f, args...) PDC_LOG(LOG_LVL_ERR, "Err: "_f, ##args)
#define PDC_LOG_INFO(_f, args...) PDC_LOG(LOG_LVL_INFO, "Info: " _f, ##args)
#define PDC_LOG_DBG(_f, args...) PDC_LOG(LOG_LVL_DBG, "Dbg: "_f, ##args)

/* Error Codes */
#define PDC_FAILED                1
#define PDC_SUCCESS               0

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define HWIO_INF_POLL_STATUS(_reg_, _field_, _status_, _poll_cnt_) \
  do                                                      \
  {                                                       \
    uint32 __cnt__ = 0;                                       \
    busywait(1);                                          \
    if (__cnt__++ > (_poll_cnt_))                         \
    {                                                     \
      ULOG_RT_PRINTF_1(PDC_ULOG_HANDLE, "%s: " #_reg_ " " #_field_, __func__);  \
      break;                                              \
    }                                                     \
  } while (HWIO_INF(_reg_, _field_) != _status_);

#define HWIO_INFI_POLL_STATUS(_reg_, _idx_, _field_, _status_, _poll_cnt_) \
  do                                                      \
  {                                                       \
    uint32 __cnt__ = 0;                                       \
    busywait(1);                                          \
    if (__cnt__++ > (_poll_cnt_))                         \
    {                                                     \
      ULOG_RT_PRINTF_1(PDC_ULOG_HANDLE, "%s: " #_reg_ " " #_field_, __func__);  \
      break;                                              \
    }                                                     \
  } while (HWIO_INFI(_reg_, _idx_, _field_) != _status_);

#define HWIO_INXF_POLL_STATUS(_base_, _reg_, _field_, _status_, _poll_cnt_) \
  do                                                      \
  {                                                       \
    uint32 __cnt__ = 0;                                       \
    busywait(1);                                          \
    if (__cnt__++ > (_poll_cnt_))                         \
    {                                                     \
      ULOG_RT_PRINTF_1(PDC_ULOG_HANDLE, "%s: " #_reg_ " " #_field_, __func__);  \
      break;                                              \
    }                                                     \
  } while (HWIO_INXF(_base_, _reg_, _field_) != _status_);

#define HWIO_INXFI_POLL_STATUS(_base_, _reg_, _idx_, _field_, _status_, _poll_cnt_) \
  do                                                      \
  {                                                       \
    uint32 __cnt__ = 0;                                       \
    busywait(1);                                          \
    if (__cnt__++ > (_poll_cnt_))                         \
    {                                                     \
      ULOG_RT_PRINTF_1(PDC_ULOG_HANDLE, "%s: " #_reg_ " " #_field_, __func__);  \
      break;                                              \
    }                                                     \
  } while (HWIO_INXFI(_base_, _reg_, _idx_, _field_) != _status_);

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Extern Variable Definitions
 * -------------------------------------------------------------------------*/
/* Logging */
extern ULogHandle  PDC_ULOG_HANDLE;
extern boolean     PDC_ULOG_INIT_STATE;
extern volatile uint32 PDC_LOG_VAR;

/* Debug */
extern volatile uint32 uDebugMode;

#endif /* #ifndef __PDC_TARGET_H__ */
