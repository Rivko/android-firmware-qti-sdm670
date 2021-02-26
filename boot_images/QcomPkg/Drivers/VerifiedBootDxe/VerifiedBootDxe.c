/** @file
                       VerifiedBootDxe.c

  This driver produces verified boot protocol.

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 09/15/16   SA      Use ASN1X509 Protocol
 07/28/16   SA      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Protocol/EFIVerifiedBoot.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/OfflineCrashDump.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/UefiCfgLib.h>
#include <Protocol/EFITzeLoader.h>
#include <Protocol/EFIQseecom.h>
#include <Protocol/EFIASN1X509.h>
#include <Protocol/Hash2.h>
#include <Base.h>
#include "boot_verifier.h"
#include "scm_util.h"
#include "img_util.h"
#include "km_defs.h"
#include "hash_defs.h"

/* The data structure to keep the boot status (READ/GREEN/YELLOW/ORANGE) */
STATIC boot_state_t dev_boot_state = BOOT_STATE_MAX;

/* The data stucture to keep the lock state of device */
STATIC device_info_vb_t dev_lock_state = {0};

/* Global variable to keep OEM certificate (read from the header file) */
STATIC CERTIFICATE oem_certificate = {0};

/* Global varaible to keep Android Verified Boot Signature (appended to the image) */
STATIC VERIFIED_BOOT_SIG embedded_vb_signature = {0};

/* Global varaible for tracking whether QCOM_VB_VerifyImage API is invoked successfully or not */
STATIC BOOLEAN image_verifiction_called = FALSE;

/* Global variable for tracking whether QCOM_VB_DeviceInit API is invoked successfully or not */
STATIC BOOLEAN is_device_init_called = FALSE;

/* Global variable needed for sending ROT in YELLOW/RED states */
STATIC BOOLEAN cert_verification_passed = FALSE;

/* Application ID for keymaster */
STATIC UINT32 keymasterAppId = 0;

/* ASN1X509 Protocol */
STATIC QCOM_ASN1X509_PROTOCOL *pEfiQcomASN1X509Protocol = NULL;

/* boot image version and patch level to set bootstate */
STATIC UINT32 bootstate_os_version = 0;

#define EFI_FAILURE (80)
#define PAGE_SIZE 4096
#define TZ_FVER_QSEE 10 /* QSEE application layer */

typedef struct {
        UINT32 is_unlocked;
        CHAR8 public_key[VB_SHA256_SIZE];
        UINT32 color;
        UINT32 system_version;
        UINT32 system_security_level;
}__attribute__((packed)) KMBootState;
	
/**
  Data structure for sending KEYMASTER_SET_BOOT_STATE command to keymaster

  @param[in]   cmd_id      Requested command
  @param[in]   bs_version  boot state version.
  @param[in]   bs_offset   Offset from the top of the struct.
  @param[in]   bs_size     Size of the boot state.
  @param[in]   boot_state  boot state information.

**/
typedef struct _km_boot_state_req_t {
        UINT32 cmd_id;
        UINT32 bs_version;
        UINT32 bs_offset;
        UINT32 bs_size;
        KMBootState boot_state;
}__attribute__((packed)) km_boot_state_req;

/**
  Data structure for receiving the response of KEYMASTER_SET_BOOT_STATE command

  @param[in]   status     Status of the request

**/
typedef struct _km_boot_state_rsp_t {
        INT32 status;
} __attribute__((packed)) km_boot_state_rsp;

typedef struct __sechsh_ctx_s sha256_ctx;


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

/**
* Initialize Verified Boot
*
* @param device_info_vb_t  *devinfo
*   Device info data
*
* @return int
*   Success:	handle to be used for all calls to
*                       Secure app. Always greater than zero.
*   Failure:	Error code (negative only).
*/

EFI_STATUS
QCOM_VB_DeviceInit(IN QCOM_VERIFIEDBOOT_PROTOCOL *This,
                   IN device_info_vb_t *device_info)
{

  EFI_STATUS err = EFI_FAILURE;

  if (device_info == NULL) {
    err = EFI_INVALID_PARAMETER;
    goto exit;
  }
  err = gBS->LocateProtocol(&gEfiQcomASN1X509ProtocolGuid, NULL, (VOID **)&pEfiQcomASN1X509Protocol);
  if (err != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: DeviceInit: Locate ASN1X509 failed!\n"));
    goto exit;
  }
  dev_lock_state = *device_info;

  /* Check if device is unlocked */
  if (dev_lock_state.is_unlocked) {
    dev_boot_state = ORANGE;
    DEBUG((EFI_D_ERROR, "VB: DeviceInit: Device is unlocked! Skipping verification!\n"));
  } else {
    dev_boot_state = GREEN;
  }
  err = EFI_SUCCESS;
exit:
  /* Only change the value of is_device_init_called if the value of dev_lock_state is written by device_info */
  if (err == EFI_SUCCESS) {
    is_device_init_called = TRUE;
  }
  return err;
}

