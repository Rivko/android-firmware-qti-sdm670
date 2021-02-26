/*========================================================================
  This file contains high level functions that are shared inside afe
  across drivers and services

  Copyright (c) 2013-2015,2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEInternal.h#2 $
 ====================================================================== */
#ifndef _AFE_INTERNAL_H_
#define _AFE_INTERNAL_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "Elite.h"
#include <stringl.h>
#include "AFEInterface.h"
#include "adsp_afe_service_commands.h"
#include "AFEPortManager.h"
#include "audio_basic_op_ext.h"
#include "AFEDynamicThread.h"
#include "AFEVfrHandler.h"
#include "AFEMmpm.h"
#include "AFEBgtHandler.h"
#include "AFEDeviceEventUtil.h"
#include "Elite_fwk_extns_feedback.h"
#include "algo_logging.h"
#include "AFEDeviceDriverInterface.h"
#include "EliteData_Util.h"

//#define AVT_BASED_AFE_DRIFT_DEBUG

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */
/* Maximum sampling rate in kHz */
#define AFE_MAX_SAMPLE_RATE	192

/* Maximum sampling rate for audio, Hz */
#define AFE_MAX_AUDIO_SAMPLE_RATE 48000

#define AFE_CLIP_MAX_REG_PER_BANK 		8 	/* Max number of registers present in a codec pcm level bank*/

/** Threshold above which if Rx buffers exist, they will be dropped. Also,
  if there is no Rx buffer but Tx buffers exist, zeros will be sent out
  in AFE_PSEUDOPORT_TIMER_MODE_THRESH milliseconds.
 */
#define AFE_PSEUDOPORT_TIMER_MODE_THRESH 25


/* Unity gain in Q13*/
#define AFE_UNITY_GAIN               0x2000
#define AFE_UNITY_GAIN_Q13           0x2000

/* Bytes per sample */
#define AFE_BYTES_PER_SAMPLE_TWO     2
#define AFE_BYTES_PER_SAMPLE_FOUR    4

/* Bits per sample */
#define AFE_BITS_PER_SAMPLE_16       16
#define AFE_BITS_PER_SAMPLE_24       24
#define AFE_BITS_PER_SAMPLE_32       32

#define BYTE_UP_CONV_SHIFT_FACT_DEFAULT   (ELITE_32BIT_PCM_Q_FORMAT - PCM_16BIT_Q_FORMAT) // (Q28 - Q15) for 24bit to 16bit conversion
#define BYTE_UP_CONV_SHIFT_FACT_24_BIT     (PCM_32BIT_Q_FORMAT - ELITE_32BIT_PCM_Q_FORMAT) // (Q31 - Q27) for 24bit to 32bit conversion
#define BYTE_UP_CONV_SHIFT_FACT_16_BIT     (PCM_32BIT_Q_FORMAT - PCM_16BIT_Q_FORMAT) // (Q31 - Q15) for 32bit to 16bit conversion


#define NUM_US_PER_SEC               (1000000)

/* Marco for ceil div, alter. (x + y - 1)/y includes overflow*/
#define ceil_div(x, y) ((x)==0 ? 0 : 1 + (((x) - 1) / (y)))

/**
 * max number of elements in the cmd or ack queue.
 */
static const uint32_t AFE_MAX_CMD_Q_ELEMENTS = 16;

/**
 * max number of elements in the cmd queue for AFE
 * Service
 */
static const uint32_t AFE_SVC_MAX_CMD_Q_ELEMENTS = 32;

/**
 * max number of elements in the rsp queue for AFE
 * Service
 */
static const uint32_t AFE_SVC_MAX_RSP_Q_ELEMENTS = 8;

/**
 * AFE thread stack size for static service thread and pseudoport threads
 */
static const uint32 AFE_THREAD_STACK_SIZE = 4096;


/*==========================================================================
  Structure definitions
  ========================================================================== */
typedef struct afe_dev_port afe_dev_port_t;

/** @brief Node for storing the linked list of clients. */
typedef struct bufQList
{
	void                *element;    /**< This element. */
	struct bufQList     *next;       /**< Pointer to the next node. */
} bufQList, afe_client_list;

/** Possible group device state. */
typedef enum
{
	AFE_GROUP_DEVICE_STATE_STOP = 0,   /**< All the ports in group are in STOP state */
	AFE_GROUP_DEVICE_STATE_CONFIG,     /**< Atleast one port is in the CONFIG state. */
	AFE_GROUP_DEVICE_STATE_RUN         /**< Atleast one port in RUN state */
} afe_group_device_state;

