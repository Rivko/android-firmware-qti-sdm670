#ifndef PM_CLK_DRIVER__H
#define PM_CLK_DRIVER__H

/*! \file pm_clk_driver.h 
*  \n
 *  \brief This file contains CLOCK peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/16/14  akm     Comm change Updates
03/31/14    akm     Updated to the latest PMIC driver architecture   
09/25/13    kt      Updated clk module driver.
02/01/13    hw      Rearchitecting clk module driver to peripheral driver
========================================================================== */
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "hw_module_type.h"
#include "pm_target_information.h"
#include "pm_comm.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/

#define PM_CLK_INVALID_DATA   99

/* Please make sure to change the clk_info and clk_dep info arrays in
   config folder when the below macro is changed */
#define PM_MAX_NUM_CLKS       14

typedef struct 
{ 
    pm_register_address_type    base_address;  
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    drv_ctl1;   //0x44
    pm_register_address_type    en_ctl;     //0x46
    pm_register_address_type    pd_ctl;    //0x48
    pm_register_address_type    div_ctl1;   //0x43
    pm_register_address_type    status1;    //0x08
}pm_clk_register_info_type;


typedef struct
{
    pm_register_address_type     smpl_ctrl1;       
    pm_register_address_type     cal_rc4;       
}pm_clk_sleep_register_info_type;

typedef struct
{
    pm_register_address_type    mode_ctl1; 
    pm_register_address_type    xo_adj_fine; 
}pm_clk_xo_register_info_type;

typedef struct 
{
    pm_comm_info_type               *comm_ptr;
    uint32                          num_of_peripherals;
    uint8                           periph_subtype[PM_MAX_NUM_CLKS];
    //array of clk peripheral index  to get to right offset from base
    uint8                           clk_perph_index[PM_ALL_CLKS];
    pm_clk_type                     *clk_common;
    pm_clk_register_info_type       *clk_reg_table;
    pm_clk_sleep_register_info_type *clk_sleep_reg_table;
    pm_clk_xo_register_info_type    *clk_xo_reg_table;
}pm_clk_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_clk_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_clk_data_type* pm_clk_get_data(uint8 pmic_index);

uint8 pm_clk_get_num_peripherals(uint8 pmic_index);


#endif /* PM_CLK_DRIVER_H */


