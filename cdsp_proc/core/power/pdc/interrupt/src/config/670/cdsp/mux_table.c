/*==============================================================================
  FILE:         mux_table.c

  OVERVIEW:     PDC hardware to local gpio mux mapping info.
 
  DEPENDENCIES: None
  
  Note:         Based on: Master_Interrupts_Napali v28.0

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/config/670/cdsp/mux_table.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_internal.h"

/*============================================================================
 *                              GLOBAL VARIABLES
 *===========================================================================*/
/* Local interrupt vector mapping for the PDC MUX outputs */
pdc_gpio_mapping g_pdcGpioMapping[] = 
{
  /* Default trigger type      Default DRV             GPIO ptr  Subsystem Interrupt#  */

  /* Mux 0 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     117 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     127 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     138 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     152 },

  /* Mux 4 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     158 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     163 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     168 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     175 },

  /* Mux 8 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     181 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     74 },  
};

/* Size of above table */
const uint32 g_pdcGPIOMappingSize = sizeof(g_pdcGpioMapping) / sizeof(g_pdcGpioMapping[0]);
