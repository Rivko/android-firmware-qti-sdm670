/*! \file pm_lcdb.c
*
*  \brief  pm_lcdb.c
*
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                        EDIT HISTORY

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header:

when        who     what, where, why
--------    ---     ---------------------------------------------------------
04/03/17    ks      SW WA for the LCDB-NCP issue (CR#2025449)
11/15/16    mr      Initial Revision
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_comm.h"
#include "pm_lcdb_driver.h"
#include "pm_lcdb.h"
#include "busywait.h"
#include "pm_version.h"


/*===========================================================================

                        FUNCTION INTERFACE DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_lcdb_get_status(uint8 pmic_index,
    pm_lcdb_sts_type *status
    )
{
    #define LCDB_NUM_STATUS_REG  6
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!status)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data[LCDB_NUM_STATUS_REG] = {0};
    pm_register_address_type lcdb_sts = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->sts1;

    /* NOTE: All LCDB Status registers to be written once with any value before reading */

    err_flag  = pm_comm_write_byte_array(lcdb_ptr->comm_ptr->slave_id, lcdb_sts, LCDB_NUM_STATUS_REG, data, 0);
    err_flag |= pm_comm_read_byte_array (lcdb_ptr->comm_ptr->slave_id, lcdb_sts, LCDB_NUM_STATUS_REG, data, 0);
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        status->lcdb_vreg_ok = data[0] & PM_BIT(7) ? TRUE : FALSE;
        status->lcdb_sc_det  = data[0] & PM_BIT(6) ? TRUE : FALSE;
        status->lcdb_seq_status = data[4] & PM_BITS(3, 0);

        status->boost_vreg_ok = data[1] & PM_BIT(7) ? TRUE : FALSE;
        status->boost_sc_det = data[1] & PM_BIT(6) ? TRUE : FALSE;
        status->boost_status = data[1] & PM_BITS(5, 4) >> PM_SHIFT_FROM_MASK(PM_BITS(5, 4));
        status->boost_output_voltage = pm_lcdb_boost_op_voltage[data[5]];

        status->ldo_vreg_ok = data[2] & PM_BIT(7) ? TRUE : FALSE;
        status->ldo_sc_detect = data[2] & PM_BIT(6) ? TRUE : FALSE;
        status->ldo_status = data[2] & PM_BITS(5, 4) >> PM_SHIFT_FROM_MASK(PM_BITS(5, 4));
        status->ldo_ireg_active = data[2] & PM_BIT(3) ? TRUE : FALSE;
        status->ldo_vgs_gt_2vt = data[2] & PM_BIT(2) ? TRUE : FALSE;

        status->ncp_vreg_ok = data[3] & PM_BIT(7) ? TRUE : FALSE;
        status->ncp_sc_detect = data[3] & PM_BIT(7) ? TRUE : FALSE;
        status->ncp_status = data[3] & PM_BITS(5, 4) >> PM_SHIFT_FROM_MASK(PM_BITS(5, 4));
    }

    return err_flag;
}