/**
* Read/ Write Device state info for VB
* For Write: The in_buf has the data to be written
* and the size of the data is provided in in_buf_len.
* out_buf will be ignored
* For Read: The api will return data read in out_buf
* and the size of the data is provided in out_buf_len.
* in_buf will be ignored
*
* @param vb_device_state_op_t    op
*   Enum to select read or write.
*
* @param void *buf
*   Pointer to the buffer for read and write
*
*
* @param uint32_t *buf_len
*   Size of the buffer
*
* @return int
*   Success:	handle to be used for all calls to
*                       Secure app. Always greater than zero.
*   Failure:	Error code (negative only).
*/

EFI_STATUS
QCOM_VB_RWDeviceState(IN QCOM_VERIFIEDBOOT_PROTOCOL *This,
                      IN vb_device_state_op_t op, IN OUT UINT8 *buf,
                      IN UINT32 buf_len)
{

  EFI_STATUS status = EFI_FAILURE;
  UINT8 img_name[MAX_PNAME_LENGTH];
  UINT16 img_label[MAX_LABEL_SIZE];
  BOOLEAN secure_device = FALSE;
  BOOLEAN secure_device_nocheck_rpmb = FALSE;
  BOOLEAN used_devinfo = FALSE;
  UINT32 AppId = 0;
  UINTN VarSize = sizeof(AppId);
  STATIC QCOM_QSEECOM_PROTOCOL *QseeComProtocol;
  VOID *info = NULL;

  if (buf == NULL || buf_len == 0) {
    DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Invalid parameters!\n"));
    status = EFI_INVALID_PARAMETER;
    goto exit;
  }
  if (( status = is_secure_device_nocheck_rpmb (&secure_device_nocheck_rpmb)) != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Cannot read security state with no rpmb check!\n"));
    goto exit;
  }
  /* We use devinfo partition when the device is not secure */
  AsciiStrnCpy((CHAR8 *)img_name, "devinfo", AsciiStrLen("devinfo"));
  if (convert_char8_to_char16(img_name, img_label, AsciiStrLen("devinfo")) != EFI_SUCCESS) {
    status = RETURN_INVALID_PARAMETER;
    goto exit;
  }
  /* A secure device may boot up without enabled rpmb fuse but after calling to keymaster 
     the fuse will be blown and we can perform the actual check for secure device.
  */
  if (secure_device_nocheck_rpmb) {
    status = gBS->LocateProtocol(&gQcomQseecomProtocolGuid, NULL,
                                 (VOID **)&QseeComProtocol);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Cannot locate gQcomQseecomProtocolGuid!\n"));
    goto exit;
  }
  if (keymasterAppId == 0) {
    status = gRT->GetVariable(L"keymaster", &gQcomTokenSpaceGuid, NULL,
                              &VarSize, &AppId);
    if (status != EFI_SUCCESS) {
      status = QseeComProtocol->QseecomStartApp(QseeComProtocol, "keymaster",
                                                &AppId);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Failed in QseecomStartApp status = %d\n", status));
        goto exit;
      }
    }
    keymasterAppId = AppId;
  }
  }
  switch (op) {
  case WRITE_CONFIG:
    if (secure_device_nocheck_rpmb) {

      km_send_devrw_req wread_req = {0};
      km_send_devrw_rsp wread_rsp = {-1};
      km_send_devrw_req write_req = {0};
      km_send_devrw_rsp write_rsp = {-1};
      info = UncachedAllocatePages(1);
      if (info == NULL) {

        DEBUG((EFI_D_ERROR, "VB: RWDeviceState: mem allocation for write failed!\n"));
        goto exit;
      }
      wread_req.cmd_id = KEYMASTER_READ_LK_DEVICE_STATE;
      wread_req.data = (UINT32)info;
      wread_req.len = PAGE_SIZE;
      status = QseeComProtocol->QseecomSendCmd(
          QseeComProtocol, keymasterAppId, (UINT8 *)&wread_req,
          sizeof(wread_req), (VOID *)&wread_rsp,
          sizeof(wread_rsp));
      if (status != EFI_SUCCESS || wread_rsp.status != 0) {

        /* Check whether the error is caused by using a non-secure device or not.
           For secure device, the RPMB fuse should be blown after 
           invoking KEYMASTER_READ_LK_DEVICE_STATE. Thus, we use actual check for 
           identifying the secure device.
        */
        if ((status = is_secure_device(&secure_device)) != EFI_SUCCESS) {
          DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Cannot read security state!\n"));
          goto exit;
        }
        if (!secure_device) {
		  if ((status = write_partition(img_label, buf, buf_len)) != EFI_SUCCESS) {
		    DEBUG((EFI_D_ERROR, " VB: RWDeviceState: write_partition err! status: %d\n", status));
			status = EFI_FAILURE;
			goto exit;
		  } 
		  used_devinfo = TRUE;
		} else {
            DEBUG((EFI_D_ERROR, "VB: RWDeviceState: wread_req err ! status: %d read status: %d \n", status, wread_rsp.status));
            status = EFI_FAILURE;
            goto exit;
		}
      }
	  else {
        CopyMemS(info, PAGE_SIZE, buf, buf_len);
        write_req.cmd_id = KEYMASTER_WRITE_LK_DEVICE_STATE;
        write_req.data = (UINT32)info;
        write_req.len = PAGE_SIZE;
        status = QseeComProtocol->QseecomSendCmd(
        QseeComProtocol, keymasterAppId, (UINT8 *)&write_req,
        sizeof(write_req), (VOID *)&write_rsp,
        sizeof(write_rsp));
        if (status != EFI_SUCCESS || write_rsp.status != 0) {
         
		  DEBUG((EFI_D_ERROR, "VB: RWDeviceState: write_req err! staus %d write status %d\n", status, write_rsp.status));
          status = EFI_FAILURE;
		  goto exit;
	    }
       
      }
    }  else {
        if ((status = write_partition(img_label, buf, buf_len)) !=
          EFI_SUCCESS) {
          DEBUG((EFI_D_ERROR, " VB: RWDeviceState: write_partition err! status: %d\n", status));
		  goto exit;
        }
	    used_devinfo = TRUE;
    }
    break;
  case READ_CONFIG:
    if (secure_device_nocheck_rpmb) {
      km_send_devrw_req read_req = {0};
      km_send_devrw_rsp read_rsp = {-1};
      info = UncachedAllocatePages(1);
      if (info == NULL) {

        DEBUG((EFI_D_ERROR, "VB: RWDeviceState: mem allocation for read failed!\n"));
        goto exit;
      }
      read_req.cmd_id = KEYMASTER_READ_LK_DEVICE_STATE;
      read_req.data = (UINT32)info;
      read_req.len = PAGE_SIZE;

      status = QseeComProtocol->QseecomSendCmd(
          QseeComProtocol, keymasterAppId, (UINT8 *)&read_req,
          sizeof(read_req), (VOID *)&read_rsp,
          sizeof(read_rsp));

      if (status != EFI_SUCCESS || read_rsp.status != 0) {
	    /* Check whether the error is caused by using a non-secure device or not.
         For secure device, the RPMB fuse should be blown after 
         invoking KEYMASTER_READ_LK_DEVICE_STATE. Thus, we use actual check for 
         identifying the secure device.
        */
        if ((status = is_secure_device(&secure_device)) != EFI_SUCCESS) {
          DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Cannot read security state!\n"));
          goto exit;
        }
        if (!secure_device) {
		  if ((status = read_partition(img_label, buf, buf_len)) != EFI_SUCCESS) {
		    DEBUG((EFI_D_ERROR, " VB: RWDeviceState: read_partition err! status: %d\n", status));
			status = EFI_FAILURE;
			goto exit;
		  } 
		  used_devinfo = TRUE; 		   
		} else {
            DEBUG((EFI_D_ERROR, "VB: RWDeviceState: read_req err ! status: %d read status: %d \n", status, read_rsp.status));
            status = EFI_FAILURE;
            goto exit;
		}
      } else {
        CopyMemS(buf, buf_len, info, PAGE_SIZE);
      }

    } else {
        if ((status = read_partition(img_label, buf, buf_len)) !=
          EFI_SUCCESS) {
          DEBUG((EFI_D_ERROR, " VB: RWDeviceState: read_partition err! status = %x\n", status));
          goto exit;
      }
	   used_devinfo = TRUE;
    }
    break;
  default:
    DEBUG((EFI_D_ERROR, "VB: RWDeviceState: invalid operation op = %x \n", op));
    status = EFI_INVALID_PARAMETER;
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  if (info) {

    UncachedFreePages(info, 1);
  }
  if (status == EFI_SUCCESS) {
    if (used_devinfo) {
	  DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Succeed using devinfo!\n"));
	} else {
	    DEBUG((EFI_D_ERROR, "VB: RWDeviceState: Succeed using rpmb!\n"));
	}
  }
  return status;
}

