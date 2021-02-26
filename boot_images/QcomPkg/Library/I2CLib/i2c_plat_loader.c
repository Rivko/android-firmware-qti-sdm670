/** 
    @file  i2c_plat.c
    @brief platform implementation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/
/*=============================================================================
                              EDIT HISTORY
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/28/17   ska     Changed GPIO configuration to use Tlmm_ConfigGpioGroup API
=============================================================================*/

#include "i2c_plat.h"
#include "i2c_api.h"
#include "i2c_log.h"    // logging

// For Wait implementation
#include "DALSys.h"

// For GPIO enable/disable 
#include "DalDevice.h"
#include "DDITlmm.h"
#include "../../Include/api/systemdrivers/ClockBoot.h"

#include "HALtlmm.h"
#include "HALhwio.h"

#define INTLOCK()

// clocks, noc etc.
boolean plat_clock_enable (plat_device_config *config)
{
	DALResult   dalRes = TRUE;
	
    if(Clock_SetQUPV3Frequency((ClockQUPV3Type)config->core_index, 19200000) == FALSE)
    {
        dalRes = FALSE;
	    goto ON_EXIT;
    }

ON_EXIT:	
	return dalRes;
   
}

boolean plat_clock_disable (plat_device_config *config)
{
	DALResult   dalRes = TRUE;
		
	if(Clock_DisableQUPV3Clock((ClockQUPV3Type)config->core_index) == FALSE)
	{
       dalRes = FALSE;
	   goto ON_EXIT;
	}

ON_EXIT:	
	return dalRes;

}

// gpio
boolean plat_gpio_enable (plat_device_config *config)
{
	DALResult dalRes = TRUE;
	
	if(!config) {
		dalRes = FALSE;
		goto ON_EXIT;
	}

   if(Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE,(DALGpioSignalType*)&config->sda_encoding, 1) != DAL_SUCCESS)
   {
       	dalRes = FALSE;
	    goto ON_EXIT;
   }       
   if(Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE,(DALGpioSignalType*)&config->scl_encoding, 1) != DAL_SUCCESS)
   {
        dalRes = FALSE;
	    goto ON_EXIT;
   }

ON_EXIT:	
	return dalRes;
}

boolean plat_gpio_disable (plat_device_config *config)
{
	DALResult dalRes = TRUE;

	if(!config) {
		dalRes = FALSE;
		goto ON_EXIT;
	}
/*
	dalRes = DalDevice_Close(hTlmm);
	if ( DAL_SUCCESS != dalRes ) {
		dalRes = I2C_SUCCESS; // Incase OPEn had failed #ag.tbd
	}

	dalRes = DAL_DeviceDetach(hTlmm);
	if ( DAL_SUCCESS != dalRes ) {
		dalRes = FALSE;
		goto ON_EXIT;
	}
*/
ON_EXIT:	
	return dalRes;
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
    uint8 *sav = NULL;
	
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
