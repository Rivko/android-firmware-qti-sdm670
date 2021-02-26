#ifndef _AFE_RT_PROXY_DRIVER_I_H_
#define _AFE_RT_PROXY_DRIVER_I_H_
/*========================================================================
 This file is afe device driver layer internal interface for real-time proxy ports (mainly USB)

 RTPPDM -> Realtime proxy port data manager

  Copyright (c) 2010 -2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/rt_proxy/src/AFERtProxyDriver_i.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  03/15/11   rb      Created file.

  ========================================================================== */
/**
@file AFERtProxyDriver_i.h

@brief has internal declarations, functions for real time proxy port
*/

#include "AFERtProxyDriver.h"
#include "AFEDeviceDriver.h"
#include "AFEInternal.h"
#include "AFEDevService.h"
#include "AFEInterface.h"
#include "qurt_elite.h"
#include "adsp_afe_service_commands.h"
#include "EliteMsg_AfeCustom.h"
#include "mmdefs.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define     AFE_RTPPDM_DATA_BUF_Q_NAME             "AfeRtPrPtDatMgr"
#define     AFE_RTPPDM_Q_ELEMENTS                  16
#define     AFE_RTPPDM_THREAD_NAME                 "AfeDataMgr"
#define     AFE_RTPPDM_THREAD_STACK_SIZE           4096
#define     AFE_RTPP_BUF_SIZE_INT_FACTOR           2      //number of times frame_size is multiplied for integer rates
#define     AFE_RTPP_BUF_SIZE_FRACT_FACTOR         3      //number of times frame_size is multiplied for fractional rates
#define     AFE_RTPP_DRIFT_ALLOWANCE_MS            5      // +/-5ms of drift allowed, arbitrarily chosen.
#define     AFE_RTPP_VFR_HAND_OFF_DRIFT_THRESH     500    //1/2 ms threshold of drift for handoff detection
#define     AFE_RTPP_VFR_DRIFT_THRESH              125    //if drift is >=125 or <=-125, then drift is reported
#define     AFE_RTPP_64_BIT_MAX                    0xFFFFFFFFFFFFFFFFLL //0x FFFF FFFF FFFF FFFF
#define     AFE_RTPP_FUTURE_TIMER_GAP_US           200    //gap between the device clock set for future to wall clock: 1/2 ms
#define     AFE_RTPP_MAX_SET_TIMER_ATTEMPTS        10       //max trials of afe_start_abs_timer

//logging with tapping into files at 4 points.
//#define     DEBUG_FILE_LOG

//logging for VFR debug
//#define     RTPP_DEBUG_VFR

//logging for timer adjustment debug
//#define     RTPP_DEBUG_TIMER_ADJUST

//logging for all underrun/overrun (one underrun/overrun is logged if per client data/buffer by default)
#define     RTPP_MEDIUM_DEBUG

/** @brief Enums for RT proxy interface cfg versions */
typedef enum
{
	RTPROXY_CFG_V1 = 0x1
}afe_param_id_rtproxy_cfg_minor_version;

//verbose logging
//#define     RTPP_VERBOSE_DEBUG
/**
 * data manager struct
 */
typedef struct
{
   elite_svc_handle_t            data_mgr_handle;
   qurt_elite_channel_t               data_mgr_channel;
   elite_apr_addr_t              apr_self_addr;
   elite_apr_port_t              apr_self_port;
   uint32_t                      event_token;
} afe_rtppdm_t;

typedef struct afe_rtp_vfr_info
{
   int64_t                 curr_vfr_net_timer_adjust;
                                                            /**< us. timer adjustments made at current VFR */
   int64_t                 prev_vfr_net_timer_adjust;
                                                            /**< us. timer adjustments made since last VFR */
#ifdef RTPP_DEBUG_VFR
   uint64_t                vfr_curr_pcycles;                /**< pcycles during current VFR*/
#endif

}afe_rtp_vfr_info_t;

/**
 * Real time proxy port
 */
