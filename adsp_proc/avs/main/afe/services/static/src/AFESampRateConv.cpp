/*========================================================================
   Implements wrapper functions for resamplers.

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFESampRateConv.cpp#2 $
 ====================================================================== */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AFESampRateConv.h"
#include "AFEPortManager.h"
#include "AFEInternal.h"
#include "AFEMmpm_i.h"
#include "adsp_vparams_internal_api.h"
#include "adsp_amdb_static.h"
#include "adsp_audproc_api.h"

//#define FIR_FILE_DUMP_SIM
//#define FIR_DBG

/* For tracking the RS process calls in order to reset the buffered samples close to this boundary.
 * Needed for upsampling case, for ex, 8 -> 176.4 Khz can generate upto OSR = (176.4/8) = 22 samples extra at anytime
 * Thats why we need to track this and reset the buffered samples at the boundary otherwise the next interrupt would
 * generate less samples than needed.
 *
 * Only needed for Output buffering, as for input buffering the internal buffer gets totally consumed at the 440th
 * RS process call, thats why we dont need to track it.
 *
 * After 440 frames, fractional sample accumulates into 1 frame. */
#define FRACTIONAL_RS_PROCESS_BOUNDARY 440

/* frac rate block transfer time (when port operates at default latency mode)in Q25  (44/44.1)*1000 = 997us */
#define FRACTIONAL_RATE_BLOCK_TRANSFER_TIME_US (33478344853U)
/* frac rate block transfer time (when port operates at low latency mode) in Q25  997/2 = 498us */
#define FRACTIONAL_RATE_BLOCK_TRANSFER_TIME__LL_US  (16739172426U)
#define FRACTIONAL_RATE_TIME_Q_FAC 25
/*Below macros will be used to derive IIR resampler BW (used as scaling factor)*/
#define IIR_RS_BW_MULTIPLI_FACTOR       25
#define IIR_RS_BW_DIVISOR_FACTOR        12
/*==========================================================================
Function Definitions
========================================================================== */
capi_v2_err_t capi_v2_event_callback_rs(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_ptr);
/**
 * This function is to select the resampler type for the given
 * case
 *
 * @param[in] resamp_type, requests the sampling rate converter type to be used
 * @param[in] in_freq, input frequency for the resampler.
 * @param[in] out_freq, output frequency for the resampler.
 * @param[in] bytes_per_sample, number of bytes per sample (Min.
 *  		of client and port bytes per sample)
 * @return samp_rate_conv_type_t, returns resampler type
 * */

samp_rate_conv_type_t get_samp_rate_conv_type(afe_client_resampler_type resamp_type, uint32_t in_freq, uint32_t out_freq, uint32_t  bytes_per_sample)
{
  samp_rate_conv_type_t resampler_type = FIR_BASED_RESAMPLER;

  if(((IIR_BASED_SRC == resamp_type)) && (TWO_BYTES_PER_SAMPLE == bytes_per_sample)
      && ((AFE_PORT_SAMPLE_RATE_8K == in_freq) || (AFE_PORT_SAMPLE_RATE_16K == in_freq)
          || (AFE_PORT_SAMPLE_RATE_48K == in_freq) || (AFE_PORT_SAMPLE_RATE_24K == in_freq) 
          || (AFE_PORT_SAMPLE_RATE_32K == in_freq))
          && ((AFE_PORT_SAMPLE_RATE_8K == out_freq) || (AFE_PORT_SAMPLE_RATE_16K == out_freq)
              || (AFE_PORT_SAMPLE_RATE_48K == out_freq) || (AFE_PORT_SAMPLE_RATE_24K == out_freq)
              || (AFE_PORT_SAMPLE_RATE_32K == out_freq)))
  {
    resampler_type = IIR_BASED_RESAMPLER;
  }

  return resampler_type;
}

