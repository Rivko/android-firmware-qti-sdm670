/** 
    @file  i2c_config.c
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_plat.h"
#include "DALSys.h"
#include "i2c_api.h"
#include "i2c_log.h"    // logging

plat_device_config *plat_get_device_config (uint8 core_index)
{
    DALSYS_PROPERTY_HANDLE_DECLARE (prop_handle);
    DALSYSPropertyVar               prop_var;
    DALResult                       result = DAL_SUCCESS;
    plat_device_config              *dcfg  = NULL;
    
    char dev_name[12] = "/dev/i2c";
    if (core_index > 9)
    {
        dev_name[8] = (char) (0x30 + (core_index / 10));
        dev_name[9] = (char) (0x30 + (core_index % 10));
    }
    else
    {
        dev_name[8] = (char) (0x30 + core_index);
    }

    result = DALSYS_GetDALPropertyHandleStr(dev_name, prop_handle);
    if (result != DAL_SUCCESS)
    {
        goto ON_EXIT;
    }

    result = DALSYS_GetPropertyValue(prop_handle, "config", 0, &prop_var);
    if (result != DAL_SUCCESS)
    {
        goto ON_EXIT;
    }
    dcfg = (plat_device_config *) prop_var.Val.pStruct;

	I2C_LOG(LEVEL_INFO, "[I2C] core_base_addr           = 0x%08x\n", dcfg->core_base_addr);
    I2C_LOG(LEVEL_INFO, "[I2C] common_base_addr         = 0x%08x\n", dcfg->common_base_addr);
	I2C_LOG(LEVEL_INFO, "[I2C] core_offset              = 0x%08x\n", dcfg->core_offset);
    I2C_LOG(LEVEL_INFO, "[I2C] qupv3_instance           = 0x%08x\n", dcfg->qupv3_instance);
    I2C_LOG(LEVEL_INFO, "[I2C] core_index               = 0x%08x\n", dcfg->core_index);
    I2C_LOG(LEVEL_INFO, "[I2C] core_irq                 = 0x%08x\n", dcfg->core_irq);
    I2C_LOG(LEVEL_INFO, "[I2C] polled_mode              = 0x%08x\n", dcfg->polled_mode);
    I2C_LOG(LEVEL_INFO, "[I2C] min_data_length_for_dma  = 0x%08x\n", dcfg->min_data_length_for_dma);
    I2C_LOG(LEVEL_INFO, "[I2C] gpi_index                = 0x%08x\n", dcfg->gpi_index);
    I2C_LOG(LEVEL_INFO, "[I2C] scl_encoding             = 0x%08x\n", dcfg->scl_encoding);
    I2C_LOG(LEVEL_INFO, "[I2C] sda_encoding             = 0x%08x\n", dcfg->sda_encoding);
    I2C_LOG(LEVEL_INFO, "[I2C] tcsr_base_addr           = 0x%08x\n", dcfg->tcsr_base_addr);
    I2C_LOG(LEVEL_INFO, "[I2C] tcsr_reg_offset          = 0x%08x\n", dcfg->tcsr_reg_offset);
    I2C_LOG(LEVEL_INFO, "[I2C] tcsr_reg_value           = 0x%08x\n", dcfg->tcsr_reg_value);
    I2C_LOG(LEVEL_INFO, "[I2C] se_clock                 = %s\n",     dcfg->se_clock);
    I2C_LOG(LEVEL_INFO, "[I2C] clock_config             = 0x%08x\n", dcfg->clock_config);
	
ON_EXIT:
    return dcfg;
}

void plat_release_device_config (uint8 core_index, plat_device_config *dcfg)
{
}

