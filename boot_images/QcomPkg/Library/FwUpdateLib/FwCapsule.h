/** @file FwCapsule.h
   
  Header for capsule related functions
  Portions of this file Copyright (c) 2011, Intel Corporation.
  All rights reserved. This program and the accompanying
  materials are licensed and made available under the terms and 
  conditions of the BSD License which accompanies this 
  distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
  BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND,
  EITHER EXPRESS OR IMPLIED. 
   
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential. 

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2013/07/02     mic         Updated CopyRight for UXCapsule
 2012/10/04     mic         Added Data-Driven update support
 2012/07/02     mic         Added debug message support for file node
 2012/06/12     rs          Added support for BreakChangeNumber
 2012/06/05     rs          Added support for UXCapsule 
 2011/10/12     jthompso    Initial version

=============================================================================*/

#ifndef __QCOM_FW_CAPSULE_H_
#define __QCOM_FW_CAPSULE_H_

#include <Include/Library/FwCommonLib.h>

#define TZ_ENCRYPT_SIGN_LEN 8

typedef struct
{
  UINT32 FwVer;
  UINT32 FwLowestSupportedVer;
  UINT32 BreakingChangeNumber;
}PAYLOAD_VERSION;


///
/// UX Capsule Header definition.
///
typedef struct {
  /// 1-bytes (8 bit) version ID. This value must be 1.
  ///
  UINT8                       Version;
  ///
  /// 1-byte checksum
  ///
  UINT8                       Checksum;
  ///
  /// 1-byte enumerated type field indicating format of the image.
  ///     0 = Bitmap
  ///     1 - 255  Reserved (for future use)
  ///
  UINT8                       ImageType;
  ///
  /// 1-byte (8 bit) Reserved field
  ///
  UINT8                       Reserved;
  ///
  UINT32                      Mode;
  ///
  /// A 4-byte (32-bit) unsigned long describing the display X-offset of the boot image.
  /// (X, Y) display offset of the top left corner of the boot image.
  /// The top left corner of the display is at offset (0, 0).
  ///
  UINT32                      ImageOffsetX;
  ///
  /// A 4-byte (32-bit) unsigned long describing the display Y-offset of the boot image.
  /// (X, Y) display offset of the top left corner of the boot image.
  /// The top left corner of the display is at offset (0, 0).
  ///
  UINT32                      ImageOffsetY;
  ///
} UX_CAPSULE_PAYLOAD;

#define QPAYLOAD_METADATA_SIGNATURE1  0x2E1946FB
#define QPAYLOAD_METADATA_SIGNATURE2  0x7F744D57

/**
 * Check if the firmware version in the payload is supported 
*
 * @param bPass -  TRUE: supported, FALSE: not supported
 * @return EFI_STATUS
 */


EFI_STATUS
FwUpdateCheckLowestSupportedVersion(
  OUT BOOLEAN  *bPass,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

  );


/** 
* Load and run the payload file
*
*@param    Image handle  UEFI Image Handle
*@param    SystemTable   UEFI system table
*@param    pos                 the capsule index 
*    
* return EFI_STATUS
**/

EFI_STATUS
FwUpdateCapsuleLoadPayload(
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable,
    IN UINTN              pos
    );
/**
 * This Function try to find if capsule exist in system table 
*
 * @param CapGUID     - the target Capsule GUID 
 * @param SystemTable - Passed from calling application
 * @param pos               - the index of the capsule in configuration table in system table
 * @return EFI_STATUS 
 */

BOOLEAN
FwUpdateFindCapsule(
  IN EFI_GUID *CapGUID,
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT UINTN             *pos
  );


/** 
*Does nothing. Just a placeholder
*    
*@param    Event       Event whose notification function is being invoked.
*@param    Context     Pointer to the notification function's context, which is
*                      always zero in current implementation.
* return                void
**/

VOID
EFIAPI
FwUpdateCapsuleEventHandler (
    IN EFI_EVENT          Event,
    IN VOID               *ContextGuardPredicateList
    );


/** 
* register the capsule ID with an event for capsule deletion
*
*@param    CapGUID      The Capsule GUID
*@param    DelEvent     The Event to associate with the GUID
* return EFI_STATUS
**/

EFI_STATUS
FwUpdateRegisterCapsuleGUID(
  IN EFI_GUID *CapGUID,
  IN EFI_EVENT *DelEvent
  );


/**
 * This Function kicks off the updateCapsule  Phase of 
 * the Firmware update process. 
 * 
 * @param ImageHandle - Passed from calling application
 * @param SystemTable - Passed from calling application
 * @param pos              - filled in by this fuction if TRUE returned
 * @return EFI_STATUS 
 */


EFI_STATUS
FwUpdateCapsulePhaseMain(
  IN  EFI_HANDLE               ImageHandle,
  IN  EFI_SYSTEM_TABLE         *SystemTable,
  IN  UINTN                    pos,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
  );


/** 
* Search the system table to see if any pending capsule exists
*    
* @param    SystemTable   :  UEFI system table
* @param    CapsuleFound  :  Pending capsule found or not
*    
* @return EFI_STATUS
**/

EFI_STATUS
FwUpdateFindPendingCapsule(
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT BOOLEAN          *CapsuleFound
  );


/** 
*Does nothing. Just a placeholder
*    
* @param    Event    : Event whose notification function is
*                     being invoked.
* @param    Context : Pointer to the notification function's
*                     context, which is always zero in current
*                     implementation.
* @return   MONE
**/

VOID
EFIAPI
FwUpdateUXCapsuleEventHandler (
  IN EFI_EVENT          Event,
  IN VOID               *ContextGuardPredicateList
  );

#endif