/**
* This function is the wrapper function for the initialization
* of IIR resampler (Sample rate converters)
*
* @param[in] pClientInfo, pointer to the client instance
* @param[in] bit_width, number of bits per sample (Min.
*     of client and port bits per sample)
* @param[in] in_freq, input frequency for the resampler.
* @param[in] out_freq, output frequency for the resampler.
* @param[in] samp_rate_conv, pointer to sample rate
*     converter (resampler) instance.
* @return ADSPResult, returns success or failure of the
*       initialization
*       Caller should take care of deallocating any memory
*       created for this API call in case of failure
* */
ADSPResult sample_rate_conv_init_iir(samp_rate_conv_t *samp_rate_conv, uint16_t num_channels,  uint32_t  bit_width, uint32_t in_freq, uint32_t out_freq)
{
  capi_v2_err_t result = CAPI_V2_EOK;
  void *iResample_inst = NULL;
  adsp_amdb_module_handle_info_t module_handle_info_ptr[1];
  module_handle_info_ptr[0].interface_type = CAPI_V2;
  module_handle_info_ptr[0].type = AMDB_MODULE_TYPE_GENERIC;
  module_handle_info_ptr[0].id1 = VOICE_MODULE_IIR_RESAMPLER;
  module_handle_info_ptr[0].id2 = 0;
  module_handle_info_ptr[0].h.capi_v2_handle = NULL;
  module_handle_info_ptr[0].result = ADSP_EFAILED;

  capi_v2_init_memory_requirement_t memory_req = { 0 };
  capi_v2_prop_t static_props[] =
  {
      { CAPI_V2_INIT_MEMORY_REQUIREMENT,
          { reinterpret_cast<int8_t*>(&memory_req), 0, sizeof(memory_req) },
          { FALSE, FALSE, 0 }
      },
  };
  capi_v2_proplist_t static_proplist =
  { SIZE_OF_ARRAY(static_props), static_props };

  capi_v2_heap_id_t heap_id = { 0 };
  capi_v2_event_callback_info_t cb_info = { capi_v2_event_callback_rs, (void *)samp_rate_conv };
  capi_v2_port_num_info_t port_info = { 1, 1 };
  capi_v2_prop_t init_props[] =
  {
      { CAPI_V2_HEAP_ID,
          { reinterpret_cast<int8_t*>(&heap_id), sizeof(heap_id), sizeof(heap_id) },
          { FALSE, FALSE, 0 }
      },
      { CAPI_V2_EVENT_CALLBACK_INFO,
          { reinterpret_cast<int8_t*>(&cb_info), sizeof(cb_info), sizeof(cb_info) },
          { FALSE, FALSE, 0 }
      },
      { CAPI_V2_PORT_NUM_INFO,
          { reinterpret_cast<int8_t*>(&port_info), sizeof(port_info), sizeof(port_info) },
          { FALSE, FALSE, 0 }
      }
  };
  capi_v2_proplist_t init_proplist =
  { SIZE_OF_ARRAY(init_props), init_props };

  struct
  {
    capi_v2_data_format_header_t d;
    capi_v2_standard_data_format_t f;
  } media_format;

  media_format.d.data_format = CAPI_V2_FIXED_POINT;
  media_format.f.bits_per_sample = (bit_width > AFE_BITS_PER_SAMPLE_16)? AFE_BITS_PER_SAMPLE_32: AFE_BITS_PER_SAMPLE_16;
  media_format.f.num_channels = num_channels;
  media_format.f.bitstream_format = CAPI_V2_FIXED_POINT;
  media_format.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
  media_format.f.q_factor = elite_data_get_q_format_factor(bit_width);
  media_format.f.data_is_signed = TRUE;
  media_format.f.sampling_rate = in_freq;

  capi_v2_prop_t prop =
  { CAPI_V2_INPUT_MEDIA_FORMAT, /* capi_v2_property_id_t */
      /* capi_v2_buf_t */
      { reinterpret_cast<int8_t*>(&media_format), sizeof(media_format), sizeof(media_format) },
      /* capi_v2_port_info_t*/
      {
          TRUE,
          TRUE, 0,
      }
  };
  capi_v2_proplist_t proplist =
  { 1, &prop };

  adsp_amdb_get_modules_request(module_handle_info_ptr, 1, NULL, NULL);

  if(ADSP_FAILED(module_handle_info_ptr[0].result) ||
     (CAPI_V2 != module_handle_info_ptr[0].interface_type))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"IIR Resampler Module not present in AMDB");
    return ADSP_EFAILED;
  }

  result = adsp_amdb_capi_v2_get_static_properties_f(module_handle_info_ptr[0].h.capi_v2_handle,
                                                      NULL, &static_proplist);
  if(CAPI_V2_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"IIR type dynamic resampler get static properties failed");
    adsp_amdb_release_handles(module_handle_info_ptr,1);
    return ADSP_EFAILED;
  }

  /*Allocating resampler module memory with the starting address is 8-byte aligned*/
  iResample_inst = qurt_elite_memory_aligned_malloc(memory_req.size_in_bytes, 8,
                                                    QURT_ELITE_HEAP_DEFAULT);
  if(NULL == iResample_inst)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for Generic resampler instance memory");
    adsp_amdb_release_handles(module_handle_info_ptr,1);
    return ADSP_ENOMEMORY;
  }
  samp_rate_conv->resample_instance_ptr = iResample_inst;
  memset(iResample_inst, 0, memory_req.size_in_bytes);

  // Set up init properties
  result = adsp_amdb_capi_v2_init_f(module_handle_info_ptr[0].h.capi_v2_handle,
                                    (capi_v2_t *)samp_rate_conv->resample_instance_ptr,
                                    &init_proplist);
  if(CAPI_V2_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "IIR resampler init failed");
    qurt_elite_memory_aligned_free(samp_rate_conv->resample_instance_ptr);
    adsp_amdb_release_handles(module_handle_info_ptr,1);
    return ADSP_EFAILED;
  }

  adsp_amdb_release_handles(module_handle_info_ptr,1);

  capi_v2_t *resampler_ptr;
  resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;

  capi_v2_port_info_t capiv2_rs_port_info;
  capiv2_rs_port_info.is_valid = FALSE;

  input_frame_size_t input_frame_size;
  input_frame_size.input_frame_size_us = ((samp_rate_conv->frame_size_in*1000000)/in_freq);

  capi_v2_buf_t rs_buf;
  rs_buf.data_ptr = (int8_t*)&input_frame_size;
  rs_buf.actual_data_len = sizeof(input_frame_size_t);
  rs_buf.max_data_len = sizeof(input_frame_size_t);

  if(NULL != resampler_ptr->vtbl_ptr)
  {
    result = resampler_ptr->vtbl_ptr->set_param(resampler_ptr,
                                                 CAPI_V2_IIR_RESAMPLER_INPUT_FRAME_SIZE,
                                                 &capiv2_rs_port_info, &rs_buf);

    if(CAPI_V2_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: IIR resampler failed setParam to send input frame size.");
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }

  if(NULL != resampler_ptr->vtbl_ptr)
  {
    result = resampler_ptr->vtbl_ptr->set_properties(resampler_ptr, &proplist);
    if(CAPI_V2_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: failed to set property for input media format");
      result = ADSP_EFAILED;
      goto __bailout;
    }

    media_format.f.sampling_rate = out_freq;

    prop.id = CAPI_V2_OUTPUT_MEDIA_FORMAT;
    prop.port_info.is_input_port = FALSE;
    prop.port_info.is_valid = TRUE;
    prop.port_info.port_index = 0;

    result = resampler_ptr->vtbl_ptr->set_properties(resampler_ptr, &proplist);
    if(CAPI_V2_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: failed to set property for output media format");
      result = ADSP_EFAILED;
      goto __bailout;
    }
  }
  return ADSP_EOK;

__bailout:
  /* release the capiv2 resampler memory and return error code */
  sample_rate_conv_deinit(samp_rate_conv);
  return result;
}
/**
 * This function is the wrapper function for the initialization
 * of various resamplers (Sample rate converters)
 *
 * @param[in] pClientInfo, pointer to the client instance
 * @param[in] bit_width, number of bits per sample (Min.
 *  		of client and port bits per sample)
 * @param[in] in_freq, input frequency for the resampler.
 * @param[in] out_freq, output frequency for the resampler.
 * @param[in] samp_rate_conv, pointer to sample rate
 *  		converter (resampler) instance.
 * @return ADSPResult, retuns success or failure of the
 *  		  initialization
 *  		  Caller should take care of deallocating any memory
 *  		  created for this API call in case of failure
 * */
