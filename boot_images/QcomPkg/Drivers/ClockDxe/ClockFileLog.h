#ifndef __CLOCKFILELOG_H__
#define __CLOCKFILELOG_H__

/*! @file ClockFileLog.h
 *
 *  CLOCK FILE Logging
 *
 *  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/17   op      Enable File Logging

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
  /**
  File Logging Dependencies
  */
#include "ULogFront.h"
#include "ULogUEFI.h"


/**
UEFI DEBUG Dependencies
*/
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>
#include <Library/QcomLib.h>
/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/
/* Clock Log File Name */
#define CLOCKLOGFILE_IN_EFS "ClockLog"
#define CLOCKLOGFILE_SIZE   524288 // 0.5MB

#define _ULOG(Expression)   ULogPrint Expression

/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/

EFI_STATUS ClockFileLog_InitFileLog(ULogHandle *ULhandle);
EFI_STATUS ClockFileLog_GetEBSClockLogSetting(BDS_CLOCKLOG_STATE *eLogState);
void Clock_GetgClockLogSetting(BDS_CLOCKLOG_STATE *eLogSetting);

#endif // __CLOCKFILELOG_H__


