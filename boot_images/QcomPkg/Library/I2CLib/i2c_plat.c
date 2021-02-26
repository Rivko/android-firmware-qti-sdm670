/*=============================================================================   
    @file  i2c_plat.c
    @brief I2C Device Platform implementation
   
    Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                        All rights reserved.
    Qualcomm Technologies, Confidential and Proprietary.
===============================================================================*/

/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 31/05/17   vg      Copyright & API updates 
=============================================================================*/

#include "i2c_plat.h"
#include "i2c_api.h"
#include "i2c_log.h"    // logging

// For Wait implementation
#include "DALSys.h"
#include "DDIClock.h"

// For GPIO enable/disable 
#include "DalDevice.h"
#include "DDITlmm.h"

#define INTLOCK()

static DalDeviceHandle *clock_handle = NULL;
static DalDeviceHandle* hTlmm = NULL;

// clocks, noc etc.
boolean plat_clock_enable (plat_device_config *config)
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
	
    /* Fails to attach to clocks, they are not available */
	res = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
	if ( (DAL_SUCCESS != res) || (NULL == clock_handle) ) {
		  result = FALSE;
		  goto ON_EXIT; 
	}

	clocks = config->common_clocks;
	while (*clocks != NULL)
	{
		res = DalClock_GetClockId(clock_handle, (const char *) (*clocks), &common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			goto ON_EXIT; 
	    }
		res = DalClock_EnableClock(clock_handle, common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			goto ON_EXIT; 
	    }

		I2C_LOG(LEVEL_INFO, "[I2C] clock_ON %s\n", (const char *) (*clocks));
		
		clocks++;
	};
    		
	res = DalClock_GetClockId(clock_handle, (const char *) (*(config->se_clock)), &se_id);
    if (DAL_SUCCESS != res) { 
	    result = FALSE;
		goto ON_EXIT; 
    } 
   
    res = DalClock_EnableClock(clock_handle, se_id);
    if (DAL_SUCCESS != res) { 
	    result = FALSE;
		goto ON_EXIT; 
    }
	
	
ON_EXIT:	
   return result;
   

}

boolean plat_clock_disable (plat_device_config *config)
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
		goto ON_EXIT;
	}

	res = DalClock_DisableClock(clock_handle, se_id);
	if (DAL_SUCCESS != res) { 
		result = FALSE;
		goto ON_EXIT; 
	}

	I2C_LOG(LEVEL_INFO, "[I2C] clock_OFF %s\n", (const char *) (*(config->se_clock)));
	
	clocks = config->common_clocks;
	while (*clocks != NULL)
	{
		res = DalClock_GetClockId(clock_handle, (const char *) (*clocks), &common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			goto ON_EXIT; 
		}

		res = DalClock_DisableClock(clock_handle, common_clock_id);
		if (DAL_SUCCESS != res) { 
			result = FALSE;
			goto ON_EXIT; 
		}

        I2C_LOG(LEVEL_INFO, "[I2C] clock_OFF %s\n", (const char *) (*clocks));
		
		clocks++;
	};
    
	res = DAL_DeviceDetach(clock_handle);
	if ( (DAL_SUCCESS != res) || (NULL == clock_handle) ) {
		result = FALSE;
		goto ON_EXIT; 
	}
    
ON_EXIT:	
   return result; 

}

// gpio
boolean plat_gpio_enable (plat_device_config *config)
{
	DALResult dalRes = I2C_SUCCESS;
	boolean result = TRUE;

	if(!config) {
		result = FALSE;
		goto ON_EXIT;
	}
	
	dalRes = DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM, DALTLMM_INTERFACE_VERSION, &hTlmm);
	if ( (DAL_SUCCESS != dalRes) || (NULL == hTlmm) ) {
	   result = FALSE;
	   goto ON_EXIT;
	}

	dalRes = DalDevice_Open(hTlmm, DAL_OPEN_SHARED);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->sda_encoding, DAL_TLMM_GPIO_ENABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   goto ON_EXIT;
	}
	
	dalRes = DalTlmm_ConfigGpio(hTlmm, config->scl_encoding, DAL_TLMM_GPIO_ENABLE);
	if ( DAL_SUCCESS != dalRes ) {
	   result = FALSE;
	   goto ON_EXIT;
	}

ON_EXIT:	
	return result; 
}