/** Possible port state. */
typedef enum
{
	AFE_PORT_STATE_STOP = 0,   /**< Port is in the Stop state. */
	AFE_PORT_STATE_CONFIG,     /**< Port is in the Config state. */
	AFE_PORT_STATE_RUN,        /**< Port is in the Run state. */
	AFE_PORT_STATE_SUSPEND     /**< Port is in the Suspend state. (Used to perform error recovery) */
} afe_port_state;

/** Possible port types. */
typedef enum
{
	AFE_UNCOMPRESSED_DATA_PORT = 0,         /**< Linear PCM data handling */
	AFE_IEC61937_COMPRESSED_DATA_PORT,  /**< Compressed IEC61937 packetized data handling */
	AFE_DSD_DOP_COMPRESSED_DATA_PORT,   /**< DSD over PCM (DOP) data handling */
	AFE_GENERIC_COMPRESSED_DATA_PORT    /**< Generic compressed data handling */
} afe_port_data_type;

/** Possible port loopback state. */
typedef enum
{
	AFE_PORT_LOOPBACK_STATE_STOP = 0,      /**< Port loopback is in the Stop
                                               state. */
	AFE_PORT_LOOPBACK_STATE_RUN            /**< Port loopback is in the Run
                                               state. */
} afe_port_loopback_state;

/** @brief Loopback information; applicable for a Src port only. */
typedef struct afe_port_loopback_info
{
	uint32_t							loopback_cfg_minor_version;
	/**< Loopback config minor version. */
	uint16_t                 	dst_port_id;
	/**< Required to reconfigure a loopback. */
	afe_port_loopback_state  afe_loopback_state;
	/**< Flag indicating, in general, whether a
                                  loopback is active.*/
	elite_svc_handle_t       	*dst_port_handle_ptr;
	/**< Handle to the Dst port in a loopback. */
	elite_svc_handle_t       	*src_port_handle_ptr;
	/**< Handle of the Src port in a loopback. */
	void                     	*src_port_client_ptr;
	/**< Client information of the Src port in a
                                  loopback. */
	uint16_t                 gain;
	/**< Gain for loopback in Q13 format*/
	uint16_t             		routing_mode;
	/**< Loopback routing mode */
	bool_t                  rate_matching_enable;
	/**< Loopback ratematching enable setting */
	void                      *sidetone_iir_info_ptr;
	  /**< structure for side tone IIR filter */
} afe_port_loopback_info;

typedef enum
{
	LOOPBACK_CFG_V1 = 0x1,
	LOOPBACK_CFG_V2 = 0x2
}afe_loopback_cfg_minor_version;


/** @brief Number of client counters. */
typedef struct afe_port_client_counters
{
	uint16_t             voice_mixer_clients;
	/**< Voice Rx client; for Rx port, 0 for Tx. */
	uint16_t             audio_mixer_clients;
	/**< Audio Rx client; for Rx port, 0 for Tx. */
	uint16_t             cust_client;
	/**< FBSP Vi proc client, for Tx/Rx port,  */
} afe_port_client_counters;

/* -----------------------------------------------------------------------
 ** Type Declarations
 ** ----------------------------------------------------------------------- */
typedef struct afe_dev_avt_drift
{
	uint64_t init_avtimer_timestamp;
	/**< AVTtimer timestamp for the first dma. */
	uint64_t  init_dev_time;
	/**< device time when first interrupt occurs. */
	uint32_t  init_fifo_samples;
	/**< FIFO samples when first interrupt occurs. */
#ifdef AVT_BASED_AFE_DRIFT_DEBUG
	uint64_t  dev_time;
	/**< device time when interrupt occurs. */
	uint64_t avtimer_timestamp;
	/**< AVTtimer timestamp each time 100th DMA interrupt occurs. */
#endif // AVT_BASED_AFE_DRIFT_DEBUG
} afe_dev_avt_drift_t;

typedef struct afe_data_logging
{
	uint16_t					  port_session_id_for_log;
	/**< session id for 0x13B0 PCM logging.
								Changes every port start/stop.*/
	uint16_t 		      	  bytes_per_int_all_ch;
	/**< total number of bytes per ch */
	int16_t                    *log_buf_ptr;
	/**< buf ptr */
	uint32_t                   log_size_used;
	/**< used size, bytes */
	uint32_t                   total_log_size;
	/**< total size of the log in bytes per diag commit. */
} afe_data_logging_t;

