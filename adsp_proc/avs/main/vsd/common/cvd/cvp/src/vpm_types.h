/**========================================================================
 @file vpm_types.h
 
 @brief This header file contains structure definitions used by vocproc
 static service and dynamic session objects
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

 ====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvp/src/vpm_types.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef _VPM_TYPES_H_
#define _VPM_TYPES_H_

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "qurt_elite.h"
#include "Elite.h"
#include "vpm_defines.h"

/****************************************************************************
 * Globals
 ****************************************************************************/

/* VoiceProcMgr SysCmdQ handler type*/
typedef int (*voice_proc_mgr_svc_cmd_q_hndlr_f)(void *p_svc_instance, elite_msg_any_t *p_msg_pkt);

/* VoiceProcMgr session resp/msg queue handler type*/
typedef int (*voice_proc_mgr_session_resp_q_hndlr_f)(void *p_session_obj, elite_msg_any_t *p_msg_pkt);

/* VoiceProcMgr session cmd queue handler type*/
typedef int (*voice_proc_mgr_session_cmd_q_hndlr_f)(void *p_session_obj, elite_msg_any_t *p_msg_pkt);

/* -----------------------------------------------------------------------
** Response handler function pointer 
** ----------------------------------------------------------------------- */

/* VPM response handler */
typedef ADSPResult(*vpm_rsp_handler_fn_t)(void *contex_ptr);

/****************************************************************************
 * SESSION CONTROL DEFINITIONS                                              *
 ****************************************************************************/

typedef enum vpm_state_enum_t
{
   VPM_STATE_ENUM_UNINITIALIZED,
   /**< Reserved. */
   VPM_STATE_ENUM_RESET,
   /**< The session resource is not acquired. */
   VPM_STATE_ENUM_IDLE,
   /**< The session resource is acquired but not running. */
   VPM_STATE_ENUM_RUN,
   /**< The session resource is running. */
   VPM_STATE_ENUM_ERROR,
   /**< The session resource is unusable and should be destroyed. */
   VPM_STATE_ENUM_INVALID
   /**< Reserved. */
}
vpm_state_enum_t;

typedef enum cvp_goal_enum_t
{
   CVP_GOAL_ENUM_UNINITIALIZED,
   CVP_GOAL_ENUM_NONE,
   CVP_GOAL_ENUM_CREATE,
   CVP_GOAL_ENUM_ENABLE,
   CVP_GOAL_ENUM_DISABLE,
   CVP_GOAL_ENUM_DESTROY,
   CVP_GOAL_ENUM_REINIT,
   CVP_GOAL_ENUM_INVALID,
}
cvp_goal_enum_t;

