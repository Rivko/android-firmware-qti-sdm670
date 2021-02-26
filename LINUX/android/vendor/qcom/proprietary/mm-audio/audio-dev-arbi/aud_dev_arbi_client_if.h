#ifndef _AUD_DEV_ARBI_CLIENT_IF_H_
#define _AUD_DEV_ARBI_CLIENT_IF_H_

/*============================================================================
                           aud_dev_arbi_client_if.h

DESCRIPTION:  This file contains the declarations for the audio device
              arbitration client-side interface.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

#include <hardware/audio.h>

/**
  Client handle
*/
typedef void* aud_dev_arbi_client_handle_t;

/**
  Audio device arbitration events for client side
*/
typedef enum {
    AUD_DEV_ARBI_EVENT_DEVICE_REQUESTED = 0,
    AUD_DEV_ARBI_EVENT_DEVICE_RELEASED,
    AUD_DEV_ARBI_NUM_EVENTS
} aud_dev_arbi_event_t;

/**
  Audio device arbitration clients
*/
typedef enum {
    AUD_DEV_ARBI_EVENT_CLIENT_UAL = 0,
    AUD_DEV_ARBI_NUM_CLIENTS
} aud_dev_arbi_client_t;

/**
  response flags returned from event callback by client
*/
typedef enum {
    AUD_DEV_ARBI_EVENT_RESP_DEFAULT = 0,
    AUD_DEV_ARBI_EVENT_RESP_REACQUIRE = 1
} aud_dev_arbi_event_cb_resp_flags_t;

/**
  Client callback function for audio device arbitration events
*/
typedef void (*aud_dev_arbi_event_cb_t)(audio_devices_t, aud_dev_arbi_event_t,
                                        aud_dev_arbi_event_cb_resp_flags_t*);

/**
  Client callback with result from processed server command
 */
typedef void (*aud_dev_arbi_client_cmd_result_cb_t)(
         audio_devices_t, bool, uint32_t);

/**
* Initializes an audio device arbitration client.
*
* @param outHandle[out]
*           client handle which is allocated by this call.
* @param evtCb[in]
*           callback function for client events.
*
* @return 0 on success, negative value on failure
*/
extern "C" int aud_dev_arbi_client_init
(
    aud_dev_arbi_client_handle_t *outHandle,
    aud_dev_arbi_event_cb_t evtCb
);

/**
* De-initializes an audio device arbitration client.
*
* @param handle[in]
*           client handle which to be de-initialized. After this
*           call, the handle is no longer valid.
*
* @return 0 on success, negative value on failure
*/
extern "C" int aud_dev_arbi_client_deinit
(
    aud_dev_arbi_client_handle_t handle
);

/**
* Registers for a specific audio device. This call is required
* before the device can be acquired or released.
*
* @param handle[in]
*           audio device arbitration client handle
* @param client[in]
*           client identification
* @param audDev[in]
*           audio device to register for
*
* @return 0 on success, negative value on failure
*/
extern "C" int aud_dev_arbi_client_register
(
    aud_dev_arbi_client_handle_t handle,
    aud_dev_arbi_client_t client,
    audio_devices_t audDev
);

/**
* Requests the audio device arbitration server (AHAL) to acquire
* a device for use by the client. If this request succeeds, the
* client is allowed to use the device. Otherwise, it must not
* use the device.
*
* @param handle[in]
*           audio device arbitration client handle
* @param audDev[in]
*           audio device to acquire
* @param resultCb[in]
*           if not 0, will be called with result of the command
*           processing. The callback is serialized with other
*           server callbacks and can be used for reliable
*           book-keeping.
* @param userData[in]
*           passed as-is to command result
*           callback (only relevant if resultCb not 0)
*
* @return 0 if the device was successfully acquired, negative
*         value otherwise. The client must not use the device if
*         the acquisition failed.
*/
extern "C" int aud_dev_arbi_client_acquire
(
    aud_dev_arbi_client_handle_t handle,
    audio_devices_t audDev,
    aud_dev_arbi_client_cmd_result_cb_t resultCb,
    uint32_t userData
);

/**
* Notifies the audio device arbitration server (AHAL) that the
* client no longer uses a device.
*
* @param handle[in]
*           audio device arbitration client handle
* @param audDev[in]
*           audio device to release
* @param resultCb[in]
*           if not 0, will be called with result of the command
*           processing. The callback is serialized with other
*           server callbacks and can be used for reliable
*           book-keeping.
* @param userData[in]
*           passed as-is to command result
*           callback (only relevant if resultCb not 0)
*
* @return 0 if the notification was received successfully by the
*         server, negative value otherwise.
*/
extern "C" int aud_dev_arbi_client_release
(
    aud_dev_arbi_client_handle_t handle,
    audio_devices_t audDev,
    aud_dev_arbi_client_cmd_result_cb_t resultCb,
    uint32_t userData
);

#endif // _AUD_DEV_ARBI_CLIENT_IF_H_
