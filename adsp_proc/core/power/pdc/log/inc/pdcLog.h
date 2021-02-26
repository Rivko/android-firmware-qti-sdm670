#ifndef _PDCINT_LOG_H_
#define _PDCINT_LOG_H_
/*============================================================================
  FILE:         pdcInt_log.h

  OVERVIEW:     This file declares the interface to the drivers logging system

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
==============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/log/inc/pdcLog.h#1 $
$DateTime: 2017/07/21 22:10:47 $
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                           GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/

/**
 * pdcLog_printf
 *
 * @brief Logs a debug message using the standard C printf function format
 * 
 * @param arg_count:  Number of parameters that are being logged.
 * @param format:     The format string for printing log.
 */
#ifdef ENABLE_PDC_LOG
void pdcLog_printf(int arg_count, const char *format, ...);
#else
#define pdcLog_printf(arg_count, format, ...)
#endif /* ENABLE_PDC_LOG */

#ifdef __cplusplus
}
#endif

#endif /* _PDCINT_LOG_H_ */