typedef enum vpm_action_enum_t
{
   VPM_ACTION_ENUM_UNINITIALIZED,
   /* Common actions. */
   VPM_ACTION_ENUM_NONE,
   /**< The first action has not started for a goal from any state. */
   VPM_ACTION_ENUM_COMPLETE,
   /**<
    * Reached the last action for a goal from a state. A multi-action goal
    * that starts from and ends in the same state may require a COMPLETE
    * action to properly differentiate a terminate signal.
    */
   VPM_ACTION_ENUM_CONTINUE,
   /**<
    * For multi-state goals, the last action from each state should set to
    * CONINTUE. This indicates to the next state that a goal is continuing
    * its operation from a previous state. Usually the previous state is
    * known given the current state and the continued goal. New actions can
    * be created to help discriminate the direction from where goals come
    * from as required.
    */
   VPM_ACTION_ENUM_SET_CLOCKS,
   VPM_ACTION_ENUM_REVERT_CLOCKS,
   VPM_ACTION_ENUM_CREATE_VOCPROC,
   VPM_ACTION_ENUM_REINIT_VOCPROC,
   VPM_ACTION_ENUM_WAIT_REINIT_DONE,
   VPM_ACTION_ENUM_HDVOICE_ENABLE,
   VPM_ACTION_ENUM_HDVOICE_ENABLE_DONE,
   VPM_ACTION_ENUM_HDVOICE_DISABLE,
   VPM_ACTION_ENUM_HDVOICE_DISABLE_DONE,
   VPM_ACTION_ENUM_CALIBRATE_COMMON,
   VPM_ACTION_ENUM_CALIBRATE_VOLUME,
   VPM_ACTION_ENUM_CALIBRATE_STATIC,
   VPM_ACTION_ENUM_CALIBRATE_DYNAMIC,
   VPM_ACTION_ENUM_GET_KPPS,
   VPM_ACTION_ENUM_GET_KPPS_DONE,
   VPM_ACTION_ENUM_WAIT_SETPARAM_DONE,
   VPM_ACTION_ENUM_SET_TX_MUTE,
   VPM_ACTION_ENUM_SET_RX_MUTE,
   VPM_ACTION_ENUM_SET_RX_TX_MUTE,
   VPM_ACTION_ENUM_SET_VP3,
   VPM_ACTION_ENUM_SET_UI_PROPERTIES,
   VPM_ACTION_ENUM_SET_SOUNDFOCUS_SECTORS,
   VPM_ACTION_ENUM_ENABLE_VOCPROC,
   VPM_ACTION_ENUM_BROADCAST_READY,
   VPM_ACTION_ENUM_BROADCAST_NOT_READY,
   VPM_ACTION_ENUM_DISABLE_VOCPROC,
   VPM_ACTION_ENUM_GET_VP3,
   VPM_ACTION_ENUM_BROADCAST_GOING_AWAY,
   VPM_ACTION_ENUM_DESTROY_VOCPROC,
   VPM_ACTION_ENUM_REGISTER_VOICE_ACTIVITY_UPDATE_EVENT,
   VPM_ACTION_ENUM_MAP_SHARED_MEMORY,
   VPM_ACTION_ENUM_UNMAP_SHARED_MEMORY,
   VPM_ACTION_ENUM_SET_VOICE_TIMING,
   VPM_ACTION_ENUM_TX_DTMF_DETECT,
   VPM_ACTION_ENUM_BROADCASAT_AFE_PORT_INFO,
   VPM_ACTION_ENUM_ERR_HNDLR,
   VPM_ACTION_ENUM_INVALID
}
vpm_action_enum_t;

typedef enum vpm_enable_state_enum_t
{
   VPM_DISABLED = 0,
   VPM_ENABLED = 1
}
vpm_enable_state_enum_t;

typedef enum vpm_dynamic_cal_usecase_enum_t
{
  VPM_DYNAMIC_CAL_USECASE_GAIN = 0,
  VPM_DYNAMIC_CAL_USECASE_VOCODER,
  VPM_DYNAMIC_CAL_USECASE_GENERIC
}
vpm_dynamic_cal_usecase_enum_t;

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef enum vpm_topology_commit_state_enum_t
{
   VPM_TOPOLOGY_COMMIT_STATE_NONE,
   VPM_TOPOLOGY_COMMIT_STATE_CREATE,
   VPM_TOPOLOGY_COMMIT_STATE_SET_DEVICE
}
vpm_topology_commit_state_enum_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * THE SESSION OBJECT                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef struct vpm_vp3_cache_t
{
   uint32_t data_len;
   uint8_t data[VPM_MAX_VP3_DATA_LEN];
}
vpm_vp3_cache_t;

typedef struct vpm_ui_prop_cache_t
{
   uint32_t data_len;
   uint32_t num_ui_prop;
   uint32_t version;
   uint8_t data[((VPM_MAX_UI_PROP_DATA_LEN +
                  sizeof(vss_icommon_param_data_v3_t))*
                 VPM_MAX_UI_PROP)];
   /**<
    * The size of the data is
    * ( UI prop data length + header ) * max number UI props.
    */
}
vpm_ui_prop_cache_t;

typedef struct vpm_vol_param_data_t vpm_vol_param_data_t;

#include "apr_pack_begin.h"

struct vpm_vol_param_data_t
{
   uint32_t volume;
   uint16_t ramp_duration_ms;
   uint16_t reserved;
}
#include "apr_pack_end.h"
;

typedef struct vpm_vol_param_t vpm_vol_param_t;

#include "apr_pack_begin.h"

struct vpm_vol_param_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
   vpm_vol_param_data_t param_data;
}
#include "apr_pack_end.h"
;

typedef struct vpm_vol_param_v2_t vpm_vol_param_v2_t;

#include "apr_pack_begin.h"

struct vpm_vol_param_v2_t
{
   uint32_t module_id;
   uint16_t instance_id;
   uint16_t reserved;
   uint32_t param_id;
   uint32_t param_size;

   vpm_vol_param_data_t param_data;
}
#include "apr_pack_end.h"
;

