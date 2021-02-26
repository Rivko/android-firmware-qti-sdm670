/**
@file biometric_result.h
@brief This file include the definitions of the biometric authentication API.
*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

when         who      what, where, why
--------     ---      ------------------------------------
Jun/1/2015  el       Initial version.
===========================================================================*/
#ifndef BIOMETRIC_RESULT_H
#define BIOMETRIC_RESULT_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "biometric.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define BIO_NA 0xFFFFFFFF

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/



typedef enum BIO_AUTH_METHOD_
{
	BIO_FINGERPRINT_MATCHING,
	BIO_IRIS_MATCHING,
	
	BIO_AUTH_METHOD_NUMBER

}BIO_AUTH_METHOD;



typedef struct bio_result_
{
	// which authentication method yielded this result.
	// this is a mandatory field.
	BIO_AUTH_METHOD method;
	
	// true means successful authentication. otherwise, false.
	// this is a mandatory field.
	bool result;

	// the ID of the identified user.
	// This is an optional parameter which may be set to BIO_NA.
	uint64_t user_id;

	// the ID of the entity that was used for authentication.
	// e.g. finger ID in case of BIO_FINGERPRINT_SCAN method.
	// This is an optional parameter which may be set to BIO_NA.
	uint64_t user_entity_id;

	// a transaction ID that binds an authentication request with this result.
	// This is an optional parameter which may be set to BIO_NA.
	uint64_t transaction_id;

} bio_result;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
 * @brief Set an authentication result
 *
 * @param[in] result		  Pointer to a bio_result structure that holds recent authentication result.
 * @param[in] extension_data  Authentication extension data that will be sent back to the client.
 *							  This is an optional parameter and may be set to NULL.
 *
 * @return BIO_NO_ERROR on success. Otherwise, returns the relevant error code.
*/
BIO_ERROR_CODE bio_set_auth_result(const bio_result* result, const bio_buffer* extension_data);
 
 /**
 * @brief Reads the authentication result
 *
 * @param[inout] result			 Pointer to a bio_result structure that will hold recent authentication
 *                               result of the required authentication method.
 *                               The user shall set result->method to the authentication method result 
 *                               he is interested at. rest of the fields will be updated by the function when
 *                               BIO_NO_ERROR is returned.
 * @param[out] 	 uptime_counter	 Number of milliseconds that elapsed between the device has
 *								 been boot up and the authentication response has been sent.
 * @param[inout] extension_data	 Buffer that will be used to hold any extension data that was sent by the
 *								 authenticator. 
 *                               extension_data->data shall point to a pre-allocated user buffer that will
 *                               hold the read extension data, while extension_data->size will indicate the
 *                               available buffer size. Once the function completes successfully, it alters
 *                               extension_data->size to the amount of bytes that were actually copied to user's buffer.
 *                               This is an optional parameter and may be set to NULL.
 *
 * &param[inout] authenticator_ta_name Buffer that will be used to hold the name of the TA that set
 *                                     that authentication result.
 *                                     authenticator_ta_name->data shall point to a pre-allocated user buffer that will
 *                                     hold the name string (32 bytes are typically the max. size),
 *                                     while authenticator_ta_name->size will indicate the available buffer size.
 *                                     Once the function completes successfully, it alters authenticator_ta_name->size
 *                                     to the amount of characters that were actually written to user's buffer,
 *                                     excluding the '\0'.
 *                                     This is an optional parameter and may be set to NULL. However, it is recommended
 *                                     for the client TA to manage a list of trusted authenticators and verify the result
 *                                     was sent by an expected TA.
 *
 * @return BIO_NO_ERROR on success. Otherwise, returns the relevant error code.
*/
BIO_ERROR_CODE bio_get_auth_result(bio_result* result,
                                   uint64_t* uptime_counter,
                                   bio_buffer* extension_data,
								   bio_buffer* authenticator_ta_name);

#endif // BIOMETRIC_RESULT_H
