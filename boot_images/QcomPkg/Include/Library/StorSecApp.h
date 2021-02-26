#ifndef __STORSEC_H__
#define __STORSEC_H__

/** @file StorSecApp.h
   
  This file provides definitions for the Storage Secure app. 
  
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.
 
**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     ---------------------------------------------------------
2017-09-11   jt      Update label name 
2016-08-16   jt      Initial Version 

=============================================================================*/

#define CMD_SET_SECURE_WP_CONFIG           0x0
#define CMD_GET_SECURE_WP_CONFIG           0x1

#define MAX_SECURE_WP_ENTRIES              0x4
#define SCM_BUFFER_SIZE                    0x1000 // 4KB

#define STORSECAPP_MBN_FILE                L"\\TZAPPS\\storsec.mbn"
#define STORSECAPP_MBN_LABEL               L"storsec_a"
#define STORSECAPP_NAME                    "storsec_a"

#define MAX_OS_TYPE_LEN                    0x4

/* Secure write protect request structure */
typedef struct 
{
  UINT32 CommandId;
  UINT32 DataSize;
  UINT32 DataOffset; 
} __attribute__ ((packed)) TzSvcSecureWpReq;

/* Secure write protect response structure */
typedef struct 
{
   UINT32 CommandId;
   UINT32 Status; 
   UINT32 DataSize;
   UINT32 DataOffset;  
} __attribute__ ((packed)) TzSvcSecureWpRsp;

/* Secure Write Protect Info Entry structure */
typedef struct
{
   UINT8  WpEnable;     /* UFS: WPF (Write Protect Flag), eMMC: SECURE_WP_MODE_ENABLE */
   UINT8  WpTypeMask;   /* UFS: WPT (Write Protect Type), eMMC: SECURE_WP_MODE_CONFIG */
   UINT64 Addr;         /* UFS: LBA, eMMC: 0x1/0x2 (address of the device config register) */
   UINT32 NumBlocks;    /* UFS: Num LBA, eMMC: Set to 0 */
} __attribute__ ((packed)) SdMgrSecureWpInfoEntry;

/* Secure Write Protect Info structure */
typedef struct
{
   UINT8  LunNumber;                                        /* UFS: LUN number, eMMC: Set to 0 */
   UINT8  NumEntries;                                       /* Number of Secure wp entries */
   SdMgrSecureWpInfoEntry WpEntries[MAX_SECURE_WP_ENTRIES]; /* Max 4 entries total */
} __attribute__ ((packed)) SdMgrSecureWpInfo;

/**
  This function sends requests to the storage secure application 
**/
EFI_STATUS
SendStorSecAppRequest(VOID *Req, UINT32 ReqLen, VOID *Resp, UINT32 RespLen);

/**
  Gets the Secure Write Protect Configuration Block 
**/
EFI_STATUS 
UFSGetSecureWpConfigBlock(SdMgrSecureWpInfo *ConfigBlk); 

/**
  Set the Secure Write Protect Configuration Block 
**/
EFI_STATUS 
UFSSetSecureWpConfigBlock(SdMgrSecureWpInfo *ConfigBlk); 

#endif /* __STORSEC_H__ */

