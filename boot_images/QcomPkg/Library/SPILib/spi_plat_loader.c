/** 
    @file  spi_plat.c
    @brief platform implementation
 */
/*=============================================================================
            Copyright (c) 2019-2020 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "spi_plat.h"
#include "SpiApi.h"
#include "spi_log.h"    // logging

// For Wait implementation
#include "DALSys.h"

// For GPIO enable/disable 
#include "DalDevice.h"
#include "DDITlmm.h"
#include "../../Include/api/systemdrivers/ClockBoot.h"

#include "HALtlmm.h"
#include "HALhwio.h"
#include "hwio_tlmm.h"

#include "api/systemdrivers/ClockBoot.h"

#define INTLOCK()
#define TILE_EAST       1 
#define TILE_WEST       2
#define TILE_NORTH      3
#define TILE_SOUTH      4
#define TILE_BAD        5
//static DalDeviceHandle* hTlmm;

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
static uint32 findTile_sdm845(uint32 nGpioNum)
{
    if(in_dword(TLMM_NORTH_REG_BASE + nGpioNum * 0x1000 + 0x10)) {
        
        return TILE_NORTH;  
    }
    else if(in_dword(TLMM_SOUTH_REG_BASE + nGpioNum * 0x1000 + 0x10)) {
        
        return TILE_SOUTH;  
    }
    
    return TILE_BAD;    
}
void plat_gpio_configure(uint32 gpio_number,
                         uint32 gpio_function_select,
                         HAL_tlmm_PullType gpio_pull_type,
                         HAL_tlmm_DriveType gpio_drive_strength,
                         HAL_tlmm_DirType gpio_output_enable)
{
  
    uint32 tile_base =0;
    uint32 tile = findTile_sdm845(gpio_number);
	
    if(tile == TILE_SOUTH){
	   tile_base = TLMM_SOUTH_REG_BASE;
	 
    } else if(tile == TILE_NORTH){
	  tile_base = TLMM_NORTH_REG_BASE;
    }

	HWIO_TLMM_GPIO_CFG_OUTM(tile_base,gpio_number,
							HWIO_TLMM_GPIO_CFG_GPIO_OE_BMSK,
							gpio_output_enable << HWIO_TLMM_GPIO_CFG_GPIO_OE_SHFT);

	  /* Set the drive strength */
	HWIO_TLMM_GPIO_CFG_OUTM(tile_base,gpio_number,
							HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_BMSK,
							gpio_drive_strength << HWIO_TLMM_GPIO_CFG_DRV_STRENGTH_SHFT);

	  /* Set the function select value */
	HWIO_TLMM_GPIO_CFG_OUTM(tile_base,gpio_number,
							HWIO_TLMM_GPIO_CFG_FUNC_SEL_BMSK,
							gpio_function_select << HWIO_TLMM_GPIO_CFG_FUNC_SEL_SHFT);

	  /* Set the pull type */
	HWIO_TLMM_GPIO_CFG_OUTM(tile_base,gpio_number,
							HWIO_TLMM_GPIO_CFG_GPIO_PULL_BMSK,
							gpio_pull_type << HWIO_TLMM_GPIO_CFG_GPIO_PULL_SHFT);
							
}

// gpio
boolean plat_gpio_enable (plat_device_config *config)
{
	DALResult dalRes = TRUE;
    uint32 gpioSig;
	
	if(!config) {
		dalRes = FALSE;
		goto ON_EXIT;
	}   

   gpioSig = config->sda_encoding;
  plat_gpio_configure(HAL_GPIO_NUMBER(gpioSig),
                      HAL_FUNC_VAL(gpioSig), 
                      HAL_PULL_VAL(gpioSig), 
                      HAL_DRVSTR_VAL(gpioSig),
					  HAL_DIR_VAL(gpioSig));

   gpioSig = config->scl_encoding;
   plat_gpio_configure(HAL_GPIO_NUMBER(gpioSig),
                       HAL_FUNC_VAL(gpioSig), 
                       HAL_PULL_VAL(gpioSig), 
                       HAL_DRVSTR_VAL(gpioSig),
					   HAL_DIR_VAL(gpioSig));

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
		dalRes = SPI_SUCCESS; // Incase OPEn had failed #ag.tbd
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
	DALResult dalRes = SPI_SUCCESS;
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

  DALResult dalRes = SPI_SUCCESS;

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
            SPI_LOG(LEVEL_INFO, "size is not multiple of 16 and also a power of 2");
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


void spi_init (void)
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
