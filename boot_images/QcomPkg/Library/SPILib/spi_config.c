/** 
    @file  SPI_config.c
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_plat.h"
#include "DALSys.h"
#include "SpiApi.h"
#include "spi_log.h"    // logging
#include "spi_api.h"

spi_plat_device_config *spi_plat_get_device_config (uint8 core_index)
{
    DALSYS_PROPERTY_HANDLE_DECLARE (prop_handle);
    DALSYSPropertyVar               prop_var;
    DALResult                       result = DAL_SUCCESS;
    spi_plat_device_config              *dcfg  = NULL;
    
    char dev_name[12] = "/dev/SPI";
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
    dcfg = (spi_plat_device_config *) prop_var.Val.pStruct;

	SPI_LOG(LEVEL_INFO, "[SPI] core_base_addr           = 0x%08x\n", dcfg->core_base_addr);
    SPI_LOG(LEVEL_INFO, "[SPI] common_base_addr         = 0x%08x\n", dcfg->common_base_addr);
	SPI_LOG(LEVEL_INFO, "[SPI] core_offset              = 0x%08x\n", dcfg->core_offset);
    SPI_LOG(LEVEL_INFO, "[SPI] qupv3_instance           = 0x%08x\n", dcfg->qupv3_instance);
    SPI_LOG(LEVEL_INFO, "[SPI] core_index               = 0x%08x\n", dcfg->core_index);
    SPI_LOG(LEVEL_INFO, "[SPI] core_irq                 = 0x%08x\n", dcfg->core_irq);
    SPI_LOG(LEVEL_INFO, "[SPI] polled_mode              = 0x%08x\n", dcfg->polled_mode);
    SPI_LOG(LEVEL_INFO, "[SPI] min_data_length_for_dma  = 0x%08x\n", dcfg->min_data_length_for_dma);
    SPI_LOG(LEVEL_INFO, "[SPI] gpi_index                = 0x%08x\n", dcfg->gpi_index);
    SPI_LOG(LEVEL_INFO, "[SPI] tcsr_base_addr           = 0x%08x\n", dcfg->tcsr_base_addr);
    SPI_LOG(LEVEL_INFO, "[SPI] tcsr_reg_offset          = 0x%08x\n", dcfg->tcsr_reg_offset);
    SPI_LOG(LEVEL_INFO, "[SPI] tcsr_reg_value           = 0x%08x\n", dcfg->tcsr_reg_value);
    SPI_LOG(LEVEL_INFO, "[SPI] se_clock                 = %s\n",     dcfg->se_clock);
    SPI_LOG(LEVEL_INFO, "[SPI] clock_config             = 0x%08x\n", dcfg->clock_config);
	
ON_EXIT:
    return dcfg;
}

void plat_release_device_config (uint8 core_index, spi_plat_device_config *dcfg)
{
}

