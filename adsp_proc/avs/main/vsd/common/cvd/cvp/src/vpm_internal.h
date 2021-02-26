/**========================================================================
 @file vpm_internal.h
 
 @brief This header file contains structure definitions used by the
 the static service and vocproc session objects
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_internal.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_INTERNAL_H_
#define _VPM_INTERNAL_H_

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "vpm_types.h"
#include "vpm_defines.h"

#include "VoiceMixerSvc.h"
#include "VoiceMsgs.h"

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */


/** @brief Structure to save the port/rate/topology configuration associated with a session */
typedef struct vpm_session_config
{
   uint16_t    tx_port;
   /**< tx port configuration of the session */

   uint32_t    tx_topology_id;
   /**< tx topology configuration of the session */

   uint16_t    tx_sampling_rate;
   /**< tx sampling rate configuration of the session */

   uint16_t    rx_port;
   /**< rx port configuration of the session */

   uint32_t    rx_topology_id;
   /**< rx topology configuration of the session */

   uint16_t    rx_sampling_rate;
   /**< rx sampling rate configuration of the session */

   uint32_t    ec_mode;
   /**< it tells whether ec is in async mode or sync mode */

   uint16_t    far_port;
   /**< audio reference port in a case where audio is external */
} vpm_session_config_t;


/** @brief Structure to define the state of a particular session.

    Tx or rx pointers may be NULL if this is not a paired session.  For example, in
    a tx only session, rx will be NULL.  One of rx or tx must be non-null.
*/
typedef struct vpm_dyn_svc_info_t
{
#if 0
   VoiceProcMgr_t           *vpm_ptr;
   /**< owning VPM of this session */
#endif
   elite_svc_handle_t        *tx;
   /**< tx voice proc handle of this session, may be NULL if tx not active */

   elite_svc_handle_t        *rx;
   /**< tx far handle for Rx AFE service*/

   elite_svc_handle_t        *tx_far;
   /**< rx voice proc handle of this session, may be NULL if tx not active */

   vmx_inport_handle_t       *mixer_in_port;
   /**< Input port of mixer for tx voice proc */

   vmx_outport_handle_t      *mixer_out_port;
   /**< Output port of mixer for rx voice proc */

#if 0
   async_struct_t            async_struct;
   /**< Output port of mixer for rx voice proc */
#endif

   uint16_t                  tx_num_channels;
   /**< Number of Mics. Number of VPTx near input channels */

   uint16_t                  rx_num_channels;
   /**< Number of speaker channels. Number of VPRx channels input/output  */
   /**< structure to manage asynchronous responses coming back from dynamic services.

        This structure will collect responses from the dynamic services, and match them
        to the original APR command, so that the APR response is sent when all the appropriate
        messages have been collected, and an appropriate response result has been generated
   */

   uint16_t                  ec_ref_num_channels;
   /**< Number EC reference channels. Number of VPTx far input channels */

   uint32_t                 input_mapping_mask;
   /**< indicates mapping from the voice proc tx modules to encoders */

   uint32_t                 output_mapping_mask;
   /**< indicates mapping from decoders to the voice proc rx modules */

   elite_svc_handle_t       *tx_afe_handle;
   /**< saved handle to Tx Near input AFE service (returned from AFE connect) */

   elite_svc_handle_t       *far_afe_handle;
   /**< saved handle to Tx Far input AFE service (returned from AFE connect) */

   elite_svc_handle_t       *rx_afe_handle;
   /**< structure used to pass in queue information to Tx Far AFE, so that far data is sent to desired Vptx gpQ */

   int16_t                   my_handle;
   /**< saved session handle that was given to Voice SW for this session */

   uint32_t                  state;
   /**< state of session, either run or stop state */

   vpm_session_config_t     session_config;
   /**< used to save the current port/sampling rate/topology configuration. 
     Used to cache values when AFE connect/disconnect done during run/stop */

   uint16_t                 host_pcm_num_active_tap_points;     
   /**< used to track number of active host pcm tap points per session 
     (can stay enabled across RUN->STOP->RUN state transitions) */

   void                     *tx_afe_drift_ptr;
   /**< saved ptr to drift info struct (returned from Tx near-end AFE connect)  */

   void                     *rx_afe_drift_ptr;
   /**< saved ptr to drift info struct (returned from Rx AFE connect)  */

   void                     *far_src_drift_ptr;
   /**< saved ptr to drift info struct (returned from Tx AFE connect)  */

   uint16_t                 timing_cmd_version;
   /**< keeps track of which version of timing command is used.
        needed for clocks requests for ADSP Test fwk.*/

   volatile uint32_t        tx_afe_delay;
   /**< Variable provided to AFE where it writes the tx port delay */

   volatile uint32_t        tx_afe_hw_delay;
   /**< Variable provided to AFE where it writes the tx hw delay */

   volatile uint32_t        rx_afe_delay;
   /**< Variable provided to AFE where it writes the rx port delay */

   volatile uint32_t        rx_afe_hw_delay;
   /**< Variable provided to AFE where it writes the rx hw delay */

}vpm_dyn_svc_info_t;


