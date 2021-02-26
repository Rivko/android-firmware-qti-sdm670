#ifndef __PM_PBS_CLIENT_DRIVER_H__
#define __PM_PBS_CLIENT_DRIVER_H__

/*! \file pm_pbs_client_driver.h 
*  \n 
*  \brief PBS Client driver type definitions and init function prototype.   
*  \n  
*  \n &copy; Copyright 2014-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/05/14   akm     Updated to the latest PMIC driver architecture   
06/13/13   kt      Initial version
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef struct
{
    pm_register_address_type base_address;   
    pm_register_address_type peripheral_offset;
    pm_register_address_type hw_trig_re_en;     //0x40
    pm_register_address_type hw_trig_fe_en;     //0x41
    pm_register_address_type trig_ctl;          //0x42
    pm_register_address_type en_ctl1;           //0x46
    pm_register_address_type trig_latched_clr;  //0x48
    pm_register_address_type scratch1;          //0x50
    pm_register_address_type scratch2;          //0x51
}pm_pbs_client_register_info_type;


typedef struct
{
	pm_comm_info_type                 *comm_ptr;
    pm_pbs_client_register_info_type*  pbs_client_reg_table;
    uint8                              num_of_peripherals;
}pm_pbs_client_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_pbs_client_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_pbs_client_data_type* pm_pbs_client_get_data(uint8 pmic_index);

#endif /* __PM_PBS_CLIENT_DRIVER_H__ */

