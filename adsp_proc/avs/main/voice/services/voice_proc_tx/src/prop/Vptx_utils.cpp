
/*========================================================================
Vptx_utils.cpp

This file contains utility function definition for voice tx proc service.

Copyright (c) 2016-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "vptx_sensors_api.h"
#include "Elite_fwk_extns_ecns.h"
#include "Elite_fwk_extns_avc_rve_imc.h"
#include "Elite_fwk_extns_avctx_ecns_rx_drc_vol_imc.h"
#include "Elite_fwk_extns_dtmf_detection.h"

#if !defined(LOG_ADSP_VCP_VOICEPROC_RTM_C)
#define LOG_ADSP_VCP_VOICEPROC_RTM_C ( 0x184B )   // local defination in case its not defined in logs_code.h
#endif

ADSPResult vptx_handle_fwk_extension(vptx_t* pVptx,voice_capi_module_t* module_info)
{
   ADSPResult result = ADSP_EOK;
   for (uint32_t i=0;i < module_info->num_extensions.num_extensions; i++)
   {
      switch (module_info->fwk_extn_ptr[i].id)
      {
         case FWK_EXTN_ECNS:
            {
               if(TRUE ==  pVptx->ec_capi_info.ecns_monitoring_info.is_rtm_supported)
               {
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: ecns_rtm_monitoring is supported");

                  capi_v2_err_t result_ecns = CAPI_V2_EOK;
                  capi_v2_log_code_t log_prop_t;
                  log_prop_t.code = LOG_ADSP_VCP_VOICEPROC_RTM_C;   // Diag log code for RTM logging

                  capi_v2_prop_t log_prop[] = {
                     { CAPI_V2_LOG_CODE, { reinterpret_cast<int8_t *>(&log_prop_t), sizeof(capi_v2_log_code_t), sizeof(capi_v2_log_code_t) }, {FALSE, FALSE, 0} },
                  };
                  capi_v2_proplist_t log_prop_list = {sizeof(log_prop)/sizeof(capi_v2_prop_t), log_prop};

                  result_ecns = module_info->module_ptr->vtbl_ptr->set_properties(module_info->module_ptr, &log_prop_list);
                  if(CAPI_V2_FAILED(result_ecns))
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: RTM log_code set prop failed for moduleId(%lu), session(%lx)", module_info->module_id, pVptx->session.session_num);
                     return capi_v2_err_to_adsp_result(result_ecns);
                  }
               }
               break;
            }
         case FWK_EXTN_AVC_RVE_IMC_SOURCE:
            {
               capi_v2_buf_t param_data_buf;
               capi_v2_port_info_t port_info = {FALSE,FALSE,0};
               avc_rve_imc_key_t key;
               capi_v2_err_t result_avc;

               key.comm_id = COMM_ID_AVC_RVE_IMC;
               key.dest_mod_id = VOICE_MODULE_RX_AVCRVE;//dest module_id
               key.src_mod_id = VOICE_MODULE_TX_AVCRVE;   //Src module_id
               key.session_num = pVptx->session.session_num;

               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(key);
               param_data_buf.data_ptr = (int8_t*)&key;

               result_avc = module_info->module_ptr->vtbl_ptr->set_param(module_info->module_ptr,PARAM_ID_AVC_RVE_IMC_KEY,&port_info,&param_data_buf);

               pVptx->modules.special_capis[AVCRVE_CAPI] = module_info;

               if(CAPI_V2_FAILED(result_avc))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to send Key for AVC_RVE IMC,ModuleId(%lx), result(%lu), session (%lx)",module_info->module_id,result_avc,pVptx->session.session_num);
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
               key.session_num = pVptx->session.session_num;

               param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(key);
               param_data_buf.data_ptr = (int8_t*)&key;

               result_avc = module_info->module_ptr->vtbl_ptr->set_param(module_info->module_ptr,PARAM_ID_AVC_RVE_TX_IMC_KEY,&port_info,&param_data_buf);
               if(CAPI_V2_FAILED(result_avc))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: failed to send Key for AVCTx IMC,ModuleId(%lx), result(%lu), session (%lx)",module_info->module_id,result_avc,pVptx->session.session_num);
                  return capi_v2_err_to_adsp_result(result_avc);
               }
               break;
            }
         case FWK_EXTN_DTMF_DETECTION:
            {
               //NOP
               break;
            }
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

bool_t vptx_is_ready_for_get_param(vptx_t* pVptx, uint32_t module_id, uint32_t param_id)
{
    if ( (VOICEPROC_MODULE_TX == module_id) &&
            (((VOICE_PARAM_FLUENCE_SOURCETRACKING == param_id)
            || (VOICE_PARAM_FLUENCE_SOUNDFOCUS == param_id))) &&
            (FALSE == pVptx->process_data_flag) )
    {
        return false;
    }
    return true;
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
        case VOICE_PARAM_VP3:
        case VOICE_PARAM_VP3_SIZE:
            if(pVptx->modules.special_capis[ECNS_CAPI] &&
                    pVptx->ec_capi_info.ecns_vp3_info.is_vp3_supported)
            {
                *modules_info = pVptx->modules.special_capis[ECNS_CAPI];
            }
            else
            {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: VP3 Error!!! Either ECNS module(%p) "
                        "is not present or it does not support VP3, session(%lx)",
                        pVptx->modules.special_capis[ECNS_CAPI],pVptx->session.session_num);
                result = ADSP_EUNSUPPORTED;
            }
            break;

        case VOICE_PARAM_FLUENCE_SOUNDFOCUS:
        case VOICE_PARAM_FLUENCE_SOURCETRACKING:
            if(pVptx->modules.special_capis[ECNS_CAPI] &&
                    pVptx->ec_capi_info.ecns_stt_info.is_stt_supported)
            {
                *modules_info = pVptx->modules.special_capis[ECNS_CAPI];
            }
            else
            {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: STT Error!!! Either ECNS module(%p) "
                        "is not present or it does not support STT, session(%lx)",
                        pVptx->modules.special_capis[ECNS_CAPI],pVptx->session.session_num);
                result = ADSP_EBADPARAM;
            }
            break;
        default:
            break;
        }
    }
    break;

    default:
    {
        //if the calibration is received for RX AVC, copy it to Tx AVC module as well
        uint32_t search_module_id = (module_id == VOICE_MODULE_RX_AVCRVE)?
                VOICE_MODULE_TX_AVCRVE: module_id;
        result = voice_topo_get_module_info(pVptx->modules.modules_array,
                search_module_id, instance_id,
                pVptx->modules.num_modules,modules_info);
    }
    break;
    }
    return result;
}

static ADSPResult vptx_fprov2_set_holding_pattern(vptx_t* pVptx,int32_t halding_pattern)
{
   ADSPResult result;
   capi_v2_err_t capi_result;
   int32_t temp_holding_pattern = halding_pattern;

   capi_v2_buf_t param_data_buf;
   capi_v2_port_info_t port_info = {FALSE,FALSE,0};
   param_data_buf.data_ptr = (int8_t*)&temp_holding_pattern;
   param_data_buf.actual_data_len = param_data_buf.max_data_len = sizeof(int32_t);

   if(pVptx->modules.special_capis[ECNS_CAPI])
   {
      capi_result = pVptx->modules.special_capis[ECNS_CAPI]->module_ptr->vtbl_ptr->set_param(pVptx->modules.special_capis[ECNS_CAPI]->module_ptr,
            VOICE_PARAM_HOLDING_PATTERN,&port_info,&param_data_buf);
      if(CAPI_V2_FAILED(capi_result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_fprov2_set_holding_pattern(): ERROR!! failed to set facing pattern, result(%#lx). session(%#lx)",capi_result, pVptx->session.session_num);
         return ADSP_ENOMEMORY;
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx_fprov2_set_holding_pattern(): ERROR!! memory for ecns capi is not allocated. session(%#lx)", pVptx->session.session_num);
      return ADSP_ENOMEMORY;
   }

   result=capi_v2_err_to_adsp_result(capi_result);

   return result;
}

ADSPResult vptx_register_sns_qmi(vptx_t *pVptx)
{
    ADSPResult nResult = ADSP_EOK;
    /*------------------------------------------------------------*/
    /* QMI interfacing */
    /*------------------------------------------------------------*/
    // register to QMI in run cmnd to get indications from sensors
    // It applies for quad mic fpro V2 topology only
    if ( (pVptx->modules.special_capis[ECNS_CAPI]) && (VOICE_MODULE_FLUENCE_PRO == pVptx->modules.special_capis[ECNS_CAPI]->module_id))
    {
       nResult = vptx_sns_qmi_register(pVptx->qmi.pObj);                    // register with QMI to get indications from sensors
       if(ADSP_FAILED(nResult))
       {
          // Do not bail out, just print the MSG in case of failure
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"VCP: vptx QMI Interfacing failed with result (%#lx)   session(%lx)",(nResult),pVptx->session.session_num);
       }
       return ADSP_EOK;
    }
    else
    {
        return ADSP_EFAILED;
    }
}

