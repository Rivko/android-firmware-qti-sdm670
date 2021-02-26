
/*! \file pm_fg_batt.c 
*  \n
*  \brief  PMIC-BMS MODULE RELATED DECLARATION
*  \details  This file contains functions and variable declarations to support 
*   the PMIC Fule Gauge memory Battery interface module.
*
*  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/02/17   cs      sbl need to set batt id threshold to 0, it is too late for uefi to do it.
01/30/17   sm      set batt id missing threshold to highest
10/06/16   cs      change batt missing config
07/18/16   llioe   Handled register-address differences v1.1(and earlier) vs v2.0
06/29/16   va      updating IBat, Vbat calculation
06/16/16   va      Reading IBat, Vbat from batt info
06/01/16   va      Update Battery Temp calculation
03/24/16   va      Adding batt missing cfg api
03/16/16   va      Adding pm_fg_batt_info_get_batt_temp_sts() 
02/16/16   va      Adding required API for enanlbing charger
01/27/16   al      Initial Release 
===========================================================================*/

#include "pm_fg_batt_info.h"
#include "pm_fg_driver.h"
#include "pm_resource_manager.h"

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

/*===========================================================================
                        TYPE DEFINITIONS 
===========================================================================*/

#define ADDRESS(x) (fg_batt_ptr->fg_register->batt_info_register->base_address + fg_batt_ptr->fg_register->batt_info_register->x)
#define SLAVE_ID   (fg_batt_ptr->comm_ptr->slave_id)

#define PM_FG_K_CONSTANT 273

#define PM_FG_BATT_INFO_MAX_RETRY      5

#define PM_FG_IBAT_INT_VAL         32768 /* (2^15) */
#define PM_FG_IBAT_MAX             16000

#define PM_FG_VBATT_MAXUINT16      65535 /* (2 ^ 16)*/
#define PM_FG_VBATT_MAX            8000

