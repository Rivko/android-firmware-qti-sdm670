#ifndef PM_RGB_DRIVER__H
#define PM_RGB_DRIVER__H

/*! \file pm_rgb_driver.h 
*  \n
*  \brief  This file contains functions prototypes and variable/type/constant
*         declarations for supporting rgb peripheral.
*  
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
03/04/13   aab     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/


#include "pm_target_information.h"
#include "pm_comm.h"

/*===========================================================================

                     RGB TYPES AND STRUCTURES 

===========================================================================*/
typedef struct
{
  pm_register_address_type base_address;  
  pm_register_address_type peripheral_offset;
  pm_register_address_type src_sel;             //0x045
  pm_register_address_type en_ctl;              //0x046
  pm_register_address_type en_ctl_atc;          //0x047
} pm_rgb_register_info_type;


typedef struct
{
    pm_comm_info_type          *comm_ptr;
    pm_rgb_register_info_type  *rgb_reg_table;
    uint8                       num_of_peripherals;
} pm_rgb_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_rgb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_rgb_data_type* pm_rgb_get_data(uint8 pmic_index);

uint8 pm_rgb_get_num_peripherals(uint8 pmic_index);

#endif // PM_RGB_DRIVER__H