ADSPResult vptx_setup_sns_qmi(vptx_t *pVptx, uint32_t qmi_reg_mask, uint32_t qmi_ind_mask)
{
    ADSPResult nResult= ADSP_EOK;
    // initialize sensors paramters // this applies only for quad mic fpro V2 topology
    if ((pVptx->modules.special_capis[ECNS_CAPI]) && (VOICE_MODULE_FLUENCE_PRO == pVptx->modules.special_capis[ECNS_CAPI]->module_id))
    {
        uint32_t qmi_size  = vptx_sns_qmi_get_mem_req();
        if (qmi_size)
        {
            qmi_size = VOICE_ROUNDTO8(qmi_size);
            pVptx->qmi.pObj = (void *)qurt_elite_memory_malloc(qmi_size, QURT_ELITE_HEAP_DEFAULT);
            if (NULL == pVptx->qmi.pObj)
            {
                MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to Allocate  Memory to QMI structure!! session(%x)",(int)pVptx->session.session_num);
                return ADSP_ENOMEMORY;
            }
            memset(pVptx->qmi.pObj,0,qmi_size);            // clear the buffer
            pVptx->qmi.ind_flag = FALSE;                       // by default disable indication and registration flag
            pVptx->qmi.reg_done = FALSE;
            nResult = vptx_sns_qmi_init(pVptx->qmi.pObj, &pVptx->qurt_elite_channel, qmi_reg_mask, qmi_ind_mask, pVptx->session.session_num);
            if (ADSP_FAILED(nResult))
            {
                MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,"VCP: Failed to Allocate  Memory to QMI params in vptx init!! session(%x)",(int)pVptx->session.session_num);
                return nResult;
            }
        }
    }
    return ADSP_EOK;
}