typedef struct vpm_device_config_entry_t vpm_device_config_entry_t;

#include "apr_pack_begin.h"

struct vpm_device_config_entry_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
   uint8_t *param_data;
}
#include "apr_pack_end.h"
;

typedef struct vpm_topology_param_entry_t vpm_topology_param_entry_t;

#include "apr_pack_begin.h"

struct vpm_topology_param_entry_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
#if 0
   uint8_t param_data[param_size];
#endif
}
#include "apr_pack_end.h"
;

typedef struct vpm_device_config_wv_sr_t vpm_device_config_wv_sr_t;

#include "apr_pack_begin.h"

struct vpm_device_config_wv_sr_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
   uint32_t rx_sr;
}
#include "apr_pack_end.h"
;

typedef struct vpm_vol_level_info_t vpm_vol_level_info_t;

#include "apr_pack_begin.h"

struct vpm_vol_level_info_t
{
   bool_t is_vol_level_found;
   uint32_t vol_level;
   uint32_t instance_id;
}
#include "apr_pack_end.h"
;

typedef struct vpm_soundfocus_param_data_t vpm_soundfocus_param_data_t;

#include "apr_pack_begin.h"

struct vpm_soundfocus_param_data_t
{
   uint16_t start_angles[8];
   uint8_t enables[8];
   uint16_t gain_step;
   uint16_t reserved;
}
#include "apr_pack_end.h"
;

typedef struct vpm_soundfocus_param_t vpm_soundfocus_param_t;

#include "apr_pack_begin.h"

struct vpm_soundfocus_param_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
   vpm_soundfocus_param_data_t param_data;
}
#include "apr_pack_end.h"
;

typedef struct vpm_sourcetrack_data_t vpm_sourcetrack_data_t;

#include "apr_pack_begin.h"

struct vpm_sourcetrack_data_t
{
   uint32_t module_id;
   uint32_t param_id;
   uint16_t param_size;
   uint16_t reserved;
   vss_isourcetrack_activity_data_t activity_data;
}
#include "apr_pack_end.h"
;

/* CVP heap shared with VPM. This heap stores the cached data:
 * VP3 data, UI property, and source track activity.
 */
typedef struct vpm_shared_heap_t
{
   vpm_vp3_cache_t vp3_cache;
   vpm_ui_prop_cache_t ui_prop_cache;
   vpm_sourcetrack_data_t sourcetrack_activity;
}
vpm_shared_heap_t;

typedef struct vpm_port_media_info_t
{
   uint32_t port_id;
   uint16_t num_channels;
   uint16_t bits_per_sample;
   uint32_t sampling_rate;
   uint8_t  channel_mapping[8];
}
vpm_port_media_info_t;

typedef struct vpm_dev_channel_info_t
{
   vss_param_vocproc_dev_channel_info_t  tx;
   vss_param_vocproc_dev_channel_info_t  rx;
   vss_param_vocproc_dev_channel_info_t  ec_ref;
}
vpm_dev_channel_info_t;

typedef struct vpm_active_settings_t
{
   uint16_t tx_port_id;
   uint16_t rx_port_id;

   uint32_t tx_topology_id;
   uint32_t rx_topology_id;

   uint32_t vol_step;
   uint32_t client_num_vol_steps;

   vss_icommon_cmd_set_system_config_t system_config;

   vss_ivocproc_cmd_set_voice_timing_t voice_timing;

   bool_t client_tx_mute;
   bool_t client_rx_mute;
   uint16_t mute_ramp_duration;

   uint32_t vocproc_mode;
   uint16_t ec_ref_port_id;

   uint32_t rx_in_sample_rate;

   vss_param_num_dev_channels_t num_dev_channels;
   /**< Vocproc inband topology param - number of device channels. */

   vpm_dev_channel_info_t dev_chan_info;
   /**< Device channel information. */
}
vpm_active_settings_t;

typedef struct vpm_target_settings_t
{
   uint16_t tx_port_id;
   uint16_t rx_port_id;

   uint32_t tx_topology_id;
   uint32_t rx_topology_id;

   uint32_t vol_step;
   uint32_t client_num_vol_steps;

   vss_icommon_cmd_set_system_config_t system_config;

   vss_ivocproc_cmd_set_voice_timing_t voice_timing;

   vss_isoundfocus_cmd_set_sectors_t soundfocus_sectors;

   bool_t client_tx_mute;
   bool_t client_rx_mute;
   uint16_t mute_ramp_duration;

   uint32_t vocproc_mode;
   uint16_t ec_ref_port_id;

   uint32_t rx_in_sample_rate;
   
   vss_param_num_dev_channels_t num_dev_channels;
   /**< Vocproc inband topology param - number of device channels. */

   vpm_dev_channel_info_t dev_chan_info;
   /**< Device channel information. */
}
vpm_target_settings_t;

