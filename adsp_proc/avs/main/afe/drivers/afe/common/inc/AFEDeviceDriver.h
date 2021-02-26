#ifndef __AFE_DEVICE_DRIVER_H__
#define __AFE_DEVICE_DRIVER_H__
/**
@file AFEDeviceDriver.h **
This module contains AFEDevice_driver, an implementation of the
audio hardware driver.
*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/inc/AFEDeviceDriver.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/22/09    GY      Port AFE/HAL unified API changes from dev branch
07/13/09    RA      Retract changes for sidetone optimization
07/01/09    AY      Sidetone optimization
06/03/09    AY      Merge the critical section change for ECRefSource and
                    and sidetone.
04/15/09    AY      Add header tag
04/09/09    AY      Sidetone support for both mono and stereo data
11/06/07    RA      Created file

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "AFEDevService.h"
#include "AFEInternal.h"
#include "avtimer_drv_api.h"
#include "AFECodecService.h"
#include "DDIGPIOInt.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DDITlmm.h"
#include "clock_manager.h"

#define AFE_TIME_SRVR_AV_TIMER


#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Struct for circular buffer type. This always has interleaved data (multi-channel)
 */
typedef struct {
   uint8_t*                  head_addr;
                             /**< head pointer */
   uint8_t*                  tail_addr;
                             /**< tail pointer */
   uint8_t*                  top_addr;
                             /**< top pointer */
   uint8_t*                  bottom_addr;
                             /**< bottom pointer */
   uint32_t                  buffer_size;
                             /**< buffer size : bytes */
} afe_circ_buff_t;


/**
 * Struct for apr source and destination address information 
 */
typedef struct afe_port_event_addr_info
{
   elite_apr_addr_t              apr_self_addr;
   elite_apr_port_t              apr_self_port;
   elite_apr_addr_t              apr_client_addr;
   elite_apr_port_t              apr_client_port;
} afe_port_event_addr_info_t;


typedef struct afe_dev_port_config
{
   afe_port_data_type   	     port_data_type;
   uint32_t                     sample_rate;
   uint32_t                     channels;
   uint16_t                     bytes_per_channel;
   uint16_t                     bit_width;      
   uint16_t                     q_format_shift_factor;
   uint16_t                     is_interleaved;
   uint16_t                     frame_idx;
}afe_dev_port_config_t;


typedef struct afe_dev_timestamp_info
{
   uint64_t            dma_int_timestamp;
   uint32_t            fifo_samples;
}afe_dev_timestamp_info_t;


typedef struct afe_dev_restart_info
{
   uint32_t            sample_rate;
}afe_dev_restart_info_t;


typedef struct afe_dev_grp_config_info
{
   uint16_t                      group_id;
   int8_t *param_buffer_ptr; // input/output i2s config params which will be modified for 
   uint32_t param_size;      // input param size for i2s config params 
   afe_dev_port_config_t port_prop; // output based on input i2s config params
   
}afe_dev_grp_config_info_t;


/**
* It initializes the device driver at the begining of system
* start.
*
* @return value Error code signifying success or failure
*/
ADSPResult afe_device_driver_init(void);

/**
* It de-initialzies the device driver.
*
* @return value Error code signifying success or failure
*/
ADSPResult afe_device_driver_deinit(void);

/**
* It initialzies the device ports.
*
* @return value Error code signifying success or failure
*/
ADSPResult afe_dev_port_init (aud_stat_afe_svc_t* pAfeSvc);

/**
* It De-initialzies the device ports.
*
* @return value Error code signifying success or failure
*/
ADSPResult afe_dev_port_deinit (aud_stat_afe_svc_t* pAfeSvc);

/**
* Allocate the resource for this device port
*
* @param[in] pDevPort: Pointer to the port instance
* @return Error code signifying success or failure
*/
ADSPResult afe_dev_alloc_resources(afe_dev_port_t *pDevPort);

/**
* Start the specified sink port
*
* @param[in] pDevPort Pointer to the port instance
* @return Error code signifying success or failure
*/
ADSPResult afe_dev_start(afe_dev_port_t *pDevPort);

/**
 * De-allocate the resources for this device port
 *
 * @param[in] pDevPort: Pointer to the port instance
 * @return Error code signifying success or failure
 */
