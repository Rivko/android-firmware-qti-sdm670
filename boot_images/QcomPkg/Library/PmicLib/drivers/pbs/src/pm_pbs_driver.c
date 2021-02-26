/*! \file  pm_pbs_driver.c 
 *  
 *  \brief  This file contains the pmic PBS implementation
 *  \details  This file contains the pm_pbs_ram_image_load & pm_pbs_ram_image_verify API
 *  implementation for SBl1 boot and also implementation for  other helper functions
 *  used in PBS driver.
 *  
 *  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
07/31/2016   aab     Removed pbs ram load/verify delay
07/31/2016   aab     Updated pbs ram load/verify delay to 5us
12/04/2015   aab     Updated pm_pbs_ram_image_verify() 
07/16/2014   akm     Comm change Updates
06/02/2014   aab     Updated to use pm_clk_busy_wait() API
12/18/2013   aab     Updated pm_pbs_ram_image_load() and  pm_pbs_ram_image_verify() 
03/20/2013   kt      Moving and renaming the file to pm_pbs_driver.c 
                     from pm_pbs.c and adding comments.
10/19/2012   umr     PBS Core Driver.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_driver.h"
#include "pm_pbs_info.h"
#include "pm_pbs.h"
#include "pm_utils.h"


/*=========================================================================
      Function Definitions
==========================================================================*/
/** 
 * @name pm_pbs_ram_image_verify 
 *  
 * @brief Please refer to pm_pbs_driver.h file for info 
 *        regarding this function.
 */
pm_err_flag_type
pm_pbs_ram_image_verify(uint32 slave_id, pm_pbs_ram_data_type *data, unsigned size)
{
  int i;
  pm_err_flag_type err = PM_ERR_FLAG_SUCCESS;
  uint32 read_val, data_val;
  uint8 pmic_index;
  pm_pbs_info_data_type** pm_pbs_temp_data = NULL;
  pm_pbs_info_data_type *pm_pbs_data = NULL;

  /* Retrieve PBS info config data */
  pm_pbs_temp_data = (pm_pbs_info_data_type**)pm_target_information_get_specific_info(PM_PROP_PBS_INFO);
  if(NULL == pm_pbs_temp_data)
    {
      return PM_ERR_FLAG_INVALID_PARAMETER;
  	}
  pmic_index = (uint8)(slave_id / 2); 
  pm_pbs_data = pm_pbs_temp_data[pmic_index];

  /* Configuring read access in burst mode to PBS RAM */
  err = pm_pbs_config_access(slave_id, pm_pbs_data->pbs_ram_start_addr, PM_PBS_ACCESS_READ_BURST);
  if(err != PM_ERR_FLAG_SUCCESS) return err; //Stop on first error

  /* Burst Read PBS sequence and verify */
  for (i = 0; i < size; i++)
  {
    err |= pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*) &read_val, 1);
    if(err != PM_ERR_FLAG_SUCCESS) return err; //Stop on first error
    data_val = *((uint32 *)&data[i]);
    if (data_val != read_val)
    {
      return PM_ERR_FLAG_DATA_VERIFY_ERR;
    }
  }

  return err;
}

/** 
 * @name pm_pbs_ram_image_load 
 *  
 * @brief Please refer to pm_pbs_driver.h file for info 
 *        regarding this function.
 */
pm_err_flag_type
pm_pbs_ram_image_load(uint32 slave_id, pm_pbs_ram_data_type *data, unsigned size)
{
  int i;
  pm_err_flag_type err = PM_ERR_FLAG_SUCCESS;

  /* Burst Write PBS sequence */
  for (i = 0; i < size; i++)
  {
    err = pm_comm_write_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_WR_DATA0_ADDR, 4, (uint8 *)&data[i], 1);
    if(err != PM_ERR_FLAG_SUCCESS) return err; //Stop on first error
  }

  //Verify
  err = pm_pbs_ram_image_verify(slave_id, data, size);
  if(err != PM_ERR_FLAG_SUCCESS) return err; //Stop on first error

  return err;
}

/** 
 * @name pm_pbs_enable_access 
 *  
 * @brief Please refer to pm_pbs_driver.h file for info 
 *        regarding this function.
 */
pm_err_flag_type 
pm_pbs_enable_access(uint32 slave_id) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  /* Configuring to enable PBS ROM/RAM access */
  err_flag = pm_comm_write_byte(slave_id, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, 0x80, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}

/** 
 * @name pm_pbs_disable_access 
 *  
 * @brief Please refer to pm_pbs_driver.h file for info 
 *        regarding this function.
 */
pm_err_flag_type 
pm_pbs_disable_access(uint32 slave_id) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  /* Configuring to disable PBS ROM/RAM access */
  err_flag |= pm_comm_write_byte(slave_id, PMIO_PBS_CORE_MEM_INTF_CFG_ADDR, 0x00, 1);
  err_flag |= pm_comm_write_byte(slave_id, PMIO_PBS_CORE_MEM_INTF_CTL_ADDR, 0x00, 1);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}

/** 
 * @name pm_pbs_config_access 
 *  
 * @brief Please refer to pm_pbs_driver.h file for info 
 *        regarding this function.
 */
pm_err_flag_type 
pm_pbs_config_access(uint32 slave_id, uint16 addr, pm_pbs_access_type access_type) 
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;
  uint8 access_val = 0;

  switch (access_type)
  {
    case PM_PBS_ACCESS_READ_SINGLE:
      access_val = 0x00;
      break;

    case PM_PBS_ACCESS_WRITE_SINGLE:
      access_val = 0x40;
      break;

    case PM_PBS_ACCESS_READ_BURST:
      access_val = 0x80;
      break;

    case PM_PBS_ACCESS_WRITE_BURST:
      access_val = 0xC0;
      break;

    default:
      return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  /* Configuring type of access to PBS ROM/RAM */
  err_flag = pm_comm_write_byte(slave_id, PMIO_PBS_CORE_MEM_INTF_CTL_ADDR, access_val, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Configuring the base address */
  err_flag = pm_comm_write_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_ADDR_LSB_ADDR, 2, (uint8*)&addr, 1);

  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  return err_flag;
}
