#ifndef PM_UTILS_H
#define PM_UTILS_H

/*! \file pm_utils.h
 *  
 *  \brief This file contain commonly used PMIC utils related function prototype.
 *  \details This file contain PMIC timetick related helper functions and
 *           wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012-2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/inc/pm_utils.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/13   hs      Adding settling time for regulators.
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "CoreVerify.h"
#include "time_service.h"
#include <stdlib.h>
#include <string.h>
#include "rex.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

void pm_malloc(uint32 dwSize, void **ppMem);

uint64 pm_convert_time_to_timetick(uint64 time_us);

uint64 pm_convert_timetick_to_time(uint64 time_tick);

void pm_lock_init(rex_crit_sect_type *lock_ptr);

void pm_lock(rex_crit_sect_type *lock_ptr);

void pm_unlock(rex_crit_sect_type *lock_ptr);

#endif // PM_UTILS_H
