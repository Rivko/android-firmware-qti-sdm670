#ifndef QSEE_SERVICES_H
#define QSEE_SERVICES_H

#include <stdint.h>

/**
@file qsee_services.h
@brief Provide API wrappers for services
*/

/*============================================================================
  Copyright (c) 2012, 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
  =========================================================================== */

/*=========================================================================
  Edit History

  $Header: //components/rel/ssg.tz/1.0.2/api/securemsm/trustzone/qsee/qsee_services.h#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $


  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  08/04/11    rv     Initial Revision

  =========================================================================== */

/**
 * @brief
 *    API for a secure application to prepare a message that can be sent to
 *    another secure application. This API writes the encryped message into
 *    the supplied output buffer, prepended with a header and appended with
 *    a MAC. This output buffer can be given to the receiving app and then
 *    passed into qsee_decapsulate_inter_app_message to be authenticated and
 *    decrypted.  The actual data exchange (passing the encrypted bufer)
 *    between the secure applications can be done between their non secure
 *    clients running in the HLOS.
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



/**
 * @brief API for secure applications to request service from non-secure listener
 * @param[in] listener_id
 * @param[in] req
 * @param[in] req_len
 * @param[out] rsp
 * @param[in] rsplen
 */
int qsee_request_service(unsigned int listener_id, void *req, unsigned int req_len, void *rsp, unsigned int rsplen);


/**
 * @brief API for secure applications to register shared buffer with QSEE.
 * @param[in] address
 * @param[in] size
 *
 * @return -1 returns error and 0 returns success
 */
int qsee_register_shared_buffer(void *address, unsigned int size);


/**
 * API for secure applications to deregister the shared buffer
 * which has been previously registered with QSEE. -1 returns
 * error and 0 returns success
 *
 * @param[in] address
 *
 * @return -1 returns error and 0 returns success
 */
int qsee_deregister_shared_buffer(void *address);


/**
 * API for prepaing shared buffer before sending it across to
 * non-secure side.
 *
 * @param[in] address
 * @param[in] size Size of region, Note: Deprecated
 *
 * @return -1 returns error and 0 returns success
 */
int qsee_prepare_shared_buf_for_nosecure_read(void * address, unsigned int size);


/**
 * @brief API for preparing the shared buffer sent by non-secure side before
 * secure side reads it.
 *
 * @param[in] address
 * @param[in] size Size of region, NOTE: Deprecated
 *
 * @return -1 returns error and 0 returns success
 */
int qsee_prepare_shared_buf_for_secure_read(void *address, unsigned int size);

/**
 * 64 bytes alignment required on start and size of the buffer
 */
#define QSEE_SHARED_BUFFER_ALIGNMENT 0x40

#endif /*QSEE_SERVICES_H*/
