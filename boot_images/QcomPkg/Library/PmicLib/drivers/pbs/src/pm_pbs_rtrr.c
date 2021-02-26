/*! \file  pm_pbs_rtrr.c 
 *  
 *  \brief  This file contains the pmic PBS implementation
 *  \details  This file contains the pm_pbs_rtrr dump during the reset
 *  
 *  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/drivers/pbs/src/pm_pbs_rtrr.c#1 $ 
$DateTime: 2017/09/18 09:26:13 $  $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pbs_driver.h"
#include "pm_pbs_rtrr.h"
#include "pm_utils.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"

#define PM_PBS_RAM_START_ADDR 0x400
#define PM_PBS_RTRR_START_ADDR 0x20
#define PM_PBS_RTRR_DUMP_SIZE_DWORDS 160

/*=========================================================================
      Function Definitions
==========================================================================*/
pm_err_flag_type
pm_pbs_rtrr_ram_dump(uint32 slave_id, pm_pbs_rtrr_dump_data_type *pm_pbs_rtrr_data)
{
	pm_err_flag_type err_flag    = PM_ERR_FLAG_SUCCESS;
	uint32 read_data             = 0;
	uint8 read_counter           = 0;
	uint16           rtrr_dataset_start_addr = 0;
	uint16           pbs_ram_rtrr_dataset_start_addr_placeholder = 0;

	
	/* Configuring the start address for reading RTRR info.	 */
	pbs_ram_rtrr_dataset_start_addr_placeholder = 0x400 + PM_PBS_RTRR_START_ADDR ;
	err_flag = pm_pbs_config_access(slave_id, pbs_ram_rtrr_dataset_start_addr_placeholder, PM_PBS_ACCESS_READ_BURST);
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
	   return err_flag;
	}

	/* Configuring to enable PBS core access for ROM reads */
	err_flag = pm_pbs_enable_access(slave_id);
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
    	return err_flag;
	}
	
	/* Reading the Start address of the RTRR data set in PBS RAM and storing it in temp variable */
	err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_data, 1);
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
		return err_flag;
	}
	rtrr_dataset_start_addr = (uint32)(read_data & 0x0000FFFF);
	/* Configuring to disable PBS core read access */
	err_flag |= pm_pbs_disable_access(slave_id);

	read_data = 0 ;

	err_flag = pm_pbs_config_access(slave_id, rtrr_dataset_start_addr , PM_PBS_ACCESS_READ_BURST);
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
		return err_flag;
	}

	/* Configuring to enable PBS core access for reads */
	err_flag = pm_pbs_enable_access(slave_id);
	if (err_flag != PM_ERR_FLAG_SUCCESS)
	{
    	return err_flag;
	}

	/* Reading the thre requested bytes from RTRR read_counter < PM_PBS_RTRR_DUMP_SIZE_DWORDS */
	for (read_counter = 0; read_counter < PM_PBS_RTRR_DUMP_SIZE_DWORDS  ; read_counter++)
	{
		err_flag = pm_comm_read_byte_array(slave_id, PMIO_PBS_CORE_MEM_INTF_RD_DATA0_ADDR, 4, (uint8*)&read_data, 1);
		if (err_flag != PM_ERR_FLAG_SUCCESS)
		{
		  return err_flag;
		}

		if(read_counter == 0 )
			{
			    pm_pbs_rtrr_data->version = (uint32)read_data;
			}

		if ((uint32)read_data == 0x00000500)
			{
			    break;
			}

		pm_pbs_rtrr_data->data[read_counter] = (uint32)read_data;
		
	}

	/* Configuring to disable PBS core read access */
	err_flag |= pm_pbs_disable_access(slave_id);

	return err_flag;

}

