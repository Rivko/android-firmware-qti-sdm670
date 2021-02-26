/**
 * @file:  SpmiBus_Runtime.h
 * @brief: This module provides the interface to the SPMI UEFI 
 *         Runtime definitions.
 * 
 * Copyright (c) 2012-2016 by Qualcomm Technologies
 * Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/SpmiBus_Runtime.h#1 $
 * $Change: 14402555 $ 
 */
/*=============================================================================
                              EDIT HISTORY

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/01/13   unr     Initial revision for UEFI

=============================================================================*/

#ifndef __SPMI_BUS_RUNTIME_H__
#define __SPMI_BUS_RUNTIME_H__

#include <Uefi.h>

/*-------------------------------------------------------------------------
* Function Declarations and Documentation
* ----------------------------------------------------------------------*/

/**
@brief Initializes the SPMI Runtime Library

This function initializes the SPMI runtime library.

@param[in] ImageHandle Image Handle 

@param[in] SystemTable UEFI System Table

@return  EFI_SUCCESS on success. EFI_STATUS on failure
*/

EFI_STATUS
EFIAPI
SPMIRuntimeLibInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

#endif	/* __SPMI_BUS_RUNTIME_H__ */
