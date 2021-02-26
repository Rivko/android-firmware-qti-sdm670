/*
===========================================================================

FILE:         ddr_log.c

DESCRIPTION:
  Information for ddr_logging

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
04/07/15   plc     Adding support for boot profiler
06/17/14   rp      Initial revision.

===========================================================================
              Copyright 2014-2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                    INCLUDES
==============================================================================*/

#include <stdio.h>
#include <stdarg.h>
#include "ddr_log.h"
#include "ddr_profiler.h"

/*==============================================================================
                                  DATA
==============================================================================*/

#ifdef BUILD_BOOT_CHAIN
static char ddr_printf_buf[DDR_LOG_BUF_LEN_MAX]={0};
#endif

/*==============================================================================
                                   FUNCTIONS
==============================================================================*/


/* ============================================================================
**  Function : ddr_logf
** ============================================================================
*/
/*!
*   @brief
*
*
*   @details
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_logf(uint32 severity, const char* fmt, ...)
{
    va_list vargs;
    /* by default SW only logs Error Conditions with DDRSNS , one need to change this condition accordingly to get more detailed logging information as per the needs*/
    #if ((defined(FEATURE_BOOT_DDR_DEBUG_IMAGE)) || (defined(DDR_PROFILER_EN_NORM_LVL)))
    if(severity > DDR_NORMAL) {
    #else
    if(severity >= DDR_NORMAL) {
    #endif
	return;
    }
#ifdef BUILD_BOOT_CHAIN
    va_start(vargs, fmt);
    vsnprintf(ddr_printf_buf, DDR_LOG_BUF_LEN_MAX-1, fmt, vargs);
    va_end(vargs); 
	boot_log_message(ddr_printf_buf);
#else
    #ifdef SVE_PRINTF_ENABLED
      va_start(vargs, fmt);
      vprintf(fmt, vargs);
      va_end(vargs);
    #endif
#endif
} /* ddr_logf */