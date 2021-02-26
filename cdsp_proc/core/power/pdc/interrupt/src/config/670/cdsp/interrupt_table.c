/*==============================================================================
  FILE:         interrupt_table.c

  OVERVIEW:     PDC hardware to local interrupt mapping info.
 
  DEPENDENCIES: None
 
  Note:         Based on: Master_Interrupts_Napali v28.0

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/config/670/cdsp/interrupt_table.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_internal.h"

/*============================================================================
 *                              GLOBAL VARIABLES
 *===========================================================================*/
/* Fixed interrupts in the PDC */
pdc_interrupt_mapping g_pdcInterruptMapping[] = 
{
  /* Default trigger type     Default DRV         Subsystem Interrupt # */
  /* Bit 0 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    14},  /*  compute_pdc_spare0  */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    21},  /*  compute_pdc_spare1  */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    29},  /*  eud_int_mx[2] */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    47},  /*  not-connected */
  
  /* Bit 4 */  
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    55},  /*  not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    71},  /*  not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    79},  /*  not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    86},  /*  not-connected */  

  /* Bit 8 */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    109}, /*  not-connected */
  {{TRIGGER_RISING_EDGE,       PDC_DRV_INVALID},    112},  /* not-connected */  
};

/* Size of the table */
const uint32 g_pdcInterruptTableSize = sizeof(g_pdcInterruptMapping) / sizeof(g_pdcInterruptMapping[0]);
