/** @file FwProvisionFuseLib.c
   
  Firmware provisioning functions used for factory provisioning for 
  Windows-on-Snapdragon (WoS) 
  
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  Copyright (C) 2012 Apple, Inc. All rights reserved. 

  All rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License 
  which accompanies this distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

  **/

/*=============================================================================
                              EDIT HISTORY


when        who        what, where, why
--------    ---        ----------------------------------------------------------- 
2015/04/16/ mic	       Use updated DppProtocol 
2015/03/22  mic        Updated log message & removed Fuse blowing support
2014/09/08  mcl        Updated security control base for 8916.
2014/08/15  sm         Switched to new SCM API
2014/02/26  mic        Added watermark checking fix
2014/02/22  mic        Write result flag file only when fuse blowing is executed 
2014/02/04  mic        Added cold reset after blowing fuses
2013/12/10  mic/manoj  Fixed fuse tz api param bug
2013/10/18  mic/manoj  Initial Version of Fuse Provisioning Library.
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EFIDpp.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/FwCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include <Protocol/EFIRng.h>
#include <Library/TzRuntimeLib.h>


/* for QrdProvStage1 */
#define BT_MAC_PROV_FILE_SIZE 8
#define WLAN_MAC_PROV_FILE_SIZE 27 

#define GEN_MAC_FILENAME         L"GENMAC.DAT"
#define BT_PROV_FILENAME         L"BT.PROVISION"
#define WLAN_PROV_FILENAME       L"WLAN.PROVISION"

EFI_STATUS
DppItemDelete (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name
  )
{

  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  
  if( (Type == NULL) ||
      (Name == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DppItemDeleteExit;
  }

  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, DppItemDeleteExit);

  /* Dpp delete*/
  Status = DppProtocol->DeleteItemDPP(DppProtocol, Type, Name);
  HANDLE_ERROR_LABEL(Status, DppProtocol->DeleteItemDPP, DppItemDeleteExit);

DppItemDeleteExit:

  return Status;

}

EFI_STATUS
DppItemRead (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  OUT UINT8       **Buff,
  OUT UINTN        *Size
)
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  UINTN             BuffSize    = 0;


  if( (Buff == NULL) || (Type == NULL) ||
      (Name == NULL) || (Size == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DppItemReadExit;
  }
  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, LocateDppProtocol, DppItemReadExit);

  /* use Dpp protocol to get required buffer size first */
  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, NULL, &BuffSize);
  HANDLE_ERROR_LABEL(Status, DppProtocol->GetDPP, DppItemReadExit);

  /* allocate required buffer */
  *Buff = (UINT8 *) AllocatePool(BuffSize);
  if(*Buff == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DppItemReadExit;
  }

  *Size = BuffSize;

  /* read dpp item */
  Status = DppProtocol->GetDPP( DppProtocol, Type, Name, *Buff, &BuffSize);
  HANDLE_ERROR_LABEL(Status, DppProtocol->GetDPP, DppItemReadExit);

DppItemReadExit:

  return Status;
}

EFI_STATUS
DppItemWrite (
  IN  CHAR16       *Type,
  IN  CHAR16       *Name,
  IN UINT8         *Buff,
  IN UINTN          Size
  )
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  EFI_DPP_PROTOCOL *DppProtocol = NULL;
  
  if( (Buff == NULL) ||
      (Type == NULL) ||
      (Name == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    goto DppItemWriteExit;
  }

  /* Get DPP protocol interface */
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, DppItemWriteExit);

  /* write dpp item*/
  Status = DppProtocol->SetDPP( DppProtocol, Type, Name, Buff, Size);
  HANDLE_ERROR_LABEL(Status, DppProtocol->SetDPP, DppItemWriteExit);

DppItemWriteExit:

  return Status;
}

