#ifndef PM_OLEDB_DRIVER_H
#define PM_OLEDB_DRIVER_H

/*! \file pm_oledb_driver.h
*  \n
*  \brief  This file contains functions prototypes and variable/type/constant
*         declarations for supporting oledb peripheral.
*
*  \n &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when        who     what, where, why
--------    ----    ---------------------------------------------------------
11/15/16    mr      Created
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"
#include "pm_comm.h"


/*===========================================================================

                        TYPES AND MACRO DEFINITIONS

===========================================================================*/
typedef struct
{
  pm_register_address_type base_address;          // 0xE000
  pm_register_address_type revision1;             // 0x00
  pm_register_address_type revision2;             // 0x01
  pm_register_address_type fault_status;          // 0x08
  pm_register_address_type int_rt_sts;            // 0x10
  pm_register_address_type int_set_type;          // 0x11
  pm_register_address_type int_polarity_high;     // 0x12
  pm_register_address_type int_polarity_low;      // 0x13
  pm_register_address_type int_latched_clr;       // 0x14
  pm_register_address_type int_en_set;            // 0x15
  pm_register_address_type int_en_clr;            // 0x16
  pm_register_address_type int_latched_sts;       // 0x18
  pm_register_address_type int_pending_sts;       // 0x19
  pm_register_address_type int_mid_sel;           // 0x1A
  pm_register_address_type int_priority;          // 0x1B
  pm_register_address_type module_rdy;            // 0x45
  pm_register_address_type module_enable;         // 0x46
  pm_register_address_type ext_pin_ctl;           // 0x47
  pm_register_address_type swire_control;         // 0x48
  pm_register_address_type vout_pgm;              // 0x49
  pm_register_address_type vout_default_ctl;      // 0x4A
  pm_register_address_type pd_ctl;                // 0x4B
  pm_register_address_type min_on_off_ctl;        // 0x4C
  pm_register_address_type switching_frequency;   // 0x4D
  pm_register_address_type ilim_nfet;             // 0x4E
  pm_register_address_type current_sense_ratio;   // 0x50
  pm_register_address_type slope_comp_adj;        // 0x51
  pm_register_address_type bias_gen_warmup_delay; // 0x52
  pm_register_address_type softstart_ramp_delay;  // 0x53
  pm_register_address_type pwr_stage_ctl;         // 0x54
  pm_register_address_type vloop_comp_gain;       // 0x55
  pm_register_address_type vloop_comp_gm;         // 0x56
  pm_register_address_type vloop_comp_zero;       // 0x57
  pm_register_address_type overload;              // 0x58
  pm_register_address_type short_protect;         // 0x59
  pm_register_address_type fast_precharge;        // 0x5A
  pm_register_address_type en_psm;                // 0x5B
  pm_register_address_type psm_ctl;               // 0x5C
  pm_register_address_type pfm_ctl;               // 0x5D
  pm_register_address_type zx_comp_ctl;           // 0x60
  pm_register_address_type nlimit;                // 0x64
  pm_register_address_type vfloat_ctl;            // 0x66
  pm_register_address_type sec_access;            // 0xD0
  pm_register_address_type perph_reset_ctl1;      // 0xD8
  pm_register_address_type perph_reset_ctl2;      // 0xD9
  pm_register_address_type perph_reset_ctl3;      // 0xDA
  pm_register_address_type perph_reset_ctl4;      // 0xDB
  pm_register_address_type int_test1;             // 0xE0
  pm_register_address_type int_test_val;          // 0xE1
  pm_register_address_type test1_dtest1;          // 0xE2
  pm_register_address_type test1_dtest2;          // 0xE3
  pm_register_address_type test2_atest;           // 0xE4
  pm_register_address_type test3_tm;              // 0xE5
  pm_register_address_type test4_en_ctl;          // 0xE6
  pm_register_address_type test4;                 // 0xE7
  pm_register_address_type test5;                 // 0xE8
  pm_register_address_type test6;                 // 0xE9
  pm_register_address_type pbus_write_sync_ctl;   // 0xEF
  pm_register_address_type trim_num;              // 0xF0
  pm_register_address_type trim_ilim;             // 0xF1
  pm_register_address_type trim_vout;             // 0xF2
} pm_oledb_register_info_type;

typedef struct
{
    pm_comm_info_type            *comm_ptr;
    pm_oledb_register_info_type  *oledb_reg;
    // uint8                        num_of_peripherals;
} pm_oledb_data_type;


/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
void pm_oledb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info);

pm_oledb_data_type *pm_oledb_get_data(uint8 pmic_index);

// uint8 pm_oledb_get_num_peripherals(uint8 pmic_index);

#endif  // PM_OLEDB_DRIVER_H

