#ifndef _SLEEP_SYNTH_H_
#define _SLEEP_SYNTH_H_
/*==============================================================================
  FILE:         sleep_synth.h
  
  OVERVIEW:     Provides core public synth functions
  
  DEPENDENCIES: None
 
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/inc/sleep_synth.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                             FUNCTION DEFINITION
 =============================================================================*/
/**
 * synthLPI_updateLatency
 *
 * @brief Used to adjust the LPI latency times in the lookup table to optimize the 
 *        LPI solver 
 *  
 * @note It is assume there is only one LPI currently 
 *  
 * @param enterAdjustment:  Amount to adjust the enter latency of the LPI 
 * @param exitAdjustment:   Amount to adjust the exit latency of the LPI 
 */
void synthLPI_updateLatency(int32 enterAdjustment, int32 exitAdjustment);

/**
 * synthLPI_getCurrentLatency
 *
 * @brief Gets the current latency values of the LPI 
 *  
 * @note It is assume there is only one LPI currently 
 *  
 * @param enterLat:  Current enter latency
 * @param exitLat:   Current exit latency 
 * @param freqIdx:   Used to retreive the data for the given frequency index
 */
void synthLPI_getCurrentLatency(uint32 *enterLat, uint32 *exitLat, uint32 freqIdx);

/*
 * synthLPRM_getConfiguringModeEnterExitLatency
 *
 * @brief Returns the sum of enter+exit latencies of the synthmode that is in
 * the process of being configured.
 */
uint32 synthLPRM_getConfiguringModeEnterExitLatency(void);

#endif /* _SYNTHLPI_H_ */

