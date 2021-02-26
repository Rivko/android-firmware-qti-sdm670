#ifndef CE_RSA_ERR_H
#define CE_RSA_ERR_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

   This file contains the definitions of the RSA interface ERROR return enum.
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/secrsa/secrsa_err.h#2 $
  $DateTime: 2020/09/28 06:39:15 $ 
  $Author: pwbldsvc $

=============================================================================*/
#include "crypto/secmath.h"

typedef enum
{
  /* Result OK */
  CE_SUCCESS                  = E_SECMATH_SUCCESS, 
  /* Generic Error */
  CE_ERROR_FAILURE            = E_SECMATH_FAILURE, 
  /* Not a failure but no operation was performed */
  CE_ERROR_NOP                = E_SECMATH_NOP,      
  /* Algorithm failed test vectors */
  CE_ERROR_FAIL_TESTVECTOR    = E_SECMATH_FAIL_TESTVECTOR,     
  /* Not enough space for output */
  CE_ERROR_BUFFER_OVERFLOW    = E_SECMATH_BUFFER_OVERFLOW,     
  /* Out of memory */
  CE_ERROR_NO_MEMORY          = E_SECMATH_MEM,                 
  /* Generic invalid argument */
  CE_ERROR_INVALID_ARG        = E_SECMATH_INVALID_ARG,         
  /* Invalid size of prime requested */
  CE_ERROR_INVALID_SIZE       = E_SECMATH_INVALID_PRIME_SIZE,  
  /* Operation not supported */
  CE_ERROR_NOT_SUPPORTED      = E_SECMATH_NOT_SUPPORTED,       
  /* invalid signature  */
  CE_ERROR_INVALID_SIGNATURE = 0x1000,                         
  /* decrypt error  */
  CE_ERROR_DECRYPT_ERROR     = 0x1002,                         
  /* invalid packet  */
  CE_ERROR_INVALID_PACKET    = 0x1003,                         
  /* message too long  */
  CE_ERROR_MESSAGE_TOO_LONG  = 0x1004,                         
  /* Modulus too small (OAEP) */
  CE_ERROR_MODULUS_TOO_SMALL = 0x1005,
} CeErrorType;

#endif

