/*=============================================================================

FILE:       rsapubkey.h

SERVICES:   API for cryptographic functions

==============================================================================
        Copyright (c) 2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
            QUALCOMM Proprietary and Confidential
==============================================================================*/
#ifndef PL_RSAPUBKEY_H
#define PL_RSAPUBKEY_H

#include "com_dtypes.h"

// return codes for PL_RSAPubKey operations
#define PL_RSAPUBKEY_SUCCESS           0
#define PL_RSAPUBKEY_E_FAILURE         1
#define PL_RSAPUBKEY_E_INVALID_PARM    2
#define PL_RSAPUBKEY_E_INVALID_DATA    3
#define PL_RSAPUBKEY_E_UNSUPPORTED     4
#define PL_RSAPUBKEY_E_RSA             5
#define PL_RSAPUBKEY_E_HASH            6
#define PL_RSAPUBKEY_E_PAD             7


// signature types
#define PL_RSAPUBKEY_SIG_PKCS1_TYPE1      0
#define PL_RSAPUBKEY_SIG_PKCS1_SHA1       3
#define PL_RSAPUBKEY_SIG_PKCS1_SHA256     4
#define PL_RSAPUBKEY_SIG_PKCS1_SHA384     5
#define PL_RSAPUBKEY_SIG_PKCS1_SHA512     6
#define PL_RSAPUBKEY_SIG_PKCS1_SHA224     7


// Handle of PL_RSAPubKey instance
typedef struct PL_RSAPubKey PL_RSAPubKey;


int PL_RSAPubKey_Open(PL_RSAPubKey **pp,
                   const byte* pModulus, int nModulusSize,
                   const byte* pExponent, int nExponentSize);

void PL_RSAPubKey_Close(PL_RSAPubKey *me);

int PL_RSAPubKey_Verify(PL_RSAPubKey *me, const byte* pSig, int cbSig,
                     const byte* pDigest, int cbDigest, int nSigType);



#endif   // PL_RSAPUBKEY_H
