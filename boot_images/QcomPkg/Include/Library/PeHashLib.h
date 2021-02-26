#ifndef __PEHASHLIB_H__
#define __PEHASHLIB_H__

/** @file  
                             PeHashLib.h

  This is for function declaration of PE/COFF hash calculation.

  Copyright (c) 2011 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/23/11   shl    Added more APIs. 
07/18/11   shl    Initial version.
===========================================================================*/

/**
  This function will calculte hash PE/COFF image.

  @param[in]  *FileBuffer        Points to PE/COFF image in disk format.
  @param[in]   FileSize          PE/COFF image size.
  @param[in]   *HashAlgorithm    Pointer to HashAlgorithm to use, NULL means use 
                                 whatever in Certificate table.
  @param[in]  *pHashOut          Pointer to calculated hash.

  @retval EFI_SUCCESS            Verified successfully.
  @retval EFI_INVALID_PARAMETER  Parmeter is not right or data not recoginized.
  @retval EFI_UNSUPPORTED        Verification not supported.
**/

EFI_STATUS EFIAPI ComputePeImageHash(
  IN VOID     *FileBuffer,
  IN UINTN     FileSize,
  IN EFI_GUID *HashAlgorithm,
  UINT8       *pHashOut
);

/**
  This function will parse PE/COFF image.

  @param[in]  *FileBuffer        Points to PE/COFF image in disk format.
  @param[in]   FileSize          PE/COFF image size.

  @retval EFI_SUCCESS            Verified successfully.
  @retval EFI_INVALID_PARAMETER  Parmeter is not right or data not recoginized.
  @retval EFI_UNSUPPORTED        Verification not supported.
**/

EFI_STATUS EFIAPI ParsePeImage(
  IN VOID     *FileBuffer,
  IN UINTN     FileSize
);

/**
  This function will return previous parsed SubSystem

  @param[in]  *SubSystem   Pointer to SubSystem.

  @retval EFI_SUCCESS      Always successfully.

**/
EFI_STATUS EFIAPI GetPeImageSubSystem(
  UINT16         *SubSystem
);


/**
  This function will return previous calculated certificate tabel offset 
  and size of the certificate table.

  @param[in]  *CertTableOffse   Pointer to certificate table offset.
  @param[in]  *SizeOfCertTable  Pointer to size of certificate table.

  @retval EFI_SUCCESS           Alwayssuccessfully.

**/
EFI_STATUS EFIAPI GetPeImageCertInfo(
  UINT32  *CertTableOffset,
  UINT32  *SizeOfCertTable
);

#endif /* __PEHASHLIB_H__ */