typedef struct afe_rt_proxy_port_dev
{
   uint16_t                port_id;             /**< port id for debug purpose */
   afe_port_state          port_state;          /**< data mgr's knowledge about port state */

   elite_apr_addr_t        apr_client_addr;     /**< APR client address */
   elite_apr_port_t        apr_client_port;     /**< APR client port number */

   uint16_t                interleaved;         /**< data exchanged between AFE & RT port is interleaved or not.*/
   uint32_t                frame_size;          /**< a single frame size in bytes (including all channels).*/
   uint32_t                jitter_allowance;    /**< jitter alloance in bytes including all channels.*/
   uint32_t                low_water_mark;      /**< low water mark in bytes (including all channels).*/
   uint32_t                high_water_mark;     /**< actual high water mark in bytes (including all channels). = total_circ_buf_size - config_high_water_mark*/
   uint32_t                config_high_water_mark;         /**< high water mark in bytes (including all channels) configured by the client*/
#if 0
   QURT_ELITE_ALIGN(uint8_t     device_channel_mapping[8], 8);  /**< Each element K of the array describes channel K inside the buffer where 0<=K<num_channels.
                                                           * The buffer has samples of different channels ordered in this way. This is the device channel mapping.*/
   QURT_ELITE_ALIGN(uint8_t     stream_channel_mapping[8], 8);  /**< Each element K of the array describes channel K inside the buffer where 0<=K<num_channels.
                                                          * The buffer has samples of different channels ordered in this way. This is the stream channel mapping.*/
#endif
   uint16_t                bytes_per_sample;    /**< number of bytes per samples across channels: bytes_per_channel*num_channel*/

   afe_circ_buff_t         circ_buf;            /**< circular buffer pointer. This buf is accesses by port manager on one side & RTPPDM on another side. */
   qurt_elite_mutex_t           circ_buf_mutex;      /**< circular buf mutex */

   uint8_t                 is_low_water_mark_event_sent;    /**< to control the amount of underrun messages */
   uint8_t                 is_high_water_mark_event_sent;   /**< to control the amount of overrun messages */

   uint8_t                 was_first_write_done;            /**< whether write to the circ buf was done at least once, so that reading can begin from half-way across */
   uint8_t                 was_first_read_done;             /**< helps adjust the read ptr to be half-away from the write ptr during first read (used in rx path. in tx: not needed bcoz we read only after they start writing)). */

   uint8_t                 *scratch_buffer_ptr;             /**< scratch buffer for interleaving/deinterleaving data */

   uint32_t                upper_drift_thresh;              /**< in bytes. thresh above which if buffer is full, drift is corrected */
   uint32_t                lower_drift_thresh;              /**< in bytes. thresh below which if buffer is full, drift is corrected */
   uint32_t                ideal_fullness;                  /**< in bytes. num of bytes to be present in circ buf ideally */
   uint32_t                real_fullness;                   /**< in bytes. num of bytes to be present in circ buf in reality */

   uint32_t                timer_adjust_counter;            /**< us. timer will be adjusted when this count reaches 0 from DRIFT_ADJUST_INTERVAL */
   uint32_t                timer_adjust_counter_constant;   /**< us. constant value of above for reload */
   int64_t                 timer_adjust_value;              /**< us. value of which timer will be adjusted every (1ms) frame */
   int64_t                 net_timer_adjust;                /**< us. timer adjustments made since port start=av timer drift. +ve -> device slower than avtimer.*/
   uint64_t                device_clock;                    /**< us. clock for this device. in us. this is the clock matched to USB clock*/

   afe_rtp_vfr_info_t      vfr_info[NUM_MAX_VFR_SRC];

   uint32_t                afe_byte_cnt;                    /**< num of bytes read/written by at AVT interrupt. includes underrun/overrun. i.e., if 10 bytes were tried to be read, but only 8 was available, this will be 10*/
   uint32_t                dev_byte_cnt;                    /**< num of bytes read/written by rt device on apps. includes underrun/overrun */

   uint16_t				   bit_width;
   uint32_t   			   dev_cfg_minor_ver;
   uint32_t                interrupt_counter;               /**< interrupt counter to indicate number of avtimer interrupts */
   uint64_t                init_device_clock_us;				/**< rt proxy device clock in us */
} afe_rt_proxy_port_dev_t;

/*=====================================================================
 Global Variables
 ======================================================================*/
extern afe_dev_port_t            port_rt_proxy[AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE];   //this is for the AFE svc purpose
extern afe_rt_proxy_port_dev_t   dev_rt_proxy[AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE];   //this is for the purpose of RT proxy port Data Mgr (indices correspond)

/*=====================================================================
 Functions
 ======================================================================*/

ADSPResult afe_rt_proxy_port_read(afe_dev_port_t *dev_port, int16_t *buf_ptr, uint32_t num_samples);
ADSPResult afe_rt_proxy_port_write(afe_dev_port_t *dev_port, int16_t *buf_ptr, uint32_t num_samples);
/**
 * Real Time Proxy Port Data Manager
 */
