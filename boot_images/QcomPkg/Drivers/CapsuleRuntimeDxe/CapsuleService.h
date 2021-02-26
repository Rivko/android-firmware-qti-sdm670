/** @file CapsuleService.h
   
  UEFI Capsule Service data structures.

  Copyright (c) 2012, 2015 Qualcomm Technologies, Inc. All rights reserved. 
Portions Copyright (c) 2004 - 2009, Intel Corporation. All 
rights 
reserved.<BR> This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/05/15   jb      Add FMP support
 03/13/12   niting  Added firmware update support.
 02/06/12   jdt     Initial revision

=============================================================================*/

#ifndef __CAPSULE_SERVICE_H__
#define __CAPSULE_SERVICE_H__


extern EFI_GUID gQcomScatterGatherListHeaderGuid;

#define SCATTERGATHERLIST_ARRAY_HEADER_REVISION 1

/**
  This header describes an array of 
  EFI_PHYSICAL_ADDRESSes, each of which points 
  to a ScatterGatherList. 
   
  Header description: 
   
  +------------------------------------------+ 
  | Revision - Revision 1                    | 
  +------------------------------------------+ 
  |GUID - Signature                          | 
  +------------------------------------------+ 
  |PointerCount - the number of elements in  | 
  |  our ScatterGatherList array|            | 
  +------------------------------------------+ 
  |Offset - Offset from the beginning of     |
  | this header to the beginning of the      | 
  | ScatterGatherList array. This value is   |
  |  usually the size of this header         | 
  +------------------------------------------+ 
  |Crc32 - Header CRC                        | 
  +------------------------------------------+ 
  |(Beginning of Array)                      | 
  +------------------------------------------+ 
 
*/
typedef struct 
{
    UINT32                  Revision;
    EFI_GUID                ScatterGatherHeaderGuid;
    UINT32                  PointerCount;
    UINT32                  ArrayOffset;  //Offset from beginning of header
    UINT32                  Crc32;
    
}SCATTERGATHERLIST_ARRAY_HEADER;

/* CapsuleBuffer is the output of CapsuleDataCoalesce */
EFI_STATUS
EFIAPI
PersistCoalescedCapsules (
  IN VOID    *CapsuleBuffer
  );

/**

  This routine is called to process capsules which were 
  pushed by the previous boot. 
   
  Capsules with flag CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE set 
  will be installed to the system table, and those without will 
  be processed by ProcessCapsuleImage. 
   

  @param CapsuleBuffer            The Capsule Buffer to process. 
  @retval EFI_INVALID_PARAMETER   boot mode is not correct for an update
  @retval EFI_SUCCESS             There is no error when processing capsule

**/
EFI_STATUS
EFIAPI
ProcessCoalescedCapsules (
  VOID    *CapsuleBuffer
  );

/**
  The firmware-specific implementation processes the capsule image
  if it recognized the format of this capsule image.
  
  @param  CapsuleHeader    Pointer to the UEFI capsule image to be processed. 
  @param  CapsuleLBAOffset Offset of the capsule in persistent storage, 0 if
                           capsule didn't come from storage
  @param  ResetNeeded      Returns if a process capsule that needs a reset was
                           processed
   
  @retval EFI_SUCESS       Capsule Image processed successfully. 
  @retval EFI_UNSUPPORTED  Capsule image is not supported by the firmware.
**/
EFI_STATUS
EFIAPI
ProcessCapsuleImageEx (
  IN EFI_CAPSULE_HEADER *CapsuleHeader,
  IN EFI_LBA             CapsuleLBAOffset,
  OUT BOOLEAN            *ResetNeeded OPTIONAL
  );

#endif /* __CAPSULE_SERVICE_H__ */

