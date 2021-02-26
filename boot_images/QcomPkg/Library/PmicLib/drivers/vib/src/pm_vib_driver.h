#ifndef PM_VIB_DRIVER__H
#define PM_VIB_DRIVER__H

/*! \file
*  \n
*  \brief  pm_vib_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting vib peripheral 
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
05/08/14   dm      Changes for 8994 Haptics
03/31/14   akm     Updated to the latest PMIC driver architecture   
03/04/13   aab     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/

#include "pm_target_information.h"

/*===========================================================================

                     VIB TYPES AND STRUCTURES 

===========================================================================*/
typedef struct
{                  
  pm_register_address_type base_address;  
  pm_register_address_type peripheral_offset;
  pm_register_address_type en_ctl;        //0x046
  pm_register_address_type vmax_cfg;      //0x051
  pm_register_address_type play;          //0x070
} pm_vib_register_info_type;

typedef struct
{
    uint16 MinVoltage;
    uint16 MaxVoltage;
} pm_vib_specific_data_type;

typedef struct
{
	pm_comm_info_type           *comm_ptr;
    uint32                      num_of_peripherals;
    pm_vib_register_info_type  *vib_reg_table;
    pm_vib_specific_data_type  *vib_data;
} pm_vib_data_type;




/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_vib_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_vib_data_type* pm_vib_get_data(uint8 pmic_index);

uint8 pm_vib_get_num_peripherals(uint8 pmic_index);

#endif // PM_VIB_DRIVER__H