typedef struct vpm_session_state_ctrl_t
{
  vpm_state_enum_t  state;
   /**< Is current command process pending */

  cvp_goal_enum_t    goal;
   /**< Is current command process pending */

  bool_t             is_goal_completed;
   /**< Is current goal completed */

  vpm_action_enum_t  action;
   /**< Is current command process pending */

  uint32_t           status;
   /**< Is current command process pending */

}vpm_session_state_ctrl_t;

typedef struct vpm_cmd_ctrl_t
{
   qurt_elite_queue_t *p_cmd_queue;
   /**< Pointer to session command queue */

   elite_msg_any_t   msg_pkt;
   /**< Current msg packet under process */

   bool              is_prev_cmd_pending;
   /**< Is previous command process pending */

   vpm_action_enum_t cmd_seq_action;
   /**< Next action for cmd sequencer */

   uint32_t           status;
   /**< Command status */

}vpm_cmd_ctrl_t;


typedef struct vpm_cmd_rsp_ctrl_t
{
   qurt_elite_queue_t   *p_rsp_queue;
   /**< Pointer to session response queue */

   uint32_t             opcode;
   /**< Current cmd opcode under process */

   elite_msg_any_t      msg_pkt;
   /**< Current rsp msg packet under process */

   uint32_t             num_cmd_issued;
   /**< Number of commands with this opcode issued */

   uint32_t             num_rsp_received;
   /**< Number of responses received */

   uint32_t             num_rsp_failed;
   /**< Number of responses received */

   uint32_t             num_unsupported_ecode;
   /**< Number of responses with ADSP_EUNSUPPORTED ecode */

   bool                is_prev_cmd_rsp_pending;
   /**< Is response for current command to peer service 
     pending */

   uint32_t             status;
   /**< Aggregated status of the responses received */

   void                 *p_ack_payload;
   /**< Used for reponse aggregated from dynamic services */

   bool_t               is_ack_payload_type_apr;
   /**< If the ACK payload is an APR packet  */

   vpm_rsp_handler_fn_t  p_rsp_hndlr_fn;
   /**< Response handler */
   
}vpm_cmd_rsp_ctrl_t;


