/*! \file pm_utils.c
 *  
 *  \brief This file contain commonly used PMIC utils related function implementation.
 *  \details This file contain PMIC timetick related helper functions implementation
 *           and wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012-2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/src/pm_utils.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_utils.h"
#include <err.h>

void pm_malloc(uint32 dwSize, void **ppMem)
{
    *ppMem = malloc(dwSize);

    if(NULL == *ppMem)
    {        
       ERR_FATAL( "PMIC-Memory allocation failed ", 0, 0, 0 );
    }
    
    memset(*ppMem, 0, dwSize);
}

uint64 pm_convert_time_to_timetick(uint64 time_us)
{
    return (time_us*19200000)/1000000;
}

uint64 pm_convert_timetick_to_time(uint64 time_tick)
{

    return (time_tick*1000000)/19200000;
}

void pm_lock_init(rex_crit_sect_type *lock_ptr)
{
    rex_init_crit_sect(lock_ptr);
}

void pm_lock(rex_crit_sect_type *lock_ptr)
{
    rex_enter_crit_sect(lock_ptr);
}

void pm_unlock(rex_crit_sect_type *lock_ptr)
{
  rex_leave_crit_sect(lock_ptr);
}

