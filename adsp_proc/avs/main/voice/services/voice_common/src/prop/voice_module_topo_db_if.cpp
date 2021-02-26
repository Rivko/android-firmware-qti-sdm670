/** @file VoiceDev_db_if.cpp

This file has the interface for the Common topology database module. This module
allows storage and query of Common topologies.

========================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/24/14   SA      Created file.
========================================================================== */


/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "EliteTopology_db.h"
#include "EliteTopology_db_if.h"
#include "voice_capi_utils.h"
#include "voice_module_topo_db_if.h"

#define VOICE_TOPO_VERSION (0)

static avcs_module_info_t vptx_none_topo[] =    {
      {VOICE_MODULE_HPCM_DEVICE_TX}
};

static avcs_module_info_t vptx_smecns_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_SMECNS},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_fv5_sm_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_FLUENCE_SM},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_smecns_v2_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_SMECNS_V2},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_dmecns_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_IIR_MIC2},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_DMECNS},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_fv5_dm_ecns_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_IIR_MIC2},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_FLUENCE_EF},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_dm_vp_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_IIR_MIC2},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_VPECNS},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_fv5_dm_broad_topo[] =  {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_IIR_MIC2},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_FLUENCE_BS},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vptx_fpv2_topo[] =    {
      {VOICE_MODULE_TX_HPF},
      {VOICE_MODULE_TX_MIC_GAIN},
      {VOICE_MODULE_IIR_MIC1},
      {VOICE_MODULE_IIR_MIC2},
      {VOICE_MODULE_IIR_MIC3},
      {VOICE_MODULE_IIR_MIC4},
      {VOICE_MODULE_TX_DTMF_DETECTION},
      {VOICE_MODULE_HPCM_DEVICE_TX},
      {VOICE_MODULE_FLUENCE_PRO},
      {VOICE_MODULE_TX_AVCRVE},
      {VOICE_MODULE_TX_FIR},
      {VOICE_MODULE_TX_IIR},
      {VOICE_MODULE_TX_AIG},
      {VOICE_MODULE_TX_DRC},
      {VOICE_MODULE_TX_VOL}
};

static avcs_module_info_t vprx_none_topo[] =    {
      {VOICE_MODULE_HPCM_DEVICE_RX}
};

static avcs_module_info_t vprx_default_topo[] =    {
      {VOICE_MODULE_RX_GAIN},
      {VOICE_MODULE_FNS},
      {VOICE_MODULE_WV},
      {VOICE_MODULE_WV_V2},
      {VOICE_MODULE_RX_AIG},
      {VOICE_MODULE_RX_DRC},
      {VOICE_MODULE_RX_FIR},
      {VOICE_MODULE_RX_IIR},
      {VOICE_MODULE_RX_VOL},
      {VOICE_MODULE_RX_AVCRVE},
      {VOICE_MODULE_HPCM_DEVICE_RX},
      {VOICE_MODULE_RX_DEC_GAIN},
      {VOICE_MODULE_PBE},
      {VOICE_MODULE_RX_HPF},
      {VOICE_MODULE_MBDRC},
      {VOICE_MODULE_RX_SPKR_GAIN}
};

static avcs_module_info_t vprx_default_v2_topo[] =    {
      {VOICE_MODULE_RX_GAIN},
      {VOICE_MODULE_FNS},
      {VOICE_MODULE_WV},
      {VOICE_MODULE_WV_V2},
      {VOICE_MODULE_RX_AIG},
      {VOICE_MODULE_RX_DRC},
      {VOICE_MODULE_RX_FIR},
      {VOICE_MODULE_RX_IIR},
      {VOICE_MODULE_RX_VOL},
      {VOICE_MODULE_RX_AVCRVE},
      {VOICE_MODULE_HPCM_DEVICE_RX},
      {VOICE_MODULE_RX_DEC_GAIN},
      {VOICE_MODULE_PBE},
      {VOICE_MODULE_RX_HPF},
      {VOICE_MODULE_MBDRC_V2},
      {VOICE_MODULE_RX_SPKR_GAIN}
};

static avcs_module_info_t vprx_default_v3_topo[] =    {
      {VOICE_MODULE_RX_GAIN},
      {VOICE_MODULE_FNS_V2},
      {VOICE_MODULE_WV},
      {VOICE_MODULE_WV_V2},
      {VOICE_MODULE_RX_AIG},
      {VOICE_MODULE_RX_DRC},
      {VOICE_MODULE_RX_FIR},
      {VOICE_MODULE_RX_IIR},
      {VOICE_MODULE_RX_VOL},
      {VOICE_MODULE_RX_AVCRVE},
      {VOICE_MODULE_HPCM_DEVICE_RX},
      {VOICE_MODULE_RX_DEC_GAIN},
      {VOICE_MODULE_PBE},
      {VOICE_MODULE_RX_HPF},
      {VOICE_MODULE_MBDRC_V2},
      {VOICE_MODULE_RX_SPKR_GAIN}
};