typedef struct afe_cont_buf_latency_log
{
	afe_algo_log_header_t	algo_log_header;

	afe_cont_buf_latency_stats_v1_t cont_buf_latency_stats;


}afe_cont_buf_latency_log_t;


typedef struct afe_cont_buf_latency_log_info
{
	uint32_t log_client_hdl;
	algo_data_logging_t *logging_ptr;
	afe_cont_buf_latency_log_t 	cont_buf_latency_log;
}afe_cont_buf_latency_log_info_t;


typedef struct afe_avt_drift_params
{
	bool_t                     is_avt_drift_updated;
	/**< enabling this flag sends a msg to the clients to read drift info
	                               & also triggers loopback rate matching if enabled. enabled every 100ms.*/
	afe_dev_avt_drift_t        *avt_drift_struct_ptr;
	/**< struct used for calculating drift for audioIf and slimbus port */

	uint32_t                   avtimer_latch_type;
	/**< whether AV timer based timestamp is  */

	uint64_t                   sw_latched_dev_time;
	/**< device time latched in software when the DMA interrupt occurs */

	uint32_t                   fifo_samples;
	/**< samples left in FIFO when the interrupt happens*/
    
    uint32_t                   resync_flag;
	/**< sets this flag to specify that resync is required on the 
         client side for drift correction; flag = 0 or 1.*/
    
    uint64_t                   avt_timestamp;
	/**< AVTtimer timestamp when accumlated drift is updated. */
    
} afe_avt_drift_params_t;


typedef struct tdm_slot_offset_info_t
{
	uint16_t                   offset_input;     //the original offset input from client
	//1st Part
	uint16_t                  offset_0;
	uint16_t                  copy_width_0;     //in bytes
	//2nd Part
	uint16_t                  offset_1;
	uint16_t                  copy_width_1;     //in bytes
}tdm_slot_offset_info_t;


typedef struct afe_slot_mapping_info_t
{
	uint16_t                  slot_bytes_per_channel;    //slot bitwidth per-channel.
	uint16_t                  slot_num_channel;         //num of channel of audio sample

	uint16_t                  data_align_type;

	tdm_slot_offset_info_t    offset_info[AFE_PORT_MAX_AUDIO_CHAN_CNT];

	bool_t                    is_cfg_received;
	/**< Received cfg params from client or not */
}afe_slot_mapping_info_t;

typedef struct afe_custom_header_info_t
{
	uint16_t                header_width_per_frame;  //in bytes, 2 bytes for MOST/TDM case

	tdm_slot_offset_info_t  header_offset;

	uint16_t                header_type;  //1 - default, 2 - entermaintment, 3 - other format, 4 - SAD herder is not insert in frame

	uint16_t                num_frame_repeat;

	void*                   p_header;
	//uint16_t    header[NUM_CUSTOM_TDM_HEADER_MAX];	  //hard code to 16 bytes now

	bool_t                  is_cfg_received;
	/**< Received cfg params from client or not */

	bool_t                  is_default_header_backup;
	/**< Default header backup for this group or not */

	uint8_t                 bit_width_from_header;
	uint8_t                 asad_from_header;
	uint8_t                 num_chan_from_header;

}afe_custom_header_info_t;

typedef struct afe_group_device_state_struct
{
	afe_dev_port_t                *p_dev_port[AFE_GROUP_DEVICE_NUM_PORTS];
	/**< Pointer to port stucts participating in group */
	uint16_t                      num_channels;
	/**< Number of channels for this group
                                 including all the sub-devices */
	uint16_t                      bytes_per_channel;
	/**< Bytes per channel */
	uint32_t                      int_samples_per_period;
	/**< Number of samples per interrupt; */

	uint32_t                      sample_rate;
	/** < Sampling rate of the group device */
	uint16_t                      bit_width;
	/** < Bit width of sample */
	void                          *p_dev_drv_state;
	/**< Pointer to the device driver object */
	qurt_elite_signal_t           dma_signal;
	/**< DMA signal for this group */
	afe_group_device_state        group_state;
	/**< DMA signal for this group */
	afe_dynamic_thread_t          *dynamic_thread_ptr;
	/**< Dynamic thread for group ports tasks.*/

	void                          *avt_drv;
	/**< Handle of the AV Timer driver. */
	void                          *err_handler_drv;
	/**< Handle of afe error handler driver. */
	bool_t                        get_hw_latched_vfr_cnt[NUM_MAX_VFR_SRC];
	/**< Flag to check if to read or not read the VFR count from
                                 h/w register */
	bool_t                        is_enabled;
	/**< Flag to indicate if group is in enable or disable state */
	bool_t                        is_config_rcvd;
	/**< Flag to indicate if group config is rcvd */
	int8_t                        *p_group_io_buf;
	/**< Working buffer for the group */
	uint16_t                      group_io_buf_size;
	/**< Size of the working buffer */
	uint16_t                      frame_size;
	/**< frame size for this group */
	uint16_t                      num_frame_in_group;
	/**< num of frames in this group */
	uint16_t                      group_id;
	/**< frame sync enable flag */
	afe_custom_header_info_t      default_header_backup;
	/**< default custom header backup */
	afe_dev_port_t               *p_first_active_port;
	/**< the first active port among group */

	afe_data_logging_t            data_log;
}afe_group_device_state_struct_t;

