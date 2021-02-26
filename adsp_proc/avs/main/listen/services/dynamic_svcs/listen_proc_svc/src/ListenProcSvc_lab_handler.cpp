/**
@file ListenProcSvc_lab_handler.cpp
@brief This file contains api for Look Ahead Buffer
 */

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The core_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      core_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2013-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_lab_handler.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
3/11/14   SivaNaga      Created file.

========================================================================== */
#include "adsp_lsm_api.h"
#include "ListenProcSvc_lab_handler.h"
#include "ListenProcSvc_CustMsg.h"
#include "AFEInterface.h"
#include "EliteMsg_AfeCustom.h"
#include "Elite_fwk_extns_detection_engine.h"
#include "Interleaver.h"
#include "audio_basic_op.h"

#define LSM_API_VERSION_LAB_CONFIG_V1       0x1

/*
  Function for initializing Look Ahead Buffer(LAB)

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t
         sampling_rate [in] sampling rate of LSM session
         bits_per_sample [in] bits per sample of LSM session
         max_kwed_delay_bytes [in] maximum key wod end delay in bytes returned by algorithm
         num_channels [in] number of channels of LSM session
         temp_buf_size [in] temp buffer size for LAB internal requirements
  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_init(lsm_lab_struct_t *lab_struct_ptr,
                                uint32_t sampling_rate,
                                int16_t bits_per_sample,
                                uint32_t max_kwed_delay_bytes,
                                uint16_t num_channels,
                                uint32_t temp_buf_size)
{
  ADSPResult result = ADSP_EOK;
  uint16_t bytes_per_sample = (bits_per_sample>16)?4:2;
  uint32_t total_delay_bytes;
  uint32_t bytes_per_ms = (sampling_rate*bytes_per_sample)/1000;
#ifdef LSM_DEBUG
  MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Llab init:max_kwed_delay_bytes=%lu",
        max_kwed_delay_bytes);
#endif
  /* Populate the max frame delays returned by keyword end detection into dynamic structure
     This values is used to check if the frame delays returned after success detection is less than this value or not*/
  lab_struct_ptr->max_kwed_delay_bytes = max_kwed_delay_bytes;
  /* convert max_kwed_frame_delay and apps_wakeup_latency_ms into samples and sum up to get the circular buffer capacity*/
  total_delay_bytes = lab_struct_ptr->max_kwed_delay_bytes + (lab_struct_ptr->apps_wakeup_latency_ms * bytes_per_ms * num_channels);

  /* circ buf memory depends on apps_wakeup_latency and max_kwed_delay_samples
         So allocating the circ buf in listen_proc_start and freeing it in listen_proc_stop so that if new setparam
         values comes in between lsm_stop and lsm_start they can be served*/
  lab_struct_ptr->circ_buf_ptr = (int8_t *) qurt_elite_memory_malloc(total_delay_bytes,
                                                                     QURT_ELITE_HEAP_DEFAULT);
  if(NULL == lab_struct_ptr->circ_buf_ptr)
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"No memory=%lu for circ buf",
          total_delay_bytes);
#endif
    return ADSP_ENOMEMORY;
  }

  if(1 != num_channels)
  {
    lab_struct_ptr->temp_buf_ptr = (int8_t *) qurt_elite_memory_malloc(temp_buf_size,
                                                                       QURT_ELITE_HEAP_DEFAULT);
    if(NULL == lab_struct_ptr->temp_buf_ptr)
    {
#ifdef LSM_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"No memory=%lu for temp buf",
            total_delay_bytes);
#endif
      return ADSP_ENOMEMORY;
    }
  }
  if(CIRCBUF_FAIL == circ_buf_init(&lab_struct_ptr->circ_buf_struct,
                                   lab_struct_ptr->circ_buf_ptr,
                                   total_delay_bytes))
  {
#ifdef LSM_DEBUG
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Circ buf init failed");
#endif
    qurt_elite_memory_free((void *)lab_struct_ptr->circ_buf_ptr);
    lab_struct_ptr->circ_buf_ptr = NULL;
    qurt_elite_memory_free(lab_struct_ptr->temp_buf_ptr);
    lab_struct_ptr->temp_buf_ptr = NULL;
    return ADSP_EFAILED;
  }

  return result;
}

