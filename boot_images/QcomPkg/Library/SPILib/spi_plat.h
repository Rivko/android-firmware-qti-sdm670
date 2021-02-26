/*=============================================================================   
    @file  spi_plat.h
    @brief SPI Platform interface implementation
   
    Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/09/19   bng     initial version
=============================================================================*/

#ifndef __SPI_PLAT_H__
#define __SPI_PLAT_H__

#include "spi_types.h"
#include <spi_config.h>

typedef enum mem_attr_type
{
    ATTRIB_NONE = 0,
    ATTRIB_TRE,
    ATTRIB_BUFFER

} mem_attr_type;

typedef enum mem_alloc_type
{
    HW_CTXT_TYPE = 1,
    CORE_IFACE_TYPE,
    CLIENT_CTXT_TYPE,
    TX_TRANSFER_ELEM_TYPE,
    RX_TRANSFER_ELEM_TYPE,
    QUEUE_LOCK_TYPE,
    CRIT_SEC_TYPE,
    SIGNAL_TYPE,
} mem_alloc_type;

// clocks, noc etc.
boolean spi_plat_clock_enable (spi_plat_device_config *config);
boolean spi_plat_clock_disable (spi_plat_device_config *config);

// gpio
boolean spi_plat_gpio_enable (spi_plat_device_config *config);
boolean spi_plat_gpio_disable (spi_plat_device_config *config);

//malloc
void *spi_plat_sys_mem_alloc (uint32 size);
void spi_plat_sys_mem_dealloc (void* ptr);

// interrupt
boolean spi_plat_interrupt_register (spi_plat_device_config *config, void (*isr) (void *), void *handle);
boolean spi_plat_interrupt_deregister (spi_plat_device_config *config);

// tcsr
boolean spi_plat_tcsr_init (spi_plat_device_config *config);
boolean spi_plat_tcsr_deinit (spi_plat_device_config *config);

// for async apis where callbacks are called from interrupt context
boolean spi_plat_in_exception_context (void);
void spi_plat_int_lock (void);
void spi_plat_int_unlock (void);

// for synchronization
void spi_plat_mutex_global_lock (void);
void spi_plat_mutex_global_unlock(void);

void *spi_plat_mutex_instance_init (uint8 core_index,mem_alloc_type type);
void spi_plat_mutex_instance_lock (void *lock);

void spi_plat_mutex_instance_unlock (void *lock);
void spi_plat_mutex_instance_de_init (void *lock);

// signals
void *spi_plat_signal_alloc (uint8 core_index);
void *spi_plat_signal_init (int core_index);
void spi_plat_wait_for_event (void *signal);
void spi_plat_signal_event (void *signal);
void spi_plat_signal_free (void *signal);

// config
spi_plat_device_config *spi_plat_get_device_config (uint8 core_index);
int32 spi_plat_get_index_in_device_config_array (uint8 core_index);

// memory
void *spi_plat_mem_alloc (uint8 core_index, uint32 size, mem_alloc_type ctxt);
void  spi_plat_mem_free  (void *ptr, uint32 size, mem_alloc_type ctxt);
boolean spi_plat_mem_flush_cache       (void *addr, uint32 size);
boolean spi_plat_mem_invalidate_cache  (void *addr, uint32 size);

// delay
void spi_plat_delay_us (uint32 us);

void *plat_virt_to_phys (void *ptr, mem_attr_type attr);

boolean spi_plat_clock_set_frequency
(
   spi_plat_device_config *config,
   uint32 frequency_hz,
   uint32* dfs_index,
   uint32* div,
   boolean set_src_clk
);
#endif
