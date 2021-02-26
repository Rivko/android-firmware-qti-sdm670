
/*==========================================================================
ELite Source File

This file implements the buffer utility functions for the Audio Post
Processing Dynamic Service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
18/07/16    HB      Created file for proprietary information.
=========================================================================== */

/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "audproc_comdef.h"
#include "capi_v2_dynamic_resampler.h"
#include "capi_v2_virtualizer.h"
#include "ds1_calib.h"
#include "dts_hpx_calib.h"

/*---------------------------------------------------------------------------
 * Function Definitions
 * -------------------------------------------------------------------------*/
//Module which uses hw resampler.
static const uint32_t module_using_hw_resampler[]=
{
		AUDPROC_MODULE_ID_RESAMPLER,
		AUDPROC_MODULE_ID_DTS_HPX_PREMIX,
		AUDPROC_MODULE_ID_DTS_HPX_POSTMIX
};

/**
 * This function is to suspend hw modules.
 * @param[in] me
 * Pointer to the current instance structure
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
ADSPResult AudPP_SuspendPrepare(const ThisAudDynaPPSvc_t *me)
{
	ADSPResult result = ADSP_EOK;
	// MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Disabling HW resampler if present",me->objId);
	for(uint8_t i = 0; i < sizeof(module_using_hw_resampler)/sizeof(module_using_hw_resampler[0]); i++)
	{
		if (topo_is_module_present(me->pTopologyObj, module_using_hw_resampler[i]))
		{
			// Send disable to HW resampler
			struct audproc_resamp_hw_resume_suspend_t
			{
				asm_stream_param_data_v2_t header;
				param_id_dynamic_rs_hw_resume_suspend_t hw_rs_resume_suspend;
			}payload;

			payload.header.module_id   = module_using_hw_resampler[i];
			payload.header.param_id    = AUDPROC_PARAM_ID_RESAMPLER_HW_RESUME_SUSPEND;
			payload.header.param_size  = sizeof(payload.hw_rs_resume_suspend);
			payload.header.reserved    = 0;
			payload.hw_rs_resume_suspend.is_suspend = 1;

			result = topo_set_param(me->pTopologyObj, &payload, sizeof(payload), ALL_INSTANCES);
		}
	}
	return result;
}

/**
 * This function is to resume hw modules.
 * @param[in] me
 * Pointer to the current instance structure
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
ADSPResult AudPP_RunPrepare(const ThisAudDynaPPSvc_t *me)
{
	ADSPResult result = ADSP_EOK;

	//MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Enabling HW resampler if present",me->objId);
	for(uint8_t i = 0; i < sizeof(module_using_hw_resampler)/sizeof(module_using_hw_resampler[0]); i++)
	{

		if (topo_is_module_present(me->pTopologyObj, module_using_hw_resampler[i]))
		{
			// Send resume to HW resampler
			struct audproc_resamp_hw_resume_suspend_t
			{
				asm_stream_param_data_v2_t header;
				param_id_dynamic_rs_hw_resume_suspend_t hw_rs_resume_suspend;
			}payload;

			payload.header.module_id   = module_using_hw_resampler[i];
			payload.header.param_id    = AUDPROC_PARAM_ID_RESAMPLER_HW_RESUME_SUSPEND;
			payload.header.param_size  = sizeof(payload.hw_rs_resume_suspend);
			payload.header.reserved    = 0;
			payload.hw_rs_resume_suspend.is_suspend = 0;

			result |= topo_set_param(me->pTopologyObj, &payload, sizeof(payload), ALL_INSTANCES);
		}
	}
	return result;
}

/**
 * Returns how many output buffers are needed based on the current PP
 * configuration.
 *   - Dependencies: The PP configuration structure must have been initialized.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 *   Pointer to the current instance structure.
 *
 * @return uint32_t
 * Returns the number of output buffers needed.
 */