/*
  Function for starting Look Ahead Buffer(LAB)

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_start(listen_proc_svc_t *session_ptr)
{
  ADSPResult result = ADSP_EOK;
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)session_ptr->lab_struct_ptr;
  /* adjust the read pointer of circular buffer so that it will start reading from the the kwed_position_samples number of samples from write_ptr */
  lab_struct_ptr->kwed_position_bytes = (lab_struct_ptr->kwed_position_bytes > lab_struct_ptr->max_kwed_delay_bytes) ? \
      lab_struct_ptr->max_kwed_delay_bytes : lab_struct_ptr->kwed_position_bytes;
#ifdef LSM_DEBUG
  MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Llab start kwed_position_samples in bytes=%lu",
        lab_struct_ptr->kwed_position_bytes);
#endif
  lsm_send_afe_reg_criteria_t params;
  params.criteria = AFE_PASS_THROUGH;
  params.criteria_enable = TRUE;   

  /* only when LSM connected to AFE this is required */
  if (LSM_CONNECTED_TO_AFE == session_ptr->lsm_client_connection)
  {
    // Register for pass through with AFE
    if (ADSP_FAILED(result = listen_proc_send_cmd_to_afe(session_ptr,
                                                         session_ptr->afe_port_id,
                                                         ELITEMSG_CUSTOM_AFE_CLIENT_CRITERIA_REGISTER,
                                                         (void*)&params)))
    {
#ifdef LSM_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Llab start session:%d Pass through reg with AFE failed, status:0x%x",
            session_ptr->session_id, result);
#endif
      listen_proc_lab_stop(session_ptr);
      return ADSP_EFAILED;
    }
    lab_struct_ptr->is_reg_with_afe = TRUE;
  }

  return ADSP_EOK;
}

/*
  Function for copying samples into circular buffer for Look Ahead Buffer(LAB)

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_process(listen_proc_svc_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  circbuf_result status;
  uint32_t circ_buf_status = 0;
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)me_ptr->lab_struct_ptr;
  int8_t *inp_ptr = me_ptr->output_buf_ptr;
  uint32_t bytes_to_write = me_ptr->output_buf_size;
  lsm_callback_handle_t *cb_data_ptr = &(me_ptr->callback_data);
  int8_t algo_status = me_ptr->detection_status;
  uint16_t bytes_per_sample = (me_ptr->bits_per_sample>16)?4:2;
  uint32_t num_samples_per_ch = bytes_to_write/(bytes_per_sample*me_ptr->num_channels);
  uint32_t num_samples = bytes_to_write/bytes_per_sample;
  int8_t *inp_buf_ptr = NULL;
#ifdef  LAB_DEBUG
  MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO,
        "Llab:bytes_to_write=%lu, algo_status=%d, port=%d",
        bytes_to_write,
        algo_status,
        cb_data_ptr->self_port);
#endif

  if(1 != me_ptr->num_channels)
  {
    /* HLOS always expects data in interleaved fashion. so interleaving the data*/
    lsm_interleave_samples(inp_ptr, lab_struct_ptr->temp_buf_ptr, me_ptr->num_channels,
                           num_samples_per_ch, bytes_per_sample);
    inp_buf_ptr = lab_struct_ptr->temp_buf_ptr;
  }
  else
  {
    inp_buf_ptr = inp_ptr;
  }
  if(24 == me_ptr->bits_per_sample)
  {
    /* convert the format Q27 to Q31 */
    int32_t *pBuf_frm_ptr32 = (int32_t *)(inp_buf_ptr);
    for(uint32_t i=0; i<num_samples; i++)
    {
      pBuf_frm_ptr32[i] = s32_shl_s32_sat(pBuf_frm_ptr32[i], QFORMAT_SHIFT_FACTOR);
    }
  }

  status = circ_buf_write(&lab_struct_ptr->circ_buf_struct,inp_buf_ptr,bytes_to_write);

  /* For the very first buffer after detection (indicated by start_lab flag, it is ok to
   * have an overflow in case of some algorithms. So do not raise an overflow event here
   * For succeeding frames we can.
   * DSP sends the  overflow events to HLOS  only after detection happened
   * Before detection if its overflowing,  DSP wont send any overflow events */
  if((CIRCBUF_OVERFLOW == status) && (FALSE == lab_struct_ptr->start_lab)&& \
      (LSM_DETECTION_STATUS_DETECTED == algo_status ))
  {
    circ_buf_status |= LSM_DATA_OVERFLOW_BIT_MASK;
    result = listen_proc_svc_send_event_to_client(cb_data_ptr,NULL,
                                                  sizeof(lsm_data_event_status_t),
                                                  LSM_DATA_EVENT_STATUS,circ_buf_status);
    if(ADSP_SUCCEEDED (result))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,
            "Llab: LSM_DATA_EVENT_STATUS Event Sent to apps, status:0x%x",
            circ_buf_status);
    }
  }
  else if(CIRCBUF_SUCCESS != status)
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Llab:Circ buf write failed, status=0x%x",status);
#endif
    result = ADSP_EFAILED;
  }

  if(TRUE == lab_struct_ptr->start_lab)
  {
    /* In case algo wants to adjust more bytes than available in the circular buffer then cap it
     * to the available bytes */
    uint32_t bytes_to_adjust = (lab_struct_ptr->kwed_position_bytes >
        lab_struct_ptr->circ_buf_struct.unread_bytes) ?
            lab_struct_ptr->circ_buf_struct.unread_bytes :
            lab_struct_ptr->kwed_position_bytes;
    if(CIRCBUF_SUCCESS != circ_adjust_read(&lab_struct_ptr->circ_buf_struct,
                                           bytes_to_adjust))
    {
#ifdef LSM_DEBUG
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to update the circ buf with kwed pos");
#endif
      result = ADSP_EFAILED;
    }
    lab_struct_ptr->start_lab = FALSE;
  }
  return result;
}

