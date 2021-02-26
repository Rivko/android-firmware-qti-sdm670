/*! \file
 *  
 *  \brief  pm_bsi.h ----This file contains MIPI - BIF related settings.
 *  \details This file contains customizable target specific 
 * driver settings & PMIC registers. This file is generated from database functional
 * configuration information that is maintained for each of the targets.
 *  
 *  &copy;
 *  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

 
when       	who     	 what, where, why
--------   	---    		 ---------------------------------------------------------- 
08/20/14        al                Updating comm lib 
05/20/14        al                Architecture update
04/18/14        al                Updated copyright
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_schg_driver.h"
#include "pm_bsi.h"
#include "pm_resource_manager.h"

/*===========================================================================
                       MACRO DECLARATIONS
===========================================================================*/

#define ADDRESS(x) (schg_ptr->schg_register->bsi_register->base_address + schg_ptr->schg_register->bsi_register->x)
#define SLAVE_ID   (schg_ptr->comm_ptr->slave_id)

/*===========================================================================

FUNCTION DEFINITIONS 

===========================================================================*/

/* BSI APIs - To be moved to independent file */
static pm_bsi_data_format_type current_bsi_rx_mode = PM_BSI_DATA_FORMAT__11_BIT;
static pm_bsi_data_format_type current_bsi_tx_mode = PM_BSI_DATA_FORMAT__11_BIT;

pm_err_flag_type pm_bsi_enable(uint32 device_index, pm_bsi_cmd_type enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x80;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bsi_en = ADDRESS(bsi_en);

        data = (PM_BSI_CMD__ENABLE == enable) ? 0xFF : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, bsi_en, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_set_tau_cfg(uint32 device_index, pm_bsi_tau_cfg_type tau_cfg)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    uint8 mask = 0x07;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_tau_cfg = ADDRESS(mipi_bif_tau_cfg);

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_tau_cfg, mask, tau_cfg, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_bsi_set_rx_data_format(uint32 device_index,  pm_bsi_data_format_type data_format)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x02;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_mode = ADDRESS(mipi_bif_mode);

        data = (PM_BSI_DATA_FORMAT__17_BIT == data_format) ? 0xFF : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_mode, mask, data, 0);

        if (errFlag == PM_ERR_FLAG_SUCCESS) current_bsi_rx_mode = data_format;
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_set_tx_data_format(uint32 device_index,  pm_bsi_data_format_type data_format)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x01;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_mode = ADDRESS(mipi_bif_mode);

        data = (PM_BSI_DATA_FORMAT__17_BIT == data_format) ? 0xFF : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_mode, mask, data, 0);

        if (errFlag == PM_ERR_FLAG_SUCCESS) current_bsi_tx_mode = data_format;
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_rx_enable(uint32 device_index,  pm_bsi_cmd_type enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x40;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_en = ADDRESS(mipi_bif_en);

        data = (PM_BSI_CMD__ENABLE == enable) ? 0xFF : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_en, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_tx_enable(uint32 device_index,  pm_bsi_cmd_type enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x80;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_en = ADDRESS(mipi_bif_en);

        data = (PM_BSI_CMD__ENABLE == enable) ? 0xFF : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_en, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_read_rx_data(uint32 device_index,  uint32 *rd_data)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);


    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_data_rx_0 = ADDRESS(mipi_bif_data_rx_0);
        pm_register_address_type mipi_bif_data_rx_1 = ADDRESS(mipi_bif_data_rx_1);
        pm_register_address_type mipi_bif_data_rx_2 = ADDRESS(mipi_bif_data_rx_2);

        if (!rd_data)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        if (PM_BSI_DATA_FORMAT__11_BIT == current_bsi_rx_mode)
        {
            errFlag = pm_comm_read_byte_mask(SLAVE_ID, mipi_bif_data_rx_0, 0xFF, &data, 0);
            *rd_data = (uint32)data & 0x000000FF;
            errFlag = pm_comm_read_byte_mask(SLAVE_ID, mipi_bif_data_rx_1, 0x07, &data, 0);
            *rd_data |= ((uint32)data << 8) & 0x0000FF00;
        }
        else
        {
            errFlag = pm_comm_read_byte_mask(SLAVE_ID, mipi_bif_data_rx_0, 0xFF, &data, 0);
            *rd_data = (uint32)data & 0x000000FF;
            errFlag = pm_comm_read_byte_mask(SLAVE_ID, mipi_bif_data_rx_1, 0xFF, &data, 0);
            *rd_data |= ((uint32)data << 8) & 0x0000FF00;
            errFlag = pm_comm_read_byte_mask(SLAVE_ID, mipi_bif_data_rx_2, 0x01, &data, 0);
            *rd_data |= ((uint32)data << 16) & 0x00FF0000;
        }

    }
    return errFlag;
}

