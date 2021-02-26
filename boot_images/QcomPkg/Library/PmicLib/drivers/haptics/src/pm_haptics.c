
/*! \file haptics_api.c
*  \n
*  \details This file contains the top-level API wrappers for the Vib
*           peripheral.
*
*  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/17   czq      Supported for WF source selection.
03/20/17   czq      1. Added get haptics ststus1 API
                    2. Added get play rate configuration and LRA auto resonance API.
                    3. Adjusting rate configuration before haptics disable
08/07/15   al       Fixing mask and shift as per doc					
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_haptics_driver.h"


/*===========================================================================

                GLOBAL TYPE DEFINITIONS

===========================================================================*/
#define PM_HAPTICS_VMAX_VOLT_HIGH 			 3596
#define PM_HAPTICS_VMAX_VOLT_LOW 			 1044
#define PM_HAPTICS_VMAX_VOLT_FACTOR 		 116
#define PM_HAPTICS_RATE_CONFIG_TIME_MAX      20476
#define PM_HAPTICS_ACTUATOR_TYPE_MASK        0x1
#define PM_HAPTICS_LRA_AUTO_RES_MODE_MASK    0x70
#define PM_HAPTICS_PLAY_MASK                 0x80
#define PM_HAPTICS_AUTO_RES_CFG_MASK         0x7F
#define PM_HAPTICS_WF_S_NUM					  8

pm_err_flag_type pm_haptics_enable( uint32 pmic_chip,  pm_on_off_type enable )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_haptics_en_ctl = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data = 0;
    pm_register_mask_type mask = 0x80;
    uint32 lra_auto_res_rate = 0;
    uint32 haptics_play_rate = 0;
    pm_haptics_status haptics_status;
	
    if ((haptics_ptr == NULL) || (enable >= PM_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (enable == PM_ON)
    {
      data = 0x80;
    }
    else
    {
	  err_flag |= pm_haptics_get_rate(pmic_chip, PM_HAPTICS_RATE_LRA_AUTO_RES, &lra_auto_res_rate);
	  err_flag |= pm_haptics_get_rate(pmic_chip, PM_HAPTICS_RATE_PLAY_CONF, &haptics_play_rate);
	  err_flag |= pm_haptics_get_status(pmic_chip, &haptics_status);
	  
	  // check if 1*haptics_play_rate_original < lra_auto_res_rate < 1.2*haptics_play_rate_original
	  if ((haptics_status.auto_res_err == FALSE)               &&
	      ((lra_auto_res_rate * 10) < (haptics_play_rate * 12)) &&
	  	((lra_auto_res_rate * 10) > (haptics_play_rate * 8)))
	  {
	  	err_flag |= pm_haptics_config_haptics_rate(pmic_chip, lra_auto_res_rate);	 
	  }
	  
      data = 0x00;
    }
	
    reg_haptics_en_ctl = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->en_ctl1;
    err_flag |= pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_haptics_en_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_haptics_set_actuator_type( uint32 pmic_chip, pm_haptics_actuator_type actuator_type)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_actuator_type = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data = 0;
    pm_register_mask_type mask = 0x01;
    
    if ((haptics_ptr == NULL) || (actuator_type >= PM_HAPTICS_ACTUATOR_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (actuator_type == PM_HAPTICS_ACTUATOR_LRA)
    {
      data = 0x00;
    }
    else
    {
      data = 0x01;
    }
    reg_actuator_type = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->actuator_type;
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_actuator_type, mask, data, 0);

    return err_flag;
}


pm_err_flag_type pm_haptics_wf_sel( uint32 pmic_chip, pm_haptics_wf_source_type wf_source_type, pm_haptics_wf_trigger_type wf_trigger_type)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_wf_sel_type = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data = 0;
	uint8 reg_wf_sel_mask = 0x31;
    
    if (haptics_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (wf_source_type >= PM_HAPTICS_WF_SOURCE_INVALID)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else if (wf_trigger_type >= PM_HAPTICS_WF_TRIG_INVALID)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }
	else
    {
      reg_wf_sel_type = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->sel;	  
	  data = (pm_register_data_type) ((wf_source_type << 4) | wf_trigger_type);
	  
      err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_wf_sel_type, reg_wf_sel_mask, data, 0);
    }
	
    return err_flag;
}


