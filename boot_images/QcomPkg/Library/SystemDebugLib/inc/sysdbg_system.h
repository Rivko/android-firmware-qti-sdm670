/*=============================================================================
                        System Debug System Level Function module

GENERAL DESCRIPTION     
          This module handles the system level functions performed by the driver
  
    Copyright 2012- 2014 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

09/14/14    AJCheriyan  Refactored for HoneyBadger
01/13/14    davidai     Ported for Bear
07/02/13    AJCheriyan  Updated for Gandalf
01/28/13    AJCheriyan  Added BOOT_PARTITION_SELECT write for v2
01/15/13    AJCheriyan  Moved reset apps core and debug feature enablement here
02/22/12    AJCheriyan  First cut for Aragorn. First ever SYSDBG
===========================================================================*/

#ifndef _SYSDBG_SYSTEM_H
#define _SYSDBG_SYSTEM_H

#include "com_dtypes.h"
#include "sysdbg.h"
#include "sysdbg_chipinfo.h"



/* Maximum clusters the driver supports */

/* Cluster data type */
typedef struct
{
    uint32 cores;
    cpu_type cpuname;
}cluster_info_type;

/* sub-system data type */
typedef struct
{
   cluster_info_type cluster_info;
   uint32 clusters;
}subsys_info_type;

/* Device specific data */
typedef struct
{
    /* Target family */
    sysdbg_chipfamilytype target_family;
    sysdbg_chipidtype target_name;
    uint32 target_version;
    /* sub-systems */
    subsys_info_type subsys[SUBSYS_MAX];
}target_info_type;


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

boolean SysdbgTargetInfoUpdate(void);

/* Retrieves the current subsystem */
subsys_type get_current_subsys(void);

/* Retrieves the total core count for a subsystem */
uint32 sysdbg_get_corecount(subsys_type subsystem);

/* Translates sequential cpu number to a cluster cpu number */
uint32 xlate_seqcpu_cluscpu(subsys_type subsystem, uint32 cpu);
/* Translate cpu number within a cluster to a generic cpu number */
uint32 xlate_cluscpu_seqcpu(subsys_type subsystem, uint32 cpu, uint32 cluster);

/* Getter for our cpu lock and subsys lock */
uint64 *get_cpu_lock(void);
uint64 *get_subsys_lock(void);

#endif /* _SYSDBG_SYSTEM_H */