/**
* Send Milestone to TZ
* API will send end milestone command to TZ
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
EFI_STATUS
QCOM_VB_Send_Milestone(IN QCOM_VERIFIEDBOOT_PROTOCOL *This)
{

  EFI_STATUS status = EFI_FAILURE;
  UINT32 AppId = 0;
  UINTN VarSize = sizeof(AppId);
  STATIC QCOM_QSEECOM_PROTOCOL *QseeComProtocol;
  UINT32 version = 0;  

  status = gBS->LocateProtocol(&gQcomQseecomProtocolGuid, NULL,
                               (VOID **)&QseeComProtocol);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: Send_Milestone: Cannot locate gQcomQseecomProtocolGuid!\n"));
    goto exit;
  }
  if (keymasterAppId == 0) {
    status = gRT->GetVariable(L"keymaster", &gQcomTokenSpaceGuid, NULL,
                              &VarSize, &AppId);
    if (status != EFI_SUCCESS) {
      status = QseeComProtocol->QseecomStartApp(QseeComProtocol, "keymaster",
                                                &AppId);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "VB: Send_Milestone: QseecomStartApp failed! status: %d\n", status));
        goto exit;
      }
    }
    keymasterAppId = AppId;
  }
  km_set_milestone_req cmd_req;
  km_set_milestone_rsp cmd_rsp;
  cmd_req.cmd_id = KEYMASTER_MILESTONE_CALL;
  status = QseeComProtocol->QseecomSendCmd(QseeComProtocol, keymasterAppId,
                                           (UINT8 *)&cmd_req, sizeof(cmd_req),
                                           (UINT8 *)&cmd_rsp, sizeof(cmd_rsp));
  if (status != EFI_SUCCESS || cmd_rsp.status != 0) {
    DEBUG((EFI_D_ERROR, "VB: Send_Milestone: QseecomSendCmd failed! status: %d rsp: %d\n", status, cmd_rsp.status));
    status = EFI_FAILURE;
    goto exit;
  }
  
  status = ScmGetFeatureVersion(TZ_FVER_QSEE, &version);
  if (status != EFI_SUCCESS) {
    goto exit;
  }

  if(allow_set_fuse(version)) { 
    status = set_fuse(TZ_HLOS_BL_MILESTONE_FUSE);
    if (status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "QCOM_VB_Send_Milestone: set_fuse (TZ_HLOS_BL_MILESTONE_FUSE) fails!\n"));
      goto exit;
    }
  } else {
      DEBUG((EFI_D_ERROR, "QCOM_VB_Send_Milestone: TZ didn't support this feature! Version: major = %d, minor = %d, patch = %d\n", (version >> 22) & 0x3FF, (version >> 12) & 0x3FF, version & 0x3FF));
      goto exit;
  }
exit:
  return status;
}

/**
* Send Delete CMD to Keymaster
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/

EFI_STATUS
QCOM_VB_Reset_State(IN QCOM_VERIFIEDBOOT_PROTOCOL *This)
{

  EFI_STATUS status = EFI_FAILURE;
  UINT32 AppId = 0;
  UINTN VarSize = sizeof(AppId);
  STATIC QCOM_QSEECOM_PROTOCOL *QseeComProtocol;
  status = gBS->LocateProtocol(&gQcomQseecomProtocolGuid, NULL,
                               (VOID **)&QseeComProtocol);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: Reset_State: Cannot locate gQcomQseecomProtocolGuid!\n"));
    goto exit;
  }
  if (keymasterAppId == 0) {
    status = gRT->GetVariable(L"keymaster", &gQcomTokenSpaceGuid, NULL,
                              &VarSize, &AppId);
    if (status != EFI_SUCCESS) {
      status = QseeComProtocol->QseecomStartApp(QseeComProtocol, "keymaster",
                                                &AppId);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "VB: Reset_State: QseecomStartApp failed! status= %d\n", status));
        goto exit;
      }
    }
    keymasterAppId = AppId;
  }
  km_reset_req cmd_req;
  km_reset_rsp cmd_rsp;
  cmd_req.cmd_id = KEYMASTER_DELETE_ALL_KEYS;
  status = QseeComProtocol->QseecomSendCmd(QseeComProtocol, keymasterAppId,
                                           (UINT8 *)&cmd_req, sizeof(cmd_req),
                                           (UINT8 *)&cmd_rsp, sizeof(cmd_rsp));
  if (status != EFI_SUCCESS || cmd_rsp.status != 0) {
    DEBUG((EFI_D_ERROR, "VB: Reset_State: QseecomSendCmd failed! status: %d rsp: %d\n", status, cmd_rsp.status));
    status = EFI_FAILURE;
    goto exit;
  }
exit:
  return status;
}

/**
* Send SendBootState to Keymaster
* API will construct the BoostState payload and provide
* it to Keymaster TA
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
EFI_STATUS
QCOM_VB_SendBootState(IN QCOM_VERIFIEDBOOT_PROTOCOL *This,
                      IN QCOM_QSEECOM_PROTOCOL *QseeComProtocol,
                      IN secasn1_data_type *public_exp,
                      IN UINTN hash_size)
{
  EFI_STATUS status = EFI_FAILURE;
  km_boot_state_rsp bs_rsp = {0};
  UINT8 bs_digest[VB_SHA256_SIZE] = {0};
  EFI_GUID *HashAlgorithm = &gEfiHashAlgorithmSha256Guid;
  EFI_HASH2_OUTPUT Hash2Output;
  EFI_HASH2_PROTOCOL *pEfiHash2Protocol = NULL;
  km_boot_state_req bs_req = {0};

  DEBUG((EFI_D_INFO, "VB: Setting Boot State\n"));

  status = gBS->LocateProtocol(&gEfiHash2ProtocolGuid, NULL, (VOID **)&pEfiHash2Protocol);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: LocateProtocol unsuccessful! status: %d\n", status));
    goto exit;
  }

  switch (dev_boot_state) {
    case GREEN:
    case YELLOW:
      status = pEfiHash2Protocol->HashInit(pEfiHash2Protocol, HashAlgorithm);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashInit unsuccessful! status: %d\n", status));
        goto exit;
      }
      status = pEfiHash2Protocol->HashUpdate(pEfiHash2Protocol, (uint8 *)public_exp->data,
		public_exp->len);
      if (EFI_SUCCESS != status) {
        DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashUpdate unsuccessful(img)! status: %d\n", status));
        goto exit;
      }
      status = pEfiHash2Protocol->HashFinal(pEfiHash2Protocol, &Hash2Output);
      if (EFI_SUCCESS != status) {
        DEBUG((EFI_D_ERROR, "VB: vb_get_image_hash: HashFinal unsuccessful! status = %d\n", status));
        goto exit;
      }
      CopyMemS(bs_digest, hash_size, (CONST UINT8 *)&Hash2Output, hash_size);
      break;
    case ORANGE:
      break;
    case RED:
    default:
      DEBUG((EFI_D_ERROR, "Invalid state to boot!\n"));
      return EFI_LOAD_ERROR;
  }

  bs_req.cmd_id = KEYMASTER_SET_BOOT_STATE;
  bs_req.bs_version = 0;
  bs_req.bs_size = sizeof(bs_req.boot_state);
  bs_req.bs_offset = (UINT8 *)&bs_req.boot_state - (UINT8 *)&bs_req;
  bs_req.boot_state.color = dev_boot_state;
  bs_req.boot_state.is_unlocked = dev_lock_state.is_unlocked;
  bs_req.boot_state.system_version = (bootstate_os_version & 0xFFFFF800) >> 11;
  bs_req.boot_state.system_security_level = (bootstate_os_version & 0x7FF);
  CopyMemS(bs_req.boot_state.public_key, hash_size, bs_digest, hash_size);
  status = QseeComProtocol->QseecomSendCmd(QseeComProtocol, keymasterAppId,
          (UINT8 *)&bs_req, sizeof(bs_req), (VOID *)&bs_rsp, sizeof(bs_rsp));
  if (status != EFI_SUCCESS || bs_rsp.status != 0) {
    DEBUG((EFI_D_ERROR, "VB: SendBootState: cmd err, status: %d bs_req status: %d\n", status, bs_rsp.status));
    status = EFI_FAILURE;
    goto exit;
  }

exit:
  return status;
}

/**
* Send ROT to Keymaster
* API will construct the ROT payload and provide
* it to Keymaster TA
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
EFI_STATUS
QCOM_VB_SendROT(IN QCOM_VERIFIEDBOOT_PROTOCOL *This)
{

  EFI_STATUS status = EFI_FAILURE;
  UINT32 AppId = 0;
  UINTN VarSize = sizeof(AppId);
  STATIC QCOM_QSEECOM_PROTOCOL *QseeComProtocol;
  RSA key;
  secasn1_data_type modulus;
  secasn1_data_type public_exp;
  VB_HASH hash_algorithm = VB_SHA256;
  UINTN hash_size = VB_SHA256_SIZE;
  UINT8 digest[hash_size];
  UINT8 unlock_digest[hash_size + 4];
  km_set_rot_req *read_req = NULL;
  UINTN read_req_size;
  km_set_rot_rsp read_rsp;
  km_get_version_req km_version_req = {0};
  km_get_version_rsp km_version_rsp = {0};
  BOOLEAN secure_device = FALSE;
  UINT32 version = 0;

  if (!image_verifiction_called) {
    DEBUG((EFI_D_ERROR, "VB: SendROT: only can be called after image verification!\n"));
    goto exit;
  }
  switch (dev_boot_state) {
  case GREEN:
    status = pEfiQcomASN1X509Protocol->ASN1X509GetRSAFromCert(pEfiQcomASN1X509Protocol, &oem_certificate, &key);
    if (status != EFI_SUCCESS) {
      goto exit;
    }
    status = pEfiQcomASN1X509Protocol->ASN1X509GetKeymaterial(pEfiQcomASN1X509Protocol, &key, &modulus, &public_exp);
    if (status != EFI_SUCCESS) {
      goto exit;
    }
    break;
  case YELLOW:
  case RED:
    if (!cert_verification_passed) {
      goto exit;
    }
    status = pEfiQcomASN1X509Protocol->ASN1X509GetRSAFromVBSig(pEfiQcomASN1X509Protocol, &embedded_vb_signature, &key);
    if (status != EFI_SUCCESS) {
      goto exit;
    }
    status = pEfiQcomASN1X509Protocol->ASN1X509GetKeymaterial(pEfiQcomASN1X509Protocol, &key, &modulus, &public_exp);
    if (status != EFI_SUCCESS) {
      goto exit;
    }
    break;
  case ORANGE:
    unlock_digest[0] = dev_lock_state.is_unlocked;
    UINT32 i;
    for (i = 1; i < hash_size + 4; i++) {
      unlock_digest[i] = 0;
    }
    break;
  default:
    goto exit;
  }
  if (dev_boot_state != ORANGE) {
    if ((status = vb_get_digest_locked_device(modulus.data, modulus.len,
                                              public_exp.data, public_exp.len,
                                              dev_lock_state,
                                              digest, hash_algorithm, hash_size)) != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "VB: SendROT: vb_get_digest_locked_device unsuccessful! \n"));
      goto exit;
    }
  } else {
    if ((status = vb_get_digest_unlocked_device(unlock_digest, hash_size + 4, digest, hash_algorithm,
                                                hash_size)) != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "VB: SendROT: vb_get_digest_unlocked_device unsuccessful! \n"));
      goto exit;
    }
  }
  /* Send command to Keymaster */
  status = gBS->LocateProtocol(&gQcomQseecomProtocolGuid, NULL,
                               (VOID **)&QseeComProtocol);
  if (status != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: SendROT: Cannot locate gQcomQseecomProtocolGuid protocol\n"));
    goto exit;
  }
  if (keymasterAppId == 0) {
    status = gRT->GetVariable(L"keymaster", &gQcomTokenSpaceGuid, NULL,
                              &VarSize, &AppId);
    if (status != EFI_SUCCESS) {
      status = QseeComProtocol->QseecomStartApp(QseeComProtocol, "keymaster",
                                                &AppId);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "VB: SendROT: QseecomStartApp failed status: %d\n", status));
        goto exit;
      }
    }
    keymasterAppId = AppId;
  }
  read_req = AllocatePool(sizeof(km_set_rot_req) + hash_size);
  read_req_size = sizeof(km_set_rot_req) + hash_size;
  if (!read_req) {
    DEBUG((EFI_D_ERROR, "VB: VB_SendROT: mem allocation for read_req failed!\n"));
    status = EFI_FAILURE;
    goto exit;
  }
  UINT8 *cpy_ptr = (UINT8 *)read_req + sizeof(km_set_rot_req);
  read_req->cmd_id = KEYMASTER_SET_ROT;
  read_req->rot_ofset = sizeof(km_set_rot_req);
  read_req->rot_size = hash_size;
  CopyMemS(cpy_ptr, read_req_size, digest, hash_size);
  status = QseeComProtocol->QseecomSendCmd(
      QseeComProtocol, keymasterAppId, (UINT8 *)read_req,
      read_req_size, (VOID *)&read_rsp,
      sizeof(read_rsp));
  if (status != EFI_SUCCESS || read_rsp.status != 0) {
    DEBUG((EFI_D_ERROR, "VB: SendROT: ROT cmd err, status: %d read_req status: %d\n", status, read_rsp.status));
    status = EFI_FAILURE;
    goto exit;
  }
  
  km_version_req.cmd_id = KEYMASTER_GET_VERSION;
  status = QseeComProtocol->QseecomSendCmd(
      QseeComProtocol, keymasterAppId, (UINT8 *)&km_version_req,
      sizeof(km_get_version_req), (VOID *)&km_version_rsp,
      sizeof(km_get_version_rsp));
  if (status != EFI_SUCCESS || km_version_rsp.status != 0) {
    DEBUG((EFI_D_ERROR, "VB: Get KM Version cmd err, status: %d resp.status: %d\n", status, km_version_rsp.status));
    status = EFI_FAILURE;
    goto exit;
  }

  if (km_version_rsp.ta_major_version > 2) {
    status = QCOM_VB_SendBootState(This, QseeComProtocol, &public_exp, hash_size);
    if (status != EFI_SUCCESS) {
      DEBUG((EFI_D_ERROR, "QCOM_VB_SendROT: QCOM_VB_SendBootState fails!\n"));
      goto exit;
    }
  }  

  if (((status = is_secure_device(&secure_device)) == EFI_SUCCESS) &&
        secure_device &&
        (dev_boot_state != GREEN)) {
    
    status = ScmGetFeatureVersion(TZ_FVER_QSEE, &version);
    if (status != EFI_SUCCESS) {
      goto exit;
    }
    if(allow_set_fuse(version)) {
      status = set_fuse(TZ_HLOS_IMG_TAMPER_FUSE);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "QCOM_VB_SendROT: set_fuse (TZ_HLOS_IMG_TAMPER_FUSE) fails!\n"));
        goto exit;
      } 
      status = set_fuse(TZ_HLOS_TAMPER_NOTIFY_FUSE);
      if (status != EFI_SUCCESS) {
        DEBUG((EFI_D_ERROR, "QCOM_VB_SendROT: set_fuse (TZ_HLOS_TAMPER_NOTIFY_FUSE) fails!\n"));
        goto exit;
      }
    } else {
      DEBUG((EFI_D_ERROR, "QCOM_VB_SendROT: TZ didn't support this feature! Version: major = %d, minor = %d, patch = %d\n", (version >> 22) & 0x3FF, (version >> 12) & 0x3FF, version & 0x3FF));
      goto exit;    
    }
  }