boolean plat_gpio_disable (plat_device_config *config)
{
	DALResult dalRes = I2C_SUCCESS;
	boolean result = TRUE;

	if(!config) {
		result = FALSE;
		goto ON_EXIT;
	}

	dalRes = DalDevice_Close(hTlmm);
	if ( DAL_SUCCESS != dalRes ) {
		result = I2C_SUCCESS; // Incase OPEn had failed #ag.tbd
	}

	dalRes = DAL_DeviceDetach(hTlmm);
	if ( DAL_SUCCESS != dalRes ) {
		result = FALSE;
		goto ON_EXIT;
	}

ON_EXIT:	
	return result;
}


void *plat_sys_mem_alloc (uint32 size)
{
	DALResult dalRes = I2C_SUCCESS;
	void *pTmpBuffer = NULL;

	dalRes = DALSYS_Malloc(size , &pTmpBuffer);
	if ( (DAL_SUCCESS != dalRes) || (NULL == pTmpBuffer) ) {
		dalRes = FALSE;
		goto ON_EXIT;
	}

ON_EXIT:	
	return pTmpBuffer;
}

void plat_sys_mem_dealloc (void* ptr)
{

  DALResult dalRes = I2C_SUCCESS;

  dalRes = DALSYS_Free(ptr);
  if ( (DAL_SUCCESS != dalRes) || (NULL == ptr) ) { 
     dalRes = FALSE; 
     goto ON_EXIT;   
  }
  
ON_EXIT:	
	return ;

}

void *plat_mem_alloc (uint8 core_index, uint32 size, mem_alloc_type ctxt)
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
            I2C_LOG(LEVEL_INFO, "size is not multiple of 16 and also a power of 2");
            return NULL;
        }

    }

    mem = plat_sys_mem_alloc (size);
	
    memset(mem, 0, size);

    return (void *) mem;
}

void plat_mem_free (void *ptr, uint32 size, mem_alloc_type ctxt)
{
	(void) size;
	uint8 *sav =NULL;
   // (void*) sav;

	memset(ptr, 0, size);

	if ((ctxt == TX_TRANSFER_ELEM_TYPE) ||
		(ctxt == RX_TRANSFER_ELEM_TYPE))
	{
		sav = *((uint8 **) ((uint8 *) ptr - sizeof(uint8 *)));
	}

	plat_sys_mem_dealloc (sav);
}

void i2c_init (void)
{
	return;
}

// utils
void plat_delay_us (uint32 us)
{
	DALSYS_BusyWait(us);
}

// interrupt
// not supported in uefi/xbl core
boolean plat_interrupt_register (plat_device_config *config, void (*isr) (void *), void *handle)
{
    return TRUE;
}

// not supported in uefi/xbl core
boolean plat_interrupt_deregister (plat_device_config *config)
{
    return TRUE;
}

// tcsr
// not supported in uefi/xbl core
boolean plat_tcsr_init (plat_device_config *config)
{
    return TRUE;
}

// not supported in uefi/xbl core
boolean plat_tcsr_deinit (plat_device_config *config)
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
boolean plat_mem_flush_cache (void *addr, uint32 size)
{
    return TRUE;
}

// not supported in uefi/xbl core
boolean plat_mem_invalidate_cache (void *addr, uint32 size)
{
    return TRUE;
}

// not supported in uefi/xbl core
void *plat_signal_alloc (uint8 core_index)
{
    return plat_mem_alloc (core_index, sizeof(uint8), SIGNAL_TYPE);
}

// not supported in uefi/xbl core
void plat_signal_free (void *signal)
{
    plat_mem_free (signal, sizeof(uint8), SIGNAL_TYPE);
}

// not supported in uefi/xbl core
void plat_signal_init(void **signal)
{
	return;
}

// not supported in uefi/xbl core
void plat_wait_for_event (void *signal)
{
	return;
}

// not supported in uefi/xbl core
void plat_signal_event (void *signal)
{
	return;
}

// critical section for specific instance
// not supported in uefi/xbl core
void *plat_mutex_instance_init (uint8 core_index)
{
    return (void *) 1;
}

// not supported in uefi/xbl core
void plat_mutex_instance_de_init (void *lock)
{
	return;
}

// not supported in uefi/xbl core
void plat_mutex_instance_lock (void *lock)
{
	return;
}

// not supported in uefi/xbl core
void plat_mutex_instance_unlock (void *lock)
{
	return;
}

// critical section for all cores instances
// not supported in uefi/xbl core
void plat_mutex_global_lock (void)
{
	return;
}

// not supported in uefi/xbl core
void plat_mutex_global_unlock (void)
{
	return;
}

// other
// not supported in uefi/xbl core
boolean plat_in_exception_context (void)
{
    return FALSE;
}
