/**
    @file  spi_plat.c
    @brief platform implementation
 */
/*=============================================================================
            Copyright (c) 2016-2018 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_plat.h"
#include "busywait.h"
#include "qurt.h"
#include "qurt_island.h"
#include "uTlmm.h"
#include "uClock.h"
#include "DDIClock.h"
#include "ClockDefs.h"
#include "DALDeviceId.h"
#include "spi_log.h"
#include <stdlib.h>
#include <stringl/stringl.h>

#define MAX_FREQ_PLAN_TABLE_SIZE    8

static DalDeviceHandle *clock_handle = NULL;
static ClockIdType      core2x_id;
static ClockIdType      corex_id;
static ClockIdType      m_hclk_id;
static ClockIdType      s_hclk_id;
static ClockFreqPlanType *freq_list = NULL;
static ClockFreqPlanType  freq_list_local[MAX_FREQ_PLAN_TABLE_SIZE] ={0};
static size_t freq_list_copied = 0;


static uint32           num_entries;

// clocks, noc etc.
boolean spi_plat_clock_enable (spi_plat_device_config *config)
{
   return uClock_EnableClock(CLOCK_SCC_QUPV3_CORE_CLK) &&
          uClock_EnableClock(CLOCK_SCC_QUPV3_2XCORE_CLK)   &&
          uClock_EnableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
          uClock_EnableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK) &&
          uClock_EnableClock((uClockIdType)(config->core_index));
}

boolean spi_plat_get_div_dfs (spi_plat_device_config *config, uint32 frequency_hz, uint32* dfs_index, uint32* div)
{
   uint32 min_error=0xFFFFFFFF;
   uint8 min_index=0xFF;
   uint32 mod = 0,i = 0;
   uint32 max_div_val = 4095;

   if (freq_list_copied == 0)
   {
	  SPI_LOG(LEVEL_ERROR,"spi_plat_get_div_dfs : freq_list_copied is NULL ");
	  return FALSE;
   }
   
   if(num_entries > MAX_FREQ_PLAN_TABLE_SIZE)
   {
      SPI_LOG(LEVEL_ERROR,"spi_plat_get_div_dfs : local freq list seems to be smaller than No of DFS entries");
      return FALSE; 
   }
   
   for (i = 0; i < num_entries; i++)
   {
      mod = freq_list_local[i].nFreqHz % frequency_hz;
      if (mod != 0)
      {
         mod = frequency_hz - mod;
      }
      if (mod < min_error)
      {
         min_error = mod;
         min_index = i;
      }
   }
   
   if(min_index > MAX_FREQ_PLAN_TABLE_SIZE)
   {
      SPI_LOG(LEVEL_ERROR,"spi_plat_get_div_dfs : Calc value for DFS is more than no of DFS entries");
      return FALSE;
   }
   
   *dfs_index = min_index;
   *div = (freq_list_local[min_index].nFreqHz / frequency_hz);
   *div += (min_error == 0) ? 0 : 1;
   
   /*IN GSI the CLK divider value is only 12 bits, 
   so forcing max value as divider so that zero is not configured*/
   if((*div) > max_div_val )
   {
	   *div = max_div_val;
   }
   return TRUE;
}