pm_err_flag_type pm_haptics_fill_wf_buff( uint32 pmic_chip, pm_haptics_sample *wf_arry, size_t length)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_wf_s0 = 0;
	int arry_index = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data_arry[PM_HAPTICS_WF_S_NUM] = {0};
    
    if (haptics_ptr == NULL || wf_arry == NULL || length > PM_HAPTICS_WF_S_NUM)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
	
	for(arry_index = 0; arry_index < length; arry_index++)
    {
	  if((wf_arry[arry_index].sign >= PM_HAPTICS_SIGN_INVALID)|| (wf_arry[arry_index].overdrive >= PM_HAPTICS_OVD_INVALID) || (wf_arry[arry_index].amp_mv >= PM_HAPTICS_VMAX_VOLT_HIGH))
	  {
		return PM_ERR_FLAG_INVALID_PARAMETER;  
	  }
	  else
	  {
	    data_arry[arry_index] = (pm_register_data_type)(((wf_arry[arry_index].sign) << 7) | ((wf_arry[arry_index].overdrive) << 6) | ((wf_arry[arry_index].amp_mv / PM_HAPTICS_VMAX_VOLT_FACTOR) << 1));
	  }

	}		
		
      reg_wf_s0 = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->wf_s0;	  	  
      err_flag = pm_comm_write_byte_array(haptics_ptr->comm_ptr->slave_id, reg_wf_s0, PM_HAPTICS_WF_S_NUM, data_arry, 0);
	
    return err_flag;
}


pm_err_flag_type pm_haptics_set_lra_resonance_type( uint32 pmic_chip, pm_haptics_lra_res_type lra_res)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_lra_resonance = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data = 0;
    pm_register_mask_type mask = 0x01;
    
    if ((haptics_ptr == NULL) || (lra_res >= PM_HAPTICS_LRA_RES_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (lra_res == PM_HAPTICS_LRA_RES_SIN)
    {
      data = 0x00;
    }
    else
    {
      data = 0x01;
    }
    reg_lra_resonance = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->lra_res_type;
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_lra_resonance, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_haptics_set_slew_rate( uint32 pmic_chip, pm_haptics_slew_rate slew_rate)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_slew_rate = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data = 0;
    pm_register_mask_type mask = 0x01;
    
    if ((haptics_ptr == NULL) || (slew_rate >= PM_HAPTICS_SLEW_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (slew_rate == PM_HAPTICS_SLEW_6NS)
    {
      data = 0x00;
    }
    else
    {
      data = 0x01;
    }
    reg_slew_rate = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->slew_rate;
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_slew_rate, mask, data, 0);

    return err_flag;
}



pm_err_flag_type pm_haptics_play( uint32 pmic_chip, pm_haptics_vibrate_type vibrate)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
  pm_register_data_type auto_res_data = 0;
  pm_register_data_type actuator_type_data = 0;

    if ((haptics_ptr == NULL) || ( vibrate >= PM_HAPTICS_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

  pm_register_address_type reg_haptics_vibrate_on_off = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->play;

    if (vibrate == PM_HAPTICS_PLAY)
    {
    pm_register_address_type auto_res_cfg = haptics_ptr->haptics_reg_table->base_address  + haptics_ptr->haptics_reg_table->auto_res_cfg;
    pm_register_address_type actuator_type = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->actuator_type;

    /*read current actuator type and auto resonance registers*/
    err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, actuator_type, &actuator_type_data, 0);
    err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, auto_res_cfg, &auto_res_data, 0);

    /*If LRA actuator and LRA_AUTO_RES_MODE_QWD*/ 
    if ((((auto_res_data & PM_HAPTICS_LRA_AUTO_RES_MODE_MASK)>>0x4) == PM_HAPTICS_LRA_AUTO_RES_MODE_QWD) 
          && ((actuator_type_data & PM_HAPTICS_ACTUATOR_TYPE_MASK) == PM_HAPTICS_ACTUATOR_LRA)
          )
    {
      /*disable auto res mode*/
      auto_res_data = (auto_res_data & ~PM_HAPTICS_LRA_AUTO_RES_MODE_MASK) | (PM_HAPTICS_LRA_AUTO_RES_MODE_NO_AUTO_RES << 4);
      err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, auto_res_cfg, PM_HAPTICS_AUTO_RES_CFG_MASK, auto_res_data, 0);

      /*enable haptics play bit*/
      err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_haptics_vibrate_on_off, PM_HAPTICS_PLAY_MASK, 0x80, 0);
      /*wait 20ms*/
      DALSYS_BusyWait(20000);
      /*enable auto res with QWD mode*/
      err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, auto_res_cfg, &auto_res_data, 0);
      auto_res_data = (auto_res_data & ~PM_HAPTICS_LRA_AUTO_RES_MODE_MASK) |  (PM_HAPTICS_LRA_AUTO_RES_MODE_QWD << 4);
      err_flag |= pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, auto_res_cfg, PM_HAPTICS_AUTO_RES_CFG_MASK, auto_res_data, 0);
    }
    else
    {
     /* Non-QWD mode LRA and ERM can be enabled without delay */
      err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_haptics_vibrate_on_off, PM_HAPTICS_PLAY_MASK, 0x80, 0);
    }
    }
    else
    {
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_haptics_vibrate_on_off, PM_HAPTICS_PLAY_MASK, 0x0, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_haptics_set_vmax_volt( uint32 pmic_chip, uint32 vmax_cfg_volt)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type reg_haptics_set_vmax = 0;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_data_type data = 0;
    pm_register_mask_type mask = 0x3E;

    if (haptics_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (vmax_cfg_volt > PM_HAPTICS_VMAX_VOLT_HIGH || vmax_cfg_volt < PM_HAPTICS_VMAX_VOLT_LOW )
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
      data = vmax_cfg_volt/PM_HAPTICS_VMAX_VOLT_FACTOR; 
    }
    reg_haptics_set_vmax = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->vmax_cfg;
    /*bit<0> of this register is not used so actual vmax code written into the regiser should be shifted by 1*/
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, reg_haptics_set_vmax, mask, (data <<1), 0);

    return err_flag;
}


