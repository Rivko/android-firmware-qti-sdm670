/*===========================================================================
  FILE:         mapping_os.c
  
  OVERVIEW:     This file contains the OS specific functions that obtain specific
                data for PDC driver and assigns them to various structures
                variables so that common driver can use them.

  DEPENDENCY:   None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/map/mapping_os.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_internal.h"
#include "pdc_target.h"

/*============================================================================
 *                            EXTERNAL VARIABLES
 *===========================================================================*/
/* Variables that directly store the mapping table data */
extern pdc_interrupt_mapping  g_pdcInterruptMapping[];
extern const uint32           g_pdcInterruptTableSize;

extern pdc_gpio_inputs        g_pdcGpioInputs[];
extern const uint32           g_pdcGPIOInputSize;

extern pdc_gpio_mapping       g_pdcGpioMapping[];
extern const uint32           g_pdcGPIOMappingSize;

/*==========================================================================
 *                               GLOBAL FUNCTIONS
 *==========================================================================*/
/*
 * mapOS_getInterruptTable 
 */ 
pdc_status_type mapOS_getInterruptTable(pdc_interrupts *intMap)
{
  /* Fill in table and size data */
  intMap->map     = g_pdcInterruptMapping;
  intMap->numInt  = g_pdcInterruptTableSize;

  return PDC_SUCCESS;
}

/*
 * mapOS_getInterruptTable 
 */
pdc_status_type mapOS_getGpioTable(pdc_gpios *gpioMap)
{
  gpioMap->map.gpio_inputs  = g_pdcGpioInputs;
  gpioMap->map.mux_map      = g_pdcGpioMapping;

  /* Count the number of GPIO's avialble for the MUX inputs */
  gpioMap->numGpio = g_pdcGPIOInputSize;

  /* Count the number of MUX's */
  gpioMap->numMux = g_pdcGPIOMappingSize;

  return PDC_SUCCESS;
}

