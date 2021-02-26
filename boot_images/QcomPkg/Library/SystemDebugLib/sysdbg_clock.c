/*=============================================================================
                        S Y S T E M D E B U G    C L O C K    D R I V E R 

GENERAL DESCRIPTION     
	This file implements the clock driver for reset debug functionality.
  
 Copyright 2015 - 2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SystemDebugLib/sysdbg_clock.c#2 $

when       who          what, where, why
--------   ---          --------------------------------------------------
01/10/15   AJCheriyan   Refactored for Honeybadger
08/21/12   AJCheriyan	Added BIMC clock init routines
02/26/12   AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#include "sysdbg_target.h"
/*===========================================================================
** 	Function: SysdbgClockInit
** ==========================================================================
*/
/*!
* 
* @brief
* 	Initialize the clock branches for the debug image
*
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/

boolean SysdbgClockInit(void)
{
    /* Only target speciic init here */
    return SysdbgTargetClockInit();
		
}
