#ifndef _PDCMAP_H_
#define _PDCMAP_H_
/*==============================================================================
  FILE:         pdcMap.h
  
  OVERVIEW:     This file provides the prototypes for PDC Interrupt driver 
                mapping functionality

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/inc/pdcMap.h#1 $
$DateTime: 2017/07/21 22:10:47 $
=============================================================================*/ 
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "pdc_types.h"
#include "pdc_internal.h"

/*=========================================================================== 
 *                        FUNCTION DECLARATIONS                           
 *===========================================================================*/
/**
 * pdcMap_getInterruptTable
 *  
 * @brief Gets the interrupt mapping data
 *  
 * @param intMap:  Returned structure pointer to the interrupt mapping data table
 * 
 * @return status 
 */
pdc_status_type pdcMap_getInterruptTable(pdc_interrupts *intMap);

/**
 * pdcMap_getGpioTable
 *  
 * @brief Gets the GPIO mapping data
 *  
 * @param intMap:  Returned structure pointers to the GPIO mapping data table
 * 
 * @return status 
 */
pdc_status_type pdcMap_getGpioTable(pdc_gpios *gpioMap);

#ifdef __cplusplus
}
#endif

#endif  /* _PDCINT_H_ */