pm_err_flag_type pm_lcdb_set_boost_output_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (op_voltage_mv < pm_lcdb_boost_op_voltage[0] ||
             op_voltage_mv > pm_lcdb_boost_op_voltage[pm_lcdb_boost_op_voltage_size - 1])
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data = 0, i = 0;
    pm_register_address_type lcdb_bst_output_voltage = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->bst_output_voltage;

    /* Output voltage can be set between 4.7V to 6.25V in 50mV resolution.
     * Vout = (4700mV + (50mV * code)) */
    for(i = 0; ((i < pm_lcdb_boost_op_voltage_size) && (op_voltage_mv > pm_lcdb_boost_op_voltage[i])); i++);
    data = i;

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_bst_output_voltage, PM_BITS(4, 0), data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_set_config_sel(uint8 pmic_index,
    pm_lcdb_config_sel config_sel
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (config_sel >= PM_LCDB_CONFIG__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type lcdb_config_sel = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->config_sel;

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_config_sel, PM_BITS(2, 0), (uint8)config_sel, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_set_module_rdy(uint8 pmic_index,
    boolean module_rdy
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lcdb_module_rdy = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->module_rdy;

    mask = PM_BIT(7);
    data = module_rdy << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_module_rdy, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_module_enable(uint8 pmic_index,
    boolean module_en
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 en_crtl_mask = 0, sc_protection_mask =0, data = 0;
    pm_device_info_type  PmicDeviceInfo;
    pm_register_address_type lcdb_enable_ctl = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->enable_ctl1;
    pm_register_address_type lcdb_misc_ctl   = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->misc_ctl;
    
    en_crtl_mask = PM_BIT(7);
    sc_protection_mask =  PM_BIT(0);
    data = module_en << PM_SHIFT_FROM_MASK(en_crtl_mask);
    
    err_flag = pm_get_pmic_info(pmic_index, &PmicDeviceInfo);
    if(err_flag != PM_ERR_FLAG__SUCCESS){return err_flag;}
    
    if(module_en && (PMIC_IS_PM660L == PmicDeviceInfo.ePmicModel))
    { 
      if((1==PmicDeviceInfo.nPmicAllLayerRevision) && (1==PmicDeviceInfo.nPmicMetalRevision))
      {
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, 0x80, 0); // LCD Enable
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, 0x00, 0); // LCD Disable
        busywait(10000); //10ms
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_misc_ctl, sc_protection_mask, 0x01, 0); //Disable SCP           
        busywait(1000); //1ms
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_misc_ctl, sc_protection_mask, 0x00, 0); //Enable SCP
        busywait(1000); //1ms
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, 0x80, 0); // Enable
      }
      else
      {
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, 0x80, 0); // Enable
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, 0x00, 0); // Disable
        err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, 0x80, 0); // Enable
      }
    }
    else
    {
      err_flag |= pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, en_crtl_mask, data, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_lcdb_module_hw_enable(uint8 pmic_index,
    boolean hwen_rdy
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lcdb_enable_ctl = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->enable_ctl1;

    mask = PM_BIT(6);
    data = hwen_rdy << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_enable_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_curr_sense_cfg(uint8 pmic_index,
    pm_lcdb_curr_sense_type *curr_sense
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(!curr_sense ||
            curr_sense->isense_tap >= PM_LCDB_ISENSE_TAP__INVALID ||
            curr_sense->isense_gain >= PM_LCDB_ISENSE_GAIN__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lcdb_current_sense = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->current_sense;

    mask  = PM_BITS(3, 2);
    data  = curr_sense->isense_tap << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= curr_sense->isense_gain << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_current_sense, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_ps_ctl(uint8 pmic_index,
    pm_lcdb_ps_ctl_type *ps_ctl
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(!ps_ctl ||
            ps_ctl->ps_threshold >= PM_LCDB_PS_THRESHOLD__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lcdb_ps_ctl = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->ps_ctl;

    mask  = PM_BIT(7);
    data  = ps_ctl->en_ps << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(3);
    data |= ps_ctl->sel_dig_ps << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(2);
    data |= ps_ctl->sel_ps_table << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= ps_ctl->ps_threshold << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_ps_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_pfm_ctl(uint8 pmic_index,
    pm_lcdb_pfm_ctl_type *pfm_ctl
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!pfm_ctl ||
             pfm_ctl->pfm_hysteresis >= PM_LCDB_PFM_HYSTERESIS__INVALID ||
             pfm_ctl->pfm_peak_current >= PM_LCDB_PFM_PEAK_CURRENT__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lcdb_pfm_ctl = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->pfm_ctl;

    mask  = PM_BIT(7);
    data  = pfm_ctl->en_pfm << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(5, 4);
    data |= pfm_ctl->pfm_hysteresis << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(3, 2);
    data |= pfm_ctl->pfm_peak_current << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(1);
    data |= pfm_ctl->spare << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(0);
    data |= pfm_ctl->byp_bst_soft_start_comp << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_pfm_ctl, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_set_ldo_output_voltage(uint8 pmic_index,
    uint32 op_voltage_mv
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (op_voltage_mv < pm_lcdb_ldo_op_voltage[0] ||
             op_voltage_mv > pm_lcdb_ldo_op_voltage[pm_lcdb_ldo_op_voltage_size - 1])
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data = 0, i = 0;
    pm_register_address_type lcdb_ldo_output_voltage = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->ldo_output_voltage;

    /* Output voltage can be set between 4.0V to 6.0V. */
    for(i = 0; ((i < pm_lcdb_ldo_op_voltage_size) && (op_voltage_mv > pm_lcdb_ldo_op_voltage[i])); i++);
    data = i;

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_ldo_output_voltage, PM_BITS(4, 0), data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_set_ncp_output_voltage(uint8 pmic_index,
    boolean en_ncp_vout,
    int32 op_voltage_mv /* negative value */
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    op_voltage_mv *= (-1);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (op_voltage_mv < pm_lcdb_ncp_op_voltage[0] ||
             op_voltage_mv > pm_lcdb_ncp_op_voltage[pm_lcdb_ncp_op_voltage_size - 1])
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0, i = 0;
    pm_register_address_type lcdb_ncp_output_voltage = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->ncp_output_voltage;

    /* Output voltage can be set between 4.0V to 6.0V. */
    mask  = PM_BIT(7);
    data  = en_ncp_vout << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(4, 0);
    for(i = 0; ((i < pm_lcdb_ncp_op_voltage_size) && (op_voltage_mv > pm_lcdb_ncp_op_voltage[i])); i++);
    data |= i;

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_ncp_output_voltage, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_ncp_ilim_ctl(uint8 pmic_index,
    pm_lcdb_ncp_ilim_ctl_type *ncp_ilim_ctl
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (!ncp_ilim_ctl ||
             ncp_ilim_ctl->ncp_ilim_max_ss_sd >= PM_LCDB_NCP_ILIM_SS_SD_CTL__INVALID)
    {
        return PM_ERR_FLAG_INVALID_PARAMETER;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 0, data = 0;
    pm_register_address_type lcdb_ncp_ilim_ctl = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->ncp_ilim_ctl1;

    /* ncp_ilim_ctl1 config */
    mask  = PM_BIT(7);
    data  = ncp_ilim_ctl->en_ncp_ilim << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(5);
    data |= ncp_ilim_ctl->byp_ncpgd_to_ilim << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BIT(4);
    data |= ncp_ilim_ctl->en_ncp_ilim_gain << PM_SHIFT_FROM_MASK(mask);

    mask |= PM_BITS(1, 0);
    data |= ncp_ilim_ctl->ncp_ilim_max_ss_sd << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_ncp_ilim_ctl, mask, data, 0);

    /* ncp_ilim_ctl2 config */
    mask  = PM_BITS(1, 0);
    data  = ncp_ilim_ctl->ncp_ilim_max_ss_sd << PM_SHIFT_FROM_MASK(mask);

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_ncp_ilim_ctl + 1, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_lcdb_ncp_soft_start_ctl(uint8 pmic_index,
    pm_lcdb_ncp_softstart_ctl ncp_softstart_time
    )
{
    pm_lcdb_data_type *lcdb_ptr = pm_lcdb_get_data(pmic_index);

    if (!lcdb_ptr)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type lcdb_ncp_soft_start_ctl = lcdb_ptr->lcdb_reg->base_address + lcdb_ptr->lcdb_reg->ncp_soft_start_ctl;

    err_flag = pm_comm_write_byte_mask(lcdb_ptr->comm_ptr->slave_id, lcdb_ncp_soft_start_ctl, PM_BITS(1, 0), (uint8) ncp_softstart_time, 0);

    return err_flag;
}

