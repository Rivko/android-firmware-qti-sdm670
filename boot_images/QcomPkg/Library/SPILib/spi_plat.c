/*=============================================================================   
    @file  SPI_plat.c
    @brief SPI Device Platform implementation
   
    Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
19/04/19    NM      Support GPIO Inactive Config
01/07/19    bng     initial version
=============================================================================*/

#include "spi_plat.h"
#include "SpiApi.h"
#include "spi_log.h"    // logging
#include "spi_api.h"

// For Wait implementation
#include "DALSys.h"
#include "DDIClock.h"

// For GPIO enable/disable 
#include "DalDevice.h"
#include "DDITlmm.h"
//#include "GPIO.h"

#include "ClockDefs.h"

#include  <api/systemdrivers/busywait.h>

#define INTLOCK()

//#include "api/systemdrivers/ClockBoot.h"

static DalDeviceHandle *clock_handle = NULL;
static DalDeviceHandle* hTlmm = NULL;


#define MAX_QUP_INSTANCES   3
#define SPI_VOTE            (960) //?????????
#define DEFAULT_BUS_WIDTH   4
#define DEFAULT_SE_CLK      19200000
#define BW                  (DEFAULT_SE_CLK * DEFAULT_BUS_WIDTH) 
#define MAX_SES 20
#define MAX_CS 4

#define SPI_GPIO_NUMBER(config) (((config) & 0x3FF0)    >>4) //????????
#define SPI_DRVSTR_VAL(config)  (((config) & 0x1E0000)  >>17)
#define SPI_PULL_VAL(config)    (((config) & 0x18000)   >>15)
#define SPI_DIR_VAL(config)     (((config) & 0x4000)    >>14)
#define SPI_FUNC_VAL(config)    (((config) & 0xF))

#define NUM_ENTRIES 7

// typedef struct plat_spi_core_ctxt
// {
    // GPIOKeyType nGPIOKey_miso;
    // GPIOKeyType nGPIOKey_mosi;
	// GPIOKeyType nGPIOKey_cs;
	// GPIOKeyType nGPIOKey_sclk;
// } plat_spi_core_ctxt;

// GPIOClientHandleType spi_gpio_config = {0};
// GPIOClientHandleType *spi_tlmm_handl = NULL;

// struct plat_spi_core_ctxt plat_spi_hw_core_ctxt[MAX_SES];

// clocks, noc etc.
boolean spi_plat_clock_enable (spi_plat_device_config *config)
{
	DALResult res = TRUE ;
	boolean result = TRUE;
	ClockIdType common_clock_id;
	ClockIdType se_id;
	uint8       **clocks;
	
    if (config == NULL) { 
		res = FALSE;
		goto ON_EXIT; 
	}
	
    // Fails to attach to clocks, they are not available 
	res = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
	if ( (DAL_SUCCESS != res) || (NULL == clock_handle) ) {
		  result = FALSE;
		  SPI_LOG(LEVEL_ERROR,"spi_plat_clock_enable : fail to attach to clocks , error %d",res);
		  goto ON_EXIT; 
	}

	clocks = config->common_clocks;
	while (*clocks != NULL)
	{
		res = DalClock_GetClockId(clock_handle, (const char *) (*clocks), &common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			SPI_LOG(LEVEL_ERROR,"spi_plat_clock_enable : fail to get common clock ID , error %d",res);
			goto ON_EXIT; 
	    }
		res = DalClock_EnableClock(clock_handle, common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			SPI_LOG(LEVEL_ERROR,"spi_plat_clock_enable : fail to enable common clock , error %d",res);
			goto ON_EXIT; 
	    }

		SPI_LOG(LEVEL_INFO, "[SPI] clock_ON %s\n", (const char *) (*clocks));
		
		clocks++;
	};
    		
	res = DalClock_GetClockId(clock_handle, (const char *) (*(config->se_clock)), &se_id);
    if (DAL_SUCCESS != res) { 
	    result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_enable : fail to get se clock ID , error %d",res);
		goto ON_EXIT; 
    } 
   
    res = DalClock_EnableClock(clock_handle, se_id);
    if (DAL_SUCCESS != res) { 
	    result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_enable : fail to enable se clock , error %d",res);
		goto ON_EXIT; 
    }
	
	
ON_EXIT:	
   return result;
}

