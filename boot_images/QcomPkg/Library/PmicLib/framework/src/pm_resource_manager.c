/*! \file pm_resource_manager.c
 *  
 *  \brief  This file contains the initialization functions for PMIC resource manager.
 *  
 *  &copy; Copyright 2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ------------------------------------------------\
08/18/17   ks      Added support for OLEDB and LCDB
01/16/17   aab     Added bob driver support
11/03/15   al      Updated for rails
06/30/15   al      Adding Haptics and coincell support
07/10/15   al      Adding support for HP buck type
06/06/15   aab     Updated to support Hawker
04/14/15   al      Adding changes for Hawker support
02/15/15   al      Adding fix for reading bitmap
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture
03/05/12   aab     Added update to create the secondPMIC device only if number Oof PMIc devices on the target is 2
12/06/12   hw      Rearchitecturing module driver to peripheral driver
10/05/12   hw      Remove Interrupt and Application Module
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_target_information.h"

#include "pm_comm.h"
#include "DALSys.h"

/* PMIC Driver includes */
#include "pm_smps_driver.h"
#include "pm_ldo_driver.h"
#include "pm_vs_driver.h"
#include "pm_bob_driver.h"
#include "pm_clk_driver.h"
#include "pm_rtc_driver.h"
#include "pm_gpio_driver.h"
#include "pm_rgb_driver.h"
#include "pm_wled_driver.h"
#include "pm_lpg_driver.h"
#include "pm_vib_driver.h"
#include "pm_haptics_driver.h"
#include "pm_pbs_client_driver.h"
#include "pm_schg_driver.h"
#include "pm_fg_driver.h"
#include "pm_lab_driver.h"
#include "pm_ibb_driver.h"
#include "pm_oledb_driver.h"
#include "pm_lcdb_driver.h"

#include "hw_module_type.h"
#include "device_info.h"
#include "pm_config_target.h"
#include "pm_coincell_driver.h"


/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*! \brief This function returns the parameter specified pmic resource.
 *
 *  \param[in] comm_ptr The comm object of the pmic resource.
 *
 *  \param[in] peripheral_info The peripheral info of the pmic resource.
 */
static void pm_resource_manager_create_pmic_resource
       (pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);



void pm_resource_manager_init(void)
{
  peripheral_info_type peripheral_info;
  uint32 pmic_index = 0;
  uint32 pmic_index_bitmap = 0;
  uint32 sid_index = 0;
  uint32 peripheral_index = 0;
  pm_comm_info_type *comm_ptr = NULL;

  uint8 *bitmap_ptr;
  uint8  bit_index;      /* index into 8 bits bitmap value */
  uint32 bitmap_index = 0;
  uint32  array_index; /* index into bit map array */
  boolean  bit_set = FALSE;
  pm_model_type pmic_model = PMIC_IS_UNKNOWN;

   
  uint8  num_slaves_per_pmic = pm_get_num_slaves_per_pmic();
  memset(&peripheral_info, 0, sizeof(peripheral_info_type));

  bitmap_ptr = pm_target_information_get_specific_info(PM_PROP_PERIPH_BITMAP);
  CORE_VERIFY_PTR(bitmap_ptr);

  for(; pmic_index < PM_MAX_NUM_PMICS; pmic_index++)
  {
    pmic_model = pm_get_pmic_model(pmic_index);
    if(pmic_model != PMIC_IS_UNKNOWN)
    {
      if(pmic_model == PMIC_IS_PMF2432)
      {
        pmic_index_bitmap = 0;
      }
      else if(pmic_model == PMIC_IS_PM2433)
      {
        pmic_index_bitmap = 1;
      }
      else
      {
       pmic_index_bitmap = pmic_index;
      }

      //for each slave id
      for(sid_index = 0; sid_index < num_slaves_per_pmic; sid_index++)
      {
        comm_ptr = pm_comm_get_comm_info(pmic_index, sid_index);

        CORE_VERIFY_PTR(comm_ptr);

        peripheral_index = 0;

        for(array_index = 0; array_index < PM_MAX_BITMAP_ENTRIES; array_index++)
        {
          //reading the bitmap element from the bitmap_array[pmic_index][slave_per_pmic][bitmap_entries]
          bitmap_index = bitmap_ptr[(pmic_index_bitmap * (num_slaves_per_pmic * PM_MAX_BITMAP_ENTRIES)) + (sid_index * PM_MAX_BITMAP_ENTRIES) + array_index];
          

          for(bit_index = 0; bit_index < 8; bit_index++)
          {
            /*look for set bit*/
            bit_set = (bitmap_index & (0x1 << bit_index)) ? TRUE : FALSE;

            if(bit_set == TRUE)
            {
              //read revision information here
              peripheral_info.base_address = peripheral_index * 0x0100;
              if(pm_target_information_read_peripheral_rev(comm_ptr, &peripheral_info) == PM_ERR_FLAG_SUCCESS)
              {
                pm_resource_manager_create_pmic_resource(comm_ptr, &peripheral_info, pmic_index);
              }
            }
            peripheral_index++;
          } // bit_index
        } // array_index
      }
    }
  } /* for pmic_index */
} 






