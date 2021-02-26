#ifndef PM_RTC_DRIVER__H
#define PM_RTC_DRIVER__H

/*! \file
*  \n
*  \brief  pm_rtc_driver.h
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting RTC pin services for the Qualcomm
*  PMIC chip set.
*  \n &copy;  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when		 who      what, where, why
--------	 ---      ----------------------------------------------------------
05/21/15         al       Adding RTC alarm and interrupt registers
08/20/14         al       Updating comm lib 
11/22/13         va       PmicLib Dec Addition
08/01/13         al       New arch
========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm_rtc.h"
#include "pm_target_information.h"

/*===================================================================
               DEFINITIONS
====================================================================*/

typedef struct
{
    pm_register_address_type    base_address;  
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    time_adj;           //0x44    //rtc_time_adjust;    
    pm_register_address_type    rtc_en_ctl;         //0x46    //sec_rtc_ctrl; 
    pm_register_address_type    alrm_en_ctl;        //0x146            
    pm_register_address_type    abort_en_ctl;       //0x46            
    pm_register_address_type    int_latched_clr;    //0x14    //sec_rtc_alrm_ctrl; 
    pm_register_address_type    wdata0;             //0x40    //sec_rtc_clk_wdata0;         
    pm_register_address_type    wdata1;             //0x41    //sec_rtc_clk_wdata1;         
    pm_register_address_type    wdata2;             //0x42    //sec_rtc_clk_wdata2;         
    pm_register_address_type    wdata3;             //0x43    //sec_rtc_clk_wdata3;         
    pm_register_address_type    rdata0;             //0x48    //sec_rtc_clk_rdata0;         
    pm_register_address_type    rdata1;             //0x49    //sec_rtc_clk_rdata1;         
    pm_register_address_type    rdata2;             //0x4A    //sec_rtc_clk_rdata2;         
    pm_register_address_type    rdata3;             //0x4B    //sec_rtc_clk_rdata3;         
    pm_register_address_type    alrm_data0;         //0x140   //sec_rtc_alrm_data0;         
    pm_register_address_type    alrm_data1;         //0x141   //sec_rtc_alrm_data1;         
    pm_register_address_type    alrm_data2;         //0x142   //sec_rtc_alrm_data2;         
    pm_register_address_type    alrm_data3;         //0x143   //sec_rtc_alrm_data3;         
    pm_register_address_type    test;               //0xE0    //sec_rtc_test;
    pm_register_address_type    alarm_status1;       //0x08    //RTC alarm status
    pm_register_address_type    int_rt_sts;            //0x10
    pm_register_address_type    int_set_type;          //0x11
    pm_register_address_type    int_polarity_high;     //0x12
    pm_register_address_type    int_polarity_low;      //0x13
    pm_register_address_type    int_en_set;            //0x15
    pm_register_address_type    int_en_clr;            //0x16
    pm_register_address_type    int_latched_sts;       //0x18
    pm_register_address_type    int_pending_sts;       //0x19
    pm_register_address_type    int_mid_sel;           //0x1A
    pm_register_address_type    int_priority;          //0x1B
                                                   
}pm_rtc_register_info_type;




typedef enum pm_hal_rtc_internal_reg_type
{
    PM_HAL_RTC_INTERNAL_REG__COUNTER,
    PM_HAL_RTC_INTERNAL_REG__ALRM1,
    PM_HAL_RTC_INTERNAL_REG__INVALID
} pm_hal_rtc_internal_reg_type;

typedef enum pm_rtc_val_shift_type
{
    PM_RTC_VAL0_SHIFT = 0,
    PM_RTC_VAL1_SHIFT = 8,
    PM_RTC_VAL2_SHIFT = 16,
    PM_RTC_VAL3_SHIFT = 24,
    PM_HAL_RTC_INTERNAL_VAL_FROM_TIME__INVALID
} pm_rtc_val_shift_type;

typedef struct
{
    pm_comm_info_type           *comm_ptr;
    pm_rtc_register_info_type   *rtc_reg_table;
    uint8                       num_of_peripherals;

}pm_rtc_data_type;
/*************************************
NOTE: Initializer IMPLEMENTATION
**************************************/
void pm_rtc_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_rtc_data_type* pm_rtc_get_data(uint8 pmic_index);

uint8 pm_rtc_get_num_peripherals(uint8 pmic_index);

#endif //PM_RTC_DRIVER__H
