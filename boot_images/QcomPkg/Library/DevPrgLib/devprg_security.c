/**************************************************************************
 * FILE: devprg_security.c
 *
 * Abstraction layer for the security APIs.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_security.c#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-07-14   sl    Move functions that change between targets to target file.
2016-11-22   wek   Create.

===========================================================================*/

#include "comdef.h"
#include "devprg_log.h"
#include "devprg_sha.h"
#include "devprg_security.h"


#ifdef FEATURE_DEVPRG_SOFTWARE_HASH  /* No hardware SHA, use software. */
static void devprg_sha256_i(uint8* inputPtr, uint32 inputLen, uint8* outputPtr)
{
  struct __sechsh_ctx_s   context;

  sechsharm_sha256_init  (&context);
  sechsharm_sha256_update(&context,
                        context.leftover,
                        &(context.leftover_size),
                        inputPtr,
                        inputLen);

  sechsharm_sha256_final (&context,
                        context.leftover,
                        &(context.leftover_size),
                        outputPtr);

}

#else
static void devprg_sha256_i(uint8* inputPtr, uint32 inputLen, uint8* outputPtr)
{
    CeMLCntxHandle*     cntx = NULL;
    CEMLIovecListType   ioVecIn;
    CEMLIovecListType   ioVecOut;
    CEMLIovecType       IovecIn;
    CEMLIovecType       IovecOut;
    uint32              digest_len = CEML_HASH_DIGEST_SIZE_SHA256;

    /* Input IOVEC */
    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = inputLen; //msg_len;
    ioVecIn.iov[0].pvBase = inputPtr; //msg;

    /* Output IOVEC */
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = digest_len;
    ioVecOut.iov[0].pvBase = outputPtr; //result_digest;


    CeMLInit();
    CeMLHashInit(&cntx, CEML_HASH_ALGO_SHA256);
    CeMLHashUpdate(cntx, ioVecIn);

    CeMLHashFinal(cntx, &ioVecOut);

    CeMLHashDeInit(&cntx);

    CeMLDeInit();
}
#endif

void devprg_sha256_direct(uint8* input, uint32 len, uint8* sha_out)
{
  devprg_sha256_i(input, len, sha_out);
}


