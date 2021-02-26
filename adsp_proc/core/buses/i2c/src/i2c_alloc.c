/**
    @file  i2c_alloc.c
    @brief abstraction for static or dynamic allocation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_plat.h"
#include "i2c_iface.h"
#include "i2c_log.h"
#include "qurt.h"
#include "qup_common.h"

#define NUM_I2C_CLOCK_CONFIG 8

extern bus_iface_hw_ctxt    bus_gpi_hw_ctxt[];
extern hw_ctxt              bus_hw_ctxt[];
extern qurt_mutex_t         bus_crit_sec[];
extern qurt_mutex_t         bus_q_mutex[];
extern qurt_signal_t        bus_signal[];
extern client_ctxt          bus_client_ctxt[];
extern bus_iface_io_ctxt    bus_gpi_io_ctxt[];

static plat_device_config   *i2c_device_config = NULL;
static uint32               num_i2c_cores_in_use = 0;
static uint32               num_i2c_clients_in_use = 0;

int32 plat_get_index_in_device_config_array (uint8 core_index)
{
    int32 i = 0;

    if (i2c_device_config == NULL)
    {
        i2c_device_config = (plat_device_config *) plat_get_device_config_base ();
        num_i2c_cores_in_use = plat_get_num_initialized_cores ();
        num_i2c_clients_in_use = plat_get_num_clients ();
    }
    if ((i2c_device_config == NULL) ||
        (num_i2c_cores_in_use == 0) ||
        (num_i2c_clients_in_use == 0))
    {
        return -1;
    }

    while (i < num_i2c_cores_in_use)
    {
        if (i2c_device_config[i].core_index == core_index)
        {
            return i;
        }
        i++;
    }

    return -1;
}

void plat_release_device_config (uint8 core_index, plat_device_config *dcfg)
{
}

plat_device_config *plat_get_device_config (uint8 core_index)
{
    int32 i = -1;
    int32 count = NUM_I2C_CLOCK_CONFIG;
    plat_device_config *dcfg = NULL;
    plat_clock_config  *temp = NULL;

    i = plat_get_index_in_device_config_array (core_index);
    if (i < 0) { return NULL; }

    dcfg = &i2c_device_config[i];

    I2C_LOG(LEVEL_VERBOSE, "core_base_addr   = 0x%08x", dcfg->core_base_addr);
    I2C_LOG(LEVEL_VERBOSE, "core_offset      = 0x%08x", dcfg->core_offset);
    I2C_LOG(LEVEL_VERBOSE, "qupv3_instance   = 0x%08x", dcfg->qupv3_instance);
    I2C_LOG(LEVEL_VERBOSE, "core_index       = 0x%08x", dcfg->core_index);
    I2C_LOG(LEVEL_VERBOSE, "core_irq         = 0x%08x", dcfg->core_irq);
    I2C_LOG(LEVEL_VERBOSE, "polled_mode      = 0x%08x", dcfg->polled_mode);
    I2C_LOG(LEVEL_VERBOSE, "dma_threshold    = 0x%08x", dcfg->min_data_length_for_dma);
    I2C_LOG(LEVEL_VERBOSE, "gpi_index        = 0x%08x", dcfg->gpi_index);
    I2C_LOG(LEVEL_VERBOSE, "scl_encoding     = 0x%08x", dcfg->active_scl_encoding);
    I2C_LOG(LEVEL_VERBOSE, "sda_encoding     = 0x%08x", dcfg->active_sda_encoding);
    I2C_LOG(LEVEL_VERBOSE, "tcsr_base_addr   = 0x%08x", dcfg->tcsr_base_addr);
    I2C_LOG(LEVEL_VERBOSE, "tcsr_reg_offset  = 0x%08x", dcfg->tcsr_reg_offset);
    I2C_LOG(LEVEL_VERBOSE, "tcsr_reg_value   = 0x%08x", dcfg->tcsr_reg_value);
    I2C_LOG(LEVEL_VERBOSE, "se_clock         = %s",     *(dcfg->se_clock));
    I2C_LOG(LEVEL_VERBOSE, "clock_config     = 0x%08x", dcfg->clock_config);

    temp = dcfg->clock_config;
    if (temp == NULL) { return NULL; }

    while ((temp->se_clock_frequency_khz != 0) && count)
    {
        I2C_LOG(LEVEL_VERBOSE, "ccfg se_clock    = %d", temp->se_clock_frequency_khz);
        I2C_LOG(LEVEL_VERBOSE, "ccfg bus_speed   = %d", temp->bus_speed_khz);
        I2C_LOG(LEVEL_VERBOSE, "ccfg clk_div     = %d", temp->clk_div);
        I2C_LOG(LEVEL_VERBOSE, "ccfg t_cycle     = %d", temp->t_cycle);
        I2C_LOG(LEVEL_VERBOSE, "ccfg t_high      = %d", temp->t_high);
        I2C_LOG(LEVEL_VERBOSE, "ccfg t_low       = %d", temp->t_low);
        temp++;
        count--;
    }

    return dcfg;
}

void *plat_mem_alloc (uint8 core_index, uint32 size, mem_alloc_type ctxt)
{
    int32 index         = 0;
    uint32 cl_index     = 0;
    uint32 sig_index    = 0;

    void *ptr = NULL;

    index = plat_get_index_in_device_config_array (core_index);
    if (index < 0) { return NULL; }

    if (ctxt == HW_CTXT_TYPE)
    {
        ptr = &bus_hw_ctxt[index];
    }
    else if (ctxt == CORE_IFACE_TYPE)
    {
        ptr = &bus_gpi_hw_ctxt[index];
    }
    else if (ctxt == CLIENT_CTXT_TYPE)
    {
        while (cl_index < num_i2c_clients_in_use)
        {
            if (bus_client_ctxt[cl_index].h_ctxt == NULL)
            {
                ptr = &bus_client_ctxt[cl_index];
                break;
            }
            cl_index++;
        }
    }
    else if (ctxt == IO_CTXT_TYPE)
    {
        while (cl_index < num_i2c_clients_in_use)
        {
            if (bus_gpi_io_ctxt[cl_index].allocated == 0)
            {
                bus_gpi_io_ctxt[cl_index].allocated = 1;
                ptr = &bus_gpi_io_ctxt[cl_index];
                break;
            }
            cl_index++;
        }
    }
    else if (ctxt == TX_TRANSFER_ELEM_TYPE)
    {
        ptr = qup_common_pram_tre_malloc(SE_PROTOCOL_I2C, size);
    }
    else if (ctxt == RX_TRANSFER_ELEM_TYPE)
    {
        ptr = qup_common_pram_tre_malloc(SE_PROTOCOL_I2C, size);
    }
    else if (ctxt == CRIT_SEC_TYPE)
    {
        ptr = &bus_crit_sec[index];
    }
    else if (ctxt == Q_MUTEX_TYPE)
    {
        ptr = &bus_q_mutex[index];
    }
    else if (ctxt == SIGNAL_TYPE)
    {
        while (sig_index < (num_i2c_cores_in_use + num_i2c_clients_in_use))
        {
            if (bus_signal[sig_index].X.queue == 0)
            {
                ptr = &bus_signal[sig_index];
                qurt_signal_init(ptr);
                break;
            }
            sig_index++;
        }
    }

    I2C_LOG(LEVEL_VERBOSE, "alloc ptr 0x%08x size %d", ptr, size);

    return ptr;
}

void plat_mem_free (void *ptr, uint32 size, mem_alloc_type ctxt)
{
    uint8 *mem = (uint8 *) ptr;
    uint32 i;

    if ((ctxt == TX_TRANSFER_ELEM_TYPE) ||
        (ctxt == RX_TRANSFER_ELEM_TYPE))
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

    I2C_LOG(LEVEL_VERBOSE, "free ptr 0x%08x size %d", ptr, size);
}
