/** 
    @file  ext_stubs.c
    @brief stubs for external dependencies
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "ext_stubs.h"

// note for CORE PRODUCTS team. please remove once PRAM APIs are available in
// ADSP
int pram_mgr_lookup_phys_addr(int client_handle, qurt_addr_t vaddr)
{
    (void) client_handle;
    return (int) vaddr;
}

int pram_mgr_lookup_phys_addr_by_name(char *name, qurt_addr_t vaddr)
{
    (void) name;
    return (int) vaddr;
}

// note for SYSTEMDRIVERS team. please remove uClock and uTlmm stubs from this
// file once you are supporting these APIs in ADSP
boolean uClock_DisableClock(uClockIdType eClockId)
{
    return TRUE;
}
boolean uClock_EnableClock(uClockIdType eClockId)
{
    return TRUE;
}
boolean uTlmm_ConfigGpio(uTlmmGpioSignalType uGpioConfig, uTlmmGpioEnableType enable)
{
    return TRUE;
}
