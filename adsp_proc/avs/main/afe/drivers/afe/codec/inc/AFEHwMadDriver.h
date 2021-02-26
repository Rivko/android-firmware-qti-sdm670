/*========================================================================
   This file contains functions to set the parameters for different modules.

 Copyright (c) 2012-2013,2017 QUALCOMM Technologies, Incorporated.
 All Rights Reserved.
 QUALCOMM Technologies Proprietary.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/inc/AFEHwMadDriver.h#1 $
 ====================================================================== */
#ifndef _AFE_HW_MAD_DRIVER_H_
#define _AFE_HW_MAD_DRIVER_H_

/*==========================================================================
  Include Files
========================================================================== */
#include "AFEInternal.h"
#include "AFECodecService.h"
#include "npa.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Macro\enum definitions
========================================================================== */
typedef enum
{
	AFE_HW_MAD_AUDIO = 0x0,
	AFE_HW_MAD_BEACON,
	AFE_HW_MAD_ULTRA_SOUND,
	AFE_HW_MAD_TYPE_MAX
} afe_hw_mad_type_e;

/*==========================================================================
  Struct definitions
========================================================================== */
typedef struct hw_mad_mod
{
	bool 				sw_master_enable;
	bool 				hw_enable;
	uint16_t 			sleep_time;
	uint16_t 			int_idx;
	cdc_client_info_t 	client_info;
	cdc_reg_dev_cfg_t 	*reg_cfg_mad_enable_ptr;
	cdc_reg_dev_cfg_t 	*reg_cfg_sleep_time_ptr;
    cdc_reg_dev_cfg_t 	*reg_cfg_tx_switch_off_ptr;
} hw_mad_mod_t;

typedef struct hw_mad_data
{
	bool 									is_initialized;
	hw_mad_mod_t    						mad_data[AFE_HW_MAD_TYPE_MAX];
	afe_slimbus_slave_port_cfg_param_v1_t 	sb_cfg;
	npa_client_handle 						listen_l2cach_npa_handle;
} hw_mad_data_t;

/*==========================================================================
  Function Prototypes
========================================================================== */
ADSPResult afe_port_hw_mad_alloc_resources(void *afe_port_ptr);
ADSPResult afe_port_hw_mad_init(void *afe_port_ptr);

ADSPResult afe_port_hw_mad_dealloc_resources(void *afe_port_ptr);
ADSPResult afe_port_hw_mad_deinit(void *afe_port_ptr);

ADSPResult afe_port_hw_mad_set_param_handler(void *afe_port_ptr, uint32_t param_id, int8_t* params_buffer_ptr, uint16_t param_size);
ADSPResult afe_port_hw_mad_enable_disable_handler(void *afe_port_ptr);
ADSPResult afe_port_hw_mad_dc_reset_handler(void *ptr, uint16_t mad_type);
ADSPResult afe_port_hw_mad_interrupt_handler(void *afe_port_ptr, cdc_client_info_t *client_info);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _AFE_HW_MAD_DRIVER_H_
