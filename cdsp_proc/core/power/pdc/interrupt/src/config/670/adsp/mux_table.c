/*==============================================================================
  FILE:         mux_table.c

  OVERVIEW:     PDC hardware to local gpio mux mapping info.
 
  DEPENDENCIES: None
  
  Note:         Based on: napaliq_top.irq.flatV16.0

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/config/670/adsp/mux_table.c#1 $
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
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     16 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     52 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     58 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     84 },

  /* Mux 4 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     92 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     101 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     126 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     131 },

  /* Mux 8 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     135 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     141 },  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     248 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     249 }, 

    /* Mux 12 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     250 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     251 }, 
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     252 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     253 }, 
  
    /* Mux 16 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     254 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     255 }, 
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     256 },
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},       NULL,     257 }, 

};

/* Size of above table */
const uint32 g_pdcGPIOMappingSize = sizeof(g_pdcGpioMapping) / sizeof(g_pdcGpioMapping[0]);
