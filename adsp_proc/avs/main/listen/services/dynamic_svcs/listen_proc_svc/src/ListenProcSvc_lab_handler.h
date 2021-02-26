#ifndef _LISTENPROCSVC_LAB_HANDLER_H_
#define _LISTENPROCSVC_LAB_HANDLER_H_
/**
@file ListenProcSvc_lab_handler.h
@brief This file contains api for circular buffer implementation for LAB
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

$Header: //components/rel/avs.adsp/2.8.5/listen/services/dynamic_svcs/listen_proc_svc/src/ListenProcSvc_lab_handler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
3/11/14   SivaNaga      Created file.

========================================================================== */
/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
/* System */
#include "qurt_elite.h"
#include "ListenProcSvc_circ_buffer.h"
#include "ListenProcSvc_Includes.h"

/** Structure for Look Ahead Buffering (LAB)*/
typedef struct
{
  int8_t        *circ_buf_ptr;                 /** Pointer to the start of the circular buffer memory*/
  int8_t        *temp_buf_ptr;                 /** Temperory buffer that is used to interleave before sending LAB data*/
  circ_buf_struct_t circ_buf_struct;           /** circular buffer structure*/
  uint32_t      apps_wakeup_latency_ms;        /** time taken by apps to wake up from the time
                                                   LSM sends detection success event*/
  uint32_t      max_kwed_delay_bytes;          /** maximum delay in bytes for keyword end discovery*/
  bool_t        lab_enable;                    /** indicated whether LAB is enabled or disabled*/
  bool_t        is_reg_with_afe;               /** indicates whether registered with AFE or not*/
  uint32_t      kwed_position_bytes;           /** kwend position in bytes*/
  bool_t        start_lab;                     /** Flag to indicate if Circ buf need to be adjusted for KWED Position */
}lsm_lab_struct_t;

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
		                        uint32_t temp_buf_size);

/*
  Function for starting Look Ahead Buffer(LAB)

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_start(listen_proc_svc_t *session_ptr);

/*
  Function for copying samples into circular buffer for Look Ahead Buffer(LAB)

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_process(listen_proc_svc_t *me_ptr);


/*
  This function Stops Look Ahead Buffer

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_stop(listen_proc_svc_t *session_ptr);

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
                                         uint32_t *actual_param_buf_len);

/*
  This function deinitializes Look Ahead Buffer

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t

  @return
  ADSPResult - indicates Success or failure

  @dependencies
  None.
 */
ADSPResult listen_proc_lab_deinit(listen_proc_svc_t *session_ptr);

/*
  This function copies data from internal listen data buffer to client buffer

  @param lab_struct_ptr [in] This points to the instance of lsm_lab_struct_t
         out_buf_params_ptr [in/out] This points to the lsm_proc_svc_out_buf_param_t

  @return
  None

  @dependencies
  None.
 */
void listen_proc_lab_copy_data_to_out_buf(lsm_lab_struct_t *lab_struct_ptr,
		                                     lsm_proc_svc_out_buf_param_t *out_buf_params_ptr);

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
                            uint32_t num_samples_per_ch, int16_t bytes_per_channel);
#endif /* _LISTENPROCSVC_LAB_HANDLER_H_ */
