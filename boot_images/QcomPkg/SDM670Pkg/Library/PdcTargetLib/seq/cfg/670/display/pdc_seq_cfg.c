/*===========================================================================
                              pdc_seq_cfg.c

DESCRIPTION:
  This file provides configuration data like sequences, delays, etc for
  various PDC sequencers handled on a given subsystem.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
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
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"
#include "pdcHal_hwio.h"

#define PDC_SEQ_ARRAY(arr) arr, (sizeof(arr)/sizeof(arr[0]))

//---------------------------------------------------------------------------
//  Global Variables
//---------------------------------------------------------------------------

/* Cx collapse with AOSS sleep */
pdc_seq_cmd_t cx_col_aoss[] =
{
  0xe1,    //CX_COL_AOSS: WAIT1    rx_fifo_empty         # Ensure RX FIFO empty from any previous SDE RSCC commands.
  0xa1,    //             SET1     pdc_mode_control      # Switch ePCB from "pass-throug mode" to PDC TCS logic,
  0x80,    //             SET0     timer_match_valid     # Enable use of PDC timer to not set from PDC_MATCH_VALUE above, CR QCTDD03836087
  0xbe,    //             SET1     seq_tcs_1_start       # Send TCS0 contents, that is, the "sleep set".
  0xfe,    //             WAIT1    tcs_1_seq_cmp         # Wait for all commands to be sent. TCS0 setup with fire-and-forget.
  0xa5,    //             SET1     wfi_prof_unit_3       # Trigger time stamp on profiling unit 3.
  0xa3,    //             SET1     cx_collapse_req       # Signal AOP that vdd_cx collapse is requested.
  0xa2,    //             SET1     soc_sleep_req         # Signal AOP that AOSS sleep is requested.
  0xa4,    //             SET1     dbg_rsrc_rel_req      # To Enable AOP to release debug resources
  0xe0,    //             WAIT1    sequence_resume       # Wait for wake event of: PDC modem_pdc_irq_in, PDC timer, or
  0xa6,    //             SET1     gen_prof_unit_5       # HW workaround for QCTDD03447497
  0x82,    //             SET0     soc_sleep_req         # Clear signal for AOSS sleep request.
  0x83,    //             SET0     cx_collapse_req       # Clear signal for vdd_cx_collapse request
  0x84,    //             SET0     dbg_rsrc_rel_req      # To indicate to AOP to vote for debug resources
  0xe2,    //             WAIT1    soc_sleep_unlock      # Wait for AOP to signal AOSS sleep state exit.
  0xe3,    //             WAIT1    cx_collapse_unlock    # Wait AOP to signal vdd_cx collapse state exit.
  0xbd,    //             SET1     seq_tcs_2_start       # Send TCS2 contents, that is, the "wake set".
  0xfd,    //             WAIT1    tcs_2_seq_cmp         # Wait for all commands to be sent. TSC1 setup with wait-for-complete.
  0x80,    //             SET0     timer_match_valid     # Disable PDC timer.
  0x81,    //             SET0     pdc_mode_control      # Restore "pas-through mode".
  0x20,    //             RETURN                         # End of sequence
  0x20,    //             RETURN                         # Pad to word boundary
  0x20,    //             RETURN                         # Pad to word boundary
  0x20,    //             RETURN                         # Pad to word boundary
};

/* Branch addresses - dummy for testing */
uint8 branches[PDC_SEQ_HAL_BR_ADDR_REG_COUNT] = { 0x0 };

/* Delay values - dummy for testing */
uint32 delays[PDC_SEQ_HAL_DELAY_REG_COUNT] =  { 0x0 };

pdc_seq_hal_bsp_cfg_t pdc_cfg =
{
  PDC_SEQ_ARRAY (branches),
  PDC_SEQ_ARRAY (delays)
};

/* PDC supported low power mode */
pdc_seq_hal_bsp_mode_t pdc_modes[] =
{
  {
    PDC_SEQ_ARRAY (cx_col_aoss),
    1,
    NULL
  }
};

/* PDC sequencers handled on this Subsystem */
pdc_seq_t g_pdc_seq_instances[] =
{
  {
    "display",
    &pdc_cfg,
    PDC_SEQ_ARRAY (pdc_modes),
    RPMH_PDC_DISPLAY_DISPLAY_PDC_REG_BASE - AOSS_BASE,  //Get target base
    0
  },
};

uint32 g_pdc_seq_instance_count =
  sizeof (g_pdc_seq_instances) / sizeof (g_pdc_seq_instances[0]);