EFI_STATUS
QrdProvStage1(VOID) 
{
  EFI_STATUS             Status              = EFI_SUCCESS;
  EFI_QCOM_RNG_PROTOCOL *pEfiQcomEngProtocol = NULL;
  EFI_GUID               AlgoID;
  UINTN                  AlgoIDSize          = sizeof(AlgoID);
  UINT8*                 pBtBuffer           = NULL;
  UINT8*                 pWlanBuffer         = NULL;  
  UINT8*                 pBufferIn           = NULL;
  UINTN                  BufferInSize        = 0;
  UINT8                  pRngBuffer[4]       = { 0x0 };
  UINT32                 RngBufSize          = 4;
  UINTN                  i                   = 0;
  UINTN                  j                   = 0;
  UINTN                  k                   = 0;

  DEBUG((EFI_D_INFO,"\n\nPerforming QrdProvStage1, provisioning temporary BT/WLAN MAC address..\n"));
  DEBUG((EFI_D_INFO,"  Reading GENMAC.DAT..\n"));

  /* Check if QCOM/GENMAC.DAT exist? */
  Status = DppItemRead(L"QCOM", GEN_MAC_FILENAME, &pBufferIn, &BufferInSize);

  if(Status == EFI_NOT_FOUND)
  {
    DEBUG((EFI_D_INFO,"  GENMAC.DAT not found\n"));
    Status = EFI_SUCCESS;

    goto QrdProvStage1Exit;
  }

  /* AsciiPrint used in this function will be called when GENMAT.DAT is found, which is in the factory line */
  AsciiPrint("  GENMAC.DAT found, deleting it..\n");
  /* Delete QCOM/GENMAC.DAT exist */
  Status = DppItemDelete(L"QCOM",GEN_MAC_FILENAME);
  HANDLE_ERROR_LABEL(Status, 5 minutes, QrdProvStage1Exit);

  /* locate rng protocol */
  Status = gBS->LocateProtocol(&gQcomRngProtocolGuid, NULL, (VOID **) &pEfiQcomEngProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, QrdProvStage1Exit);

  Status = pEfiQcomEngProtocol->GetInfo(pEfiQcomEngProtocol, &AlgoIDSize, &AlgoID);
  HANDLE_ERROR_LABEL(Status, pEfiQcomEngProtocol->GetInfo, QrdProvStage1Exit);
  
  /* allocate BT prov file buffer */
  pBtBuffer = (UINT8 *) AllocatePool(BT_MAC_PROV_FILE_SIZE);
  if(pBtBuffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto QrdProvStage1Exit;
  }
  ZeroMem(pBtBuffer, BT_MAC_PROV_FILE_SIZE);

  /* Create BT MAC address 
      The random MAC for BT uses first MSB 2 bytes assigned number to company id 
      (qualcomm: 0x001D, QUIC: 0x00B8, Atheros: 0x0045) and last 4 bytes can be 
      used for random number
    */
  i = 0;
  pBtBuffer[i++]= 0x01; /* version */;
  pBtBuffer[i++] = 0x06; /* Length */;
  pBtBuffer[i++] = 0x00; /* addr 1 */;
  pBtBuffer[i++] = 0x1d; /* addr 2 */;
  
  Status = pEfiQcomEngProtocol->GetRNG(pEfiQcomEngProtocol, &AlgoID, RngBufSize, pRngBuffer);
  HANDLE_ERROR_LABEL(Status, pEfiQcomEngProtocol->GetRNG, QrdProvStage1Exit);

  for (j = 0; j < RngBufSize; j++)
  {
     pBtBuffer[i++]= pRngBuffer[j]; /* addr 3 ~ 6 */
  }

  /* Write BT provision file */
  Status = DppItemWrite(L"QCOM", BT_PROV_FILENAME, (UINT8*)pBtBuffer, BT_MAC_PROV_FILE_SIZE);
  HANDLE_ERROR_LABEL(Status, DppItemWrite, QrdProvStage1Exit);
  AsciiPrint("  Provisioning BT MAC address completed.\n");
  /* allocate WLAN prov file buffer */
  pWlanBuffer = (UINT8 *) AllocatePool(WLAN_MAC_PROV_FILE_SIZE);
  if(pWlanBuffer == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto QrdProvStage1Exit;
  }
  ZeroMem(pWlanBuffer, WLAN_MAC_PROV_FILE_SIZE);

  /* Create WLAN MAC address
        The random MAC for WLAN uses Qualcomm OUI (first 3 octets == 00-A0-C6) 
        with the last 3 Octets can be randomized.
  */
  i = 0;
  pWlanBuffer[i++]= 0x01; /* version */;
  pWlanBuffer[i++]= 0x19; /* Length */;
  pWlanBuffer[i++]= 0x04; /* number of MAC */;

  for (k = 0; k < 4; k++)
  {
    if ( i == WLAN_MAC_PROV_FILE_SIZE )
    {
      break;
    }
    pWlanBuffer[i++]= 0x00; /* addr 1 */;
    pWlanBuffer[i++]= 0xa0; /* addr 2 */;
    pWlanBuffer[i++]= 0xc6; /* addr 3 */;

     Status = pEfiQcomEngProtocol->GetRNG(pEfiQcomEngProtocol, &AlgoID, RngBufSize, pRngBuffer);
     HANDLE_ERROR_LABEL(Status, pEfiQcomEngProtocol->GetRNG, QrdProvStage1Exit);

     for (j = 0; j < RngBufSize -1 ; j++)
     {
       pWlanBuffer[i++]= pRngBuffer[j]; /* addr 4 ~ 6 */
     }
  }

  /* Write WLAN provision file */
  Status = DppItemWrite(L"QCOM", WLAN_PROV_FILENAME, (UINT8*)pWlanBuffer, WLAN_MAC_PROV_FILE_SIZE);
  HANDLE_ERROR_LABEL(Status, DppItemWrite, QrdProvStage1Exit);
  AsciiPrint("  Provisioning WLAN MAC address completed.\n");
QrdProvStage1Exit:

  if (pBufferIn != NULL)
  {
    gBS->FreePool(pBufferIn);
    pBufferIn = NULL;
  }

  if (pBtBuffer != NULL)
  {
    gBS->FreePool(pBtBuffer);
    pBtBuffer = NULL;
  }
  if (pWlanBuffer != NULL)
  {
    gBS->FreePool(pWlanBuffer);
    pWlanBuffer = NULL;
  }
  return Status;
}

