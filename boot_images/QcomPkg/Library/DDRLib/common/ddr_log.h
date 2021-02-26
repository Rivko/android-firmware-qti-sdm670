#ifndef __DDR_LOG_H__
#define __DDR_LOG_H__

/**
 * @file ddr_log.h
 * @brief
 * Header file for DDR logging.
 */
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/18/15   elt     Converted ddr_log to macro that calls boot_profiler feature
04/11/14   sr      supported DDR logging for SNS DDR driver .
09/12/12   sl      Use macro-style DDR logging.
07/19/12   tw      Implemented qdss swevent wrapper around DDR logging.
03/02/12   tw      Initial version.
================================================================================
                      Copyright 2012 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/

#include <stdio.h>
#include <stdarg.h>
#include "ddr_profiler.h"

/*==============================================================================
                                  MACROS
==============================================================================*/

#define DDR_LOG_BUF_LEN_MAX		256

#ifdef BUILD_BOOT_CHAIN
  #include "boot_logger.h"
  #define DDR_LOG(...)
#else
  #include "swevent.h"

  #define DDR_LOG(level, event, ...)                                 \
            do                                                       \
            {                                                        \
              if ((level) >= ddr_log_level)                          \
              {                                                      \
                SWEVENT((tracer_event_id_t)(event), ##__VA_ARGS__);  \
              }                                                      \
            } while (0)
#endif


/*==============================================================================
                                  TYPES
==============================================================================*/
enum ddr_log_level
{
  DDR_ERROR, /* provided for flagging Error conditions in DDRSNS */
  DDR_NORMAL, /* provided for printing interesting/debug  information along with BEGIN/END of each DDRSNS function */
  DDR_DETAIL, /* provided for detailed logging information i.e; coarse/fine delay training & other training(CA/WRLRL/RCW) results with DDRSNS */
};

enum ddr_log_event
{
#ifdef BUILD_BOOT_CHAIN
  DDR_INIT,
  DDR_SELF_REFRESH_ENTRY_IDLE,
  DDR_SELF_REFRESH_ENTRY_ASAP,
  DDR_SELF_REFRESH_EXIT,
  DDR_DPD_ENTRY,
  DDR_DPD_EXIT,
  DDR_DMM,
  DDR_BIMC_POWER_COLLAPSE_ENTRY,
  DDR_BIMC_POWER_COLLAPSE_EXIT,
  DDR_PRE_CLK_SWITCH,
  DDR_POST_CLK_SWITCH,
  DDR_FATAL_ERROR,
  DDR_SITE_UPDATE_CALIBRATION,
#else
  DDR_INIT,
  DDR_SELF_REFRESH_ENTRY_IDLE = DDR_EVENT_BIMC_SR_ENTRY_IDLE,
  DDR_SELF_REFRESH_ENTRY_ASAP = DDR_EVENT_BIMC_SR_ENTRY_ASAP,
  DDR_SELF_REFRESH_EXIT = DDR_EVENT_BIMC_SR_EXIT,
  DDR_DPD_ENTRY = DDR_EVENT_BIMC_DPD_ENTRY,
  DDR_DPD_EXIT = DDR_EVENT_BIMC_DPD_EXIT,
  DDR_DMM = DDR_EVENT_BIMC_PASR_UPDATE,
  DDR_BIMC_POWER_COLLAPSE_ENTRY = DDR_EVENT_BIMC_PWR_COLLAPSE_ENTRY,
  DDR_BIMC_POWER_COLLAPSE_EXIT = DDR_EVENT_BIMC_PWR_COLLAPSE_EXIT,
  DDR_PRE_CLK_SWITCH = DDR_EVENT_BIMC_PRE_CLK_SWITCH,
  DDR_POST_CLK_SWITCH = DDR_EVENT_BIMC_POST_CLK_SWITCH,
  DDR_FATAL_ERROR = DDR_EVENT_DDR_DRIVER_ERROR_FATAL,
  DDR_SITE_UPDATE_CALIBRATION,
#endif
};

enum ddr_log_error
{
  DDR_UNSUPPORTED_DEVICE,
  DUAL_RANK_NOT_SELECTED,
  SITE_CALIBRATION_NOT_FOUND,
};


/*==============================================================================
                                  DATA
==============================================================================*/
extern enum ddr_log_level ddr_log_level;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

static inline void ddr_sprintf(char* ddr_sprintf_buf, size_t ddr_sprintf_buf_len, const char* fmt, ...)
{
#if defined( BUILD_DDI ) || defined( BUILD_BOOT_CHAIN ) || defined( COMPILING_TINY_AXF )
    va_list vargs;
    va_start(vargs, fmt);
    vsnprintf(ddr_sprintf_buf, ddr_sprintf_buf_len-1, fmt, vargs);
    va_end(vargs); 
#endif
} /* ddr_sprintf */

void ddr_logf(uint32 severity, const char* fmt, ...);

#define ddr_printf(severity, xxstring, ...)                               \
      do                                                                  \
      {                                                                   \
        boot_profiler_ddr_message((boot_profiler_log_level)severity, xxstring, ##__VA_ARGS__);     \
      } while (0)                                                  
#endif 

/* __DDR_LOG_H__ */
