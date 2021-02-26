#ifndef __PM_LOG_UTILS_H__
#define __PM_LOG_UTILS_H__

/*! \file
 *  
 *  \brief  pm_log_utils.h ----This file contain PMIC wrapper function of DALSYS_Malloc()
 *  \details his file contain PMIC wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012-2016 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/16   aab     Updated logging message size since this Macro is used to handle all SBL logging 
08/05/16   aab     Created 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include <stdio.h>
#include "boot_logger.h"

#define PM_STATUS_MSG_LEN 300
#define PM_STATUS_MSG_HEAD_LEN 4
static char status_message[PM_STATUS_MSG_LEN + PM_STATUS_MSG_HEAD_LEN]="PM: ";

#define pm_log_message(...)      \
   snprintf((status_message+PM_STATUS_MSG_HEAD_LEN), PM_STATUS_MSG_LEN,__VA_ARGS__); \
   boot_log_message(status_message);

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/



#endif // __PM_LOG_UTILS_H__