#define BATT_MISSING_THR_MAX          3

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/
/**
* @brief This function grant access to FG batt access *
* @details
* This function grant access to FG batt access 
* 
* @param[in] pm_fg_data_type Self
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_info_unlock_perph_write(pm_fg_data_type *fg_batt_ptr)
{

  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  if (NULL == fg_batt_ptr)
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


pm_err_flag_type pm_fg_batt_irq_enable(uint32 pmic_device, pm_fg_batt_info_irq_type irq, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type irq_reg;
    pm_register_data_type data = 1 << irq;

    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }               
    else if (irq >= PM_FG_BATT_INFO_IRQ_INVALID)
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

pm_err_flag_type pm_fg_batt_irq_clear(uint32  pmic_device, pm_fg_batt_info_irq_type irq)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data = 1 << irq;
    pm_register_address_type int_latched_clr;
    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_FG_BATT_INFO_IRQ_INVALID)
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


pm_err_flag_type pm_fg_batt_irq_set_trigger(uint32 pmic_device, pm_fg_batt_info_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    uint8 mask = 1 << irq;
    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_FG_BATT_INFO_IRQ_INVALID)
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


pm_err_flag_type pm_fg_batt_irq_status(uint32 pmic_device, pm_fg_batt_info_irq_type irq, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data;
    uint8 mask = 1 << irq;
    pm_register_address_type int_sts;
    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_FG_BATT_INFO_IRQ_INVALID)
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



pm_err_flag_type pm_fg_batt_info_clear_jeita_status(uint32 pmic_device, boolean clear_status)
{
 pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data = (clear_status)? 0xFF : 0x00;

    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type jeita_ctls = ADDRESS(jeita_ctls);

        err_flag = pm_comm_write_byte_mask(SLAVE_ID, jeita_ctls, 0x1, data, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_fg_batt_info_set_therm_coeff_value(uint32 pmic_device, pm_fg_batt_info_therm_coeff_type coeff_type, uint8 coeff_value)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type therm_c = 0;

    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (coeff_type >= PM_FG_BATT_INFO_THERM_COEF_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        switch (coeff_type)
        {
        case PM_FG_BATT_INFO_THERM_COEFF_C1:
            therm_c = ADDRESS(therm_therm_c1);
            break;
        case PM_FG_BATT_INFO_THERM_COEFF_C2:
            therm_c = ADDRESS(therm_therm_c2);
            break;
        case PM_FG_BATT_INFO_THERM_COEFF_C3:
            therm_c = ADDRESS(therm_therm_c3);
            break;
        case PM_FG_BATT_INFO_THERM_HALF_RANGE:
              therm_c = ADDRESS(therm_therm_half_range);
            break;
        default:
            return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

        err_flag = pm_comm_write_byte(SLAVE_ID, therm_c, (pm_register_data_type)coeff_value, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_fg_batt_info_set_jeita_threshold_value(uint32 pmic_device, pm_fg_batt_info_jeita_threshold_type threshold_type, uint8 threshold_value)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type jeita_reg = 0;

    pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);

    if (NULL == fg_batt_ptr)
    {
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (threshold_type >= PM_FG_BATT_INFO_JEITA_THRESHOLD_INVALID)
    {
        err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        switch (threshold_type)
        {
        case PM_FG_BATT_INFO_JEITA_THRESHOLD_TOO_COLD:
            jeita_reg = ADDRESS(jeita_too_cold);
            break;
        case PM_FG_BATT_INFO_JEITA_THRESHOLD_COLD:
            jeita_reg = ADDRESS(jeita_cold);
            break;
        case PM_FG_BATT_INFO_JEITA_THRESHOLD_HOT:
            jeita_reg = ADDRESS(jeita_hot);
            break;
        case PM_FG_BATT_INFO_JEITA_THRESHOLD_TOO_HOT:
            jeita_reg = ADDRESS(jeita_too_hot);
            break;
        default:
            return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

        err_flag = pm_comm_write_byte(SLAVE_ID, jeita_reg, (pm_register_data_type)threshold_value, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_fg_batt_info_get_batt_temp(uint32 pmic_device, int32 *batt_temp)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_register_data_type data_lsb = 0, data_msb = 0;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == batt_temp)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type batt_temp_lsb_reg = ADDRESS(battery_temperature_lsb);
    pm_register_address_type batt_temp_msb_reg = ADDRESS(battery_temperature_msb);
    err_flag  = pm_comm_read_byte(SLAVE_ID, batt_temp_lsb_reg, &data_lsb, 0);
    err_flag |= pm_comm_read_byte_mask(SLAVE_ID, batt_temp_msb_reg, 0x07, &data_msb, 0);

    /* Append MSB to LSB */
    *batt_temp = ((data_msb << 8 | data_lsb ) / 4 ) - PM_FG_K_CONSTANT; /* encoding 0.5C -30C = C*/
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_info_get_batt_temp_hyst_cfg(uint32 pmic_device, pm_fg_batt_info_batt_temp_hyst_cfg *hyst_cfg)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_register_data_type data = 0;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == hyst_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type hyst_cfg_reg = ADDRESS(battery_temp_cfg);
    err_flag  = pm_comm_read_byte_mask(SLAVE_ID, hyst_cfg_reg, 0x30, &data, 0);
    *hyst_cfg = (pm_fg_batt_info_batt_temp_hyst_cfg)((data & 0x30) >> 4);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_batt_info_set_batt_temp_hyst_cfg(uint32 pmic_device, pm_fg_batt_info_batt_temp_hyst_cfg hyst_cfg)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (hyst_cfg >= PM_FG_BATT_INFO_BATT_TEMP_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type hyst_cfg_reg = ADDRESS(battery_temp_cfg);
    err_flag  = pm_comm_write_byte_mask(SLAVE_ID, hyst_cfg_reg, 0x30, (pm_register_data_type)(hyst_cfg << 4 ), 0);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_info_get_ibatt_sense_cfg(uint32 pmic_device, pm_fg_batt_info_ibatt_sensing_cfg *ibatt_sense_cfg)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;

  pm_register_data_type data = 0;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == ibatt_sense_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type ibatt_sense_cfg_reg = ADDRESS(ibatt_sensing_cfg);
    err_flag  = pm_comm_read_byte(SLAVE_ID, ibatt_sense_cfg_reg, &data, 0);
    ibatt_sense_cfg->adc_bitstream_inv = (boolean) (data & 0x10);
    ibatt_sense_cfg->source_select     = ( data & 0x03);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_info_set_ibatt_sense_cfg(uint32 pmic_device, pm_fg_batt_info_ibatt_sensing_cfg ibatt_sense_cfg)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data = 0;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (ibatt_sense_cfg.source_select > 2)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type ibatt_sense_cfg_reg = ADDRESS(ibatt_sensing_cfg);
    data =  (ibatt_sense_cfg.adc_bitstream_inv << 4 ) | ibatt_sense_cfg.source_select;
    err_flag  = pm_comm_write_byte_mask(SLAVE_ID, ibatt_sense_cfg_reg, 0x13, data, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_batt_info_get_batt_temp_sts(uint32 device_index, pm_fg_batt_info_batt_temp_sts temp_sts, boolean *status)
{
  pm_err_flag_type      err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_register_data_type data        = 0;
  pm_fg_data_type *fg_batt_ptr      = pm_fg_get_data(device_index);
  
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (temp_sts >= PM_FG_BATT_INFO_BATT_TEMP_STS_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type batt_temp_sts_addr = ADDRESS(battery_temp_sts);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, batt_temp_sts_addr, (1 << temp_sts), &data, 0);
    *status = data ? TRUE : FALSE;
  }
  return err_flag;

}


/**
* @brief This function returns  Fule Gauge Batt system status * 
* @details
*  This function returns BMS Fule Gauge Batt system status 
*
* @param[in] pmic_pmic_device. Primary: 0 Secondary: 1
* @param[out]pm_fg_batt_sys_sts system profile status
*
* @return  pm_err_flag_type 
*          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
*          version of the PMIC.
*          PM_ERR_FLAG_SUCCESS               = SUCCESS.
*
*/
pm_err_flag_type pm_fg_batt_info_get_sys_batt_sts(uint32 pmic_device, pm_fg_batt_info_sys_batt_sts batt_sys_sts, boolean *status)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  pm_register_address_type  fg_batt_sys_sts_addr = 0x00;
  pm_register_data_type data = 0x00;

  pm_fg_data_type*  fg_batt_ptr = pm_fg_get_data(pmic_device);

  if (NULL == fg_batt_ptr )
  {
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (batt_sys_sts >= PM_FG_BATT_INFO_BATT_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    fg_batt_sys_sts_addr = ADDRESS(sys_batt);
    err_flag = pm_comm_read_byte(SLAVE_ID, fg_batt_sys_sts_addr, &data, 0);
    err_flag = pm_comm_read_byte_mask(SLAVE_ID, fg_batt_sys_sts_addr , (1 << batt_sys_sts), &data, 0);
    *status = data ? TRUE : FALSE;
  }

  return err_flag;

}


pm_err_flag_type pm_fg_batt_info_clr_batt_removed_latched_sts(uint32 pmic_device)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type batt_rem_ltd_sts_reg = ADDRESS(batt_removed_latched);
    err_flag  = pm_comm_write_byte_mask(SLAVE_ID, batt_rem_ltd_sts_reg, 0x80, 0x80, 0);
  }

  return err_flag;
}


