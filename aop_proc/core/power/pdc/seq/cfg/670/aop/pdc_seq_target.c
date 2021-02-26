/*===========================================================================
                             pdc_seq_target.c

DESCRIPTION:
  This file implements target specific functions for PDC sequencer driver.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                   Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include "pdc_seq.h"
#include "pdc_seq_internal.h"

//---------------------------------------------------------------------------
//  Global and External Variables
//---------------------------------------------------------------------------

/* Pointer to the list of PDC sequencer handles and list size - must be set 
 * up during target initialization */
pdc_seq_t *g_pdc_seqs = NULL;
uint32 g_pdc_seq_count = 0;

/* Externed variables for PDC sequencer configuration/handles for different
 * version of a chipset. At the moment we just have these for just one
 * version but if newer version requires different config, extern them
 * here */
extern pdc_seq_t g_pdc_seq_instances[];
extern uint32 g_pdc_seq_instance_count;

//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
//  Public Function Definitions
//---------------------------------------------------------------------------

/**
 * <!-- pdc_seq_target_init -->
 *
 * @brief Target specific initialization for the PDC sequencer driver.
 *
 * Usually this function is expected to be called as part of common driver
 * initialization and will obtain different PDCs driver will talk to.
 */
void pdc_seq_target_init (void)
{
  /* Get chip version if becomes necessary */
  g_pdc_seqs = g_pdc_seq_instances;
  g_pdc_seq_count = g_pdc_seq_instance_count;
}
