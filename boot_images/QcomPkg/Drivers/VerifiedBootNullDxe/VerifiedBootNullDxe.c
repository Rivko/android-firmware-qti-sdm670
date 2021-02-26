/** @file VerifiedBootDxeNull.c

  This driver produces verified boot pass-through null protocol.

  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 10/04/16   bh       Initial version

=============================================================================*/

#include <Uefi.h>
#include <Protocol/EFIVerifiedBoot.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>


typedef struct d_info
{
  CHAR8 temp[13];
  BOOLEAN flag1; 
  BOOLEAN flag2;
} Info; 


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

  return EFI_SUCCESS;
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
  Info *info = (Info *) buf;
  info->flag1 = TRUE;
  info->flag2 = TRUE;

  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
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
  return EFI_SUCCESS;
}

/*=========================================================================
      Data Declarations
==========================================================================*/

// VerifiedBoot Protocol

STATIC QCOM_VERIFIEDBOOT_PROTOCOL QCOMVerifiedBootProtocol = {
    QCOM_VERIFIEDBOOT_PROTOCOL_REVISION,
    QCOM_VB_RWDeviceState,
    QCOM_VB_DeviceInit,
    QCOM_VB_SendROT,
    QCOM_VB_Send_Milestone,
    QCOM_VB_VerifyImage,
    QCOM_VB_Reset_State,
    QCOM_VB_Is_Device_Secure
};

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
