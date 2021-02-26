#ifndef SECBOOT_ASN1_H_
#define SECBOOT_ASN1_H_

/** @file SecBoot_asn1.h

DESCRIPTION:
  This software module contains the source code for the asn.1 DER parsing
  functions

  Here a format of a simple ASN.1 document

  SEQUENCE {
    name          OCTECT STRING
    version       [0] INTEGER DEFAULT (0)
    valid         BOOLEAN
    storage       Storage
  }

  Storeage ::== SEQUENCE {
    time          UTC TIME
    data          BIT STRING
  }


EXTERNALIZED FUNCTIONS

  secasn1_start()                Starts an ASN.1 document for parsing

  secasn1_end()                  Closes an ASN.1 document

  secasn1_close_sequence()       Closes a sequence in an ASN.1 document

  secasn1_next_field()           Parses the next field in an ASN.1 document

  secasn1_decode_bit_string()    Decodes a bit string from a generic
                                     ASN.1 field

  secasn1_decode_boolean()       Decodes a boolean value from a generic
                                     ASN.1 field

  secasn1_decode_time()          Decode a time value from a generic
                                     ASN.1 field

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c)2010 Qualcomm Technologies Incorporated. All Rights Reserved.
 
   
**/

/*==========================================================================
          EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------


==========================================================================*/


/*========================================================================

                             Include Files

=========================================================================*/
#include "SecBoot_i.h"

/*========================================================================

                       Constants Definitions

=========================================================================*/

#define SECASN1_NO_TYPE_CHECK         (0x00)
#define SECASN1_BOOLEAN_TYPE          (0x01)
#define SECASN1_INTEGER_TYPE          (0x02)
#define SECASN1_BIT_STRING_TYPE       (0x03)
#define SECASN1_OCTET_STRING_TYPE     (0x04)
#define SECASN1_NULL_TYPE             (0x05)
#define SECASN1_OID_TYPE              (0x06)
#define SECASN1_SEQUENCE_TYPE         (0x10)
#define SECASN1_SET_TYPE              (0x11)
#define SECASN1_PRINTABLE_STRING_TYPE (0x13)
#define SECASN1_TELETEX_STRING_TYPE   (0x14)
#define SECASN1_UTC_TYPE              (0x17)

#define SECASN1_MAX_LEN               0xFFFFU  /* allows SECASN1_UNDEFINED_LEN to be legal */ 


/*========================================================================

                       Type Definitions

=========================================================================*/

/* ASN.1 Error Codes */
typedef enum
{
  E_ASN1_SUCCESS = 0,
  E_ASN1_INVALID_TAG,
  E_ASN1_NO_DATA,
  E_ASN1_INVALID_DATA,
  E_ASN1_INVALID_ARG,
  E_ASN1_RESERVED  = 0x7FFFFFFF
} secasn1_err_type;

#define CHECK_ASN1_INVARIANTS(s) \
(((s).len == SECASN1_UNDEFINED_LEN || \
  ((s).len <= SECASN1_MAX_LEN && \
   (s).data + (s).len <= (s).data_bound \
   )) && \
 (s).data <= (s).data_bound)

// Try to combine these types into a common type later on
// diff. authors using diff. types for very similar/identical
// structs.
typedef secasn1_data_type SECSSL_ASN1_INTEGER;
typedef secasn1_data_type SECSSL_ASN1_STRING;
typedef secasn1_data_type SECSSL_ASN1_UTCTIME;


/*--------------------------------------------------------------------------
                Function Declarations
--------------------------------------------------------------------------*/

// Added to complete some missing code
uint32 clk_julian_to_secs(clk_julian_type *jts);