void vptx_destroy_sns_qmi(vptx_t *pVptx)
{
    // destroy memory allocated to sensors
    // It applies for quad mic fpro V2 topology only
    if ((pVptx->modules.special_capis[ECNS_CAPI]) && (VOICE_MODULE_FLUENCE_PRO == pVptx->modules.special_capis[ECNS_CAPI]->module_id))
    {
        pVptx->qmi.ind_flag = FALSE;               // disable indication and registration flag during re-init or destroy
        pVptx->qmi.reg_done = FALSE;
        vptx_sns_qmi_destroy(pVptx->qmi.pObj);
        if (NULL != pVptx->qmi.pObj)             // free qmi obj structure memory
        {
            qurt_elite_memory_free(pVptx->qmi.pObj);
            pVptx->qmi.pObj = NULL;
        }
    }
}

void vptx_handle_sns_qmi_ind(vptx_t *pVptx)
{
    // query holding pattern from fprov2 accelerometer module
    int32_t holding_pattern = vptx_sns_get_holding_pattern(pVptx->qmi.pObj);
    // Can reuse Frame counter from Profiler structure, which is reset every create/reconfig.
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx session # (%lx): Received indication from sensors at frame #%d, State = %d",
            pVptx->session.session_num,
            pVptx->profiler.num_samples,
            holding_pattern);

    vptx_sns_clear_qmi_signal(pVptx->qmi.pObj,IND_SIG);          // second arg 1 means clear indication signal
    //qurt_elite_signal_clear(pVptx->qmi_obj.ind_sig);
    pVptx->qmi.ind_flag = TRUE;

    // Call Fluence Pro V2 set_param with QUARTET_FPV2_HOLDING_PATTERN_PARAM
    vptx_fprov2_set_holding_pattern(pVptx,holding_pattern);
}

