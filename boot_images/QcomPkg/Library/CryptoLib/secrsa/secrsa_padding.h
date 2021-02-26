#ifndef CE_RSA_PADDING_H
#define CE_RSA_PADDING_H

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This file contains the definitions of the constants, data structures
   and interfaces that provide RSA PKCS #1 v1.5 & v2.1 Alogrithms support. It
   defines the interfaces for RSA padding
  ===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CryptoLib/secrsa/secrsa_padding.h#1 $
  $DateTime: 2017/09/18 09:26:13 $ 
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  04/11/11   qxu     initial version
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto/secrsa.h"
#include "secrsa_err.h"
#include "secrsa_utils.h"
/** 
  PKCS #1 v1.5 encode.
 
  @param[in]  msg              The data to encode
  @param[in]  msglen           The length of the data to encode (octets)
  @param[in]  padding_type     Block type to use in padding
  @param[in]  modulus_bitlen   The bit length of the RSA modulus
  @param[in]  hashidx          The hash index used for signature
  @param[out] out              [out] The destination for the encoded data
  @param[in,out] outlen        [in/out] The max size and resulting size of the encoded data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_v1_5_padding_encode
(
   const unsigned char     *msg,
   int                     msglen,
   CE_RSA_PADDING_TYPE     padding_type,
   int                     modulus_bitlen,
   int                     hashidx,
   unsigned char           *encmsg,
   int                     *encmsglen
);

/** 
  PKCS #1 v1.5 decode.

  @param[in] msg              The encoded data to decode
  @param[in] msglen           The length of the encoded data (octets)
  @param[in] block_type       Block type to use in padding (\sa ltc_pkcs_1_v1_5_blocks)
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[out] decmsg          Destination of decoding
  @param[in, out] decmsglen   The max size and resulting size of the decoding


  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_v1_5_padding_decode
(
   const unsigned char   *msg, 
   int                   msglen,
   CE_RSA_PADDING_TYPE   padding_type,
   int                   modulus_bitlen,
   int                   hashidx,
   unsigned char         *decmsg, 
   int                   *decmsglen
);

/**
  PKCS #1 OAEP encode.
 
  @param[in] msg              The data to encode
  @param[in] msglen           The length of the data to encode (octets) 
  @param[in] lparam           Label to  identify which system 
                              encoded the message
  @param[in] lparamlen        The length of Label
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[in] hashidx          The hash index for encryption
  @param[out] out             [out] The destination for the encoded data
  @param[in,out] outlen       [in/out] The max size and resulting size of the encoded data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_oaep_padding_encode
(
   const unsigned char     *msg,
   unsigned long           msglen,
   const unsigned char     *lparam,
   unsigned long           lparamlen,
   unsigned long           modulus_bitlen,
   int                     hash_idx,
   unsigned char           *out,
   unsigned long           *outlen
);

/**
  PKCS #1 OAEP decode.

  @param[in] msg             The encoded data to decode
  @param[in] msglen          The length of the encoded data (octets)
  @param[in] lparam          Label to  identify which system 
                             encoded the message
  @param[in] lparamlen       The length of Label
  @param[in] modulus_bitlen  The bit length of the RSA modulus
  @param[out] out            Destination of decoding
  @param[in, out] outlen     The max size and resulting size of 
                             the decoding


  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_oaep_padding_decode
(
   const unsigned char       *msg,
   unsigned long             msglen,
   const unsigned char       *lparam,
   unsigned long             lparamlen,
   unsigned long             modulus_bitlen,
   int                       hash_idx,
   unsigned char             *out,
   unsigned long             *outlen
);

/**
  PKCS #1 PSS encode.
 
  @param[in] msghash         The data to encode
  @param[in] msghashlen      The length of the data to encode(octets)
  @param[in] saltlen         The desired length of salt
  @param[in] modulus_bitlen  The bit length of the RSA modulus
  @param[in] hashidx         The hash index used for signature
  @param[out] out            [out] The destination for the encoded data
  @param[in,out] outlen      [in/out] The max size and resulting size of the encoded data

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_INVALID_ARG - Invalid input parameter. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

  @dependencies
  None. 
 */
int ce_pkcs1_pss_padding_encode
(
   const unsigned char     *msghash,
   unsigned long           msghashlen,
   unsigned long           saltlen,
   unsigned long           modulus_bitlen,
   int                     hash_idx,
   unsigned char           *out,
   unsigned long           *outlen
);

/**
  PKCS #1 PSS verify.

  @param[in] msghash          The encoded data to decode
  @param[in] msghashlen       The length of the encoded data (octets)
  @param[in] sig              The signature
  @param[in] siglen           The signature length
  @param[in] saltlen          The length of salt
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[in] hashidx          The hash index used for signature

  @return 
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_pss_padding_verify
(
   const unsigned char       *msghash,
   unsigned long             msghashlen,
   const unsigned char       *sig,
   unsigned long             siglen,
   unsigned long             saltlen,
   unsigned long             modulus_bitlen,
   int                       hash_idx
);

/**
  PKCS #1 PSS verify.

  @param[in] msghash          The encoded data to decode
  @param[in] msghashlen       The length of the encoded data (octets)
  @param[in] sig              The signature
  @param[in] siglen           The signature length
  @param[in] saltlen          The length of salt
  @param[in] modulus_bitlen   The bit length of the RSA modulus
  @param[in] hashidx          The hash index used for signature

  @return
   CE_SUCCESS     - Function executes successfully. \n
   CE_ERROR_NOT_SUPPORTED - the feature is not supported. \n
   CE_ERROR_INVALID_PACKET - invalid packet. \n
   CE_ERROR_BUFFER_OVERFLOW - not enough space for output. \n

 */
int ce_pkcs1_pss_padding_verify_auto_recover_saltlen
(
   const unsigned char       *msghash,
   unsigned long             msghashlen,
   const unsigned char       *sig,
   unsigned long             siglen,
   unsigned long             modulus_bitlen,
   int                       hash_idx
);
#ifdef __cplusplus
}
#endif

#endif
