#ifndef _SLEEP_H_
#define _SLEEP_H_
/*==============================================================================
  FILE:         sleep.h

  OVERVIEW:     This file contains public APIs related to Sleep task.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/api/power/sleep2.0/sleep.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/*==============================================================================
                            GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/ 
/**
 * sleepOS_isProcessorInSTM
 *
 * @brief Checks if the processor is in Single threaded mode (also known
 *        as INTLOCK'd or Sleep context) when this function is called.
 *
 * For multicore (HLOS) targets, it should gives the status of core 
 * on which this function was called.
 *
 * @NOTE 
 * This function is more like a query function which returns the value 
 * at any given point. It may have changed immediately after calling it.
 *
 * @return Boolean value indicating if the processors is in single threaded mode (int locked).
 */
boolean sleepOS_isProcessorInSTM(void);

#ifdef __cplusplus
}
#endif

#endif /* _SLEEP_H_ */

