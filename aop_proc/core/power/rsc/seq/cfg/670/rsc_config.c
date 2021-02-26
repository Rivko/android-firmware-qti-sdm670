/*==============================================================================
  FILE:         rsc_config.c

  OVERVIEW:     This file contains target specific data/configuration for rsc
                instances.

  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/


/*History
   Desc		            commit ID 		date
Initial warlok seq      30207d4	       9/25/17
Note: ADDED: "0x06,      // DELI 5044ns" to sync with Napali
*/


#include "rsc.h"
#include "rsc_seq_hal.h"
#include "rsc_hal_bsp.h"

/*==========================================================================
 *                           INTERNAL MACROS
 *=========================================================================*/

/**
 * @brief Common macro to fill out structure members for array followed by
 *        its size.
 */
#define RSC_ARRAY(arr) arr, (sizeof(arr)/sizeof(arr[0]))

/*==========================================================================
 *                          QDSP6 RSC CONFIGURATIONS
 *=========================================================================*/

static uint16 aop_rsc_seq[] =
{
  0xa3,      // SET1 rsc_pdc_start_monitoring
  0xe3,      // WAIT1 pdc_rsc_monitoring_started
  0x83,      // SET0 rsc_pdc_start_monitoring
  0x70,      // BEQR sneak_interrupt=1 ReplayIntr
  0xa0,      // SET1 hidden_tcs_trig_from_seq
  0x06,      // DELI 5044ns
  0xb8,      // SET1 aop_proc_fclk_en
  0x04,      // DELI 520ns
  0xb9,      // SET1 cc_aop_pll_enable
  0xa1,      // SET1 rsc_pdc_shutdown_req

  0xe1,      // WAIT1 pdc_rsc_shutdown_ack
  0x81,      // SET0 rsc_pdc_shutdown_req
  0xa5,      // SET1 seq_wait_wakeup
  0xe0,      // WAIT1 wakeup_event
  0xa2,      // SET1 rsc_pdc_bringup_req
  0xe2,      // WAIT1 pdc_rsc_bringup_ack
  0x82,      // SET0 rsc_pdc_bringup_req
  0x99,      // SET0 cc_aop_pll_enable
  0x98,      // SET0 aop_proc_fclk_en
  0xd8,      // WAIT0 cc_aop_proc_fclk_off
  0x03,      // DELI 208ns
  0x38,      // CALL ReplayIntr
  0x20,      // RETURN
  0xa4,      // ReplayIntr: SET1 rsc_pdc_start_replay
  0xe4,      // WAIT1 pdc_rsc_replay_done
  0x84,      // SET0 rsc_pdc_start_replay
  0x20,      // RETURN
};

static rsc_hal_br_info_t aop_rsc_seq_br_info[] =
{
  /* Branch address */
  {   0x17  },
  {   0x0   },
  {   0x0   },
  {   0x0   }
};


static rsc_hal_bsp_mode_t aop_rsc_modes[] =
{
  {
    RSC_ARRAY(aop_rsc_seq),
    0x0,  // mode_id
    0x0,  // start_address
    0x0,  // start_addr_offset
    0x0,  // mode_id_same_as (0 = not applicable)
    RSC_ARRAY(aop_rsc_seq_br_info),  // branch info (static)
    NULL, // time stamp info (dynamic)
    0x0   // time stamp index
  },
};

/*==========================================================================
 *                          SUBSYSTEM rsc INSTANCES
 *=========================================================================*/

/**
 * @brief Array containing necessary information of each rsc instance on a 
 *        given subsystem.
 *
 */
rsc_t g_rsc_instances[] =
{
  {
    "aop",
    SIMPLE_8BIT_MODE,
    RSC_ARRAY(aop_rsc_modes),
    0x000d0000 
  },
};

/**
 * @brief Variable to indicate rsc instance counts.
 *
 * @note This will be externed in to rsc target file.
 */
size_t g_rsc_count = 
  sizeof(g_rsc_instances)/sizeof(g_rsc_instances[0]);

