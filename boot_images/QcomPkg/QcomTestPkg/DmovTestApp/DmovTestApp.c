/** @file DmovTestApp.c
   
  This file contains a sample usage of DMOV DAL and UEFI interfaces.

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 5/31/11    rl      Created.

=============================================================================*/



/*=========================================================================
      Include Files
==========================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Protocol/LoadedImage.h> 
#include <Protocol/EFIDmov.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include "DmovTestApp.h"

/* align a given address to a dword boundary */
#define ALIGN8(x) (((UINT32)x+7)&(~7))

/* global vars used by the random number generator */
static UINT32 m_z = 0x453;
static UINT32 m_w = 0x987;


/*=========================================================================
      Functions
==========================================================================*/

/** 
  Generate a 32 bit random number
**/
static UINT32 GenerateRandom()
{
    m_z = 36969 * (m_z & 65535) + ( m_z >> 16);
    m_w = 18000 * (m_w & 65535) + ( m_w >> 16);
    return (m_z << 16 ) + m_w;
}

/**
  Populate a source buffer with random numbers  

  @param[in] p pointer to source buffer

**/
VOID RandomizeSrcBuffers(UINT8* p)
{
    UINT32 i = 0;

    for ( i = 0 ; i < DMOV_BUFFER_SIZE; i++)
    {
        p[i] = GenerateRandom()%255;
    }
}


/**
  Validate data to make sure a transfer happened by comparing source and
  desitation buffers.

  @param[in] s Pointer to source buffer
  @param[in] d Pointer to destination buffer

  @return      source buffers match TRUE
               source buffers mismatch FALSE 
**/
unsigned int IsDataValid(UINT8* s, UINT8* d)
{
    UINT32 i = 0;

    /* Invalidate destination cache range */
    InvalidateDataCacheRange((void*)d,DMOV_BUFFER_SIZE);

    for (; i < DMOV_BUFFER_SIZE; i ++ )
    {
        if ( *s != *d )
        {
            AsciiPrint("mismatch at 0x%x\n: expected 0x%x, got 0x%x",s,*s,*d);
            return FALSE;
        }
    }
    return TRUE;
}

extern EFI_STATUS  DmovTransferDALApi(DmovTestMemoryHandle* pMemHandle,unsigned int deviceId);
extern EFI_STATUS  DmovTransferEFIProtocol(DmovTestMemoryHandle* pMemHandle,unsigned int deviceId);


/**
  Dmov test application entry point. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DmovTestAppMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    DmovTestMemoryHandle *pMemHandle=NULL;
    EFI_STATUS            status;
    UINT8* V=NULL;

    AsciiPrint("--------------------------------------------------\n");
    AsciiPrint("----D-M-O-V----T-E-S-T----A-P-P-L-I-C-A-T-I-O-N---\n");
    AsciiPrint("--------------------------------------------------\n\n");

    /* allocate a chunk of memory for source, destination and cmd buffers */
    status=gBS->AllocatePool(EfiBootServicesData,SLAB_SIZE,(VOID**)&V);

    if (EFI_SUCCESS != status)
    {
        AsciiPrint("Allocate memory of size 0x%x     [FAILED]\n",0x1000);
        return EFI_PROTOCOL_ERROR;
    }

    pMemHandle = (DmovTestMemoryHandle*)ALIGN8(V);

    /* Run the DAL API test */
    if (FeaturePcdGet(PcdDmovSupportedDevicesADM0))
    {
        if (EFI_SUCCESS != DmovTransferDALApi(pMemHandle,DMOV_TEST_ADM0))
        {
            AsciiPrint("DMOV DAL API Test FAILED\n");
            goto cleanup;
        }
    }
    if (FeaturePcdGet(PcdDmovSupportedDevicesADM1))
    {
        if (EFI_SUCCESS != DmovTransferDALApi(pMemHandle,DMOV_TEST_ADM1))
        {
            AsciiPrint("DMOV DAL API Test FAILED\n");
            goto cleanup;
        }
    }
    AsciiPrint("\nDmov DAL API Test PASSED \n\n");


    /* Run the EFI Protocol test */
    if (FeaturePcdGet(PcdDmovSupportedDevicesADM0))
    {
        if (EFI_SUCCESS != DmovTransferEFIProtocol(pMemHandle,DMOV_TEST_ADM0))
        {
            AsciiPrint("DMOV EFI API Test FAILED\n");
            goto cleanup;
        }
    }
    if (FeaturePcdGet(PcdDmovSupportedDevicesADM1))
    {
        if (EFI_SUCCESS != DmovTransferEFIProtocol(pMemHandle,DMOV_TEST_ADM1))
        {
            AsciiPrint("DMOV EFI API Test FAILED\n");
            goto cleanup;
        }
    }
    AsciiPrint("Dmov EFI API Test PASSED \n\n");

    
cleanup:
    /*deallocate buffers */
    gBS->FreePool(V);
    return status;
}