/** Types of Device process sequences */
typedef enum afe_dev_proc_seq_t
{
	AFE_NORMAL_SEQ = 0,     /**< Normal afe device process sequence (normal port start/ stop) */
	AFE_DEV_RESTART,        /**< Device restart sequence */
	AFE_DEV_RECOVERY        /**< Device recovery  */
} afe_dev_proc_seq_t;

typedef ADSPResult (*afe_dev_port_proc_func) (afe_dev_port_t* /*pDevport*/, int16_t* /*buf ptr*/, uint32_t /* number of samples*/);

/** @brief Snapshot of the ports. Each port is an instance of this
  structure. */
struct afe_dev_port
{
	afe_dev_proc_seq_t       dev_proc_seq;
	/**< Used to specify which kind of device process sequence it is.
	 * Whether device restart, device recovery or normal sequence */
	uint16_t                   is_first_interrupt;
	uint16_t					      is_interleaved;
	/** Device driver	input	is	inter-leaved? */
	afe_port_client_counters   client_counter;
	/**< counter for different kind of clients */
	qurt_elite_mutex_t         port_mutex;
	/**< Mutex to make sure that this port is handled in a
										  thread-safe manner when needed. */
	afe_port_state             port_state;
	/**< DMA state stop/start; flag = 0 or 1. */

	uint32_t                   latency_mode;
  /**< AFE port operating latency mode DEFAULT_LATENCY_MODE or LOW_LATENCY_MODE. */
	uint16_t                    		intf;
	/**< Interface ID for the port. */
	uint16_t                    		dir;
	/**< Tx/Rx direction for the port. */
	bool_t                         is_port_media_type_set;
	/**< Flag indicating whether the port media type was set by an explicit set param. */
	uint32_t               		sample_rate;
	/**< Port sampling rate. */
	uint32_t               		channels;
	/**< Port #ch: Mono, stereo, or 5.1.  */
	uint16_t               		bytes_per_channel;
	/**< Port 16-, 24-, or 32-bit format. */
	uint16_t               		bit_width;
	/**< Port bit width */
	uint32_t               		dev_sample_rate;
	/**< Device sampling rate. */
	uint32_t               		dev_channels;
	/**< Device #ch: Mono, stereo, or 5.1.  */
	uint16_t               		dev_bytes_per_channel;
	/**< Device 16-, 24-, or 32-bit format. */
	uint16_t               		dev_bit_width;
	/**< Device bit width */
	uint16_t               		q_format_shift_factor;
	/**< q format shift factor for 24bit and 32bit formats. */
	uint32_t               		int_samples_per_period;
	/**< Number of samples per interrupt;*/
        uint32_t               		dev_int_samples_per_period;
	/**< Number of device samples per interrupt;*/
	uint64_t                   port_cmn_group_delay_us;
	/**< Common group delay in us. */
	uint32_t                   dev_hw_delay_us;
	/**< device hw delay in us. */
	uint64_t 					   port_av_timestamp;
	/**< AVTtimer timestamp each DMA interrupt*/
	int8_t               		*working_buf_1;
	/**< working Buffer 1 to copy from/to the DMA. */
	int8_t                 		*working_buf_2;
	/**< working Buffer 2 to copy from/to the DMA. */
	int8_t                 		*scratch_buf;
	/**< scratch Buffer for AFE processing */
	uint32_t                  scratch_buf_size;
	/**< size of the scratch Buffer */
	int8_t                 		*group_device_out_buf_ptr;
	/**< pointer contains the pointer of output buffer */
	uint32_t               		num_client;
	/**< Total number of clients. */
	bufQList               		*afe_rx_client_list_ptr;
	/**< Rx client list; only for the Rx path. */
	bufQList               		*afe_tx_client_list_ptr;
	/**< Tx path: list of all the clients in the Tx path;
		Rx path: list of all the clients for the far-end
						 	 	 	 	 	 path. */
	feedback_client_list_t       *afe_cust_client_list_ptr;
	/**< FBSP VI client list  */
	int                    		thread_id;
	/**< Thread ID for the port. */
	void*                  		afe_drv;
	/**< Handle of the device driver. */
	void*                  		avt_drv;
	/**< Handle of the AV Timer driver. */
	void*                  		avsync_test_ptr;
	/**< pointer for AV sync testing statistics in SIM testing. */
	void*                     err_handler_drv;
	/**< Handle of afe error handler driver. */
	afe_dev_port_proc_func 		afe_dev_process;
	/**< pointer for AV sync testing statistics in SIM testing. */
	afe_driver_func_tbl_t        *afe_drv_ftbl;
	/**< Handle of the device driver. */
	qurt_elite_signal_t        dma_signal;
	/**< The port-corresponding DMA signal
		instance, OR the pointer to the timer interrupt
		generated signal; applicable for pseudo ports.*/
	qurt_elite_timer_t         *timer_ptr;
	/**< Pointer to the timer used for the pseudo port
		in real-time mode. **/
	uint64_t					init_port_clock;
	/** <init_port_clock value. used for psuedo port timer calculations. This denotes the origin time of the port.
	Based on this origin time, the one shot timer of psuedo port is set each time*/
	afe_port_loopback_info     *loopback_info_ptr;
	/**< TX port: of type: afe_tx_port_loopback_info.
	 * Loopback-related information; if the value is NULL,
	   the loopback is not enabled at all; if the
	   value is not NULL, the loopback is enabled, but may not be
	   running if either port is stopped.
	   RX port: NULL*/
	void                     	*dtmf_info_ptr;
	/**< DTMF ptr: It is created when DTMF config is received and destroyed during port deinit */
	capi_v2_t * limiter_ptr;
	/**< pointer to the limiter capi_v2 instance for
	audio+voice; required for Rx ports; Tx is NULL.*/