ADSPResult afe_dev_dealloc_resources(afe_dev_port_t *pDevPort);

/**
* Stop the specified sink port
*
* @param[in] pDevPort Pointer to the port instance
* @return Error code signifying success or failure
*/
ADSPResult afe_dev_stop(afe_dev_port_t *pDevPort);

/**
* This function is to reconfigure the clocks for sink\source port
*
* @param[in] pDevRxPort, ptr to the rx port instance.
* @param[in] pDevTxPort, ptr to the tx port instance.
* @return the ADSP_EOK error code
*/
ADSPResult afe_reconfig_clocks_for_restart(afe_dev_port_t *pDevRxPort, afe_dev_port_t *pDevTxPort);

/**
  @brief Remote API to AFE to vote LPASS Core HW Module by remote clients

  @param[in] pMe, pointer to AFE static service
  @param[in] pAprPacket, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_remote_lpass_core_hw_vote_reqeust(aud_stat_afe_svc_t* pMe, elite_apr_packet_t* pAprPacket);

/**
  @brief Remote API to AFE to devote LPASS Core HW Module by remote clients

  @param[in] pMe, pointer to AFE static service
  @param[in] pAprPacket, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_remote_lpass_core_hw_devote_reqeust(aud_stat_afe_svc_t* pMe, elite_apr_packet_t* pAprPacket);
/**
 *  Perform specific cmd processing for the given device
 *
 * @param[in] opcode - cmd opcode
 * @param[in] obj_ptr - object pointer used for serving the
 *  		given cmd
 * @param[in] cfg_ptr - configuration info if any
 *
 * @return
 * ADSP_EOK error code.
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_device_generic_cmd (uint32_t opcode, void *obj_ptr, void *cfg_ptr);

/**
 * This function gets the current time
 *
 * @param[in] port_id
 * @param[out] curr_time - current time
 * @return Error code signifying success or failure
 */
ADSPResult afe_drv_get_time_for_port(uint16_t port_id, uint64 *curr_time);

/**
Allocate the memory for low power memories to achevie low power
audio solutions

@param[in] num_bytes Number of bytes need to be allocated
@param[in] align_size - alignment size in bytes

@return
Valid memory pointer in case of success, otherwise NULL ptr is
returned
*/
void* afe_dev_lpa_aligned_malloc(uint32_t num_bytes, uint32_t align_size);

/**
De-Allocate the low power memory

@param[in] ptr Pointer to be freed

@return
None
*/
void afe_dev_lpa_aligned_free(void *ptr);

/**
 * in order to update the accumulated avt drift
 */
void afe_drv_update_avt_drift(afe_dev_port_t *dev_port_ptr);

static inline void afe_drv_update_avt_drift_info_element(volatile uint64_t *drift_info, int32_t acc_drift_us, uint16_t reset_frame_cntr)
{
   afe_client_avt_drift_info_t client_drift_info;
   if (!reset_frame_cntr)
   {
      afe_port_read_avt_drift(drift_info, &client_drift_info);
      client_drift_info.frame_counter++;
   }
   else
   {
      client_drift_info.frame_counter = 0;
   }
   uint64_t temp = ((int64_t)acc_drift_us<<AFE_ACC_AVT_DRIFT_US_SHIFT) | (uint64_t) (client_drift_info.frame_counter);
   *drift_info = temp; //update all 64 bits in one shot. MEMD
}

/**
  This function set the params for the module
  AFE_MODULE_AUDIO_DEV_INTERFACE . It supports the following Params:
  AFE_PARAM_ID_I2S_CONFIG
  AFE_PARAM_ID_PCM_CONFIG
  AFE_PARAM_ID_TDM_CONFIG
  AFE_PARAM_ID_HDMI_CONFIG
  AFE_PARAM_ID_BT_FM_CONFIG
  AFE_PARAM_ID_SLIMBUS_CONFIG
  AFE_PARAM_ID_RT_PROXY_CONFIG


  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration
        data to fetch.
  @param[in] param_size  Actual Param size

  @return
  ADSP_EOK/ error code depends on success/failure.

  @dependencies
  None.
 */

ADSPResult afe_port_set_device_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
                                                          int8_t *param_buffer_ptr, uint32_t param_size);

