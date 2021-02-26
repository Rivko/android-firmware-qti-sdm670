#ifndef __TS_FWUPGRADE_DXE_H__
#define __TS_FWUPGRADE_DXE_H__

/** @file TsFwUpgradeDxe.h

  header file for Touch screen device firmware update using UpdateCapsule .
  
  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     --------------------------------------------------------
 04/24/13     dm      Added support for HIDI2C Communication
 03/22/13     dm      Added battery level checking
 02/07/13     dm      Fixed Warnings 
 08/09/12     dm      Added Check Power level structure
 06/20/12     sr      Initial version
=============================================================================*/

// Touch Screen Device Firmware Class GUID: {7D0F613D-595C-4161-BAF3-C0609EAF0294}

#define TS_DEVICE_FW_CLASS_GUID \
{ \
  0x7d0f613d, 0x595c, 0x4161, { 0xba, 0xf3, 0xc0, 0x60, 0x9e, 0xaf, 0x2, 0x94 }  \
}

// Microsoft-defined GUID to identify ESRT table in EFI configuration table 
#define ESRT_GUID \
{ \
  0xB122A263, 0x3661, 0x4F68, { 0x99, 0x29, 0x78, 0xf8, 0xb0, 0xd6, 0x21, 0x80 } \
}

//Qcom-defined GUID for signaling device updater 
#define QCOM_FW_UPDATE_UX_CAPSULE_OEM_GUID \
{ \
  0x592515FE, 0xF062, 0x4AD0, { 0x89, 0xC8, 0x91, 0x18, 0xF0, 0xF2, 0xD6, 0xBF } \
}
 
typedef unsigned char UCHAR;
typedef UCHAR* PUCHAR;
typedef unsigned long ULONG;
 
typedef struct _TS_DEVICE_PAYLOAD_VERSION
{
  UINT32 FwVer;
  UINT32 FwLowestSupportedVer;
} TS_DEVICE_PAYLOAD_VERSION;


EFI_STATUS
FwUpdateCheckPowerLevel(
  OUT BOOLEAN *bPowerGood,
  OUT ESRT_LAST_ATTEMPT_STATUS *lastAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
);
  
VOID
ParsePayload(
IN VOID *pPayload, 
IN UINTN PayloadSize, 
OUT UINT8 **pFWBuffer, 
OUT UINTN *FWSize, 
OUT UINT8 **pCFGBuffer,
OUT UINTN *CFGSize,
OUT UINTN *Version
);

extern void PowerUpAndInitController(void);

extern void ResetControllerForTouchOpMode(void);

extern EFI_STATUS
PerformFWUpgradeProc(PUCHAR Buffer, UINTN PayloadSize);

extern EFI_STATUS PerformTsCfgUpd(
   PUCHAR CfgBuffer,
   ULONG  CfgBufferSize
);
   
VOID
CalculateHexValue(
IN VOID *BufferPointer,
OUT UINTN *HexValue
);   

  

#endif // __TS_FWUPGRADE_DXE_H__