/*
  This function copies data from internal circ buffer to client buffer

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t
         out_buf_params_ptr [in/out] This points to the lsm_proc_svc_out_buf_param_t

  @return
  None

  @dependencies
  None.
 */
void listen_proc_lab_copy_data_to_out_buf(lsm_lab_struct_t *lab_struct_ptr,
                                          lsm_proc_svc_out_buf_param_t *out_buf_params_ptr)
{
  uint32_t out_buf_num_bytes;
  uint32_t bytes_to_copy;
  circ_buf_struct_t *circ_buff_struct_ptr = &lab_struct_ptr->circ_buf_struct;
  circbuf_result status;
  int8_t *dest_ptr = NULL;

  out_buf_num_bytes = out_buf_params_ptr->empty_bytes;

  if (out_buf_num_bytes > 0)
  {

    bytes_to_copy = circ_buff_struct_ptr->unread_bytes;
    bytes_to_copy = (bytes_to_copy > out_buf_num_bytes) ? out_buf_num_bytes : bytes_to_copy;

    dest_ptr = ((int8_t *)(out_buf_params_ptr->shared_memmap_type_node.unVirtAddr)) + \
        (out_buf_params_ptr->out_buf_wr_offset);

    status = circ_buf_read(circ_buff_struct_ptr,dest_ptr,bytes_to_copy);
    if(CIRCBUF_FAIL == status)
    {
#ifdef LSM_DEBUG
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Llab copy_data_to_buffer:Circ buf read failed");
#endif
      return;
    }
#ifdef LAB_DEBUG
    FILE *fp;
    fp = fopen("lab_input.raw","ab");
    fwrite(dest_ptr,1,bytes_to_copy,fp);
    fclose(fp);
#endif
    out_buf_params_ptr->empty_bytes = out_buf_params_ptr->empty_bytes - bytes_to_copy;
    out_buf_params_ptr->out_buf_wr_offset = out_buf_params_ptr->out_buf_wr_offset + bytes_to_copy;

  }

  return;
}

/*
  This function Stops Look Ahead Buffer

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_stop(listen_proc_svc_t *session_ptr)
{
  ADSPResult result = ADSP_EOK;
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)session_ptr->lab_struct_ptr;
  /* reset the circ buf */
  circ_buf_reset(&lab_struct_ptr->circ_buf_struct);

  if(TRUE == lab_struct_ptr->is_reg_with_afe)
  {
    lsm_send_afe_reg_criteria_t params;
    params.criteria = AFE_PASS_THROUGH;
    params.criteria_enable = FALSE;   

    // De-Register for pass through with AFE
    if (ADSP_FAILED(result = listen_proc_send_cmd_to_afe(session_ptr,
                                                         session_ptr->afe_port_id,
                                                         ELITEMSG_CUSTOM_AFE_CLIENT_CRITERIA_REGISTER,
                                                         (void*)&params)))
    {
#ifdef LSM_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Llab stop session : %d Pass through dereg with AFE failed, status:: %d",
            session_ptr->session_id, result);
#endif
    }
    lab_struct_ptr->is_reg_with_afe = FALSE;
  }
  return result;
}