ADSPResult vptx_deregister_sns_qmi(vptx_t *pVptx)
{
    // deregister from sensors
    // It applies for quad mic fpro V2 topology only
    if ((pVptx->modules.special_capis[ECNS_CAPI]) && (VOICE_MODULE_FLUENCE_PRO == pVptx->modules.special_capis[ECNS_CAPI]->module_id))
    {
        vptx_sns_qmi_deregister(pVptx->qmi.pObj);    // deregister from sensors

        return ADSP_EOK;
    }
    else
    {
        return ADSP_EFAILED;
    }
}

void vptx_clear_sns_qmi_signal(vptx_t *pVptx)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: Vptx session # (%lx): QMI Response for Sensors succeeded",
         pVptx->session.session_num);

   pVptx->qmi.reg_done = TRUE;
   vptx_sns_clear_qmi_signal(pVptx->qmi.pObj,REG_SIG);           // second arg 0 means clear reg signal
}


void vptx_check_set_internal_param(vptx_t *pVptx)
{
    // set param might change the holding pattern configuration
    //  we need to set it back to whatever holding pattern information sensor has passed
    // this applies for quad mic FPRO V2 only
    if ((pVptx->modules.special_capis[ECNS_CAPI]) && (VOICE_MODULE_FLUENCE_PRO == pVptx->modules.special_capis[ECNS_CAPI]->module_id))
    {
        if (pVptx->qmi.ind_flag)             // set holding pattern if we have got atleast one indication from sensors
        {
            vptx_fprov2_set_holding_pattern(pVptx,vptx_sns_get_holding_pattern(pVptx->qmi.pObj));
        }
    }
}

