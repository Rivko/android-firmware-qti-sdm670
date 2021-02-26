/*==============================================================================
  FILE:         pdc_seq_target.c

  OVERVIEW:     This file implements target specific functions for PDC sequencer driver.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/seq/cfg/670/cdsp/pdc_seq_target.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "pdc_seq.h"
#include "pdc_seq_internal.h"
#include "pdc_seq_hal.h"
#include "DDIChipInfo.h"

/*==============================================================================
                               GLOBAL VARIABLES
 =============================================================================*/
/* Pointer to the list of PDC sequencer handles and list size - must be set 
 * up during target initialization */
pdc_seq_t *g_pdcSeq = NULL;

/* Externed variables for PDC sequencer configuration/handles for different
 * version of a chipset. At the moment we just have these for just one
 * version but if newer version requires different config, extern them
 * here */
extern pdc_seq_t  g_pdcSeqInstance;
extern pdc_seq_t  g_pdcSeqInstanceV2;

/*==============================================================================
                               GLOBAL FUNCTIONS
 =============================================================================*/
/* 
 * pdc_seq_target_init
 */
void pdc_seq_target_init (void)
{
  DalChipInfoVersionType version = DalChipInfo_ChipVersion ();
  
  if(version < DALCHIPINFO_VERSION(2, 0))
  {
    g_pdcSeq = &g_pdcSeqInstance;
  }
  else
  {
    g_pdcSeq = &g_pdcSeqInstanceV2;
  }

  return;
}

