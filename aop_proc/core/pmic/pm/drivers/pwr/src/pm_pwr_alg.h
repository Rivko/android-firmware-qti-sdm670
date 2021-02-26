#ifndef PM_PWR_ALG__H
#define PM_PWR_ALG__H

/*! \file pm_pwr_alg.h
 *  \n
 *  \brief   
 *  \details  
 *  \n &copy; Copyright 2011-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/drivers/pwr/src/pm_pwr_alg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "bare_metal_info.h"
#include "pm_resources_and_types.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_comm.h"

/*===========================================================================

                      TYPES AND STRUCTURES 

===========================================================================*/
#define PM_VOLT_INVALID_RANGE 255

typedef struct
{
    unsigned    start:16;
    unsigned    end:16;
}pm_pwr_range_info_type;

typedef struct
{
    uint32    RangeMin;
    uint32    RangeMax;
    uint32    VStep;			
}pm_pwr_volt_info_type;

/* These fields are mapped to registers that are needed by power algorithms */
typedef struct 
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    STATUS;      // STATUS 0x08
    pm_register_address_type    INT_SET_TYPE; // INT_SET_TYPE 0x11
    pm_register_address_type    INT_POLARITY_HIGH; // INT_POLARITY_HIGH 0x12
    pm_register_address_type    INT_POLARITY_LOW; // INT_POLARITY_LOW 0x13
    pm_register_address_type    INT_LATCHED_CLR; // INT_LATCHED_CLR 0x14
    pm_register_address_type    INT_EN_SET; // INT_EN_SET 0x15
    pm_register_address_type    INT_EN_CLR; // INT_EN_CLR 0x16
    pm_register_address_type    VOLTAGE_CTRL1; // VOLTAGE_CTL1 0x40 
    pm_register_address_type    VOLTAGE_CTRL2; // VOLTAGE_CTL2 0x41 
    pm_register_address_type    VSET_VALID_LB; // VSET_VALID_LB 0x42
    pm_register_address_type    VSET_VALID_UB; // VSET_VALID_UB 0x43
    pm_register_address_type    MODE_CTL;      // MODE_CTL 0x45 
    pm_register_address_type    EN_CTL;        // EN_CTL 0x46
    pm_register_address_type    FOLLOW_HWEN; // FOLLOW_HWEN 0x47  
    pm_register_address_type    SOFT_START_CTL; // SOFT_START_CTL 0x60
    pm_register_address_type    VOLTAGE_STEP_CTL; // VOLTAGE_STEP_CTL 0x61
    pm_register_address_type    OCP; // OCP 0x66
    pm_register_address_type    ULS_VSET_LB; // ULS_VSET_LB 0x39
    pm_register_address_type    PMIC4_ULS_VSET_LB; // ULS_VSET_LB 0x68
    pm_register_address_type    SEC_ACCESS; // SEC_ACCESS 0xD0
}pm_pwr_register_info_type;

typedef struct
{
    pm_pwr_volt_info_type         pwr_vset;
    pm_register_address_type      periph_base_address;
    uint8                         periph_type;
    boolean                       is_periph_stepper;
    boolean                       is_periph_unified_reg;
    boolean                       pmic4_flag;
}pm_pwr_specific_info_type;

typedef struct
{
    pm_pwr_register_info_type     *pwr_reg_table;
    uint32                        num_of_peripherals;
    pm_pwr_specific_info_type     *pwr_specific_info;
}pm_pwr_data_type;

/*===========================================================================

                     FUNCTION DECLARATIONS 

===========================================================================*/

pm_err_flag_type pm_pwr_sw_mode_raw_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8 mode);

pm_err_flag_type pm_pwr_sw_mode_status_raw_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8 *mode_ctl);

pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_on_off_type on_off);

pm_err_flag_type pm_pwr_sw_enable_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_on_off_type *on_off);

pm_err_flag_type pm_pwr_is_vreg_ready_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, boolean *vreg_ready);

pm_err_flag_type pm_pwr_pin_ctrl_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_on_off_type *on_off, uint8 *select_pin);

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_volt_level_type  volt_level);

pm_err_flag_type pm_pwr_volt_level_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_volt_level_type *volt_level);

pm_err_flag_type pm_pwr_volt_get_vstep(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8 *vstep_ptr);

pm_err_flag_type pm_pwr_enable_get_softstart(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, uint8* ss_ptr);   

pm_err_flag_type pm_pwr_irq_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_pwr_irq_type irq, boolean enable);

pm_err_flag_type pm_pwr_irq_clear_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_pwr_irq_type irq);

pm_err_flag_type pm_pwr_irq_set_trigger_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint32 peripheral_index, pm_pwr_irq_type irq, pm_irq_trigger_type trigger);

pm_err_flag_type pm_pwr_ocp_latched_status_clear_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *ocp_occured);

#endif /* PM_PWR_ALG__H */
