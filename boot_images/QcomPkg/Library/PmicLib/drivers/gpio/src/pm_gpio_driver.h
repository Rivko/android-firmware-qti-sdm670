#ifndef PM_GPIO_DRIVER__H
#define PM_GPIO_DRIVER__H

/*! \file
*  \n
*  \brief  pm_gpio_driver.h 
*  \details  This file contains functions prototypes and variable/type/constant
 *  declarations for supporting GPIO pin services for the Qualcomm
 *  PMIC chip set.
 *  \n &copy; Copyright 2010-2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
03/25/13   aab     Created
========================================================================== */

/*===========================================================================
						INCLUDE FILES
===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"

/*===========================================================================

                     TYPE DEFINITIONS 

===========================================================================*/
typedef struct pm_gpio_register_info_type
{
    pm_register_address_type    base_address;  
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    status;                //0x08
    pm_register_address_type    int_rt_sts;            //0x10
    pm_register_address_type    int_set_type;          //0x11
    pm_register_address_type    int_polarity_high;     //0x12
    pm_register_address_type    int_polarity_low;      //0x13
    pm_register_address_type    int_latched_clr;       //0x14
    pm_register_address_type    int_en_set;            //0x15
    pm_register_address_type    int_en_clr;            //0x16
    pm_register_address_type    int_latched_sts;       //0x18
    pm_register_address_type    int_pending_sts;       //0x19
    pm_register_address_type    int_mid_sel;           //0x1A
    pm_register_address_type    int_priority;          //0x1B
    pm_register_address_type    mode_ctl;              //0x40
    pm_register_address_type    dig_pull_ctl;          //0x42
    pm_register_address_type    dig_in_ctl;            //0x43
    pm_register_address_type    dig_out_ctl;           //0x45
    pm_register_address_type    dig_vin_ctl;           //0x41
    pm_register_address_type    en_ctl;                //0x46
    pm_register_address_type    dig_out_src_ctl;       //0x44
}pm_gpio_register_info_type;

/* GPIO config type */
typedef enum pm_drv_gpio_config_type
{
    PM_GPIO_DIG_IN, // - 0: Digital Input
    PM_GPIO_DIG_OUT, //- 1: Digital Output
    PM_GPIO_DIG_IN_DIG_OUT, //- 2: Digital Input and Digital Output
    PM_GPIO_ANALOG_PASS_THRU,  //- 3: Analog Pass Through
    PM_GPIO_TYPE_INVALID
} pm_drv_gpio_config_type;

typedef struct pm_gpio_data_type
{
    pm_comm_info_type            *comm_ptr;
    pm_gpio_register_info_type   *gpio_register;
    uint8                        num_of_peripherals;
    uint8 is_periph_unified_reg; 			
}pm_gpio_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_gpio_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_gpio_data_type* pm_gpio_get_data(uint8 pmic_index);

uint8 pm_gpio_get_num_peripherals(uint8 pmic_index);
#endif //PM_GPIO_DRIVER__H
