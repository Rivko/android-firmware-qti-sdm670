/** @file UefiSigDb.c
   
  Contains UEFI Signature DB handling Library functions.

  Copyright (c) 2011-2012,2014, Qualcomm Technologies Inc. All rights reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/18/14   vk      Fix gcc warnings
 05/07/12   yg      Addressed some possible security vulnerabilities.
 09/14/11   yg      Add function to get supported signatures
 08/17/11   yg      Created new file

=============================================================================*/


#include <Library/UefiSigDb.h>
#include <Library/BaseMemoryLib.h>


#if 0
/* Private types for easy identification */
typedef enum {
   EFI_CERT_SHA256_TYPE = 0,
   EFI_CERT_RSA2048_TYPE = 1,
   EFI_CERT_RSA2048_SHA256_TYPE = 2,
   EFI_CERT_SHA1_TYPE = 3,
   EFI_CERT_RSA2048_SHA1_TYPE = 4,
   EFI_CERT_SHA224_TYPE = 5,
   EFI_CERT_SHA384_TYPE = 6,
   EFI_CERT_SHA512_TYPE = 7,
   EFI_CERT_X509_TYPE = 8
} SignatureType;
#endif

/*
 *   Signature DB data format:
 *     - Signature DB is comprised of Zero or more "Signature Lists"
 *     - Size of Signature DB is the size of the Variable
 *     - Each signature list is a list of signatures of a type
 *     - The type of signature is indicated by the SignatureType Field
 *     - The Signature List contains a header and an array of Zero or more signatures
 *
 *  
 *   Signature DB:
 *   -------------
 *   +-------------+
 *   |  Signature  |
 *   |    List 0   |
 *   +-------------+
 *   |             |
 *   |             |
 *   |             |
 *   |             |
 *   |             |
 *   |  Signature  |
 *   |    List 1   |
 *   |             |
 *   |             |
 *   |             |
 *   +-------------+
 *   |             |
 *   |             |
 *   |  Signature  |
 *   |    List 2   |
 *   |             |
 *   |             |
 *   +-------------+
 *
 *   Signature List:
 *   +-------------------------+
 *   |  Signature List Header  |
 *   +-------------------------+
 *   |   Signature Header (A)  |
 *   +-------------------------+
 *   |       Signature #0      |
 *   +-------------------------+
 *   |       Signature #1      |
 *   +-------------------------+
 *   |       Signature #2      |
 *   +-------------------------+
 *   |       Signature #3      |
 *   +-------------------------+
 *   |            .            |
 *   |            .            |
 *   +-------------------------+
 *   |       Signature #n      |
 *   +-------------------------+
 *
 *  Signature Types and Header Size + GUID Size + Signature Size
 *
 *  EFI_CERT_SHA256_GUID          0 + 16 + 32
 *  EFI_CERT_RSA2048_GUID         0 + 16 + 256
 *  EFI_CERT_RSA2048_SHA256_GUID  0 + 16 + 256
 *  EFI_CERT_SHA1_GUID            0 + 16 + 20
 *  EFI_CERT_RSA2048_SHA1_GUID    0 + 16 + 256
 *  EFI_CERT_SHA224_GUID          0 + 16 + 28
 *  EFI_CERT_SHA384_GUID          0 + 16 + 48
 *  EFI_CERT_SHA512_GUID          0 + 16 + 64
 *  EFI_CERT_X509_GUID    Size varies, different certs of the same chain could be in different lists
 *
 * */

typedef struct _EFI_SIGNATURE_DATA {
  /* 16 byte Owner GUID */
  GUID       SignatureOwner;
  /* Free form data, size depends on the type of the Signature represented */
  UINT8      SignatureData[1];
} EFI_SIGNATURE_DATA;

typedef struct _EFI_SIGNATURE_LIST {
  GUID       SignatureType;       // 16 Byte GUID indicating signature type
  UINT32     SignatureListSize;   // Signature List Size including this header
  UINT32     SignatureHeaderSize; // Size of Signature Header's (A) size
  UINT32     SignatureSize;       // Size of each signature, typically minimum EFI_SIGNATURE_DATA size

  /* Following are present in free form based on which signature type is
   * stored in the list. So the size of the list varies by type and how many
   * signatures present */
  // UINT8  SignatureHeader[SignatureHeaderSize];
  // EFI_SIGNATURE_DATA Signatures[][SignatureSize];
}EFI_SIGNATURE_LIST;