pm_err_flag_type pm_bsi_write_tx_data(uint32 device_index,  uint32 wr_data)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_data_tx_0 = ADDRESS(mipi_bif_data_tx_0);
        pm_register_address_type mipi_bif_data_tx_1 = ADDRESS(mipi_bif_data_tx_1);
        pm_register_address_type mipi_bif_data_tx_2 = ADDRESS(mipi_bif_data_tx_2);

        if (PM_BSI_DATA_FORMAT__11_BIT == current_bsi_tx_mode)
        {
            data = (pm_register_data_type)wr_data;
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_data_tx_0, 0xFF, data, 0);
            data = (pm_register_data_type)(wr_data >> 8);
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_data_tx_1, 0x07, data, 0);
        }
        else
        {
            data = (pm_register_data_type)wr_data;
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_data_tx_0, 0xFF, data, 0);
            data = (pm_register_data_type)(wr_data >> 8);
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_data_tx_1, 0xFF, data, 0);
            data = (pm_register_data_type)(wr_data >> 16);
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_data_tx_2, 0x01, data, 0);
        }

    }
    return errFlag;
}

pm_err_flag_type pm_bsi_tx_go(uint32 device_index)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x01;
    uint8 mask = 0x01;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_tx_ctl = ADDRESS(mipi_bif_tx_ctl);

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_tx_ctl, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_get_tx_go_status(uint32 device_index,  boolean *go_sts)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x02;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bsi_sts = ADDRESS(bsi_sts);

        if (!go_sts)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        errFlag = pm_comm_read_byte_mask(SLAVE_ID, bsi_sts, mask, &data, 0);

        *go_sts = (data == 0x02);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_get_err_flag_status(uint32 device_index, boolean *err_flag_sts)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x10;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bsi_sts = ADDRESS(bsi_sts);

        if (!err_flag_sts)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        errFlag = pm_comm_read_byte_mask(SLAVE_ID, bsi_sts, mask, &data, 0);

        *err_flag_sts = (data == 0x10);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_set_sampling_rate(uint32 device_index, pm_bsi_sampling_rate_type sampling_rate)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    uint8 mask = 0x10;
    uint8 data;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_tau_cfg = ADDRESS(mipi_bif_tau_cfg);

        data = (PM_BSI_SAMPLING_RATE__8X == sampling_rate) ? 0x10 : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_tau_cfg, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_clear_error(uint32 device_index)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x80;
    uint8 mask = 0x80;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_err_clear = ADDRESS(mipi_bif_err_clear);

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_err_clear, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_get_error_code(uint32 device_index,  pm_bsi_error_code_type *error)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x07;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_error = ADDRESS(mipi_bif_error);

        if (!error)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        errFlag = pm_comm_read_byte_mask(SLAVE_ID, mipi_bif_error, mask, &data, 0);

        *error = (pm_bsi_error_code_type)data;

    }
    return errFlag;
}


pm_err_flag_type pm_bsi_irq_enable(uint32 device_index, pm_bsi_irq_type irq, boolean enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type irq_reg;
    uint8 mask = 1 << (irq & 0x07); //irq%8 == irq &0x07
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_BSI_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        if (enable)
        {
            irq_reg = schg_ptr->schg_register->bsi_register->base_address + 0x15;
        }
        else
        {
            irq_reg = schg_ptr->schg_register->bsi_register->base_address + 0x16;
        }

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, irq_reg, mask, 0xFF, 0);
    }
    return errFlag;
}

