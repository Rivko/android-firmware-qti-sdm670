
/*========================================================================
Vprx_utils.cpp

This file contains utility function definition for voice rx proc service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
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

#include "Vprx_Modules.h"
#include "Vprx_Svc.h"
#include "Elite_fwk_extns_avc_rve_imc.h"
#include "Elite_fwk_extns_avctx_ecns_rx_drc_vol_imc.h"

ADSPResult vprx_handle_fwk_extension(vprx_t* pVprx,voice_capi_module_t* module_info)
{
   ADSPResult result = ADSP_EOK;
   for (uint32_t i=0;i < module_info->num_extensions.num_extensions; i++)
   {
      switch (module_info->fwk_extn_ptr[i].id)
      {
         case FWK_EXTN_AVC_RVE_IMC_DESTINATION:
         {
            capi_v2_buf_t param_data_buf;
            capi_v2_port_info_t port_info = {FALSE,FALSE,0};
            avc_rve_imc_key_t key;
            capi_v2_err_t result_avc;

            key.comm_id = COMM_ID_AVC_RVE_IMC;
            key.dest_mod_id = VOICE_MODULE_RX_AVCRVE;//dest module_id
            key.src_mod_id = VOICE_MODULE_TX_AVCRVE;   //Src module_id
            key.session_num = pVprx->session.session_num;

            param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(key);
            param_data_buf.data_ptr = (int8_t*)&key;

            result_avc = module_info->module_ptr->vtbl_ptr->set_param(module_info->module_ptr,PARAM_ID_AVC_RVE_IMC_KEY,&port_info,&param_data_buf);
            if(CAPI_V2_FAILED(result_avc))
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to send Key for AVC_RVE IMC, ModuleId(%lx), result(%lu), session (%lx)",module_info->module_id,result_avc,pVprx->session.session_num);
               return capi_v2_err_to_adsp_result(result_avc);
            }
            break;
         }
         case FWK_EXTN_AVC_RVE_TX_IMC_SOURCE:
         case FWK_EXTN_AVC_RVE_TX_IMC_DESTINATION:
         {
            capi_v2_buf_t param_data_buf;
            capi_v2_port_info_t port_info = {FALSE,FALSE,0};
            avc_rve_tx_imc_key_t key;
            capi_v2_err_t result_avc;

            key.comm_id = COMM_ID_AVC_RVE_TX_IMC;
            key.dest_mod_id = VOICE_MODULE_TX_AVCRVE;
            key.session_num = pVprx->session.session_num;

            param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(key);
            param_data_buf.data_ptr = (int8_t*)&key;

            result_avc = module_info->module_ptr->vtbl_ptr->set_param(module_info->module_ptr,PARAM_ID_AVC_RVE_TX_IMC_KEY,&port_info,&param_data_buf);
            if(CAPI_V2_FAILED(result_avc))
            {
               MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to send Key for AVCTx IMC, ModuleId(%lx), result(%lu), session (%lx)",module_info->module_id,result_avc,pVprx->session.session_num);
               return capi_v2_err_to_adsp_result(result_avc);
            }
            break;
         }
         case FWK_EXTN_HPCM:
         {
            //just updating the pointer.
            //hanlding will be done after the module is created.
            pVprx->modules.special_capis[HPCM] = module_info;
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

bool_t vprx_is_custom_topology(uint32_t topology_id)
{
    switch (topology_id)
    {
    case(VPM_RX_NONE):
    case(VPM_RX_DEFAULT):
    case(VPM_RX_DEFAULT_V2):
    case(VPM_RX_DEFAULT_V3):
    {
        return FALSE;
    }
    default:
    {
        return TRUE;
    }
    }
}