ADSPResult sample_rate_conv_init(samp_rate_conv_t *samp_rate_conv, uint16_t num_channels,  uint32_t  bit_width, 
                                 uint32_t in_freq, uint32_t out_freq, samp_rate_conv_type_t resamp_type)
{
  ADSPResult result = ADSP_EOK;
  capi_v2_err_t result2 = CAPI_V2_EOK;
  void *iResample_inst = NULL;
  uint32_t bytes_per_sample = 0;

  /* Checking if the instance memory is already allocated
   * then deinit the existing resampler
   */
  if(NULL != samp_rate_conv->resample_instance_ptr)
  {
    sample_rate_conv_deinit(samp_rate_conv);
  }

  bytes_per_sample = (bit_width > AFE_BITS_PER_SAMPLE_16)? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
  switch(resamp_type)
  {
    case FIR_BASED_RESAMPLER:
    {
      /* Check for fractional resampling, only FIR resampler supports it */
      if(afe_is_fractional_resampling_needed(in_freq, out_freq))
      {
        if(afe_is_sample_rate_fract(in_freq))
        {
          samp_rate_conv->buff_scheme = RS_INPUT_BUFFERING;
          samp_rate_conv->rs_buff_ch_spacing_in_bytes = bytes_per_sample
              * samp_rate_conv->frame_size_in * RS_INTERNAL_BUFFER_SCALING;
        }
        else
        {
          samp_rate_conv->buff_scheme = RS_OUTPUT_BUFFERING;
          samp_rate_conv->rs_buff_ch_spacing_in_bytes = bytes_per_sample
              * samp_rate_conv->frame_size_out * RS_INTERNAL_BUFFER_SCALING;
        }
      }

      void *buff_ptr = NULL;

      adsp_amdb_module_handle_info_t module_handle_info_ptr[1];
      module_handle_info_ptr[0].interface_type = CAPI_V2;
      module_handle_info_ptr[0].type = AMDB_MODULE_TYPE_GENERIC;
      module_handle_info_ptr[0].id1 = AUDPROC_MODULE_ID_RESAMPLER;
      module_handle_info_ptr[0].id2 = 0;
      module_handle_info_ptr[0].h.capi_v2_handle = NULL;
      module_handle_info_ptr[0].result = ADSP_EFAILED;

      adsp_amdb_get_modules_request(module_handle_info_ptr, 1, NULL, NULL);

      if(ADSP_FAILED(module_handle_info_ptr[0].result) ||
         (CAPI_V2 != module_handle_info_ptr[0].interface_type))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"FIR Resampler Module not present in AMDB");
        return ADSP_EFAILED;
      }

      capi_v2_init_memory_requirement_t memory_req = { 0 };
      capi_v2_prop_t static_props[] =
      {
          { CAPI_V2_INIT_MEMORY_REQUIREMENT,
              { reinterpret_cast<int8_t*>(&memory_req), 0, sizeof(memory_req) },
              { FALSE, FALSE, 0 }
          },
      };
      capi_v2_proplist_t static_proplist =
      { SIZE_OF_ARRAY(static_props), static_props };

      result2 = adsp_amdb_capi_v2_get_static_properties_f(module_handle_info_ptr[0].h.capi_v2_handle ,
                                                          NULL, &static_proplist);
      if(result2 != ADSP_EOK)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"FIR type dynamic resampler get static properties failed. checking for IIR type");
        adsp_amdb_release_handles(module_handle_info_ptr,1);
        goto __generic_resampler_error_bailout;
      }

      /*Allocating resampler module memory with the starting address is 8-byte aligned*/
      iResample_inst = qurt_elite_memory_aligned_malloc(memory_req.size_in_bytes, 8,
                                                        QURT_ELITE_HEAP_DEFAULT);
      if(NULL == iResample_inst)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for Generic resampler instance memory");
        adsp_amdb_release_handles(module_handle_info_ptr,1);
        return ADSP_ENOMEMORY;
      }
      samp_rate_conv->resample_instance_ptr = iResample_inst;
      memset(iResample_inst, 0, memory_req.size_in_bytes);

      // Set up init properties

      capi_v2_heap_id_t heap_id =
      { 0 };
      capi_v2_event_callback_info_t cb_info =
      { capi_v2_event_callback_rs, (void *)samp_rate_conv };

      capi_v2_prop_t init_props[] =
      {
          { CAPI_V2_HEAP_ID,
              { reinterpret_cast<int8_t*>(&heap_id), sizeof(heap_id), sizeof(heap_id) },
              { FALSE, FALSE, 0 } },
              { CAPI_V2_EVENT_CALLBACK_INFO,
                  { reinterpret_cast<int8_t*>(&cb_info), sizeof(cb_info), sizeof(cb_info) },
                  { FALSE, FALSE, 0 } } };
      capi_v2_proplist_t init_proplist =
      { SIZE_OF_ARRAY(init_props), init_props };

      result = adsp_amdb_capi_v2_init_f(module_handle_info_ptr[0].h.capi_v2_handle,
                                       (capi_v2_t *)samp_rate_conv->resample_instance_ptr,
                                       &init_proplist);
      if(result2 != ADSP_EOK)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FIR dynamic resampler init failed");
        qurt_elite_memory_aligned_free(samp_rate_conv->resample_instance_ptr);
        adsp_amdb_release_handles(module_handle_info_ptr,1);
        result = ADSP_EFAILED;
        goto __generic_resampler_error_bailout;
      }

      adsp_amdb_release_handles(module_handle_info_ptr,1);

      capi_v2_t *resampler_ptr;
      resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;

      //DISABLE USING HARDWARE RS FOR BOTH THE RESAMPLERS
      capi_v2_port_info_t capiv2_rs_port_info;
      capiv2_rs_port_info.is_valid = FALSE;

      param_id_dynamic_rs_force_disable_hw_rs_t capi_v2_rs_disable;
      capi_v2_rs_disable.force_disable = TRUE;

      capi_v2_buf_t rs_buf;
      rs_buf.data_ptr = (int8_t*)&capi_v2_rs_disable;
      rs_buf.actual_data_len = sizeof(param_id_dynamic_rs_force_disable_hw_rs_t);
      rs_buf.max_data_len = sizeof(param_id_dynamic_rs_force_disable_hw_rs_t);

      if(NULL != resampler_ptr->vtbl_ptr)
      {
        result2 = resampler_ptr->vtbl_ptr->set_param(resampler_ptr,
                                                     AUDPROC_PARAM_ID_RESAMPLER_FORCE_DISABLE_HW_RS,
                                                     &capiv2_rs_port_info, &rs_buf);

        if(CAPI_V2_FAILED(result2))
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: FIR resampler failed setParam to disable HW.");
          result = ADSP_EFAILED;
          goto __capiv2_resampler_error_bailout;
        }
      }

      struct
      {
        capi_v2_data_format_header_t d;
        capi_v2_standard_data_format_t f;
      } media_format;

      media_format.d.data_format = CAPI_V2_FIXED_POINT;
      media_format.f.bits_per_sample = (bit_width > AFE_BITS_PER_SAMPLE_16)? AFE_BITS_PER_SAMPLE_32 : AFE_BITS_PER_SAMPLE_16 ; 
      media_format.f.num_channels = num_channels;
      media_format.f.bitstream_format = CAPI_V2_FIXED_POINT;
      media_format.f.data_interleaving = CAPI_V2_DEINTERLEAVED_UNPACKED;
      media_format.f.q_factor = elite_data_get_q_format_factor(bit_width);
      media_format.f.data_is_signed = TRUE;
      media_format.f.sampling_rate = in_freq;

      capi_v2_prop_t prop =
      { CAPI_V2_INPUT_MEDIA_FORMAT, /* capi_v2_property_id_t */
          /* capi_v2_buf_t */
          { reinterpret_cast<int8_t*>(&media_format), sizeof(media_format), sizeof(media_format) },
          /* capi_v2_port_info_t*/
          {
              TRUE,
              TRUE, 0,
          }
      };
      capi_v2_proplist_t proplist =
      { 1, &prop };

      if(NULL != resampler_ptr->vtbl_ptr)
      {
        result2 = resampler_ptr->vtbl_ptr->set_properties(resampler_ptr, &proplist);
        if(CAPI_V2_FAILED(result2))
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: failed to set property for input media format");
          result = ADSP_EFAILED;
          goto __capiv2_resampler_error_bailout;
        }

        media_format.f.sampling_rate = out_freq;

        prop.id = CAPI_V2_OUTPUT_MEDIA_FORMAT;
        prop.port_info.is_input_port = FALSE;
        prop.port_info.is_valid = TRUE;
        prop.port_info.port_index = 0;

        result2 = resampler_ptr->vtbl_ptr->set_properties(resampler_ptr, &proplist);
        if(CAPI_V2_FAILED(result2))
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: failed to set property for output media format");
          result = ADSP_EFAILED;
          goto __capiv2_resampler_error_bailout;
        }
        samp_rate_conv->resamp_type = FIR_BASED_RESAMPLER;
      }

      if(RS_NO_BUFFERING != samp_rate_conv->buff_scheme)
      {
        /* After successful init of the resampler, allocate the internal buffer */
        buff_ptr = qurt_elite_memory_aligned_malloc(
            (samp_rate_conv->rs_buff_ch_spacing_in_bytes * num_channels), 8,
            QURT_ELITE_HEAP_DEFAULT);

        if(NULL == buff_ptr)
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for frac resampling buffer!");
          return ADSP_ENOMEMORY;
        }

        /* It needs to be filled with Zeros initially, as it will be drained out during first intr */
        memset(buff_ptr, 0, samp_rate_conv->rs_buff_ch_spacing_in_bytes * num_channels);
        samp_rate_conv->rs_buff_ptr = (int8_t *)buff_ptr;

        if(RS_OUTPUT_BUFFERING == samp_rate_conv->buff_scheme)
        {
          samp_rate_conv->rs_buffered_samples_in_bytes = 0;
        }
        else if(RS_INPUT_BUFFERING == samp_rate_conv->buff_scheme)
        {
          samp_rate_conv->rs_buffered_samples_in_bytes =
              samp_rate_conv->rs_buff_ch_spacing_in_bytes / RS_INTERNAL_BUFFER_SCALING;
        }
      }
    }
    break;
    case IIR_BASED_RESAMPLER:
    {
      result = sample_rate_conv_init_iir(samp_rate_conv, num_channels, bit_width, in_freq,
                                         out_freq);
      if(ADSP_EOK != result)
      {
         return result;
      }
      samp_rate_conv->resamp_type = IIR_BASED_RESAMPLER;
    }
    break;
    default:
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported resampler type");
      return ADSP_EUNSUPPORTED;
    }
    break;
  }
  samp_rate_conv->bytes_per_sample = bytes_per_sample;

  return result;

  __generic_resampler_error_bailout:
  /* Come to bailout if FIR resampler is not present. Resampler is changed to IIR only if the below conditions are met. */
  if(IIR_BASED_RESAMPLER
      == get_samp_rate_conv_type(IIR_BASED_SRC, in_freq, out_freq, bytes_per_sample))
  {
    result = sample_rate_conv_init_iir(samp_rate_conv, num_channels, bit_width, in_freq,
                                       out_freq);
    if(ADSP_EOK != result)
    {
       return result;
    }
    samp_rate_conv->resamp_type = IIR_BASED_RESAMPLER;
    return result;
  }
  else
  {
    return ADSP_EFAILED;
  }

  __capiv2_resampler_error_bailout:
  /* release the capiv2 resampler memory and return error code */
  sample_rate_conv_deinit(samp_rate_conv);
  return result;
}

