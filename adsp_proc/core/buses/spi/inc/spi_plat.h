/**
    @file  spi_plat.h
    @brief platform interface
 */
/*=============================================================================
            Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __SPI_PLAT_H__
#define __SPI_PLAT_H__

#include "comdef.h"
#include "spi_config.h"

typedef enum spi_mem_attr_type
{
    ATTRIB_NONE = 0,
    ATTRIB_TRE,
    ATTRIB_BUFFER
} spi_mem_attr_type;



typedef enum spi_mem_alloc_type
{
    HW_CTXT_TYPE = 1,
    CLIENT_CTXT_TYPE,
    TX_TRANSFER_ELEM_TYPE,
    RX_TRANSFER_ELEM_TYPE,
    CRIT_SEC_TYPE,
    SIGNAL_TYPE,
    QUEUE_LOCK_TYPE,
} spi_mem_alloc_type;

// clocks, noc etc.
boolean spi_plat_clock_enable (spi_plat_device_config *config);
boolean spi_plat_clock_disable (spi_plat_device_config *config);
boolean spi_plat_clock_set_frequency (spi_plat_device_config *config, uint32 frequency_hz, uint32* dfs_index, uint32* div, boolean set_src_only);
boolean spi_plat_src_clk_reset (spi_plat_device_config *config);
boolean spi_plat_get_div_dfs (spi_plat_device_config *config, uint32 frequency_hz, uint32* dfs_index, uint32* div);

// tlmm
boolean spi_plat_gpio_enable  (spi_plat_device_config *config);
boolean spi_plat_gpio_disable (spi_plat_device_config *config);

// interrupt
boolean spi_plat_interrupt_register (spi_plat_device_config *config, void (*isr) (void *), void *handle);
boolean spi_plat_interrupt_deregister (spi_plat_device_config *config);

// for async apis where callbacks are called from interrupt context
boolean spi_plat_in_exception_context (void);
void spi_plat_int_lock (void);
void spi_plat_int_unlock (void);

// for synchronization
void spi_plat_mutex_global_lock (void);
void spi_plat_mutex_global_unlock (void);

void *spi_plat_mutex_instance_init (uint8 core_index, spi_mem_alloc_type t);
void spi_plat_mutex_instance_lock (void *lock);
void spi_plat_mutex_instance_unlock (void *lock);
void spi_plat_mutex_instance_deinit (void *lock);

// signals
void *spi_plat_signal_init(uint8 core_index);
void spi_plat_signal_deinit(void *signal);
void spi_plat_wait_for_event (void **signal);
void spi_plat_signal_event (void *signal);

// config
spi_plat_device_config *spi_plat_get_device_config (uint8 core_index);
int32 spi_plat_get_index_in_device_config_array (uint8 core_index);

// memory
void *spi_plat_mem_alloc (uint8 core_index, uint32 size, spi_mem_alloc_type ctxt);
void  spi_plat_mem_free  (void *ptr, uint32 size, spi_mem_alloc_type ctxt);
boolean spi_plat_mem_flush_cache      (void *addr, uint32 size);
boolean spi_plat_mem_invalidate_cache (void *addr, uint32 size);

// delay
void spi_plat_delay_us (uint32 us);

void *spi_plat_virt_to_phys (void *ptr, spi_mem_attr_type attr);

#endif /*__SPI_PLAT_H__*/