typedef struct {
  /* Signature "List" that's currently being traversed
   * If it points to end of the SigDB then the traversing has completed */
  EFI_SIGNATURE_LIST* CurrentSigList;

  /* Signature that's Enumerated currently
   * If the Value is NULL Traversing has not been started
   * If it points past the current List then the traversing has completed */
  EFI_SIGNATURE_DATA* CurrentSigData;

  /* GUID Filters used while traversing */
  GUID    OwnerGUID;
  GUID    SigTypeGUID;
  UINT32  FilterSigSize;

  /* Buffer location where Signature buffer is starting */
  UINT8*  SigDBStart;
  UINT32  SigDBBufferSize;

  /* Where the Signature DB ends. */
  UINT8*  SigDBEnd;

  /* This context instance is in use */
  UINT8   CtxInUse;

  /* Filters in use */
  UINT8   FilterByOwner;   // Should be 0 if filter is not needed
  UINT8   FilterBySigType; // Should be 0 if filter is not needed

  /* For future use */
  UINT8   Reserved;
}SigDBCtx;

typedef enum {
  SIG_DB_SUCCESS = 0,
  SIG_DB_NOT_FOUND = 1,
  SIG_DB_NOT_INITIALIZED = 2
}SIG_DB_ERROR_TYPE;