pm_err_flag_type pm_bsi_irq_clear(uint32 device_index, pm_bsi_irq_type irq)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    uint8 mask = 1 << (irq & 0x07); //irq%8 == irq &0x07
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_BSI_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type int_latched_clr = schg_ptr->schg_register->bsi_register->base_address + 0x14;
        errFlag = pm_comm_write_byte_mask(SLAVE_ID, int_latched_clr, mask, 0xFF, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_bsi_irq_set_trigger(uint32 device_index, pm_bsi_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    uint8 mask = 1 << (irq & 0x07); //irq%8 == irq &0x07
    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_BSI_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    else
    {
        pm_register_address_type int_set_type = schg_ptr->schg_register->bsi_register->base_address + 0x11;
        pm_register_address_type int_polarity_high = schg_ptr->schg_register->bsi_register->base_address + 0x12;
        pm_register_address_type int_polarity_low = schg_ptr->schg_register->bsi_register->base_address + 0x13;

        switch (trigger)
        {
        case PM_IRQ_TRIGGER_ACTIVE_LOW:
            set_type = 0x00;
            polarity_high = 0x00;
            polarity_low = 0xFF;
            break;
        case PM_IRQ_TRIGGER_ACTIVE_HIGH:
            set_type = 0x00;
            polarity_high = 0xFF;
            polarity_low = 0x00;
            break;
        case PM_IRQ_TRIGGER_RISING_EDGE:
            set_type = 0xFF;
            polarity_high = 0xFF;
            polarity_low = 0x00;
            break;
        case PM_IRQ_TRIGGER_FALLING_EDGE:
            set_type = 0xFF;
            polarity_high = 0x00;
            polarity_low = 0xFF;
            break;
        case PM_IRQ_TRIGGER_DUAL_EDGE:
            set_type = 0xFF;
            polarity_high = 0xFF;
            polarity_low = 0xFF;
            break;
        default:
            return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }
        errFlag = pm_comm_write_byte_mask(SLAVE_ID, int_set_type, mask, set_type, 0);
        errFlag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_high, mask, polarity_high, 0);
        errFlag |= pm_comm_write_byte_mask(SLAVE_ID, int_polarity_low, mask, polarity_low, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_bsi_irq_status(uint32 device_index, pm_bsi_irq_type irq, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data;
    uint8 mask = 1 << (irq & 0x07); //irq%8 == irq &0x07
    pm_register_address_type int_sts;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_BSI_IRQ_INVALID)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        switch (type)
        {
        case PM_IRQ_STATUS_RT:
            int_sts = schg_ptr->schg_register->bsi_register->base_address + 0x10;
            break;
        case PM_IRQ_STATUS_LATCHED:
            int_sts = schg_ptr->schg_register->bsi_register->base_address + 0x18;
            break;
        case PM_IRQ_STATUS_PENDING:
            int_sts = schg_ptr->schg_register->bsi_register->base_address + 0x19;
            break;
        default:
            return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
        }

        errFlag = pm_comm_read_byte_mask(SLAVE_ID, int_sts, mask, &data, 0);
        *status = data ? TRUE : FALSE;
    }

    return errFlag;
}


pm_err_flag_type pm_bsi_set_bcl_force_low(uint32 device_index, boolean bcl_force_low)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = (bcl_force_low) ? 0x80 : 0x00;
    uint8 mask = 0x80;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type reg = ADDRESS(mipi_bif_force_bcl_low);

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, reg, mask, data, 0);
    }

    return errFlag;
}


