/*==================================================================

GENERAL DESCRIPTION
  This module contains the software implementation for sha256.

Copyright (c) 2015,2017 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
=========================================================================*/

/*=========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the
  module. Notice that changes are listed in reverse chronological
  order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/DevPrgLib/devprg_sha.h#1 $
  $DateTime: 2017/09/18 09:26:13 $

YYYY-MM-DD   who   what, where, why
----------   ---   ----------------------------------------------
2017-07-14   sl    Move functions that change between targets to target file.
2015-08-04   wek   Create. Move SHA functions from security to a new file.

=========================================================================*/

#ifndef DEVICEPROGRAMMER_SHA_H
#define DEVICEPROGRAMMER_SHA_H

#include "comdef.h"
#include "devprg_target.h"

#define CONTEXT_LEFTOVER_FIELD_SIZE 64

/* This is the state of a running hash, known by all platforms */
struct __sechsh_ctx_s
{
    uint32  counter[2];
    uint32  iv[16];  // is 64 byte for SHA2-512
    uint8   leftover[CONTEXT_LEFTOVER_FIELD_SIZE];
    uint32  leftover_size;
};

void sechsharm_sha256_init(   struct __sechsh_ctx_s* );
void sechsharm_sha256_update( struct __sechsh_ctx_s*, uint8*, uint32*, uint8*, uint32 );
void sechsharm_sha256_final(  struct __sechsh_ctx_s*, uint8*, uint32*, uint8* );


#endif
