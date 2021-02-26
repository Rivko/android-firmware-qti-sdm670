/*=============================================================================
          S Y S D B G   E R R   S E R V I C E S    R O U T I N E S
GENERAL DESCRIPTION     
	Error handling services for the System debug
  
  

 Copyright 2014 - 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdebug/sysdbg_err.h#2 $

when       who          what, where, why
--------   ---          --------------------------------------------------
01/12/15	AJCheriyan	New for Honeybadger 

===========================================================================*/
#ifndef _SYSDBG_ERR_H
#define _SYSDBG_ERR_H

#include "comdef.h"


/*===========================================================================
**  Function : SysdbgClearTracks
** ==========================================================================
*/
/*!
* 
* @brief
*	Simplest form of an error handler callback which helps clean up before handling 
*	the error
*
* @param[in] 
*	None
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function clears anything that got the system to execute SDI i.e. SW / HW triggers
* 
*/
boolean SysdbgClearTracks(void);


/*===========================================================================
**  Function : SysdbgWaitForReset
** ==========================================================================
*/
/*!
* 
* @brief
*	Final infinite loop in IMEM before DDR goes back to self-refresh
*
* @param[in] 
*	None
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function cannot reside in DDR or pIMEM
* 
*/
__declspec(noreturn) void SysdbgWaitForReset(void);



/*===========================================================================
**  Function : SysdbgResetSoC
** ==========================================================================
*/
/*!
* 
* @brief
*	Resets the SoC using PMIC to toggle resin_n
*
* @param[in] 
*	None
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function simply resets the HW using the appropriate trigger. Default, is to use
*   PSHOLD
* 
*/
void SysdbgResetSoC(void);

/*===========================================================================
**  Function : sysdbg_err_fatal_handler
** ==========================================================================
*/
/*!
* 
* @brief
*	Fatal error handler. Simply de-initializes the log and resets HW
*
* @param[in] 
*	None
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   This function simply resets the HW using the appropriate trigger. Default, is to use
*   PSHOLD
* 
*/
#define SYSDBG_ERR_FATAL(arg0, arg1, arg2, arg3)	sysdbg_err_fatal_handler(FATAL_ERROR, arg1, arg2, arg3)

void sysdbg_err_fatal_handler(uint64 arg0, uint64 arg1, uint64 arg2, uint64 arg3);

#endif /* _SYSDBG_ERR_H */