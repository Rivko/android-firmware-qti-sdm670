/** @file UpdaterMain.h

  STMicro QiNEMO Firmware Updater DXE
  
  Copyright (c) 2013, Qualcomm Technologies, Inc. All rights reserved.

**/

#ifndef _UPDATER_MAIN_H_
#define _UPDATER_MAIN_H_


// STMicro QINEMO Firmware Class GUID
// {38298766-e6cd-4136-afac-861ed93de9a2}
#define ST_QINEMO_FW_CLASS_GUID \
{ \
  0x38298766, 0xe6cd, 0x4136, { 0xaf, 0xac, 0x86, 0x1e, 0xd9, 0x3d, 0xe9, 0xa2 } \
}

// Microsoft-defined GUID to identify ESRT table in EFI configuration table 
#define ESRT_GUID \
{ \
  0xB122A263, 0x3661, 0x4F68, { 0x99, 0x29, 0x78, 0xf8, 0xb0, 0xd6, 0x21, 0x80 } \
}

//Qcom-defined GUID for signaling device updater 
#define QCOM_FW_UPDATE_OEM_UPDATE_EVENT_GUID \
{ \
  0x592515FE, 0xF062, 0x4AD0, { 0x89, 0xC8, 0x91, 0x18, 0xF0, 0xF2, 0xD6, 0xBF } \
}

typedef enum {
  FW_TYPE_UNKNOWN,
  FW_TYPE_SYSTEM,
  FW_TYPE_DEVICE,
  FW_TYPE_DRIVER
} ESRT_FWTYPE;

typedef enum {
  FW_LAST_ATMPT_STATUS_SUCCESS,
  FW_LAST_ATMPT_STATUS_UNSUCCESSFUL,
  FW_LAST_ATMPT_STATUS_INSUFF_RESOURCE,
  FW_LAST_ATMPT_STATUS_INCORRECT_VERSION,
  FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT,
  FW_LAST_ATMPT_STATUS_AUTHEN_ERROR,
  FW_LAST_ATMPT_STATUS_POWER_AC_NOT_CONNECTED,
  FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY
} ESRT_LAST_ATTEMPT_STATUS;

typedef struct
{

    EFI_GUID FWClass ;
    UINT32 FWType;
    UINT32 FWVer;
    UINT32 FwLowestSupportedVer;
    UINT32 CapsuleFlag;
    UINT32 LastAttemptVer;
    UINT32 LastAttemptStatus;

}ESRT_ENTRY;

typedef struct
{
    UINT32 FWResrcCnt;
    UINT32 FWResrcMax;
    UINT64 FWResrcVer;

}ESRT_TABLE_HEADER;

typedef struct _FW_PAYLOAD_VERSION
{
  UINT32 FwVer;
  UINT32 FwLowestSupportedVer;
} FW_PAYLOAD_VERSION, *PFW_PAYLOAD_VERSION;

#endif // _UPDATER_MAIN_H_
