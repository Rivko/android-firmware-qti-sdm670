/*! \file
*  
*  \brief  pm_ulog.c ----This file contains the implementation of ulog init functions.
*  \details This file contains the implementation of ulog functions.
*  
*  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header:
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/13   rl      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_ulog.h"

pm_ulog_vars_type pm_ulog_vars; 

pm_log_level_type pm_log_level = PM_LOG_LEVEL_INFO; 


void pm_ulog_init(void){
    
      pm_ulog_vars.log_size=4096;
      ULogFront_RealTimeInit(
      &pm_ulog_vars.pm_log,
      "PMIC Log",
      pm_ulog_vars.log_size,
      ULOG_MEMORY_LOCAL,
      ULOG_LOCK_OS);
      
      ULogCore_HeaderSet(pm_ulog_vars.pm_log, "Content-Type: text/tagged-log-1.0;");
}