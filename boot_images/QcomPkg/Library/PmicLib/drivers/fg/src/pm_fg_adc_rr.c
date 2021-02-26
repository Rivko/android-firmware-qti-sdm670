
/*! \file pm_fg_adc_rr.c 
*  \n
*  \brief  PMIC-BMS MODULE RELATED DECLARATION
*  \details  This file contains functions and variable declarations to support 
*   the PMIC Fule Gauge RR ADC .
*
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when          who     what, where, why
--------      ---     ----------------------------------------------------------
06/13/17      czq     correct API pm_fg_adc_rr_batt_id_cfg error.
03/15/17      cs      fixed wrong function name 
10/06/16      cs      configure batt therm/aux therm bias wait 
08/25/16      pxm     Add skin temperature threshold configuration API
04/01/16      aab     Added more ADC RR APIs
01/27/16      al      Initial Release 
===========================================================================*/

#include "pm_fg_adc_rr.h"
#include "pm_fg_driver.h"


/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

#define ADDRESS(x) (fg_adc_rr_ptr->fg_register->adc_rr_register->base_address + fg_adc_rr_ptr->fg_register->adc_rr_register->x)
#define SLAVE_ID   (fg_adc_rr_ptr->comm_ptr->slave_id)

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/
/**
* @brief This function grant access to FG ADC User access *
* @details
* This function grant access to FG ADC User access 
* 
* @param[in] pm_fg_data_type Self
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_adc_rr_unlock_perph_write(pm_fg_data_type *fg_adc_rr_ptr)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if(NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type sec_access = ADDRESS(sec_access);
    err_flag = pm_comm_write_byte(SLAVE_ID, sec_access, 0xA5, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_adc_rr_irq_enable(uint32 pmic_device, pm_fg_adc_rr_irq_type irq, boolean enable)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type irq_reg;
  pm_register_data_type data = 1 << irq;

  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_RR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    if (enable)
    {
      irq_reg = ADDRESS(int_en_set);
    }
    else
    {
      irq_reg = ADDRESS(int_en_clr);
    }

    err_flag = pm_comm_write_byte(SLAVE_ID, irq_reg, data, 0);
  }
  return err_flag;
}

pm_err_flag_type pm_fg_adc_rr_irq_clear(uint32 pmic_device, pm_fg_adc_rr_irq_type irq)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 1 << irq;
  pm_register_address_type int_latched_clr;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_RR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    int_latched_clr = ADDRESS(int_latched_clr);
    err_flag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_adc_rr_irq_set_trigger(uint32 pmic_device, pm_fg_adc_rr_irq_type irq, pm_irq_trigger_type trigger)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  uint8 mask = 1 << irq;
  pm_register_data_type set_type, polarity_high, polarity_low;
  pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_RR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    int_set_type = ADDRESS(int_set_type);
    int_polarity_high = ADDRESS(int_polarity_high);
    int_polarity_low = ADDRESS(int_polarity_low);

    switch (trigger)
    {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
        set_type = 0x00;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
        set_type = 0x00;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_RISING_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_FALLING_EDGE:
        set_type = 0xFF;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_DUAL_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0xFF;
        break;
    default:
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    err_flag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_adc_rr_irq_status(uint32 pmic_device, pm_fg_adc_rr_irq_type irq, pm_irq_status_type type, boolean *status)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data;
  uint8 mask = 1 << irq;
  pm_register_address_type int_sts;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (irq >= PM_FG_ADC_RR_IRQ_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    switch (type)
    {
    case PM_IRQ_STATUS_RT:
      int_sts = ADDRESS(int_rt_sts);
        break;
    case PM_IRQ_STATUS_LATCHED:
      int_sts = ADDRESS(int_latched_sts);
        break;
    case PM_IRQ_STATUS_PENDING:
      int_sts = ADDRESS(int_pending_sts);
        break;
    default:
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(SLAVE_ID, int_sts, mask, &data, 0);
    *status = data ? TRUE : FALSE;
  }

  return err_flag;
}




pm_err_flag_type pm_fg_adc_rr_ref_cfg(uint32 pmic_device, pm_fg_adc_rr_ref_vreg_fg_level_type vreg_fg_level, boolean vreg_fg_bypass)
{ //ref_cfg(Reg offset:0x054)   
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type vreg_fg_level_reg;
  uint8 data = 0x08;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (vreg_fg_level >= PM_FG_ADC_RR_REF_VREG_FG_LEVEL_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    vreg_fg_level_reg = ADDRESS(ref_cfg);

    data = vreg_fg_level | (vreg_fg_bypass << 3);

    err_flag = pm_comm_write_byte_mask(SLAVE_ID, vreg_fg_level_reg, 0x0F, data, 0);
  }

  return err_flag;
}




pm_err_flag_type pm_fg_adc_rr_rbiase_cnls_buf_sel(uint32 pmic_device, boolean rbias_cnls_buf_sel)
{//rbias_cnls((Reg offset:0x055)
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type rbias_cnls_reg;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    rbias_cnls_reg = ADDRESS(rbias_cnls);

    err_flag = pm_comm_write_byte_mask(SLAVE_ID, rbias_cnls_reg, 0x01, rbias_cnls_buf_sel, 0);
  }

  return err_flag;
}




pm_err_flag_type pm_fg_adc_rr_batt_id_cfg(uint32 pmic_device, pm_fg_adc_rr_bias_wait_type batt_id_cfg_bias_wait)
{ //batt_id_cfg(Reg offset:0x063)
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type bias_wait_reg;
  uint8 data = 0;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (batt_id_cfg_bias_wait >= PM_FG_ADC_RR_BIAS_WAIT_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    bias_wait_reg = ADDRESS(batt_id_cfg);

    data = batt_id_cfg_bias_wait << 5;

    err_flag = pm_comm_write_byte_mask(SLAVE_ID, bias_wait_reg, 0xE0, data, 0);
  }

  return err_flag;
}





pm_err_flag_type pm_fg_set_adc_rr_batt_therm_cfg(uint32 pmic_device, pm_fg_adc_rr_bias_wait_type batt_therm_cfg_bias_wait, boolean gnd_sel)
{ //batt_therm_cfg(Reg Offset:0x073)
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type bias_wait_reg;
  uint8 data = 0;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (batt_therm_cfg_bias_wait >= PM_FG_ADC_RR_BIAS_WAIT_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    bias_wait_reg = ADDRESS(batt_therm_cfg);

    data = (batt_therm_cfg_bias_wait << 5) | gnd_sel;

    err_flag = pm_comm_write_byte_mask(SLAVE_ID, bias_wait_reg, 0xE1, data, 0);
  }

  return err_flag;
}



pm_err_flag_type pm_fg_adc_rr_aux_therm_cfg( uint32 pmic_device, pm_fg_adc_rr_aux_therm_cfg_type* aux_therm_cfg)
{ //aux_therm_cfg(Reg Offset:0x083)
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type aux_therm_cfg_reg;
  uint8 data = 0;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (aux_therm_cfg->aux_therm_cfg_bias_wait >= PM_FG_ADC_RR_BIAS_WAIT_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else if (aux_therm_cfg->tmp_hyst >= PM_FG_ADC_RR_AUX_THERM_CFG_TMP_HYST_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    aux_therm_cfg_reg = ADDRESS(aux_therm_cfg);

    data = (aux_therm_cfg->aux_therm_cfg_bias_wait << 5) | 
           (aux_therm_cfg->tmp_hyst << 3)                | 
           (aux_therm_cfg->batt_therm_sync << 2)         | 
           (aux_therm_cfg->aux_therm_mode << 1)          | 
            aux_therm_cfg->gnd_sel;

    err_flag = pm_comm_write_byte(SLAVE_ID, aux_therm_cfg_reg, data, 0);
  }

  return err_flag;
}




pm_err_flag_type pm_fg_adc_rr_set_aux_therm_coefficient(uint32 pmic_device, pm_fg_adc_rr_aux_therm_coefficient_type aux_therm_coef, uint8 coef_value)
{ //aux_therm_c1(Reg Offset:0x088);  aux_therm_c2(Reg Offset:0x089) ; aux_therm_c3(Reg Offset:0x08A);  aux_therm_half_range(Reg Offset:0x08B)
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type aux_therm_coef_reg;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (aux_therm_coef >= PM_FG_ADC_RR_AUX_THERM_COEF_INVALID)
  {
    err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    switch (aux_therm_coef)
    {
    case PM_FG_ADC_RR_AUX_THERM_COEF_C1:
        aux_therm_coef_reg = ADDRESS(aux_therm_c1);
        break;
    case PM_FG_ADC_RR_AUX_THERM_COEF_C2:
        aux_therm_coef_reg = ADDRESS(aux_therm_c2);
        break;
    case PM_FG_ADC_RR_AUX_THERM_COEF_C3:
        aux_therm_coef_reg = ADDRESS(aux_therm_c3);
        break;
    case FG_FG_ADC_RR_AUX_THERM_HALF_RANGE:
        aux_therm_coef_reg = ADDRESS(aux_therm_half_range);
        break;
        
    default:
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    err_flag |= pm_comm_write_byte(SLAVE_ID, aux_therm_coef_reg, coef_value, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_adc_rr_set_skin_temp_threshold(uint32 pmic_device, pm_fg_adc_rr_charger_temp_type skin_temp_type, int temp)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_data_type    data = 0;
   pm_register_address_type reg = 0;

   pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

   if (NULL == fg_adc_rr_ptr)
   {
    err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (skin_temp_type >= PM_FG_ADC_RR_FG_CHARGER_TEMP_INVALID)
   {
    err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else if (temp < PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MIN || temp > PM_FG_ADC_RR_FG_CHARGER_SKIN_TEMP_MAX)
   {
    err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      switch (skin_temp_type)
      {
      case PM_FG_ADC_RR_FG_CHARGER_TEMP_HOT:
         reg = ADDRESS(skin_hot);
         break;

      case PM_FG_ADC_RR_FG_CHARGER_TEMP_TOO_HOT:
         reg = ADDRESS(skin_too_hot);
         break;

      default:
         return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
         break;
      }
	  
	  data = (pm_register_data_type)((temp+30)*2);

      err_flag = pm_comm_write_byte(SLAVE_ID, reg, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_fg_adc_rr_make_sw_req(uint32 pmic_device)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type reg = 0;
   pm_register_data_type data = 0;

   
   pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

   if (NULL == fg_adc_rr_ptr)
   {
    err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      reg = ADDRESS(rr_adc_ctl);
      data = pm_comm_read_byte_mask(SLAVE_ID, reg, 0x2, &data, 0);
	  data = ~data;
      pm_comm_write_byte_mask(SLAVE_ID, reg, 0x2, data,0);	  
   }

   return err_flag;
}

pm_err_flag_type pm_fg_adc_rr_batt_id_trig(uint32 pmic_device, boolean enable)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   pm_register_address_type reg = 0;
   pm_register_data_type data = (enable ? 0x1: 0x0);

   
   pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

   if (NULL == fg_adc_rr_ptr)
   {
    err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else
   {
      reg = ADDRESS(batt_id_trigger);
      err_flag = pm_comm_write_byte(SLAVE_ID, reg, data, 0);
   }

   return err_flag;
}

pm_err_flag_type pm_fg_adc_rr_set_fake_batt_id(uint32 pmic_device, uint32 fake_batt_min_ohms, uint32 fake_batt_max_ohms)
{
   pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
   uint32                   fake_batt_min_code = 0;
   uint32                   fake_batt_max_code = 0;
   pm_register_address_type reg = 0;
   pm_register_data_type    fake_batt_reg_data[4] = {0};
   
   pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

   if (NULL == fg_adc_rr_ptr)
   {
    err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
   }
   else if (fake_batt_max_ohms > PM_FG_ADC_RR_FG_FAKE_BATT_MAX || fake_batt_min_ohms > fake_batt_max_ohms)
   {
    err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
   }
   else
   {
      reg = ADDRESS(fake_batt_low_lsb);
      fake_batt_min_code = (fake_batt_min_ohms * PM_FG_ADC_RR_FG_FAKE_BATT_CI* 1024)/PM_FG_ADC_RR_FG_FAKE_BATT_FS;
	  fake_batt_max_code = (fake_batt_max_ohms * PM_FG_ADC_RR_FG_FAKE_BATT_CI* 1024)/PM_FG_ADC_RR_FG_FAKE_BATT_FS;
	  fake_batt_reg_data[0] = (fake_batt_min_code%0x100) & 0xFF;  //fake_batt_low_lsb
      fake_batt_reg_data[1] = (fake_batt_min_code/0x100) & 0xFF;  //fake_batt_low_msb
	  fake_batt_reg_data[2] = (fake_batt_max_code%0x100) & 0xFF;  //fake_batt_high_lsb
      fake_batt_reg_data[3] = (fake_batt_max_code/0x100) & 0xFF;  //fake_batt_high_msb
	  
      err_flag = pm_comm_write_byte_array(SLAVE_ID, reg, 4, fake_batt_reg_data, 0);
   }

   return err_flag;
}


pm_err_flag_type pm_fg_adc_rr_batt_id_ctrl(uint32 pmic_device, pm_fg_adc_rr_batt_id_ctrl_type batt_id_ctrl_type, boolean enable)
{ 
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type batt_id_ctrl_reg;
  pm_register_data_type data   = enable? 0xFF:0x0;
  uint8 mask = 1 << batt_id_ctrl_type;
  pm_fg_data_type* fg_adc_rr_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_adc_rr_ptr)
  {
    err_flag |= PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (batt_id_ctrl_type >= PM_FG_ADC_RR_BATT_ID_CTRL_INVALID)
  {
    err_flag |= PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
	batt_id_ctrl_reg = ADDRESS(batt_id_ctrl);
    err_flag |= pm_comm_write_byte_mask(SLAVE_ID, batt_id_ctrl_reg, mask, data, 0);
  }

  return err_flag;
}


