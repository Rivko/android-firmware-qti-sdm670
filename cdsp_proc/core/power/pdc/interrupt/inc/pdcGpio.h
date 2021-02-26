#ifndef _PDCGPIO_H_
#define _PDCGPIO_H_
/*==============================================================================
  FILE:         pdcGpio.h
  
  OVERVIEW:     This file provides the prototypes for PDC GPIO driver functionality

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/inc/pdcGpio.h#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "pdc_types.h"

/*=========================================================================== 
 *                      PUBLIC GPIO FUNCTION DECLARATIONS                           
 *===========================================================================*/
/**
 * gpio_findEntryByVector
 *  
 * @brief Reverse looks up the PDC bit configuration offset by the interrupt vector
 *        specific to GPIO mux output
 * 
 * @param intVector  [IN]: Subsystem interrupt vector 
 * @param bitNum    [OUT]: Associated PDC bit number
 *  
 * @return status 
 */
pdc_status_type gpio_findEntryByVector(uint32 intVector, uint32 *bitNum);

/**
 * pdcGpio_config
 *  
 * @brief Configures the GPIO trigger type on the PDC for the subsystem GPIO provided 
 *        as input
 *
 * @note This function only configures the pin (pin is not enabled) with the configuation 
 *       information provided.
 *  
 * @param gpioNumber            [IN]:   Desired sub-system GPIO to configure as a wakeup
 *                                      at the PDC level
 * @param config                [IN]:   Trigger configuration
 * @param subsystemInt          [OUT]:  Main subsytem interrupt corresponding to the given GPIO.
 *                                      This pointer MUST be given.
 * @param secondarySubsystemInt [OUT]:  Secondary subsytem interrupt corresponding to the given GPIO
 *                                      in the cases of dual edge registration.
 *                                      Note: This parameter can be null if not configuring
 *                                            dual edge, and MUST be given if registering dual edge
 * @return status 
 */
pdc_status_type pdcGpio_config(uint32              gpioNumber,
                               pdc_trigger_config  *config,
                               uint32              *subsystemInt,
                               uint32              *secondarySubsystemInt);

/**
 * pdcGpio_enable 
 *  
 * @brief Enables GPIO monitoring for the given sub-system GPIO on the PDC hardware
 * 
 * @param gpioNumber  [IN]: Desired sub-system GPIO to enable at the PDC level 
 *  
 * @return status 
 */
pdc_status_type pdcGpio_enable(uint32 gpioNumber);

/**
 * pdcGpio_disable 
 *  
 * @brief Disables GPIO monitoring for the given sub-system GPIO on the PDC hardware
 *  
 * @note Pin is not un-configured, only monitoring is disabled 
 *  
 * @param gpioNumber  [IN]: Desired sub-system GPIO to disable at the PDC level 
 *  
 * @return status 
 */
pdc_status_type pdcGpio_disable(uint32 gpioNumber);

/**
 * pdcGpio_unconfig 
 *  
 * @brief Unconfigures the previously configured GPIO, and releases the assigned MUX
 *  
 * @param gpioNumber  [IN]: Desired sub-system GPIO to unconfigure at the PDC level 
 *  
 * @return status 
 */
pdc_status_type pdcGpio_unconfig(uint32 gpioNumber);

/**
 * pdcGpio_isSupported 
 *  
 * @brief Checks if the given GPIO number is valid in the PDC
 *  
 * @param gpioNumber  [IN]: Desired sub-system GPIO to check
 *  
 * @return TRUE if the GPIO is valid, else FALSE
 */
boolean pdcGpio_isSupported(uint32 gpioNumber);

/**
 * pdcGpio_getNumberSupported 
 *  
 * @brief Returns the total number of GPIO's supported
 * 
 * @return Total number of possible GPIO inputs
 */
uint32 pdcGpio_getNumberSupported(void);

#ifdef __cplusplus
}
#endif

#endif  /* _PDCGPIO_H_ */