pm_err_flag_type pm_haptics_internal_pwm_freq_sel (uint32 pmic_chip, uint32 freq_sel_data_in_khz)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_address_type pwm_freq_sel;
    pm_register_data_type data = 0;

  if (haptics_ptr == NULL)
    {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else 
    {
    /*taking mean of two boudries to round off */
    if (freq_sel_data_in_khz < ((505 + 253) / 2))
    { /*0 to 378 KHZ*/
      data = 0x0;
    }
    else if (freq_sel_data_in_khz < ((739 + 505) / 2))
    { /*379 to 621KHZ*/
      data = 1;
    }
    else if (freq_sel_data_in_khz < ((1076 + 739) / 2))
    { /*622 to 905KHZ*/
      data = 2;
    }
    else
    { /*906KHZ onwards*/
      data = 3;
    }
    
    pwm_freq_sel = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->internal_pwm;
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, pwm_freq_sel ,0x03, data, 0);
    
  }

    return err_flag;
}

pm_err_flag_type pm_haptics_external_pwm_freq_sel (uint32 pmic_chip, uint32 freq_sel_data_in_khz)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
    pm_register_address_type pwm_freq_sel;
    pm_register_data_type data = 0;

    if ((haptics_ptr == NULL)  || (freq_sel_data_in_khz > 100) || (freq_sel_data_in_khz < 25))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else 
    {
    /*taking mean of two boudries to round off */
    if (freq_sel_data_in_khz < ((50 + 25) / 2))
    { /*0 to 37 KHZ*/
      data = 0x0;
    }
    else if (freq_sel_data_in_khz < ((75 + 50) / 2))
    { /*38 to 62KHZ*/
      data = 1;
    }
    else if (freq_sel_data_in_khz < ((100 + 75) / 2))
    { /*63 to 87 KHZ*/
      data = 2;
    }
    else
    { /*88KHZ onwards*/
      data = 3;
    }
    }
    
    pwm_freq_sel = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->external_pwm;
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, pwm_freq_sel ,0x03, data, 0);
    
    return err_flag;
}


pm_err_flag_type pm_haptics_pwm_cap_sel (uint32 pmic_chip, pm_haptics_pwm_cap_sel_type cap_sel_data)
{
   
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
   pm_register_address_type pwm_cap_sel;

   if ((haptics_ptr == NULL)  || (cap_sel_data >= PM_HAPTICS_PWM_CAP_INVALID))
   {
     return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   
   pwm_cap_sel = haptics_ptr->haptics_reg_table->base_address  + haptics_ptr->haptics_reg_table->pwm_cap_sel;
   err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, pwm_cap_sel ,0x03, cap_sel_data, 0);

   return err_flag;
  
}

pm_err_flag_type pm_haptics_wf_repeat  (uint32 pmic_chip, pm_haptics_waveform_repeat_type wf_repeat_data,pm_haptics_waveform_s_repeat_type wf_s_repeat_data)
{
   
   pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
   pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
   pm_register_address_type wf_repeat = 0;
   pm_register_data_type data = 0;

   if ((haptics_ptr == NULL) || (wf_repeat_data >= PM_HAPTICS_WF_REPEAT_INVALID) || (wf_s_repeat_data >= PM_HAPTICS_WF_S_REPEAT_INVALID))
   {
     return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   
   wf_repeat = haptics_ptr->haptics_reg_table->base_address  + haptics_ptr->haptics_reg_table->wf_repeat;
   data = (wf_repeat_data<<4) | wf_s_repeat_data ;
   err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, wf_repeat,0x73, data, 0);
   
   return err_flag;
  
}