GUID NullGuid = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0} };
GUID InvalidGuid = {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

/* EFI_CERT_SHA256_GUID          0 + 16 + 32 */
GUID CertSHA256Guid   = {0xC1C41626, 0x504C, 0x4092, {0xAC, 0xA9, 0x41, 0xF9, 0x36, 0x93, 0x43, 0x28}};

/* EFI_CERT_RSA2048_GUID         0 + 16 + 256 */
GUID CertRSA2048Guid  = {0x3C5766E8, 0x269C, 0x4E34, {0xAA, 0x14, 0xED, 0x77, 0x6E, 0x85, 0xB3, 0xB6}};

/*  EFI_CERT_RSA2048_SHA256_GUID  0 + 16 + 256 */
GUID CertRSA2048SHA256Guid  = {0xE2B36190, 0x879B, 0x4A3D, {0xAD, 0x8D, 0xF2, 0xE7, 0xBB, 0xA3, 0x27, 0x84}};

/*  EFI_CERT_SHA1_GUID            0 + 16 + 20  */
GUID CertSHA1Guid = {0x826CA512, 0xCF10, 0x4AC9, {0xB1, 0x87, 0xBE, 0x01, 0x49, 0x66, 0x31, 0xBD}};

/*  EFI_CERT_X509_GUID            0 + 16 + Variable  */
GUID CertX509Guid = {0xA5C059A1, 0x94E4, 0x4AA7, {0x87, 0xB5, 0xAB, 0x15, 0x5C, 0x2B, 0xF0, 0x72}};

#define VARIABLE_SIGNATURE_SIZE                 0xFFFFFFFF

typedef struct {
  GUID*   SignatureType;
  UINT32  HeaderSize;
  UINT32  SignatureSize;
}SignatureConfig;

SignatureConfig SupportedSignatureTypes[] = 
{ 
  {&CertSHA256Guid,         0, 16 + 32  },
  {&CertSHA1Guid,           0, 16 + 20  },
  {&CertRSA2048Guid,        0, 16 + 256 },
  {&CertRSA2048SHA256Guid,  0, 16 + 256 },
  {&CertX509Guid,           0, VARIABLE_SIGNATURE_SIZE }
};

#define MAX_ENUM_CONTEXTS   8

SigDBCtx EnumContext[MAX_ENUM_CONTEXTS];

int ResetSigDBContext (IN SigDBHandle SDbh)
{
  SigDBCtx* EnumCtx = (SigDBCtx*)SDbh;

  EnumCtx->CurrentSigList = 0;
  EnumCtx->CurrentSigData = 0;
  return 0;
}

// typedef SigDBCtx* SigDBHandle;

/* Allocate a Signature DB Enumeration Context and initialize it with the
 * Signature DB data passed in. This call is required if Enumeration of the
 * signatures are required. The Signature DB data should not be changed outside
 * the Context API's (ie until the context is closed), doing so will result into
 * undefined behavior.
 *
 * DataBufferSize indicates how big is the actual buffer holding the Signature DB
 * Having a big buffer than the Signature DB enables adding the signatures to the
 * Signature DB using the AddSignature API.
 * */
EFIAPI
int OpenSigDBContext (IN UINT8* SigDBData,
                      IN UINT32 SigDBSize,
                      IN UINT32 DataBufferSize,
                      OUT SigDBHandle* Hp)
{
  int i;
  SigDBCtx* EnumCtx = 0;
  UINTN Ptr;

  /* Do a sanity check on all except SigDBSize. Its ok to have
   * SigDBSize null for the case of uninitialized SigDB */
  if ((Hp == 0) || (SigDBData == 0) || (DataBufferSize == 0))
    return -1;

  /* Check for wrap around */
  Ptr = (UINTN)SigDBData;
  if ((Ptr + SigDBSize) < Ptr)
    return -3;

  *Hp = 0;

  /* Search for a free context */
  for (i = 0; i < MAX_ENUM_CONTEXTS; ++i)
  {
    if (EnumContext[i].CtxInUse == 0)
    {
      EnumCtx = &EnumContext[i];
      EnumCtx->CtxInUse = 1;
      break;
    }
  }

  /* If no free context, just return failure */
  if (EnumCtx == 0)
    return -2;

  EnumCtx->SigDBStart = SigDBData;
  EnumCtx->SigDBBufferSize = DataBufferSize;

  EnumCtx->CurrentSigList = 0;
  EnumCtx->SigDBEnd = SigDBData + SigDBSize; // size is already validated above 
  EnumCtx->FilterSigSize = 0;
  EnumCtx->CurrentSigData = 0;
  EnumCtx->FilterByOwner = 0;
  EnumCtx->FilterBySigType = 0;

  *Hp = (SigDBHandle)EnumCtx;

  return 0;
}

/* Get next SigDB List unfiltered */
EFI_SIGNATURE_LIST* GetNextList (SigDBCtx* EnumCtx)
{
  UINTN TempPtr;

  /* If we didn't even start, then return the first one */
  if (EnumCtx->CurrentSigList == 0)
  {
    EnumCtx->CurrentSigList = (EFI_SIGNATURE_LIST *)EnumCtx->SigDBStart;

    /* Do we even have a valid list yet? */
    if ((UINT8*)EnumCtx->CurrentSigList >= EnumCtx->SigDBEnd)
      return 0;

    return EnumCtx->CurrentSigList;
  }

  /* Return Null if its already past the SigDB */
  if ((UINT8*)EnumCtx->CurrentSigList >= EnumCtx->SigDBEnd)
    return 0;

  TempPtr = (UINTN)EnumCtx->CurrentSigList;

  /* Check for Wrap around */
  if ((TempPtr + EnumCtx->CurrentSigList->SignatureListSize) < TempPtr)
    return 0;

  /* Hop to Next one */
  EnumCtx->CurrentSigList = (EFI_SIGNATURE_LIST*)((UINT8*)EnumCtx->CurrentSigList +
                            EnumCtx->CurrentSigList->SignatureListSize);

  if ((UINT8*)EnumCtx->CurrentSigList >= EnumCtx->SigDBEnd)
    return 0;

  /* Reset SigData to start enumerating */
  EnumCtx->CurrentSigData = 0;

  return EnumCtx->CurrentSigList;
}

/* Search for a SigDB List by Signature Type */
EFI_SIGNATURE_LIST* GetNextListBySigType (SigDBCtx* EnumCtx, GUID* Id, UINT32 SignatureSize)
{
  EFI_SIGNATURE_LIST* Listp;

  /* Loop through until we find a match or go past the limit */
  while (1)
  {
    /* Get Next List */
    Listp = GetNextList (EnumCtx);

    if (Listp == 0)
      return 0;

    /* Check if this what we were looking for */
    if (CompareGuid(&EnumCtx->CurrentSigList->SignatureType, Id))
    {
      if ((SignatureSize == DEFAULT_SIGNATURE_SIZE) ||
          (SignatureSize == EnumCtx->CurrentSigList->SignatureSize - sizeof(GUID)))
        return EnumCtx->CurrentSigList;
    }
  }
}

/* Get Next Signature in the List, unfiltered */
EFI_SIGNATURE_DATA* GetNextSignatureInList (SigDBCtx* EnumCtx)
{
  UINT8 *ListEnd;
  UINTN TempPtr;

  /* If the List is not selected yet, return 0 */
  if (EnumCtx->CurrentSigList == 0)
    return 0;

  TempPtr = (UINTN)EnumCtx->CurrentSigList;

  /* Check wrap around */
  if ((TempPtr + EnumCtx->CurrentSigList->SignatureListSize) < TempPtr)
    return 0;

  ListEnd = (UINT8*)EnumCtx->CurrentSigList + EnumCtx->CurrentSigList->SignatureListSize;

  /* Check for either list end or SigDB End */
  if (ListEnd >= EnumCtx->SigDBEnd)
    ListEnd = EnumCtx->SigDBEnd;

  /* If enumeration has not started yet, return first one */
  if (EnumCtx->CurrentSigData == 0)
  {
    UINTN temp_ptr = (UINTN)(EnumCtx->CurrentSigList + 1);   // Point past the headers

    /* Check for Wrap around */
    if ((temp_ptr + EnumCtx->CurrentSigList->SignatureHeaderSize) < temp_ptr)
      return 0;

    EnumCtx->CurrentSigData = (EFI_SIGNATURE_DATA*)(temp_ptr + EnumCtx->CurrentSigList->SignatureHeaderSize);

    if ((UINT8*)EnumCtx->CurrentSigData < ListEnd)
      return EnumCtx->CurrentSigData;
    else
      return 0;
  }

  /* If the pointer is past the List, then we don't have much to traverse */
  if ((UINT8*)EnumCtx->CurrentSigData >= ListEnd)
    return 0;

  /* We are here means we still have room to traverse, so hop to next signature */
  {
    UINTN temp_ptr = (UINTN)EnumCtx->CurrentSigData;

    /* Check for Wrap around */
    if ((temp_ptr + EnumCtx->CurrentSigList->SignatureSize) < temp_ptr)
      return 0;

    EnumCtx->CurrentSigData = (EFI_SIGNATURE_DATA*)(temp_ptr + EnumCtx->CurrentSigList->SignatureSize);
  }

  if ((UINT8*)EnumCtx->CurrentSigData >= ListEnd)
    return 0;

  return EnumCtx->CurrentSigData;
}

/* Search for the Signature by owner in the current Pointed list only.
 * Client could traverse through the Lists to select appropriate list by type */
EFI_SIGNATURE_DATA* GetNextSignatureByOwner (SigDBCtx* EnumCtx, GUID* Id)
{
  EFI_SIGNATURE_DATA* SigDat;

  while (1)
  {
    SigDat = GetNextSignatureInList (EnumCtx);

    if (SigDat == 0)
      return 0;

    if (CompareGuid(&SigDat->SignatureOwner, Id))
      return SigDat;
  }
}

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
 *              SIG_DB_ENUM_BUFFER_SIZE_INSUFFICIENT
 * */
EFIAPI
int GetNextSignature (IN      SigDBHandle SDbh,
                      IN OUT  EFI_GUID* OwnerGUID, 
                      IN OUT  EFI_GUID* SignatureType,
                      OUT     UINT8* Signature,
                      IN OUT  UINT32* SignatureSizePtr,
                      IN      UINT32  BufferSize)
{
  EFI_SIGNATURE_LIST* EnumSigList;
  EFI_SIGNATURE_DATA* EnumSigData;
  SigDBCtx* EnumCtx = (SigDBCtx*)SDbh;
  UINT32 CurrentSigSize;


  /* Check if its first time the call was made, then record the filters */
  if (EnumCtx->CurrentSigData == 0)
  {
    /* Check and assign if Owner GUID can be used as filter */
    if (OwnerGUID && !CompareGuid(&NullGuid, OwnerGUID) && !CompareGuid(&InvalidGuid, OwnerGUID))
    {
      EnumCtx->FilterByOwner = 1;
      CopyGuid(&EnumCtx->OwnerGUID, OwnerGUID);
    }

    /* Check and assign if Signature Type can be used as filter */
    if (SignatureType && !CompareGuid(&NullGuid, SignatureType) && !CompareGuid(&InvalidGuid, SignatureType))
    {
      EnumCtx->FilterSigSize = DEFAULT_SIGNATURE_SIZE;
      if (SignatureSizePtr)
        EnumCtx->FilterSigSize = *SignatureSizePtr;

      EnumCtx->FilterBySigType = 1;
      CopyGuid(&EnumCtx->SigTypeGUID, SignatureType);
    }
  }

  /* Step to next List only if we run out of signatures in the current list */
  while(1)
  {
    /* For the list that we have, get the signature based on the Owner criteria */
    if (EnumCtx->FilterByOwner)
      EnumSigData = GetNextSignatureByOwner (EnumCtx, &EnumCtx->OwnerGUID);
    else
      EnumSigData = GetNextSignatureInList (EnumCtx);

    /* If we didn't get the Signature in the current list or its not selcted at all
     * get the next list */
    if (EnumSigData == 0)
    {
      /* Get the next list to next list per filter */
      if (EnumCtx->FilterBySigType)
        EnumSigList = GetNextListBySigType (EnumCtx, &EnumCtx->SigTypeGUID, EnumCtx->FilterSigSize);
      else
        EnumSigList = GetNextList (EnumCtx);

      /* If we ran out of lists we are done */
      if (EnumSigList == 0)
        return SIG_DB_ENUM_NOT_FOUND;

      /* Try to get the Signature again in the new list */
      continue;
    }

    /* Cache the value */
    EnumSigList = EnumCtx->CurrentSigList;
    CurrentSigSize = EnumCtx->CurrentSigList->SignatureSize - sizeof(GUID);

    /* Return the size if interested, if the buffer size is insufficient then get
     * can be issued to get current signature with appropriate size */
    if (SignatureSizePtr)
      *SignatureSizePtr = CurrentSigSize;

    /* We got our Signature Data */
    if (CurrentSigSize > BufferSize)
      return SIG_DB_ENUM_BUFFER_SIZE_INSUFFICIENT;

    /* Return the signature data */
    if (OwnerGUID)
      CopyGuid(OwnerGUID, &EnumSigData->SignatureOwner);

    if (SignatureType)
      CopyGuid(SignatureType, &EnumSigList->SignatureType);

    if (Signature)
      CopyMem(Signature, EnumSigData->SignatureData, EnumSigList->SignatureSize - sizeof(GUID));

    return 0;
  }
}

/* Close the active Signature DB Enum context. After this operation the
 * data can be changed.
 * */
EFIAPI
int CloseSigDBContext (IN SigDBHandle SDbh)
{
//  SigDBCtx *ctx = (SigDBCtx*)SDbh;
  SetMem (SDbh, sizeof(SigDBCtx), 0);
  return 0;
}

UINT8 SigBuffer[2048];
 
/* Find if a Signature with the specified criteria exists in the db. This is
 * just a presence test to determine if we need to append this signature into
 * the DB or not. Signature Type is needed. Owner GUID can be skipped.
 * Signature is buffer in which the signature is returned.
 *
 * Returns 0 on finding the signature
 *         -1 on failure
 */
EFIAPI
int FindSignature (IN UINT8*         SigDBData,
                   IN UINT32         SigDBSize,
                   IN OUT EFI_GUID*  OwnerGUID, 
                   IN OUT EFI_GUID*  SignatureType,
                   IN UINT8*         Signature,
                   IN OUT UINT32*    SigSizePtr OPTIONAL
                  )
{
  int Status;
  SigDBHandle EnumHandle;
  UINT32 SignatureSize;

  SignatureSize = DEFAULT_SIGNATURE_SIZE;
  if (SigSizePtr)
    SignatureSize = *SigSizePtr;

  /* Open a enum context */
  Status = OpenSigDBContext (SigDBData, SigDBSize, SigDBSize, &EnumHandle);
  if (Status != 0)
  {
    return Status;
  }

  /* Parse through with filters */
  do
  {
    Status = GetNextSignature (EnumHandle, OwnerGUID, SignatureType, SigBuffer, &SignatureSize, sizeof(SigBuffer));
    if (Status == 0)
    {
      /* If we get a match to what we are looking for return that */
      if (CompareMem (Signature, SigBuffer, SignatureSize) == 0)
      {
        if (SigSizePtr)
          *SigSizePtr = SignatureSize;

        CloseSigDBContext (EnumHandle);
        return 0;
      }
    }
  }while (Status == 0);

  CloseSigDBContext (EnumHandle);

  return Status;
}

EFI_SIGNATURE_LIST * AddSigList (IN SigDBCtx* EnumCtx, GUID* SigType, UINT32 SignatureSize)
{
  SignatureConfig *SigConfig = 0;
  UINT32 i;
  UINT32 MinMemRequired, MemoryRemaining;
  EFI_SIGNATURE_LIST *NewSigList;

  for (i = 0; i < sizeof(SupportedSignatureTypes) / sizeof(*SupportedSignatureTypes); ++i)
  {
    if (CompareGuid (SigType, SupportedSignatureTypes[i].SignatureType))
    {
      SigConfig = &SupportedSignatureTypes[i];
      break;
    }
  }

  /* If we didn't find the requested SigType then its not supported
   * TODO: add support for any signature type if the list parameters can be specified */
  if (SigConfig == 0)
    return 0;

  /* We would be populating the SignatureSize with configured size. We consider
   * the signature size only for the variable signature size components */
  if (SigConfig->SignatureSize != VARIABLE_SIGNATURE_SIZE)
    SignatureSize = SigConfig->SignatureSize;
  else
  {
    /* Client code would not have accounted for Owner GUID in the signature size */
    SignatureSize = SignatureSize + sizeof(GUID);
  }

  /* Check if we have enough space to add a list and a signature */
  MinMemRequired = SignatureSize + SigConfig->HeaderSize;
  MinMemRequired += sizeof(EFI_SIGNATURE_LIST);

  MemoryRemaining = EnumCtx->SigDBStart + EnumCtx->SigDBBufferSize - EnumCtx->SigDBEnd;

  if (MemoryRemaining < MinMemRequired)
    return 0;

  /* We can have new List at the end of the list */
  NewSigList = (EFI_SIGNATURE_LIST*)EnumCtx->SigDBEnd;

  /* Initialize all the fields before linking it */
  CopyGuid (&NewSigList->SignatureType, SigType);
  NewSigList->SignatureListSize = sizeof(EFI_SIGNATURE_LIST);
  NewSigList->SignatureHeaderSize = SigConfig->HeaderSize;
  NewSigList->SignatureSize = SignatureSize;

  /* Now this list is officially in the SigDB */
  EnumCtx->CurrentSigList = NewSigList;
  EnumCtx->CurrentSigData = 0;
  EnumCtx->SigDBEnd += sizeof(EFI_SIGNATURE_LIST);

  return NewSigList;
}

int AddSignatureToList (IN SigDBCtx* EnumCtx,  IN EFI_SIGNATURE_LIST *DstSigList, IN GUID *OwnerGuid, IN UINT8* Signature)
{
  UINT32 MemSizeToMove, MemoryRemaining;
  UINT8 *MemMoveFrom, *MemMoveTo;

  /* We need correct Signature List be pointed to in Context to make sure
   * that this routine is not spoofed */
  if (DstSigList != EnumCtx->CurrentSigList)
    return -1;
  
  /* Check if we have memory */
  MemoryRemaining = EnumCtx->SigDBStart + EnumCtx->SigDBBufferSize - EnumCtx->SigDBEnd;

  if (MemoryRemaining < DstSigList->SignatureSize)
    return -1;

  /* Prepare the parameters to move memory if we have to insert */
  MemMoveFrom = (UINT8*)DstSigList;
  MemMoveFrom += DstSigList->SignatureListSize;
  MemMoveTo = MemMoveFrom + DstSigList->SignatureSize;  // Signature size include owner GUID + Signature data
  MemSizeToMove = EnumCtx->SigDBEnd - MemMoveFrom;

  /* Make space where we need this signature to be present */
  if (MemSizeToMove)
    CopyMem (MemMoveTo, MemMoveFrom, MemSizeToMove);

  /* Now we can insert the OwnerGUID + Signature */
  CopyGuid ((GUID*)MemMoveFrom, OwnerGuid);
  MemMoveFrom += sizeof(GUID);
  CopyMem (MemMoveFrom, Signature, DstSigList->SignatureSize - sizeof(GUID));

  /* Adjust sizes accordingly *SignatureSize already includes GUID* */
  DstSigList->SignatureListSize += DstSigList->SignatureSize;
  EnumCtx->SigDBEnd += DstSigList->SignatureSize;

  return 0;
}

/* Only the signature type defined as that could have variable size could be different values
 * otherwise it has to match the value given in the definition table */
int ValidateSignatureSize ( IN EFI_GUID* SigType,
                            IN UINT32 SignatureSize)
{
  UINT32 i;

  for (i = 0; i < sizeof(SupportedSignatureTypes) / sizeof(*SupportedSignatureTypes); ++i)
  {
    if (CompareGuid (SigType, SupportedSignatureTypes[i].SignatureType))
    {
      if (SupportedSignatureTypes[i].SignatureSize == VARIABLE_SIGNATURE_SIZE)
        return 0;
      else
      {
        if ((SignatureSize == DEFAULT_SIGNATURE_SIZE) || (SupportedSignatureTypes[i].SignatureSize == SignatureSize + sizeof(GUID)))
          return 0;
        else
          return -1;
      }
    }
  }
          
  return -1;
}

/* Add a single Signature into the Signature DB. All the Params must be given.
 * Caller has to make sure that the signature is not present already.  */
EFIAPI
int AddSignature (IN SigDBHandle SDbh,
                  IN EFI_GUID* OwnerGUID, 
                  IN EFI_GUID* SignatureType,
                  IN UINT8* Signature,
                  IN UINT32 SignatureSize,
                  OUT UINT32* NewSigDBSize)
{
  EFI_SIGNATURE_LIST * DstSigList = 0;
  SigDBCtx* EnumCtx = (SigDBCtx*)SDbh;
  UINT32 NewSize;
  int Res;

  /* Validate the signature size passed in, allow only variable signature type to be
   * of different length. Others should match the size defined */
  if (ValidateSignatureSize (SignatureType, SignatureSize) < 0)
    return SIG_DB_ENUM_INVALID_PARAM;

  /* Reset so that we can start afresh */
  ResetSigDBContext (SDbh);

  /* Check if there is a list already for this type */
  DstSigList = GetNextListBySigType (EnumCtx, SignatureType, SignatureSize);

  /* If we didn't find any or the List is Null then its time to add */
  if (DstSigList == 0)
    DstSigList = AddSigList (EnumCtx, SignatureType, SignatureSize);

  if (DstSigList == 0)
    return -1;

  /* Coming here we should have located or added a list entry
   * Now attempt to insert the signature */
  Res = AddSignatureToList (EnumCtx, DstSigList, OwnerGUID, Signature);

  /* Check how much the size changed */
  NewSize = EnumCtx->SigDBEnd - EnumCtx->SigDBStart;

  /* Return the amount of size change happened */
  if (NewSigDBSize)
    *NewSigDBSize = NewSize;

  if (Res < 0)
    return Res;

  return 0;
}

/* Return SignatureTypes supported by SigDB. If authentication routines
 * support more than this, then those should be taken from Auth routines */
EFIAPI
int GetSupportedSignatureTypes (OUT      UINT8*  DataBuffer,
                                IN OUT   UINT32* SizePtr)
{
  UINT32 i, TotalSize, SignatureCount;
  int Status = SIG_DB_ENUM_SUCCESS;

  /* If the size is non zero then we expect the data buffer to be valid */
  if ((SizePtr == 0) || ((*SizePtr > 0) && (DataBuffer == 0)))
    return SIG_DB_ENUM_INVALID_PARAM;

  /* How many supported signatures we got? */
  SignatureCount = sizeof(SupportedSignatureTypes) / sizeof(*SupportedSignatureTypes);

  /* Total size needed to fill all GUIDs */
  TotalSize = SignatureCount * sizeof(EFI_GUID);

  /* If buffer was not big enough return size and error */
  if (*SizePtr < TotalSize)
  {
    *SizePtr = TotalSize;
    return SIG_DB_ENUM_BUFFER_SIZE_INSUFFICIENT;
  }

  /* Copy GUIDs to buffer */
  for (i = 0; i < SignatureCount; ++i)
  {
    CopyGuid ((GUID*)DataBuffer, SupportedSignatureTypes[i].SignatureType);
    DataBuffer += sizeof(EFI_GUID);
  }

  /* Return the size we filled */
  *SizePtr = TotalSize;

  return Status;
}