pm_err_flag_type pm_bsi_get_status(uint32 device_index, pm_bsi_status_type *bsi_status)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {

        pm_register_address_type bsi_en = ADDRESS(bsi_en);
        pm_register_address_type mipi_bif_tau_cfg = ADDRESS(mipi_bif_tau_cfg);
        pm_register_address_type mipi_bif_mode = ADDRESS(mipi_bif_mode);
        pm_register_address_type mipi_bif_en = ADDRESS(mipi_bif_en);
        pm_register_address_type reg = ADDRESS(mipi_bif_force_bcl_low);

        /*bsi enable*/
        errFlag = pm_comm_read_byte(SLAVE_ID, bsi_en, &data, 0);
        bsi_status->bsi_enable = (data & 0x80) ? PM_BSI_CMD__ENABLE : PM_BSI_CMD__DISABLE;

        /*pm_bsi_tau_cfg_type tau_cfg / sampling rate*/
        errFlag = pm_comm_read_byte(SLAVE_ID, mipi_bif_tau_cfg, &data, 0);
        bsi_status->tau_cfg = (pm_bsi_tau_cfg_type)(data & 0x07);

        data &= 0x10;
        bsi_status->sampling_rate = (data == 0x10) ? PM_BSI_SAMPLING_RATE__8X : PM_BSI_SAMPLING_RATE__4X;

        /*pm_bsi_data_format_type rx_data_format/ tx_data_format */
        errFlag = pm_comm_read_byte(SLAVE_ID, mipi_bif_mode, &data, 0);
        bsi_status->rx_data_format = (data & 0x02) ? PM_BSI_DATA_FORMAT__17_BIT : PM_BSI_DATA_FORMAT__11_BIT;
        bsi_status->tx_data_format = (data & 0x01) ? PM_BSI_DATA_FORMAT__17_BIT : PM_BSI_DATA_FORMAT__11_BIT;

        /*rx_enable / tx_enable*/
        errFlag = pm_comm_read_byte(SLAVE_ID, mipi_bif_en, &data, 0);
        bsi_status->rx_enable = (data & 0x40) ? PM_BSI_CMD__ENABLE : PM_BSI_CMD__DISABLE;
        bsi_status->tx_enable = (data & 0x80) ? PM_BSI_CMD__ENABLE : PM_BSI_CMD__DISABLE;

        /*BCL force low*/
        errFlag = pm_comm_read_byte(SLAVE_ID, reg, &data, 0);
        bsi_status->bcl_force_low = (data & 0x80) ? TRUE : FALSE;
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_pullup_enable(uint32 device_index,  pm_bsi_cmd_type enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;

    pm_register_data_type data = 0x00;
    uint8 mask = 0x80;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bsi_en = ADDRESS(mipi_bif_cfg);

        data = (PM_BSI_CMD__ENABLE == enable) ? 0xFF : 0x00;

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, bsi_en, mask, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_fsm_reset(uint32 device_index, boolean fsm_reset, boolean rx_fifo_reset)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_data_type data = (fsm_reset << 7) | (rx_fifo_reset << 6);
        pm_register_address_type mipi_bif_fsm_reset = ADDRESS(mipi_bif_fsm_reset);

        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_fsm_reset, 0xC0, data, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_rx_cfg(uint32 device_index, boolean fifo_en, uint8 rx_irq_freq)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data;

    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_rx_cfg = ADDRESS(mipi_bif_rx_cfg);

        if (rx_irq_freq > 8)
        {
            errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
        }
        else
        {
            data = (rx_irq_freq) ? (rx_irq_freq - 1) : 0;
            data = (fifo_en << 4) | data;
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_rx_cfg, 0x1F, data, 0);
        }

    }
    return errFlag;
}

pm_err_flag_type pm_bsi_cfg_tx_dly(uint32 device_index, pm_bsi_dly_type dly_type)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else if (dly_type >= PM_MIPI_BIF__DLY_TAU_INVALID)
    {
        errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
    }
    else
    {
        pm_register_address_type mipi_bif_tx_dly = ADDRESS(mipi_bif_tx_dly);
        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_tx_dly, 0x3, (pm_register_data_type)dly_type, 0);
    }

    return errFlag;
}

