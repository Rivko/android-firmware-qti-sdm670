/*=============================================================================
                        S Y S D B G    R E S E T     P R O C E D U R E S  

GENERAL DESCRIPTION     
  This module defines all init functions to be performed related to the reset debug path 
  
	Copyright 2014 - 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---         --------------------------------------------------
07/24/2015 AJCheriyan   Fixed comments, API for DDR Init
11/04/2014 AJCheriyan   New for HoneyBadger
===========================================================================*/

#ifndef _SYSDBG_RESET_H
#define _SYSDBG_RESET_H

#include "com_dtypes.h"



/*===========================================================================
** 	Function: SYSDBGHWINIT
** ==========================================================================
*/
/*!
* 
* @brief
*	API to perform HW Init in the first pass
*
* @param[in] 
*	Subsystem information (not used generally)
*  
* @par Dependencies
*	None
* 
* @retval
*   Success or failure is returned
* 
* @par Side Effects
*	Must be called only after global struct is initialized
*/

boolean SysdbgHWInit(uint32);

/*===========================================================================
** 	Function: SysdbgFlushCache
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper function that checks whether cache needs to be flushed or not 
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
*	Must be called only after global struct is initialized
*/
boolean SysdbgFlushCache(uint32);


/*===========================================================================
** 	Function: SysdbgResetCore
** ==========================================================================
*/
/*!
* 
* @brief
*	Wrapper function which resets the next core to be used in the flow. It 
*	could be the next core in the same sub-system or another
*
* @param[in] 
*	Sub-system information is passed in
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*	Must be called only after global struct is initialized
*/
boolean SysdbgResetCore(uint32);

/*===========================================================================
**  Function :  SYSDBG_MEMORY_RESET
** ==========================================================================
*/
/*!
* 
* @brief
* The function performs any init of the memory regions used by system debug code
* 
* @param[in] 
*   None
*
* @par Dependencies
*   Requires memory configuration data structure to be initialized 
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/
boolean SysdbgMemoryReset(void);


/*===========================================================================
**  Function :  SYSDBG_DDR_INIT
** ==========================================================================
*/
/*!
* 
* @brief
* API to re-init DDR in the first pass
* 
* @param[in] 
*   No parameters needed. Clock for re-init is hardcoded
*
* @par Dependencies
*   All dependencies to be handled by the DDR driver
* 
* @retval
*   Success / Failure is returned
* 
* @par Side Effects
*   None
* 
*/
boolean SysdbgDDRInit(uint32);


















#endif /* _SYSDBG_RESET_H */
