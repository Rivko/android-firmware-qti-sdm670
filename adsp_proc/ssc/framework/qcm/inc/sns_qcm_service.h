#pragma once
/*=============================================================================
  @file sns_qcm_service.h

  Incoming requests to the Framework will arrive via the QSocket Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  The module
  will translate incoming QSocket  messages into Client Request objects,
  and similarly will translate Sensor Events into QSocket messages.

  This file handles the QSocket-specific service handling.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

struct sns_qcm_service;
struct sns_qcm_conn;

/*=============================================================================
  Service Callback API
  ===========================================================================*/

/**
 * A new client has arrived.
 *
 * @param[i] conn Connection/client handle used in sns_qcm_service_send
 * @param[i] service_cookie Callback data from sns_qcm_service_init
 * @param[o] connection_handle Will be returned in other functions
 *
 * @return
 * SNS_RC_SUCCESS
 * SNS_RC_POLICY - Maximum number of clients reached; OOM
 */
typedef sns_rc (*sns_qcm_service_connect)(struct sns_qcm_conn *conn,
    void *service_cookie, void **connection_handle);

/**
 * An existing client has disconnected.
 *
 * @param[i] connection_handle As registered from sns_qcm_service_connect
 * @param[i] service_cookie As registered in sns_qcm_service_init
 */
typedef void (*sns_qcm_service_disconnect)(void *connection_handle,
    void *service_cookie);

/**
 * A new request from an existing client has arrived.
 *
 * @param[i] connection_handle As registered from sns_qcm_service_connect
 * @param[i] req_handle UNUSED
 * @param[i] req Encoded request message of type sns_client_request_msg
 * @param[i] req_len Length (in bytes) of req
 * @param[i] service_cookie As registered in sns_qcm_service_init
 *
 * @return
 * SNS_RC_SUCCESS
 * SNS_RC_INVALID_TYPE - Invalid SUID and/or message ID
 * SNS_RC_POLICY - OOM
 * SNS_RC_FAILED - Message not properly encoded
 */
typedef sns_rc (*sns_qcm_service_req)(void *connection_handle,
    void *req_handle, void *req, uint32_t req_len, void *service_cookie);

/**
 * A client connection is now available again to send events.  Will be called
 * at some point after SNS_RC_NOT_AVAILABLE is returned from
 * sns_qcm_service_send.
 *
 * @param[i] connection_handle As registered from sns_qcm_service_connect
 * @param[i] service_cookie As registered in sns_qcm_service_init
 */
typedef void (*sns_qcm_service_resume)(void *connection_handle,
    void *service_cookie);

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Initialize all data structures and threads needed for CM service.
 *
 * @param[o] service Service object to initialize
 * @param[i] service_cookie Callback data returned in callback functions
 * @param[i] connect
 * @param[i] disconnect
 * @param[i] request
 * @param[i] resume
 * @param[i] sensor QCM Sensor handle; used only for Printf
 */
void sns_qcm_service_init(struct sns_qcm_service **service,
    void *service_cookie, sns_qcm_service_connect connect,
    sns_qcm_service_disconnect disconnect, sns_qcm_service_req request,
    sns_qcm_service_resume resume, struct sns_sensor *sensor);

/**
 * Send a message to a client.
 *
 * @param[i] conn On which connection to send the message
 * @param[i] event Event message to be sent
 *
 * @return
 * SNS_RC_SUCCESS
 * SNS_RC_NOT_AVAILABLE - Connection is busy, wait for callback
 * SNS_RC_INVALID_STATE - Connection has been closed; disconnect pending
 * SNS_RC_INVALID_VALUE - Unable to encode event
 * SNS_RC_POLICY - Exceeded maximum indication message size
 * SNS_RC_FAILED - Temporarily OOM, please try again
 */
sns_rc sns_qcm_service_send(struct sns_qcm_conn *conn,
    sns_client_event_msg const *event);

/**
 * Inform the QCM Service that now would be an optimal time to verify the
 * continued presence of all existing clients.
 *
 * @note This function must be called in big Image.
 */
void sns_qcm_service_check(struct sns_qcm_service *service);

/**
 * Query for the maximum indication length.
 *
 * PEND: This logic needs to be re-thought, as it's a bit clunky.  We need
 * it because QCM doesn't know how many events it has space to encode.  The
 * proper "fix" is probably in NanoPB itself.
 *
 * @param[i] conn The client connection whose limit to access
 */
uint32_t sns_qcm_service_max_ind_len(struct sns_qcm_conn *conn);
