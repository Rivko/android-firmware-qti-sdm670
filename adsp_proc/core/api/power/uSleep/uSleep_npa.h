#ifndef _USLEEP_NPA_H_
#define _USLEEP_NPA_H_
/*==============================================================================
  FILE:         uSleep_npa.h
  
  OVERVIEW:     This file provides the public externs and definitions that
                are exported by the uSleep module for accessing the various sleep 
                nodes.

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/api/power/uSleep/uSleep_npa.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
                            LATENCY NODE DEFINITIONS
 =============================================================================*/
/* Name of the uSleep latency node with values in ticks.
 * Indicates how fast the system must respond to a wakeup interrupt. 
 * Input Value: Latency requirement in 19.2Mhz ticks */
#define USLEEP_LATENCY_NODE_NAME              "/uSleep/latency"

/*==============================================================================
                               LPR DEFINITIONS
 =============================================================================*/
/* Global LPR node state that enables ALL component modes */
#define USLEEP_LPR_ENABLE_ALL                 0

/*------------
 * CPU VDD LPR
 *------------*/
/* Name of the CPU VDD LPR node. 
 * Input Value: Predefined state values below */
#define USLEEP_CPUVDD_LPR_NODE_NAME           "/uSleep/cpu/vdd"

/* Valid LPRM state requests */
#define USLEEP_CPUVDD_LPR_ENABLE_ALL          USLEEP_LPR_ENABLE_ALL /* All LPRM's allowed. */
#define USLEEP_CPUVDD_LPR_DISABLE_LPM_PLUS    1                     /* Disables the LPM+ mode 
                                                                     * (i.e. full pc or APCR+) */
#define USLEEP_CPUVDD_LPR_MAX_VALUE           USLEEP_CPUVDD_LPR_DISABLE_LPM_PLUS
#ifdef __cplusplus
}
#endif

#endif /* _USLEEP_NPA_H_ */

