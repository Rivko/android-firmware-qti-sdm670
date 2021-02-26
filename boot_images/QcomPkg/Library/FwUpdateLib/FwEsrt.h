/** @file FwESRT.h
   
   Header for functions to maintain the ESRT , including ESRT
   header and ESRT entry .
  
  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who    what, where, why
 --------       ---    --------------------------------------------------
 2015/03/31     mic    Added 8994 platform types
 2013/04/11     rsb    Added 8084 platform types
 2012/11/30     mic    Added ESRT entry print function
 2012/11/09     jmb    Adding CBH platform
 2012/10/21     mic    Code review
 2012/08/14     ash    Added 8064 WB292 and V1 variants
 2012/06/18     jd     rename OEM_UPDATE_EVENT_GUID 
 2012/06/05     rs     Added UXCapsule OEM Update Event GUID
 2012/05/09     rsb    Platform run-time type detection for FW Resource GUID
 2012/05/07     jd     Migrated to DPP Protocol.
 2012/02/01     mic    Initial version

=============================================================================*/
#ifndef __QCOM_FW_ESRT_H_
#define __QCOM_FW_ESRT_H_

#define QCOM_FW_UPDATE_PRODUCT_VERSION_10 0x00010000
#define QCOM_FW_UPDATE_PRODUCT_VERSION_11 0x00010001
#define QCOM_FW_UPDATE_PRODUCT_VERSION_12 0x00010002

/* System Firmware Resource GUID for 8996 MTP: {7569FC0D-4352-4269-BE07-9056DFD34716} */
#define QCOM_FW_UPDATE_SYSTEM_FW_8996_MTP_GUID \
{ \
  0x7569FC0D, 0x4352, 0x4269, { 0xBE, 0x07, 0x90, 0x56, 0xDF, 0xD3, 0x47, 0x16 } \
}


/* System Firmware Resource GUID for UNKNOWN platform type: {06CE6651-B594-4AC3-B57A-D9637CC335A8} */

#define QCOM_FW_UPDATE_SYSTEM_FW_UNKNOWN_GUID \
{ \
  0x06CE6651, 0xB594, 0x4AC3, { 0xB5, 0x7A, 0xD9, 0x63, 0x7C, 0xC3, 0x35, 0xA8 } \
}

/* UX Capsule GUID : {3b8c8162-188c-46a4-aec9-be43f1d65697}  */

#define QCOM_FW_UPDATE_UX_CAPSULE_GUID \
{ \
  0x3B8C8162, 0x188C, 0x46A4, { 0xAE, 0xC9, 0xBE, 0x43, 0xF1, 0xD6, 0x56, 0x97 } \
}

#define QCOM_FW_UPDATE_ESRT_GUID \
{ \
  0xB122A263, 0x3661, 0x4F68, { 0x99, 0x29, 0x78, 0xf8, 0xb0, 0xd6, 0x21, 0x80 } \
}

/*{592515FE-F062-4AD0-89C8-9118F0F2D6BF}*/
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

/* The list of System Firmware Resource types for each platform, this enumeration must match 
   one to one with qcomFwResourceGUIDs. */
typedef enum _QCOM_FW_RESOURCE_TYPE
{
  QCOM_FW_UPDATE_SYSTEM_FW_8996_MTP_TYPE,
  QCOM_FW_UPDATE_SYSTEM_FW_UNKNOWN_TYPE

} QCOM_FW_RESOURCE_TYPE;

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

/**
 * Function to print ESRT entry 
 * 
 * @param ent  - ESRT entry
 * @DebugLevel - Debug level 
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
QcomPrintEsrtEntry(
  IN ESRT_ENTRY *ent, 
  IN UINTN      DebugLevel
  );

/**
* Get the ESRT entry info 
*
* @param  ent         - the address of the pointer which points to ESRT entry
* @return EFI_STATUS
**/
EFI_STATUS
QcomGetEsrtEntry(
  IN OUT ESRT_ENTRY  **ent
  );

/**
 * This Function try to update ESRT in both DPP and system table
 * 
 * @param  SystemTable   - Passed from calling application
 * @param  attemptStatus - Passed from the calling application
 *
 * @return EFI_STATUS 
 **/
EFI_STATUS
QcomUpdateESRT(
  IN EFI_SYSTEM_TABLE         *SystemTable,
  IN ESRT_LAST_ATTEMPT_STATUS attemptStatus
  );

/**
 * This Function kicks off the ESRT  Phase of 
 * the Firmware update process. 
 * 
 * @param  SystemTable - Passed from calling application
 * 
 * @return EFI_STATUS 
 **/
EFI_STATUS
QcomESRTPhaseMain(
  IN OUT EFI_SYSTEM_TABLE  *SystemTable
  );

#endif
