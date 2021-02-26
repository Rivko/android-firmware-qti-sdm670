#ifndef CE_RSA_KEYGEN_H
#define CE_RSA_KEYGEN_H
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/secrsa/shared/inc/secrsa_keygen.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  09/11/12   yk     initial version
=============================================================================*/

#include "secrsa.h"

#define MAX_PUB_EXP_SIZE 5 // Array of public exponent e.g., {0x01, 0x00, 0x01}
/*
 * RSA private/public key generatio function. The step taken is as follows:
 *
 * 1. generate random number usiung hw rng, size if 2048/1024 (mod size)
 * 2. check if it is prime - p
 * 3. if it is prime, then genrate another rng, check if it is prime - q
 * 4. multiply P.Q = N
 * 5. PHI = (p-1).(q-1)
 * 6. find d = invmod (e, PHI)
 * 7. take two same big numbers, x, y
 * 8. encrypt x with (e, n) using modexp = x1
 * 9. decrypt x1 with (d, n) using mod exp = x2
 * 10. see if x2 and x match, if not go back to (1)
 *  
 * @param[out]  key       The public/private RSA key
 * @param[in]  keylen     RSA key length (Bytes)
 *
 * @return 
 * CE_SUCCESS     - Function executes successfully. \n
 * CE_ERROR_FAILURE - Generic Error. \n
 * CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
 * CE_ERROR_INVALID_ARG - Generic invalid argument. \n
 * CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n
 * CE_ERROR_NO_MEMORY - Out of memory. \n
 * CE_ERROR_INVALID_SIGNATURE - invalid signature. \n 
 */
 
int ce_rsa_key_gen(CE_RSA_KEY *key, int keylen, unsigned char *pub_exp, int pub_exp_len);
#endif

