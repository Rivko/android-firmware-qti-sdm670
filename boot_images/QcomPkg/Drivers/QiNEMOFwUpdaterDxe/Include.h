/** @file Include.h

  STMicro QiNEMO Firmware Updater Header
  
  Copyright (c) 2013, Qualcomm Technologies, Inc. All rights reserved.

**/

#pragma once

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

/*===========================================================================
                        CONST and MACRO DEFINITIONS
===========================================================================*/

#define MINIMUM_BATTERY_CHARGE_PERCENT 25

/**
  Define ALLOW_NO_BATTERY_FIRMWARE_UPDATE to allow firmware update to
  proceed for non-production mode devices that have no battery and only
  DC power adapter connected.

  The logic will only permit this behavior for non-production mode
  devices for use in lab automation testing. Devices that are production
  mode will not be permitted to update when no battery is present,
  even if the macro is defined.
  */
#define ALLOW_NO_BATTERY_FIRMWARE_UPDATE

/*===========================================================================
                                   FUNCTIONS
===========================================================================*/

// Init
EFI_STATUS InitFirmwareUpdate(VOID);
VOID DeInitFirmwareUpdate(VOID);

// General
BOOLEAN IsINT(VOID);
VOID DelayUs(UINT32 us);

// Battery
EFI_STATUS CheckPowerLevel(OUT BOOLEAN *bPowerGood);

// Payload
EFI_STATUS AuthenticateAndParsePayload(IN OUT UINT32 *pLastAttemptStatus, IN GUID *pTargetGUID,
             IN VOID *pPayload, IN UINTN PayloadSize, OUT UINT8 **ppFWBuffer, OUT UINTN *pFWSize);

// I2C
EFI_STATUS cmdWrite(UINTN WriteAddress, CONST UINT8 *buf, UINT32 nBytes);
EFI_STATUS cmdRead(UINTN WriteAddress, UINT8 *buf, UINT32 nBytes);
EFI_STATUS cmdWriteRead(UINTN WriteAddress, CONST UINT8 *wrBuf, UINT32 nwrBytes, UINTN ReadAddress, UINT8 *rdBuf, UINT32 nrdBytes);

// STMicro-Implemented Functions
EFI_STATUS PerformFirmwareUpdate(VOID *pPayload, UINT32 Size);
EFI_STATUS GetFirmwareUpdateStatus(UINT32 *LastAttemptStatus, UINT32 *FwVersion, UINT32 *LowestSupportedFwVersion);
EFI_STATUS GetCurrentFirmwareVersion(UINT32 *CurrentFwVersion);
