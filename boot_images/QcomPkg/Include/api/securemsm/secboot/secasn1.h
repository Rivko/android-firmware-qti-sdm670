#ifndef SECASN1_H
#define SECASN1_H

/** @file secasn1.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and interfaces that provides ASN.1 DER encoding.
 * 
 */

/*=========================================================================
Copyright (c) 2003 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==========================================================================*/
/*==========================================================================
          EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
04/03/06   lx      Add UTF8String support
01/24/03   Jay     Initial version

==========================================================================*/


/*==========================================================================

             Include Files for Module

==========================================================================*/
#include "comdef.h"


/**
@name Alert Mask Definations
The following Alert masks can be returned by SECX509 library
@{
*/

/**
  ASN1 Constants and definitions
*/
#define SECASN1_NO_TYPE_CHECK         (0x00)
#define SECASN1_BOOLEAN_TYPE          (0x01)
#define SECASN1_INTEGER_TYPE          (0x02)
#define SECASN1_BIT_STRING_TYPE       (0x03)
#define SECASN1_OCTET_STRING_TYPE     (0x04)
#define SECASN1_NULL_TYPE             (0x05)
#define SECASN1_OID_TYPE              (0x06)
#define SECASN1_UTF8_STRING_TYPE      (0x0c)
#define SECASN1_SEQUENCE_TYPE         (0x10)
#define SECASN1_SET_TYPE              (0x11)
#define SECASN1_PRINTABLE_STRING_TYPE (0x13)
#define SECASN1_TELETEX_STRING_TYPE   (0x14)
#define SECASN1_UTC_TYPE              (0x17)
/*! 
  @}
*/
/*--------------------------------------------------------------------------
                Modular Constants and definitions
--------------------------------------------------------------------------*/

/**
  ASN.1 Error Codes
*/
typedef enum
{
  E_ASN1_SUCCESS = 0,
  E_ASN1_INVALID_TAG,
  E_ASN1_NO_DATA,
  E_ASN1_INVALID_DATA,
  E_ASN1_INVALID_ARG
} secasn1_err_type;

/**
  ASN.1 data holder
*/
typedef struct
{
  uint8 *data;
  uint16 len;
} secasn1_data_type;

/**
  ASN.1 bit string data holder
*/
typedef struct
{
  uint8 *data;
  uint16 len;
  uint8 unused;
} secasn1_bit_string_type;

/*--------------------------------------------------------------------------
                Function Declarations
--------------------------------------------------------------------------*/
/*!
   @addtogroup secx509_apis
   @{
*/

