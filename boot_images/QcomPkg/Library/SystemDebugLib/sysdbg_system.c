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

#include "sysdbg_system.h"
#include "sysdbg_util.h"
#include "sysdbg_log.h"
#include "sysdbg_chipinfo.h"
#include "sysdbg_err.h"
#include "sysdbg_asm.h"
#include "sysdbg_sysreg.h"

/* Top level struct which keeps track of the target information */
static target_info_type target_info;

/* Our global CPU lock */
static global_lock_type cpu_lock;
static global_lock_type subsys_lock;

/* Getter for our cpu lock */
uint64 *get_cpu_lock(void)
{
    return (uint64 *)&cpu_lock;
}

uint64 *get_subsys_lock(void)
{
    return (uint64 *)&subsys_lock;
}




/*===========================================================================
**  Function :  SYSDBG_TARGET_INFOUPDATE
** ==========================================================================
*/
/*!
* 
* @brief
* The function that enables the reset debug feature
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

boolean SysdbgTargetInfoUpdate(void)
{
    static boolean init = FALSE;

    if (!init)
    {

        /* Find the target family and version if needed */
        target_info.target_family = SysdbgGetChipFamily ();
        target_info.target_name = SysdbgGetChipId();

        switch (target_info.target_family)
        {
            case FAMILY_SR:
                switch(target_info.target_name)
                {
                    
                    case CHIP_QDF2432:
                            target_info.subsys[SUBSYS_IMC].cluster_info.cores = 1;
                            target_info.subsys[SUBSYS_IMC].cluster_info.cpuname = ARMv8_CPU;
                            target_info.subsys[SUBSYS_IMC].clusters = 1;

                            target_info.subsys[SUBSYS_APPS].cluster_info.cores = 2;
                            target_info.subsys[SUBSYS_APPS].cluster_info.cpuname = HYDRA_CPU;
                            target_info.subsys[SUBSYS_APPS].clusters = 12;
                        break;

                    default:
                        break;
                }
                break;

            default:
                SYSDBG_ERR_FATAL(FATAL_ERROR, __current_pc(), target_info.target_family, target_info.target_version );
                break;
        }

    init = TRUE;

    }

    return init;
}

uint32 sysdbg_get_corecount(subsys_type subsystem)
{
    uint32 corecount = 0;

    /* Make sure init is done */
    SysdbgTargetInfoUpdate();

    if (!(subsystem < SUBSYS_MAX))
        SYSDBG_LOG_EVENT(FATAL_ERROR, 0, 0, 0);
    else
    {
        if (target_info.subsys[subsystem].cluster_info.cpuname != UNKNOWN_CPU)
            corecount += (target_info.subsys[subsystem].clusters * target_info.subsys[subsystem].cluster_info.cores);
    }

    return corecount;
}



subsys_type get_current_subsys(void)
{
    /* Depending on the CPU, we get the subsys */
    switch (get_current_cpuname())
    {
        case CORTEX_A53:
            return SUBSYS_IMC;
            break;
        case HYDRA:
            return SUBSYS_APPS;
            break;
        default:
            return SUBSYS_UNKNOWN;
            break;

    }
}


/* Translates sequential cpu number to a cluster cpu number */
uint32 xlate_seqcpu_cluscpu(subsys_type subsystem, uint32 cpu)
{
    uint32 i = 0, cluscpu = cpu;
    
   /* Make sure init is done */
    SysdbgTargetInfoUpdate();

    // Find the max 
    for (i = 0; ((i <  target_info.subsys[subsystem].clusters) && (target_info.subsys[subsystem].cluster_info.cpuname != UNKNOWN_CPU)) ; i++)
    {
        if (cluscpu >= target_info.subsys[subsystem].cluster_info.cores)
            cluscpu -= target_info.subsys[subsystem].cluster_info.cores;
        else
            break;
    }

    return cluscpu;
}

/* Translate cpu number within a cluster to a generic cpu number */
uint32 xlate_cluscpu_seqcpu(subsys_type subsystem, uint32 cpu, uint32 cluster)
{
    uint32 i = 0, seqcpu = cpu;

    /* Make sure init is done */
    SysdbgTargetInfoUpdate();

    for (i = 0; i < cluster; i++)
        seqcpu += target_info.subsys[subsystem].cluster_info.cores;

    return seqcpu;
}