uint32_t AudPP_NumOutputBuffersRequired(ThisAudDynaPPSvc_t *me)
{

	if(((!ADSP_FAILED(topo_db_is_module_present(TOPO_DB_CLIENT_TYPE_COPP, me->ppCfgInfo.dynaPPSvcTopologyID, AUDPROC_MODULE_ID_DS1AP)))
			||(!ADSP_FAILED(elite_cmn_topo_db_is_module_present(1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT, me->ppCfgInfo.dynaPPSvcTopologyID, AUDPROC_MODULE_ID_DS1AP))))
			&&(BUF_NUM_AFE_FRAME_LOW_LATENCY == (me->ppCfgInfo.dynaPPSvcOutBufNumAfeFrames))
			&&(DYNA_SVC_PP_TYPE_COPP == (me->ppCfgInfo.dynaPPSvcType)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ALLOCATING 8 BUFFERS FOR DS1 WITH TOPO ID %x IN LOW LATENCY",(unsigned int)me->ppCfgInfo.dynaPPSvcTopologyID);
      return 8; // Allocate 8 buffers for DS1 in low latency mode to compensate processing delay if any.
   }
	else if(((!ADSP_FAILED(topo_db_is_module_present(TOPO_DB_CLIENT_TYPE_COPP, me->ppCfgInfo.dynaPPSvcTopologyID, AUDPROC_MODULE_ID_DTS_HPX_POSTMIX)))
			||(!ADSP_FAILED(elite_cmn_topo_db_is_module_present(1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT, me->ppCfgInfo.dynaPPSvcTopologyID, AUDPROC_MODULE_ID_DTS_HPX_POSTMIX))))
			&&(DYNA_SVC_PP_TYPE_COPP == (me->ppCfgInfo.dynaPPSvcType)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "ALLOCATING 4 BUFFERS FOR DTS HPX POSTMIX WITH TOPO ID %x", (unsigned int)me->ppCfgInfo.dynaPPSvcTopologyID);
      return 4; // Allocate 4 buffers for DTSHPX as frame size is 512 samples
   }
   else
   {
      return 2; // Double buffering to enable concurrent processing, with minimum delay.
   }
}

static ADSPResult AudPP_Init_Hpf_Coeffs(topo_struct *topo_obj)
{
   static const uint16_t num_stages = 3;
   static const uint16_t coeffs_per_stage = 5;

   struct hpf_coeff_data_struct
   {
      asm_stream_param_data_v2_t header;
      audproc_iir_filter_config_params_t config_params;
      int32_t filter_coeffs[num_stages * coeffs_per_stage];
      int16_t num_shift_factor[num_stages];
      uint16_t pan_setting[num_stages];
   } hpf_init_data;

   hpf_init_data.header.module_id = AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER;
   hpf_init_data.header.param_id  = AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS;
   hpf_init_data.header.param_size = sizeof(hpf_init_data) - sizeof(hpf_init_data.header);

   hpf_init_data.config_params.num_biquad_stages = num_stages;
   hpf_init_data.config_params.reserved          = 0;

   // initialize counter
   int16_t k=0;
   // For HPF the default coefficients set here
   // Filter coeff setting for band = 1
   hpf_init_data.filter_coeffs[ ( k * 5)]     = 0x3FDF73F8;
   hpf_init_data.filter_coeffs[ ( k * 5) + 1] = 0x80413203;
   hpf_init_data.filter_coeffs[ ( k * 5) + 2] = 0x3FDF73F8;
   hpf_init_data.filter_coeffs[ ( k * 5) + 3] = 0x804187F5;
   hpf_init_data.filter_coeffs[ ( k * 5) + 4] = 0x3FBF3DE2;

   // Filter coeff setting for band = 2
   k+=1;
   hpf_init_data.filter_coeffs[ ( k * 5)]     = 0x3FAB5548;
   hpf_init_data.filter_coeffs[ ( k * 5) + 1] = 0x80A95F52;
   hpf_init_data.filter_coeffs[ ( k * 5) + 2] = 0x3FAB5548;
   hpf_init_data.filter_coeffs[ ( k * 5) + 3] = 0x80A9B4FE;
   hpf_init_data.filter_coeffs[ ( k * 5) + 4] = 0x3F57003B;

   // Filter coeff setting for band = 3
   k+=1;
   hpf_init_data.filter_coeffs[ ( k * 5)]     = 0x3FCBAC3A;
   hpf_init_data.filter_coeffs[ ( k * 5) + 1] = 0xC03453C6;
   hpf_init_data.filter_coeffs[ ( k * 5) + 2] = 0;
   hpf_init_data.filter_coeffs[ ( k * 5) + 3] = 0xC068A78C;
   hpf_init_data.filter_coeffs[ ( k * 5) + 4] = 0;

   for (uint32_t i = 0; i < num_stages; i++)
   {
      hpf_init_data.num_shift_factor[i] = 2;
      hpf_init_data.pan_setting[i] = 1;
   }

   return topo_set_param(topo_obj, &hpf_init_data, sizeof(hpf_init_data), FIRST_INSTANCE);
}

/**
 * Function to initialize specific modules.
 * @param[in, out] me
 * Pointer to the current instance structure.
 *
 * @return ADSPResult
 * Returns the error code indicating status.
 */
ADSPResult AudPP_InitializePPFeatures(ThisAudDynaPPSvc_t* me)
{
	ADSPResult result = ADSP_EOK;

   if (topo_is_module_present(me->pTopologyObj, AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER))
   {
      ADSPResult errCode;
      errCode = AudPP_Init_Hpf_Coeffs(me->pTopologyObj);
      if (ADSP_FAILED(errCode))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: Could not initialize HPF coefficients.", me->objId);
      }
   }

   //Disable Hw resampler for low latency use cases.
   if(ASM_LEGACY_STREAM_SESSION != me->ppCfgInfo.perf_mode)
   {
	   for(uint8_t i = 0; i < sizeof(module_using_hw_resampler)/sizeof(module_using_hw_resampler[0]); i++)
	   {
		   if (topo_is_module_present(me->pTopologyObj, module_using_hw_resampler[i]))
		   {
			   //Disable Hw resampler
			   struct audproc_disable_hw_resampler_t
			   {
				   asm_stream_param_data_v2_t header;
				   param_id_dynamic_rs_force_disable_hw_rs_t disable;
			   }payload;

			   payload.header.module_id   = module_using_hw_resampler[i];
			   payload.header.param_id    = AUDPROC_PARAM_ID_RESAMPLER_FORCE_DISABLE_HW_RS;
			   payload.header.param_size  = sizeof(payload.disable);
			   payload.header.reserved    = 0;
			   payload.disable.force_disable = 1;

			   result = topo_set_param(me->pTopologyObj, &payload, sizeof(payload), ALL_INSTANCES);
			   if (ADSP_FAILED(result))
			   {
				   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: Could not disable HW resampler..", me->objId);
                   return result;
               }
		   }
	   }
   }

   return result;
}