exit:
  if (read_req) {
    FreePool(read_req);
  }
  return status;
}

/**
* Return if the device is secure or not
* API will set the State flag to indicate if its a secure device
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/
EFI_STATUS
QCOM_VB_Is_Device_Secure(IN QCOM_VERIFIEDBOOT_PROTOCOL *This,
                         OUT BOOLEAN *State)
{

  EFI_STATUS status = EFI_FAILURE;
  BOOLEAN secure_device = FALSE;
  if (!State) {
    goto exit;
  }
  if ((status = is_secure_device(&secure_device)) != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: Is_Device_Secure: Cannot read security state\n"));
    goto exit;
  }
  *State = secure_device;
  status = EFI_SUCCESS;
exit:
  return status;
}

/**
* Authenticate Linux Image based on VB
*
* @param UINT8 pname[MAX_PNAME_LENGTH]
*   Pname of image to be authenticated
*
* @param UINT32*img
*   Pointer to the img loaded from "pname" partition
*
* @param uint32_t img_len
*   Size of the img
*
* @param boot_state bootstate
*   Enum of the boot state of the device.
*
* @return int
*   Status:
*     0 - Success
*     Negative value indicates failure.
*/

EFI_STATUS
QCOM_VB_VerifyImage(IN QCOM_VERIFIEDBOOT_PROTOCOL *This,
                    IN UINT8 pname[MAX_PNAME_LENGTH], IN UINT8 *img,
                    IN UINT32 img_len, OUT boot_state_t *bootstate)
{
  EFI_STATUS status = EFI_FAILURE;
  struct boot_img_hdr *img_hdr = NULL;
  VB_HASH hash_algorithm = VB_SHA256;
  UINTN hash_size = VB_SHA256_SIZE;
  UINT8 img_hash[hash_size];
  UINT32 startTime = 0;
  UINT32 endTime = 0;

  startTime = GetTimerCountms();
  if (!is_device_init_called) {

    dev_boot_state = BOOT_STATE_MAX;
    goto exit;
  }
  if (!pname || !img || !bootstate || img_len == 0 || img_len > MAX_UINT64 - (UINT64)img) {

    dev_boot_state = BOOT_STATE_MAX;
    status = EFI_INVALID_PARAMETER;
    goto exit;
  }

  /* Skip verification in case the dev_boot_state (initialized by
 * QCOM_VB_DeviceInit API) is Orange */
  if (dev_boot_state == ORANGE) {
    status = EFI_SUCCESS;
    goto exit;
  }
  /* Read Android Verified Boot Signature from the image */
  if (vb_read_embedded_vb_signature(pEfiQcomASN1X509Protocol, img, img_len, &embedded_vb_signature) != EFI_SUCCESS) {
    dev_boot_state = RED;
    status = EFI_SUCCESS;
    goto exit;
  }
  /* Read OEM certificate from the embedded header file */
  if (vb_read_oem_certificate(pEfiQcomASN1X509Protocol, &oem_certificate) != EFI_SUCCESS) {
    dev_boot_state = RED;
    status = EFI_SUCCESS;
    goto exit;
  }
  /* Calculate the hash of image */
  if (vb_get_image_hash(pEfiQcomASN1X509Protocol, pname, img, img_len, &embedded_vb_signature, img_hash,
                        hash_size, hash_algorithm) != EFI_SUCCESS) {
    dev_boot_state = RED;
    status = EFI_SUCCESS;
    goto exit;
  }
  /* Verify the hash of image with OEM key */
  if (vb_verify_hash_oem_certificate(pEfiQcomASN1X509Protocol, img_hash, hash_algorithm, &oem_certificate,
                                     &embedded_vb_signature) != EFI_SUCCESS) {
    dev_boot_state = YELLOW;
    /* Verify the hash of image with embedded key */
    if (vb_verify_hash_embedded_key(pEfiQcomASN1X509Protocol, img_hash, hash_algorithm,
                                    &embedded_vb_signature) != EFI_SUCCESS) {
      dev_boot_state = RED;
      status = EFI_SUCCESS;
      goto exit;
    }
    cert_verification_passed = TRUE;
  } else {

    dev_boot_state = GREEN;
  }
  status = EFI_SUCCESS;
exit:
  if (bootstate) {
    *bootstate = dev_boot_state;
  }
  if (status == EFI_SUCCESS) {
    image_verifiction_called = TRUE;
  }

  DEBUG((EFI_D_ERROR, "VB: VerifyImage: Image verification done! boot state is: "));
  switch (dev_boot_state) {
  case GREEN:
    DEBUG((EFI_D_ERROR, "GREEN\n"));
    break;
  case YELLOW:
    DEBUG((EFI_D_ERROR, "YELLOW\n"));
    break;
  case RED:
    DEBUG((EFI_D_ERROR, "RED\n"));
    break;
  case ORANGE:
    DEBUG((EFI_D_ERROR, "ORANGE\n"));
    break;
  default:
    DEBUG((EFI_D_ERROR, "UNDEFINED\n"));
    break;
  }
  /* Extract os version and patch level */
  if (img) {
     img_hdr = (struct boot_img_hdr *)img;
     bootstate_os_version = img_hdr->os_version;
  }
  endTime = GetTimerCountms();
  DEBUG((EFI_D_WARN, "VB:VerifyImage time spent: %d mS\n", endTime - startTime));
  return status;
}