/**
 * THis function is to clear the data memory of voice resampler
 * @param[in] resampler, pointer to instance of the resampler
 * @param[in] num_channels, number of channels
 * return ADSPResult
 */
ADSPResult sample_rate_conv_memory_init(samp_rate_conv_t *samp_rate_conv, uint16_t num_channels)
{
  capi_v2_err_t result = CAPI_V2_EOK;
  capi_v2_t *resampler_ptr;
  resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;
  capi_v2_prop_t props[] =
  {
      { CAPI_V2_ALGORITHMIC_RESET,
          {},
          { FALSE, FALSE, 0 }
      }
  };
  capi_v2_proplist_t proplist =
  { SIZE_OF_ARRAY(props), props };

  if(NULL != resampler_ptr->vtbl_ptr)
  {
    result = resampler_ptr->vtbl_ptr->set_properties(resampler_ptr,
                                                     &proplist);

    if(CAPI_V2_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE sample rate conv: IIR resampler failed setParam to reset algo.");
      return ADSP_EFAILED;
    }
  }
  return ADSP_EOK;
}
/**
 * This function is for freeing the memory created for the
 * resampler.
 *
 * @param[in] resampler, pointer to instance of the resampler
 * @param[in] resampler_memory, pointer to instance voice
 *  		resampler memory.
 * @param[in] resamplerType, information about the type of the
 *  		resampler.
 * @return none
 */
void sample_rate_conv_deinit(samp_rate_conv_t *samp_rate_conv)
{
  capi_v2_t *resampler_ptr = NULL;
  if(!samp_rate_conv)
  {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sample rate conv already deinited ");
    return;
  }
  if(NULL != samp_rate_conv->resample_instance_ptr)
  {
    resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;
    if(NULL != resampler_ptr->vtbl_ptr)
    {
      resampler_ptr->vtbl_ptr->end(resampler_ptr);
    }

    if(samp_rate_conv->rs_buff_ptr)
    {
      qurt_elite_memory_aligned_free(samp_rate_conv->rs_buff_ptr);
      samp_rate_conv->rs_buff_ptr = NULL;
    }

    qurt_elite_memory_aligned_free(samp_rate_conv->resample_instance_ptr);
    samp_rate_conv->resample_instance_ptr = NULL;
    samp_rate_conv->resamp_type = INVALID_RESAMP_TYPE;

    samp_rate_conv->buff_scheme = RS_NO_BUFFERING;
    samp_rate_conv->rs_buff_ch_spacing_in_bytes = 0;
    samp_rate_conv->rs_buffered_samples_in_bytes = 0;
    samp_rate_conv->rs_buff_flag = RS_NORMAL;
    samp_rate_conv->rs_num_process = 0;
    samp_rate_conv->rs_buff_delay_us = 0;
    samp_rate_conv->delay_in_us = 0;
  }
}

/**
 * This function is the wrapper function for output buffering fractional rs process
 *
 * @param[in] samp_rate_conv, resampler strcture
 * @param[in] in_buf, input buffer for the resampler.
 * @param[in] out_buf, output buffer for the resampler.
 * @param[in] nchan, number of channels.
 * @param[out] samples_processed, actual number of samples
 *        processed by resampler.
 * @return ADSPResult
 */
