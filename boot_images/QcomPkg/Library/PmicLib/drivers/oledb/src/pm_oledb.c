/*! \file pm_oledb.c
*
*  \brief  pm_oledb.c
*
*  &copy; Copyright 2016-2017 QUALCOMM Technologies Incorporated, All Rights Reserved.
*/

/*===========================================================================

                        EDIT HISTORY

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header:

when        who     what, where, why
--------    ----    ---------------------------------------------------------
04/12/17    ks      Added oledb swire sparebit get and clear api's(CR-2004206)
02/07/17    sv      Added pm_oledb_pd_ctrl API
11/15/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_comm.h"
#include "pm_oledb_driver.h"
#include "pm_oledb.h"


/*===========================================================================

                        FUNCTION INTERFACE DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_oledb_get_status(uint8 pmic_index,
    pm_oledb_sts_type *status
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);

    if (!status || !oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type oledb_fault_status = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->fault_status;

    err_flag = pm_comm_read_byte(oledb_ptr->comm_ptr->slave_id, oledb_fault_status, &data, 0);
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        status->sc_fault   = data & PM_BIT(2) ? TRUE : FALSE;
        status->ol_fault   = data & PM_BIT(1) ? TRUE : FALSE;
        status->ilim_fault = data & PM_BIT(0) ? TRUE : FALSE;
    }

    return err_flag;
}

pm_err_flag_type pm_oledb_get_swire_sparebit_status(uint8 pmic_index,
    boolean *SparebitEnabled
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);

    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_register_address_type oledb_spare_reg = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->test6;

    err_flag = pm_comm_read_byte(oledb_ptr->comm_ptr->slave_id, oledb_spare_reg, &data, 0);
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        *SparebitEnabled   = data & PM_BIT(7) ? TRUE : FALSE;  // 7th Bit of OLEDB_TEST6 will be set to 1 by PBS once 51 swire rising edges are seen.
    }

    return err_flag;
}

pm_err_flag_type pm_oledb_clear_swire_sparebit_status(uint8 pmic_index
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);

    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0,sec_reg_data=0xa5; 
    pm_register_address_type oledb_spare_reg  = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->test6;
    pm_register_address_type oledb_sec_access = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->sec_access;
   
    err_flag  = pm_comm_write_byte(oledb_ptr->comm_ptr->slave_id, oledb_sec_access, sec_reg_data, 0); 
    err_flag |= pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_spare_reg, PM_BIT(7), data, 0);
   
    return err_flag;
}

pm_err_flag_type pm_oledb_set_module_rdy(uint8 pmic_index,
    boolean module_rdy
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type oledb_module_rdy = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->module_rdy;

    mask = PM_BIT(7);
    data = module_rdy << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_module_rdy, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_module_enable(uint8 pmic_index,
    boolean en_module
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type oledb_module_enable = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->module_enable;

    mask = PM_BIT(7);
    data = en_module << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_module_enable, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_set_ext_pin_ctl(uint8 pmic_index,
    boolean ext_pin_ctl
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type oledb_ext_pin_ctl = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->ext_pin_ctl;

    mask = PM_BIT(7);
    data = ext_pin_ctl << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_ext_pin_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_swire_ctl(uint8 pmic_index,
    pm_oledb_swire_control swire_control,
    boolean enable
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type oledb_swire_control = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->swire_control;

    switch(swire_control)
    {
        case PM_OLEDB_SWIRE_CONTROL__PD_EN:
            mask = PM_BIT(7);
            break;
        case PM_OLEDB_SWIRE_CONTROL__VOUT_EN:
            mask = PM_BIT(6);
            break;
        default:
            return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    data = enable << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_swire_control, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_vout_programming(uint8 pmic_index,
    pm_oledb_vout_program vout_program,
    uint32 op_voltage_mv
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (op_voltage_mv < pm_oledb_op_voltage[0] ||
             op_voltage_mv > pm_oledb_op_voltage[pm_oledb_op_voltage_size - 1])
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data = 0, i = 0;
    pm_register_address_type oledb_vout_pgm;

    switch(vout_program)
    {
        case PM_OLEDB_VOUT_PROGRAM__VOUT_REQ:
            oledb_vout_pgm = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->vout_pgm;
            break;
        case PM_OLEDB_VOUT_PROGRAM__VOUT_DEFAULT:
            oledb_vout_pgm = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->vout_default_ctl;
            break;
        default:
            return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    /* Output voltage can be set between 5.0V to 8.1V. */
    for(i = 0; ((i < pm_oledb_op_voltage_size) && (op_voltage_mv > pm_oledb_op_voltage[i])); i++);
    data = i;

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_vout_pgm, PM_BITS(4, 0), data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_pd_ctrl(uint8 pmic_index,
    boolean pd_enable
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type oledb_pd_ctl = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->pd_ctl;

    mask = PM_BIT(0);
    data = pd_enable;

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_pd_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_softstart_ctl(uint8 pmic_index,
    pm_oledb_softstart_ctl_type *softstart_ctl
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!softstart_ctl ||
             softstart_ctl->ss_time_vref >= PM_OLEDB_SOFTSTART_TIME_VREF__INVALID ||
             softstart_ctl->ss_iout_offset > PM_OLEDB_SOFTSTART_IOUT_OFFSET ||
             softstart_ctl->ss_ilim_time > PM_OLEDB_SOFTSTART_ILIM_TIME)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type oledb_softstart_ctl = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->softstart_ramp_delay;

    mask  = PM_BITS(7, 6);
    data  = softstart_ctl->ss_time_vref << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(5, 3);
    data |= softstart_ctl->ss_iout_offset << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(2, 0);
    data |= softstart_ctl->ss_ilim_time << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, oledb_softstart_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_psm_ctl(uint8 pmic_index,
    pm_oledb_psm_ctl_type *psm_ctl
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!psm_ctl ||
             psm_ctl->psm_vref >= PM_OLEDB_PSM_VREF__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type psm_ctrl;

    /* Config EN_PSM 0xE05B */
    psm_ctrl = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->en_psm;
    mask  = PM_BIT(7);
    data  = psm_ctl->psm_vref << PM_SHIFT_FROM_MASK(mask);

    err_flag  = pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, psm_ctrl, mask, data, 0);

    /* Config PSM_CTL 0xE05C */
    psm_ctrl = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->psm_ctl;
    mask  = PM_BIT(3);
    data  = psm_ctl->psm_hys_ctrl << PM_SHIFT_FROM_MASK(mask);
    mask |= PM_BITS(2, 0);
    data |= psm_ctl->psm_vref << PM_SHIFT_FROM_MASK(mask);

    err_flag |= pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, psm_ctrl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_oledb_pfm_ctl(uint8 pmic_index,
    pm_oledb_pfm_ctl_type *pfm_ctl
    )
{
    pm_oledb_data_type *oledb_ptr = pm_oledb_get_data(pmic_index);
    if (!oledb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!pfm_ctl ||
             pfm_ctl->pfm_ilim >= PM_OLEDB_PFM_ILIM__INVALID ||
             pfm_ctl->pfm_toff >= PM_OLEDB_PFM_TOFF__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type pfm_ctrl = oledb_ptr->oledb_reg->base_address + oledb_ptr->oledb_reg->pfm_ctl;

    mask  = PM_BIT(7);
    data  = pfm_ctl->en_psm << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(4);
    data |= pfm_ctl->pfm_hys_ctrl << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(3, 2);
    data |= pfm_ctl->pfm_ilim << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= pfm_ctl->pfm_toff << PM_SHIFT_FROM_MASK(mask);

    err_flag |= pm_comm_write_byte_mask(oledb_ptr->comm_ptr->slave_id, pfm_ctrl, mask, data, 0);

    return err_flag;
}
