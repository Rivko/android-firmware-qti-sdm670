/**
    @file  i2c_plat.c
    @brief platform implementation
 */
/*=============================================================================
            Copyright (c) 2016-2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "i2c_plat.h"
#include "i2c_log.h"
#include "busywait.h"
#include "uTlmm.h"
#include "uClock.h"
#include "DDIClock.h"
#include "ClockDefs.h"
#include "DALDeviceId.h"
#include "qurt.h"
#include "qurt_island.h"
#include <stringl/stringl.h>
#include EXT_DEP_HEADER

#define PLAT_SIGNAL_GENERIC_SIGNAL      0x1
#define PLAT_SIGNAL_GENERIC_WAIT_ALL    0x1

#define MAX_FREQ_PLAN_TABLE_SIZE    8

static qurt_mutex_t         global_cs;
static DalDeviceHandle      *clock_handle   = NULL;
static ClockFreqPlanType    *freq_list      = NULL;
static ClockFreqPlanType    freq_list_local[MAX_FREQ_PLAN_TABLE_SIZE];
static uint32               num_entries;

// clocks, noc etc.
// note that this will get called from big image for sure, since this is in the
// path of setting up the clock/voltage resources for big/sdc image
uint32 plat_clock_get_config
(
    plat_device_config *config,
    uint32 bus_frequency_khz,
    uint8  *dfs_index,
    uint8  *divider,
    uint8  *t_cycle,
    uint8  *t_low,
    uint8  *t_high,
    uint32 *clock_period_ns
)
{
    boolean found;
    uint32 i = 0;
    uint32 src_frequency_hz = 0;
    plat_clock_config *clk_cfg = config->clock_config;

    found = FALSE;
    while (clk_cfg[i].bus_speed_khz != 0)
    {
        if (clk_cfg[i].bus_speed_khz == bus_frequency_khz)
        {
            src_frequency_hz = clk_cfg[i].se_clock_frequency_khz * 1000;
            if (divider != NULL) { *divider = clk_cfg[i].clk_div; }
            if (t_cycle != NULL) { *t_cycle = clk_cfg[i].t_cycle; }
            if (t_high  != NULL) { *t_high  = clk_cfg[i].t_high;  }
            if (t_low   != NULL) { *t_low   = clk_cfg[i].t_low;   }

            found = TRUE;
            break;
        }
        i++;
    }
    if (!found) { return 0; }

    found = FALSE;
    for (i = 0; i < num_entries; i++)
    {
        if (freq_list_local[i].nFreqHz == src_frequency_hz)
        {
            if (dfs_index != NULL) { *dfs_index = i; }

            found = TRUE;
            break;
        }
    }
    if (!found) { return 0; }

    if ((clock_period_ns != NULL) && (divider != NULL) && (*divider != 0))
    { *clock_period_ns = 1000000000/(src_frequency_hz/(*divider)); }

    return src_frequency_hz;
}

boolean plat_setup_se_clock (plat_device_config *config, uint32 bus_frequency_khz)
{
    DALResult res;
    ClockIdType se_id;
    const char *clock_str = (const char *) (*(config->se_src_clock));
    uint32 frequency_match_hz;

    if (qurt_island_get_status ())
    {
        I2C_LOG(LEVEL_ERROR, "ERROR: setup clock called in island %s", clock_str);
        return FALSE;
    }

    if (clock_handle == NULL) { return FALSE; }

    I2C_LOG(LEVEL_VERBOSE, "req bus freq = %d Hz", bus_frequency_khz);

    res = DalClock_GetClockId(clock_handle, clock_str, &se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    if (freq_list == NULL)
    {
        size_t copied = 0;
        res = DalClock_GetFrequencyPlan(clock_handle, se_id, &freq_list, &num_entries);
        if (DAL_SUCCESS != res) { return FALSE; }

        copied = memscpy(freq_list_local, sizeof(freq_list_local), freq_list, num_entries * sizeof(ClockFreqPlanType));
        if (copied != num_entries * sizeof(ClockFreqPlanType)) { return FALSE; }
    }

    frequency_match_hz = plat_clock_get_config (config, bus_frequency_khz, NULL, NULL, NULL, NULL, NULL, NULL);
    if (!frequency_match_hz) { return FALSE; }

    res = DalClock_SetClockFrequency(clock_handle, se_id, frequency_match_hz, CLOCK_FREQUENCY_HZ_EXACT, NULL);
    if (DAL_SUCCESS != res) { return FALSE; }

    res = DalClock_EnableClock(clock_handle, se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    config->resource_votes++;

    I2C_LOG(LEVEL_VERBOSE, "req src freq = %d Hz", frequency_match_hz);

    return TRUE;
}

boolean plat_unset_se_clock (plat_device_config *config)
{
    DALResult res;
    ClockIdType se_id;
    const char *clock_str = (const char *) (*(config->se_src_clock));

    if (qurt_island_get_status ())
    {
        I2C_LOG(LEVEL_ERROR, "ERROR: unset clock called in island %s", clock_str);
        return FALSE;
    }

    if (clock_handle == NULL) { return FALSE; }

    res = DalClock_GetClockId(clock_handle, clock_str, &se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    res = DalClock_DisableClock(clock_handle, se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    if (config->resource_votes) { config->resource_votes--; }

    I2C_LOG(LEVEL_VERBOSE, "se clock unset %s", clock_str);

    return TRUE;
}

boolean plat_clock_enable (plat_device_config *config)
{
    I2C_LOG(LEVEL_VERBOSE, "clock_ON ALL");
    return uClock_EnableClock(CLOCK_SCC_QUPV3_CORE_CLK)   &&
           uClock_EnableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
           uClock_EnableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
           uClock_EnableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK) &&
           uClock_EnableClock((uClockIdType)(config->core_index - 1));
}

boolean plat_clock_disable (plat_device_config *config)
{
    I2C_LOG(LEVEL_VERBOSE, "clock_OFF ALL");
    return uClock_DisableClock((uClockIdType)(config->core_index - 1)) &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK) &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_CORE_CLK);
}

// gpio
boolean plat_gpio_enable (plat_device_config *config)
{
    return  uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->active_scl_encoding, UTLMM_GPIO_ENABLE) &&
            uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->active_sda_encoding, UTLMM_GPIO_ENABLE);
}

boolean plat_gpio_disable (plat_device_config *config)
{
    if (config->gpio_exclusive_access)
    {
        return  uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->inactive_scl_encoding, UTLMM_GPIO_ENABLE) &&
                uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->inactive_sda_encoding, UTLMM_GPIO_ENABLE);
    }

    return TRUE;
}

// interrupt
boolean plat_interrupt_register (plat_device_config *config, void (*isr) (void *), void *handle)
{
    return TRUE;
}

boolean plat_interrupt_deregister (plat_device_config *config)
{
    return TRUE;
}

// tcsr
boolean plat_tcsr_init (plat_device_config *config)
{
    return TRUE;
}

boolean plat_tcsr_deinit (plat_device_config *config)
{
    return TRUE;
}

// memory
void *plat_virt_to_phys (void *ptr, mem_attr_type attr)
{
    if (attr == ATTRIB_TRE)
    {
        return (void *) pram_mgr_lookup_phys_addr_by_name ("BUSES", (qurt_addr_t) ptr);
    }

    return ptr;
}

boolean plat_mem_flush_cache (void *addr, uint32 size, mem_attr_type attr)
{
    return TRUE;
}

boolean plat_mem_invalidate_cache (void *addr, uint32 size, mem_attr_type attr)
{
    return TRUE;
}

// utils
void plat_delay_us (uint32 us)
{
    busywait (us);
}

// signals
void *plat_signal_alloc (uint8 core_index)
{
    return plat_mem_alloc (core_index, sizeof(qurt_signal_t), SIGNAL_TYPE);
}

void plat_signal_free (void *signal)
{
    plat_mem_free (signal, sizeof(qurt_signal_t), SIGNAL_TYPE);
}

void plat_signal_init(void **signal)
{
    qurt_signal_init(*signal);
}

void plat_signal_de_init(void **signal)
{
    if (*signal != NULL)
    {
        qurt_signal_destroy(*signal);
    }
}

void plat_wait_for_event (void *signal)
{
  unsigned int mask = 0;
  if (QURT_EOK == qurt_signal_wait_cancellable (signal,
              PLAT_SIGNAL_GENERIC_SIGNAL,
              PLAT_SIGNAL_GENERIC_WAIT_ALL,
              &mask))
  {
      qurt_signal_clear(signal,
              PLAT_SIGNAL_GENERIC_SIGNAL);
  }
}

void plat_signal_event (void *signal)
{
    qurt_signal_set(signal, PLAT_SIGNAL_GENERIC_SIGNAL);
}

// critical section for specific instance
void *plat_mutex_instance_init (uint8 core_index, mem_alloc_type type)
{
    void *cs = plat_mem_alloc (core_index, sizeof(qurt_mutex_t), type);

    if (cs != NULL)
    {
        qurt_mutex_init(cs);
    }

    return cs;
}

void plat_mutex_instance_de_init (void *lock, mem_alloc_type type)
{
    if (lock != NULL)
    {
        qurt_mutex_destroy(lock);
        plat_mem_free (lock, sizeof(qurt_mutex_t), type);
    }
}

void plat_mutex_instance_lock (void *lock, exec_level_type type, sync_type sync)
{
    if (type == EL_THREAD)
    {
        if (sync == SYNC_WITH_INTERRUPT)
        {
            // INTLOCK()
        }
        else if (sync == SYNC_WITH_THREAD)
        {
            qurt_mutex_lock (lock);
        }
    }
}

void plat_mutex_instance_unlock (void *lock, exec_level_type type, sync_type sync)
{
    if (type == EL_THREAD)
    {
        if (sync == SYNC_WITH_INTERRUPT)
        {
            // INTFREE()
        }
        else if (sync == SYNC_WITH_THREAD)
        {
            qurt_mutex_unlock (lock);
        }
    }
}

// critical section for all cores instances
void plat_mutex_global_lock (void)
{
    qurt_mutex_lock (&global_cs);
}

void plat_mutex_global_unlock (void)
{
    qurt_mutex_unlock (&global_cs);
}

// initialization functions
void i2c_init (void)
{
    DALResult res;

    qurt_mutex_init (&global_cs);
    i2c_log_init();

    res = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
    if (DAL_SUCCESS != res) { return; }
}

// other
boolean plat_in_exception_context (void)
{
    return FALSE;
}