	bool_t			   limiter_enable;
	/** <flag to hold limiter enable/disable. */
	void                       *vfr_drv_handle;
	/**< handle to the audio if vfr driver. same as afe_drv for audio_if ports,
	 * but for non-audio_if ports it's handle of proxy port on audio if driver.
	 * type is AFEDeviceHandle_t unlike afe_drv which is AFEDeviceHandle_t* */
	afe_drift_info_t           drift_info[NUM_MAX_VFR_SRC];
	/**< drift info. pointer shared with clients */
	uint16_t                   is_send_eos_to_tx;
	/**< A flag used for routing RX eos signals to the TX side for pseudoports. */

	uint16_t                  loopback_cached_gain;
	/**< This variable to store loopback gain
	if apps sends the gain cmd before enabling loopback */
	afe_port_data_type			dev_data_type;
	/**< enumeration to store the data type of the device */
	afe_port_data_type			port_data_type;
	/**< enumeration to store the data type of the port */
	uint16_t                   periodic_int_counter;
	/**< periodic interrupt counter. incr every ms of dma int*/
	uint32_t                   running_int_counter;
	/**< running interrupt counter. incr every ms of dma int*/
	afe_avt_drift_params_t     avt_drift_params;
	/**< struct containing the AV timer drift params */
	//void                       *afe_rmc_ptr;
	/**< struct ptr used for holding RemoveClick module state and config info */
	bool_t                     is_first_dma;
	/**< is it first dma? */
	void							   *sw_mad_ptr;
	/**< struct ptr used for holding sw mad related information*/
	void							   *hw_mad_ptr;
	/**< struct ptr used for holding hw mad related information*/
	void							   *bgt_handle;
	/**< Handle to the background thread information */
	void	                     *mmpm_info_ptr;
	/**< Handle to the MMPM information for this port */
	afe_data_logging_t          	data_log;
	afe_cont_buf_latency_log_info_t		*cont_buf_latency_log_info_ptr;

	afe_event_client_list_t *port_evt_client_list;
	/**< port event client list */

	void 					*port_topo_ptr;
	/**< Pointer to the topo object */
	uint32_t						topo_rcvd_flag;
	/**< Flag to represent whether topology ID received by AFE port or not */

	afe_port_multi_feedback_info_t *feedback_path_info_ptr;
	/**< Pointer to the feedback path info */
	afe_dynamic_thread_t     *dynamic_thread_ptr;
	/**< Dynamic thread for port tasks.*/