typedef struct vpm_attached_stream_item_t
{
   apr_list_node_t link;

   uint16_t addr;
   uint16_t port;
}
vpm_attached_stream_item_t;

typedef struct cvp_set_tx_dtmf_detect_settings_t
{
   uint16_t client_addr;
   uint16_t client_port;
   vpm_enable_state_enum_t  enable_flag;
}
cvp_set_tx_dtmf_detect_settings_t;

typedef struct vpm_vpcm_info_t
{
   bool_t is_enabled;
   /* Flag to indicate whether vpcm is enabled or not. */
   uint16_t client_addr;
   /* Address of VPCM client. */
   uint16_t client_handle;
   /* Handle for VPCM client. */
   uint32_t mem_handle;
   /* VPCM Client's memory handle. */
}
vpm_vpcm_info_t;

typedef struct vpm_common_cal_info_t /* BACKWARD COMPATIBILITY. */
{
   bool_t is_registered;
   /* Indicates whether calibration table is registered. */
   bool_t is_calibrate_needed;
   /**<
     * Indicates whether the vocproc common calibration data need to be
     * applied. Specifically, this flag is set to TRUE under the following
     * circumstances:
     *
     *   - upon a new set of common calibration data being registered with the
     *     vocproc.
     *
     *   - upon the vocproc has been re-initialized, and there is common calibration
     *     data currently being registered with the vocproc.
     *
     *   - upon the vocproc receiving a VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG
     *     command and any of the following parameters is different between
     *     the vocproc active_set and target_set: network_id, tx_pp_sr,
     *     rx_pp_sr, and there is common calibration data currently
     *     registered.
     *
     * This flag is set to FALSE under the following circumstances:
     *
     *   - upon session creation.
     *
     *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
     *
     *   - upon common calibration being deregistered.
     */
   cvd_cal_table_handle_t *table_handle;
   /* Handle to parsed calibration table information. */
   uint32_t vpm_mem_handle;
   /* Handle to VPM shared memory. */
   cvd_cal_entry_t matching_entries[(VPM_MAX_NUM_MATCHING_COMMON_CAL_ENTRIES *
                                     sizeof(cvd_cal_entry_t))];
   /* Memory for storing matched entries. */
   uint32_t num_matching_entries;
}
vpm_common_cal_info_t;

typedef struct vpm_vol_cal_info_t /* BACKWARD COMPATIBILITY. */
{
   bool_t is_registered;
   /* Indicates whether calibration table is registered. */
   bool_t is_calibrate_needed;
   /**<
     * Indicates whether the vocproc volume calibration data need to be
     * applied. Specifically, this flag is set to TRUE under the following
     * circumstances:
     *
     *   - upon a new set of volume calibration data being registered with the
     *     vocproc.
     *
     *   - upon the vocproc has been re-initialized, and there is volume calibration
     *     data currently being registered with the vocproc.
     *
     *   - upon the vocproc receiving a VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG
     *     command and any of the following parameters is different between
     *     the vocproc active_set and target_set: network_id, tx_pp_sr,
     *     rx_pp_sr, and there is volume calibration data currently
     *     registered.
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
     * This flag is set to FALSE under the following circumstances:
     *
     *   - upon session creation.
     *
     *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
     *
     *   - upon volume calibration being deregistered.
     */
   cvd_cal_table_handle_t *table_handle;
   /* Handle to parsed calibration table information. */
   uint32_t vpm_mem_handle;
   /* Handle to VPM shared memory. */
   cvd_cal_entry_t matching_entries[(VPM_MAX_NUM_MATCHING_VOLUME_CAL_ENTRIES *
                                     sizeof(cvd_cal_entry_t))];
   /* Memory for storing matched entries. */
   uint32_t num_matching_entries;
   uint32_t min_vol_index;
   /* Min volume index in table. */
   uint32_t max_vol_index;
   /* Max volume index in table. */
   uint32_t num_vol_indices;
   /* Number of volume indices. */
   bool_t is_v1_format;
   /* Volume table format flag. */
}
vpm_vol_cal_info_t;

