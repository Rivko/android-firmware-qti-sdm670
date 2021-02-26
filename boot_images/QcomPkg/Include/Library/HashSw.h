#ifndef HASHSW_H
#define HASHSW_H
/*
                    HashSw.h

  Sw Hash header file, interface between HashDxe and SW Hash library

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/12   bn      Added UEFI header files to avoid build errors 
11/12/11   shl     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Include/com_dtypes.h>
#include <Uefi.h>
#include <Library/BaseLib.h>

#define SHA256_BLOCK_LENGTH      64
#define SW_SHA1_DIGEST_SIZE      20
#define SW_SHA256_DIGEST_SIZE    32
#define SW_SHA_BLOCK_SIZE        64

/* This is the state of a running hash, known by all platforms */
struct __sechsh_ctx_s
{
  uint32  counter[2];
  uint32  iv[16];          // is 64 byte for SHA2-512
  uint8   leftover[64];
  uint16  leftover_size;
};

typedef enum
{
  SW_AUTH_ALG_SHA1    = 0x1,
  SW_AUTH_ALG_SHA256
} SW_Auth_Alg_Type;

typedef struct 
{
  struct __sechsh_ctx_s  ctx; 
  SW_Auth_Alg_Type       Algo;
  UINT8                  Digest[64];
  UINT16                 DigestSize; 
} SW_SHA_Ctx;

EFI_STATUS HashInitSw
(
  SW_SHA_Ctx *pCtx
); 

EFI_STATUS HashUpdateSw
(
  SW_SHA_Ctx       *pCtx,
  UINT8            *pData, 
  UINT32            nDataLen
);

EFI_STATUS HashFinalSw
(
  SW_SHA_Ctx       *pCtx
);

#endif // HASHSW_H
