/**
 * Copyright 2014 QUALCOMM Technologies, Inc. 
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 * @file qsee_macchiato.h
 * @author	qxu
 * @version     1.0
 * @since       2014-05-16
 *
 * @brief header file for macchiato_key_service.c
 */

/*===========================================================================
   Copyright (c) 2014-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  
   This file contains the definitions of the constants, data structures
   and interfaces that provide Macchiato RSA Key service support. It defines
   the external interfaces for Macchiato RSA key encrpytion/decryption, signature/
   verification, key generation.
  ===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  when       who      what, where, why
  --------   ---      ------------------------------------
  10/12/15   rk       Added new API to get public signing key
  02/10/15   cz       Added more error codes
  05/11/14   cz       Added qsee macchiato APIs
  28/05/14   qxu      First version of qsee_macchiato.h 

===========================================================================*/

#ifndef QSEE_MACCHIATO_H
#define QSEE_MACCHIATO_H

#include <stdint.h>

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* Error codes */
 typedef enum {
    MACCHIATO_SUCCESS                                = 0,
    MACCHIATO_KEYGEN_INVALID_PARAM                   = 1,
    MACCHIATO_KEYGEN_CIPHER_ERROR                    = 2,
    MACCHIATO_INVALID_QSEECOM_PARAM                  = 3,
    MACCHIATO_INVALID_PARAM                          = 4,   /* Invalid arguments passed to the API */
    MACCHIATO_NOT_SUPPORTED                          = 5,   /* Operation Not supported */
    MACCHIATO_GENERAL_ERROR                          = 6,


    /*macchiato_ecc.c*/	                            
    MACCHIATO_SIGNATURE_BUFFER_TOO_SMALL             = 20,   /* memory buffer from caller is too small */
    MACCHIATO_HASH_ERROR                             = 21,   /* hash operation error */
    MACCHIATO_SIGNATURE_NOMATCH                      = 22,   /* signature doesn't match */
    MACCHIATO_ECDSA_ERROR                            = 23,   /* ecdsa operation error */


    /*macchiato_service.c*/	    
    MACCHIATO_SERVICE_BUFFER_TOO_SMALL	             = 40,   /* Macchiato service buffer is too small*/
    MACCHIATO_SERVICE_INPUT_POINTER_NULL             = 41,   /* input pointer is NULL */
    MACCHIATO_SERVICE_INPUT_INVALID                  = 42,   /* input is invalid data */
    MACCHIATO_SERVICE_INT_OVERFLOW                   = 43,   /* integer overflow */
    MACCHIATO_SERVICE_READ_FUSE_FAILED               = 44,   /* reading a fuse failed */


    /*macchiato.c*/	    
    MACCHIATO_FAILED_GET_SECURE_STATE                = 60,  /* failed to get MSA/APP secure state */
    MACCHIATO_UNSUCCESSFUL_ERROR_CODE                = 61,  /* unsuccessful error code*/
    MACCHIATO_FAILED_SIGN_DATA                       = 62,  /* signing service data failed */
    MACCHIATO_TOO_SHORT_OUT_BUFFER_ERROR             = 63, 
    MACCHIATO_ALLOCATION_ERROR                       = 64,
    MACCHIATO_GOT_NULL_INPUT_ERROR                   = 65,
    MACCHIATO_FAILED_GENERATE_RANDOM_RESPONSE        = 66, 
    MACCHIATO_FAILED_GENERATE_KEYS                   = 67,
    MACCHIATO_INVALID_INPUT_ERROR                    = 68,
    MACCHIATO_FAILED_STORING_SERVICE_KEY             = 69,
    MACCHIATO_PROTOCOL_VERSION_MISMATCH              = 70,
    MACCHIATO_ERROR_UNEXPECTED_MSG_ID                = 71,
    MACCHIATO_ERROR_FEATURE_NOT_SUPPORTED            = 72,
    MACCHIATO_INVALID_OEMID                          = 73,
    MACCHIATO_ERROR_INVALID_HWVERSION                = 74,
    MACCHIATO_INVALID_SERVICEID                      = 75, 
    MACCHIATO_ERROR_GET_TZ_APP_ID                    = 76, 
    MACCHIATO_ERROR_QSEE_FRAMEWORK                   = 77,
    MACCHIATO_ERROR_GET_OEM_PK_HASH                  = 78,
    MACCHIATO_ERROR_GET_TA_HASH                      = 79,
    MACCHIATO_ERROR_GET_UTC_SECOND                   = 80,

    /*macchiato_eciesad.h */
    MACCHIATO_ERROR_HASH_INIT                        = 90,
    MACCHIATO_ERROR_HASH_UPDATE                      = 91,
    MACCHIATO_ERROR_HASH_FINAL                       = 92,
    MACCHIATO_ERROR_HASH_FREE                        = 93,
    MACCHIATO_ERROR_CIPHER_INIT                      = 94,
    MACCHIATO_ERROR_CIPHER_SET_KEY                   = 95,
    MACCHIATO_ERROR_CIPHER_SET_MODE                  = 96,
    MACCHIATO_ERROR_CIPHER_SET_IV                    = 97,
    MACCHIATO_ERROR_CIPHER_ENCRYPT                   = 98,
    MACCHIATO_ERROR_CIPHER_DECRYPT                   = 99,
    MACCHIATO_ERROR_CIPHER_DEINT                     = 100,
    MACCHIATO_ERROR_BAD_DATA                         = 101,
    MACCHIATO_ERROR_INVALID_ARG                      = 102,
    MACCHIATO_ERROR_HMAC_FAILED                      = 103,
    MACCHIATO_ERROR_ECIES_COMPARISON_FAILED          = 104,
    MACCHIATO_ERROR_SW_ECDH_GENERATE                 = 105,
    MACCHIATO_ERROR_SW_ECDH_DERIVE                   = 106,
    MACCHIATO_ERROR_PRNG_DATA                        = 107,
    MACCHIATO_ERROR_POINT_NOT_IN_CURVE               = 108,
    MACCHIATO_ERROR_NOT_ALIGNMENT                    = 109,

    /* macchiato_cert.c */
    MACCHIATO_ERROR_PARSING_CERTIFICATE              = 120,
    MACCHIATO_ERROR_DEVICE_NONCE_MISMATCH            = 121,
    MACCHIATO_ERROR_CERTIFICATE_PUBKEY               = 122,
    MACCHIATO_ERROR_SYSTIME                          = 123,
    MACCHIATO_ERROR_CERTIFICATE_EXPIRED              = 124,
    MACCHIATO_ERROR_CERTIFICATE_ID                   = 125,
	
    /* macchiato_keygen.c */
    MACCHIATO_CRYPTO_DEINIT                          = 130,


    /* THE LAST ERROR CODE */
    MACCHIATO_ERR_SIZE                               = 0x7FFFFFFF
 } macchiato_err_t;

