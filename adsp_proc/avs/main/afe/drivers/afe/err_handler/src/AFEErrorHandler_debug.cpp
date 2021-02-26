/*========================================================================
   This file contains AFE Error handler debug implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: 
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */

#include "AFEErrorHandler_i.h"
#include "AFEErrorHandler_debug.h"

/*----------------------------------------------------------------------------
 * Global variables
 * -------------------------------------------------------------------------*/

/**< Pointer to access global signal miss info pool array.*/
afe_signal_miss_info_t *signal_miss_pool_global_ptr;

static afe_signal_miss_info_t afe_signal_miss_info_pool_global[MAX_NUM_PORTS_IN_SIG_MISS_POOL];
/**< This holds the configured signal miss information of ports. The size of this array is
 * MAX_NUM_ALLOWED_PORTS_IN_SIGNAL_MISS_POOL, which limits, only those many ports can be configured
 * for signal miss at that instant.
 * */

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/


/**
 * Performs afe error handler debug initialization.
 * 			Initializes the content of global signal miss
 * 			information pool to zero.
 *
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_debug_init()
{
  ADSPResult result = ADSP_EOK;

  memset(afe_signal_miss_info_pool_global, 0, MAX_NUM_PORTS_IN_SIG_MISS_POOL*sizeof(afe_signal_miss_info_t));

  signal_miss_pool_global_ptr = NULL;

  return result;
}

/**
 * This function is to CONFIGURE DEVICE RECOVERY values.
 * Overwrites the global variables max_num_allowed_recoveries
 * and window_duration with the current configured values.
 *
 * @param[in] param_buffer_ptr, pointer to the parameter buffer
 * @param[in] param_size, param size
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_set_dev_recovery_cfg(int8_t *param_buffer_ptr, uint16_t  param_size)
{
  ADSPResult result = ADSP_EOK;

  afe_device_recovery_params_t *dev_rec_param_ptr = (afe_device_recovery_params_t *)param_buffer_ptr;

  /* Validate the size of payload */
  if( param_size < (sizeof(afe_device_recovery_params_t)) )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: %d", param_size);
    return ADSP_EBADPARAM;
  }

  /** Initializes the global signal miss pool pointer */
  signal_miss_pool_global_ptr = (afe_signal_miss_info_t *) (&afe_signal_miss_info_pool_global);

  dev_rec_params_global_ptr->max_num_allowed_recoveries = (uint32_t)dev_rec_param_ptr->max_num_allowed_recoveries;
  dev_rec_params_global_ptr->window_duration            = (uint32_t)dev_rec_param_ptr->window_duration;

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE DEVICE RECOVERY CONFIG :: max_num_allowed_recoveries: %ld, window_duration:%ld us ", \
        dev_rec_params_global_ptr->max_num_allowed_recoveries, dev_rec_params_global_ptr->window_duration);

  return result;
}

/**
 * This function is to get the signal miss config handle.
 *
 * @param[in] port_id, port id.
 *            If port_id = 0, then returns the free slot handle
 *            If port_id is valid port id, then returns the handle
 *            corresponding to the port.
 *
 * @return the handle of signal miss config structure
 */
afe_signal_miss_info_t * afe_get_port_signal_miss_cfg(uint32_t port_id)
{
  if(NULL == signal_miss_pool_global_ptr)
  {
    /* Do not print any message here, since this might be called every ms,
     * (and it is not an error) irrespective of whether port is configured
     * to signal miss or not*/

    return NULL;
  }

  afe_signal_miss_info_t *signal_miss_info_ptr = (afe_signal_miss_info_t *)signal_miss_pool_global_ptr;
  bool_t  is_port_id_matched = FALSE;
  uint8_t             index = 0;

  /*Finds the port config data slot in the pool */
  for(index = 0; index < MAX_NUM_PORTS_IN_SIG_MISS_POOL; index++)
  {
    if(port_id == signal_miss_info_ptr->port_id )
    {
      is_port_id_matched =  TRUE;

      break;
    }

    signal_miss_info_ptr++;
  }

  if(FALSE == is_port_id_matched)
  {
    /* Do not print any message here, since this might be called every ms,
     * (and it is not an error) irrespective of whether port is configured
     * to signal miss or not*/

    return NULL;
  }

  return signal_miss_info_ptr;

}

/**
 * This function CONFIGURES SIGNAL MISS DURATIONS of AFE port.
 *
 * Finds the free slot in a pool of signal miss information
 * from the global array (by checking port-id) and stores the
 * configured information in that available slot.
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 * @param[in] p_cfg pointer to AFE port signal miss config
 *            strcutre
 * @param[in] cfg_payload_size config struct payload size
 * @param[in] param_id, parameter id
 *
 * @return  ADSP_EOK on success, an error code on error
 */