boolean spi_plat_clock_disable (spi_plat_device_config *config)
{
	DALResult   res;
	boolean result = TRUE;
	ClockIdType se_id;
	ClockIdType common_clock_id;
	uint8       **clocks;

	if (clock_handle == NULL) { return FALSE; }

	res = DalClock_GetClockId(clock_handle, (const char *) (*(config->se_clock)), &se_id);
	if (DAL_SUCCESS != res) { 
		result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_disable : fail to get se clock ID , error %d",res);
		goto ON_EXIT;
	}

	res = DalClock_DisableClock(clock_handle, se_id);
	if (DAL_SUCCESS != res) { 
		result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_disable : fail to disable se clock , error %d",res);
		goto ON_EXIT; 
	}

	SPI_LOG(LEVEL_INFO, "[SPI] clock_OFF %s\n", (const char *) (*(config->se_clock)));
	
	clocks = config->common_clocks;
	while (*clocks != NULL)
	{
		res = DalClock_GetClockId(clock_handle, (const char *) (*clocks), &common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			SPI_LOG(LEVEL_ERROR,"spi_plat_clock_disable : fail to get common clock ID , error %d",res);
			goto ON_EXIT; 
		}

		res = DalClock_DisableClock(clock_handle, common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			SPI_LOG(LEVEL_ERROR,"spi_plat_clock_disable : fail to disable common clock , error %d",res);
			goto ON_EXIT; 
		}

        SPI_LOG(LEVEL_INFO, "[SPI] clock_OFF %s\n", (const char *) (*clocks));
		
		clocks++;
	};
    
	res = DAL_DeviceDetach(clock_handle);
	if ( (DAL_SUCCESS != res) || (NULL == clock_handle) ) {
		result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_disable : fail to detach clock handle, error %d",res);
		goto ON_EXIT; 
	}
    
ON_EXIT:	
   return result;
   
}

// gpio
boolean spi_plat_gpio_enable (spi_plat_device_config *config)
{ 
	DALResult dalRes = SPI_SUCCESS;
	boolean result = TRUE;

	if(!config) {
		result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : invalid parameter");
		goto ON_EXIT;
	}
	
	dalRes = DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM, DALTLMM_INTERFACE_VERSION, &hTlmm);
	if ( (DAL_SUCCESS != dalRes) || (NULL == hTlmm) ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : failed to get dal device handle , error %d",dalRes);
	   goto ON_EXIT;
	}

	dalRes = DalDevice_Open(hTlmm, DAL_OPEN_SHARED);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : failed to open dal device handle , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->miso_encoding, DAL_TLMM_GPIO_ENABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : failed to enable config->miso_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->mosi_encoding, DAL_TLMM_GPIO_ENABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : failed to enable config->mosi_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->cs_encoding, DAL_TLMM_GPIO_ENABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : failed to enable config->cs_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->clk_encoding, DAL_TLMM_GPIO_ENABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_enable : failed to enable config->clk_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}

ON_EXIT:	
	return result; 
	
}

