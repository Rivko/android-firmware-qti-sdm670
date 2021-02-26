#ifndef _PDCINT_H_
#define _PDCINT_H_
/*==============================================================================
  FILE:         pdcInt.h
  
  OVERVIEW:     This file provides the prototypes for PDC Interrupt driver functionality

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/inc/pdcInt.h#4 $
$DateTime: 2017/12/13 21:59:53 $
=============================================================================*/ 
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "pdc_types.h"

/*=========================================================================== 
 *                      PUBLIC INTERRUPT FUNCTION DECLARATIONS                           
 *===========================================================================*/
/**
 * pdcInt_findEntry
 *  
 * @brief Verifies the given interrupt number is valid for the subsystem and returns
 *        the PDC bit configuration number.
 *  
 * @param interrupt:  desired interrupt
 * @param bitNum:     PDC register bit for assocated interrupt
 *  
 * @return PDC status
 */
uint32 pdcInt_findEntry(uint32 interrupt, uint32 *bitNum);

/**
 * pdcInt_config
 *  
 * @brief Configures the interrupt trigger type on the PDC for the subsystem interrupt provided 
 *        as input
 *
 * @note This function only configures the pin (pin is not enabled) with the configuation 
 *       information provided.
 *  
 * @param subSystemInterrupt  [IN]: Desired sub-system interrupt to configure as a wakeup
 *                                  at the PDC level
 * @param config              [IN]: Trigger configuration
 *                        
 * @return status 
 */
pdc_status_type pdcInt_config(uint32              subSystemInterrupt,
                              pdc_trigger_config  *config);

/**
 * pdcInt_enable 
 *  
 * @brief Enables IRQ monitoring for the given sub-system interrupt on the PDC hardware
 * 
 * @param subSystemInterrupt  [IN]: Desired sub-system interrupt to enable at the PDC level 
 *  
 * @return status 
 */
pdc_status_type pdcInt_enable(uint32 subSystemInterrupt);

/**
 * pdcInt_disable 
 *  
 * @brief Disables IRQ monitoring for the given sub-system interrupt on the PDC hardware 
 *  
 * @note Pin is not un-configured, only monitoring is disabled 
 * 
 * @param subSystemInterrupt  [IN]: Desired sub-system interrupt to disable at the PDC level 
 *  
 * @return status 
 */
pdc_status_type pdcInt_disable(uint32 subSystemInterrupt);

/**
 * pdc_getSettings 
 *  
 * @brief Gets the max interrupt and gpio numbers in the tables 
 *  
 * @param maxGpio [OUT]: Total number of Gpios
 * @param maxInt  [OUT]: Total number of Interrupts
 *  
 * @return None 
 */
void pdc_getSettings(uint32 *maxGpio, uint32 *maxInt);

#ifdef __cplusplus
}
#endif

#endif  /* _PDCINT_H_ */