/**
* Get boot state
*
* @return boot_state_t
* In case of success returns 0.
*
*
*/
EFI_STATUS
QCOM_VB_Get_Boot_State(IN QCOM_VERIFIEDBOOT_PROTOCOL *This, OUT boot_state_t *bootstate)
{
  EFI_STATUS status = EFI_FAILURE;
  if (!image_verifiction_called) {
    DEBUG((EFI_D_ERROR, "VB: GetBootState: call the API after image verification\n"));
    status = EFI_FAILURE;
    goto exit;
  }
  if (bootstate == NULL) {
    DEBUG((EFI_D_ERROR, "VB: GetBootState: Invalid input\n"));
    status = EFI_INVALID_PARAMETER;
    goto exit;
  }
  *bootstate = dev_boot_state;
  status = EFI_SUCCESS;
exit:
  return status;
}

/**
*  Get hash of certificate in Yellow boot state
*  @return *buf
*  In case of success return 0.
*
*/ 
EFI_STATUS
QCOM_VB_Get_Cert_FingerPrint(IN QCOM_VERIFIEDBOOT_PROTOCOL *This, OUT UINT8 *buf, IN UINTN buf_len, OUT UINTN *out_len)
{
  EFI_STATUS status = EFI_FAILURE;

  if (dev_boot_state != YELLOW) {
    DEBUG((EFI_D_ERROR, "VB: GetCertFingerPrint: only can be called after image verification for YELLOW state!\n"));
    goto exit;
  }
  if (buf == NULL || buf_len == 0 || out_len == NULL) {
    DEBUG((EFI_D_ERROR, "VB: GetCertFingerPrint: invalid input parameters!\n"));
    goto exit;
  }
  if ((status = vb_get_cert_fingerprint(embedded_vb_signature.certificate, embedded_vb_signature.certificate_len, buf, buf_len, out_len)) != EFI_SUCCESS) {
    DEBUG((EFI_D_ERROR, "VB: GetCertFingerPrint: cannot get finger print!\n"));
    goto exit;
  }
  status = EFI_SUCCESS;
exit:
  return status;
}

/*=========================================================================
      Data Declarations
==========================================================================*/

// VerifiedBoot Protocol

STATIC QCOM_VERIFIEDBOOT_PROTOCOL QCOMVerifiedBootProtocol = {
    QCOM_VERIFIEDBOOT_PROTOCOL_REVISION, QCOM_VB_RWDeviceState,
    QCOM_VB_DeviceInit, QCOM_VB_SendROT, QCOM_VB_Send_Milestone,
    QCOM_VB_VerifyImage, QCOM_VB_Reset_State, QCOM_VB_Is_Device_Secure,
    QCOM_VB_Get_Boot_State, QCOM_VB_Get_Cert_FingerPrint};

EFI_STATUS
EFIAPI
VerifiedBootDxeEntryPoint(IN EFI_HANDLE ImageHandle,
                          IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  EFI_HANDLE Handle = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces(
      &Handle, &gEfiQcomVerifiedBootProtocolGuid,
      (VOID **)&QCOMVerifiedBootProtocol, NULL);

  return Status;
}