	afe_group_device_state_struct_t *p_group_device_state;
	/**< ptr to group device object */

	uint16_t frame_idx;
	/**< This AFE port is associated to which frame. The index is from 0. The default is 0. */

	afe_slot_mapping_info_t slot_mapping_info;
	/*tdm slot mapping info */

	afe_custom_header_info_t custom_header_info;
	/*tdm custom header info */

	void  *sw_duty_cycle_data_ptr;
	/**< struct ptr used for holding sw duty cycle related information*/

	uint32_t working_buf_size;
	/** port's working buffer size*/

	void *danger_info_ptr;
	/** port's information structs to handle danger gen info */

	void *port_enc_dec_ptr;
	/** pointer to the port's encoder/decoder structure.
	 *    this will be non-null in the case of pcm->compressed use case (rx path, encoding) or
	 *    this will be non-null in the case of compressed->pcm use case (tx path, decoding)
	 *    this pointer will get initialized (if applicable) during the AFE start command
	 *    this will be null for all other use cases*/
};

/** @brief AFE device service declarations. */
typedef struct aud_stat_afe_svc
{
	elite_svc_handle_t   afe_service_handle;
	/**< Handle to give out to others. */
	elite_svc_handle_t   cdc_service_handle;
	/**< codec service handle to give out to others. */
	afe_dev_port_t       *ports_audioif_ptr[AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE];
	/**< pointers to audio if Ports*/
	afe_dev_port_t       **ports_audioif_tdm_ptr;
	/**< pointers to audio if TDM Ports*/
	uint32_t              num_audioif_tdm_port;
	/**< Number of audio if TDM Ports existed in the above pointer */
	afe_dev_port_t       *ports_riva_ptr[AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE];
	/**< pointer to internal bt-fm port objects. */
	afe_dev_port_t       *ports_slimbus_ptr[AFE_PORT_ID_SLIMBUS_RANGE_SIZE];
	/**< pointer to SLIMBUS port pool objects. */
	afe_dev_port_t       *port_rt_proxy_ptr[AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE];
	/**< port instance pointer array for real-time device running on apps*/
	afe_dev_port_t       *port_hdmi_output_ptr[AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE/2];
	afe_dev_port_t       *pseudo_ports_ptr[AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE];

	afe_dev_port_t       *ports_usba_ptr[AFE_PORT_ID_USB_AUDIO_RANGE_SIZE];
	/**< pointer to USB audio port pool objects. */

	/**< Pseudo port instance. */
	afe_dev_port_t       *ports_spdif_ptr;
	/**< pointers to spdif Ports*/
	qurt_elite_channel_t channel;
	/**< channel belonging to the AFE service. */
	elite_apr_handle_t   apr_handle;
	/**< AFE service APR handle. */
	qurt_elite_queue_t   *static_svc_cmdq_ptr;
	/**< static svc cmdQ */
	qurt_elite_queue_t   *static_svc_rspq_ptr;
	/**< static svc rspQ */
	qurt_elite_queue_t   *aanc_cmdq_ptr;
	/**< cmdQ for handling Adaptive ANC */
	uint32_t             afe_memory_map_client;
	/**< AFE Memory map client. */
	qurt_elite_queue_t   *rt_port_mgr_q;
	/** RT port data,buf q pointer to route the apr data msg */
	afe_bgt_svc_info_t   *afe_bgt_pool_ptr;
	/** Pointer to afe bgt pool */
} aud_stat_afe_svc_t;

/** Global pointer to the static service. */
extern aud_stat_afe_svc_t     *afe_svc_global_ptr;

/**
  Gets the device port instance, given the port ID and direction.

   This function gives the ptr to the instance of the device port.

  @param[in]  port_id  Port ID defined in AFE APR API.
  @param[out] dev_port  Pointer to the pointer of the port.

  @result
  ADSPResult.

  @dependencies
  None.
 */
ADSPResult afe_svc_get_port(uint16 unPortId, afe_dev_port_t **dev_port);

/**
  Initialize the port struct for defaul values and also register
  the task with thread pool

  @param[in]  pDevPort      device port instance.
  @param[in]  portInt       port interface id
  @param[in]  portDir       port direction

  @result
  ADSPResult indicating whether the port initialization was
  successfully handled

  @dependencies
  None.
 */
ADSPResult afe_svc_init_port(afe_dev_port_t *pDevPort, uint16_t portInt, uint16_t portDir);

