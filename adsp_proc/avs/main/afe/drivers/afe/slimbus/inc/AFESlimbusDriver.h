#ifndef __AFE_SLIMBUS_DRIVER_H__
#define __AFE_SLIMBUS_DRIVER_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/inc/AFESlimbusDriver.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFESlimbusDriver.h

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/inc/AFESlimbusDriver.h#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft


============================================================================*/
/**
@file AFESlimbusDriver.h

@brief has high level APIs for SLIMBUS driver
*/

#include "AFEInternal.h"
#include "mmdefs.h"
#include "VfrDriverInternal.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*=====================================================================
 Macros
 ======================================================================*/


/*=====================================================================
 Typedef's
 ======================================================================*/
typedef enum afe_sb_client_type
{
  AFE_SB_GEN_CLIENT_AANC = 0,
  AFE_SB_GEN_CLIENT_CDC,
}afe_sb_client_type_e;

/* Properties used across slimbus based register accesses */
typedef struct afe_reg_prop
{
  uint8_t slave_dev_la;
  /* logical address of the slave device */
  bool_t uses_paging;
  /* flag indicating whether to use paging scheme for register access or not */
  uint8_t proc_id;
  /* processor id used for domain specific page access */
  uint8_t current_page;
  /* cached page number indicating the current page in this proc_id domain */
  uint16_t slimbus_la_offset;
  /* each slave device in slimbus has a particular logical address offset */
}afe_reg_prop_t;

/*=====================================================================
 Global Variables
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
 * Perform SLIMBUS port initialization 
 *
 * @param[in] afe_svc_ptr pointer for AFE static service 
 *  
 * @return  ADSP_EOK on success, an error code on error
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_slimbus_port_init(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Perform SLIMBUS port de-initialization 
 *
 * @param[in] afe_svc_ptr pointer for AFE static service 
 *  
 * @return  ADSP_EOK on success, an error code on error
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_slimbus_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Perform one time initialization of the AFE Slimbus driver
 *
 * @return  ADSP_EOK on success, an error code on faliure
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_slimbus_driver_init(void);

/**
 * Perform de-initialization of the AFE Slimbus driver
 *	
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_slimbus_driver_deinit(void);

/**
 * Perform configuration of SLIMBUS streams for the given 
 * configuration 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @param[in] p_cfg pointer to AFE port slimbus config strcutre 
 *  
 * @param[in] cfg_payload_size config struct payload size 
 *  
 * @param[in] cfg_ver config version 
 *
 * @return  ADSP_EOK on success, an error code on error
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_set_slimbus_cfg (afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, bool is_it_shared_chan, uint32_t sample_rate);
ADSPResult afe_get_slimbus_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);

/**
 * Open the requested data transfer link to the requested 
 * device. 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @return  ADSP_EOK on success, an error code on error
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_slimbus_open (afe_dev_port_t* afe_port_ptr);

/**
 * Close the requested data transfer link to the requested 
 * device. 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @return
 * ADSP_EOK on success, an error code on error 
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult	afe_slimbus_close(afe_dev_port_t* afe_port_ptr);

/**
 * Data is fecthed from DMA buffer to local buffer in case of 
 * Mic path (Tx path)
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @param[in] pBuffer - buffer to store the fetched data 
 *  
 * @param[in] numSamples - number of samples to be fetched
 *  
 * @return  ADSP_EOK on success, an error code on error 
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_slimbus_read(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples);

/**
  @brief Read the 32bit Mic path data from descriptor buffers, extract 16bit samples and store to
         client's buffer

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[in] numSamples  Number of samples to be read

  @param[out] pBuffer client buffer for storing the read samples

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult  afe_slimbus_read_32bit(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples);


/**
 * Device data will be written into DMA buffers from local
 * buffer in case of Speaker path (Rx path) 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @param[in] pBuffer - data buffer to be pushed into DMA buffer
 *  
 * @param[in] numSamples - number of samples to be copied
 *  
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error 
 * code 
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_slimbus_write(afe_dev_port_t *afe_port_ptr, int16_t  *pBuffer, uint32_t numSamples);

/**
 * @brief Reads the de-interleaved data from working buffer and
 *        writes the data to descriptor buffers (of master ports
 *        that supports multi end point) in interleaved fashion.
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @param[in] pBuffer - data buffer to be pushed into DMA buffer
 *
 * @param[in] numSamples - number of samples to be copied
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_slimbus_multi_ep_write(afe_dev_port_t *afe_port_ptr, int16_t  *pBuffer, uint32_t numSamples);


/**
 * Data is fetched from DMA buffer to local buffer in case of
 * Mic path (Tx path)
 *
 * @brief Reads the interleaved data from descriptor buffers
 *       (master ports that supports multi end point) and
 *       writes the data to working buffer in de-interleaved fashion.
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] pBuffer - buffer to store the fetched data
 *
 * @param[in] numSamples - number of samples to be fetched
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_slimbus_multi_ep_read(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples);

/**
 * Calculates the PCM sample's drift (in us) between 
 * consequitive VFR interrupts. It provides the VFR shift 
 * information also. 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @dependencies
 * None.
 *
 */