/**
  This function get the params for the module
  AFE_MODULE_AUDIO_DEV_INTERFACE . It supports the following Params:
  AFE_PARAM_ID_I2S_CONFIG
  AFE_PARAM_ID_PCM_CONFIG
  AFE_PARAM_ID_TDM_CONFIG
  AFE_PARAM_ID_HDMI_CONFIG
  AFE_PARAM_ID_BT_FM_CONFIG
  AFE_PARAM_ID_SLIMBUS_CONFIG
  AFE_PARAM_ID_RT_PROXY_CONFIG


  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration
        data to fetch.
  @param[in] param_size  Actual Param size

  @return
  ADSP_EOK/ error code depends on success/failure.

  @dependencies
  None.
 */

ADSPResult afe_port_get_device_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,\
              int8_t* param_buffer_ptr, int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr);


/**
  This function returns the timestamp value read from the timestamp block

  @param[in] mux_id: mux id for the timestamp block

  @return
  Timestamp

  @dependencies
  None.
 */
uint64_t afe_device_read_timestamp_register(uint32_t mux_id);
/**
  This function set the custom event params for the module
  AFE_MODULE_AUDIO_DEV_INTERFACE . It supports the following Params:
  AFE_PARAM_ID_SIF_EVENTS_CFG

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the event
        data to fetch.
  @param[in] param_size  Actual Param size

  @return
  ADSP_EOK/ error code depends on success/failure.

  @dependencies
  None.
 */
ADSPResult afe_port_set_custom_events_params(afe_dev_port_t *pDevPort,uint32_t param_id, int8_t *param_buffer_ptr, \
   uint32_t param_size, afe_port_event_addr_info_t *pEvent_addr_info);


static inline ADSPResult afe_time_srvr_timer_create(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType,
                                                    qurt_elite_signal_t *pSignal)
{
#ifdef AFE_TIME_SRVR_AV_TIMER
   return avtimer_drv_create(pTimer, timerType, pSignal);
#else // use Qtimer offered by Qurt operating system
   return qurt_elite_timer_create(pTimer, timerType, QURT_ELITE_TIMER_USER, pSignal);
#endif // AFE_TIME_SRVR_AV_TIMER
}

static inline ADSPResult afe_time_srvr_timer_delete(qurt_elite_timer_t *pTimer)
{
#ifdef AFE_TIME_SRVR_AV_TIMER
   return avtimer_drv_delete(pTimer);
#else // use Qtimer offered by Qurt operating system
   return qurt_elite_timer_delete(pTimer);
#endif // AFE_TIME_SRVR_AV_TIMER
}

static inline ADSPResult afe_time_srvr_timer_periodic_start(qurt_elite_timer_t *pTimer, uint64_t duration)
{
#ifdef AFE_TIME_SRVR_AV_TIMER
   return avtimer_drv_start_periodic(pTimer, duration);
#else // use Qtimer offered by Qurt operating system
   return qurt_elite_timer_periodic_start(pTimer, duration);
#endif // AFE_TIME_SRVR_AV_TIMER
}

static inline ADSPResult afe_time_srvr_timer_oneshot_start_absolute(qurt_elite_timer_t *pTimer, uint64_t duration)
{
#ifdef AFE_TIME_SRVR_AV_TIMER
   return avtimer_drv_start_absolute(pTimer, duration);
#else // use Qtimer offered by Qurt operating system
   return qurt_elite_timer_oneshot_start_absolute(pTimer, duration);
#endif // AFE_TIME_SRVR_AV_TIMER
}

static inline uint64_t afe_time_srvr_timer_get_time(avtimer_drv_handle_t avtimer_h)
{
#ifdef AFE_TIME_SRVR_AV_TIMER
   return avtimer_drv_get_time(avtimer_h);
#else // use Qtimer offered by Qurt operating system
   return qurt_elite_timer_get_time();
#endif // AFE_TIME_SRVR_AV_TIMER
}

static inline ADSPResult afe_time_srvr_timer_cancel_oneshot(qurt_elite_timer_t *pTimer)
{
#ifdef AFE_TIME_SRVR_AV_TIMER
   return avtimer_drv_cancel_oneshot(pTimer);
#else // use Qtimer offered by Qurt operating system
   return qurt_elite_timer_oneshot_cancel(pTimer);
#endif // AFE_TIME_SRVR_AV_TIMER
}

