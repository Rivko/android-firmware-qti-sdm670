/*=============================================================================

                             SDI Timer / Busywait Driver

GENERAL DESCRIPTION
  This file declares the APIs for timer / busywait mini-driver. 

 Copyright 2012 - 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SystemDebugLib/inc/sysdbg_timer.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/28/2015 amcheriy Removed custom busywait
10/28/2014 amcheriy Added MPM timer api
11/13/2013 amcheriy Improved version for MSM8994
06/19/2012 amcheriy	First cut for MSM8974

=============================================================================*/


#ifndef _SYSDBG_TIMER_H
#define _SYSDBG_TIMER_H

#include "comdef.h"

/*===========================================================================
**  Function :  SYSDBG_TIMER_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* 	This function initializes frame 0 of the QTIMER instance in the APPS subsystem
* 
* @param[in] 
*   None
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*
*/

boolean SysdbgTimerInit(void);



#endif /* _SYSDBG_TIMER_H */