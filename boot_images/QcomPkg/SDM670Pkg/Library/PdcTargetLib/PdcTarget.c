/*============================================================================
  FILE:         PdcTarget.c

  OVERVIEW:     Implementation of Pdc init in boot

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/BootConfig.h>
#include "DALSys.h"
#include "PdcLib.h"
#include "DALStdDef.h"
#include <assert.h>
#include "comdef.h"
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPlatformInfo.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
/* PROTOCOL HANDLES */
EFI_CHIPINFO_PROTOCOL      *ChipInfoProtocol = NULL;
EFI_PLATFORMINFO_PROTOCOL  *PlatformInfoProtocol = NULL;

EFI_PLATFORMINFO_PLATFORM_INFO_TYPE  PlatformInfo = {0};
EFIChipInfoIdType ChipInfo = EFICHIPINFO_ID_UNKNOWN ;
uint32            ChipVersion = 0x0;

extern void pdcLog_initialize(void);
extern void pdcTcs_initialize(void);
extern void pdc_seq_sys_init(void);

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/* DEBUG */
volatile uint32 uDebugMode = 0x0;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static void LoadSysInfo(void)
{
  EFI_STATUS Status;

  /* Get Protocol handles */
  Status = gBS->LocateProtocol (&gEfiChipInfoProtocolGuid, NULL,
                               (VOID **) &ChipInfoProtocol);
  if (Status == EFI_SUCCESS)
  {
    ChipInfoProtocol->GetChipVersion(ChipInfoProtocol, &ChipVersion);
    ChipInfoProtocol->GetChipId(ChipInfoProtocol, &ChipInfo);
  }

  Status = gBS->LocateProtocol ( &gEfiPlatformInfoProtocolGuid, NULL,
                                (VOID **) &PlatformInfoProtocol);
  if (Status == EFI_SUCCESS)
  {
    PlatformInfoProtocol->GetPlatformInfo(PlatformInfoProtocol, &PlatformInfo);
  }
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
   @brief Initializes Pdc library.

   Initialize the Pdc library for core.

   @return EFI_STATUS
 */
EFI_STATUS PdcTargetInit(void)
{
  EFI_STATUS status = EFI_SUCCESS;

  DALSYS_InitMod(NULL);
  LoadSysInfo();

  pdcLog_initialize();
  pdcTcs_initialize();
  pdc_seq_sys_init();

  return status;
}

