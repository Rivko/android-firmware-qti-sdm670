/*! \file pm_pbs_client.c
*  \n
*  \brief This file contains the implementation of the public APIs for PBS Client module.
*  \n
*  \details Implementation file for PBS Client module. Each of the APIs checks for
*           access and then if necessary directs the call to Driver implementation.
*  \n  
*  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================
EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/21/17   ks      Added pm_pbs_oledb_sw_trigger api(CR-2004206)
05/15/16   aab     Moved temp monitoring function to app layer and updated pm_pbs_client_hw_trigger_enable() to support FE/RE Trigger
12/13/16   cs      added support for pbs triggered AFP
01/27/17   sm      Added support for pbs triggered AFP
03/30/15   aab     Updated to support MSM8998 target
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
06/16/13   kt      Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "pm_pbs_client.h"
#include "pm_comm.h"
#include "pmio_pbs_core.h"
#include "busywait.h"

#define DELAY_500MS 500000

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_pbs_client_enable(uint8 pmic_device_index, pm_pbs_client_type pbs_client_index, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type base_addr = 0;
    pm_register_address_type reg = 0;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_device_index);
    uint8 data = 0;
    uint8 mask = 0x80;

    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    if ((pbs_client_index >= pbs_client_ptr->num_of_peripherals) || (pbs_client_index >= PM_PBS_CLIENT_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_ptr->pbs_client_reg_table->peripheral_offset*pbs_client_index);
    reg = base_addr + pbs_client_ptr->pbs_client_reg_table->en_ctl1;
  
    if (enable == TRUE) 
    {
        /* Enable PBS Client module */
        data = 0xFF;
    }
    else
    {
        /* Disable PBS Client module */
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, reg, mask, data, 0);

    return err_flag; 
}

pm_err_flag_type pm_pbs_client_enable_status(uint8 pmic_device_index, pm_pbs_client_type pbs_client_index, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type base_addr = 0;
    pm_register_address_type reg = 0;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_device_index);
    uint8 mask = 0x80;
    uint8 data = 0;

    if(status == NULL)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }
    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    if ((pbs_client_index >= pbs_client_ptr->num_of_peripherals) || (pbs_client_index >= PM_PBS_CLIENT_INVALID))
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_ptr->pbs_client_reg_table->peripheral_offset*pbs_client_index);
    reg = base_addr + pbs_client_ptr->pbs_client_reg_table->en_ctl1;

    /* Read PBS Client module enable status */
    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, reg, mask, &data,0);

    *status = data ? TRUE : FALSE;

    return err_flag; 
}


pm_err_flag_type pm_pbs_client_sw_trigger(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger trigger, 
                                          uint8 *trigger_argument, uint8 *trigger_return_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 data = 0x80;
    pm_register_address_type base_addr = 0;
    pm_register_address_type trig_ctl = 0;
    pm_register_address_type scratch1 = 0;
    pm_register_address_type scratch2 = 0;

    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    if ((pbs_client_index >= pbs_client_ptr->num_of_peripherals) || (pbs_client_index >= PM_PBS_CLIENT_INVALID) || 
        (trigger >= PM_PBS_CLIENT_TRIGGER_INVALID))
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (trigger == PM_PBS_CLIENT_SW_TRIGGER)
    {
      data = 0x80;
    }
    else
    {
        data = 1<<trigger;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*pbs_client_ptr->pbs_client_reg_table->peripheral_offset);

    if (trigger_argument != NULL)
    {
        scratch1 = base_addr + pbs_client_ptr->pbs_client_reg_table->scratch1;

        // Write to SCRATCH1 register if any argument value to be used with the PBS sequence
        err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, scratch1, *trigger_argument, 0);
    }

    trig_ctl = base_addr + pbs_client_ptr->pbs_client_reg_table->trig_ctl;

    // Trigger the PBS sequence
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, trig_ctl, data, 0);

    if (trigger_return_value != NULL)
    {
        scratch2 = base_addr + pbs_client_ptr->pbs_client_reg_table->scratch2;

        // Read from SCRATCH2 register if any return value expected from the PBS sequence
        err_flag |= pm_comm_read_byte(pbs_client_ptr->comm_ptr->slave_id, scratch2, trigger_return_value, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pbs_oledb_sw_trigger(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger trigger, 
                                          uint8 *trigger_argument, uint8 *trigger_return_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 data = 0x80,mask = 0x01;
    pm_register_address_type base_addr = 0;
    pm_register_address_type trig_ctl  = 0;
    pm_register_address_type scratch1  = 0;
    pm_register_address_type scratch2  = 0;
    uint8 scratch2Data =0;
    uint8 waitcount =0;

    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((pbs_client_index >= pbs_client_ptr->num_of_peripherals) || (pbs_client_index >= PM_PBS_CLIENT_INVALID) || 
        (trigger >= PM_PBS_CLIENT_TRIGGER_INVALID) || (trigger_argument == NULL) || (trigger_return_value == NULL) )
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
    }
    if (trigger == PM_PBS_CLIENT_SW_TRIGGER)
    {
      data = 0x80;
    }
    else
    {
      data = 1<<trigger;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*pbs_client_ptr->pbs_client_reg_table->peripheral_offset);
    scratch2 = base_addr + pbs_client_ptr->pbs_client_reg_table->scratch2;
    scratch1 = base_addr + pbs_client_ptr->pbs_client_reg_table->scratch1;
    trig_ctl = base_addr + pbs_client_ptr->pbs_client_reg_table->trig_ctl;
    //Clear Ack register
    err_flag |= pm_comm_read_byte(pbs_client_ptr->comm_ptr->slave_id, scratch2, &scratch2Data, 0);
    if(scratch2Data == 0xFF)
    {
      err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, scratch2, 0x00, 0);   //Clear entire scratch2 register
    }
    else if(scratch2Data & mask)
    {
      err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, scratch2, mask, 0x00, 0);  //clear only Bit Zero
    }
    // Write to SCRATCH1 register if any argument value to be used with the PBS sequence
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, scratch1, *trigger_argument, 0);
    // Trigger the PBS sequence
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, trig_ctl, data, 0);
    //wait for acknowledgement
    do
    {
      err_flag |= pm_comm_read_byte(pbs_client_ptr->comm_ptr->slave_id, scratch2, trigger_return_value, 0);
      if(*trigger_return_value == 0xFF)
      {
         err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, scratch2, 0x00, 0);   //Clear entire scratch2 register
         err_flag |= PM_ERR_FLAG_DATA_VERIFY_ERR;
          break;
      }
      else if((*trigger_return_value & mask))
      {
        err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, scratch2, mask, 0x00, 0);  //clear only Bit Zero
        break;
      }
      waitcount++;
      busywait (DELAY_500MS);
    }while(waitcount < 4); // 2 seconds = 4* DELAY_500MS;
    
    if(waitcount == 4)  // 2 Sec timer expired
    {
      err_flag |= PM_ERR_FLAG_DATA_VERIFY_ERR;
    }
    //clear the PBS trigger
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, scratch1, 0x00, 0);
  
    return err_flag;
}

