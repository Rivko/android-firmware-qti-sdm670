/*=============================================================================
                        System Debug Reset Module 

GENERAL DESCRIPTION     
          This module handles the main entry point of the system debug functionalities.
  
    Copyright 2015- 2016 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


04/06/15    AJCheriyan  Removed delay and replaced logic
01/14/15    AJCheriyan  First cut for Honeybadger
===========================================================================*/



#include "sysdbg_reset.h"
#include "com_dtypes.h"
#include "sysdbg_main.h"
#include "sysdbg_target.h"
#include "sysdbg_util.h"
#include "string.h"
#include "sysdbg_timer.h"
#include "sysdbg_clock.h"
#include "sysdbg_system.h"
#include "busywait.h"
#include "sysdbg_mem_dump.h"
#include "sysdbg_sysreg.h"


extern void dcache_flush_inv_cache_level(uint64);
extern void dcache_flush_inv_cache();
/*===========================================================================
**  Function :  SYSDBG_MEMORY_RESET
** ==========================================================================
*/
/*!
* 
* @brief
* The function performs any init of the memory regions used by system debug code
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Requires memory configuration data structure to be initialized 
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/
boolean SysdbgMemoryReset(void)
{
    const mem_config_type *config_data = Sysdbg_MemConfig();

    uint32 i = 0, count = ARRAY_SIZE(config_data->slices);

    for (i = 0; i < count; i++)
    {
       const mem_slice_type *slice = &config_data->slices[i];

       if (!slice->size)
           continue;
       else
       {
           if (slice->zinit)
               memset((void *)((uintptr_t)(slice->base + slice->offset)), 0, slice->size);
       }
    }

    return TRUE;
}

/*===========================================================================
**  Function :  SYSDBG_COLD_BOOT_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* The function performs any init of the hardware before enabling debug features
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Calls target specific enablement function
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/

boolean SysdbgColdBootInit(void)
{
    return (SysdbgMemoryReset() && SysdbgDumpTableInit());
}


boolean SysdbgHWInit(uint32 subsys)
{
    boolean success = TRUE;

    /* Timer Init */
    // success &= SysdbgTimerInit();

    /* Clock Init */
    success &= SysdbgClockInit();


    return success;
}

boolean SysdbgFlushCache(uint32 subsys)
{
    dcache_flush_inv_cache_level(0);
    dcache_flush_inv_cache_level(1);
    dcache_flush_inv_cache_level(2);

    return TRUE;
}

boolean SysdbgResetCore(uint32 subsys)
{
    /* Change the logic in the future to check for the subsystem and then cores */
    /* Find the next cpu to be brought out of reset for this sub-system */
    uint32 cpu = 0, cluster = 0;
    boolean next_core = FALSE;

    /* Currently, only boolean logic is only required for IMC + Apps */
    if (SUBSYS_IMC != subsys)
    {
        cpu = xlate_seqcpu_cluscpu(SUBSYS_APPS, get_current_cpu_seq());
        cluster = get_current_cluster();

    }

    /* Find and reset the next core */
    next_core = SysdbgTargetResetCore(cluster, cpu);

    /* Note that the next core need not be cpu + 1. We just need to reset the lock */
    if (next_core)
        /* Set the global lock value */
        set_global_lock(get_cpu_lock(), 0xFF00 | cpu);

    return next_core;

}