pm_err_flag_type pm_fg_batt_info_set_batt_missing_cfg(uint32 pmic_device, pm_fg_batt_info_batt_missing_cfg batt_missing_cfg)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  pm_register_data_type data = 0x00;
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (batt_missing_cfg.bm_therm_th >= PM_FG_BATT_INFO_BATT_BH_THERM_DRV_TH_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type batt_miss_reg = ADDRESS(battery_missing_cfg);
    data = (batt_missing_cfg.bm_from_batt_id) | (batt_missing_cfg.bm_from_therm << 1) | (batt_missing_cfg.bm_therm_th << 4) | (batt_missing_cfg.bm_batt_id_th << 2);
    err_flag  = pm_comm_write_byte_mask(SLAVE_ID, batt_miss_reg, 0x3F, data, 0);
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_info_get_batt_missing_cfg(uint32 pmic_device, pm_fg_batt_info_batt_missing_cfg *batt_missing_cfg)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  pm_register_data_type data = 0x00;
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == batt_missing_cfg)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type batt_miss_reg = ADDRESS(battery_missing_cfg);
    err_flag  = pm_comm_read_byte_mask(SLAVE_ID, batt_miss_reg, 0x3F, &data, 0);
    batt_missing_cfg->bm_from_batt_id = data & 0x01;
    batt_missing_cfg->bm_from_therm   = (data & 0x02) >> 1;
    batt_missing_cfg->bm_therm_th     = (data & 0x30) >> 4;
    batt_missing_cfg->bm_batt_id_th   = (data & 0x0C) >> 2;
  }

  return err_flag;
}


