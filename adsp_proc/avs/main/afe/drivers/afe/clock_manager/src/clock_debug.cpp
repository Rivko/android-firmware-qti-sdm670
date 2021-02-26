/*========================================================================
   This file contains audio clock specific configuration.

  Copyright (c) 2011-2014 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/clock_debug.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "lpasshwio_devcfg.h"
#include "clock_manager_type_i.h"
#include "clock_debug_i.h"

#ifdef DEBUG_GENERIC_CLK
void afe_print_dev_cfg(generic_clk_cfg_global_info_t *
  afe_generic_clk_cfg_global_ctx_ptr)
{
  int i;

  MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "Tcsr base: 0x%lx size: 0x%lx verion: 0x%lx", 
    (afe_generic_clk_cfg_global_ctx_ptr->devcfg).tcsrRegBase,
    (afe_generic_clk_cfg_global_ctx_ptr->devcfg).tcsrRegSize,
    (afe_generic_clk_cfg_global_ctx_ptr->devcfg).hw_revision);
  
  for(i = 0; i < (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkIdCnt; i++)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "supportedClkId[] = 0x%lx", 
      (afe_generic_clk_cfg_global_ctx_ptr->devcfg).supportedClkId[i]);
  }
  
  for(i = 0; i < (afe_generic_clk_cfg_global_ctx_ptr->devcfg).infPcmOeCnt; i++)
  {
    MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, 
      "infPcmOeSupportedClkId[] infPcmOeClkIdToUse[] infPcmOeClkFreq[] 0x%lx 0x%lx 0x%lx", 
      (afe_generic_clk_cfg_global_ctx_ptr->devcfg).infPcmOeSupportedClkId[i],
      (afe_generic_clk_cfg_global_ctx_ptr->devcfg).infPcmOeClkIdToUse[i],
      (afe_generic_clk_cfg_global_ctx_ptr->devcfg).infPcmOeClkFreq[i]);
  }
}

void afe_print_cfg_nodes(
  generic_clk_cfg_global_info_t *afe_generic_clk_cfg_global_ctx_ptr)
{
   generic_clk_cfg_info_t *curr_cfg_node_ptr;
   uint32_t i;
   
   curr_cfg_node_ptr = afe_generic_clk_cfg_global_ctx_ptr->clks_cfg_node_ptr;
   if(NULL == curr_cfg_node_ptr)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No cfg");
     return;
   }
   for (i = 0; i < afe_generic_clk_cfg_global_ctx_ptr->num_cfg_nodes; i++)
   {
     // Cfg entries
     MSG_7(MSG_SSID_QDSP6, DBG_MED_PRIO, 
        "Cfg: Clk Id: 0x%05lx Attr: %ld Clk Regime Id: 0x%02lx OeClk Id: 0x%05lx OeFreq %08ld clk_freq_in_hz %08ld clk_enable_cnt %ld", 
        curr_cfg_node_ptr->clkid, curr_cfg_node_ptr->clk_attr,
        curr_cfg_node_ptr->clk_reg_id, curr_cfg_node_ptr->oe_clkid, curr_cfg_node_ptr->oe_clk_freq,
        curr_cfg_node_ptr->clk_freq_in_hz, curr_cfg_node_ptr->clk_enable_cnt
        );
     curr_cfg_node_ptr++;
   }
}

void afe_print_map(generic_clk_cfg_global_info_t *afe_generic_clk_cfg_global_ctx_ptr)
{
   const generic_map_tab_info_t *curr_map_node_ptr;
   uint32_t i;
   
   curr_map_node_ptr = afe_generic_clk_cfg_global_ctx_ptr->clks_map_table_ptr;
   if(NULL == curr_map_node_ptr)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No map");
     return;
   }
   
   for (i = 0; i < afe_generic_clk_cfg_global_ctx_ptr->num_map_table_entries; i++)
   {
     // Map entries
     MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "Map: Clk Id: 0x%05lx clk_name: %s ", 
        curr_map_node_ptr->clkid, curr_map_node_ptr->clk_name);
     curr_map_node_ptr++;
   }
   
}
#endif