void pm_resource_manager_create_pmic_resource(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index)
{    
    switch((pm_hw_module_type)peripheral_info->peripheral_type)
    {
      case PM_HW_MODULE_FTS:
      case PM_HW_MODULE_HF_BUCK:
      case PM_HW_MODULE_HP_BUCK:
	  case PM_HW_MODULE_ULT_BUCK:
            pm_smps_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_LDO:
        case PM_HW_MODULE_ULT_LDO:
            pm_ldo_driver_init(comm_ptr, peripheral_info, pmic_index); 
            break;
        case PM_HW_MODULE_VS:
            pm_vs_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_BOB:
            pm_bob_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_CLOCK:
            pm_clk_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_RTC:
            pm_rtc_driver_init(comm_ptr, peripheral_info, pmic_index);
            break;
        case PM_HW_MODULE_PBS:
            pm_pbs_client_driver_init( comm_ptr, peripheral_info, pmic_index );
            break;
        case PM_HW_MODULE_GPIO:
            pm_gpio_driver_init( comm_ptr, peripheral_info, pmic_index );             
            break;
        case PM_HW_MODULE_LPG:
            pm_lpg_driver_init( comm_ptr, peripheral_info, pmic_index );
            break;
        case PM_HW_MODULE_RGB:
            pm_rgb_driver_init( comm_ptr, peripheral_info, pmic_index );
            break;
        case PM_HW_MODULE_WLED:
            pm_wled_driver_init( comm_ptr , peripheral_info,  pmic_index);
            break;
        case PM_HW_MODULE_HAPTICS:
            switch(peripheral_info->peripheral_subtype)
            {
             case PM_HW_MODULE_HAPTICS_VIB:
               pm_vib_driver_init(comm_ptr, peripheral_info, pmic_index);
             break;
             case PM_HW_MODULE_HAPTICS_HAPTICS:
                pm_haptics_driver_init(comm_ptr, peripheral_info, pmic_index);
             break;
             default:
              break;
             }
            break;
        case PM_HW_MODULE_CHARGER:
          switch(peripheral_info->peripheral_subtype)
          {
            case PM_HW_MODULE_CHARGER_COINCELL:
              pm_coincell_driver_init(comm_ptr, peripheral_info, pmic_index );
              break;
            default:
              pm_schg_driver_init( comm_ptr, peripheral_info, pmic_index );
              break;
          }
           break;
       case PM_HW_MODULE_FG:
           pm_fg_driver_init( comm_ptr, peripheral_info, pmic_index );
           break;
       case PM_HW_MODULE_IBB:
           pm_ibb_driver_init(comm_ptr, peripheral_info, pmic_index);
           break;
       case PM_HW_MODULE_LAB:
           pm_lab_driver_init(comm_ptr, peripheral_info, pmic_index);
           break;
       case PM_HW_MODULE_OLEDB:
            pm_oledb_driver_init(comm_ptr, peripheral_info);
            break;
        case PM_HW_MODULE_LCDB:
            pm_lcdb_driver_init(comm_ptr, peripheral_info);
            break;

        default:
            break;
    }
}