static ADSPResult fir_rs_output_buffering_process(samp_rate_conv_t *samp_rate_conv, int8 *in_buf, int8 *out_buf,
                                                  uint32 *samples_processed_ptr, uint16_t nChan)
{
  int8_t        *rs_out_ptr;
  int8_t        *read_ptr;
  capi_v2_err_t resampler_result        = CAPI_V2_EOK;
  capi_v2_t     *resampler_ptr          = NULL;
  uint16_t      i                       = 0;
  int8_t        *extra_copy_start_ptr   = NULL;
  uint32_t      bytes_per_sample        = samp_rate_conv->bytes_per_sample;
  uint32_t      input_chan_spacing      = samp_rate_conv->in_ch_spacing_in_bytes;
  uint32_t      output_chan_spacing     = samp_rate_conv->out_ch_spacing_in_bytes;
  uint32_t      rs_buff_ch_spacing      = samp_rate_conv->rs_buff_ch_spacing_in_bytes;
  uint32_t      fr_size_out_bytes       = samp_rate_conv->frame_size_out * bytes_per_sample;
  uint32_t      fr_size_in_bytes        = samp_rate_conv->frame_size_in * bytes_per_sample;
  int32_t       *buff_samples_bytes_ptr = &(samp_rate_conv->rs_buffered_samples_in_bytes);
  uint64_t      block_tf_time_us        = FRACTIONAL_RATE_BLOCK_TRANSFER_TIME_US;

  if(AFE_PORT_LOW_LATENCY_MODE == samp_rate_conv->port_latency_mode)
  {
    block_tf_time_us = FRACTIONAL_RATE_BLOCK_TRANSFER_TIME__LL_US;
  }

  /* For RS_OUTPUT_BUFFERING
   * -> Copy frame_size_out from port_buffer to out_ptr.
   * -> Copy extra sample if present in rs_buffer to port_buffer
   * -> Call Resampler process with rs_buffer as output / input
   * -> Copy remaining samples needed to fill port_buffer for next interrupt.
   *
   */
  for(i = 0; i < nChan; i++)
  {
    read_ptr = samp_rate_conv->rs_buff_ptr + (i * rs_buff_ch_spacing);

    memscpy(out_buf + (i * output_chan_spacing), fr_size_out_bytes, read_ptr, fr_size_out_bytes);
  }

#ifdef FIR_FILE_DUMP_SIM
  FILE *f_out_1 = fopen("rs_out_1.raw", "ab");
  if(f_out_1)
  {
    fwrite(out_buf, fr_size_out_bytes, 1, f_out_1);
    fclose(f_out_1);
  }

  if(nChan > 1)
  {
    FILE *f_out_2 = fopen("rs_out_2.raw", "ab");
    if(f_out_2)
    {
      fwrite(out_buf + output_chan_spacing, fr_size_out_bytes,
             1, f_out_2);
      fclose(f_out_2);
    }
  }
#endif

  *samples_processed_ptr = samp_rate_conv->frame_size_out;

  /* Now copy extra samples to the top of internal buffer for the next intr */

  for(i = 0; i < nChan; i++)
  {
    extra_copy_start_ptr = samp_rate_conv->rs_buff_ptr + fr_size_out_bytes
        + (i * rs_buff_ch_spacing);

    memscpy(samp_rate_conv->rs_buff_ptr + (i * rs_buff_ch_spacing), *buff_samples_bytes_ptr,
            extra_copy_start_ptr, *buff_samples_bytes_ptr);
  }

#ifdef FIR_DBG
  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Copying extra: %lu init_ptr:%p",
        *buff_samples_bytes_ptr, samp_rate_conv->rs_buff_ptr);
#endif

  if(RS_BUFF_FULL != samp_rate_conv->rs_buff_flag)
  {

#ifdef FIR_FILE_DUMP_SIM
    FILE *f_in_1 = fopen("rs_in_1.raw", "ab");
    if(f_in_1)
    {
      fwrite(in_buf, samp_rate_conv->frame_size_in * bytes_per_sample, 1, f_in_1);
      fclose(f_in_1);
    }

    if(nChan>1)
    {
      FILE *f_in_2 = fopen("rs_in_2.raw", "ab");
      if(f_in_2)
      {
        fwrite(in_buf + (1 * input_chan_spacing),
               samp_rate_conv->frame_size_in * bytes_per_sample, 1, f_in_2);
        fclose(f_in_2);
      }
    }
#endif

    resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;
    capi_v2_stream_data_t inp_buf_stream;
    capi_v2_stream_data_t out_buf_stream;
    capi_v2_stream_data_t *inp_buf_stream_ptr;
    capi_v2_stream_data_t *out_buf_stream_ptr;

    memset(&inp_buf_stream, 0, sizeof(capi_v2_stream_data_t));
    memset(&out_buf_stream, 0, sizeof(capi_v2_stream_data_t));

    capi_v2_buf_t input_buf[nChan];
    capi_v2_buf_t output_buf[nChan];
    memset(&input_buf, 0, nChan * sizeof(capi_v2_buf_t));
    memset(&output_buf, 0, nChan * sizeof(capi_v2_buf_t));

    inp_buf_stream_ptr = &inp_buf_stream;
    inp_buf_stream.bufs_num = nChan;

    for(i = 0; i < nChan; i++)
    {
      input_buf[i].data_ptr = in_buf + (i * input_chan_spacing);
      input_buf[i].actual_data_len = fr_size_in_bytes;
      input_buf[i].max_data_len = fr_size_in_bytes;
    }
    inp_buf_stream.buf_ptr = &input_buf[0];

    out_buf_stream_ptr = &out_buf_stream;
    out_buf_stream.bufs_num = nChan;

    rs_out_ptr = samp_rate_conv->rs_buff_ptr + (*buff_samples_bytes_ptr);

    for(i = 0; i < nChan; i++)
    {
      output_buf[i].data_ptr = rs_out_ptr + (i * rs_buff_ch_spacing);
      output_buf[i].actual_data_len = 0;
      output_buf[i].max_data_len = rs_buff_ch_spacing - (*buff_samples_bytes_ptr);
    }
    out_buf_stream.buf_ptr = &output_buf[0];

    resampler_result = resampler_ptr->vtbl_ptr->process(resampler_ptr, &inp_buf_stream_ptr,
                                                        &out_buf_stream_ptr);

    if(CAPI_V2_EOK != resampler_result)
    {
      return ADSP_EFAILED;
    }

    samp_rate_conv->rs_num_process++;

#ifdef FIR_FILE_DUMP_SIM
FILE *f_rs_out_1 = fopen("frs_out_1.raw", "ab");
if(f_rs_out_1)
{
  fwrite(output_buf[0].data_ptr, output_buf[0].actual_data_len, 1, f_rs_out_1);
  fclose(f_rs_out_1);
}
if(nChan > 1)
{
  FILE *f_rs_out_2 = fopen("frs_out_2.raw", "ab");
  if(f_rs_out_2)
  {
    fwrite(output_buf[1].data_ptr, output_buf[1].actual_data_len, 1, f_rs_out_2);
    fclose(f_rs_out_2);
  }
}
#endif

*buff_samples_bytes_ptr += (output_buf[0].actual_data_len - fr_size_out_bytes);

/* We should have buffered one full frame at the rs process calls boundary */
if(FRACTIONAL_RS_PROCESS_BOUNDARY == samp_rate_conv->rs_num_process)
{
  samp_rate_conv->rs_buff_flag = RS_BUFF_FULL;

  /* Error check */
  if((*buff_samples_bytes_ptr) != (rs_buff_ch_spacing / RS_INTERNAL_BUFFER_SCALING))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error Input buffering: buff_bytes: %llu expected_bytes: %llu",
          *buff_samples_bytes_ptr, rs_buff_ch_spacing / RS_INTERNAL_BUFFER_SCALING);
  }
}