typedef struct vpm_static_cal_info_t
{
   bool_t is_registered;
   bool_t is_calibrate_needed;
   /**<
     * Indicates whether the vocproc static calibration data need to be
     * applied. Specifically, this flag is set to TRUE under the following
     * circumstances:
     *
     *   - upon a new set of static calibration data being registered with the
     *     vocproc.
     *
     *   - upon the vocproc has been re-initialized, and there is static
     *     calibration data currently being registered with the vocproc.
     *
     *   - upon the vocproc receiving a VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG
     *     command and any of the following system configurations is different
     *     between the vocproc active_set and target_set: network_id, tx_pp_sr,
     *     rx_pp_sr, tx_voc_op_mode, rx_voc_op_mode, media_id, feature, and
     *     there is static calibration data currently registered.
     *
     * This flag is set to FALSE under the following circumstances:
     *
     *   - upon session creation.
     *
     *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
     *
     *   - upon static calibration being deregistered.
     */
   uint32_t table_handle;
   uint32_t query_handle;
   uint8_t matching_entries[CVD_CAL_GET_REQUIRED_MATCHING_ENTRIES_BYTE_SIZE(VPM_NUM_STATIC_CAL_COLUMNS)];
   /**<
     * Book-keeping buffer to be provided to CVD CAL utility for maintaining
     * the location (address or offset) of the matching calibration entries in
     * the calibration table; so that multiple calls of cvd_cal_query with the
     * the same query_handle can simply return the calibration values based on
     * the cached matching entries rather than performing a fresh search on
     * every cvd_cal_query call.
     */
   cvd_cal_column_t query_key_columns[VPM_NUM_STATIC_CAL_COLUMNS];
}
vpm_static_cal_info_t;

typedef struct vpm_dynamic_cal_info_t
{
   bool_t is_registered;
   bool_t is_calibrate_needed;
   /**<
     * Indicates whether the vocproc dynamic calibration data need to be
     * applied. Specifically, this flag is set to TRUE under the following
     * circumstances:
     *
     *   - upon a new set of dynamic calibration data being registered with
     *     the vocproc.
     *
     *   - upon the vocproc has been re-initialized, and there is dynamic
     *     calibration data currently being registered with the vocproc.
     *
     *   - upon the vocproc receiving a VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG
     *     command and any of the following parameters is different between
     *     the vocproc active_set and target_set: network_id, tx_pp_sr,
     *     rx_pp_sr, rx_vol_index, tx_voc_op_mode, rx_voc_op_mode, media_id,
     *     feature, and there is volume calibration data currently registered.
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
     * This flag is set to FALSE under the following circumstances:
     *
     *   - upon session creation.
     *
     *   - upon VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG has been processed.
     *
     *   - upon dynamic calibration being deregistered.
     */

   uint32_t min_vol_index;
   /* Min volume index in table. */
   uint32_t max_vol_index;
   /* Max volume index in table. */
   uint32_t num_vol_indices;
   /* Number of volume indices. */
   bool_t is_v1_vol_format;
   /* Volume calibration format flag. */

   uint32_t table_handle;
   uint32_t query_handle;
   uint8_t matching_entries[CVD_CAL_GET_REQUIRED_MATCHING_ENTRIES_BYTE_SIZE(VPM_NUM_DYNAMIC_CAL_COLUMNS)];
   /**<
     * Book-keeping buffer to be provided to CVD CAL utility for maintaining
     * the location (address or offset) of the matching calibration entries in
     * the calibration table; so that multiple calls of cvd_cal_query with the
     * the same query_handle can simply return the calibration values based on
     * the cached matching entries rather than performing a fresh search on
     * every cvd_cal_query call.
     */
   cvd_cal_column_t query_key_columns[VPM_NUM_DYNAMIC_CAL_COLUMNS];
}
vpm_dynamic_cal_info_t;

typedef struct vpm_kpps_info_t
{
   uint32_t vp_rx;
   uint32_t vp_tx;
}
vpm_kpps_info_t;

typedef struct vpm_hdvoice_config_hdr_t
{
   uint32_t minor_version;
   uint32_t size;
   uint32_t num_columns;
   void *columns;
   uint32_t num_sys_config_entries;
   void *sys_config_list_head;
}
vpm_hdvoice_config_hdr_t;