static voice_topo_db_entry_t voice_static_topos[] = {
      {{VOICE_TOPO_VERSION, VPM_TX_NONE, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_none_topo)},                           vptx_none_topo                  },
      {{VOICE_TOPO_VERSION, VPM_TX_SM_ECNS, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_smecns_topo)},                      vptx_smecns_topo                },
      {{VOICE_TOPO_VERSION, VPM_TX_SM_FLUENCEV5, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_fv5_sm_topo)},                 vptx_fv5_sm_topo                },
      {{VOICE_TOPO_VERSION, VPM_TX_SM_ECNS_V2, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_smecns_v2_topo)},                vptx_smecns_v2_topo             },
      {{VOICE_TOPO_VERSION, VPM_TX_DM_FLUENCE, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_dmecns_topo)},                   vptx_dmecns_topo                },
      {{VOICE_TOPO_VERSION, VPM_TX_DM_FLUENCEV5, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_fv5_dm_ecns_topo)},            vptx_fv5_dm_ecns_topo           },
      {{VOICE_TOPO_VERSION, VPM_TX_DM_VPECNS, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_dm_vp_topo)},                     vptx_dm_vp_topo                 },
      {{VOICE_TOPO_VERSION, VPM_TX_DM_FLUENCEV5_BROADSIDE, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_fv5_dm_broad_topo)}, vptx_fv5_dm_broad_topo          },
      {{VOICE_TOPO_VERSION, VPM_TX_QM_FLUENCE_PROV2, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vptx_fpv2_topo)},               vptx_fpv2_topo                  },
      {{VOICE_TOPO_VERSION, VPM_RX_NONE, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vprx_none_topo)},                           vprx_none_topo                  },
      {{VOICE_TOPO_VERSION, VPM_RX_DEFAULT, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vprx_default_topo)},                     vprx_default_topo               },
      {{VOICE_TOPO_VERSION, VPM_RX_DEFAULT_V2, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vprx_default_v2_topo)},               vprx_default_v2_topo            },
      {{VOICE_TOPO_VERSION, VPM_RX_DEFAULT_V3, 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, VOICE_SIZE_OF_ARRAY(vprx_default_v3_topo)},               vprx_default_v3_topo            },
};


ADSPResult voice_add_static_topologies_and_modules(void)
{
   ADSPResult result=ADSP_EOK;
#if 0
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_add_static_topologies_and_modules(): Adding voice topology and modules to the database");

   /*
    * Currently VCP is not adding the topologies to the database. Max modules per topology is limited to 16 in audproc_topology_definition_t.
    * VCP has more modules than 16. Need to change the db implementation or VCP can create its own DB.
    */

   //ADD topologies to the database
   result = topo_db_add_internal_topologies(TOPO_DB_CLIENT_TYPE_VOICE, VOICE_SIZE_OF_ARRAY(voice_vptx_static_topos), (const audproc_topology_definition_t*)voice_vptx_static_topos);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to add VPTx topologies with result(%d)", result);
      return result;
   }

   result = topo_db_add_internal_topologies(TOPO_DB_CLIENT_TYPE_VOICE, VOICE_SIZE_OF_ARRAY(voice_vprx_static_topos), (const audproc_topology_definition_t*)voice_vprx_static_topos);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to add VPRx topologies with result(%d)", result);
      return result;
   }



   //ADD capi modules to the data base
   for (uint32_t i = 0; i < VOICE_SIZE_OF_ARRAY(voice_capi_v2_module_definitions); i++)
   {
      ADSPResult errCode = adsp_amdb_add_static_capi_v2(ADSP_AMDB_ENTRY_TYPE_LEGACY,AMDB_MODULE_TYPE_GENERIC, voice_capi_v2_module_definitions[i].module_id, 0,
            voice_capi_v2_module_definitions[i].get_static_properties_f,
            voice_capi_v2_module_definitions[i].init_f);
      if(ADSP_FAILED(errCode))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Failed to add static capi. moduleId(%lu)", voice_capi_v2_module_definitions[i].module_id);
      }
   }
#endif
   return result;
}

ADSPResult voice_add_static_topologies_and_modules_v2(void)
{
   ADSPResult result = ADSP_EOK;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_add_static_topologies_and_modules_v2(): Adding voice topology and modules to the database");

   for (uint32_t i=0; i< VOICE_SIZE_OF_ARRAY(voice_static_topos); i++)
   {
      //Add single topology
      result = elite_cmn_topo_db_add_topology(&(voice_static_topos[i].definition), voice_static_topos[i].modules,ELITE_CMN_STATIC_TOPOLOGIES);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Static topo(%lx) addition failed with result(%lx)",voice_static_topos[i].definition.topology_id, result);
      }
   }
   //adding static modules to AMDB is now done in module_mgr
   return result;
}

ADSPResult voice_add_static_topos_to_cmn_db(void)
{
   ADSPResult result = ADSP_EOK;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_add_static_topos_to_cmn_db(): Adding voice topology to the database");

   for (uint32_t i=0; i< VOICE_SIZE_OF_ARRAY(voice_static_topos); i++)
   {
      //Add single topology
      result = elite_cmn_topo_db_add_topology(&(voice_static_topos[i].definition), voice_static_topos[i].modules,ELITE_CMN_STATIC_TOPOLOGIES);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Static topo(%lx) addition failed with result(%lx)",voice_static_topos[i].definition.topology_id, result);
      }
   }

   return result;
}

ADSPResult voice_update_topo_information(uint32_t topo_id, elite_cmn_topo_db_entry_t** topo_entry_ptr, uint32_t* actual_size_ptr, uint32_t* handle_ptr)
{
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: Update topo, topo entry %p, size %p, handle %p", topo_entry_ptr, actual_size_ptr, handle_ptr);
   ADSPResult result = elite_cmn_topo_db_get_topology( 1 << AVCS_TOPO_CFG_AREA_VOICE_BIT, topo_id, topo_entry_ptr, actual_size_ptr, handle_ptr);
   if( (ELITE_CMN_TOPO_DB_INVALID_HANDLE == *handle_ptr) || (ADSP_FAILED(result)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: Failed to find topology id %lx", topo_id);
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}

ADSPResult voice_release_topo_information(uint32_t handle)
{
   return elite_cmn_topo_db_release_topology(handle);
}