#ifdef FIR_DBG
uint64_t cur_time = qurt_elite_timer_get_time();
MSG_7(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
      "Time: %llu Max Input: %lu Actual Input: %lu "
      "Max Output size: %lu Actual Out: %lu Track Size: %lu Process: %lu",
      cur_time, input_buf[0].max_data_len, input_buf[0].actual_data_len,
      output_buf[0].max_data_len, output_buf[0].actual_data_len,
      (*buff_samples_bytes_ptr), samp_rate_conv->rs_num_process);
#endif

  }
  else if(RS_BUFF_FULL == samp_rate_conv->rs_buff_flag)
  {
    samp_rate_conv->rs_buff_flag = RS_NORMAL;
    *buff_samples_bytes_ptr = 0;
    samp_rate_conv->rs_num_process = 0;

#ifdef FIR_DBG
    uint64_t cur_time = qurt_elite_timer_get_time();
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RS_BUFF_FULL for RS at: %llu", cur_time);
#endif

  }

  samp_rate_conv->rs_buff_delay_us = ((((*buff_samples_bytes_ptr) / bytes_per_sample)
      * block_tf_time_us) >> FRACTIONAL_RATE_TIME_Q_FAC)/(samp_rate_conv->frame_size_out);

  return ADSP_EOK;
}

/**
 * This function is the wrapper function for input buffering fractional rs process
 *
 * @param[in] samp_rate_conv, resampler strcture
 * @param[in] in_buf, input buffer for the resampler.
 * @param[in] out_buf, output buffer for the resampler.
 * @param[in] nchan, number of channels.
 * @param[out] samples_processed, actual number of samples
 *        processed by resampler.
 * @return ADSPResult
 */
static ADSPResult fir_rs_input_buffering_process(samp_rate_conv_t *samp_rate_conv, int8 *in_buf, int8 *out_buf,
                                                 uint32 *samples_processed_ptr, uint16_t nChan)
{
  int8_t        *read_ptr;
  capi_v2_err_t resampler_result        = CAPI_V2_EOK;
  capi_v2_t     *resampler_ptr          = NULL;
  uint16_t      i                       = 0;
  uint32_t      bytes_per_sample        = samp_rate_conv->bytes_per_sample;
  uint32_t      input_chan_spacing      = samp_rate_conv->in_ch_spacing_in_bytes;
  uint32_t      output_chan_spacing     = samp_rate_conv->out_ch_spacing_in_bytes;
  uint32_t      rs_buff_ch_spacing      = samp_rate_conv->rs_buff_ch_spacing_in_bytes;
  uint32_t      fr_size_out_bytes       = samp_rate_conv->frame_size_out * bytes_per_sample;
  uint32_t      fr_size_in_bytes        = samp_rate_conv->frame_size_in * bytes_per_sample;
  int32_t       *buff_samples_bytes_ptr = &(samp_rate_conv->rs_buffered_samples_in_bytes);
  uint64_t      block_tf_time_us        = FRACTIONAL_RATE_BLOCK_TRANSFER_TIME_US;

  if(AFE_PORT_LOW_LATENCY_MODE == samp_rate_conv->port_latency_mode)
  {
    block_tf_time_us = FRACTIONAL_RATE_BLOCK_TRANSFER_TIME__LL_US;
  }

  /* For RS_INTPUT_BUFFERING
   * -> Copy the incoming buffer to internal buffer
   * -> Call the RS process
   * -> Copy old samples to the top for next intr processing
   */

  for(i = 0; i < nChan; i++)
  {
    read_ptr = samp_rate_conv->rs_buff_ptr + (*buff_samples_bytes_ptr) + (i * rs_buff_ch_spacing);

    memscpy(read_ptr, fr_size_in_bytes, in_buf + (i * input_chan_spacing), fr_size_in_bytes);
  }

#ifdef FIR_FILE_DUMP_SIM
  /*Capture input */
  FILE *f_in_1 = fopen("rs1_in_1.raw", "ab");
  if(f_in_1)
  {
    fwrite(in_buf, samp_rate_conv->frame_size_in * bytes_per_sample, 1, f_in_1);
    fclose(f_in_1);
  }
  if(nChan > 1)
  {
    FILE *f_in_2 = fopen("rs1_in_2.raw", "ab");
    if(f_in_2)
    {
      fwrite(in_buf + input_chan_spacing, samp_rate_conv->frame_size_in * bytes_per_sample, 1,
             f_in_2);
      fclose(f_in_2);
    }
  }
#endif

  if(RS_BUFF_EMPTY != samp_rate_conv->rs_buff_flag)
  {
    /* RS reads samples always from the top of the internal buffer */

    int32_t extra_bytes_read = 0;
    resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;
    capi_v2_stream_data_t inp_buf_stream;
    capi_v2_stream_data_t out_buf_stream;
    capi_v2_stream_data_t *inp_buf_stream_ptr;
    capi_v2_stream_data_t *out_buf_stream_ptr;

    memset(&inp_buf_stream, 0, sizeof(capi_v2_stream_data_t));
    memset(&out_buf_stream, 0, sizeof(capi_v2_stream_data_t));

    capi_v2_buf_t input_buf[nChan];
    capi_v2_buf_t output_buf[nChan];
    memset(&input_buf, 0, nChan * sizeof(capi_v2_buf_t));
    memset(&output_buf, 0, nChan * sizeof(capi_v2_buf_t));

    inp_buf_stream_ptr = &inp_buf_stream;
    inp_buf_stream.bufs_num = nChan;

    for(i = 0; i < nChan; i++)
    {
      input_buf[i].data_ptr = samp_rate_conv->rs_buff_ptr + (i * rs_buff_ch_spacing);
      input_buf[i].actual_data_len = fr_size_in_bytes + (*buff_samples_bytes_ptr);
      input_buf[i].max_data_len = fr_size_in_bytes + (*buff_samples_bytes_ptr);
    }
    inp_buf_stream.buf_ptr = &input_buf[0];

    out_buf_stream_ptr = &out_buf_stream;
    out_buf_stream.bufs_num = nChan;

    for(i = 0; i < nChan; i++)
    {
      output_buf[i].data_ptr = out_buf + (i * output_chan_spacing);
      output_buf[i].actual_data_len = 0;
      output_buf[i].max_data_len = fr_size_out_bytes;
    }
    out_buf_stream.buf_ptr = &output_buf[0];

    resampler_result = resampler_ptr->vtbl_ptr->process(resampler_ptr, &inp_buf_stream_ptr,
                                                        &out_buf_stream_ptr);

    if(CAPI_V2_EOK != resampler_result)
    {
      return ADSP_EFAILED;
    }

    *samples_processed_ptr = samp_rate_conv->frame_size_out;

    samp_rate_conv->rs_num_process++;

#ifdef FIR_FILE_DUMP_SIM
FILE *f_rs_in_1 = fopen("frs1_in_1.raw", "ab");
if(f_rs_in_1)
{
  fwrite(input_buf[0].data_ptr, input_buf[0].actual_data_len, 1, f_rs_in_1);
  fclose(f_rs_in_1);
}

if(nChan > 1)
{
  FILE *f_rs_in_2 = fopen("frs1_in_2.raw", "ab");
  if(f_rs_in_2)
  {
    fwrite(input_buf[1].data_ptr, input_buf[1].actual_data_len, 1, f_rs_in_2);
    fclose(f_rs_in_2);
  }
}

FILE *f_rs_out_1 = fopen("frs1_out_1.raw", "ab");
if(f_rs_out_1)
{
  fwrite(output_buf[0].data_ptr, output_buf[0].actual_data_len, 1, f_rs_out_1);
  fclose(f_rs_out_1);
}

if(nChan > 1)
{
  FILE *f_rs_out_2 = fopen("frs1_out_2.raw", "ab");
  if(f_rs_out_2)
  {
    fwrite(output_buf[1].data_ptr, output_buf[1].actual_data_len, 1, f_rs_out_2);
    fclose(f_rs_out_2);
  }
}
#endif

extra_bytes_read = input_buf[0].actual_data_len - fr_size_in_bytes;

*buff_samples_bytes_ptr -= extra_bytes_read;

/* If buffered samples reaches 0, then need to read two buffers to refill the internal buffer */
if(FRACTIONAL_RS_PROCESS_BOUNDARY == samp_rate_conv->rs_num_process)
{
  samp_rate_conv->rs_buff_flag = RS_BUFF_EMPTY;

  /* Error check */
  if((*buff_samples_bytes_ptr) != 0)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error Input buffering: buff_bytes: %llu expected_bytes: 0",
          *buff_samples_bytes_ptr);
  }
}

