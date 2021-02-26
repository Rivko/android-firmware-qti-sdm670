#ifndef QSEE_MESSAGE_API_H
#define QSEE_MESSAGE_API_H

/**
@file qsee_message.h
@brief Provide APIs for sending messages between secure applications.
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/02/13   nbeier      Initial version.

===========================================================================*/
#include <stdint.h>

/*---------------------------------------------------------------------------
 *  Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief
 *    API for a secure application to prepare a message that can be sent to
 *    another secure application. This API writes the encryped message into
 *    the supplied output buffer, prepended with a header and appended with
 *    a MAC. This output buffer can be given to the receiving app and then
 *    passed into qsee_decapsulate_inter_app_message to be authenticated and
 *    decrypted.  The actual data exchange (passing the encrypted bufer)
 *    between the secure applications can be done between their non secure
 *    clients running in the HLOS.  Both apps must remain loaded in the
 *    secure execution environment until all encapsulated messages are
 *    decapsulated.  If either the sending or receiving app is unloaded during
 *    this time, the already encapsulated messages will no longer be valid.
 *
 * @param[in] dest_app_name
 * @param[in] in_buf
 * @param[in] in_len
 * @param[out] out_buf
 * @param[in, out] out_len
 *
 * @requires
 *   in_len should indicate the exact size of in_buf.
 *   out_len should indicate the exact size of out_buf, and this should be
 *     greater than in_len by 144 bytes to allow room for the header and MAC.
 *     out_len is modified to reflect the exact length of the data
 *     written into out_buf.
 *
 * @return 0 indicates sucess, all other values indicate failure and
 *     correspond to a specific error code.
 */
int qsee_encapsulate_inter_app_message(char* dest_app_name, uint8_t* in_buf,
  uint32_t in_len, uint8_t* out_buf, uint32_t* out_len);


/**
 * @brief
 *    API for a secure application to decapsulate (ie decrypt) a message
 *  from another secure application.  This API authenticates the message,
 *  decrypts the input buffer, and writes the plaintext message into the
 *  supplied output buffer.  The input buffer must have been prepared by
 *  qsee_encapsulate_inter_app_message(), containing a header and MAC.
 *  Both apps must remain loaded in the secure execution environment until
 *  all encapsulated messages are decapsulated.  If either the sending or
 *  receiving app is unloaded during this time, the already encapsulated
 *  messages will no longer be valid.
 *
 * @param[out] source_app_name
 * @param[in] in_buf
 * @param[in] in_len
 * @param[out] out_buf
 * @param[in, out] out_len
 *
 * @requires
 *   source_app_name is modified to return the sending application's identity
 *      to the caller.
 *   in_len should indicate the exact size of in_buf.
 *   out_len should indicate the exact size of out_buf, and this should be
 *      greater than or equal to in_len.
 *
 * @return 0 indicates sucess, all other values indicate failure and
 *     correspond to a specific error code.
 */
int qsee_decapsulate_inter_app_message(char* source_app_name, uint8_t* in_buf,
  uint32_t in_len, uint8_t* out_buf, uint32_t* out_len);



/*----------------------------------------------------------------------------
 * This needs to been defined here in the first place
 * -------------------------------------------------------------------------*/
#define QSEE_MESSAGE_APP_NAME_MAX_LEN 128


/*----------------------------------------------------------------------------
 * Error codes
 * -------------------------------------------------------------------------*/
#define QSEE_MESSAGE_SUCESS                      0
#define QSEE_MESSAGE_ERROR                       0xFF000FFF
#define QSEE_MESSAGE_ERROR_ZERO_BUFFER_LEN       0xFF000FFE
#define QSEE_MESSAGE_ERROR_INSUFFCIENT_OUT_LEN   0xFF000FFD
#define QSEE_MESSAGE_ERROR_BAD_DEST_APPNAME      0xFF000FFC     // Deprecated
#define QSEE_MESSAGE_ERROR_NO_DEST_APP_THREAD    0xFF000FFB     // Deprecated
#define QSEE_MESSAGE_ERROR_NO_DEST_APP_KEYS      0xFF000FFA
#define QSEE_MESSAGE_ERROR_NO_SPACE_IN_TABLE     0xFF000FF9
#define QSEE_MESSAGE_ERROR_CIPHER_INIT           0xFF000FF8
#define QSEE_MESSAGE_ERROR_CIPHER_SET_PARAM      0xFF000FF7
#define QSEE_MESSAGE_ERROR_CIPHER_ENCRYPT        0xFF000FF6
#define QSEE_MESSAGE_ERROR_HMAC_FAIL             0xFF000FF5
#define QSEE_MESSAGE_ERROR_PRNG_FAIL             0xFF000FF4
#define QSEE_MESSAGE_ERROR_BAD_SOURCE_NAME       0xFF000FF3     // Deprecated
#define QSEE_MESSAGE_ERROR_MESSAGE_NOT_FOR_YOU   0xFF000FF2
#define QSEE_MESSAGE_ERROR_NO_COUNTER_VALS       0xFF000FF1
#define QSEE_MESSAGE_ERROR_MAC_MISMATCH          0xFF000FF0
#define QSEE_MESSAGE_ERROR_BAD_COUNTER_VALS      0xFF000FEF
#define QSEE_MESSAGE_ERROR_MAXED_OUT_COUNTER_VAL 0xFF000FEE
#define QSEE_MESSAGE_ERROR_MALLOC_INTERNAL_BUF   0xFF000FED
#define QSEE_MESSAGE_ERROR_INTEGER_OVERFLOW      0xFF000FEC
#define QSEE_MESSAGE_ERROR_INTEGER_UNDERFLOW     0xFF000FEB
#define QSEE_MESSAGE_ERROR_MALLOC_HMAC_BUF       0xFF000FEA
#define QSEE_MESSAGE_ERROR_STRLCPY_FAIL          0xFF000FE9     // Deprecated

#endif
