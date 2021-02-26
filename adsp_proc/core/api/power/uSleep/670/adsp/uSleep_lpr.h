#ifndef _USLEEP_LPR_H_
#define _USLEEP_LPR_H_
/*==============================================================================
  FILE:         uSleep_lpr.h

  OVERVIEW:     Target specific LPR definitions and functions

  DEPENDENCIES: The build option for uImage / island must be enabled
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/api/power/uSleep/670/adsp/uSleep_lpr.h#3 $
$DateTime: 2018/01/03 05:43:53 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                              DEFINES & TYPES
 =============================================================================*/
/* Index values for the supported main LPR mode table */
typedef enum uSleep_mode_choices
{
  USLEEP_MODE_APCRPLUS = 0,
  
  USLEEP_NUMBER_LOW_POWER_MODES
}uSleep_mode_choices;

#ifdef __cplusplus
}
#endif

#endif /* _USLEEP_LPR_H_ */