ADSPResult afe_set_dev_hw_delay_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size);


/**
  Update the dift structure for all VFR sources

  @param[in]  afe_port_ptr: pointer to afe port structure
        corresponding to a particular VFR source
  @param[in]  dev_timestamp: latest device interrupt timestamp


  @result
  ADSP_EOK on success

  @dependencies
  None.
*/
void afe_device_update_port_vfr_drift_info(void *afe_port_ptr, uint64_t dev_timestamp);


/**
  Updating the accumulate sample drift for a VFR source

  @param[in]  p_drift_info: pointer to port drift info object
        corresponding to a particular VFR source
  @param[in]  num_vfr_sample: Number of samples in the VFR 
        interrupt duration

  @result
  ADSP_EOK on success

  @dependencies
  None.
*/
void afe_device_update_acc_sample_drift(afe_drift_info_t *p_drift_info, uint32_t num_vfr_sample);

/**
  Updating the AV Timer timestamp for each running port

  @param[in]  dev_port_ptr: pointer to port structure 
   
  @result
  None

  @dependencies
  None.
*/
void afe_port_update_timestamp(afe_dev_port_t *dev_port_ptr);

/**
  Create and launches timer for an AFE port

  @param[in]  pDevPort  : pointer to port structure 
  @param[in]  timer_type: timer type (periodic, one shot etc)
  @param[in]  duration  : timer duration
   
  @result
  ADSP Error Code

  @dependencies
  None.
*/
ADSPResult afe_port_create_and_start_timer(afe_dev_port_t *pDevPort,
                                           qurt_elite_timer_duration_t timer_type,
                                           uint64_t duration);


/**
  Deletes the allocated timer for an AFE port

  @param[in]  pDevPort  : pointer to port structure
   
  @result
  ADSP Error Code

  @dependencies
  None.
*/
ADSPResult afe_port_delete_timer(afe_dev_port_t *pDevPort);

/**
  Function to get time per sample

  @param[in]  sample_rate:  port sampling rate 
  @param[in]  p_time:  time per sample
  @param[in]  p_format:  format
   
  @result
  None

  @dependencies
  None.
*/
void afe_device_gettime_per_sample(uint32 sample_rate, uint32 *p_time, int32 *p_format);

/**
* This function moves port state from run to suspend state
*
* @param[in] port_id, id of the port
* @return the ADSP_EOK error code
*/
ADSPResult afe_device_move_to_suspend_state(uint32_t port_id);

/**
* This function is to restore device configuration parameters
* during device recovery
* @param[in] p_dev_port, ptr to the port instance.
* @return the ADSP_EOK error code
*/
ADSPResult afe_restore_dev_config_param(afe_dev_port_t *p_dev_port);
/**
  Function to do unmute

  @param[in]  pDevPort:  pointer of afe port
  @param[in]  p_ctx:  pointer of context structure
   
  @result
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_dev_unmute(afe_dev_port_t *pDevPort,void* p_ctx);

/**
  Function to do mute

  @param[in]  pDevPort:  pointer of afe port
  @param[in]  p_ctx:  pointer of context structure
   
  @result
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_dev_mute(afe_dev_port_t *pDevPort,void* p_ctx);

/**
  @brief Function which allocates LPASS resources to remote 
         client(HLOS).
   Currently supported resources are LPAIF DMA  

  @param[in] pMe, pointer to AFE static service
  @param[in] apr_pkt_ptr, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_svc_cmd_request_lpass_resources(aud_stat_afe_svc_t *pMe, elite_apr_packet_t *apr_pkt_ptr);


/**
  @brief Function which releases LPASS resources which are  
    under control of remote  client(HLOS).
   Currently supported resources are LPAIF DMA.      
   
  @param[in] pMe, pointer to AFE static service
  @param[in] apr_pkt_ptr, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_svc_cmd_release_lpass_resources(aud_stat_afe_svc_t *pMe, elite_apr_packet_t *apr_pkt_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif // !defined(__AFEDEVICEDRIVER_H__)
