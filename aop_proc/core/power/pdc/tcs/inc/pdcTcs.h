#ifndef _PDCTCS_H_
#define _PDCTCS_H_
/*============================================================================
  FILE:         pdcTcs.h
  
  OVERVIEW:     This file provides the types for the PDC TCS driver
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=============================================================================
$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/pdc/tcs/inc/pdcTcs.h#1 $
$DateTime: 2018/02/16 04:48:18 $
============================================================================*/
#include "comdef.h"
#include "pdcTcsCfg.h"
#include "tcs_resource.h"

/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/
/**
 * pdcTcs_getResources
 *
 * @brief Retreives the command/data pairs for each PDC TCS
 * 
 * @return Array pointer of NUM_COMMANDS_PER_TCS size which contains the TCS commands for
 *         the given TCS number
 * 
 * @param tcsNum: TCS number to get
 */
const pdc_tcs_config* pdcTcs_getResources(tcs_usage tcsNum);

/**
 * pdcTcs_getAllResourceList
 *
 * @brief Retreives a list of all the resources that are avaiable for control in this TCS target 
 *        driver 
 * 
 * @return Array pointer of TCS_TOTAL_RESOURCE_NUM size which contain the resources controlled 
 *         for this target 
 */
const pdc_tcs_resource* pdcTcs_getAllResourceList(void);

#endif /* _PDCTCS_H_ */

