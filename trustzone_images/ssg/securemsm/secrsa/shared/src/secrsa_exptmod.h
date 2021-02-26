#ifndef CE_RSA_EXPTMOD_H
#define CE_RSA_EXPTMOD_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/secrsa/shared/src/secrsa_exptmod.h#1 $
  $DateTime: 2018/02/06 03:00:17 $ 
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "secrsa.h"
#include "secrsa_err.h"
#include "secrsa_utils.h"


/*===========================================================================

                            FUNCTION DEFINITIONS

===========================================================================*/

/** 
   Compute an RSA modular exponentiation 
  @param[in]  key        The RSA key to use 
  @param[in]  in         The input data to send into RSA
  @param[in]  inlen      The length of the input (octets)
  @param[out] out        The destination 
  @param[in,out] outlen  The max size and resulting size of the output
  @param[in]  which      Which exponent to use, e.g. PRIVATE or PUBLIC

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 

*/   
int ce_rsa_exptmod
(
   CE_RSA_KEY              *key,
   const unsigned char     *in,   
   int                     inlen,
   unsigned char           *out,  
   int                     *outlen, 
   int                     which
);

#ifdef __cplusplus
}
#endif

#endif
