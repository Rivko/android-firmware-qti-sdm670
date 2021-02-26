#ifndef __MVM_CCM_API_I_H__
#define __MVM_CCM_API_I_H__

/*
   Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/mvm/inc/mvm_ccm_api_i.h#2 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"
#include "mvm_i.h"
/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

 typedef struct vss_iccm_mvm_voice_session_update_t vss_iccm_mvm_voice_session_update_t;
 
#include "apr_pack_begin.h"
 
 /** @brief Payload structure for the #VSS_IMVM_EVT_VOICE_SESSION_ACTIVE event.
 */
 struct vss_iccm_mvm_voice_session_update_t
 {
   uint16_t num_nb_streams;
     /**< Number of narrow-band (8 KHz) streams connected to this MVM session. */
   uint16_t num_wb_streams;
     /**< Number of wide-band (16 KHz) or higher streams connected to this MVM 
          session. */
   uint16_t num_swb_streams;
     /**< Number of super wide-band (32 KHz) streams connected to this MVM 
          session. */
   uint16_t num_fb_plus_streams;
     /**< Number of full-band (48 KHz) or higher streams connected to this MVM 
          session. */   
   uint16_t num_nb_vocprocs;
     /**< Number of narrow-band (8 KHz) vocprocs connected to this MVM session. */
   uint16_t num_wb_vocprocs;
     /**< Number of wide-band (16 Khz) vocprocs connected to this MVM 
          session. */
   uint16_t num_swb_vocprocs;
     /**< Number of super wide-band (32 Khz) vocprocs connected to this MVM 
          session. */
   uint16_t num_fb_plus_vocprocs;
     /**< Number of full-band (48 Khz) or higher vocprocs connected to this MVM 
          session. */
   uint32_t enc_kpps;
    /**< Encoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
   uint32_t dec_kpps;
    /**< Decoder KPPS requirements. Applicable only if there is a single stream
         attached to this MVM session. It is ignored otherwise. */
   uint32_t dec_pp_kpps;
    /**< Stream RX post-processing block KPPS requirements. Applicable only if
         there is a single stream attached to this MVM session. It is ignored
         otherwise. */
   uint32_t vp_rx_kpps;
    /**< Vocproc RX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
   uint32_t vp_tx_kpps;
    /**< Vocproc TX KPPS requirements. Applicable only if there is a single vocproc
         attached to this MVM session. It is ignored otherwise. */
   uint32_t total_kpps;
     /**< Total number of KPPS for all the streams and vocprocs connected to this 
          MVM session. */
   uint32_t tx_afe_port_sr;
    /** Tx AFE port SR. */
   uint32_t tx_topology_id;
     /**< The TX vocproc topology ID connected to this MVM session. This paramter 
          is ignored if more than one vocproc is connected to the MVM. */
   uint32_t rx_topology_id;
     /**< The RX vocproc topology ID connected to this MVM session. This parameter 
          is ignored if more than one vocproc is connected to the MVM. */
   uint32_t media_id;
     /**< The Media ID of the stream connected to this MVM session. This parameter 
          is ignored if more than one vocproc is connected to the MVM. */
   uint16_t vfr_mode; 
     /**< Vocoder frame synchronization mode. Possible values:\n
          - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization. \n
          - #VSS_ICOMMON_VFR_MODE_HARD -- Hard Vocoder Frame Reference (VFR). 
            A 20 ms VFR interrupt. */
   uint32_t tx_num_channels;
     /**< Tx Number of channels. */
   uint32_t max_thrd_kpps;
     /**< Max number of KPPS of all the streams and vocprocs connected to this
          MVM session. */
   uint32_t tx_mpps_scale_factor;
     /** Tx MPPS scale factor. */
   uint32_t tx_bw_scale_factor;
     /** Tx BW scale factor. */
   uint32_t rx_mpps_scale_factor;
     /** Rx MPPS scale factor. */
   uint32_t rx_bw_scale_factor;
     /** Rx BW scale factor. */
 }
#include "apr_pack_end.h"
 ;

typedef struct vss_icsm_mvm_voice_config_update_t vss_icsm_mvm_voice_config_update_t;
 
#include "apr_pack_begin.h"
 
 /** @brief Payload structure for the #VSS_IMVM_EVT_VOICE_SESSION_ACTIVE event.
 */
 struct vss_icsm_mvm_voice_config_update_t
 {
   mvm_voice_timing_t voice_timing; 
   mvm_voice_processing_delays_t processing_times;
   mvm_vfr_subscription_info_t vfr_info;
   uint32_t vptx_frame_size;
 }
 #include "apr_pack_end.h"
 ;
/****************************************************************************
 * PROTOTYPES                                                               *
 ****************************************************************************/

/**
  Initialize the MVM Concurrency Monitor (MVM-CCM). Must be called before any
  other MVM-CCM APIs can be called, and must be called only once.

  @param[in] mvm_service_apr_handle APR handle of the MVM service.
  @param[in] mvm_service_apr_addr   APR address of the MVM service.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t mvm_ccm_init ( 
  uint32_t mvm_service_apr_handle,
  uint16_t mvm_service_apr_addr
);

/**
  Deinitialize the MVM-CCM.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t mvm_ccm_deinit ( void );

/**
  MVM-CCM adds a voice session to the overall system configuration and tags it 
  with corresponding mvm session handle.

  @param[in] mvm_session_handle.

  @return APR_EOK when successful.
*/
 APR_INTERNAL int32_t mvm_ccm_add_voice_session (
  uint16_t mvm_session_handle
 );

/**
  MVM-CCM removes the voice session from overall system configuration that 
  tagged with corresponding mvm session handle. Current system configuration is
  committed to MMPM and broadcasted to all active mvm sessions.
  
  @param[in] mvm_session_handle.

  @return APR_EOK when successful.
*/
 APR_INTERNAL int32_t mvm_ccm_remove_voice_session (
  uint16_t mvm_session_handle
 );


/**
  MVM-CCM is updated with the voice session configuration corresponidng to mvm 
  session. MVM-CCM commits the overall system configuration to MMPM and 
  broadcasted to all active mvm sessions.

  @param[in] packet Event packet to be processed.

  @return APR_EOK when successful.
*/
 APR_INTERNAL int32_t mvm_ccm_update_voice_session_config (
  uint16_t mvm_session_handle,
  vss_iccm_mvm_voice_session_update_t *in_args
 );
 
/**
  MVM-CSM is updated with the voice session configuration 
  corresponidng to mvm session. 

  @param[in] packet Event packet to be processed.

  @return APR_EOK when successful.
*/
 APR_INTERNAL int32_t mvm_csm_update_voice_session_config (
  uint16_t mvm_session_handle,
  vss_icsm_mvm_voice_config_update_t *in_args
 );


APR_INTERNAL int32_t mvm_csm_cmd_request_vote( 
   vss_imvm_csm_cmd_request_vote_t* vote
);

/**
  Enable/disable CSM and update CSM with the voice timing and 
  processing times info corresponidng to mvm session. 

  @param[in] enable flag indicating whether to enable or disable
             CSM.
  @param[in] voice timing info contianing offset details of 
             dynamic threads.
  @param[in] processing times of the dynamic threads. 

  @return void.
*/
void mvm_csm_update_sleep_manager( 
   bool_t enable
 );
#endif /* __MVM_CCM_API_I_H__ */