#define MACCHIATO_SERVER_NONCE_LEN                16 /*128 bits*/
#define MACCHIATO_DEVICE_NONCE_LEN                16 /*128 bits*/ 

/* This struct represents the Macchiato server nonce message. */
struct macchiato_challenge_msg{
	uint8_t serverNonce[MACCHIATO_SERVER_NONCE_LEN];
};


/* generic buffer parameter */
typedef struct {
	uint8_t* pu8Buffer;
	uint32_t u32Len;
} buffer_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
  @brief qsee_macc_sign_service_data
  Use this function to sign service data with the device Machhiato private key.
  
  @param[in]    serviceData               - buffer that contains the service data to be signed.
  @param[in/out]   signedServiceData         - buffer (also allocated by the calling app) to fill the signature output in the following format:
	                                                                         <----------------------------------------------------------------- ECC signature covers this area -------------------------------------------------------------------->
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	|      32 bits     | 32 bits |   32 bits  |   32 bits  |   512 bits    |          512 bits         |           512 bits           | 256 bits |   32 bits   | 32 bits each |  16 bits   |    32 bits    |  128 bits | 128 bits | variable length |
	|------------------|---------|------------|------------|---------------|---------------------------|------------------------------|----------|-------------|--------------|------------|---------------|-----------|----------|-----------------|
	| Protocol Version | MSG ID  | MSG length | Error Code | ECC Signature | Device Public Signing Key | Device Public Encryption Key | TA Hash  | Num Svc IDs |    Svc IDs   | HW Version | Serial Number | Challenge | Response |   Opaque Data   |
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
												  
	Note: The byte order is LITTLE ENDIAN !!! (except the keys and the signature)
	Svc IDs are the identifiers of the calling QSEE application.
	All the buffers should be allocated by the caller.
	The Nonce field is zeroed.
												  
  @param[out]  pu32SignedServiceDataLen  - the length of the signature output in bytes.
  
  @return
  MACCHIATO_SUCCESS – success
  MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
  MACCHIATO_TOO_SHORT_OUT_BUFFER_ERR - the signature output overflows the given buffer (signedServiceData), the required buffer size is returned in pu32SignedServiceDataLen
  MACCHIATO_FAILED_GET_SECURE_STATE - failed to get the error code for the message
  MACCHIATO_FAILED_SIGN_DATA - failed signing the data using Macchiato private key
  

  @dependencies
  None.

  @sideeffects
  None.
*/
macchiato_err_t  qsee_macc_sign_service_data
(
    buffer_t serviceData, 
    buffer_t signedServiceData, 
    uint32_t*  pu32SignedServiceDataLen
);


