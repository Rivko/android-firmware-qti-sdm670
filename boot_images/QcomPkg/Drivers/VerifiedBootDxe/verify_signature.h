/** @file  
                        verify_signature.h

  Header file containg the wrapper function for ce_rsa_verify_signature
 
  Copyright (c) 2016 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                EDIT HISTORY


 when       who    what, where, why
 --------   ---     ----------------------------------------------------------
 07/28/16   SA     Initial version

=============================================================================*/

#ifndef VERIFY_SIGNATURE_H
#define VERIFY_SIGNATURE_H

#include "hash_defs.h"

EFI_STATUS VerifySignature(
    CONST UINT8 *signature_ptr,
    UINT32      signature_len,
    UINT8       *hash_out,
    VB_HASH     hash_algorithm,
    CONST UINT8 *modules,
    UINT32      modules_len,
    CONST UINT8 *public_exp,
    UINT32      public_exp_len);

#endif
