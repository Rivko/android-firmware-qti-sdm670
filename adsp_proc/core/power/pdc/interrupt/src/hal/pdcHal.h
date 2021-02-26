#ifndef _PDC_HAL_H_
#define _PDC_HAL_H_
/*===========================================================================

  FILE:         pdcHal.h

  DESCRIPTION:  This is the hardware abstraction layer interface for the PDC 
                interrupt controller block.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/hal/pdcHal.h#1 $
$DateTime: 2017/07/21 22:10:47 $
=============================================================================*/ 
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "pdc_types.h"
#include "pdc_internal.h"

/*============================================================================
 *                                 FUNCTIONS
 *===========================================================================*/
/** 
 * pdcHAL_enable
 * 
 * @brief This function is used to enable a specific interrupt or GPIO in the PDC
 *        interrupt controller HW.
 *
 * @param pdcBitNum:  Interrupt or GPIO bit number 
 * @param pinType:    IRQ or GPIO pin type 
 * 
 * @return PDC API status
 */
pdc_status_type pdcHAL_enable(uint32         pdcBitNum,
                              pdc_pin_type   pinType);

/** 
 * pdcHAL_disable
 * 
 * @brief This function is used to enable a specific interrupt or GPIO in the PDC
 *        interrupt controller HW.
 *
 * @param pdcBitNum:  Interrupt or GPIO bit number 
 * @param pinType:    IRQ or GPIO pin type 
 * 
 * @return PDC API status
 */
pdc_status_type pdcHAL_disable(uint32         pdcBitNum,
                               pdc_pin_type   pinType);


/** 
 * pdcHAL_setTriggerConfig
 * 
 * @brief Configures the GPIO or interrupt PIN with the given configuration
 *
 * @param pdcBitNum:        Interrupt or GPIO bit number 
 * @param pinType:          IRQ or GPIO pin type 
 * @param requestedConfig:  configuraiton settings for the interrupt 
 * 
 * @return PDC API status
 */
pdc_status_type pdcHAL_setTriggerConfig(uint32              pdcBitNum,
                                        pdc_pin_type        pinType,
                                        pdc_trigger_config  *requestedConfig);

/** 
 * pdcHAL_setGPIOConfig
 * 
 * @brief Configures the GPIO mux and trigger type with the given configuration
 *
 * @param pdcBitNum:        Interrupt or GPIO bit number 
 * @param gpioNumber:       GPIO number to set as the MUX input
 * @param requestedConfig:  configuraiton settings for the interrupt 
 * 
 * @return PDC API status
 */
pdc_status_type pdcHAL_setGPIOConfig(uint32             pdcBitNum,
                                     uint32             gpioNumber,
                                     pdc_trigger_config *requestedConfig);

#ifdef __cplusplus
}
#endif

#endif /* _PDC_HAL_H_ */

