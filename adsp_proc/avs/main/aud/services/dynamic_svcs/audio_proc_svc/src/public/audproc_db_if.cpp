/*==========================================================================
ELite Source File

This file implements the setting of static topologies and modules into the
PP databases.

Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/16/10   DG      Created file
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "EliteTopology_db_if.h"
#include "EliteCmnTopology_db.h"
#include "audproc_comdef.h"
#include "adsp_core_api.h"
#include "adsp_amdb_static.h"
#include "AudDynaPPSvc.h"
#include "AudDevMgr_PrivateDefs.h"

#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

static avcs_module_info_t vol_ctrl_def = {
AUDPROC_MODULE_ID_VOL_CTRL,
};

static avcs_module_info_t resampler_def = {
AUDPROC_MODULE_ID_RESAMPLER,
};

static avcs_module_info_t sampleslip_def = {
AUDPROC_MODULE_ID_SAMPLESLIP,
};

static avcs_topology_definition_t none_copp_topo_def = {
		0,
		ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		3
};

static avcs_topology_definition_t none_popp_topo_def = {
		0,
       ASM_STREAM_POSTPROC_TOPO_ID_NONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_topology_definition_t none_poprep_topo_def = {
		0,
		ASM_STREAM_PREPROC_TOPO_ID_NONE,
		1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT,
		2
};

static avcs_module_info_t popp_topo_mod[] = {
		resampler_def,
		vol_ctrl_def
};

static avcs_module_info_t copp_topo_mod[] = {
		resampler_def,
		vol_ctrl_def,
		sampleslip_def
};

static avcs_topology_definition_t avcs_audio_static_topos[] = {
		none_popp_topo_def,
		none_poprep_topo_def,
		none_copp_topo_def,
};

static avcs_module_info_t *avcs_audio_static_topos_mods_ptr[] = {
		&popp_topo_mod[0],
		&popp_topo_mod[0],
		&copp_topo_mod[0],
};

// Module definition table for CAPIv2 modules
typedef struct _capi_v2_module_definition_t
{
   uint32_t module_id;
   capi_v2_get_static_properties_f get_static_properties_f;
   capi_v2_init_f init_f;
} capi_v2_module_definition_t;

ADSPResult AudPP_pptype_to_topo_db_client_type(AudPP_AudProcType pp_type, elite_topo_db_client_type* client_type)
{
   ADSPResult result = ADSP_EOK;

   switch (pp_type)
   {
      case DYNA_SVC_PP_TYPE_POPP:
         *client_type = TOPO_DB_CLIENT_TYPE_POPP;
         break;
      case DYNA_SVC_PP_TYPE_COPP:
         *client_type = TOPO_DB_CLIENT_TYPE_COPP;
         break;
      case DYNA_SVC_PP_TYPE_POPREP:
         *client_type = TOPO_DB_CLIENT_TYPE_POPREP;
         break;
      case DYNA_SVC_PP_TYPE_COPREP:
         *client_type = TOPO_DB_CLIENT_TYPE_COPREP;
         break;
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported PP type %ld", (uint32_t)pp_type);
         *client_type = TOPO_DB_CLIENT_TYPE_INVALID;
         result = ADSP_EUNSUPPORTED;
   }

   return result;
}

ADSPResult AudPP_AddStaticTopologies(void)
{
	ADSPResult result = ADSP_EOK;
      return result;
   }

ADSPResult AudPP_AddStaticTopologiesAndModules(void)
   {
	ADSPResult result = ADSP_EOK;
      return result;
   }

ADSPResult aud_add_static_topos_and_mods_to_cmn_db(void)
   {
	ADSPResult result = ADSP_EOK;

	//Add the audio PP static topologies to the common global database
	result = aud_add_static_topos_to_cmn_db();
   if (ADSP_FAILED(result))
   {
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "aud_add_static_topos_to_cmn_db failed with %d", result);
      return result;
   }
   //adding static modules to AMDB is now done in module_mgr
    return result;
}

ADSPResult aud_add_static_topos_to_cmn_db(void)
{
	ADSPResult result = ADSP_EOK;

	for (uint32_t i=0; i< SIZE_OF_ARRAY(avcs_audio_static_topos); i++)
	{
		avcs_topology_definition_t *topo_def_ptr = &avcs_audio_static_topos[i];
		avcs_module_info_t *mod_def_ptr = avcs_audio_static_topos_mods_ptr[i];

		//Add single topology
		result = elite_cmn_topo_db_add_topology(topo_def_ptr, mod_def_ptr, ELITE_CMN_STATIC_TOPOLOGIES);
		if (ADSP_FAILED(result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_cmn_topo_db_add_topology failed with %d", result);
			return result;
		}
	}
	return result;
}
