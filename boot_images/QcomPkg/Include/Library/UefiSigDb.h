/** @file UefiSigDb.h
   
  Contains Signature DB handling library functions.

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/14/11   yg      Add function to get supported GUIDs list
 08/16/11   yg      Created New file.

=============================================================================*/
#ifndef __UEFI_SIGNATURE_DB_H__
#define __UEFI_SIGNATURE_DB_H__

#include "Uefi.h"

typedef struct SigDBCtx* SigDBHandle;

#define DEFAULT_SIGNATURE_SIZE                  0

#define SIG_DB_ENUM_SUCCESS                     0
#define SIG_DB_ENUM_NOT_FOUND                   -1
#define SIG_DB_ENUM_NOT_INITIALIZED             -2
#define SIG_DB_ENUM_BUFFER_SIZE_INSUFFICIENT    -3
#define SIG_DB_ENUM_INVALID_PARAM               -4

/* Supported Signature types */
extern GUID CertSHA256Guid;
extern GUID CertRSA2048Guid;
extern GUID CertRSA2048SHA256Guid;
extern GUID CertX509Guid;

/* Allocate a Signature DB Enumeration Context and initialize it with the
 * Signature DB data passed in. This call is required if Enumeration of the
 * signatures are required. The Signature DB data should not be changed outside
 * the Context API's (ie until the context is closed), doing so will result into
 * undefined behavior.
 * */
EFIAPI
int OpenSigDBContext (IN  UINT8        *Data,
                      IN  UINT32       SigDBSize,
                      IN  UINT32       DataBufferSize,
                      OUT SigDBHandle  *Hp);

/* Enumerate through all or any filtered signature lists. Should have initialized
 * the Signature DB Enum Context for the variable before using. When called first
 * time the fist signature will be returned, subsequent times next occuring instance
 * of the signature returned according to the initial filters set when called first time.
 * Input arguments are ignored after first time.
 *
 * On Success : Returns 0 (SIG_DB_ENUM_SUCCESS)
 *              Returns all the OUT params for the found Signature to return.
 *
 * On Failure : Returns one of the following Failure Codes
 *              SIG_DB_ENUM_NOT_FOUND
 *              SIG_DB_ENUM_NOT_INITIALIZED
 * */
EFIAPI
int GetNextSignature (IN      SigDBHandle  SDbh,
                      IN OUT  EFI_GUID*    OwnerGUID, 
                      IN OUT  EFI_GUID*    SignatureType,
                      OUT     UINT8*       Signature,
                      IN OUT  UINT32*      SignatureSizePtr,
                      IN      UINT32       BufferSize);

/* Close the active Signature DB Enum context. After this operation the
 * data can be changed.
 * */
EFIAPI
int CloseSigDBContext (IN SigDBHandle SDbh);


/* Find if a Signature with the specified criteria exists in the db. This is
 * just a presence test to determine if we need to append this signature into
 * the DB or not.
 * */
EFIAPI
int FindSignature (IN UINT8*         SigDBData,
                   IN UINT32         SigDBSize,
                   IN OUT EFI_GUID*  OwnerGUID, 
                   IN OUT EFI_GUID*  SignatureType,
                   IN UINT8*         Signature,
                   IN OUT UINT32*    SigSizePtr OPTIONAL
                  );

/* Add Signatures into the Signature DB. All the Params must be given. The Signatures
 * that are not already present will be added into the SigDB
 * This API is not for use outside TZ context
 * */
EFIAPI
int AddSignature (IN  SigDBHandle    SDbh,
                  IN  EFI_GUID*      OwnerGUID, 
                  IN  EFI_GUID*      SignatureType,
                  IN  UINT8*         Signature,
                  IN  UINT32         SignatureSize,
                  OUT UINT32*        NewSigDBSize);

/* Return Supported signature type GUIDs by the SigDB routines in the buffer
 * If the size is more than 0 then the DataBuffer should be a valid buffer
 * pointer. Otherwise the size needed can be expected by passing size
 * as 0 in SizePtr */
EFIAPI
int GetSupportedSignatureTypes (OUT      UINT8*  DataBuffer,
                                IN OUT   UINT32* SizePtr);
#endif /* __UEFI_SIGNATURE_DB_H__ */
