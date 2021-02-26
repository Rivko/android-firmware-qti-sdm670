
#ifndef _SCMAPP_H
#define _SCMAPP_H

/** @file ScmApp.h

  UEFI application to test the ScmDxe

  Copyright (c) 2012 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/13/12   shl     Switched to new files and interfaces
 12/12/12   xun     Initial version

=============================================================================*/

// service command id
#define TZEXEC_SVC_TESTSRV_ID               0x0001000
#define TZEXEC_SVC_UEFI_AUTH_ID             0x0002000
#define TZEXEC_TPM_SVC_ID                   0x0004000
#define TZEXEC_SVC_UEFI_VAR_ID              0x0008000
#define TZEXEC_SVC_UEFI_RPMB_ID             0x0010000
#define TZEXEC_SVC_CRYPTO_ID                0X0020000

#define TESTSRV_CREATE_CMD(x)              (TZEXEC_SVC_TESTSRV_ID | x)

#define TESTSRV_GET_ENCRYPTED_BLOB         TESTSRV_CREATE_CMD(1)


EFI_GUID TzExecPartitionType = { 0x69B4201F, 0xA5AD, 0x45EB, { 0x9F, 0x49, 0x45, 0xB3, 0x8C, 0xCD, 0xAE, 0xF5}};

/*------------------------------ EncBlobTest App ----------------------------------------*/
#define ENCRYPTED_BLOB_SIZE                32
typedef struct _TESTSRC_ENCRYPTED_BLOB_REQ
{
 /* Command ID */
  UINT32            commandId;
} __attribute__ ((packed)) TESTSRC_ENCRYPTED_BLOB_REQ, *PTR_TESTSRC_ENCRYPTED_BLOB_REQ;

typedef struct _TESTSRC_ENCRYPTED_BLOB_RSP
{
 /* status indicate ERROR */
  INT32            status;
  //len of buf
  UINT32           len;
  //buffer with encrypted data
  UINT8            buf[ENCRYPTED_BLOB_SIZE];
} __attribute__ ((packed)) TESTSRC_ENCRYPTED_BLOB_RSP, *PTR_TESTSRC_ENCRYPTED_BLOB_RSP;
/*--------------------------------------------------------------------------------------*/

#endif //_SCMAPP_H

