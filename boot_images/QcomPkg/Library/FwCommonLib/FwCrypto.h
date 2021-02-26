/** @file FwCrypto.h
   
  Cryptography functions used for firmware update

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/10/08    mic         Initial version

=============================================================================*/
#ifndef __QCOM_FW_CRYPTO_H__
#define __QCOM_FW_CRYPTO_H__

#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/FwCommonLib.h>

/**
 * . Function to check if buffer is SSD encrypted
 * 
 * @param buffer           - buffer
 * @param bufferSize     - buffer size
 * @param bEncrypted   - buffer is encrypted or not
 * @return : EFI_STATUS
 */
EFI_STATUS 
FwCryptoSsdChecking(
  IN VOID     *buffer, 
  IN UINTN    bufferSize,
  OUT BOOLEAN *bEncrypted
  );

/**
 * . Function to for SSD decryption
 * 
 * @param InBuf           - Input buffer
 * @param InBufSize     - Input buffer size
 * @param OutBuf         - Output buffer
 * @param OutBufSize   - Output buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
FwCryptoSsdDecrypt(
  IN  VOID     *InBuf, 
  IN  UINTN    InBufSize,
  OUT VOID     **OutBuf, 
  OUT UINTN    *OutBufSize
  );

/**
 * . Function to decrypt and encrypt OPM private key
 * 
 * @param InBuf           - Input buffer
 * @param InBufSize     - Input buffer size
 * @param OutBuf         - Output buffer
 * @param OutBufSize   - Output buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
FWCryptoOpmPrivKey(
  IN  VOID     *InBuf, 
  IN  UINTN    InBufSize,
  OUT VOID     **OutBuf, 
  OUT UINTN    *OutBufSize
  );


/**
 * . Function to encrypt SSD key
 * 
 * @param InBuf           - Input buffer
 * @param InBufSize     - Input buffer size
 * @param OutBuf         - Output buffer
 * @param OutBufSize   - Output buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
FwCryptoMsmEncrypt(
  IN  VOID     *InBuf, 
  IN  UINTN    InBufSize,
  OUT VOID     **OutBuf, 
  OUT UINTN    *OutBufSize
  );


#endif