boolean spi_plat_clock_set_frequency 
(
   spi_plat_device_config *config,
   uint32 frequency_hz,
   uint32* dfs_index,
   uint32* div,
   boolean set_src_clk
)
{
   DALResult res;
   ClockIdType se_id, se_src_id;
   uint32 i = 0;
   uint8 se_clock[] = "scc_qupv3_seN_clk";
   uint8 se_src_clock[] = "scc_qupv3_seN_clk_src";
   uint32 min_error=0xFFFFFFFF;
   uint8 min_index=0xFF;
   uint32 mod;
   uint32 max_div_val = 4095;

   if (qurt_island_get_status ())
   {
	   return FALSE;
   }
   if (clock_handle == NULL) { return FALSE; }

   if (set_src_clk)
   {
      se_src_clock[12] = (uint8) (0x30 + (config->core_index));
      res = DalClock_GetClockId(clock_handle, (const char *) se_src_clock, &se_src_id);
	  if (DAL_SUCCESS != res) 
      { 
          SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : se_src_clock DalClock_GetClockId failed with res %d", res);
          return FALSE; 
      }
   }

   if (freq_list == NULL)
   {
      se_clock[12] = (uint8) (0x30 + (config->core_index));
      res = DalClock_GetClockId(clock_handle, (const char *) se_clock, &se_id);
      if (DAL_SUCCESS != res) 
      { 
          SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : se_clock DalClock_GetClockId failed with res %d", res);
          return FALSE; 
      }

      res = DalClock_GetFrequencyPlan(clock_handle, se_id, &freq_list, &num_entries);
      if (DAL_SUCCESS != res) 
      { 
          SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : DalClock_GetFrequencyPlan failed with res %d", res);
          return FALSE;
      }
	  freq_list_copied = memscpy(freq_list_local, sizeof(freq_list_local), freq_list, num_entries * sizeof(ClockFreqPlanType));
      if (freq_list_copied != num_entries * sizeof(ClockFreqPlanType)) { return FALSE; }
   }

   for (i = 0; i < num_entries; i++)
   {

      mod = freq_list[i].nFreqHz % frequency_hz;
      if (mod != 0)
      {
         mod = frequency_hz - mod;
      }
      if (mod < min_error)
      {
         min_error = mod;
         min_index = i;
      }
   }
   *dfs_index = min_index;
   *div = (freq_list[min_index].nFreqHz / frequency_hz);
   *div += (min_error == 0) ? 0 : 1;
   
   /*IN GSI the CLK divider value is only 12 bits, 
   so forcing max value as divider so that zero is not configured*/
   if((*div) > max_div_val )
   {
	   *div = max_div_val;
   }

   if (set_src_clk)
   {
      res = DalClock_SetClockFrequency(clock_handle, se_src_id, freq_list[min_index].nFreqHz, CLOCK_FREQUENCY_HZ_EXACT, NULL);
      if (DAL_SUCCESS != res) 
      { 
          SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : DalClock_SetClockFrequency failed with res %d", res);
          return FALSE;
      }
	  res = DalClock_EnableClock(clock_handle, se_src_id);
	  if (DAL_SUCCESS != res) 
      { 
          SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : DalClock_EnableClock failed with res %d", res);
          return FALSE;
      }
	  config->resource_votes++;
   }
   // uClock changes: clock frequency must already be set using resource API
   // else
   // {
   //   res = DalClock_SetClockFrequency(clock_handle, se_id, freq_list[min_index].nFreqHz, CLOCK_FREQUENCY_HZ_EXACT, NULL);
   //   if (DAL_SUCCESS != res) { return FALSE; }	  
   // }
   return TRUE;
}

boolean spi_plat_src_clk_reset (spi_plat_device_config *config)
{
   DALResult res;
   ClockIdType se_src_id;
   uint8 se_src_clock[] = "scc_qupv3_seN_clk_src";

   if (qurt_island_get_status ())
   {
	   return FALSE;
   }

   if (clock_handle == NULL) { return FALSE; }
   
   se_src_clock[12] = (uint8) (0x30 + (config->core_index));
   res = DalClock_GetClockId(clock_handle, (const char *)se_src_clock, &se_src_id);
   if (DAL_SUCCESS != res) 
   {
        SPI_LOG(LEVEL_ERROR,"spi_plat_src_clk_reset : se_src_clock DalClock_GetClockId failed with res %d", res);
        return FALSE; 
   }

   res = DalClock_DisableClock(clock_handle, se_src_id);
   if (DAL_SUCCESS != res) 
   { 
       SPI_LOG(LEVEL_ERROR,"spi_plat_src_clk_reset : se_src_clock DalClock_DisableClock failed with res %d", res);
       return FALSE; 
   }
 
 if (config->resource_votes) { config->resource_votes--; }

   return TRUE;   
}
boolean spi_plat_clock_disable (spi_plat_device_config *config)
{
   return uClock_DisableClock((uClockIdType)(config->core_index)) &&
          uClock_DisableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK) &&
          uClock_DisableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
          uClock_DisableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
          uClock_DisableClock(CLOCK_SCC_QUPV3_CORE_CLK);
}

// gpio
boolean spi_plat_gpio_enable (spi_plat_device_config *config)
{
    uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->miso_encoding, UTLMM_GPIO_ENABLE);
    uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->mosi_encoding, UTLMM_GPIO_ENABLE);
    for (uint8 i = 0; i < SPI_NUM_CHIP_SELECTS; i++)
    {
        if (config->cs_encoding[i])
        {
           uTlmm_ConfigGpio((uTlmmGpioSignalType)config->cs_encoding[i],   UTLMM_GPIO_ENABLE);
        }
    }
    uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->clk_encoding,  UTLMM_GPIO_ENABLE);
    return TRUE;
}

boolean spi_plat_gpio_disable (spi_plat_device_config *config)
{
   if (config->exclusive)
   {
	   uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->sleep_miso_encoding, UTLMM_GPIO_ENABLE);
	   uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->sleep_mosi_encoding, UTLMM_GPIO_ENABLE);
	   for (uint8 i = 0; i < SPI_NUM_CHIP_SELECTS; i++)
	   {
		   if (config->sleep_cs_encoding[i])
		   {
			  uTlmm_ConfigGpio((uTlmmGpioSignalType)config->sleep_cs_encoding[i],   UTLMM_GPIO_ENABLE);
		   }
	   }
	   uTlmm_ConfigGpio ((uTlmmGpioSignalType) config->sleep_clk_encoding,  UTLMM_GPIO_ENABLE);
   }
   return TRUE;
}