/**
  De-initialize the device port

  @param[in]  pDevPort      device port instance.

  @result
  ADSPResult indicating whether the port initialization was
  successfully handled

  @dependencies
  None.
 */
ADSPResult afe_svc_deinit_port(afe_dev_port_t *pDevPort);

/**
 * function to get AFE APR handle.
 *
 * @return return code elite_apr_handle_t
 */
elite_apr_handle_t   afe_svc_get_apr_handle(void);

/**
  deinitialization to be done at port stop.

  @param[in]  pDevPort       port instance

  @result
  ADSPResult error code.

  @dependencies
  None.
 */
ADSPResult afe_port_de_init_at_port_stop(afe_dev_port_t* pDevPort);

/**
  Initialization of sample rate dependent modules

  @param[in]  pDevPort       port instance

  @result
  ADSPResult error code.

  @dependencies
  None.
 */
ADSPResult afe_port_de_init_sample_rate_dependent_modules(afe_dev_port_t* pDevPort);

/**
  De-Initialization of sample rate dependent modules

  @param[in]  pDevPort       port instance

  @result
  ADSPResult error code.

  @dependencies
  None.
 */
ADSPResult afe_port_re_init_sample_rate_dependent_modules(afe_dev_port_t* pDevPort);

/**
  Initialization to be done at port start.

  @param[in]  pDevPort       port instance

  @result
  ADSPResult error code.

  @dependencies
  None.
 */
ADSPResult afe_port_init_at_port_start(afe_dev_port_t* pDevPort);

/**
 * converts samples to time in us using us per sample in Q25.
 * @param[in] time_per_sample, microseconds per sample in Q25
 * @param[in] samples, number of samples to be converted.
 * @param[out] samples converted to time.
 */
int64_t afe_convert_sample_to_time (uint32_t time_per_sample, int32_t samples);

static   inline     void afe_svc_print_status_message(ADSPResult result)
{
	if (ADSP_FAILED(result))
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " message FAIL %8x", result );
	else
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " message SUCCESS %8x", NULL );
}

/* Function to retrive  the address where calib data need to be stored for get param command
@param[in]  apr_pkt_ptr       apr packet received from client 

@param[out] payload_address  address where calib data need to be stored 

@param[out] ack_apr_pkt_ptr  Apr packet to return to client in case of inband get param, for outband this is NULL

 */


ADSPResult afe_port_get_calibdata_address(elite_apr_packet_t *apr_pkt_ptr, uint32_t *payload_address,\
		elite_apr_packet_t **ack_apr_pkt_ptr);


/* Function to send  calib data  to client/HLOS
@param[in]  apr_pkt_ptr       apr packet received from client 

@param[out] payload_address  address where calib data need to be stored 

@param[out] ack_apr_pkt_ptr  Apr packet to return to client in case of inband get param, for outband this is NULL

 */
ADSPResult afe_port_send_calibdata_to_client(elite_apr_packet_t *apr_pkt_ptr,uint32_t param_result,\
		elite_apr_packet_t *ack_apr_pkt_ptr);

/* Function to configure timer for timer mode and real time mode
    and launch dynamic thread for real time pseudoports
   @param[in/out]  pDevPort       Devport instance */
ADSPResult afe_pseudo_port_create_timer_and_dyn_thread(afe_dev_port_t *pDevPort);

/* Function to delete timer for timer mode and real time mode and
   destroy dynamic thread for real time pseudoports 
   @param[in/out]  pDevPort       Devport instance */
ADSPResult afe_pseudo_port_destroy_timer_and_dyn_thread(afe_dev_port_t *pDevPort, bool_t stop_device);

/**
 * to acquire mutex & elevate the prio of static svc thread.
 * This function should be called only by AfeS thread.
 */
static inline void afe_port_mutex_lock(afe_dev_port_t *pDevPort)
{
	qurt_elite_mutex_lock( &(pDevPort->port_mutex));
}

/**
 * to release mutex & decrease the prio of static svc thread.
 * This function should be called only by AfeS thread.
 */
static inline void afe_port_mutex_unlock(afe_dev_port_t *pDevPort)
{
	qurt_elite_mutex_unlock( &(pDevPort->port_mutex));
}

/*=============================================================================
 FUNCTION      void afe_port_multiply_saturate

 DESCRIPTION   apply gain (Q13) to the input buffer (Q15), shift  and place the
 result in the output buffer (Q15). Optimized for Q6 by loading 2
 values of input at the same time.
 OUTPUTS

 DEPENDENCIES  None

 RETURN VALUE  None

 SIDE EFFECTS
 ===============================================================================*/