ADSPResult afe_err_handler_set_port_signal_miss_cfg(afe_dev_port_t* p_dev_port, int8_t *p_cfg, uint16_t cfg_payload_size, uint32_t param_id)
{
  ADSPResult result = ADSP_EOK;

  if(cfg_payload_size < sizeof(afe_param_id_signal_miss_config_t))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE port signal miss Config Fails, Bad Param");
    return ADSP_EBADPARAM;
  }

  switch(param_id)
  {
    case AFE_PARAM_ID_PORT_SIGNAL_MISS_CONFIG:
    {
      afe_param_id_signal_miss_config_t *config_ptr    = (afe_param_id_signal_miss_config_t *)p_cfg;

      if(NULL == signal_miss_pool_global_ptr)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE port signal miss Config Fails:signal_miss_pool_global_ptr is NULL, port id: 0x%x", p_dev_port->intf);

        return ADSP_EFAILED;
      }

      /*Gets the free slot from the signal miss config data pool*/
      afe_signal_miss_info_t *signal_miss_info_ptr = afe_get_port_signal_miss_cfg(0);

      if(NULL == signal_miss_info_ptr)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Signal miss info pool is full, cannot configure signal miss for port: 0x%lx", p_dev_port->intf);

        return ADSP_EUNSUPPORTED;
      }

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port 0x%x: Signal miss config data ", p_dev_port->intf);


      /**<Store port id to identify the corresponding signal miss
       * config. data from the pool.*/
      signal_miss_info_ptr->port_id = p_dev_port->intf;

      for(uint8_t index = 0; index < MAX_NUM_SIG_MISS_PER_PORT; index++)
      {
        signal_miss_info_ptr->int_counter[index] = config_ptr->int_counter[index];

        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "int_counter[%ld]= %d", index, signal_miss_info_ptr->int_counter[index] );
      }
    }
    break;

    default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported Param id ::0x%lx", param_id);
      return ADSP_EUNSUPPORTED;
  }

  return result;
}

/**
 * This function clears the configured signal miss information for that
 * particular port in signal miss information pool. (Called during afe port stop)
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_free_port_signal_miss_cfg(afe_dev_port_t * pDevPort)
{
  ADSPResult result = ADSP_EOK;

  /* In signal miss pool, finds the corresponding
   * structure used for this port and clear it's contents.*/
  afe_signal_miss_info_t *signal_miss_info_ptr = afe_get_port_signal_miss_cfg(pDevPort->intf);

  if(NULL != signal_miss_info_ptr)
  {
    memset(signal_miss_info_ptr, 0, sizeof(afe_signal_miss_info_t));
  }

  return result;
}

/**
 * This function is to forces signal miss, when the running
 * interrupt counter value matches with the configured interrupt
 * count.
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_force_signal_miss(afe_dev_port_t *pDevPort)
{
  ADSPResult                            result = ADSP_EOK;

  afe_signal_miss_info_t *signal_miss_info_ptr = afe_get_port_signal_miss_cfg(pDevPort->intf);

  if(NULL != signal_miss_info_ptr)
  {
    afe_dynamic_thread_t     *dynamic_thread_ptr = pDevPort->dynamic_thread_ptr;

    /* Check if this port is member of group device */
    if(NULL != pDevPort->p_group_device_state)
    {
      dynamic_thread_ptr = pDevPort->p_group_device_state->dynamic_thread_ptr;
    }

    /* NOTE: After successful device recovery, then only recovery index will be incremented.*/
    if(signal_miss_info_ptr->recovery_index < MAX_NUM_SIG_MISS_PER_PORT)
    {
      uint32_t int_counter_value = signal_miss_info_ptr->int_counter[signal_miss_info_ptr->recovery_index];

      /* Introduces signal miss, if current interrupt counter value
       *  matches the configured counter value pointed by the index */
      if(int_counter_value == pDevPort->running_int_counter)
      {
        dynamic_thread_ptr->is_signal_miss = TRUE;
      }
    }
  }

  return result;
}

/**
 * This function increments recovery index
 * (index of running interrupt counter array)
 * after successful afe device recovery.
 *
 * For crash scenario, this count won't be incremented
 * and hence, signal miss will not be introduced thereafter.
 *
 *@param[in] port_id, ID of the port
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_err_handler_incr_recovery_index_counter(uint16_t port_id)
{
  ADSPResult        result = ADSP_EOK;

  afe_signal_miss_info_t *signal_miss_info_ptr = afe_get_port_signal_miss_cfg(port_id);

  if(NULL != signal_miss_info_ptr)
  {
      signal_miss_info_ptr->recovery_index++;
  }

  return result;
}
