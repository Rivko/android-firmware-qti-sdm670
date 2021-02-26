/**
@file biometric.h
@brief Common definitions for biometric authentication services
*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

when         who      what, where, why
--------     ---      ------------------------------------
Apr/12/2015  el       Initial version.
===========================================================================*/
#ifndef BIOMETRIC_H
#define BIOMETRIC_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum BIO_ERROR_CODE_
{
	BIO_NO_ERROR,
	BIO_NO_RESULT,
	BIO_INVALID_INPUT,
	BIO_INTERNAL_ERROR,
	BIO_LIB_VERSION_MISMATCH,
	BIO_TIMEOUT_EXPIRATION,
	BIO_FILE_ACCESS_ERROR,

}BIO_ERROR_CODE;

 
/**
 * This structure is used sometimes as an input parameter and sometimes as output parameter.
 * When it is used as an input parameter, the usage is trivial.
 * When used as output parameter (i.e. used to read data) the 'data' field should point to
 * a buffer allocated by the user and the size of this buffer is to be specified in the 'size' field.
 * IMPORTANT: ==> When the function copies data into the buffer successfully, it alters the 'size' value,
 * to specify the amount of bytes that were actually copied into the buffer.
*/
typedef struct bio_buffer_
{
	uint8_t* data;
	uint32_t size;

} bio_buffer;

 
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


#endif // BIOMETRIC_H
