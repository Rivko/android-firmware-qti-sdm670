/*=============================================================================
        S Y S D B G    M E M D U M P      T A R G E T     M O D U L E  

GENERAL DESCRIPTION     
  This module declares all the target specific modules of the memory dump table 
  module of systemdebug
  
	Copyright 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---         --------------------------------------------------
01/08/2014 AJCheriyan   New for Hawker
===========================================================================*/

#ifndef __SYSDBG_TARGET_MEMDUMP_H
#define __SYSDBG_TARGET_MEMDUMP_H

#include "comdef.h"


/*===========================================================================
**  Function: SysdbgGetDebugRegion
** ==========================================================================
*/
/*!
* 
* @brief
*   Base address of the entire debug region
*
* @param[in] 
*   None
*  
* @par Dependencies
*   None
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*   This is statically allocated space in the memory map. 
*/
void *SysdbgGetDebugRegion(void);


/*===========================================================================
** 	Function: SysdbgGetTableBase
** ==========================================================================
*/
/*!
* 
* @brief
*	Base address of the space allocated to the dump tables
*
* @param[in] 
*	Size of the table to be allocated
*  
* @par Dependencies
*	None
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*	This is statically allocated space. The size and space restrictions need
*	to be calculated offline before providing values in the macros.
*/
void *SysdbgAllocTableRegion(uint32);

/*===========================================================================
** 	Function: SysdbgAllocEntryRegion(boolean)
** ==========================================================================
*/
/*!
* 
* @brief
*	Base address of the space allocated for various entries in a given table. Each
*	entry is of type dump_data_type.
*
* @param[in] 
*	Size of the entry to be carved out
*  
* @par Dependencies
*	This will allocate space using a call to SysdbgAllocDumpSpace based on the boolean
*	input
* 
* @retval
*   True or false indicating success or failure
* 
* @par Side Effects
*	This is statically allocated space. The size and space restrictions need
*	to be calculated offline before providing values in the macros.
*/

void *SysdbgAllocEntryRegion(uint32);

/*===========================================================================
** 	Function: SysdbgAllocDumpSpace
** ==========================================================================
*/
/*!
* 
* @brief
*	Base address of the buffers for the various entries in the dump table. Every
*	entry in the dump table contains a buffer. 
*	
* @param[in] 
*	Size of the buffer to be allocated
*	  
* @par Dependencies
*	None
*
* @retval
*   Pointer to the allocated buffer is returned
* 
* @par Side Effects
*	This is statically allocated space. The size and space restrictions need
*	to be calculated offline before providing values in the macros. This api 
*	does not have to be called directly. Allocating an entry for the table will
*	typically invoke a call to allocate a buffer based on the argument.
*/

void *SysdbgAllocDumpSpace(uint32);

#endif /* __SYSDBG_TARGET_MEMDUMP_H */