/*
  Function for doing set/getparam for Look Ahead Buffer(LAB)

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t
         param_id [in] Param ID of the input parameter
         param_size [in] Size of the parameter
         params_buffer_ptr [in] pointer of the parameter payload
         set_flag [in] Indicated if it is set param or get param
         actual_param_buf_len [in] Size of the Payload to be filled in case of get param

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_set_get_param(listen_proc_svc_t *session_ptr,
                                         uint32_t param_id,
                                         uint32_t param_size,
                                         int8_t *params_buffer_ptr,
                                         bool_t set_flag,
                                         uint32_t *actual_param_buf_len)
{
  ADSPResult result = ADSP_EOK;
  lsm_lab_struct_t *lab_session_ptr = NULL;
  lsm_param_id_enable_t *enable_ptr = NULL;
  lsm_param_id_lab_config_v1_t *lab_config_param_ptr = NULL;

  if(TRUE == set_flag) /* SET_PARAM */
  {
    if(NULL == session_ptr->lab_struct_ptr)
    {
      session_ptr->lab_struct_ptr = qurt_elite_memory_malloc(sizeof(lsm_lab_struct_t),
                                                             QURT_ELITE_HEAP_DEFAULT);
      if(NULL == session_ptr->lab_struct_ptr)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"No memory for allocating LAB struct");
        return ADSP_EBADPARAM;
      }
      memset(session_ptr->lab_struct_ptr,0,sizeof(lsm_lab_struct_t));
    }
  } /* SET_PARAM */

  lab_session_ptr = (lsm_lab_struct_t *)session_ptr->lab_struct_ptr;

  switch(param_id)
  {
    case LSM_PARAM_ID_ENABLE:
    {
      enable_ptr = (lsm_param_id_enable_t *)params_buffer_ptr;

      if (param_size < sizeof(lsm_param_id_enable_t))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"PARAM_ENABLE[%d] fail: paramsize=%lu",
              set_flag,param_size);
        return ADSP_EBADPARAM;
      }

      if(TRUE == set_flag) /* SET_PARAM */
      {
        if(enable_ptr->enable == 1)
        {
          lab_session_ptr->lab_enable = TRUE;
        }
        else if (enable_ptr->enable == 0)
        {
          lab_session_ptr->lab_enable = FALSE;
        }
        else
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"PARAM_ENABLE[set] invalid value=%d",
                enable_ptr->enable);
          return ADSP_EBADPARAM;
        }
      } /* SET_PARAM */
      else /* GET_PARAM */
      {
        if(NULL == lab_session_ptr)
        {
          enable_ptr->enable = 0;
        }
        else if(FALSE == lab_session_ptr->lab_enable)
        {
          enable_ptr->enable = 0;
        }
        else
        {
          enable_ptr->enable = 1;
        }

        enable_ptr->reserved = 0;
        *actual_param_buf_len = sizeof(lsm_param_id_enable_t);
      } /* GET_PARAM */

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"LSM LAB enable:%d",enable_ptr->enable);
      break;
    }
    case LSM_PARAM_ID_LAB_CONFIG:
    {
      lab_config_param_ptr = (lsm_param_id_lab_config_v1_t *)params_buffer_ptr;

      if (param_size < sizeof(lsm_param_id_lab_config_v1_t))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"PARAM_LAB_CONFIG[%d] fail:paramsize=%lu",
              set_flag, param_size);
        return ADSP_EBADPARAM;
      }

      if(TRUE == set_flag) /* SET_PARAM */
      {
        /* Choose the appropriate config version  */
        uint32_t lab_config_minor_version =  *((uint32_t *)params_buffer_ptr);

        if(lab_config_minor_version > LSM_API_VERSION_LAB_CONFIG)
        {
          lab_config_minor_version = LSM_API_VERSION_LAB_CONFIG;
        }

        if(LSM_API_VERSION_LAB_CONFIG_V1 == lab_config_minor_version)
        {
          lab_config_param_ptr = (lsm_param_id_lab_config_v1_t *)params_buffer_ptr;
          lab_session_ptr->apps_wakeup_latency_ms = lab_config_param_ptr->wake_up_latency_ms;
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APPS Wakeup latency in ms = %lu",
                lab_config_param_ptr->wake_up_latency_ms);
        }
      } /* SET_PARAM */
      else /* GET_PARAM */
      {
        lab_config_param_ptr->minor_version = LSM_API_VERSION_LAB_CONFIG_V1;
        if(NULL == lab_session_ptr)
        {
          lab_config_param_ptr->wake_up_latency_ms = 0;
        }
        else
        {
          lab_config_param_ptr->wake_up_latency_ms = lab_session_ptr->apps_wakeup_latency_ms;
        }
        *actual_param_buf_len = sizeof(lsm_param_id_lab_config_v1_t);
      } /* GET_PARAM */
      break;
    }
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"LAB Unsupported Param ID[0x%lx]",
            param_id);
      result = ADSP_EUNSUPPORTED;
      break;
    }
  }
  return result;
}

