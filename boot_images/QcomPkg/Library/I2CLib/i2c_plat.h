/*=============================================================================   
    @file  i2c_plat.h
    @brief I2C Platform interface implementation
   
    Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 31/05/17   vg      Copyright updated 
=============================================================================*/

#ifndef __I2C_PLAT_H__
#define __I2C_PLAT_H__

#include "i2c_types.h"
#include <i2c_config.h>

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
    Q_MUTEX_TYPE,
    CRIT_SEC_TYPE,
    SIGNAL_TYPE,

} mem_alloc_type;

// clocks, noc etc.
boolean plat_clock_enable (plat_device_config *config);
boolean plat_clock_disable (plat_device_config *config);

// gpio
boolean plat_gpio_enable (plat_device_config *config);
boolean plat_gpio_disable (plat_device_config *config);

//malloc
void *plat_sys_mem_alloc (uint32 size);
void plat_sys_mem_dealloc (void* ptr);

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

void *plat_mutex_instance_init (uint8 core_index);
void plat_mutex_instance_lock (void *lock);

void plat_mutex_instance_unlock (void *lock);
void plat_mutex_instance_de_init (void *lock);

// signals
void *plat_signal_alloc (uint8 core_index);
void plat_signal_init (void **signal);
void plat_wait_for_event (void *signal);
void plat_signal_event (void *signal);
void plat_signal_free (void *signal);

// config
plat_device_config *plat_get_device_config (uint8 core_index);
int32 plat_get_index_in_device_config_array (uint8 core_index);

// memory
void *plat_mem_alloc (uint8 core_index, uint32 size, mem_alloc_type ctxt);
void  plat_mem_free  (void *ptr, uint32 size, mem_alloc_type ctxt);
boolean plat_mem_flush_cache       (void *addr, uint32 size);
boolean plat_mem_invalidate_cache  (void *addr, uint32 size);

// delay
void plat_delay_us (uint32 us);

void *plat_virt_to_phys (void *ptr, mem_attr_type attr);

#endif