uint32_t topo_get_search_module_id(const uint32_t module_id)
{
   const uint32_t ds1ap_module_ids[] = {
             AUDPROC_MODULE_ID_DS1AP,
             AUDPROC_MODULE_ID_DS1AP_BUNDLE,
             AUDPROC_MODULE_ID_DS1AP_VIS,
             AUDPROC_MODULE_ID_DS1AP_NGS,
             AUDPROC_MODULE_ID_DS1AP_DE ,
             AUDPROC_MODULE_ID_DS1AP_DV ,
             AUDPROC_MODULE_ID_DS1AP_IEQ ,
             AUDPROC_MODULE_ID_DS1AP_GEQ ,
             AUDPROC_MODULE_ID_DS1AP_VIRTUALIZER,
             AUDPROC_MODULE_ID_DS1AP_VM ,
             AUDPROC_MODULE_ID_DS1AP_AO ,
             AUDPROC_MODULE_ID_DS1AP_ARE ,
             AUDPROC_MODULE_ID_DS1AP_PLIM
           };

   for(uint32_t i = 0; i < SIZE_OF_ARRAY(ds1ap_module_ids); i++)
   {
      if (module_id == ds1ap_module_ids[i])
      {
         return AUDPROC_MODULE_ID_DS1AP;
      }
   }

   return module_id;
}

ADSPResult topo_get_virtualizer_module_id(uint32_t *module_id)
{
	*module_id = AUDPROC_MODULE_ID_VIRTUALIZER;
	return ADSP_EOK;
}
ADSPResult topo_get_virtualizer_delay_param_id(uint32_t *param_id)
{
	*param_id = AUDPROC_PARAM_ID_VIRTUALIZER_DELAY;
	return ADSP_EOK;
}
ADSPResult topo_get_virtualizer_volume_ramp_param_id(uint32_t *param_id)
{
	*param_id = AUDPROC_PARAM_ID_VIRTUALIZER_VOLUME_RAMP;
	return ADSP_EOK;
}