/*
  This function deinitializes Look Ahead Buffer

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_deinit(listen_proc_svc_t *session_ptr)
{
  ADSPResult result = ADSP_EOK;
  lsm_lab_struct_t *lab_struct_ptr = (lsm_lab_struct_t *)session_ptr->lab_struct_ptr;

  listen_proc_lab_stop(session_ptr);

  circ_buf_deinit(&lab_struct_ptr->circ_buf_struct);

  if(NULL != lab_struct_ptr->circ_buf_ptr)
  {
    qurt_elite_memory_free(lab_struct_ptr->circ_buf_ptr);
    lab_struct_ptr->circ_buf_ptr = NULL;
  }
  if(NULL != lab_struct_ptr->temp_buf_ptr)
  {
    qurt_elite_memory_free(lab_struct_ptr->temp_buf_ptr);
    lab_struct_ptr->temp_buf_ptr = NULL;
  }
  return result;
}

/*
  This function interleaves the data if it is deinterleaved fashion

  @param src_addr [in] address of the input data
         dst_addr [out] address of the output data
         num_channels [in] num of channels in input data
         num_samples_per_ch [in] number of samples are there per channel
         bytes_per_channel [in] number of bytes are there per channel

  @return
  None

  @dependencies
  None.
 */
void lsm_interleave_samples(int8_t* src_addr, int8_t* dst_addr, int16_t num_channels,
                            uint32_t num_samples_per_ch, int16_t bytes_per_channel)
{
  int16_t     nChan;
  uint32_t    sampIndex;
  int16_t     *src_addr_ptr16, *dst_addr_ptr16;
  int32_t     *src_addr_ptr32, *dst_addr_ptr32;

  if (1 == num_channels)
  {
    memscpy(dst_addr,
            num_samples_per_ch*bytes_per_channel,
            src_addr,
            num_samples_per_ch*bytes_per_channel);
  }
  else
  {
    if(2 == bytes_per_channel)
    {
      src_addr_ptr16 = (int16_t *)src_addr;
      dst_addr_ptr16 = (int16_t *)dst_addr;

      if(2 == num_channels)
      {
        Interleave_16(src_addr_ptr16, (src_addr_ptr16 + num_samples_per_ch), dst_addr_ptr16, num_samples_per_ch);
      }
      else
      {
        //general code
        for (nChan = 0; nChan < num_channels; nChan++)
        {
          for (sampIndex = 0; sampIndex < num_samples_per_ch; sampIndex++)
          {
            *(dst_addr_ptr16 + sampIndex*num_channels + nChan) = *(src_addr_ptr16 + nChan*num_samples_per_ch + sampIndex);
          }
        }
      }
    }
    else
    {
      src_addr_ptr32 = (int32_t *)src_addr;
      dst_addr_ptr32 = (int32_t *)dst_addr;

      if(num_channels == 2)
      {
        Interleave_32(src_addr_ptr32, (src_addr_ptr32 + num_samples_per_ch), dst_addr_ptr32, num_samples_per_ch);
      }
      else
      {
        //general code
        for (nChan = 0; nChan < num_channels; nChan++)
        {
          for (sampIndex = 0; sampIndex < num_samples_per_ch; sampIndex++)
          {
            *(dst_addr_ptr32 + sampIndex*num_channels + nChan) = *(src_addr_ptr32 + nChan*num_samples_per_ch + sampIndex);
          }
        }
      }
    }
  }
}
