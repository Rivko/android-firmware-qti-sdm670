/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/stub_src/AFESlimbusDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusDriver.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/stub_src/AFESlimbusDriver.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFESlimbusDriver.h"

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform SLIMBUS port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_slimbus_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{

	return ADSP_EUNSUPPORTED;
}

/**
  @brief Perform SLIMBUS port deinitialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_slimbus_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{

	return ADSP_EUNSUPPORTED;
}

/**
  @brief Perform one time initialization of the AFE Slimbus driver

  @return  valid 'p_sb_driver' on success, NULL on error

*/
ADSPResult afe_slimbus_driver_init()
{
    return ADSP_EUNSUPPORTED;
}

/**
  @brief Perform de-initialization of the AFE Slimbus driver

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_driver_deinit()
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Perform configuration of SLIMBUS streams for the given
         configuration

  @param[in] afe_port_ptr pointer to AFE device port structure
  @param[in] p_afesb_cfg pointer to AFE port slimbus config
        strcutre
  @param[in] cfg_payload_size config struct payload size
  @param[in] cfg_ver -> config version

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_set_slimbus_cfg (afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, bool is_it_shared_chan, uint32_t sample_rate)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_get_slimbus_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Open the configured SLIMBUS streams for data transfers

  @param[in] afe_port_ptr pointer to AFE device port structure

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_slimbus_open (afe_dev_port_t* afe_port_ptr)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Close the SLIMBUS streams associated with the current
         AFE port.

  @param[in] afe_port_ptr pointer AFE dev port structure

  @return  ADSP_EOK on success, an error code on error 

*/
ADSPResult afe_slimbus_close(afe_dev_port_t* afe_port_ptr)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Read the Mic path data from descriptor buffers to
         client's buffer

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[in] numSamples  Number of samples to be read

  @param[out] pBuffer client buffer for storing the read samples

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult  afe_slimbus_read(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Write the data to speaker path descriptors and do BAM
         transfer

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[in] pBuffer port buffer to write the samples

  @param[in] numSamples  Number of samples to be written.

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult  afe_slimbus_write(afe_dev_port_t *afe_port_ptr, int16_t  *pBuffer, uint32_t numSamples)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Calculates the PCM sample's drift (in us) between
  	consequitive VFR interrupts. It provides the VFR shift
  	information also.

  @param[in] afe_port_ptr pointer AFE dev port structure

*/
void afe_sb_read_vfr_drift(afe_dev_port_t* afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
	return;
}

/**
  @brief Calculates the PCM sample's drift (in us) between
  	consequitive VFR interrupts. It provides the VFR shift
  	information also.

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[out] time_in_us - time stamp in micro sec

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult 	afe_sb_get_time(afe_dev_port_t* afe_port_ptr, uint64_t *time_in_us)
{
	return ADSP_EUNSUPPORTED;
}

/**
 * calculate avt drift for slimbus ports
 * below function executes in fast-int
 */
void afe_calc_avt_drift_for_sb_port(afe_dev_port_t *dev_port_ptr)
{
	return;
}


/**
 * Get the current time using Slimbus port data transfer
 * rate used only by avt afe code.
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[out] time_in_us - time stamp in micro sec
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
void afe_sb_get_time_for_avt_afe(afe_dev_port_t *afe_port_ptr, uint64_t *time_in_us)
{
	return;
}

 /**
  @brief Get the logical address for a SB device, for a given device enumeration address

  @param[in] afe_port_ptr - Pointer to the AFE device port structure

  @param[in] device_enum_addr - 48-bit enumerated device address

  @param[out] device_la_ptr - pointer to the 8-bit device logical address

  @return  ADSP_EOK on success, an error code on error

  @dependencies
  None.

 */
ADSPResult afe_sb_get_logical_addr(void* p_drv, uint64_t device_enum_addr, uint8_t *device_la_ptr)
{
	return ADSP_EUNSUPPORTED;
}

/**
 * Generic API to write to Control and Status Registers(CSR).
 * Parameters passed are device address, register address and register value
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] slave_dev_la logical address of the slave device
 *
 * @param[in] reg_addr is the logical address of the Slimbus Codec CSR
 *
 * @param[in] write_value_ptr is a pointer to value to be written to the Codec CSR
 *
 * @param[in] reg_field_bit_mask is the valid  bit-field of the parameter being passed
 *
 * @param[out] reg_bit_width is the width of the register to be written to, in bits. Possible values are
 *             8, 16 and 32
 *
 * @param[out] is_blocking indicates if write should be blocking. Possible values are TRUE and FALSE
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */

ADSPResult afe_sb_write_register(void* p_dev_state, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t* write_value_ptr, \
                                 uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
                                 bool_t is_blocking)
{
	return ADSP_EUNSUPPORTED;
}

/**
 * To get core slimbus driver handle
 *
 * @param[in] dev_handle: Valid driver handle
 *
 * @return
 * handle to core slimbus driver
 *
 * @dependencies
 * None.
 *
 */
void *afe_sb_get_core_drv_hdl(void* p_drv)
{
  return NULL;
}

ADSPResult afe_sb_update_register(void* p_dev_state, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t  *write_value_ptr, \
                                 uint32_t reg_field_bit_mask, uint32_t reg_bit_width, \
                                 bool_t is_blocking)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_sb_read_register(void* p_dev_state, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t* read_value_ptr, \
                                 uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
                                 bool_t is_blocking)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_sb_reg_page_flush(void* p_drv, afe_reg_prop_t* p_reg_prop)
{
  return ADSP_EUNSUPPORTED;
}

ADSPResult afe_slimbus_core_driver_open(void *p_drv)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_slimbus_core_driver_close(void *p_drv)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_slimbus_bw_voting(void *p_drv, uint32_t bw)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_sb_get_dev_prog_counter(afe_dev_port_t* afe_port_ptr, uint64_t *p_cur_pg_samp_cnt)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_sb_get_timestamp_and_fifo_count(afe_dev_port_t *afe_port_ptr)
{
   return ADSP_EUNSUPPORTED;
}

void *afe_slimbus_init_dev_handle(uint32_t afe_slimbus_id)
{
   return NULL;
}

void afe_slimbus_deinit_dev_handle(void *dev_handle)
{
    return;
}

ADSPResult afe_restore_slimbus_cfg(afe_dev_port_t *afe_port_ptr)
{
  return ADSP_EOK;
}