void afe_port_multiply_saturate(int16_t* outptr, int16_t* inptr, uint16_t gain, int32_t samples);


/*=============================================================================
 FUNCTION      void afe_port_multiply_saturate_int32
 Right now the function in CSoftVolumeControlsLib is for Q12, not Q13 as we need it.

 DESCRIPTION   apply gain (Q13) to the input buffer (32-bit PCM),
 output buffer (32-bit).
 OUTPUTS

 DEPENDENCIES  None

 RETURN VALUE  None

 SIDE EFFECTS
 ===============================================================================*/
void afe_port_multiply_saturate_int32(int32_t* outptr, int32_t* inptr, uint16_t gain,
		int32_t samples);


/*=============================================================================
 FUNCTION      void afe_port_send_media_t

 DESCRIPTION   propagate media format from port to client
 OUTPUTS

 DEPENDENCIES  None

 RETURN VALUE  ADSPResult

 SIDE EFFECTS
 ===============================================================================*/
ADSPResult afe_port_send_media_t(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo);

/*=============================================================================
 FUNCTION      bool_t afe_is_sample_rate_fract

 DESCRIPTION   Checks if samplerate is integer or fractional
 OUTPUTS	   FALSE or TRUE based on whether samplerate is fractional

 DEPENDENCIES  None

 RETURN VALUE  bool_t

 SIDE EFFECTS
 ===============================================================================*/ 
bool_t afe_is_sample_rate_fract(uint32_t sample_rate);

/*=============================================================================
 FUNCTION      bool_t afe_is_fractional_resampling_needed

 DESCRIPTION   Checks if the combination of SR will need fractional resampling
 OUTPUTS	   FALSE or TRUE 

 DEPENDENCIES  None

 RETURN VALUE  bool_t

 SIDE EFFECTS
 ===============================================================================*/ 
bool_t afe_is_fractional_resampling_needed(uint32_t sample_rate_1, uint32_t sample_rate_2);

/**
 * This function is to initialize modules and to start the device
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return    ADSP_EOK error code
 */
ADSPResult afe_port_modules_init_and_dev_start(afe_dev_port_t  *pDevPort);

/**
 * This function is to de-initialize modules and to stop the device
 *
 * @param[in]  pDevPort,  pointer to port instance
 * @return     ADSP_EOK error code
 */
ADSPResult afe_port_modules_deinit_and_dev_stop(afe_dev_port_t *pDevPort);

/**
 * This function is to process DMA STOP command
 *
 * @param[in] pDevPort,  pointer to port instance
 * @param[in] pkt_ptr, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_apr_dma_stop(afe_dev_port_t* pDevPort,elite_apr_packet_t* pkt_ptr);


/*=============================================================================
 FUNCTION      ADSPResult afe_svc_apr_set_param_handler

 DESCRIPTION   apr parameter handler
 OUTPUTS

 DEPENDENCIES  None

 RETURN VALUE  ADSPResult

 SIDE EFFECTS
 ===============================================================================*/
ADSPResult afe_svc_apr_set_param_handler(elite_apr_packet_t* pkt_ptr);

/*=============================================================================
 FUNCTION      ADSPResult afe_ports_apr_dma_restart

 DESCRIPTION   restart DMA
 OUTPUTS

 DEPENDENCIES  None

 RETURN VALUE  ADSPResult

 SIDE EFFECTS
 ===============================================================================*/
ADSPResult afe_ports_apr_dma_restart (elite_apr_handle_t *apr_handle_ptr, elite_apr_packet_t *pkt_ptr);

/** a utility function to validate all of the currently connected clients on the port */
ADSPResult afe_port_client_validation(afe_dev_port_t *pDevPort, uint16_t direction);

/*=============================================================================
 * FUNCTION         afe_port_copy_media_type_dev_to_port
 *
 * DESCRIPTION    utility function to copy device media type -> port media type (if applicable)
 * Param ID: #AFE_PARAM_ID_PORT_MEDIA_TYPE set on module ID: #AFE_MODULE_PORT
 * is used by HLOS client to explicitly set the port media type.
 * If HLOS client does not set this port media type explicitly, this utility function helps the driver code to
 * overwrite the port media type with the device media type.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_port_copy_media_type_dev_to_port(afe_dev_port_t* pDevPort);

#ifdef __cplusplus
}
#endif //__cplusplus

/** @} */ /* end_addtogroup AFEInternal */
/** @endcond */

#endif /* _AFE_INTERNAL_H_ */
