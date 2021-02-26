#ifndef PM_IBB_DRIVER__H
#define PM_IBB_DRIVER__H

/*! \file pm_ibb _driver.h
 *  \n
 *  \brief This file contains functions prototypes and variable/type/constant
*          declarations for supporting SMBB peripheral
 *  \n
 *  \n &copy;
 *  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     ---------------------------------------------------------
12/02/16    ks      Added support for new IBB Type/Subtype
08/20/14    al      Updating comm lib
04/30/14    al      Initial revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"


/*===========================================================================

                        TYPE AND ENUM DEFINITIONS

===========================================================================*/
/************************************************************************/
/* register definitions                                                 */
/************************************************************************/
typedef struct
{
    uint32                    base_address;               //0xDC00
    pm_register_address_type  revision1;                  // 0x00
    pm_register_address_type  revision2;                  // 0x01
    pm_register_address_type  periph_subtype;             // 0x05
    pm_register_address_type  status1;                    // 0x08
    pm_register_address_type  status2;                    // 0x09
    pm_register_address_type  status3;                    // 0x0A
    pm_register_address_type  status4;                    // 0x0B
    pm_register_address_type  int_rt_sts;                 // 0x10
    pm_register_address_type  default_voltage;            // 0x40
    pm_register_address_type  output_voltage;             // 0x41
    pm_register_address_type  lcd_amoled_sel;             // 0x44
    pm_register_address_type  module_rdy;                 // 0x45
    pm_register_address_type  enable_ctl;                 // 0x46
    pm_register_address_type  pd_ctl;                     // 0x47
    pm_register_address_type  iplimit_startup_ctl_1;      // 0x49
    pm_register_address_type  iplimit_startup_ctl_2;      // 0x4A
    pm_register_address_type  current_limit;              // 0x4B
    pm_register_address_type  ps_ctl;                     // 0x50
    pm_register_address_type  vreg_ok_ctl;                // 0x55
    pm_register_address_type  pwrup_pwrdn_ctl_1;          // 0x58
    pm_register_address_type  pwrup_pwrdn_ctl_2;          // 0x59
    pm_register_address_type  swire_ctl;                  // 0x5A
    pm_register_address_type  output_slew_ctl;            // 0x5D
    pm_register_address_type  soft_start_ctl;             // 0x5F
    pm_register_address_type  smart_ps_ctl;               // 0x65
    pm_register_address_type  sec_access;                 // 0xD0
} pm_ibb_register_info_type;


typedef struct
{
    pm_comm_info_type            *comm_ptr;
    pm_ibb_register_info_type    *ibb_reg_table;
    uint8                        num_of_peripherals;
    pm_peripheral_revision_type  periph_revision;
} pm_ibb_data_type;


/*===========================================================================

                        FUNCTION DECLARATION

===========================================================================*/
void pm_ibb_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_ibb_data_type* pm_ibb_get_data(uint8 pmic_index);

uint8 pm_ibb_get_num_peripherals(uint8 pmic_index);

#endif // PM_IBB_DRIVER__H