// interrupt
boolean spi_plat_interrupt_register (spi_plat_device_config *config, void (*isr) (void *), void *handle)
{
    return TRUE;
}

boolean spi_plat_interrupt_deregister (spi_plat_device_config *config)
{
    return TRUE;
}

// memory
void *spi_plat_virt_to_phys (void *ptr, spi_mem_attr_type attr)
{
    (void)attr;
    return (void *) qurt_lookup_physaddr ((qurt_addr_t) ptr);
}

boolean spi_plat_mem_flush_cache (void *addr, uint32 size)
{
	if (!qurt_island_get_status ())
	{
		if (QURT_EOK != qurt_mem_cache_clean((qurt_addr_t) addr,
											 (qurt_size_t) size,
											 QURT_MEM_CACHE_FLUSH,
											 QURT_MEM_DCACHE))
		{
			//ASSERT?
			return FALSE;
		}
	}
    return TRUE;
}

boolean spi_plat_mem_invalidate_cache (void *addr, uint32 size)
{
	/*if (!qurt_island_get_status ())
	{    
		if (QURT_EOK != qurt_mem_cache_clean((qurt_addr_t) addr,
											 (qurt_size_t) size,
											 QURT_MEM_CACHE_INVALIDATE,
											 QURT_MEM_DCACHE))
		{
			//ASSERT?
			return FALSE;
		}
	}*/
    return TRUE;
}

// utils
void spi_plat_delay_us (uint32 us)
{
    busywait (us);
}

//signals
#define SPI_SIGNAL 0x2
#define SPI_SIGNAL_WAITALL 0x2
void *spi_plat_signal_init (uint8 core_index)
{
    void *sig = spi_plat_mem_alloc (core_index, sizeof(qurt_signal_t), SIGNAL_TYPE);

    if (sig != NULL)
    {
        qurt_signal_init(sig);
    }

    return sig;
}

void spi_plat_signal_deinit(void *signal)
{
    if (signal != NULL)
    {
        qurt_signal_destroy(signal);
    }

    spi_plat_mem_free (signal, sizeof(qurt_signal_t), SIGNAL_TYPE);
}

void spi_plat_wait_for_event (void **signal)
{
   unsigned int mask = 0;
   if (QURT_EOK == qurt_signal_wait_cancellable (*signal,
                                                 SPI_SIGNAL,
                                                 SPI_SIGNAL_WAITALL,
                                                 &mask))
   {
      qurt_signal_clear(*signal, SPI_SIGNAL);
   }
}

void spi_plat_signal_event (void *signal)
{
   qurt_signal_set(signal, SPI_SIGNAL);
}

// critical section for specific instance
void *spi_plat_mutex_instance_init (uint8 core_index, spi_mem_alloc_type mutex_type)
{
    void *cs = spi_plat_mem_alloc (core_index, sizeof(qurt_mutex_t), mutex_type);

    if (cs != NULL)
    {
        qurt_mutex_init (cs);
    }

    return cs;
}

void spi_plat_mutex_instance_deinit (void *lock)
{
    if (lock != NULL)
    {
        qurt_mutex_destroy (lock);
    }

    spi_plat_mem_free (lock, sizeof(qurt_mutex_t), CRIT_SEC_TYPE);
}

void spi_plat_mutex_instance_lock (void *lock)
{
    qurt_mutex_lock (lock);
}

void spi_plat_mutex_instance_unlock (void *lock)
{
    qurt_mutex_unlock (lock);
}

// critical section for all cores instances
static qurt_mutex_t global_cs;
void spi_plat_mutex_global_lock (void)
{
    qurt_mutex_lock(&global_cs);
}

void spi_plat_mutex_global_unlock (void)
{
    qurt_mutex_unlock(&global_cs);
}

void spi_plat_init (void)
{
   DALResult res;

#ifdef SPI_ENABLE_ULOG
	spi_log_init();
#endif  

    qurt_mutex_init (&global_cs);

    res = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
    if (DAL_SUCCESS != res) { return; }

    res = DalClock_GetClockId(clock_handle, "scc_qupv3_2xcore_clk", &core2x_id);
    if (DAL_SUCCESS != res) { return; }

    res = DalClock_GetClockId(clock_handle, "scc_qupv3_core_clk",   &corex_id);
    if (DAL_SUCCESS != res) { return; }

    res = DalClock_GetClockId(clock_handle, "scc_qupv3_m_hclk_clk", &m_hclk_id);
    if (DAL_SUCCESS != res) { return; }

    res = DalClock_GetClockId(clock_handle, "scc_qupv3_s_hclk_clk", &s_hclk_id);
    if (DAL_SUCCESS != res) { return; }

}

// other
boolean spi_plat_in_exception_context (void)
{
    return FALSE;
}