#ifdef FIR_DBG
uint64_t cur_time = qurt_elite_timer_get_time();
MSG_7(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
      "Time: %llu Max Input: %lu Actual Input: %lu Max Output size: %lu Actual Out: %lu Track Size: %lu Process: %lu",
      cur_time, input_buf[0].max_data_len, input_buf[0].actual_data_len,
      output_buf[0].max_data_len, output_buf[0].actual_data_len,
      (*buff_samples_bytes_ptr), samp_rate_conv->rs_num_process);
#endif

/* Now move old samples to the Top for next intr processing */
for(i = 0; i < nChan; i++)
{
  read_ptr = samp_rate_conv->rs_buff_ptr + (rs_buff_ch_spacing / RS_INTERNAL_BUFFER_SCALING)
                         + extra_bytes_read + (i * rs_buff_ch_spacing);

  memscpy(samp_rate_conv->rs_buff_ptr + (i * rs_buff_ch_spacing), (*buff_samples_bytes_ptr),
          read_ptr, (*buff_samples_bytes_ptr));
}
  }
  else if(RS_BUFF_EMPTY == samp_rate_conv->rs_buff_flag)
  {
    samp_rate_conv->rs_buff_flag = RS_NORMAL;
    samp_rate_conv->rs_num_process = 0;
    *buff_samples_bytes_ptr = (rs_buff_ch_spacing / RS_INTERNAL_BUFFER_SCALING);
    /* As zero samples were generated in this */
    *samples_processed_ptr = 0;

#ifdef FIR_DBG
    uint64_t cur_time = qurt_elite_timer_get_time();
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RS_BUFF_EMPTY for RS at: %llu", cur_time);
#endif

  }

  samp_rate_conv->rs_buff_delay_us = ((((*buff_samples_bytes_ptr) / bytes_per_sample)
      * block_tf_time_us) >> FRACTIONAL_RATE_TIME_Q_FAC)/(samp_rate_conv->frame_size_in);

  return ADSP_EOK;
}

/**
 * This function is the wrapper function for no buffering in case of non-fractional rs process
 *
 * @param[in] samp_rate_conv, resampler strcture
 * @param[in] in_buf, input buffer for the resampler.
 * @param[in] out_buf, output buffer for the resampler.
 * @param[in] nchan, number of channels.
 * @param[out] samples_processed, actual number of samples
 *        processed by resampler.
 * @return ADSPResult
 */
static ADSPResult rs_no_buffering_process(samp_rate_conv_t *samp_rate_conv, int8 *in_buf, int8 *out_buf,
                                              uint32 *samples_processed_ptr, uint16_t nChan)
{
  capi_v2_err_t resampler_result    = CAPI_V2_EOK;
  capi_v2_t     *resampler_ptr      = NULL;
  uint16_t      i                   = 0;
  uint32_t      bytes_per_sample    = samp_rate_conv->bytes_per_sample;
  uint32_t      input_chan_spacing  = samp_rate_conv->in_ch_spacing_in_bytes;
  uint32_t      output_chan_spacing = samp_rate_conv->out_ch_spacing_in_bytes;
  uint32_t      fr_size_out_bytes   = samp_rate_conv->frame_size_out * bytes_per_sample;
  uint32_t      fr_size_in_bytes    = samp_rate_conv->frame_size_in * bytes_per_sample;

  resampler_ptr = (capi_v2_t *)samp_rate_conv->resample_instance_ptr;
  capi_v2_stream_data_t inp_buf_stream;
  capi_v2_stream_data_t out_buf_stream;
  capi_v2_stream_data_t *inp_buf_stream_ptr;
  capi_v2_stream_data_t *out_buf_stream_ptr;

  memset(&inp_buf_stream, 0, sizeof(capi_v2_stream_data_t));
  memset(&out_buf_stream, 0, sizeof(capi_v2_stream_data_t));

  capi_v2_buf_t input_buf[nChan];
  capi_v2_buf_t output_buf[nChan];
  memset(&input_buf, 0, nChan * sizeof(capi_v2_buf_t));
  memset(&output_buf, 0, nChan * sizeof(capi_v2_buf_t));

  inp_buf_stream_ptr = &inp_buf_stream;
  inp_buf_stream.bufs_num = nChan;

  for(i = 0; i < nChan; i++)
  {
    input_buf[i].data_ptr = in_buf + (i * input_chan_spacing);
    input_buf[i].actual_data_len = fr_size_in_bytes;
    input_buf[i].max_data_len = fr_size_in_bytes;
  }
  inp_buf_stream.buf_ptr = &input_buf[0];

  out_buf_stream_ptr = &out_buf_stream;
  out_buf_stream.bufs_num = nChan;

  for(i = 0; i < nChan; i++)
  {
    output_buf[i].data_ptr = out_buf + (i * output_chan_spacing);
    output_buf[i].actual_data_len = 0;
    output_buf[i].max_data_len = fr_size_out_bytes;
  }
  out_buf_stream.buf_ptr = &output_buf[0];

  resampler_result = resampler_ptr->vtbl_ptr->process(resampler_ptr, &inp_buf_stream_ptr,
                                                      &out_buf_stream_ptr);
  if(CAPI_V2_EOK != resampler_result)
  {
    return ADSP_EFAILED;
  }

  *samples_processed_ptr = (output_buf[0].actual_data_len) / bytes_per_sample;

  return ADSP_EOK;
}
/**
 * This function is the wrapper function for the various
 * resamplers (Sample rate converters)
 *
 * @param[in] samp_rate_conv, resampler strcture
 * @param[in] in_buf, input buffer for the resampler.
 * @param[in] out_buf, output buffer for the resampler.
 * @param[in] nchan, number of channels.
 * @param[out] samples_processed, actual number of samples
 *  		processed by resampler.
 * @param[in] mode, resampler mode. *
 * @return ADSPResult
 */