bool_t vptx_is_custom_topology(uint32_t topology_id)
{
    switch (topology_id)
    {
    case(VPM_TX_DM_FLUENCEV5 ):
    case(VPM_TX_SM_FLUENCEV5):
    case(VPM_TX_SM_ECNS_V2):
    case(VPM_TX_QM_FLUENCE_PROV2):
    case(VPM_TX_DM_FLUENCEV5_BROADSIDE):
    case(VPM_TX_NONE):
    case VPM_TX_CUSTOM_SM_ECNS_1:
    case VPM_TX_CUSTOM_SM_ECNS_2:
    case VPM_TX_CUSTOM_SM_ECNS_3:
    case VPM_TX_CUSTOM_DM_ECNS_1:
    case VPM_TX_CUSTOM_DM_ECNS_2:
    case VPM_TX_CUSTOM_DM_ECNS_3:
    case VPM_TX_CUSTOM_QM_ECNS_1:
    case VPM_TX_CUSTOM_QM_ECNS_2:
    case VPM_TX_CUSTOM_QM_ECNS_3:
    case(VPM_TX_DM_FLUENCE):
    case(VPM_TX_SM_ECNS):
    case(VPM_TX_DM_VPECNS):
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
    switch (topology_id)
    {
    case(VPM_TX_DM_FLUENCE):
    case(VPM_TX_SM_ECNS):
    case(VPM_TX_DM_VPECNS):
    {
        if (VOICE_WB_SAMPLING_RATE < sample_rate)
        {
            return ADSP_EUNSUPPORTED;
        }
        break;
    }
    default:
        break;
    }
    return ADSP_EOK;
}

void vptx_set_media_type_for_static_topology(vptx_t *pVptx)
{
    switch(pVptx->session.topology_id)
    {
    case VPM_TX_DM_FLUENCEV5:
    case VPM_TX_DM_FLUENCEV5_BROADSIDE:
    case VPM_TX_CUSTOM_DM_ECNS_1:
    case VPM_TX_CUSTOM_DM_ECNS_2:
    case VPM_TX_CUSTOM_DM_ECNS_3:
    {
#if defined(__qdsp6__) && !defined(SIM)
        pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;

        if(pVptx->session.num_ref_channels==2)
        {
           pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
        }
#endif
        pVptx->session.num_mics = 2;
        pVptx->io.near_in.mediatype.num_channels = 2;               // Dual mic implementation
        pVptx->io.num_1ms_frame_block_process = 20;


#if defined(__qdsp6__) && !defined(SIM)		
        if(pVptx->session.num_ref_channels==2)
        {
           pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
        }
#endif

        break;
    }
    case VPM_TX_DM_FLUENCE:
    case VPM_TX_DM_VPECNS:
    {
#if defined(__qdsp6__) && !defined(SIM)
        pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
#endif
        pVptx->session.num_mics = 2;
        pVptx->io.near_in.mediatype.num_channels = 2;               // Dual mic implementation
        pVptx->io.num_1ms_frame_block_process = 10;
        break;
    }
    case VPM_TX_QM_FLUENCE_PROV2:
    case VPM_TX_CUSTOM_QM_ECNS_1:
    case VPM_TX_CUSTOM_QM_ECNS_2:
    case VPM_TX_CUSTOM_QM_ECNS_3:
    {
#if defined(__qdsp6__) && !defined(SIM)
        pVptx->log_data_format_near= VOICE_LOG_DATA_FORMAT_PCM_4CHAN_NON_INTLV;               // Dual mic implementation
        if(pVptx->session.num_ref_channels==2)
        {
           pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
        }
#endif
        pVptx->session.num_mics = 4;
        pVptx->io.near_in.mediatype.num_channels = 4;               // Quad mic implementation
        pVptx->io.num_1ms_frame_block_process = 20;
		
#if defined(__qdsp6__) && !defined(SIM)		
        if(pVptx->session.num_ref_channels==2)
        {
           pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
        }
#endif

        break;
    }
    case VPM_TX_SM_FLUENCEV5:
    case VPM_TX_SM_ECNS_V2:
    case VPM_TX_CUSTOM_SM_ECNS_1:
    case VPM_TX_CUSTOM_SM_ECNS_2:
    case VPM_TX_CUSTOM_SM_ECNS_3:
    {
#if defined(__qdsp6__) && !defined(SIM)
        pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_MONO;

        if(pVptx->session.num_ref_channels==2)
        {
           pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
        }
#endif
        pVptx->session.num_mics = 1;
        pVptx->io.near_in.mediatype.num_channels = 1;
        pVptx->io.num_1ms_frame_block_process = 20;
		
#if defined(__qdsp6__) && !defined(SIM)		
        if(pVptx->session.num_ref_channels==2)
        {
           pVptx->log_data_format_far = VOICE_LOG_DATA_FORMAT_PCM_STEREO_NON_INTLV;
        }
#endif

        break;
    }
    case VPM_TX_SM_ECNS:
    case VPM_TX_NONE:
    {
#if defined(__qdsp6__) && !defined(SIM)
        pVptx->log_data_format_near = VOICE_LOG_DATA_FORMAT_PCM_MONO;
#endif
        pVptx->session.num_mics = 1;
        pVptx->io.near_in.mediatype.num_channels = 1;
        pVptx->io.num_1ms_frame_block_process = 10;
        break;
    }
    default:
    {
        break;
    }
    }
}

