/** @file MpPowerLib.c

  This file implements specialized chip specific code for Multicore
  protocol

  Copyright (c) 2015-17, Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/MpPowerLib/MpPowerLib.c#2 $
  $DateTime: 2017/09/21 10:45:06 $
  $Author: pwbldsvc $

=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIPmicVreg.h>
#include <Library/IoLib.h>
#include <Library/MpPowerLib.h>
#include <Protocol/EFIPlatformInfo.h>
#include <Protocol/EFISPMI.h>
#include <Library/DebugLib.h>  

#define APC1_NOM_VOLTAGE     828
#define MISC_PMIC_OPT_ADDR   0x95C /*PMIC address to read the buck configuration*/
#define SINGLEPHASE          0x03  /* Buck configuration value for single phase*/   

/**
  Chipset-specific initialization.
  @return - EFI_SUCCESS on success, otherwise error status
*/
EFI_STATUS MpPowerInit (VOID)
{
  return EFI_SUCCESS;
}

/**
  Chipset-specific multicore enablement

  @param[in]  CoreNum - number of cores
  @return - EFI_SUCCESS on success, otherwise error status
*/
EFI_STATUS MpPower (UINTN  CoreNum)
{
  EFI_QCOM_PMIC_VREG_PROTOCOL  *PmicVRegProtocol = NULL;
  EFI_PLATFORMINFO_PROTOCOL  *PlatformInfoProtocol = NULL;
  EFI_QCOM_SPMI_PROTOCOL *SpmiProtocol = NULL;
  EFI_PLATFORMINFO_PLATFORM_INFO_TYPE  PlatformInfo = {0};
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PM_VREG_STATUS_TYPE VregStatus;
  EFI_PM_VREG_ID_TYPE PmicVregID = EFI_PM_SMPS_3; /*By default, buck configuration for APC1 rail will be dual phase. So, SMPS3A needs to be turned on*/
  INT32 RetryCnt = 200; /* Only give PMIC 2ms to adjust otherwise HW error */
  UINT8 PhaseConfig = 0x0;
  Spmi_Result Result = SPMI_SUCCESS;

  /* Acquire PMIC Vreg EFI Protocol handle */
  Status = gBS->LocateProtocol( &gQcomPmicVregProtocolGuid,
                                NULL, (VOID **)&PmicVRegProtocol );
  if (EFI_ERROR(Status))
  {
    return EFI_NOT_FOUND;
  }

  /* Acquire Platform info Protocol handle */
  Status = gBS->LocateProtocol ( &gEfiPlatformInfoProtocolGuid, NULL,
                                (VOID **) &PlatformInfoProtocol);
  if (EFI_ERROR(Status))
  {
    return EFI_NOT_FOUND;
  }

  Status = PlatformInfoProtocol->GetPlatformInfo(PlatformInfoProtocol,
                                                 &PlatformInfo);
  if (EFI_ERROR(Status))
  {
    return Status;
  }  
  
  /* Get SPMI Protocol */
  Status = gBS->LocateProtocol(&gQcomSPMIProtocolGuid, NULL, (VOID **)&SpmiProtocol);
  if ( EFI_ERROR (Status) )
  {
    DEBUG(( EFI_D_ERROR, "MemMapDump: failed to locate SPMIProtocol, Status = (0x%x)\r\n", Status));
    return Status;
  } 
  
  /* Read PMIC register value */
  Result = SpmiProtocol->ReadLong(SpmiProtocol, 0, SPMI_BUS_ACCESS_PRIORITY_LOW, MISC_PMIC_OPT_ADDR, (UINT8*)&PhaseConfig, (UINT32)1, 0);
  if ( Result != SPMI_SUCCESS )
  {
    DEBUG(( EFI_D_ERROR, "SPMI read failed \n"));
  }

  if(SINGLEPHASE == PhaseConfig)   // For single phase buck configuration, SMPS2A needs to be turned on for APC GOLD cluster
  {
    PmicVregID = EFI_PM_SMPS_2;
  }

  /* Set Gold Cluster Rail to Nominal */
  Status = PmicVRegProtocol->VregSetLevel(0, PmicVregID, APC1_NOM_VOLTAGE);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  /* Enable Gold Cluster Rail */
  Status = PmicVRegProtocol->VregControl(0, PmicVregID, TRUE);
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  /* Return immediatly if this is a Emulation Platform */
  if ((PlatformInfo.platform ==  EFI_PLATFORMINFO_TYPE_RUMI) ||
      (PlatformInfo.platform ==  EFI_PLATFORMINFO_TYPE_VIRTIO))
  {
    return EFI_SUCCESS;
  }

  /* Wait for rail to come up */
  do
  {
    if (RetryCnt-- <= 0)
    {
      ASSERT(0);
    }

    gBS->Stall(10);
    Status = PmicVRegProtocol->VregGetStatus(0, PmicVregID,  &VregStatus);
    if (EFI_ERROR(Status))
    {
      return Status;
    }
  } while (!(VregStatus.VregOk));

  return EFI_SUCCESS;
}