typedef struct vpm_session_object_t
{

   vpm_dyn_svc_info_t     dyn_svc_info;

   uint32_t               session_id;

   uint16_t self_apr_addr;
   uint16_t self_apr_handle;
   uint16_t self_apr_port;

   uint16_t full_ctrl_client_addr;
   uint16_t full_ctrl_client_port;

   uint32_t vocproc_session_idx;

   uint16_t attached_mvm_handle;
   uint16_t mvm_apr_addr;
   /**<
     * The MVM session handle that this vocproc is attached to. A handle with
     * a value of APR_NULL_V indicates that this vocproc is not attached to an
     * MVM session.
     */

   bool_t is_vocproc_config_changed;
   /**<
     * Indicates whether the vocproc configurations have changed since the
     * last time when VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
     * Specifically, this flag is set to TRUE under the following
     * circumstances:
     *
     *   - upon the vocproc receiving a VSS_IVOCPROC_CMD_SET_SAMPLING_RATE
     *     command such that it results in the vocproc to have different
     *     sample rates from those currently configured on the vocproc.
     *
     *   - upon the vocproc receiving a VSS_IVOCPROC_CMD_SET_DEVICE or
     *     VSS_IVOCPROC_CMD_SET_DEVICE_V2 command such that it results in
     *     the vocporc being re-initialized.
     *
     *   - upon the vocproc receiving a
     *     VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG such that it results in
     *     the vocproc to have a different rx sampling rate from what is
     *     currently configured on the vocproc.
     *
     *   - upon a new set of calibration data (either volume or common) being
     *     registered with the vocproc.
     *
     *   - upon the vocproc receiving a VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX
     *     or VSS_IVOLUME_CMD_SET_STEP when the vocproc is in IDLE state such
     *     that it results in the vocproc to have a different volume level than
     *     what is currently configured on the vocproc.
     *
     *   - upon the vocproc receiving a VSS_IVOLUME_CMD_SET_NUMBER_OF_STEPS
     *     such that it results in the vocproc to have a different number of
     *     volume steps than what is currently configured on the vocproc.
     *
     * The first four events have the effect of potentially changing the
     * current KPPS requirements of the vocproc. And the vocproc must be
     * reconfigured.
     *
     * The last two events do not change the current KPPS requirement. Note
     * that changing the volume level or the number of volume steps will not
     * cause additional modules to be enabled/disabled (assuming the volume
     * calibration data has been applied for a different volume level).
     * This is because by design, each volume level in the volume calibration
     * table uses the same set of module enable/disable parameters, and volume
     * level change only changes the gain-dependent parameter data values. But
     * the vocproc must be reconfigured in-order to apply the new volume
     * settings.
     *
     * Upon the vocproc receiving a VSS_IVOCPROC_CMD_ENABLE command, if
     * is_vocproc_config_changed is TRUE, and the vocproc is attached to an MVM
     * session, the vocproc will send a VSS_IVOCPROC_EVT_RECONFIG event to MVM
     * in order to notify MVM to reconfigure this vocproc.
     *
     * This flag is set to FALSE under the following circumstances:
     *
     *   - upon session creation.
     *
     *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
     */

   uint32_t direction;

   bool_t public_enable;
   bool_t modem_enable;

   vpm_shared_heap_t shared_heap;
   /**< Cached VP3, UI property, and source track data that to be shared with VPM. */

   vpm_cmd_ctrl_t  session_cmd_ctrl;

   vpm_cmd_rsp_ctrl_t  session_rsp_ctrl;

   vpm_session_state_ctrl_t   state_ctrl;

   vpm_active_settings_t active_set;
   vpm_target_settings_t target_set;

   apr_list_t attached_stream_list;
   /**< Handles of streams attached to this CVP session. */

   cvp_set_tx_dtmf_detect_settings_t set_tx_dtmf_detect;

   vpm_vpcm_info_t vpcm_info;
   //bool_t is_vol_table_format_v1;

   bool_t is_sr_set_by_client;
   /**< TRUE: Sampling rate is set by client. */

   bool_t is_device_config_registered;
   /**< TRUE: Device config data is registered. */

   uint32_t widevoice_rx_sr;
   /**< Widevoice Rx sampling rate. */

   vpm_common_cal_info_t common_cal; /* BACKWARD COMPATIBILITY */
   /**< Common calibration table information. */

   vpm_vol_cal_info_t volume_cal; /* BACKWARD COMPATIBILITY */
   /**< Volume calibration table information. */

   vpm_static_cal_info_t static_cal;
   /**< Static calibration table information. */

   vpm_dynamic_cal_info_t dynamic_cal;
   /**< Dynamic calibration table information. */

   bool_t is_client_set_num_vol_steps;
   /**< TRUE: client has set number of volume steps. */


   bool_t is_valid_afe_port_provided;
   /**<
     * Flag to indicate if valid AFE port IDs has been provided. This is used
     * for a temporary hack to solve the following problem. and the long-term
     * proper solution is required to be done on the VCP side.
     *
     * Problem: In the current VCP architecture, issuing VPM_CREATE without Rx
     * or Tx AFE port results in the VPM not being created. As a result, any
     * attach of vocproc to stream before providing AFE ports, although it
     * reports success, is actually a no-op. This means following valid
     * sequence does not work:
     * 1. Create stream.
     * 2. Create vocproc with TOPOLOGY_ID_NONE and PORT_ID_NONE for both Tx
     *    and Rx.
     * 3. Attach vocproc to stream.
     * 4. SET_DEVICE on vocproc (with new topologies and port IDs).
     * 5. Run.
     * Although all the steps above will report success, there is no data
     * transfer, because step 3 is actually a no-op.
     *
     * Temoprary hack in CVP:
     *
     * 1. Set the AFE ports to AFE pseudo-ports upon vocproc creation if the
     *    Tx or Rx AFE ports provided by the client is
     *    VSS_IVOCPROC_PORT_ID_NONE.
     * 2. Keep a flag indicating whether the client has provided valid Tx or
     *    Rx AFE ports or not.
     * 3. If client attempts to RUN the vocproc without having provided valid
     *    AFE port, we will fail the RUN. This is to avoid running with some
     *    pseudo-port that perhaps Audio path is actually doing something with
     *    and picking up that data on the voice path.
     */

   bool_t is_client_set_vp3_data;
   /**< Flag to indicate if the client has set the VP3 data via the
        VSS_IVP3_CMD_SET_DATA command and the VP3 data has not been
        applied. */

   vpm_sound_device_info_t sound_device_info;

   vpm_kpps_info_t kpps_info;

   bool_t is_kpps_changed;

   uint32_t vptx_frame_size_us;

   bool_t is_clk_overridden;
     /**< TRUE (1): Clock / BW / MPPS are voted for session creation.
      *   FALSE (0): Clock / BW / MPPS are not voted or released.
      */

   vpm_hdvoice_config_info_t hdvoice_config_info;
   /**< HD Voice config information */

   uint32_t call_num;

   cvd_virt_addr_t sourcetrack_mem_addr;
   /**< Address of client mapped memory for getting
        source track sound activity. */

   bool_t is_client_set_sectors;
   /**< TRUE: client has set sound focus sectors. */

   uint32_t ref_cnt;
   /**< Reference counter. To safely free session object
      and associated resources. */

   vpm_topo_param_info_t topo_param;
   /**< Topology param information. */

   vpm_topology_commit_state_enum_t topo_commit_state;
   /**< Topology parameters commit state. */

   vpm_clk_control_config_t clk_ctrl_table;
   /**< Clock control table containing per use case MPPS and bus BW scaling factors. */
   
   vpm_port_media_info_t tx_port_media_info;
   vpm_port_media_info_t rx_port_media_info;
   vpm_port_media_info_t ec_ref_port_media_info;
}vpm_session_object_t;

