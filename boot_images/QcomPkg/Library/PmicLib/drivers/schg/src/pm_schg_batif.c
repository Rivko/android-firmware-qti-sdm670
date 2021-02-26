/*! \file
*  
*  \brief  pm_schg_bat_if.c driver implementation.
*  \details charger driver implementation.
*  &copy; Copyright 2016-2017 QUALCOMM Technologies, Inc All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/16   sm      Added pm_schg_batif_cfg_batt_missing_src()
02/16/16   va      Adding required API for enabling charger
01/14/16   al      Initial revision
========================================================================== */

/*===========================================================================

          INCLUDE FILES

===========================================================================*/

#include "pm_schg_batif.h"
#include "pm_schg_driver.h"
#include "pm_resource_manager.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (schg_ptr->schg_register->batif_register->base_address + schg_ptr->schg_register->batif_register->x)
#define SLAVE_ID   (schg_ptr->comm_ptr->slave_id)

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/


/*writes to secured acess register to unlock pmic write operation*/
pm_err_flag_type pm_schg_batif_unlock_perph_write(pm_schg_data_type *schg_ptr)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

    if (NULL == schg_ptr)
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


pm_err_flag_type pm_schg_batif_enable_shipmode(uint32 device_index)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
      pm_register_address_type shipmode = ADDRESS(ship_mode);

      errFlag = pm_schg_batif_unlock_perph_write(schg_ptr);
      errFlag = pm_comm_write_byte(SLAVE_ID, shipmode, 0x01, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_schg_batif_irq_enable(uint32 device_index, pm_schg_batif_irq_type irq, boolean enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type irq_reg;
    pm_register_data_type data = 1 << irq;

    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SCHG_BATIF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
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

        errFlag = pm_comm_write_byte(SLAVE_ID, irq_reg, data, 0);
    }
    return errFlag;
}

pm_err_flag_type pm_schg_batif_irq_clear(uint32  device_index, pm_schg_batif_irq_type irq)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data = 1 << irq;
    pm_register_address_type int_latched_clr;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SCHG_BATIF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        int_latched_clr = ADDRESS(int_latched_clr);
        errFlag = pm_comm_write_byte(SLAVE_ID, int_latched_clr, data, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_schg_batif_irq_set_trigger(uint32 device_index, pm_schg_batif_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    uint8 mask = 1 << irq;
    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;

    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SCHG_BATIF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
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
        errFlag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
        errFlag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
        errFlag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_schg_batif_irq_status(uint32 device_index, pm_schg_batif_irq_type irq, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data;
    uint8 mask = 1 << irq;
    pm_register_address_type int_sts;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_SCHG_BATIF_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
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

        errFlag = pm_comm_read_byte_mask(SLAVE_ID, int_sts, mask, &data, 0);
        *status = data ? TRUE : FALSE;
    }

    return errFlag;
}


/* This API enable low battery detect*/
pm_err_flag_type pm_schg_batif_enable_low_batt_detect(uint32 device_index, boolean enable)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
      errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type low_batt_cfg_reg = ADDRESS(low_batt_detect_en_cfg);

    errFlag = pm_schg_batif_unlock_perph_write(schg_ptr);
    errFlag = pm_comm_write_byte_mask(SLAVE_ID, low_batt_cfg_reg, 0x01, (pm_register_data_type) enable, 0);
  }

  return errFlag;
  }

/* This API gets low battery detect cfg */
pm_err_flag_type pm_schg_batif_get_low_batt_detect_cfg(uint32 device_index, boolean *enable)
{
  pm_err_flag_type errFlag     = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == enable)
  {
    errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type low_batt_cfg_reg = ADDRESS(low_batt_detect_en_cfg);

    errFlag = pm_schg_batif_unlock_perph_write(schg_ptr);
    errFlag = pm_comm_read_byte_mask(SLAVE_ID, low_batt_cfg_reg, 0x01, enable, 0);
  }

  return errFlag;
  }


/* This API enable low battery detect*/
pm_err_flag_type pm_schg_batif_set_low_batt_detect_threshold(uint32 device_index, pm_schg_batif_low_battery_threshold lbt_cfg)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

  if (NULL == schg_ptr)
  {
    errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (PM_SCHG_BATIF_LOW_BATTERY_THRESH_INVALID >= lbt_cfg)
  {
    errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type lbt_cfg_reg = ADDRESS(low_batt_detect_en_cfg);

    errFlag = pm_schg_batif_unlock_perph_write(schg_ptr);
    errFlag = pm_comm_write_byte_mask(SLAVE_ID, lbt_cfg_reg , 0x0F, (pm_register_data_type)lbt_cfg, 0);
  }

  return errFlag;
}


/* This API enable low battery detect*/
pm_err_flag_type pm_schg_batif_get_low_batt_detect_threshold(uint32 device_index, pm_schg_batif_low_battery_threshold *lbt_cfg)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  pm_register_data_type data = 0;

  if (NULL == schg_ptr)
  {
    errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else if (NULL == lbt_cfg)
  {
    errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
  }
  else
  {
    pm_register_address_type lbt_cfg_reg = ADDRESS(low_batt_threshold_cfg);

    errFlag = pm_schg_batif_unlock_perph_write(schg_ptr);
    errFlag = pm_comm_read_byte_mask(SLAVE_ID, lbt_cfg_reg , 0x0F, &data, 0);
    *lbt_cfg = (pm_schg_batif_low_battery_threshold )data;
  }

  return errFlag;
}

pm_err_flag_type pm_schg_batif_cfg_batt_missing_src(uint32 device_index, pm_schg_batif_bat_miss_src_cfg_type bat_miss_cfg, boolean enable)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
  pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
  uint8 mask = (1 << bat_miss_cfg);

  if (NULL == schg_ptr)
  {
    errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    pm_register_address_type batt_miss_cfg_reg = ADDRESS(bat_miss_src_cfg);

    errFlag = pm_schg_batif_unlock_perph_write(schg_ptr);
    errFlag = pm_comm_write_byte_mask(SLAVE_ID, batt_miss_cfg_reg , mask, (pm_register_data_type)enable, 0);
  }

  return errFlag;
}
