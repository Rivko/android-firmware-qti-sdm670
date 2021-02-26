/** 
    @file  spi_devcfg.c
    @brief device configuration implementation
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_plat.h"   // implement configuration specific functions
#include "spi_devcfg.h" // include target specific config
#include "pram_mgr.h"
#include "qup_common.h"

spi_plat_device_config *spi_plat_get_device_config (uint8 core_index)
{
    uint32 i = 0;

    while (i < NUM_SPI_CORES_IN_USE)
    {
        if (spi_device_config[i].core_index == core_index)
        {
            return &spi_device_config[i];
        }
        i++;
    }

    return NULL;
}

int32 spi_plat_get_index_in_device_config_array (uint8 core_index)
{
    int32 i = 0;

    while (i < NUM_SPI_CORES_IN_USE)
    {
        if (spi_device_config[i].core_index == core_index)
        {
            return i;
        }
        i++;
    }

    return -1;
}

void *spi_plat_mem_alloc (uint8 core_index, uint32 size, spi_mem_alloc_type ctxt)
{
    int32 index = 0, cl_index = 0;

    index = spi_plat_get_index_in_device_config_array (core_index);
    if (index < 0)
    {
        return NULL;
    }

    if (ctxt == HW_CTXT_TYPE)
    {
        spi_bus_hw_ctxt[index].core_iface = &spi_bus_gpi_ctxt[index];
        return &spi_bus_hw_ctxt[index];
    }
    else if (ctxt == CLIENT_CTXT_TYPE)
    {
        while (cl_index < (NUM_SPI_CORES_IN_USE * NUM_SPI_CLIENTS) )
        {
            if (spi_bus_client_ctxt[cl_index].h_ctxt == NULL)
            {
                spi_bus_client_ctxt[cl_index].t_ctxt.io_ctxt = &spi_bus_gpi_io_ctxt[cl_index];
                return &spi_bus_client_ctxt[cl_index];
            }
            cl_index++;
        }
    }
    else if (ctxt == TX_TRANSFER_ELEM_TYPE)
    {
        spi_plat_device_config *dcfg = (spi_plat_device_config *) spi_bus_hw_ctxt[index].core_config;
        //return dcfg->tx_dma_ring_base;

        dcfg->tx_dma_ring_base = qup_common_pram_tre_malloc(SE_PROTOCOL_SPI, size);
        return dcfg->tx_dma_ring_base;
        //if it returns phys addr
        //dcfg->tx_dma_ring_base = pram_mgr_lookup_phys_addr_by_name("BUSES", (qurt_addr_t)pram_addr);
        //return dcfg->tx_dma_ring_base;
    }
    else if (ctxt == RX_TRANSFER_ELEM_TYPE)
    {
        spi_plat_device_config *dcfg = (spi_plat_device_config *) spi_bus_hw_ctxt[index].core_config;
        //return dcfg->rx_dma_ring_base;
        dcfg->rx_dma_ring_base = qup_common_pram_tre_malloc(SE_PROTOCOL_SPI, size);;
        return dcfg->rx_dma_ring_base;
    }
    else if (ctxt == CRIT_SEC_TYPE)
    {
        return &spi_bus_crit_sec[index];
    }
    else if (ctxt == QUEUE_LOCK_TYPE)
    {
        return &spi_bus_queue_lock[index];
    }

    else if (ctxt == SIGNAL_TYPE)
    {
        return &spi_bus_signal[index];
    }

    return NULL;
}

void spi_plat_mem_free (void *ptr, uint32 size, spi_mem_alloc_type ctxt)
{
    uint8 *mem = (uint8 *) ptr;
    uint32 i;

    if (ctxt == TX_TRANSFER_ELEM_TYPE)
    {
         qup_common_pram_tre_free(ptr);
    }
    else if (ctxt == RX_TRANSFER_ELEM_TYPE)
    {
         qup_common_pram_tre_free(ptr);
    }
    else if (mem != NULL)
    {
        for (i = 0; i < size; i++)
        {
            mem[i] = 0;
        }
    }
}