pm_err_flag_type
pm_pbs_client_hw_trigger_enable(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger hw_trigger, pm_pbs_client_trigger_edge_type trigger_edge, boolean enable)
{
    pm_err_flag_type err_flag                      = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type base_addr             = 0;
    pm_register_address_type en_ctl_addr           = 0;
    pm_register_address_type hw_trig_re_en_addr    = 0;
    pm_register_data_type hw_trig_re_fe_en_mask    = 0;
    pm_register_address_type hw_trig_fe_en_addr    = 0;
    pm_register_address_type trig_latch_clr_addr   = 0;
    pm_register_data_type    hw_trigger_data       = 0; 
    pm_register_data_type    hw_trigger_re_data    = 0; 
    pm_register_data_type    hw_trigger_fe_data    = 0; 

    pm_pbs_client_data_type *pbs_client_ptr        = pm_pbs_client_get_data(pmic_chip);
    if (pbs_client_ptr == NULL)
    {
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
  
  //Disabling HW trigger for PBS client 0 is not allowed as it used for PON, also failing if SW trigger is given as argument
    if ( (pbs_client_index >= pbs_client_ptr->num_of_peripherals) || 
         (pbs_client_index == PM_PBS_CLIENT_0)       || 
         (pbs_client_index >= PM_PBS_CLIENT_INVALID) || 
         (hw_trigger >= PM_PBS_CLIENT_SW_TRIGGER)    || 
         (trigger_edge >= PM_PBS_CLIENT_TRIGGER_EDGE_INVALID)) 
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    base_addr = pbs_client_ptr->pbs_client_reg_table->base_address + (pbs_client_index*pbs_client_ptr->pbs_client_reg_table->peripheral_offset);
    en_ctl_addr         =  base_addr + pbs_client_ptr->pbs_client_reg_table->en_ctl1;
    hw_trig_re_en_addr  =  base_addr + pbs_client_ptr->pbs_client_reg_table->hw_trig_re_en;
    hw_trig_fe_en_addr  =  base_addr + pbs_client_ptr->pbs_client_reg_table->hw_trig_fe_en;
    trig_latch_clr_addr =  base_addr + pbs_client_ptr->pbs_client_reg_table->trig_latched_clr;

    hw_trig_re_fe_en_mask = 0x01 << hw_trigger;
    if(enable == TRUE)  //Enable triggering
{
       hw_trigger_data = hw_trig_re_fe_en_mask;
  }
    else //Disable triggering
  {
       hw_trigger_data = 0x00;
}

    switch (trigger_edge)
  {
    case PM_PBS_CLIENT_TRIGGER_RISING_EDGE:
      hw_trigger_re_data = hw_trigger_data;
      hw_trigger_fe_data = 0x00;
      break;
    case PM_PBS_CLIENT_TRIGGER_FALLING_EDGE:
      hw_trigger_re_data = 0x00;
      hw_trigger_fe_data = hw_trigger_data;
      break;
    case PM_PBS_CLIENT_TRIGGER_DUAL_EDGE:
      hw_trigger_re_data = hw_trigger_data;
      hw_trigger_fe_data = hw_trigger_data;
      break;
    default:
    return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, en_ctl_addr, 0x00, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, hw_trig_re_en_addr, hw_trig_re_fe_en_mask, hw_trigger_re_data, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, hw_trig_fe_en_addr, hw_trig_re_fe_en_mask, hw_trigger_fe_data, 0);

    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, trig_latch_clr_addr, 0xFF, 0);
    err_flag |= pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, en_ctl_addr, 0x80, 0);

    //Enables/Disable RIF memory interface and the RIF Memory Access Mode. When enabled the PBS can run concurrently.
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, PMIO_PBS_CORE_MEM_INTF_CFG_RMSK, 0x80, 0); //0x7040 = 0x80 : Enables 
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, PMIO_PBS_CORE_MEM_INTF_CFG_RMSK, 0x00, 0); //0x7040 = 0x00 : Disable 

  return err_flag;
}


