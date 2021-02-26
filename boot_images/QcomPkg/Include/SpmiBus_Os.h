/**
 * @file:  SpmiBus_Os.h
 * @brief: This module provides the interface to the Spmi Bus OS
 *       layer. It mainly provides APIs to set and get the SPMI
 *       base address
 * 
 * Copyright (c) 2012-2016 by Qualcomm Technologies
 * Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/SpmiBus_Os.h#1 $
 * $Change: 14402555 $ 
 */
/*=========================================================================
                    Edit History

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/03/14   NA      Fixing size of input address for 64-bit
04/10/14   SG      SpmiBus_OsGetCoreMemoryMapSize() added.
08/16/12   UR      Initial revision.

===========================================================================*/

#ifndef __SPMI_BUS_OS_H__
#define __SPMI_BUS_OS_H__

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "SpmiTypes.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
* Type Declarations
* ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
* Function Declarations and Documentation
* ----------------------------------------------------------------------*/
/**
@brief Get the SPMI base address

This function gets the base address of the SPMI core

@param[in] uCoreIndex Core Index of the SPMI core
@param[out] puAddress Pointer to hold the base address

@return  SPMI_BUS_SUCCESS on success, error code on error

*/
SpmiBus_ResultType SpmiBus_OsGetCoreBaseAddress(
        uint32 uCoreIndex,
        UINTN *puAddress);

/**
@brief Set the SPMI base address

This function sets the base address of the SPMI core

@param[in] uCoreIndex Core Index of the SPMI core
@param[in] uAddress The address that needs to be set

@return  SPMI_BUS_SUCCESS on success, error code on error

*/
SpmiBus_ResultType SpmiBus_OsSetCoreBaseAddress(
        uint32 uCoreIndex,
        UINTN uAddress);

		
/**
@brief Get the SPMI Memory map size

This function gets the memory map size of the SPMI core

@param[in] uCoreIndex Core Index of the SPMI core
@param[out] puAddress Pointer to hold the memory size

@return  SPMI_BUS_SUCCESS on success, error code on error

*/
SpmiBus_ResultType SpmiBus_OsGetCoreMemoryMapSize(
        uint32 uCoreIndex,
        uint32 *puAddress);
	
		
#endif /* __SPMI_BUS_OS_H__ */