pm_err_flag_type pm_fg_batt_info_get_ibatt_match_cp(uint32 pmic_device, int32 *pIBat, boolean *ibat_match_cp)
{
  pm_err_flag_type  err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  pm_register_data_type data16[2] = {0};
  pm_register_data_type data16Cp[2] = {0};
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == pIBat || NULL == ibat_match_cp)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type ibatt_msb_reg = ADDRESS(ibatt_msb);
    pm_register_address_type ibatt_lsb_reg = ADDRESS(ibatt_lsb);
    pm_register_address_type ibatt_msb_cp_reg = ADDRESS(ibatt_msb_cp);
    pm_register_address_type ibatt_lsb_cp_reg = ADDRESS(ibatt_lsb_cp);

    err_flag =  pm_comm_read_byte(SLAVE_ID, ibatt_msb_reg, (uint8 *)&data16[1], 0);
    err_flag |= pm_comm_read_byte(SLAVE_ID, ibatt_lsb_reg, (uint8 *)&data16, 0);
    err_flag |= pm_comm_read_byte(SLAVE_ID, ibatt_msb_cp_reg, (uint8 *)&data16Cp[1], 0);
    err_flag |= pm_comm_read_byte(SLAVE_ID, ibatt_lsb_cp_reg, (uint8 *)&data16Cp, 0);

    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
       *pIBat = data16[1] << 8 | data16[0];
       *ibat_match_cp = (data16[1] << 8 | data16[0]) ^ (data16Cp[1] << 8 | data16Cp[0]) ? FALSE : TRUE;
    }
  }

  return err_flag;
}

pm_err_flag_type pm_fg_batt_info_get_ibatt(uint32 pmic_device, int32 *pIBat)
{
  pm_err_flag_type  err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  boolean ibatt_match_cp = FALSE;
  uint8  count = 0;
  INT32  IBatt = 0;
  INT16  SignedIBatt16 = 0; INT32 SignedIBatt32 = 0;
  INT32  result = 0;

  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == pIBat )
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag |= pm_fg_batt_info_get_ibatt_match_cp(pmic_device, &IBatt, &ibatt_match_cp);

    //If ibatt_match_cp is FALSE, retry
    if (ibatt_match_cp == FALSE)
    {
      for (count =0; count < PM_FG_BATT_INFO_MAX_RETRY; count++) 
      {
        err_flag |= pm_fg_batt_info_get_ibatt_match_cp(pmic_device, &IBatt, &ibatt_match_cp);
        if (ibatt_match_cp == TRUE)
        {
          break;
        }
      }
    }
    if (ibatt_match_cp == FALSE)
    {
      DEBUG(( EFI_D_WARN, "PmicDxe:: %a ibatt_match_cp = %d \n\r",__FUNCTION__, ibatt_match_cp));
      err_flag |= PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else
    {
      SignedIBatt16 = (int16)IBatt;
      SignedIBatt32 =  (int32)SignedIBatt16;
      result = (SignedIBatt32 * PM_FG_IBAT_MAX)/ PM_FG_IBAT_INT_VAL ;
      /* DEBUG(( EFI_D_WARN, "PmicDxe:: %a IBatt = 0x%x SignedIBatt = %d Result = %d mA IBatt = 0x%x mA\n\r",__FUNCTION__, IBatt, SignedIBatt32, result, result));*/
      *pIBat = result ;
    }
  }