/** below functions run in calling threads context */
ADSPResult afe_rtppdm_init(void);
ADSPResult afe_rtppdm_deinit(void);
ADSPResult afe_rtppdm_port_start(uint16_t port_id);
ADSPResult afe_rtppdm_port_stop(uint16_t port_id);

/** below functions run in RTPPDM thread context */
ADSPResult afe_rtppdm_destroy (void *data_mgr, elite_msg_any_t* pMsg);
int        afe_rtppdm_work_loop (void* vp_afe_rtppdm);
   /** APR commands */
ADSPResult afe_rtppdm_apr_cmd_handler (void* data_mgr, elite_msg_any_t* pMsg);
ADSPResult afe_rtppdm_read_handler(elite_apr_packet_t* pkt_ptr);
ADSPResult afe_rtppdm_write_handler(elite_apr_packet_t* pkt_ptr);
ADSPResult afe_rtppdm_register_handler(elite_apr_packet_t* pkt_ptr);
ADSPResult afe_rtppdm_unregister_handler(elite_apr_packet_t* pkt_ptr);
   /** Custom commands */
ADSPResult afe_rtppdm_custom_cmd_handler (void* data_mgr, elite_msg_any_t* pMsg);
ADSPResult afe_rtppdm_destroy_handler (void *data_mgr, elite_msg_any_t* pMsg);
   /** Events */
ADSPResult afe_rtppdm_send_event (uint16_t port_id, uint16_t event_type);
static inline ADSPResult afe_rtppdm_send_low_watermark_event(uint16_t port_id)
{
   return afe_rtppdm_send_event(port_id, AFE_EVENT_TYPE_RT_PROXY_PORT_LOW_WATER_MARK);
}
static inline ADSPResult afe_rtppdm_send_high_watermark_event(uint16_t port_id)
{
   return afe_rtppdm_send_event(port_id, AFE_EVENT_TYPE_RT_PROXY_PORT_HIGH_WATER_MARK);
}
ADSPResult afe_rtppdm_send_high_watermark_event(uint16_t port_id);

   /**< Utilities */
/**
 * utlility to validate port condition before read/write
 */
ADSPResult  afe_rtppdm_validate_port(uint16_t port_id, uint16_t expected_dir, \
                           afe_dev_port_t **dev_port_ptr_ptr, afe_rt_proxy_port_dev_t **rt_proxy_dev_ptr_ptr);

/**
 * to get virtual address
 */
ADSPResult afe_rtppdm_map_phy_addr(uint32 mem_handle, uint32_t phy_address_lsw, uint32_t phy_address_msw, uint32_t size, uint32_t* virtual_addr, uint16_t port_id);

/**
 * data formatting.
 */
void afe_rtppdm_deinterleave(uint8_t* src_addr, uint8_t* dst_addr, 
                             uint32_t num_channels, uint32_t num_bytes_per_sample, 
                             uint32_t num_bytes, uint32_t dst_buf_size_in_bytes);

void afe_rtppdm_interleave(uint8_t* src_addr, uint8_t* dst_addr, 
                           uint32_t num_channels, uint32_t num_bytes_per_sample, 
                           uint32_t num_bytes, uint32_t dst_buf_size_in_bytes);

ADSPResult  afe_rt_proxy_channel_mapping(uint8_t *src_ch_map, uint8_t *dest_ch_map, uint16_t bytes_per_ch_samples, \
         uint8_t *buffer, uint16_t num_channels, uint16_t buf_size);
ADSPResult afe_rt_proxy_port_timer_adjustment(afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr, afe_dev_port_t *dev_port);
ADSPResult afe_rt_proxy_port_get_adj_val(uint32_t *adj_counter, int64_t* adj_val, uint32_t sample_rate);

/** timer resolution in microseconds*/
#define TIMER_RESOLUTION_US      1

/** every DRIFT_ADJUST_INTERVAL timer is adjusted. in num of blk_transfer_time frames
 * 200 ppm design: if drift is adjusted every 4 ms by 1us, then 200 ppm drift can be corrected.
 * following value works for 1ms blk transfer time. */
#define DRIFT_ADJUST_FRAME       4     //4 for 200ppm design, 1 for 500ppm
#define DRIFT_ADJUST_INTERVAL    (DRIFT_ADJUST_FRAME*TIMER_RESOLUTION_US)

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_RT_PROXY_DRIVER_I_H_ */

