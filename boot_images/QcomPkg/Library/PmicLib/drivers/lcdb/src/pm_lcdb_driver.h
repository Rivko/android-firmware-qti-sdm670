#ifndef PM_LCDB_DRIVER_H
#define PM_LCDB_DRIVER_H

/*! \file pm_lcdb_driver.h
*  \n
*  \brief  This file contains functions prototypes and variable/type/constant
*         declarations for supporting lcdb peripheral.
*
*  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved.
*/

/*==========================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when        who     what, where, why
--------    ----    ---------------------------------------------------------
11/15/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"
#include "pm_comm.h"


/*===========================================================================

                        TYPES DEFINITIONS

===========================================================================*/
typedef struct
{
  pm_register_address_type base_address;               // 0xEC00
  pm_register_address_type revision1;                  // 0x00
  pm_register_address_type revision2;                  // 0x01
  pm_register_address_type sts1;                       // 0x08
  pm_register_address_type sts2;                       // 0x09
  pm_register_address_type sts3;                       // 0x0A
  pm_register_address_type sts4;                       // 0x0B
  pm_register_address_type sts5;                       // 0x0C
  pm_register_address_type sts6;                       // 0x0D
  pm_register_address_type int_rt_sts;                 // 0x10
  pm_register_address_type int_set_type;               // 0x11
  pm_register_address_type int_polarity_high;          // 0x12
  pm_register_address_type int_polarity_low;           // 0x13
  pm_register_address_type int_latched_clr;            // 0x14
  pm_register_address_type int_en_set;                 // 0x15
  pm_register_address_type int_en_clr;                 // 0x16
  pm_register_address_type int_latched_sts;            // 0x18
  pm_register_address_type int_pending_sts;            // 0x19
  pm_register_address_type int_mid_sel;                // 0x1A
  pm_register_address_type int_priority;               // 0x1B
  pm_register_address_type auto_touch_wake_ctl;        // 0x40
  pm_register_address_type bst_output_voltage;         // 0x41
  pm_register_address_type ring_suppression_ctl;       // 0x42
  pm_register_address_type config_sel;                 // 0x43
  pm_register_address_type hwen_ctl;                   // 0x44
  pm_register_address_type module_rdy;                 // 0x45
  pm_register_address_type enable_ctl1;                // 0x46
  pm_register_address_type bst_pd_ctl;                 // 0x47
  pm_register_address_type bst_clk_div;                // 0x48
  pm_register_address_type enable_ctl2;                // 0x4A
  pm_register_address_type bst_ilim_ctl;               // 0x4B
  pm_register_address_type current_sense;              // 0x4C
  pm_register_address_type loop_comp_ctl;              // 0x4D
  pm_register_address_type slope_comp_current;         // 0x4E
  pm_register_address_type ip_zero_detector;           // 0x4F
  pm_register_address_type ps_ctl;                     // 0x50
  pm_register_address_type pwm_ctl_1;                  // 0x51
  pm_register_address_type pwm_ctl_2;                  // 0x52
  pm_register_address_type rdson_mngmnt;               // 0x53
  pm_register_address_type bst_short_circuit_ctl;      // 0x54
  pm_register_address_type bst_vreg_ok_ctl;            // 0x55
  pm_register_address_type vregok_blank_ctl;           // 0x56
  pm_register_address_type bst_hdr_ctl;                // 0x57
  pm_register_address_type precharge_ctl;              // 0x5E
  pm_register_address_type soft_start_ctl;             // 0x5F
  pm_register_address_type misc_ctl;                   // 0x60
  pm_register_address_type pwm_ctrl_3;                 // 0x61
  pm_register_address_type pfm_ctl;                    // 0x62
  pm_register_address_type high_bw_ctl;                // 0x63
  pm_register_address_type spare_ctl1;                 // 0x64
  pm_register_address_type spare_ctl2;                 // 0x65
  pm_register_address_type pwrup_pwrdn_ctl;            // 0x66
  pm_register_address_type pwrup_pwrdn_ctl2;           // 0x68
  pm_register_address_type pfm_ctl2;                   // 0x69
  pm_register_address_type ldo_ctl;                    // 0x70
  pm_register_address_type ldo_output_voltage;         // 0x71
  pm_register_address_type ldo_short_circuit_ctl;      // 0x74
  pm_register_address_type ldo_vreg_ok_ctl;            // 0x75
  pm_register_address_type ldo_pd_ctl;                 // 0x77
  pm_register_address_type ldo_ilim_ctl1;              // 0x7B
  pm_register_address_type ldo_ilim_ctl2;              // 0x7C
  pm_register_address_type ldo_test_ctl;               // 0x7D
  pm_register_address_type ldo_soft_start_ctl;         // 0x7F
  pm_register_address_type ncp_ctl;                    // 0x80
  pm_register_address_type ncp_output_voltage;         // 0x81
  pm_register_address_type ncp_short_circuit_ctl;      // 0x84
  pm_register_address_type ncp_vreg_ok_ctl;            // 0x85
  pm_register_address_type ncp_pd_ctl;                 // 0x87
  pm_register_address_type ncp_clk_div;                // 0x88
  pm_register_address_type ncp_ilim_ctl1;              // 0x8B
  pm_register_address_type ncp_ilim_ctl2;              // 0x8C
  pm_register_address_type ncp_test_ctl;               // 0x8D
  pm_register_address_type ncp_soft_start_ctl;         // 0x8F
  pm_register_address_type dig_pwr_ctl;                // 0x90
  pm_register_address_type sec_access;                 // 0xD0
  pm_register_address_type perph_reset_ctl1;           // 0xD8
  pm_register_address_type perph_reset_ctl2;           // 0xD9
  pm_register_address_type perph_reset_ctl3;           // 0xDA
  pm_register_address_type perph_reset_ctl4;           // 0xDB
  pm_register_address_type int_test1;                  // 0xE0
  pm_register_address_type int_test_val;               // 0xE1
  pm_register_address_type dtest1_access;              // 0xE2
  pm_register_address_type dtest2_access;              // 0xE3
  pm_register_address_type dtest3_access;              // 0xE4
  pm_register_address_type dtest4_access;              // 0xE5
  pm_register_address_type atest1_access;              // 0xE6
  pm_register_address_type atest2_access;              // 0xE7
  pm_register_address_type atest3_access;              // 0xE8
  pm_register_address_type atest4_access;              // 0xE9
  pm_register_address_type atest5_access;              // 0xEA
  pm_register_address_type atest6_access;              // 0xEB
  pm_register_address_type test_ctl;                   // 0xEC
  pm_register_address_type pbus_write_sync_ctl;        // 0xEF
  pm_register_address_type trim_num;                   // 0xF0
  pm_register_address_type trim_pwm;                   // 0xF1
  pm_register_address_type trim_pfm;                   // 0xF2
  pm_register_address_type trim_ldo;                   // 0xF3
  pm_register_address_type trim_ncp;                   // 0xF4
} pm_lcdb_register_info_type;

typedef struct
{
    pm_comm_info_type           *comm_ptr;
    pm_lcdb_register_info_type  *lcdb_reg;
    // uint8                        num_of_peripherals;
} pm_lcdb_data_type;


/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
void pm_lcdb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info);

pm_lcdb_data_type *pm_lcdb_get_data(uint8 pmic_index);

// uint8 pm_lcdb_get_num_peripherals(uint8 pmic_index);

#endif  // PM_LCDB_DRIVER_H

