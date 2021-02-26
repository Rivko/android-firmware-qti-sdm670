/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFEHwMadUtils.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFEHwMadDriver_i.h"

/*==========================================================================
  Functions
========================================================================== */
ADSPResult hw_mad_interrupt_register_handler(afe_dev_port_t *pDevPort, uint16_t mad_ctr, uint16_t mad_type)
{
	ADSPResult result = ADSP_EOK;
	uint8_t dest_value = 0x0;
	bool enable_request = FALSE, hw_enable = FALSE;
	hw_mad_data_t 	*hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;
	cdc_reg_dev_cfg_t *reg_cfg_ptr = hw_mad_ptr->mad_data[mad_type].reg_cfg_mad_enable_ptr;
	
	if(ADSP_EOK != afe_svc_check_codec_interrupts_supported())
	{
	  MSG(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "skipping interrupt registrations, codec interrupts not supported");
	  return ADSP_EFAILED;
	}
	// Enable\disable HW MAD audio based on client connections and master ctrl
	enable_request = FALSE;
	if ((0 != mad_ctr) && (TRUE == hw_mad_ptr->mad_data[mad_type].sw_master_enable))
	{
	   enable_request = TRUE;
	}

	hw_enable = hw_mad_ptr->mad_data[mad_type].hw_enable;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "CDC Mad Ctrl: req: %d, current: %d", enable_request, hw_enable);

	if ((TRUE == enable_request) && (TRUE != hw_enable))
	{
		// register for codec 
		// Register for MAD HW interrupt
		memset(&(hw_mad_ptr->mad_data[mad_type].client_info), 0, sizeof(hw_mad_ptr->mad_data[mad_type].client_info));
		hw_mad_ptr->mad_data[mad_type].client_info.enable_int_ack_ctrl = TRUE;
		hw_mad_ptr->mad_data[mad_type].client_info.client_id = pDevPort->intf;
		hw_mad_ptr->mad_data[mad_type].client_info.int_index = hw_mad_ptr->mad_data[mad_type].int_idx;
		hw_mad_ptr->mad_data[mad_type].client_info.client_q_ptr = afe_svc_global_ptr->static_svc_cmdq_ptr;
                afe_cdc_intr_reg_handler(&(hw_mad_ptr->mad_data[mad_type].client_info), TRUE);

		// enable hw MAD module
		dest_value =  0x1;
		result = afe_cdc_update_register(reg_cfg_ptr, &dest_value, 0);
		hw_mad_ptr->mad_data[mad_type].hw_enable = TRUE;
	}

	if ((FALSE == enable_request) && (FALSE != hw_enable))
	{
		// disable hw MAD module
		dest_value =  0x0;
		result = afe_cdc_update_register(reg_cfg_ptr, &dest_value, 0);
		hw_mad_ptr->mad_data[mad_type].hw_enable = FALSE;

		// De-register for interrupt
                afe_cdc_intr_reg_handler(&(hw_mad_ptr->mad_data[mad_type].client_info), FALSE);

		// Disable the data path if it is not already disabled
		// this is useful for master disable control
		afe_port_hw_mad_dc_reset_handler(pDevPort, mad_type);
	}

	return result;
}


ADSPResult afe_hw_mad_dc_codec_sequence(hw_mad_data_t *hw_mad_ptr, uint16_t mad_type)
{
	ADSPResult result = ADSP_EOK;
	cdc_reg_dev_cfg_t *reg_cfg_ptr = NULL;
	uint8_t dest_value = 0x0;

	// write sleep time to codec
	reg_cfg_ptr = hw_mad_ptr->mad_data[mad_type].reg_cfg_sleep_time_ptr;
	dest_value = hw_mad_ptr->mad_data[mad_type].sleep_time;
	if (reg_cfg_ptr->reg_addr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Writting sleep time to codec, st - 0x%x", dest_value);
		result = afe_cdc_update_register(reg_cfg_ptr, &dest_value, 0);
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Missing device cfg for sleep time register");
	}

	// write Tx switch off reg to codec
	reg_cfg_ptr = hw_mad_ptr->mad_data[mad_type].reg_cfg_tx_switch_off_ptr;
	dest_value = 0x0;
	if (reg_cfg_ptr->reg_addr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Writting tx switch off value to codec, st - 0x%x", dest_value);
		result = afe_cdc_update_register(reg_cfg_ptr, &dest_value, 0);
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Missing device cfg for tx switch off register");
	}

	return result;
}
