#ifndef PM_HAPTICS_DRIVER__H
#define PM_HAPTICS_DRIVER__H

/*! \file
*  \n
*  \brief  pm_haptics_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting haptics peripheral 
*  
*  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/17   czq      Supported for WF source selection.

========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/

#include "pm_target_information.h"
#include "pm_haptics.h"
#include "pm_comm.h"

/*===========================================================================

                     HAPTICS TYPES AND STRUCTURES 

===========================================================================*/
typedef struct
{                  
  pm_register_address_type base_address;        //0x1c000
  pm_register_address_type status1;             //0x00A
  pm_register_address_type auto_res_lo;         //0x00B
  pm_register_address_type auto_res_hi;         //0x00C
  pm_register_address_type en_ctl1;             //0x046
  pm_register_address_type en_ctl2;             //0x048
  pm_register_address_type en_ctl3;             //0x04A
  pm_register_address_type actuator_type;       //0x04C
  pm_register_address_type lra_res_type;        //0x04D
  pm_register_address_type sel;                 //0x04E
  pm_register_address_type auto_res_cfg;        //0x04F                                                
  pm_register_address_type slew_rate;           //0x050
  pm_register_address_type vmax_cfg;            //0x051
  pm_register_address_type rate_cfg1;           //0x054
  pm_register_address_type rate_cfg2;           //0x055
  pm_register_address_type internal_pwm;        //0x056
  pm_register_address_type external_pwm;        //0x057
  pm_register_address_type pwm_cap_sel;         //0x058
  pm_register_address_type brake_pattern;       //0x05C
  pm_register_address_type wf_repeat;           //0x05E
  pm_register_address_type wf_s0;               //0x060
  pm_register_address_type play;                //0x070
} pm_haptics_register_info_type;

typedef struct
{
    pm_comm_info_type              *comm_ptr;
    uint32                         num_of_peripherals;
    pm_haptics_register_info_type  *haptics_reg_table;
} pm_haptics_data_type;


/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_haptics_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint32 pmic_index);

pm_haptics_data_type* pm_haptics_get_data(uint32 pmic_index);

uint32 pm_haptics_get_num_peripherals(uint32 pmic_index);

#endif // PM_HAPTICS_DRIVER__H

