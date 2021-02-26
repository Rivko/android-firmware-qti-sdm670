#ifndef __SDMGRMSGS_H__
#define __SDMGRMSGS_H__

/** @file SDMgrMsgs.h
 *
 * This file provides message structures for the communication between SD_MGR
 * in TZ and RPMB/GPT listeners on the non-secure side
 *
 * Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     ---------------------------------------------------------
  2015-10-19   jt      Update GPT messages version 
  2015-04-17   jt/rm   Add support for ufs rpmb
  2014-06-30   jt      Change the UINTN fields back to UINT32 
  2014-03-27   bn      Fixed 64-bit pointer size 
  2013-05-28   jt      Added reliable write count/version to read/write request
  2013-04-05   jt      Return listener buffer size as part of init 
  2013-02-19   jt      Added command for RPMB/GPT partitioning 
  2012-11-02   bb      Initial Version

 =============================================================================*/

#include <Uefi.h>

/*****************************************************************************
 *                                                                           *
 *      R P M B / G P T   M E S S A G I N G   V E R S I O N                  *
 *                                                                           *
 *****************************************************************************/

/* This is used to ensure SD manager in TZ and RPMB/GPT listeners are in
   sync with the messaging structures. Since the components are in
   different software images, this will help to figure out the incompatibility
   between different components */

#define SD_MGR_GPT_MSGS_VERSION  0x02
#define SD_MGR_RPMB_MSGS_VERSION 0x02

/*****************************************************************************
 *                                                                           *
 *              R P M B / G P T   L I S T E N  E R   I D                     *
 *                                                                           *
 *****************************************************************************/

/* RPMB Listner ID */
#define SD_MGR_RPMB_LISTENER_ID   0x2000

/* GPT Listner ID */
#define SD_MGR_GPT_LISTENER_ID    0x2001

/*****************************************************************************
 *                                                                           *
 *                 V A L I D   S T A T U S   V A L U E S                     *
 *                                                                           *
 *****************************************************************************/

/* Possible Status values between sd_mgr in TZ and listener */
#define SD_MGR_MSG_SUCCESS                 0 /* Success */
#define SD_MGR_MSG_ERROR                  -1 /* Generic Failure */
#define SD_MGR_MSG_INVALID_PARAM_ERROR    -2 /* Invalid arguments passed */
#define SD_MGR_MSG_DEVICE_NOT_FOUND_ERROR -3 /* Device Not found Error */
#define SD_MGR_MSG_PARTI_NOT_FOUND_ERROR  -4 /* Partition Not found Error */
#define SD_MGR_MSG_VERSION_MISMATCH       -5 /* TZ-Listner Version mismatch */
#define SD_MGR_MSG_OUT_OF_RESOURCES       -6 /* Out of memory/other resources*/
#define SD_MGR_MSG_NOT_SUPPORTED          -7 /* Operation Not supported */

/*****************************************************************************
 *                                                                           *
 *                   R P M B / G P T   C O M M A N D S                       *
 *                                                                           *
 *****************************************************************************/

typedef enum
{
  SD_MGR_MSG_RPMB_INIT_ID = 0x101,  /* RPMB partition init command */
  SD_MGR_MSG_RPMB_READ_ID,          /* RPMB read sectors command */
  SD_MGR_MSG_RPMB_WRITE_ID,         /* RPMB write sectors command */
  SD_MGR_MSG_RPMB_PARTITION_ID,     /* RPMB partitioning command */
  SD_MGR_MSG_GPT_PARTI_INIT_ID = 0x401,     /* GPT partition init command */
  SD_MGR_MSG_GPT_PARTI_READ_ID,             /* GPT read sectors comamnd */
  SD_MGR_MSG_GPT_PARTI_WRITE_ID,            /* GPT write sectors command */
  SD_MGR_MSG_GPT_PARTITION_ID              /* GPT partitioning command */
} SDMgrMsgsType;


/*****************************************************************************
 *                                                                           *
 *             S T O R A G E   D E V I C E   T Y P E S                       *
 *                                                                           *
 *****************************************************************************/

