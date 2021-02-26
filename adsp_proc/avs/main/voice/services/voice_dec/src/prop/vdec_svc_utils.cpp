
/*========================================================================
vdec_svc_utils.cpp

This file contains utility function definition for voice decoder service.

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

#include "vdec_private.h"
#include "adsp_vparams_internal_api.h"
#include "capi_v2_adsp_error_code_converter.h"


ADSPResult vdec_beamr_set_param (vdec_t* vdec_ptr, int8_t *cal_payload, uint32_t pid, uint16 param_size)
{
   ADSPResult result = ADSP_EOK;
   if(NULL == vdec_ptr->modules.dec_capi.common.module_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_beamr_set_param(): dec capi module ptr is NULL");
      return ADSP_EBADPARAM;
   }
   switch (pid)
   {
      case VOICE_PARAM_MOD_ENABLE:
         {

            capi_v2_err_t capi_result = CAPI_V2_EOK;
            capi_v2_buf_t vdec_beamr_set_param;
            capi_v2_port_info_t port_info = {0, 0 , 0};
            voc_param_beamr_t beamr_set_param;

            beamr_set_param.beamr_enable = *((int16_t*) cal_payload);

            vdec_beamr_set_param.data_ptr = (int8_t*)&beamr_set_param;
            vdec_beamr_set_param.actual_data_len = sizeof(voc_param_beamr_t);
            vdec_beamr_set_param.max_data_len = sizeof(voc_param_beamr_t);

            //if BeAMR is enabled and setparam is for disable. This condtion is added to support following sequence,
            //set media type amr-nb -> beamr enable -> run -> stop -> set media type FR -> beamr disable -> widevoice enable -> run -> stop
            if ((FALSE == beamr_set_param.beamr_enable) && (vdec_ptr->voice_module_bbwe == VOICE_BBWE_BEAMR || vdec_ptr->voice_module_bbwe == VOICE_BBWE_NONE))
            {
               // if media_type is amr-nb or amr-nb-if2 or eamr, route set_param to capi
               if( (VSM_MEDIA_TYPE_AMR_NB_IF2 == vdec_ptr->voc_type)
                   || (VSM_MEDIA_TYPE_AMR_NB_MODEM == vdec_ptr->voc_type)
                   || (VSM_MEDIA_TYPE_EAMR == vdec_ptr->voc_type))
               {
                  capi_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.dec_capi.common.module_ptr, VOC_PARAM_DEC_BEAMR, &port_info, &vdec_beamr_set_param);

                  if(CAPI_V2_EOK != capi_result)
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_beamr_set_param(): failed. result(%ld)", capi_result);
                     result = capi_v2_err_to_adsp_result(capi_result);
                  }
               }
               else // for other media types
               {
                  vdec_ptr->voice_module_bbwe = VOICE_BBWE_NONE;

                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "VCP: vdec_beamr_set_param():: disabling BeAMR for mediatype(%x), session(%#lx)", (int )vdec_ptr->voc_type, vdec_ptr->session_num);
                  result = ADSP_EOK;
               }
               return result;
            }

            //Reject set param for beamr for all the media types except amr-nb, amr-nb-if2 and eamr
            if((VSM_MEDIA_TYPE_AMR_NB_IF2 != vdec_ptr->voc_type) && (VSM_MEDIA_TYPE_AMR_NB_MODEM != vdec_ptr->voc_type) && (VSM_MEDIA_TYPE_EAMR != vdec_ptr->voc_type))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "vdec_beamr_set_param: BeAMR enable/disable is unspported for media_type(0x%lx)", vdec_ptr->voc_type);
               result = ADSP_EUNSUPPORTED;
               break;
            }

            //Reject set param for beamr for media type amr-nb and amr-nb-if2 if in RUN state
            if((TRUE == vdec_ptr->process_data) && ((VSM_MEDIA_TYPE_AMR_NB_IF2 == vdec_ptr->voc_type) || (VSM_MEDIA_TYPE_AMR_NB_MODEM == vdec_ptr->voc_type)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "vdec_beamr_set_param: BeAMR enable/disable is unsupported in RUN state for AMR-NB/AMR-NB-IF2 vocoder");
               result = ADSP_EUNSUPPORTED;
               break;
            }

            //Reject set param for beamr if any other Blind bandwidth extension feature (WV1/WV2) is already enabled
            if ((VOICE_BBWE_NONE != vdec_ptr->voice_module_bbwe) && (VOICE_BBWE_BEAMR != vdec_ptr->voice_module_bbwe))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "vdec_beamr_set_param: BeAMR enable/disable is unspported while other BBWE module is alredy enabled, BBWE_flag(%d)",
                     (int)vdec_ptr->voice_module_bbwe);
               result = ADSP_EUNSUPPORTED;
               break;
            }

            //send set param to eamr capi
            capi_result = vdec_ptr->modules.dec_capi.common.module_ptr->vtbl_ptr->set_param(vdec_ptr->modules.dec_capi.common.module_ptr, VOC_PARAM_DEC_BEAMR, &port_info, &vdec_beamr_set_param);

            if(CAPI_V2_EOK != capi_result)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_beamr_set_param(): failed. result(%ld)", capi_result);
               return capi_v2_err_to_adsp_result(capi_result);
            }

            //Update bbwe flag
            if (beamr_set_param.beamr_enable)
            {
               vdec_ptr->voice_module_bbwe = VOICE_BBWE_BEAMR;
            }
            else
            {
               vdec_ptr->voice_module_bbwe = VOICE_BBWE_NONE;
            }

            //update decoder sampling rate
            vdec_ptr->sampling_rate_dec = vdec_ptr->modules.dec_capi.output_media_type.data_format.sampling_rate;
            vdec_ptr->samp_rate_factor = (vdec_ptr->sampling_rate_dec / VOICE_NB_SAMPLING_RATE);
            vdec_ptr->frame_samples_dec = VOICE_FRAME_SIZE_NB_20MS * (vdec_ptr->samp_rate_factor);

         }
         break;
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_beamr_set_param():: Error!! Bad Param id. session(%#lx)", vdec_ptr->session_num);
            result = ADSP_EBADPARAM;
         }
         break;
   }
   return result;
}

ADSPResult vdec_beamr_get_param (vdec_t* vdec_ptr, int8_t *cal_payload, uint32_t pid, uint16 buffer_size, uint16_t *param_size)
{
   ADSPResult result = ADSP_EOK;

   switch (pid)
   {
      case VOICE_PARAM_MOD_ENABLE:
         {
            if (sizeof(int32_t) > buffer_size)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_beamr_get_param():: Error!! Bad Param Size. session(%#lx)", vdec_ptr->session_num);
               result = ADSP_EBADPARAM;
               break;
            }
            *param_size = sizeof(int32_t);

            *((int32_t*) cal_payload) = 0;   // Clearing the whole buffer
            *((int16_t*) cal_payload) = (VOICE_BBWE_BEAMR == vdec_ptr->voice_module_bbwe) ? TRUE : FALSE;
         }
         break;
      default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VCP: vdec_beamr_get_param():: Error!! Bad Param id. session(%#lx)", vdec_ptr->session_num);
            result = ADSP_EBADPARAM;
         }
         break;
   }
   return result;
}

