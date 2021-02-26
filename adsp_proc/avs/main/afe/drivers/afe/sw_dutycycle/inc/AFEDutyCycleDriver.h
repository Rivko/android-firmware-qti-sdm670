/*========================================================================
 This file contains functions to set the parameters for different modules.

 Copyright (c) 2014-2016,2017 QUALCOMM Technologies, Incorporated.
 All Rights Reserved.
 QUALCOMM Technologies Proprietary. 
 ====================================================================== */
#ifndef _AFE_DUTY_CYCLE_DRIVER_H_
#define _AFE_DUTY_CYCLE_DRIVER_H_

/*==========================================================================
 Include Files
 ========================================================================== */
#include "AFEInternal.h"
#include "avtimer_drv_api.h"
#include "AFEDeviceDriver.h"
#include "npa.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
  /*==========================================================================
 Macro\enum definitions
 ========================================================================== */

  /*==========================================================================
 Struct definitions
 ========================================================================== */

  typedef struct sw_duty_cycle_data
  {
    bool_t               is_initialized; /**< flag to check for init done or not >*/
    bool_t               sw_dc_master_enable; /**< Flag whetehr HLOS client enabled DC deature or not >*/
    bool_t               is_timer_configured; /**< internal variable to check tiemr configured >*/
    uint32_t             sleep_time_us; /**< sleep time in usec >*/
    qurt_elite_signal_t  sw_dc_timer_sig; /**< signal used for timer interrupt >*/
#ifdef AFE_TIME_SRVR_AV_TIMER
    uint32_t             sw_dc_avt_drv_handle; /**< avtimer handle  >*/
#endif
    qurt_elite_timer_t   *sw_dc_timer_ptr; /**< pointer to  tiemr object >*/
    afe_dynamic_thread_t *dynamic_thread_ptr; /**< pointer to dynamic thread > */
    npa_client_handle 	  l2cache_npa_handle; /** < handle for l2 cache retention > */
    void                 *device_cfg_ptr; /** < Backup of the device config, needs to be set every duty cycle >*/
    uint32_t             device_cfg_size; /** < Size of the device config >*/
    uint32_t             device_param_id; /** < Param ID of the device config >*/
  } sw_duty_cycle_data_t;

  /*==========================================================================
 Function Prototypes
 ========================================================================== */
  /**
   * This function allocates sw dc related parameters
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_alloc_resources(afe_dev_port_t *dev_port_ptr);

  /**
   * This function inits sw dc related parameters
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_init(afe_dev_port_t *dev_port_ptr);

  /**
   * This function deallocates sw dc related parameters
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_dealloc_resources(afe_dev_port_t *dev_port_ptr);

  /**
   * This function deinits sw dc related object
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_deinit(afe_dev_port_t *dev_port_ptr);

  /**
 This function set the params for the module
 AFE_MODULE_SW_DUTYCYCLE. It supports the following Params:
 AFE_PARAM_ID_SW_DUTY_CYCLE_CFG 
 AFE_PARAM_ID_ENABLE

 @param[in] afe_port_ptr: Pointer to the instance of the port.
 @param[in] param_id: Parameter id  .
 @param[in] param_buffer_ptr: Pointer from where the calibration
 data to fetch.
 @param[in] param_size: Actual Param size


 @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_set_param_handler(afe_dev_port_t *dev_port_ptr,
                                                      uint32_t param_id,
                                                      int8_t* params_buffer_ptr,
                                                      uint16_t param_size);

  /**
   * This function enables and disables sw duty cycle
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_enable_disable_handler(afe_dev_port_t *dev_port_ptr);

  /**
   * Handler for reseetting duty cycle
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  ADSPResult afe_port_sw_duty_cycle_reset_handler(afe_dev_port_t *dev_port_ptr);

  /**
   * Handler for disabling duty cycle
   *
   * @param[in] afe_port_ptr: pointer to the afe port
   * @return ADSPResult
   */
  void afe_port_disable_sw_duty_cycle(afe_dev_port_t *afe_port_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // _AFE_DUTY_CYCLE_DRIVER_H_