typedef enum
{
  SD_MGR_DEVICE_EMMC_USER = 0,    /**< User Partition in eMMC */
  SD_MGR_DEVICE_EMMC_BOOT1,       /**< Boot1 Partition in eMMC */
  SD_MGR_DEVICE_EMMC_BOOT0,       /**< Boot2 Partition in eMMC */
  SD_MGR_DEVICE_EMMC_RPMB,        /**< RPMB Partition in eMMC */
  SD_MGR_DEVICE_EMMC_GPP1,        /**< GPP1 Partition in eMMC */
  SD_MGR_DEVICE_EMMC_GPP2,        /**< GPP2 Partition in eMMC */
  SD_MGR_DEVICE_EMMC_GPP3,        /**< GPP3 Partition in eMMC */
  SD_MGR_DEVICE_EMMC_GPP4,        /**< GPP4 Partition in eMMC */
  SD_MGR_DEVICE_EMMC_ALL,         /**< Entire eMMC */
  SD_MGR_UFS_RPMB,                /**< RPMB Partition in UFS device */
  SD_MGR_UFS_ALL,                 /**< Entire UFS device */
  SD_MGR_DEVICE_ID_MAX,           /**< Reserved: Indicates max. devices */
  SD_MGR_DEVICE_ID_RESERVED=0x7FFFFFFF /**< Reserved: Device ID Max */
} SDMgrDeviceType;


/*****************************************************************************
 *                                                                           *
 *                      R P M B   M E S S A G E S                            *
 *                                                                           *
 *****************************************************************************/

/* RPMB Init request message */
typedef struct TzSDRpmbInitReq
{
  UINT32 CmdID;           /* Command ID */
  UINT32 Version;         /* Messaging Version from sd_mgr in TZ */
} __attribute__ ((packed)) TzSDRpmbInitReqType;


/* RPMB Init response message */
typedef struct TzSDRpmbInitRsp
{
  UINT32 CmdID;         /* Command ID */
  UINT32 Version;       /* Messaging Version from RPMB listener */
  INT32 Status;        /* RPMB init Status */
  UINT32 NumSectors;    /* Number of sectors (512B) in RPMB */
  UINT32 RelWrCount;    /* Reliable write count for the RPMB */
} __attribute__ ((packed)) TzSDRpmbInitRspType;

/* RPMB Init response message - Version 2 */
typedef struct TzSDRpmbInitRsp_v02
{
  UINT32 CmdID;            /* Command ID */
  UINT32 Version;          /* Messaging version from RPMB listener */
  INT32 Status;            /* RPMB init status */
  UINT32 NumSectors;       /* Size of RPMB (in sectors) */
  UINT32 RelWrCount;       /* Reliable write count for the RPMB */
  UINT32 DevType;          /* Storage device type (like eMMC or UFS) */
  UINT32 Reserved1;        /* Reserved 1 */
  UINT32 Reserved2;        /* Reserved 2 */
  UINT32 Reserved3;        /* Reserved 3 */
  UINT32 Reserved4;        /* Reserved 4 */
} __attribute__ ((packed)) TzSDRpmbInitRspType_v02;


/* RPMB read/write request message */
typedef struct TzSDRpmbRWReq
{
  UINT32 CmdID;          /* Command ID */
  UINT32 NumHalfSectors; /* Number of half-sectors to read/write */
  UINT32 ReqBuffLen;     /* Buffer length */
  UINT32 ReqBuffOffset;  /* Offset to RPMB frames in the reqest buffer */
  UINT32 Version;        /* Message version from sd_mgr in TZ */
  UINT32 RelWrCount;     /* Reliable write count */
} __attribute__ ((packed)) TzSDRpmbRWReqType;


/* RPMB read/write response message */
typedef struct TzSDRpmbRWRsp
{
  UINT32 CmdID;         /* Command ID */
  INT32  Status;        /* Status from read/write operation */
  UINT32 RspBuffLen;    /* Response buffer length */
  UINT32 RspBuffOffset; /* Offset to RPMB frames in the response buffer */
  UINT32 Version;       /* Message version from listener */
} __attribute__ ((packed)) TzSDRpmbRWRspType;


/* RPMB partitioning request message */
typedef struct TzSDRpmbPartitionReq
{
  UINT32 CmdID;           /* Command ID */
  UINT32 Version;         /* Partition Table Version */
  UINT32 DevID;           /* Device ID */
} __attribute__ ((packed)) TzSDRpmbPartitionReqType; 


/* RPMB partitioning response message */
typedef struct TzSDRpmbPartitionRsp
{
  UINT32 CmdID;           /* Command ID */
  INT32  Status;          /* Status from RPMB partitioning */
  UINT32 NumPartitions;   /* Number of partitions added */
  UINT32 RspBuffOffset;   /* Offset to the partition addition info */     
} __attribute__ ((packed)) TzSDRpmbPartitionRspType;