/*===========================================================================

FUNCTION SECASN1_START

DESCRIPTION
  Starts an ASN.1 DER encoding by creating an initial container for the
  next_field() function

DEPENDENCIES
  None

PARAMETERS
  data    - pointer to the data
  data_bound - pointer to after the data (for catching bogus length)
  ret_ptr - pointer to the returned data to start the ASN.1 block

RETURN VALUE
  E_ASN1_SUCCESS - if the sequence is properly started
  E_ASN1_INVALID_ARG - if the pointer arguments have a NULL value
  E_ASN1_INVALID_TAG - if it is not a sequence tag at the beginning
  E_ASN1_INVALID_DATA - if the top level object would extend beyond data_bound

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_start
(
  const uint8 *data,
  const uint8 *data_bound,
  secasn1_data_type *ret_ptr
);

/*===========================================================================

FUNCTION SECASN1_END

DESCRIPTION
  closes and verifies an ASN.1 DER encoding, by checking that no data
  has been left unprocessed at the end of the stream

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the end of the data holder

RETURN VALUE
  E_ASN1_SUCCESS - if the document has been closed successfully
  E_ASN1_INVALID_ARG - if the pointer argument has a NULL value
  E_ASN1_INVALID_DATA - all the data was not processed

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_end
(
  secasn1_data_type *data_ptr
);

/*===========================================================================

FUNCTION SECASN1_CLOSE_SEQUENCE

DESCRIPTION
  This function verifies that all the data in the inner sequence has
  been processed.  The outer sequence is included in the case that
  a sequence has an undefined length so the "0x00, 0x00" end marker
  can be removed.

DEPENDENCIES
  secasn1_next_field() to start a sequence value has been called

PARAMETERS
  outer - pointer to the outer sequence
  inner - pointer to the inner sequence

RETURN VALUE
  E_ASN1_SUCCESS - if the sequence was successful closed
  E_ASN1_INVALID_ARG - if a pointer argument has a NULL value
  E_ASN1_INVALID_DATA - otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_close_sequence
(
  secasn1_data_type *outer,
  secasn1_data_type *inner
);

/*===========================================================================

FUNCTION SECASN1_NEXT_FIELD

DESCRIPTION
  This function parses the next field of data in the ASN.1 DER encoding.
  The function points the secasn1_data_type at the data contained within
  the field.  ret_ptr must be allocated before it is passed into the
  function or the error E_ASN1_INVALID_ARG will be returned.  If
  verify_tag_id != 0, then the function verifies the tag_id against the
  current fields tags.  If the tag_ids do not match an error code of
  E_ASN1_INVALID_TAG is returned.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the data to be parsed
  ret_ptr  - where the inner field data is returned
  verify_tag_id - tag value to check the tag against

RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and ret_ptr will be updated
  to contain the data in the field.  If an error occurs during the parsing
  the appropriate error will be returned.  On an error the data_ptr is
  returned to the initial position

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_next_field
(
  secasn1_data_type *data_ptr,
  secasn1_data_type *ret_ptr,
  uint32             verify_tag_id
);

/*===========================================================================

FUNCTION SECASN1_DECODE_BIT_STRING

DESCRIPTION
  This function takes a data pointer and decodes it to a bit_string pointer
  In the ASN.1 DER encoding, the first byte of a byte string indicates, how
  many unused bytes are at the end of the string.  Assumes that the bit_ptr
  has been pre-allocated.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the bit string field
  bit_ptr  - where the decoded bit string will be returned


RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and bit_ptr will be updated
  to contain the bit string.  If an error occurs during the conversion
  the appropriate error will be returned.

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_decode_bit_string
(
  secasn1_data_type *data_ptr,
  secasn1_bit_string_type *bit_ptr
);

/*===========================================================================

FUNCTION SECASN1_DECODE_BOOLEAN

DESCRIPTION
  This function takes a data pointer and decodes it to a boolean pointer
  In the ASN.1 DER encoding, the boolean data should only be a single byte,
  with 0x00 meaning FALSE and anything else being true

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the boolean field
  b_ptr - pointer where the boolean value will be returned

RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and the boolean pointer will
  will be set.  If the data_ptr contains more than a single byte
  E_ASN1_INVALID_DATA will be returned

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_decode_boolean
(
  secasn1_data_type *data_ptr,
  boolean *b_val
);

/*===========================================================================

FUNCTION SECASN1_DECODE_TIME

DESCRIPTION
  This function takes a data pointer representing time and decodes it to
  the number of seconds since CDMA epoch time of Jan 6th, 1980.  If the time
  occurs before this date, the time is adjusted to 0.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - pointer to the time data field
  time     - pointer where the time in seconds is returned
  utc_time - whether the time is in UTC time format

RETURN VALUE
  If successful, E_ASN1_SUCCESS will be returned and the time pointer will
  will be set.  E_ASN1_INVALID_DATA is returned if an invalid format is
  encountered.

SIDE EFFECTS
  None
===========================================================================*/
extern secasn1_err_type secasn1_decode_time
(
  secasn1_data_type *data_ptr,
  uint32  *time,
  boolean utc_time
);

#endif /* _SECBOOT_ASN1_H */