pm_err_flag_type pm_bsi_get_rx_fifo_word(uint32 device_index, uint64 *words)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data1, data2, data3, data4, data5, data6, data7, data8;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type rx_fifo_word1_0 = ADDRESS(rx_fifo_word1_0);
        pm_register_address_type rx_fifo_word1_1 = ADDRESS(rx_fifo_word1_1);

        pm_register_address_type rx_fifo_word2_0 = ADDRESS(rx_fifo_word2_0);
        pm_register_address_type rx_fifo_word2_1 = ADDRESS(rx_fifo_word2_1);

        pm_register_address_type rx_fifo_word3_0 = ADDRESS(rx_fifo_word3_0);
        pm_register_address_type rx_fifo_word3_1 = ADDRESS(rx_fifo_word3_1);

        pm_register_address_type rx_fifo_word4_0 = ADDRESS(rx_fifo_word4_0);
        pm_register_address_type rx_fifo_word4_1 = ADDRESS(rx_fifo_word4_1);

        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word1_0, &data1, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word1_1, &data2, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word2_0, &data3, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word2_1, &data4, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word3_0, &data5, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word3_1, &data6, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word4_0, &data7, 0);
        errFlag = pm_comm_read_byte(SLAVE_ID, rx_fifo_word4_1, &data8, 0);

        *words = (uint64)(data1 | (data2 << 8) | (data3 << 16) | (data4 << 24) | ((uint64)data5 << 32) | ((uint64)data6 << 40) | ((uint64)data7 << 48) | ((uint64)data8 << 58));
    }

    return errFlag;
}

/*Select raw or debounced battery gone signal from analog: 0: select batt_miss_raw, 1: select batt_miss_deb */
pm_err_flag_type pm_bsi_bat_gone_deb_cfg(uint32 device_index, boolean select_batt_miss_deb)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bat_gone_cfg = ADDRESS(bat_gone_cfg);
        errFlag = pm_comm_write_byte_mask(SLAVE_ID, bat_gone_cfg, 0x1, (pm_register_data_type)select_batt_miss_deb, 0);
    }

    return errFlag;
}

/*Program battery removal debouncer timing (unit: Sleep Clock Cycle)
Programmable Range: 0 - 31 Sleep clocks
A maximum one sleep clock cycle uncertainty shall be expected on top of each programmed value.
For example:
0: 0 - 1 sleep clock
1: 1 - 2 sleep clocks
31: 31 - 32 sleep clocks
Battery detection is active only when BSI is enabled (BSI_EN=1).*/
pm_err_flag_type pm_bsi_set_batt_rmv_deb_timer(uint32 device_index, uint8 sleep_clk_cycle)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);
    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bat_rmv_deb_timer = ADDRESS(bat_rmv_deb_timer);

        if (sleep_clk_cycle > 32)
        {
            errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
        }
        else
        {
            sleep_clk_cycle = (sleep_clk_cycle) ? (sleep_clk_cycle - 1) : 0;
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, bat_rmv_deb_timer, 0x1F, (pm_register_data_type)sleep_clk_cycle, 0);
        }
    }
    return errFlag;
}

/*Program battery presence debouncer timing (unit: Sleep Clock Cycle)
Programmable Range: 0 - 31 Sleep clocks
A maximum one sleep clock cycle uncertainty shall be expected on top of each programmed value.
For example:
0: 0 - 1 sleep clock
1: 1 - 2 sleep clocks
31: 31 - 32 sleep clocks
Battery detection is active only when BSI is enabled (BSI_EN=1).*/
pm_err_flag_type pm_bsi_set_batt_pres_deb_timer(uint32 device_index, uint8 sleep_clk_cycle)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG_SUCCESS;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type bat_pres_deb_timer = ADDRESS(bat_pres_deb_timer);

        if (sleep_clk_cycle > 32)
        {
            errFlag = PM_ERR_FLAG_INVALID_PARAMETER;
        }
        else
        {
            sleep_clk_cycle = (sleep_clk_cycle) ? (sleep_clk_cycle - 1) : 0;
            errFlag = pm_comm_write_byte_mask(SLAVE_ID, bat_pres_deb_timer, 0x1F, (pm_register_data_type)sleep_clk_cycle, 0);
        }
    }

    return errFlag;
}


pm_err_flag_type pm_bsi_pullup_en(uint32 device_index,  pm_bsi_cmd_type enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data = 0x00;
    uint8 mask = 0x80;
    pm_schg_data_type *schg_ptr  = pm_schg_get_data(device_index);

    if (NULL == schg_ptr)
    {
        errFlag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type mipi_bif_cfg = ADDRESS(mipi_bif_cfg);
      
        data = (PM_BSI_CMD__ENABLE == enable) ? 0xFF : 0x00;
        errFlag = pm_comm_write_byte_mask(SLAVE_ID, mipi_bif_cfg, mask, data, 0);
    }
    
    return errFlag;
}