/*****************************************************************************
 *                                                                           *
 *                        G P T   M E S S A G E S                            *
 *                                                                           *
 *****************************************************************************/

/* Initialize GPT partition request message */
typedef struct TzSDGptInitReq
{
  UINT32 CmdID;         /* Command ID */
  UINT32 Version;       /* Messaging Version from sd_mgr in TZ */
  UINT32 DevID;         /* Device ID - Corresponds physical partition ID */
  UINT8  GptGuid[16];   /* GUID for the partition to be opened */
} __attribute__ ((packed)) TzSDGptInitReqType;


/* Initialize GPT partition response message */
typedef struct TzSDGptInitRsp
{
  UINT32 CmdID;         /* Command ID */
  UINT32 Version;       /* Messaging Version from GPT listener */
  INT32  Status;        /* RPMB init Status */
  UINT32 NumSectors;    /* Number of sectors in GPT partition */
  UINT32 ListBufSize;   /* Listener Buffer Size (in bytes) */
  UINT32 CtxtID;        /* Context ID/Handle for the GPT partition */
} __attribute__ ((packed)) TzSDGptInitRspType;


/* Initialize GPT partition response message- version 2 */
typedef struct TzSDGptInitRspV02
{
  UINT32 CmdID;          /* Command ID */
  UINT32 Version;        /* Messaging Version from GPT listener */
  INT32  Status;         /* RPMB init Status */
  UINT32 NumSectors;     /* Number of sectors in GPT partition */
  UINT32 ListBufSize;    /* Listener Buffer Size (in bytes) */
  UINT32 CtxtID;         /* Context ID/Handle for the GPT partition */
  UINT32 BytesPerSector; /* Bytes per sector of the storage medium */ 
  UINT32 Reserved1;      /* Reserved 1 */
  UINT32 Reserved2;      /* Reserved 2 */
  UINT32 Reserved3;      /* Reserved 3 */
  UINT32 Reserved4;      /* Reserved 4 */
} __attribute__ ((packed)) TzSDGptInitRspTypeV02;


/* GPT partition read/write request message */
typedef struct TzSDGptRWReq
{
  UINT32 CmdID;        /* Command ID */
  UINT32 CtxtID;       /* Context ID/Handle for the GPT partition */
  UINT32 StartSector;  /* Start sector for R/W operation */
  UINT32 NumSectors;   /* Number of sectors to read/write */
  UINT32 ReqBuffLen;   /* Buffer length */
  UINT32 ReqBuffOffset;/* Offset to RPMB frames in the reqest buffer */
} __attribute__ ((packed)) TzSDGptRWReqType;


/* GPT Partition read/write response message */
typedef struct TzSDGptRWRsp
{
  UINT32 CmdID;         /* Command ID */
  UINT32 CtxtID;        /* Context ID/Handle for the GPT partition */
  INT32  Status;        /* Status from read/write operation */
  UINT32 RspBuffLen;    /* Response buffer length */
  UINT32 RspBuffOffset; /* Offset to data in the response bufefr */
} __attribute__ ((packed)) TzSDGptRWRspType;

/* GPT partitioning request message */
typedef struct TzSDGptPartitionReq
{
  UINT32 CmdID;               /* Command ID */
  UINT32 Version;             /* Partition Table Version */
  UINT32 DevID;               /* Device ID - Corresponds physical partition ID */
  UINT8  PartitionGuid[16];   /* GUID for the partition */
} __attribute__ ((packed)) TzSDGptPartitionReqType; 


/* GPT partitioning response message */
typedef struct TzSDGptPartitionRsp
{
  UINT32 CmdID;           /* Command ID */
  INT32  Status;          /* Status from GPT partitioning */
  UINT32 NumPartitions;   /* Number of partitions added */
  UINT32 RspBuffOffset;   /* Offset to the partition addition info */     
} __attribute__ ((packed)) TzSDGptPartitionRspType;


/* Get TZ Error code from non-secure error code */
int TZSDMgrGetErrorCode(EFI_STATUS ErrorCode);


/* Check RPMB Listener Version with TZ
   Return 1 if both TZ and listener use same Version
   Return 0 if there is a diff. between TZ and listener */
int TZSDMgrRpmbVersionCheck(UINT32 Version);

/* Check GPT Listener Version with TZ
   Return 1 if both TZ and listener use same Version
   Return 0 if there is a diff. between TZ and listener */
int TZSDMgrGptVersionCheck(UINT32 Version);

#endif /* __SDMGRMSGS_H__ */