boolean spi_plat_gpio_disable (spi_plat_device_config *config)
{ 
	DALResult dalRes = SPI_SUCCESS;
	boolean result = TRUE;

	if(!config) {
		result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_disable : invalid parameter");
		goto ON_EXIT;
	}
	
	/*dalRes = DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM, DALTLMM_INTERFACE_VERSION, &hTlmm);
	if ( (DAL_SUCCESS != dalRes) || (NULL == hTlmm) ) {
	   result = FALSE;
	   goto ON_EXIT;
	} */

	dalRes = DalTlmm_ConfigGpio(hTlmm, config->sleep_miso_encoding, DAL_TLMM_GPIO_DISABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_disable : failed to enable config->miso_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->sleep_mosi_encoding, DAL_TLMM_GPIO_DISABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_disable : failed to enable config->mosi_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->sleep_cs_encoding, DAL_TLMM_GPIO_DISABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_disable : failed to enable config->cs_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->sleep_clk_encoding, DAL_TLMM_GPIO_DISABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_disable : failed to enable config->clk_encoding , error %d",dalRes);
	   goto ON_EXIT;
	}

	dalRes = DalDevice_Close(hTlmm);
	if ( DAL_SUCCESS != dalRes ) {
		result = SPI_SUCCESS; // Incase OPEn had failed #ag.tbd
	}

	dalRes = DAL_DeviceDetach(hTlmm);
	if ( DAL_SUCCESS != dalRes ) {
		result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_gpio_disable : failed to detach device handle, error %d",dalRes);
		goto ON_EXIT;
	}

ON_EXIT:	
	return result;
}
	

void *spi_plat_sys_mem_alloc (uint32 size)
{
	DALResult dalRes = SPI_SUCCESS;
	void *pTmpBuffer = NULL;

	dalRes = DALSYS_Malloc(size , &pTmpBuffer);
	if ( (DAL_SUCCESS != dalRes) || (NULL == pTmpBuffer) ) {
		dalRes = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_sys_mem_alloc : failed to allocate memory , error %d",dalRes);
		goto ON_EXIT;
	}

ON_EXIT:	
	return pTmpBuffer;
}

void spi_plat_sys_mem_dealloc (void* ptr)
{

  DALResult dalRes = SPI_SUCCESS;

  dalRes = DALSYS_Free(ptr);
  if ( (DAL_SUCCESS != dalRes) || (NULL == ptr) ) { 
     dalRes = FALSE; 
	 SPI_LOG(LEVEL_ERROR,"spi_plat_sys_mem_dealloc : failed to free memory , error %d",dalRes);
     goto ON_EXIT;   
  }
  
ON_EXIT:	
	return ;

}

void *spi_plat_mem_alloc (uint8 core_index, uint32 size, mem_alloc_type ctxt)
{
    uint8 *mem = NULL;
    (void) core_index;
	//(void*) size ;

    if ((ctxt == TX_TRANSFER_ELEM_TYPE) ||
        (ctxt == RX_TRANSFER_ELEM_TYPE))
    {
        // enforce that size is multiple of 16 and also a power of 2
        if (((size & 0xF) != 0) ||
             (size & (size - 1)))
        {   
            SPI_LOG(LEVEL_INFO, "size is not multiple of 16 and also a power of 2");
            return NULL;
        }

    }

    mem = spi_plat_sys_mem_alloc (size);
	if(mem != NULL)
	{
		memset(mem, 0, size);
	}

    return (void *) mem;
}

void spi_plat_mem_free (void *ptr, uint32 size, mem_alloc_type ctxt)
{
	(void) size;
	memset(ptr, 0, size);
	spi_plat_sys_mem_dealloc (ptr);
	
}

void SPI_init (void)
{
	return;
}

// utils
void spi_plat_delay_us (uint32 us)
{
	//DALSYS_BusyWait(us);
	busywait(us);
}

// interrupt
// not supported in uefi/xbl core
boolean plat_interrupt_register (spi_plat_device_config *config, void (*isr) (void *), void *handle)
{
    return TRUE;
}

// not supported in uefi/xbl core
boolean plat_interrupt_deregister (spi_plat_device_config *config)
{
    return TRUE;
}

// tcsr
// not supported in uefi/xbl core
boolean plat_tcsr_init (spi_plat_device_config *config)
{
    return TRUE;
}

// not supported in uefi/xbl core
boolean plat_tcsr_deinit (spi_plat_device_config *config)
{
    return TRUE;
}

// not supported in uefi/xbl core
void *plat_virt_to_phys (void *ptr, mem_attr_type attr)
{
    return ptr;
}