void afe_sb_read_vfr_drift(afe_dev_port_t* afe_port_ptr, vfr_src_state_t *vfr_state_ptr);

/**
 * Get the current time using Slimbus port data transfer 
 * rate
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
ADSPResult 	afe_sb_get_time(afe_dev_port_t* afe_port_ptr, uint64_t *time_in_us);


/**
  @brief Get the current SLIMbus counter value

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[out] p_cur_pg_samp_cnt - Current device sample count

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_sb_get_dev_prog_counter(afe_dev_port_t* afe_port_ptr, uint64_t *p_cur_pg_samp_cnt);

/**
  @brief Get the current timestamp for the last SLIMbus BAM interrupt and FIFO count
  when the last interrupt occurred.

  @param[in,out] afe_port_ptr pointer AFE dev port structure

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_sb_get_timestamp_and_fifo_count(afe_dev_port_t *afe_port_ptr);

/**
 * calculate avt drift for slimbus ports
 * below function executes in fast-int
 */
void afe_calc_avt_drift_for_sb_port(afe_dev_port_t *dev_port_ptr);

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
void afe_sb_get_time_for_avt_afe(afe_dev_port_t *afe_port_ptr, uint64_t *time_in_us);

/**
 * Get the 8-bit logical address of a given 48-bit enumerated device address
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] device_enum_addr 48-bit enumerated device address
 *
 * @param[out] device_la_ptr - pointer to the 8-bit device logical address
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_sb_get_logical_addr(void* p_drv, uint64_t device_enum_addr, uint8_t *device_la_ptr);

/**
 * Generic API to write to Control and Status Registers(CSR).
 * Parameters passed are device address, register address and register value
 *
 * @param[in] p_drv pointer to global afe_slimbus_driver_t state
 *
 * @param[in] afe_sb_reg_prop_t pointer to register properties
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
ADSPResult afe_sb_update_register(void* p_dev_state, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t  *write_value_ptr, \
                                 uint32_t reg_field_bit_mask, uint32_t reg_bit_width, \
                                 bool_t is_blocking);

ADSPResult afe_sb_write_register(void* p_dev_state, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t* write_value_ptr, \
                                 uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
                                 bool_t is_blocking);

ADSPResult afe_sb_read_register(void* p_dev_state, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t* read_value_ptr, \
                                 uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
                                 bool_t is_blocking);

/**
 * Generic API to read multiple contiguous registers
 *
 * @param[in] p_drv pointer to global afe_slimbus_driver_t state
 *
 * @param[in] afe_sb_reg_prop_t pointer to register properties
 *
 * @param[in] reg_addr is the logical address of the Slimbus Codec CSR
 *
 * @param[in] write_value_ptr is a pointer to value to be written to the Codec CSR
 *
 * @param[in] reg_field_bit_mask is the valid  bit-field of the parameter being passed
 *
 * @param[in] reg_bit_width is the width of the register to be 
 *  		   written to, in bits. Possible values are 8, 16
 *  		   and 32
 *@param[in] no_of_regs_to_read is the number of contiguous  
 *  	regiters to read
 *  
 * @param[in] is_blocking indicates if write should be blocking.
 *  	 Possible values are TRUE and FALSE
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */

ADSPResult afe_sb_read_multi_register(void *p_drv, afe_reg_prop_t*  p_reg_prop, \
                                 uint32_t	reg_addr, uint8_t* read_value_ptr, \
                                 uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
                                 uint32_t no_of_regs_to_read, bool_t is_blocking);

/**
 * Register page init ensure that the software page variable and the on-target
 * page variable are in same state.
 *
 * @param[in] p_drv pointer to global afe_slimbus_driver_t state
 *
 * @param[in] p_reg_prop Slimbus register property
 *
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_sb_reg_page_init(void* p_drv, afe_reg_prop_t* p_reg_prop);

/**
 * Allocate and init handle of SLIMbus core driver. This 
 * allocation must be done before opening the SLIMbus core 
 * driver 
 *
 * @param[in] afe_slimbus_id: SLIMbus h/w ID
 *
 * @return
 * Valid handle if allocation is successful, NULL pointer in 
 * case of failure 
 *
 * @dependencies
 * None.
 *
 */
void *afe_slimbus_init_dev_handle(uint32_t afe_slimbus_id);

/**
 * De-init and de-allocate handle of SLIMbus core driver. This 
 * de-allocation must be done after closing the SLIMbus core 
 * driver 
 *
 * @param[in] dev_handle: Valid driver handle
 *
 * @return
 * None
 *
 * @dependencies
 * None.
 *
 */
void afe_slimbus_deinit_dev_handle(void *dev_handle);

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
void *afe_sb_get_core_drv_hdl(void* p_drv);

ADSPResult afe_slimbus_core_driver_open(void *p_drv);
ADSPResult afe_slimbus_core_driver_close(void *p_drv);
ADSPResult afe_slimbus_bw_voting(void *p_drv, uint32_t bw);

ADSPResult afe_restore_slimbus_cfg(afe_dev_port_t *afe_port_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*#ifndef __AFE_SLIMBUS_DRIVER_H__*/

