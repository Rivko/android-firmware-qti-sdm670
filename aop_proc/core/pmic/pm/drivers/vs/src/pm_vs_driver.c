/*! \file pm_vs_driver.c 
*  \n
*  \brief This file contains VS peripheral driver initialization during which the driver
*         driver data is stored.
*  \n
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/drivers/vs/src/pm_vs_driver.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/13   aks     Code Refactoring: Removing PMIC_SubRsc from pwr algs  
04/12/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_vs_driver.h"
#include "pm_target_information.h"
#include "device_info.h"
#include "pm_debug.h"
/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/

/* Static global variable to store the VS driver data */
static pm_vs_data_type *pm_vs_data_arr[PM_MAX_NUM_PMICS];

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/
__attribute__((section("pm_cram_reclaim_pool")))
void pm_vs_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{
    pm_vs_data_type *vs_ptr = NULL;
    uint16 vs_index = 0;
    pm_register_address_type base_address = 0;
    pm_register_address_type periph_offset = 0;
    pm_debug_info_type *pmic_debug = pm_get_debug_info();
    CORE_VERIFY_PTR(pmic_debug);
    pmic_debug->pm_vs_data_arr = pm_vs_data_arr;
    
    vs_ptr = pm_vs_data_arr[pmic_index];
    
    if (vs_ptr == NULL)
    {
        pm_malloc( sizeof(pm_vs_data_type), (void**)&vs_ptr);
                                                           
        /* Assign Comm ptr */
        vs_ptr->comm_ptr = comm_ptr;

        /* VS Register Info - Obtaining Data through dal config */
        vs_ptr->pm_pwr_data.pwr_reg_table = (pm_pwr_register_info_type*)pm_target_information_get_common_info(PM_PROP_VS_REG);

        CORE_VERIFY_PTR(vs_ptr->pm_pwr_data.pwr_reg_table);

       
        vs_ptr->pm_pwr_data.num_of_peripherals = pm_target_information_get_periph_count_info(PM_PROP_VS_NUM, pmic_index);

        /* Num of peripherals cannot be 0 if this driver init gets called */
        CORE_VERIFY(vs_ptr->pm_pwr_data.num_of_peripherals != 0);

        /* VS pwr rail specific info pointer malloc to save all the peripheral's base address, Type, Range and Vset */
        pm_malloc(sizeof(pm_pwr_specific_info_type)*(vs_ptr->pm_pwr_data.num_of_peripherals), (void**)&(vs_ptr->pm_pwr_data.pwr_specific_info));

        CORE_VERIFY_PTR(vs_ptr->pm_pwr_data.pwr_specific_info);

        /* Save first VS peripheral's base address */
        vs_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address = peripheral_info->base_address;

        pm_vs_data_arr[pmic_index] = vs_ptr;
    }
    
    if (vs_ptr != NULL)
    {
        base_address = vs_ptr->pm_pwr_data.pwr_specific_info[0].periph_base_address;
        periph_offset = vs_ptr->pm_pwr_data.pwr_reg_table->peripheral_offset;

        /* Peripheral Baseaddress should be >= first peripheral's base addr */
        CORE_VERIFY(peripheral_info->base_address >= base_address);

        /* Calculate VS peripheral index */
        vs_index = ((peripheral_info->base_address - base_address)/periph_offset);

        /* Peripheral Index should be less than number of peripherals */
        CORE_VERIFY(vs_index < (vs_ptr->pm_pwr_data.num_of_peripherals));

        /* Save VS's Peripheral Type value */
        vs_ptr->pm_pwr_data.pwr_specific_info[vs_index].periph_type = peripheral_info->peripheral_type;

        /* Save each VS peripheral's base address */
        vs_ptr->pm_pwr_data.pwr_specific_info[vs_index].periph_base_address = peripheral_info->base_address;
    }
}

pm_vs_data_type* pm_vs_get_data(uint8 pmic_index)
{
    if(pmic_index < PM_MAX_NUM_PMICS)
    {
        return pm_vs_data_arr[pmic_index];
    }

    return NULL;
}

__attribute__((section("pm_cram_reclaim_pool")))
uint8 pm_vs_get_num_peripherals(uint8 pmic_index)
{
  if((pmic_index < PM_MAX_NUM_PMICS) && 
  	  (pm_vs_data_arr[pmic_index] !=NULL))
  {
      return pm_vs_data_arr[pmic_index]->pm_pwr_data.num_of_peripherals;
  }

  return NULL;
} 