// memory
// not supported in uefi/xbl core
boolean spi_plat_mem_flush_cache (void *addr, uint32 size)
{
    return TRUE;
}

// not supported in uefi/xbl core
boolean spi_plat_mem_invalidate_cache (void *addr, uint32 size)
{
    return TRUE;
}

// not supported in uefi/xbl core
void *spi_plat_signal_alloc (uint8 core_index)
{
    return spi_plat_mem_alloc (core_index, sizeof(uint8), SIGNAL_TYPE);
}

// not supported in uefi/xbl core
void spi_plat_signal_free (void *signal)
{
    spi_plat_mem_free (signal, sizeof(uint8), SIGNAL_TYPE);
}

// not supported in uefi/xbl core
void *spi_plat_signal_init(int core_index)
{
	return NULL;
}

// not supported in uefi/xbl core
void spi_plat_wait_for_event (void *signal)
{
	return;
}

// not supported in uefi/xbl core
void spi_plat_signal_event (void *signal)
{
	return;
}

// critical section for specific instance
// not supported in uefi/xbl core
void *spi_plat_mutex_instance_init (uint8 core_index,mem_alloc_type type)
{
    return (void *) 1;
}

// not supported in uefi/xbl core
void spi_plat_mutex_instance_de_init (void *lock)
{
	return;
}

// not supported in uefi/xbl core
void spi_plat_mutex_instance_lock (void *lock)
{
	return;
}

// not supported in uefi/xbl core
void spi_plat_mutex_instance_unlock (void *lock)
{
	return;
}

// critical section for all cores instances
// not supported in uefi/xbl core
void spi_plat_mutex_global_lock (void)
{
	return;
}

// not supported in uefi/xbl core
void spi_plat_mutex_global_unlock (void)
{
	return;
}

// other
// not supported in uefi/xbl core
boolean plat_in_exception_context (void)
{
    return FALSE;
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
	if(config == NULL || div == NULL )
	{
		return FALSE;
	}
	DALResult res = TRUE ;
	boolean result = TRUE;
	ClockIdType se_id;
	uint32 pnResultFreq = 0;
	
    spi_plat_clock_config *clk_cfg = config->clock_config;
	uint8 i = 0;
	*dfs_index = 0;
	uint32 min_error=0xFFFFFFFF;
	uint8 min_index=0xFF;
    uint32 mod;
	
	for (i = 0; i < NUM_ENTRIES; i++)
	{
	   mod = clk_cfg[i].se_clock_frequency_khz % frequency_hz;
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
	*div = (clk_cfg[min_index].se_clock_frequency_khz / frequency_hz);
    *div += (min_error == 0) ? 0 : 1;
	
	res = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
	if ( (DAL_SUCCESS != res) || (NULL == clock_handle) ) {
		  result = FALSE;
		  SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : fail to attach to clocks , error %d",res);
		  goto ON_EXIT; 
	}
	
	res = DalClock_GetClockId(clock_handle, (const char *) (*(config->se_clock)), &se_id);
    if (DAL_SUCCESS != res) { 
	    result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : fail to get se clock ID , error %d",res);
		goto ON_EXIT; 
    } 
	res = DalClock_SetClockFrequency(clock_handle, se_id, clk_cfg[min_index].se_clock_frequency_khz, CLOCK_FREQUENCY_MHZ_CLOSEST,  &pnResultFreq);
	if (DAL_SUCCESS != res) { 
	    result = FALSE;
		SPI_LOG(LEVEL_ERROR,"spi_plat_clock_set_frequency : fail to set freq , error %d",res);
		goto ON_EXIT; 
    }
	/*if(Clock_SetQUPV3Frequency((ClockQUPV3Type)config->core_index, clk_cfg[min_index].se_clock_frequency_khz) == FALSE)
    {
        return FALSE;
    }*/
ON_EXIT:
	return result;
}
boolean spi_plat_in_exception_context (void)
{
    return FALSE;
}
