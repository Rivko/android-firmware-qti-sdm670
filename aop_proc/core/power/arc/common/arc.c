/*===========================================================================
                                 arc.c

DESCRIPTION:
  This file initializes ARC HW block using auto generated R-Init output.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "arc.h"
#include "rinit.h"
#include "arc_internal.h"
#include "arc_hal.h"
#include "pwr_utils_lvl.h"
#include "CoreVerify.h"
#include "ChipInfo.h"
//===========================================================================
//                       Function Defintions
//===========================================================================

//---------------------------------------------------------------------------
// Public Function Definition
//---------------------------------------------------------------------------
/*
 * arc_init
 */
void arc_init (void)
{
  int ret_val = rinit_module_init ("arc");
  CORE_VERIFY (ret_val == RINIT_SUCCESS);

  if ((ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_SDM670) ||
	  (ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_SXR1130) )
  {
	  int ret_val = rinit_module_init ("arc_wl");
	  CORE_VERIFY (ret_val == RINIT_SUCCESS);
  }
  else if(ChipInfo_GetChipFamily() == CHIPINFO_FAMILY_QCS605)
  {
	  int ret_val = rinit_module_init ("arc_rd");
	  CORE_VERIFY (ret_val == RINIT_SUCCESS);
  }
	  
  /* perform target specific initialization, if any */
  arc_internal_init ();

  /* Initializing pwr_utils for vlvl <-> hlvl translation for ARC */
  pwr_utils_lvl_init ();
}

/*
 * arc_rm_id_by_vote_addr 
 */
int8 arc_rm_id_by_vote_addr (uint32 vote_addr, boolean use_slave_id)
{
  int8 i;
  uint32 rm_vote_addr;
  const uint32 addr_offset = 0xFFFF;

  for (i = 0; i < g_arc_rm_addr_map_size; i++)
  {
    rm_vote_addr = g_arc_rm_addr_map[i].addr;
    if (use_slave_id == FALSE)
    {
      rm_vote_addr &= addr_offset;
    }

    if (rm_vote_addr == vote_addr)
    {
      return i;
    }
  }

  return -1;
}

/*
 * arc_rm_op_level
 */ 
int8 arc_rm_op_level (int8 rm_id, arc_ol_t ol)
{
  uint32 rm_ols;
  int8 req_ol;

  if ((rm_id < 0) || (rm_id >= g_arc_rm_addr_map_size))
  {
    return -1;
  }

  rm_ols = arc_rm_hal_ols (rm_id);

  switch (ol)
  {
    case ARC_OL_CURRENT:
      req_ol = ARC_HAL_OL_CURRENT (rm_ols);
      break;

    case ARC_OL_SOLVED:
      req_ol = ARC_HAL_OL_SOLVED (rm_ols);
      break;

    case ARC_OL_AGGREGATED:
      req_ol = ARC_HAL_OL_AGGREGATED (rm_ols);
      break;

    case ARC_OL_SEQUENCE:
      req_ol = ARC_HAL_OL_SEQUENCE (rm_ols);
      break;

    case ARC_OL_DESTINATION:
      req_ol = ARC_HAL_OL_DESTINATION (rm_ols);
      break;

    default:
      return -1;
  }

  return req_ol;
}

//---------------------------------------------------------------------------
// Test Functions
//---------------------------------------------------------------------------
#ifdef ARC_DEBUG_TEST

#include "rpmh_client.h"
#include "cmd_db.h"

rpmh_client_handle arc_client = NULL;
uint32 arc_req_addr[] = 
{
  0x30000,
//  0x30010,
//  0x30020,
//  0x30030,     // SSC_Cx
//  0x30040,     // SSC_Mx
  0x30050,     // GFx
  0x30060,     // MSS
//  0x30070,     // MSS
//  0x30080,     // MSS
};

uint32 vrm_arcs[2];

void arc_test(void)
{
  uint32 ols[] = {0x7, 0x2, 0xF, 0x1, 0xC, 0x9, 0xD, 0xA, 0x0};
  arc_client = rpmh_create_handle (RSC_DRV_AOP, "ARC");
  uint32 req_id, i, j;
  uint32 temp_addr;

  /* Enabling ARC PDC */
  volatile uint32 *aop_pdc_en = (uint32 *)0x264500;
  *aop_pdc_en = 0x1;

  /* cmd db query */
  vrm_arcs[0] = cmd_db_query_addr("vrm.gfx");
  vrm_arcs[1] = cmd_db_query_addr("vrm.mss");

  /* Simple test */
  for (i = 0; i < sizeof (arc_req_addr)/sizeof (arc_req_addr[0]); i++)
  {
    temp_addr = arc_req_addr[i]; 
    for (j = 0; j < sizeof (ols)/sizeof (ols[0]); j++)
    {
      req_id = rpmh_issue_command (arc_client, RPMH_SET_ACTIVE, 
                                   0, temp_addr, ols[j]);
      rpmh_churn_all (arc_client, req_id);
    }
  }
}
#endif