return err_flag;

}


pm_err_flag_type pm_fg_batt_info_get_vbatt_match_cp(uint32 pmic_device, uint32 *pVBat, boolean *vbat_match_cp)
{
  pm_err_flag_type  err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  pm_register_data_type data16[2] = {0};
  pm_register_data_type data16Cp[2] = {0};
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == pVBat || NULL == vbat_match_cp)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type vbatt_msb_reg = ADDRESS(vbatt_msb);
    pm_register_address_type vbatt_lsb_reg = ADDRESS(vbatt_lsb);
    pm_register_address_type vbatt_msb_cp_reg = ADDRESS(vbatt_msb_cp);
    pm_register_address_type vbatt_lsb_cp_reg = ADDRESS(vbatt_lsb_cp);

    err_flag  = pm_comm_read_byte(SLAVE_ID, vbatt_msb_reg, (uint8 *)&data16[1], 0);
    err_flag |= pm_comm_read_byte(SLAVE_ID, vbatt_lsb_reg, (uint8 *)&data16[0], 0);
    err_flag |= pm_comm_read_byte(SLAVE_ID, vbatt_msb_cp_reg, (uint8 *)&data16Cp[1], 0);
    err_flag |= pm_comm_read_byte(SLAVE_ID, vbatt_lsb_cp_reg, (uint8 *)&data16Cp[0], 0);

    if (err_flag == PM_ERR_FLAG_SUCCESS)
    {
      *pVBat = data16[1] << 8 | data16[0];
      *vbat_match_cp = (data16[1] << 8 | data16[0]) ^ (data16Cp[1] << 8 | data16Cp[0]) ? FALSE:TRUE;
    }
  }

  return err_flag;
}


/*
Return vbat in Mv
*/pm_err_flag_type pm_fg_batt_info_get_vbatt(uint32 pmic_device, uint32 *pVBat)
{
  pm_err_flag_type  err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  boolean vbatt_match_cp = FALSE;
  uint8  count =0;
  uint32 VBatt = 0;

  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == pVBat )
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    err_flag |= pm_fg_batt_info_get_vbatt_match_cp(pmic_device, &VBatt, &vbatt_match_cp);

    //If vbatt_match_cp is FALSE, retry
    if (vbatt_match_cp == FALSE)
    {
      for (count =0; count < PM_FG_BATT_INFO_MAX_RETRY; count++) 
      {
        err_flag |= pm_fg_batt_info_get_vbatt_match_cp(pmic_device, &VBatt, &vbatt_match_cp);
        if (vbatt_match_cp == TRUE)
        {
          break;
        }
       }
    }
    if (vbatt_match_cp == FALSE)
    {
      DEBUG(( EFI_D_WARN, "PmicDxe:: %a vbatt_match_cp = %d \n\r",__FUNCTION__, vbatt_match_cp));
      err_flag |= PM_ERR_FLAG_PERIPHERAL_ERR;
    }
    else
    {
      VBatt =  (VBatt * PM_FG_VBATT_MAX)/PM_FG_VBATT_MAXUINT16;
      /* DEBUG(( EFI_D_WARN, "PmicDxe:: %a VBatt = %d mV VBatt = 0x%x \n\r",__FUNCTION__, VBatt, VBatt));*/
      *pVBat = VBatt;
    }
  }

  return err_flag;
}



pm_err_flag_type pm_fg_batt_info_set_esr_pull_down_cfg(uint32 pmic_device, pm_fg_batt_info_batt_esr_pull_down_cfg esr_cfg, uint8 esr_value)
{
  pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
  pm_fg_data_type *fg_batt_ptr  = pm_fg_get_data(pmic_device);
  if (NULL == fg_batt_ptr)
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (esr_cfg >= PM_FG_BATT_INFO_ESR_CFG_INVALID)
  {
    err_flag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type esr_reg = ADDRESS(esr_cfg);
    err_flag  = pm_comm_write_byte_mask(SLAVE_ID, esr_reg, (esr_value << esr_cfg ), (pm_register_data_type)(esr_value << esr_cfg ), 0);
  }

  return err_flag;
}

