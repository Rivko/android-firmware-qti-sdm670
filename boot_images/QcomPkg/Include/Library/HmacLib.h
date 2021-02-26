#ifndef HMACLIB_H
#define HMACLIB_H
/*
                    HmacLib.h

  Copyright (c) 2012 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/12   shl     initial version
===========================================================================*/


#include <Include/com_dtypes.h>
#include <Uefi.h>
#include <Library/BaseLib.h>

EFI_STATUS 
HmacLib (UINT8              * key_ptr, 
         UINT32               keylen, 
         UINT8              * msg_ptr,
         UINT32               msglen,
         UINT8              * hash_ptr);

#endif // HMACLIB_H