typedef struct vpm_hdvoice_config_info_t
{
   vpm_hdvoice_config_hdr_t hdvoice_config_hdr;
   /**< HD Voice config information */
   bool_t is_custom_bwe_config_registered;
   /**< Supported values: \n
        - TRUE -- if device config is registered with custom bwe feature config 
        - FALSE -- Otherwise */
}
vpm_hdvoice_config_info_t;

typedef struct cvp_hdvoice_config_index_t
{
   uint32_t *index_columns;
   vss_param_hdvoice_config_data_t hdvoice_config_data;
}
cvp_hdvoice_config_t;

typedef struct vpm_hdvoice_enable_param_info_t
{
   voice_param_data_t param_hdr;
   /**< Header for set param payload */

   uint16_t enable;
   /**< Enablement value. Supported values: \n
        - 0 -- Disabled
        - 1 -- Enabled */

   uint16_t reserved;
}vpm_hdvoice_enable_param_info_t;

/****************************************************************************
 * CVP VP3 TRACKING                                                         *
 ****************************************************************************/

typedef struct vpm_sound_device_pair_t
{
   uint16_t direction;
   /**<
    * The direction of the sound device pair. The supported values:\n
    * CVP_SOUND_DEVICE_DIRECTION_RX \n
    * CVP_SOUND_DEVICE_DIRECTION_TX \n
    * CVP_SOUND_DEVICE_DIRECTION_RX_TX
    */
   uint32_t tx_device_id;
   /**<
    * Logical Tx sound device ID. This field is ignored if the sound device
    * direction is CVP_SOUND_DEVICE_DIRECTION_RX.
    */
   uint32_t rx_device_id;
   /**<
    * Logical Rx sound device ID. This field is ignored if the sound device
    * direction is CVP_SOUND_DEVICE_DIRECTION_TX.
    */
}
vpm_sound_device_pair_t;

typedef struct cvp_per_device_pair_vp3_t
{
   vpm_sound_device_pair_t sound_device_pair;
   /**< The sound device pair in which the VP3 data is associated with. */

   uint32_t                data_len;
   /**< VP3 data length. */

   uint8_t                 data[VPM_MAX_VP3_DATA_LEN];
   /**< VP3 data. */
}
cvp_per_device_pair_vp3_t;

typedef struct vpm_vp3_list_item_t
{
   apr_list_node_t link;
   cvp_per_device_pair_vp3_t *vp3;
}
vpm_vp3_list_item_t;

typedef struct vpm_vp3_info_t
{
   cvp_per_device_pair_vp3_t     *p_sound_dev_pair_info;
   vpm_vp3_list_item_t           list_item_pool[VPM_MAX_NUM_DEVICE_PAIRS_OF_VP3_STORED];
   apr_list_t                    list_free_q;
   apr_list_t                    list_used_q;
}vpm_vp3_info_t;

typedef struct vpm_sound_device_info_t
{
   bool_t is_available;
   /**<
     * Flag that indicates if the sound device pair information is available.
     * This flag is set to TRUE upon receiving the
     * VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG command and the sound device
     * information is available in the device configuration data.
     */
   vpm_sound_device_pair_t device_pair;
   /**< The sound device pair. */
}
vpm_sound_device_info_t;

typedef struct vpm_clk_control_config_t
{
   uint32_t minor_version;
   uint32_t size;
   uint32_t num_columns;
   void *columns;
}
vpm_clk_control_config_t;

typedef struct vpm_topo_param_info_t
{
   bool_t is_registered;
   /* Topology OOB param registered status flag. */
   void *vir_addr;
   /**< Topology OOB param data virtual address. */
   uint32_t mem_size;
   /**< Topology OOB param size. */
   vpm_create_param_t vpm_param[VPM_MAX_NUM_TOPO_PARAMS];

   uint32_t vpm_param_size;
}
vpm_topo_param_info_t;


/* Static info per session */
typedef struct vpm_session_static_info_t
{
   qurt_elite_queue_t *p_session_cmd_q;
   /**< Client to AudioStreamMgr cmd Q */

   qurt_elite_queue_t *p_session_resp_Q;
   /**< Other svcs to AudioStreamMgr response Q */

   uint32_t                session_cmdq_mask;
   /**< Mask for Session Cmd Q */

}vpm_session_static_info_t;


#endif /* #ifndef _VPM_TYPES_H_ */
