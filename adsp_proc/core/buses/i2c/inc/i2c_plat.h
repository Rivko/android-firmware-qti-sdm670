/** 
    @file  i2c_plat.h
    @brief platform interface
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __I2C_PLAT_H__
#define __I2C_PLAT_H__

#include "i2c_types.h"
#include "i2c_config.h"

typedef enum mem_attr_type
{
    ATTRIB_NONE = 0,
    ATTRIB_TRE,
    ATTRIB_BUFFER

} mem_attr_type;

typedef enum exec_level_type
{
    EL_THREAD = 0,
    EL_INTERRUPT,

} exec_level_type;

typedef enum sync_type
{
    SYNC_WITH_THREAD = 0,
    SYNC_WITH_INTERRUPT,

} sync_type;

typedef enum mem_alloc_type
{
    HW_CTXT_TYPE            = 1,
    CORE_IFACE_TYPE         = 2,
    CLIENT_CTXT_TYPE        = 3,
    TX_TRANSFER_ELEM_TYPE   = 4,
    RX_TRANSFER_ELEM_TYPE   = 5,
    IO_CTXT_TYPE            = 6,
    CRIT_SEC_TYPE           = 7,
    Q_MUTEX_TYPE            = 8,
    SIGNAL_TYPE             = 9,

} mem_alloc_type;

// clocks, noc etc.
boolean plat_setup_se_clock (plat_device_config *config, uint32 bus_frequency_khz);
boolean plat_unset_se_clock (plat_device_config *config);
boolean plat_clock_enable (plat_device_config *config);
boolean plat_clock_disable (plat_device_config *config);
uint32 plat_clock_get_config (plat_device_config *config, uint32 bus_frequency_khz,
                              uint8 *dfs_index, uint8 *divider, uint8 *t_cycle, uint8 *t_low,
                              uint8 *t_high, uint32 *clock_period_ns);

// gpio
boolean plat_gpio_enable (plat_device_config *config);
boolean plat_gpio_disable (plat_device_config *config);

// interrupt
boolean plat_interrupt_register (plat_device_config *config, void (*isr) (void *), void *handle);
boolean plat_interrupt_deregister (plat_device_config *config);

// tcsr
boolean plat_tcsr_init (plat_device_config *config);
boolean plat_tcsr_deinit (plat_device_config *config);

// for async apis where callbacks are called from interrupt context
boolean plat_in_exception_context (void);
void plat_int_lock (void);
void plat_int_unlock (void);

// for synchronization
void plat_mutex_global_lock (void);
void plat_mutex_global_unlock (void);

void *plat_mutex_instance_init (uint8 core_index, mem_alloc_type type);
void plat_mutex_instance_lock (void *lock, exec_level_type type, sync_type sync);
void plat_mutex_instance_unlock (void *lock, exec_level_type type, sync_type sync);
void plat_mutex_instance_de_init (void *lock, mem_alloc_type type);

// signals
void *plat_signal_alloc (uint8 core_index);
void plat_signal_free (void *signal);

void plat_signal_init (void **signal);
void plat_signal_de_init (void **signal);

void plat_wait_for_event (void *signal);
void plat_signal_event (void *signal);

// config
plat_device_config *plat_get_device_config (uint8 core_index);
void plat_release_device_config (uint8 core_index, plat_device_config *dcfg);

// memory
void    *plat_mem_alloc (uint8 core_index, uint32 size, mem_alloc_type type);
void    plat_mem_free (void *ptr, uint32 size, mem_alloc_type type);

boolean plat_mem_flush_cache (void *addr, uint32 size, mem_attr_type attr);
boolean plat_mem_invalidate_cache (void *addr, uint32 size, mem_attr_type attr);

void    *plat_virt_to_phys (void *ptr, mem_attr_type attr);

// delay
void plat_delay_us (uint32 us);

#endif
