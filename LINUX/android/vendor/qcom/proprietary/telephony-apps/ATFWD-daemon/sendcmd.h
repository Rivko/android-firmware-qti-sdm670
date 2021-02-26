/*!
  @file
  sendcmd.h

  @brief
	Contains functions required to send commands to android through the
	AtCmdFwd Service
*/

/*===========================================================================

Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/11/11   jaimel    First cut.


===========================================================================*/

#ifndef __ATFWDDAEMON_H
#define __ATFWDDAEMON_H

#include "AtCmdFwd.h"

#ifdef __cplusplus
extern "C" {
#endif
AtCmdResponse *sendit(const AtCmd *cmd);
#ifdef __cplusplus
}
#endif

#endif /* __ATFWDDAEMON_H */
