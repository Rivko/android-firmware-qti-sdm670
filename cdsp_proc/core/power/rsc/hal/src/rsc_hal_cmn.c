/*==============================================================================
  FILE:         rsc_hal_cmn.c
  
  OVERVIEW:     This file implements HAL APIs exposed to rsc driver and performs
                actual HW register read and write in island section if supported
 
  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/rsc/hal/src/rsc_hal_cmn.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "rsc_hwio.h"

/*===========================================================================
 *                            EXTERNAL FUNCTIONS
 *==========================================================================*/
/*
 * rsc_hal_set_trigger_start_address
 */
void rsc_hal_set_trigger_start_address(uint32 rsc_base, uint16 addr)
{
  RSC_HWIO_OUTX(rsc_base, RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0, addr);
}

/* 
 * rsc_hal_seq_busy
 */
uint32 rsc_hal_seq_busy(uint32 rsc_base)
{
  return RSC_HWIO_INX(rsc_base, RSC_SEQ_BUSY_DRV0);
}

/*
 * rsc_hal_set_trigger_seq
 */
void rsc_hal_set_trigger_seq(uint32 rsc_base, uint8 val)
{
  RSC_HWIO_OUTX(rsc_base, RSC_SEQ_OVERRIDE_TRIGGER_DRV0, val);
}