ADSPResult sample_rate_conv_process(samp_rate_conv_t *samp_rate_conv, int8 *in_buf, int8 *out_buf,
                                    uint32 *samples_processed, uint16_t nChan)
{
  ADSPResult result = ADSP_EOK;

  switch(samp_rate_conv->resamp_type)
  {
    case FIR_BASED_RESAMPLER:
    {
      if(RS_OUTPUT_BUFFERING == samp_rate_conv->buff_scheme)
      {
        result = fir_rs_output_buffering_process(samp_rate_conv, in_buf, out_buf, samples_processed, nChan);
      }
      else if(RS_INPUT_BUFFERING == samp_rate_conv->buff_scheme)
      {
        result = fir_rs_input_buffering_process(samp_rate_conv, in_buf, out_buf, samples_processed, nChan);
      }
      else //RS_NO_BUFFERING == samp_rate_conv->buff_scheme
      {
        result = rs_no_buffering_process(samp_rate_conv, in_buf, out_buf, samples_processed, nChan);
      }
    }
    break;
    case IIR_BASED_RESAMPLER:
    {
      result = rs_no_buffering_process(samp_rate_conv, in_buf, out_buf, samples_processed, nChan);
    }
    break;
    default:
      /* return error*/
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported resampler type: %lu", samp_rate_conv->resamp_type);
      return ADSP_EFAILED;
  }

  return result;
}

/**
 * This function is the wrapper function for getting the 
 * algorithm delay of various resamplers (Sample rate converters)
 *  
 * @param[in] samp_rate_conv, pointer to sample rate 
 *  		converter (resampler) instance.
 * @param[out] delay_us, delay of the resampler in micro sec.  
 * */ 
void sample_rate_conv_get_delay(samp_rate_conv_t *samp_rate_conv, uint64_t *delay_us)
{	
  *delay_us = (samp_rate_conv->delay_in_us + samp_rate_conv->rs_buff_delay_us);

  return;
}


capi_v2_err_t capi_v2_event_callback_rs(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_ptr)
{

  samp_rate_conv_t* samp_rate_conv = (samp_rate_conv_t*)(context_ptr);

  switch(id)
  {
    case CAPI_V2_EVENT_KPPS:
    {
      if (event_ptr->payload.actual_data_len < sizeof(capi_v2_event_KPPS_t))
      {
        return CAPI_V2_ENEEDMORE;
      }
      samp_rate_conv->kpps = reinterpret_cast<capi_v2_event_KPPS_t*>(event_ptr->payload.data_ptr)->KPPS;
      break;
    }

    case CAPI_V2_EVENT_BANDWIDTH:
    {
      if (event_ptr->payload.actual_data_len < sizeof(capi_v2_event_KPPS_t))
      {
        return CAPI_V2_ENEEDMORE;
      }
      capi_v2_event_bandwidth_t *bw=reinterpret_cast<capi_v2_event_bandwidth_t*>(event_ptr->payload.data_ptr);
      samp_rate_conv->bw = bw->code_bandwidth + bw->data_bandwidth;
      break;
    }

    case CAPI_V2_EVENT_ALGORITHMIC_DELAY:
    {
      if (event_ptr->payload.actual_data_len < sizeof(capi_v2_event_algorithmic_delay_t))
      {
        return CAPI_V2_ENEEDMORE;
      }
      samp_rate_conv->delay_in_us = reinterpret_cast<capi_v2_event_algorithmic_delay_t*>(event_ptr->payload.data_ptr)->delay_in_us;
      break;
    }
    /* This is added just to avoid unnecessary error message logging in capi_v2 and here*/
    case CAPI_V2_EVENT_OUTPUT_MEDIA_FORMAT_UPDATED:
    {
    }
    break;
    default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFESampleRateconv: callback_rs: Event id %lu is not supported.", static_cast<uint32_t>(id));
      return CAPI_V2_EUNSUPPORTED;
  }
  return CAPI_V2_EOK;
}

/*This function should be called by connecting afe client, when re-sampler is needed.
 * If FIR re-sampler, gets client re-sampler kpps and BW through CAPIv2.
 * If IIR re-sampler, gets client re-sampler kpps from IIR re-sampler predefined table.
 *
 *pDevPort          : Used pDevPort to access mmpm_info_ptr, from which aggregated client kpps/bw is updated/stored
 *psNewClient       : Used psNewClient to store individual client kpps/bw
 * */
void afe_port_aggregate_client_resampler_kpps_bw(void *dev_ptr, void *client_info_ptr)
{
  afe_dev_port_t *pDevPort = (afe_dev_port_t *)dev_ptr;
  afe_client_info_t *psNewClient = (afe_client_info_t *)client_info_ptr;

  if( NULL == pDevPort || NULL == psNewClient)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Port or/and client info pointer is NULL, port pointer:0x%x, client info pointer: 0x%x",
          pDevPort, psNewClient);
    return;
  }

  samp_rate_conv_t *samp_rate_conv = &psNewClient->samp_rate_conv;
  afe_mmpm_info_t *mmpm_info_ptr = (afe_mmpm_info_t *)pDevPort->mmpm_info_ptr;

  if((NULL == mmpm_info_ptr) || (NULL == samp_rate_conv))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "mmpm_info_ptr/samp_rate_conv pointer is/are NULL, mmpm_info_ptr: 0x%x, samp_rate_conv ptr: 0x%x",
          mmpm_info_ptr, samp_rate_conv );
    return;
  }

  if(IIR_BASED_RESAMPLER == samp_rate_conv->resamp_type)
  {
    /*samp_rate_conv->rs_bw  = scaling_factor*common_bw_factor
     *               200000  = scaling_factor*(48000*2*1)           => scaling factor as 25/12
     * */
    samp_rate_conv->bw = (IIR_RS_BW_MULTIPLI_FACTOR * afe_port_get_common_bw_factor(pDevPort)) / IIR_RS_BW_DIVISOR_FACTOR;
  }

  psNewClient->client_kpps += samp_rate_conv->kpps;
  mmpm_info_ptr->dyn_vot_info.agr_client_kpps += samp_rate_conv->kpps;

  psNewClient->client_bw += samp_rate_conv->bw;
  mmpm_info_ptr->dyn_vot_info.agr_client_bw += samp_rate_conv->bw;

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE MPPS:: client_resamp_kpps: %lu, client_resamp_bw: %lu",
        samp_rate_conv->kpps, samp_rate_conv->bw );
}