/* Overall Voice Proc Manager service structure. */
typedef struct voice_proc_mgr
{
   elite_svc_handle_t      svc_handle;
   /**< VPM handle to give out to others */   
   
   elite_svc_handle_t	   *h_med_svc;
   /**< Helper service */

   uint16_t                session_cnt;
   /**< Counter for active sessions */

   vpm_session_object_t    *sessions[VPM_MAX_NUM_SESSIONS];
   /**< Maintain a list of all sessions. */   

   uint32_t                apr_handle;
   /**< VPM APR handle obtained after APR registration */

   uint16_t                apr_addr;
   /**< VPM self APR address */

   uint32_t                memory_map_client;
   /**< Memory map client */

   qurt_elite_channel_t    channel;
   /**< Mask for MQ's owned by this obj */

   uint32_t                svc_cmd_q_wait_mask;
   /**< Signal wait mask for command Q for static service */

   uint32_t                session_cmd_q_wait_mask;
   /**< Signal wait mask for command Q for all the sessions */

   uint32_t                session_rsp_q_wait_mask;
   /**< Signal wait mask for response Q for all the sessions */

   uint32_t                curr_wait_mask;      
   /**< Contains the current channel mask or signals to act
    *  upon */

   uint32_t                output_mask;
   /**< Current signal received */

   uint32_t                pending_cmd_mask;
   /**< Mask for pending commands */
   
   QURT_ELITE_ALIGN(char svc_cmd_q[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VPM_MAX_SVC_CMD_Q_ELEMENTS)], 8);
   /**< Service level command queue */

   QURT_ELITE_ALIGN(char svc_rsp_q[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(VPM_MAX_SVC_RSP_Q_ELEMENTS)], 8);
   /**< Service level command queue */

   vpm_session_static_info_t  session_static_info[VPM_MAX_NUM_SESSIONS];
   /**< Service level command queue */

   qurt_elite_queue_t      *p_afe_svc_cmd_q;
   /**< AFE service command queue */

   vpm_vp3_info_t          vp3_info;
   /**< VP3 info struct shared across sessions */

   vpm_cmd_ctrl_t          cmd_ctrl;
   /**< Command control info */

   vpm_cmd_rsp_ctrl_t      rsp_ctrl;
   /**< Response control for responses from peer svcs  */

   bool_t vpm_csm_vote;
   /**< CSM vote status for VPM.  */
}voice_proc_mgr_t;

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */


extern voice_proc_mgr_t  *p_vpm_svc_global_obj;

/* -----------------------------------------------------------------------
** Function Definitions
** ----------------------------------------------------------------------- */

static inline uint32_t vpm_svc_get_mem_map_client()
{
   return p_vpm_svc_global_obj->memory_map_client;
}

ADSPResult vpm_action_broadcast_afe_port_info(vpm_session_object_t *session_obj);

ADSPResult vpm_state_control(vpm_session_object_t *session_obj);

#endif /* #ifndef _VPM_INTERNAL_H_ */