pm_err_flag_type pm_haptics_config_haptics_rate(uint32 pmic_chip, uint32 config_rate_in_us)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
  pm_register_address_type rate_cfg1_reg;
  pm_register_address_type rate_cfg2_reg;
  uint32 data = 0;
  pm_register_data_type rate_cfg1_data;
  pm_register_data_type rate_cfg2_data;


  if ((haptics_ptr == NULL) || (config_rate_in_us >= PM_HAPTICS_RATE_CONFIG_TIME_MAX) || (config_rate_in_us <= 0))
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  data = (config_rate_in_us * 192) / 960; // play rate period=(96*code)/(19.2MHz)
  rate_cfg1_data = data & 0xFF;
  rate_cfg2_data = (data & 0xF00) >> 8;



  rate_cfg1_reg = haptics_ptr->haptics_reg_table->base_address  + haptics_ptr->haptics_reg_table->rate_cfg1;
  err_flag = pm_comm_write_byte(haptics_ptr->comm_ptr->slave_id, rate_cfg1_reg, rate_cfg1_data, 0);

  rate_cfg2_reg = haptics_ptr->haptics_reg_table->base_address  + haptics_ptr->haptics_reg_table->rate_cfg2;
  err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, rate_cfg2_reg, 0x0F, rate_cfg2_data, 0);


  return err_flag;
}


pm_err_flag_type pm_haptics_auto_res_cfg(uint32                            pmic_chip, 
                                         pm_haptics_lra_res_calib_type     lra_res_calib, 
                                         pm_haptics_lra_high_z_type        lra_high_z, 
                                         pm_haptics_lra_auto_res_mode_type auto_res_mode)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
  pm_register_address_type auto_res_cfg = 0;
  pm_register_data_type data = 0;


  if ((haptics_ptr == NULL) 
      || (lra_res_calib >= PM_HAPTICS_LRA_RES_CALIB_INVALID) 
      || (lra_high_z >= PM_HAPTICS_LRA_HIGH_Z_INVALID)
      || (auto_res_mode>= PM_HAPTICS_LRA_AUTO_RES_MODE_INVALID)
      )
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    data = (lra_res_calib) | (lra_high_z << 0x2) | (auto_res_mode<<4);
    auto_res_cfg = haptics_ptr->haptics_reg_table->base_address  + haptics_ptr->haptics_reg_table->auto_res_cfg;
    err_flag = pm_comm_write_byte_mask(haptics_ptr->comm_ptr->slave_id, auto_res_cfg, PM_HAPTICS_AUTO_RES_CFG_MASK, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_haptics_get_status(uint32 pmic_chip, pm_haptics_status *haptics_status)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
  
  pm_register_data_type data;
  pm_register_address_type status_reg;
  

  if (haptics_ptr == NULL)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (haptics_status == NULL)
  {
	return PM_ERR_FLAG_INVALID_PARAMETER;;  
  }
  else
  {            
     status_reg = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->status1;
     err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, status_reg, &data, 0); 
	 
     haptics_status->play_sts       = (data & 0x01) ? TRUE : FALSE;
	 haptics_status->busy           = (data & 0x02) ? TRUE : FALSE;
	 haptics_status->oc_flag        = (data & 0x04) ? TRUE : FALSE;
	 haptics_status->sc_flag        = (data & 0x08) ? TRUE : FALSE;
	 haptics_status->auto_res_err   = (data & 0x10) ? TRUE : FALSE;
	 haptics_status->en_sts         = (data & 0x80) ? TRUE : FALSE;
   }	   
   
   return err_flag;  
}


pm_err_flag_type pm_haptics_get_rate(uint32 pmic_chip, pm_haptics_rate_type rate_type, uint32 *rate)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_haptics_data_type *haptics_ptr = pm_haptics_get_data(pmic_chip);
  
  pm_register_address_type reg1;
  pm_register_address_type reg2;
  uint32 data = 0;
  pm_register_data_type data1;
  pm_register_data_type data2;
  

  if (haptics_ptr == NULL)
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (rate == NULL)
  {
	return PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {      
     switch(rate_type)
     {
      case PM_HAPTICS_RATE_PLAY_CONF:
	     reg1 = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->rate_cfg1;
		 reg2 = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->rate_cfg2;
		 err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, reg1, &data1, 0);       
         err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, reg2, &data2, 0);	 	    
	     data = data1 | ((data2 & 0x0F) << 8);
         break;	
      case PM_HAPTICS_RATE_LRA_AUTO_RES:	  
         reg1 = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->auto_res_lo;
		 reg2 = haptics_ptr->haptics_reg_table->base_address + haptics_ptr->haptics_reg_table->auto_res_hi;		
         err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, reg1, &data1, 0);       
         err_flag |= pm_comm_read_byte(haptics_ptr->comm_ptr->slave_id, reg2, &data2, 0);	 	    
	     data = data1 | ((data2 >> 4)<<8);
         break;
      default:
	     return PM_ERR_FLAG_INVALID_PARAMETER;
     }
	 
     *rate = (data * 960) / 192;   // rate period=(96*code)/(19.2MHz)	 
  }
  
  return err_flag;
}