/**
  @brief qsee_macc_authenticate_device
  Use this function to authenticate the device using challenge response with Machhiato private key.

  This function also outputs the device id and Macchiato public key to enable signature verification.
  
  @param[in]   serviceNonce                  - buffer that contains the authentication challenge.

  @param[in]   opaqueData                   - buffer that contains additional opaque data to be signed (optional).
  @param[in/out]  signedChallengeResponse   - buffer (also allocated by the calling app) to fill the challenge response output in the following format:
                                                                            <----------------------------------------------------------------- ECC signature covers this area -------------------------------------------------------------------->
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	|      32 bits     | 32 bits |   32 bits  |   32 bits  |   2048 bits   |          2048 bits        |           2048 bits          | 256 bits |   32 bits   | 32 bits each |  16 bits   |    32 bits    |  128 bits | 128 bits | variable length |
	|------------------|---------|------------|------------|---------------|---------------------------|------------------------------|----------|-------------|--------------|------------|---------------|-----------|----------|-----------------|
	| Protocol Version | MSG ID  | MSG length | Error Code | RSA Signature | Device Public Signing Key | Device Public Encryption Key | TA Hash  | Num Svc IDs |    Svc IDs   | HW Version | Serial Number | Challenge | Response |   Opaque Data   |
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
										  
	Note: The byte order is LITTLE ENDIAN !!!(except the keys and the signature)
	Svc IDs are the identifiers of the calling QSEE application.
	All the buffers should be allocated by the caller.
  
  @param[out]  pu32SignedChallengeResponseLen  - the length of the challenge response output in bytes.
  
  @return
  MACCHIATO_SUCCESS – success
  MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
  MACCHIATO_TOO_SHORT_OUT_BUFFER_ERR - the signature output overflows the given buffer (signedChallengeResponse), the required buffer size is returned in pu32SignedChallengeResponseLen
  MACCHIATO_FAILED_GET_SECURE_STATE - failed to get the error code for the message
  MACCHIATO_FAILED_SIGN_DATA - failed signing the data using Macchiato private key
  MACCHIATO_ALLOCATION_ERROR - failed to allocate helper buffer
  MACCHIATO_FAILED_GENERATE_RANDOM_RESPONSE - failed to generate random response
  @dependencies
  None.

  @sideeffects
  None.
*/
macchiato_err_t  qsee_macc_authenticate_device
(
    buffer_t       serviceNonce, 
    buffer_t       deviceNonce, 
    buffer_t       opaqueData, 
    buffer_t       signedChallengeResponse, 
    uint32_t*      pu32SignedChallengeResponseLen
);

/**
 @brief qsee_macc_provision_service_key
 Use this function to get the unwrapped service key from the Macchiato provisioning message.

 @param[in]   provisionMsg          - buffer that contains the provisioning message of the following format:
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	   <------------------------ RSA signature covers this area ------------------------>
 	 --------------------------------------------------------------------------------------------------------------------------------------------------------
	|      32 bits     | 32 bits |   32 bits  |   32 bits  |   2048 bits   |  variable length | 128 bits |          2048 bits         |    variable length   |
	|------------------|---------|------------|------------|---------------|------------------|----------|----------------------------|----------------------|
	| Protocol Version | MSG ID  | MSG length | Error Code | RSA Signature | KPSP Certificate | Response | ECC Encrypted Wrapping Key | Wrapped Service Key  |
	 --------------------------------------------------------------------------------------------------------------------------------------------------------


 @param[in/out]   unwrappedKey - A buffer to contain the unwrapped service key, the maximum size should be known to the calling application.
 @param[out]      pu32UnwarppedKeyLen
 @param[in]       deviceNonce - deviceNonce passsed in to compare with the nonce embedded in provision message.

 @return
 MACCHIATO_SUCCESS – success
 MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
 @dependencies
 None.

 @sideeffects
 None.
*/
macchiato_err_t  qsee_macc_provision_service_key
(
    buffer_t       provisionMsg,
    buffer_t       unwrappedKey,
    uint32_t*      pu32UnwarppedKeyLen,
    buffer_t       deviceNonce
);

typedef enum
{
    MACCHIATO_PUBLIC_KEY_TYPE_INVALID = 0,
    MACCHIATO_PUBLIC_KEY_TYPE_SIGNING_KEY = 1,
    MACCHIATO_PUBLIC_KEY_TYPE_ENCRYPT_KEY = 2,
    MACCHIATO_PUBLIC_KEY_TYPE_UNKNOWN = 0xFFFFFFFF
} public_key_type_t;

/**
 @brief qsee_macc_get_ecc_public_key_point
 Use this function to get public key (signing or encryption).
 Currently obtaining encryption public key is not supported.

 @param[in]        PubKeyType:  Key type signing or encrypt
 @param[out]      PublicKeyData
 @param[out]      PublicKeyDataLen - Length of the data returned

 @return
 MACCHIATO_SUCCESS – success
 MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
 @dependencies
 None.

 @sideeffects
 None.
*/
macchiato_err_t qsee_macc_get_ecc_public_key_point
(
    public_key_type_t  PubKeyType,
    buffer_t           PublicKeyData,
    uint32_t*          PublicKeyDataLen
);


#endif //QSEE_MACCHIATO_H

