/*========================================================================
  This file contains AFE Error handler debug related API's

  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header:
 ====================================================================== */

#ifndef _AFE_ERR_HNDLR_DEBUG_H_
#define _AFE_ERR_HNDLR_DEBUG_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEInternal.h"

/*----------------------------------------------------------------------------
 * Macro definition
 * -------------------------------------------------------------------------*/

#define AFE_PARAM_ID_DEVICE_RECOVERY_CONFIG             0xFFFF0005

#define AFE_PARAM_ID_PORT_SIGNAL_MISS_CONFIG            0xFFFF0006

#define MAX_NUM_PORTS_IN_SIG_MISS_POOL                  0x3
/**< Indicates maximum number of ports allowed to configure for signal miss
 * (in signal  miss information pool) at that instant. This can be varied,
 * currently set to 3 ports.*/

#define MAX_NUM_SIG_MISS_PER_PORT                       0x8
/**< Indicates maximum number of signal misses to be configured per port.
 * This can be varied, currently set to 8 signal misses.
 * .*/

/* Payload of signal miss config param id.*/
typedef struct afe_param_id_signal_miss_config
{
  uint32_t int_counter[MAX_NUM_SIG_MISS_PER_PORT];
  /**< Array of device interrupt counter, able to configure maximum of
   * MAX_NUM_SIGNAL_MISSES_TO_CONFIG_PER_PORT signal misses per port.
   *
   * If device running interrupt counter reaches the value pointed by
   * array index, then signal miss will be introduced. After successful
   * device recovery, this index will be incremented.
   * .*/

} afe_param_id_signal_miss_config_t;

typedef struct afe_signal_miss_info
{
  uint32_t port_id;
  /**<Id of the port, which is configured to
   * introduce signal misses.
   *
   * In pool of signal miss information (global array),
   * using this port id, will be able to identify
   * the corresponding signal  miss configuration*/

  uint32_t int_counter[MAX_NUM_SIG_MISS_PER_PORT];
  /**< Points to an array of interrupt counter values.
   * Whenever port reaches particular interrupt count,
   * then signal miss would be introduced.*/
  uint16_t recovery_index;
  /**< Index to above interrupt counter array.
   * After recovery due to signal miss,
   * this index value would be incremented.*/
}afe_signal_miss_info_t;

extern afe_signal_miss_info_t *signal_miss_pool_global_ptr;
/* Global Pointer to access signal miss info pool pointer*/

/*==========================================================================
  Function declarations
  ========================================================================== */

/**
 * This function performs the initialization part of
 * error handler debug.
 *
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_debug_init();

/**
 * This function is to CONFIGURE DEVICE RECOVERY values.
 * Overwrites the global variables 'max_num_allowed_recoveries'
 * and 'window_duration' with the current configured values.
 *
 * @param[in] param_buffer_ptr, pointer to the parameter buffer
 * @param[in] param_size, param size
 * @return the ADSP_EOK, error code
 */
ADSPResult afe_err_handler_set_dev_recovery_cfg(int8_t *param_buffer_ptr, uint16_t  param_size);

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
ADSPResult afe_err_handler_set_port_signal_miss_cfg(afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, uint32_t param_id);

/**
 * This function is forces signal miss, when the running
 * interrupt counter value matches with the configured interrupt
 * count.
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_force_signal_miss(afe_dev_port_t *pDevPort);

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
ADSPResult afe_err_handler_incr_recovery_index_counter(uint16_t port_id);

/**
 * This function clears the configured signal miss information for that
 * particular port in signal miss information pool. (Called during afe port stop)
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @return the ADSP_EOK error code
 */
ADSPResult afe_err_handler_free_port_signal_miss_cfg(afe_dev_port_t * pDevPort);

#endif /* _AFE_ERR_HNDLR_DEBUG_H_ */
