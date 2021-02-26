/*! \file pm_comm.c
*  
*  \brief This file contains processor specific initialization functions.
*         This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ------------------------------------------------
10/12/15   aab    Updated pm_comm_info_init()
07/16/14   akm    Multiple PMIC's configuration support ( >2 PMICS)

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_resource_manager.h"
#include "pm_target_information.h"
#include "pm_comm.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_version.h"
#include "device_info.h"


/* PMIC Comm Info, Each PMIC device has 1 or more slave ids */
static pm_comm_info_type *pm_comm_info = NULL ; 
static boolean pm_comm_info_initialized = FALSE;

#define PRIMARY_SLAVE_IDX    0

void pm_comm_info_init()
{
    uint8  pmic_counter = 0;
    uint32 total_slave_count = 0; //counter for total num of pmic slaves on the target
    uint32 slave_id_counter = 0;  //counter for num of slaves ON A PMIC

    uint8  num_slaves_per_pmic = pm_get_num_slaves_per_pmic();
    
    if (pm_comm_info_initialized == TRUE)
    {
       return;
    }     
    
    pm_malloc(num_slaves_per_pmic*PM_MAX_NUM_PMICS * sizeof(pm_comm_info_type), (void**)&pm_comm_info); 
        
    for ( ; pmic_counter < PM_MAX_NUM_PMICS ; pmic_counter++ )
    {
         
        for(slave_id_counter = 0; 
            slave_id_counter < num_slaves_per_pmic ; 
            slave_id_counter++,total_slave_count++)
        {

          pm_comm_info[total_slave_count].mCommType = SPMI_1_0Type;
          pm_comm_info[total_slave_count].pmic_index = pmic_counter;
          pm_get_slave_id(pmic_counter , slave_id_counter, 
          	                  &pm_comm_info[total_slave_count].slave_id );
        }

        
    }
   
    pm_comm_info_initialized = TRUE;
   
}


pm_comm_info_type* pm_comm_get_comm_info(uint32 pmic_index, uint32 slave_id_index)
{
    uint32 comm_info_idx = 0;
    uint8  pm_num_slaves_per_pmic = pm_get_num_slaves_per_pmic();
    comm_info_idx = (pmic_index * pm_num_slaves_per_pmic) + slave_id_index;
    return &pm_comm_info[comm_info_idx];
}


