/** @file DeviceUpdaterSkeleton.h

  A skeleton device firmware updater driver that demonstrates 
  device firmeare update solution using UpdateCapsule. 
  
  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 --------     ---     --------------------------------------------------------
 07/04/12     Mic     Removed dependency on FwCommonLib and FwUpdateLib
 06/14/12     jd/MiC  Initial version
=============================================================================*/

#ifndef _DEVICE_UPDATER_SKELETON_H_
#define _DEVICE_UPDATER_SKELETON_H_


// Dummy Device Firmware Class GUID: {1E7C3B59-F602-4D74-A531-060ABAEEC51A}
#define DUMMY_DEVICE_FW_CLASS_GUID \
{ \
  0x1e7c3b59, 0xf602, 0x4d74, { 0xa5, 0x31, 0x06, 0x0a, 0xba, 0xee, 0xc5, 0x1a } \
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

typedef struct _DUMMY_DEVICE_PAYLOAD_VERSION
{
  UINT32 FwVer;
  UINT32 FwLowestSupportedVer;
} DUMMY_DEVICE_PAYLOAD_VERSION;

#endif // _DEVICE_UPDATER_SKELETON_H_
