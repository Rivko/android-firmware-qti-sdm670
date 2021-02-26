
/*========================================================================
Vptx_utils.cpp

This file contains utility function definition for voice tx proc service.

Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/08/16   HB      Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "Vptx_Modules.h"
#include "Vptx_Svc.h"

ADSPResult vptx_handle_fwk_extension(vptx_t* pVptx,voice_capi_module_t* module_info)
{
   ADSPResult result = ADSP_EOK;
   for (uint32_t i=0;i < module_info->num_extensions.num_extensions; i++)
   {
      switch (module_info->fwk_extn_ptr[i].id)
      {
          case FWK_EXTN_HPCM:
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: check and enable SS in HPCM CAPI");

               capi_v2_err_t result_hpcm = CAPI_V2_EOK;
               uint8_t ss_enable = FALSE;
               // sample-slip in HPCM module is required only when the topo sample-slip is done after the HPCM module placement
               // (ss_capi.mem_size_allocated == 0) indicates sample-slip capi isn't created as first module in topo
               if ( 0 == pVptx->modules.ss_capi.mem_size_allocated )
               {
                     if(pVptx->modules.special_capis[ECNS_CAPI] && pVptx->modules.special_capis[HPCM])
                     {
                        // check if HPCM module is located before ECNS module
                        if(pVptx->modules.special_capis[HPCM]->module_index < pVptx->modules.special_capis[ECNS_CAPI]->module_index)
                        {
                           ss_enable = TRUE;
                        }
                     }
               }

               fwk_extn_property_id_hpcm_ss_enable_t ss_enable_t;
               ss_enable_t.ss_enable = ss_enable;
               fwk_extn_property_id_hpcm_custom_prop_t cust_prop_t = {FWK_EXTN_PROPERTY_ID_HPCM_SS_ENABLE, &ss_enable_t};
               capi_v2_prop_t cust_prop[] = {
                  { CAPI_V2_CUSTOM_PROPERTY, { reinterpret_cast<int8_t *>(&cust_prop_t), sizeof(fwk_extn_property_id_hpcm_custom_prop_t), sizeof(fwk_extn_property_id_hpcm_custom_prop_t) }, {FALSE, FALSE, 0} },
               };
               capi_v2_proplist_t cust_prop_list = {sizeof(cust_prop)/sizeof(capi_v2_prop_t), cust_prop};

               result_hpcm = module_info->module_ptr->vtbl_ptr->set_properties(module_info->module_ptr, &cust_prop_list);
               if(CAPI_V2_FAILED(result_hpcm))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: HPCM SS enable set cust prop failed for moduleId(%lu), session(%lx)", module_info->module_id, pVptx->session.session_num);
                  return capi_v2_err_to_adsp_result(result_hpcm);
               }

               break;
            }
         default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: ERROR !! un supported fwk extention : %lx", module_info->fwk_extn_ptr[i].id);
               break;
            }
      }
   }
   return result;
}


ADSPResult voice_topo_get_module_info_tx(vptx_t* pVptx,
        uint32_t module_id, uint16_t instance_id, uint32_t param_id, voice_capi_module_t **modules_info)
{
    ADSPResult result = ADSP_EOK;

    switch (module_id)
    {
    case VOICEPROC_MODULE_TX:
    {
        switch(param_id)
        {
        default:
            break;
        }
    }
    break;

    default:
    {
        result = voice_topo_get_module_info(pVptx->modules.modules_array,
                module_id,instance_id,
                pVptx->modules.num_modules,modules_info);
    }
    break;
    }
    return result;
}

ADSPResult vptx_register_sns_qmi(vptx_t *pVptx)
{
    return ADSP_ENOTIMPL;
}

ADSPResult vptx_setup_sns_qmi(vptx_t *pVptx, uint32_t qmi_reg_mask, uint32_t qmi_ind_mask)
{
    return ADSP_EOK;
}

void vptx_destroy_sns_qmi(vptx_t *pVptx)
{
    return;
}

ADSPResult vptx_deregister_sns_qmi(vptx_t *pVptx)
{
    return ADSP_ENOTIMPL;
}

void vptx_handle_sns_qmi_ind(vptx_t *pVptx)
{
    return;
}

void vptx_clear_sns_qmi_signal(vptx_t *pVptx)
{
	return;
}

bool_t vptx_is_custom_topology(uint32_t topology_id)
{
    switch (topology_id)
    {
    case(VPM_TX_NONE):
    {
        return FALSE;
    }
    default:
    {
        return TRUE;
    }
    }
}

ADSPResult vptx_validate_sample_rate_for_static_topology(uint32_t topology_id, uint32_t sample_rate)
{
    return ADSP_EOK;
}

void vptx_set_media_type_for_static_topology(vptx_t *pVptx)
{
    switch(pVptx->session.topology_id)
    {
    case VPM_TX_NONE:
    {
#if defined(__qdsp6__) && !defined(SIM)
        pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_MONO;
#endif
        pVptx->session.num_mics = 1;
        pVptx->io.near_in.mediatype.num_channels = 1;
        pVptx->io.num_1ms_frame_block_process = 10;
        pVptx->io.near_in.mediatype.channel_mapping[0] = PCM_CHANNEL_PRI_MIC;
        break;
    }
    default:
    {
        break;
    }
    }
}

void vptx_check_set_internal_param(vptx_t *pVptx)
{
	return;
}

bool_t vptx_is_ready_for_get_param(vptx_t* pVptx, uint32_t module_id, uint32_t param_id)
{
    return true;
}

