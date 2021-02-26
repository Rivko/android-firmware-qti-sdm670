/** @file SPMIApp.c
   
  SPMI App

  Copyright (c) 2012,2014, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
 **/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/20/12   unr     Added ReadModifyWriteLongByte API
 07/05/12   unr     Supported API changes
 06/28/12   unr     Initial revision of SPMI App

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/TestInterface.h>
#include <Library/DebugLib.h>
#include "SpmiBus.h"

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

 **/

EFI_STATUS
EFIAPI
SPMIAppMain(
            IN EFI_HANDLE ImageHandle,
            IN EFI_SYSTEM_TABLE *SystemTable
            )
{
    static const Spmi_AccessPriority prio = SPMI_BUS_ACCESS_PRIORITY_LOW;
    
    EFI_STATUS status;
    Spmi_Result result;
    UINT8 ucData[4] = {0, 0, 0, 0};
    UINT32 bytesRead = 0;
    
    UINT32 sid = PcdGet32( SPMIPOnID );
    UINT32 readAddr = PcdGet32( SPMIPOnTypeReg );
    UINT32 expectedVal = PcdGet32( SPMIPOnType );
    UINT32 writeAddr = PcdGet32( SPMIPOnWriteReg );

    TEST_START( "SPMIApp" );

    if((result = SpmiBus_Init()) != SPMI_SUCCESS) {
        DebugPrint( DEBUG_ERROR, "SPMI init failed: %d\n", result );
        goto exit;
    }
    
    if((result = SpmiBus_ReadLong( sid, prio, readAddr, ucData, 2, &bytesRead )) != SPMI_SUCCESS) {
        DebugPrint( DEBUG_ERROR, "SPMI read failed: %d\n", result );
        goto exit;
    }
    
    if(bytesRead != 2) {
        DebugPrint( DEBUG_ERROR, "SPMI read verify len failed\n" );
        goto exit;
    }

    if((ucData[0] != (expectedVal & 0xFF)) || (ucData[1] != ((expectedVal & 0xFF00) >> 8))) {
        DebugPrint( DEBUG_ERROR, "SPMI read verify failed: %d\n", result );
        goto exit;
    }

    if(writeAddr != 0) 
    {
        ucData[0] = 0x03;
        if((result = SpmiBus_WriteLong( sid, prio, writeAddr, &ucData[0], 1 )) != SPMI_SUCCESS) {
            DebugPrint( DEBUG_ERROR, "SPMI write failed: %d\n", result );
            goto exit;
        }
        
        ucData[1] = 0;
        if((result = SpmiBus_ReadLong( sid, prio, writeAddr, &ucData[1], 1, &bytesRead )) != SPMI_SUCCESS) {
            DebugPrint( DEBUG_ERROR, "SPMI write failed: %d\n", result );
            goto exit;
        }
        
        if(bytesRead != 1) {
            DebugPrint( DEBUG_ERROR, "SPMI read verify len failed\n" );
            goto exit;
        }

        if(ucData[0] != ucData[1]) {
            DebugPrint( DEBUG_ERROR, "SPMI write verify failed\n" );
            goto exit;
        }

        ucData[0] = 0x03;
        ucData[1] = 0xFC;
        ucData[2] = 0x1E;
        if((result = SpmiBus_ReadModifyWriteLongByte( sid, prio, writeAddr, 
                                                     (UINT32) ucData[1], (UINT32) ucData[2], &ucData[3] )) != SPMI_SUCCESS) 
        {
            DebugPrint( DEBUG_ERROR, "SPMI read-modify-write failed: %d\n", result );
            goto exit;
        }

        /*  Result     = (ReadData   & ~Mask)      | (WriteData & Mask) */
        /*  0x1D       = (0x3        & ~0x1E)      | (0xFC      & 0x1E) */
        if(ucData[3] != ((ucData[0] & ~ucData[2]) | (ucData[1] & ucData[2]))) 
        {
            DebugPrint( DEBUG_ERROR, "SPMI read-modify-write verify failed\n" );
            goto exit;
        }
    }

    exit:

    status = result == SPMI_SUCCESS ? EFI_SUCCESS : EFI_DEVICE_ERROR;
    TestStatus( "SPMIApp", status );
    TEST_STOP( "SPMIApp" );
    
    return status;
}