/*!
  @brief
   Start the ASN.1 DER encoding

  @details
   Starts an ASN.1 DER encoding by creating an initial container for the
   next_field() function

  @dependencies
   None

  @param[in] data         - pointer to the data
  @param[in,out] ret_ptr  - pointer to returned data to start the ASN.1 block

  @return
   E_ASN1_SUCCESS - if the sequence is properly started
   E_ASN1_INVALID_ARG - if the pointer arguments have a NULL value
   E_ASN1_INVALID_TAG - if it is not a sequence tag at the beginning

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_start
(
  uint8             *data,
  secasn1_data_type *ret_ptr
);


/*!
  @brief
   Close and verify the ASN.1 DER encoding

  @details
   Closes and verifies an ASN.1 DER encoding, by checking that no data
   has been left unprocessed at the end of the stream

  @dependencies
   None

  @param[in] data_ptr  - pointer to end of data holder

  @return
   E_ASN1_SUCCESS - if the document has been closed successfully
   E_ASN1_INVALID_ARG - if the pointer argument has a NULL value
   E_ASN1_INVALID_DATA - all the data was not processed

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_end
(
  secasn1_data_type *data_ptr
);

/*!
  @brief
   Close the ASN.1 sequence

  @details
   This function verifies that all the data in the inner sequence has
   been processed.  The outer sequence is included in the case that
   a sequence has an undefined length so the "0x00, 0x00" end marker
   can be removed.

  @dependencies
  secasn1_next_field() to start a sequence value has been called

  @param[in] outer - pointer to the outer sequence
  @param[in] inner - pointer to the inner sequence

  @return
   E_ASN1_SUCCESS - if the sequence was successful closed
   E_ASN1_INVALID_ARG - if a pointer argument has a NULL value
   E_ASN1_INVALID_DATA - otherwise

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_close_sequence
(
  secasn1_data_type *outer,
  secasn1_data_type *inner
);


/*!
  @brief
   Parse the next field in ASN.1 DER encoding

  @details
   This function parses the next field of data in the ASN.1 DER encoding.
   The function points the secasn1_data_type at the data contained within
   the field.  ret_ptr must be allocated before it is passed into the
   function or the error E_ASN1_INVALID_ARG will be returned.  If
   verify_tag_id != 0, then the function verifies the tag_id against the
   current fields tags.  If the tag_ids do not match an error code of
   E_ASN1_INVALID_TAG is returned.

  @dependencies
   None

  @param[in] data_ptr - pointer to the data to be parsed
  @param[in] ret_ptr  - where the inner field data is returned
  @param[in] verify_tag_id - tag value to check the tag against

  @return
   If successful, E_ASN1_SUCCESS will be returned and ret_ptr will be updated
   to contain the data in the field.  If an error occurs during the parsing
   the appropriate error will be returned.  On an error the data_ptr is
   returned to the initial position

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_next_field
(
  secasn1_data_type *data_ptr,
  secasn1_data_type *ret_ptr,
  uint8 verify_tag_id
);

/*!
  @brief
   Decode an ASN.1 data pointer to a bit string pointer

  @details
   This function takes a data pointer and decodes it to a bit_string pointer
   In the ASN.1 DER encoding, the first byte of a byte string indicates, how
   many unused bytes are at the end of the string.  Assumes that the bit_ptr
   has been pre-allocated.

  @dependencies
   None

  @param[in] data_ptr - pointer to the bit string field
  @param[in] bit_ptr  - where the decoded bit string will be returned

  @return
   If successful, E_ASN1_SUCCESS will be returned and bit_ptr will be updated
   to contain the bit string.  If an error occurs during the conversion
   the appropriate error will be returned.

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_decode_bit_string
(
  secasn1_data_type *data_ptr,
  secasn1_bit_string_type *bit_ptr
);

/*!
  @brief
   Decode an ASN.1 data pointer to a boolean pointer

  @details
   This function takes a data pointer and decodes it to a boolean pointer
   In the ASN.1 DER encoding, the boolean data should only be a single byte,
   with 0x00 meaning FALSE and anything else being true

  @dependencies
   None

  @param[in] data_ptr - pointer to the boolean field
  @param[in] b_ptr - pointer where the boolean value will be returned

  @return
   If successful, E_ASN1_SUCCESS will be returned and the boolean pointer will
   will be set.  If the data_ptr contains more than a single byte
   E_ASN1_INVALID_DATA will be returned

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_decode_boolean
(
  secasn1_data_type *data_ptr,
  boolean *b_val
);

/*!
  @brief
   Decode an ASN.1 data pointer to a boolean pointer

  @details
   This function takes a data pointer and decodes it to a boolean pointer
   In the ASN.1 DER encoding, the boolean data should only be a single byte,
   with 0x00 meaning FALSE and anything else being true

  @dependencies
   None

  @param[in] data_ptr - pointer to the boolean field
  @param[in] b_ptr - pointer where the boolean value will be returned

  @return
   If successful, E_ASN1_SUCCESS will be returned and the boolean pointer will
   will be set.  If the data_ptr contains more than a single byte
   E_ASN1_INVALID_DATA will be returned

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_decode_boolean
(
  secasn1_data_type *data_ptr,
  boolean *b_val
);


/*!
  @brief
   Decode a data pointer to number of seconds since epoch time

  @details
   This function takes a data pointer representing time and decodes it to
   the number of seconds since CDMA epoch time of Jan 6th, 1980.  If the time
   occurs before this date, the time is adjusted to 0.

  @dependencies
   None

  @param[in] data_ptr - pointer to the time data field
  @param[in] time     - pointer where the time in seconds is returned
  @param[in] utc_time - whether the time is in UTC time format

  @return
   If successful, E_ASN1_SUCCESS will be returned and the time pointer will
   will be set.  E_ASN1_INVALID_DATA is returned if an invalid format is
   encountered.

  @sideeffects
   None
*/
extern secasn1_err_type secasn1_decode_time
(
  secasn1_data_type *data_ptr,
  uint32  *time,
  boolean utc_time
);

/*!
  @}
*/
#endif /* SECASN1_H */
