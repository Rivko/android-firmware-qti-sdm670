/*=============================================================================
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef _QSEE_SPCOM_H_
#define _QSEE_SPCOM_H_

#include <comdef.h>	/* uint32_t */
#ifndef bool
	#include <stdbool.h>		/* bool */
#endif

/**
 * @brief - Secure Processor Communication API
 *
 * The API is based on Client/Server model.
 * The API resemble the trustzone QSEECOM API.
 * Currently expecting the Secure Processor side to have servers and the HLOS
 * side to have clients. Request is initiated by the client and responded by the
 * server.
 * This API should be used by Android (HLOS) User Space Application.
 * Message/Response are expected to be synchronous.
 */

/*===========================================================================*/
/*                           defines, enums , types                          */
/*===========================================================================*/

/* Maximum number of logical channels supported */
#define SPCOM_MAX_CHANNELS     20

/* Maximum size (including null) for channel names */
#define SPCOM_CHANNEL_NAME_SIZE		32

/**
 * Request buffer size.
 * Any large data (multiply of 4KB) is provided by temp buffer in DDR.
 * Request shall provide the temp buffer physical address (align to 4KB).
 * Maximum request/response size of 268 is used to accommodate APDU size.
 * From kernel spcom driver perspective a PAGE_SIZE of 4K
 * is the actual maximum size for a single read/write file operation.
 */
#define SPCOM_MAX_REQUEST_SIZE		268
#define SPCOM_MAX_RESPONSE_SIZE		268

/**
 * Abstract spcom handle.
 * The actual struct definition is internal to the spcom driver.
 */
struct spcom_client; /* Forward declaration */

/**
 * Client registration info
 *
 * @ch_name:	glink logical channel name
 * @notify_ssr_cb: callback when the remote SP side reset (power down).
 *      This is likely to happen due to remote subsystem restart (SSR).
 *      NULL callback means no notification required.
 *      Upon ssr callback, the user should unregister,
 *      Poll for link up and then register again.
 */
struct spcom_client_info {
	const char *ch_name;
};

/*===========================================================================*/
/*                           RESET                                           */
/*===========================================================================*/

/* NOTE: RESET API NOT SUPPORTED YET BY SPSS SIDE */

/**
 * spcom_reset_sp_subsystem() - send reset command to secure processor.
 *
 * Gracefully ask the remote SP to reset itself.
 * SP will probably initiate a Watch-Dog-Bite.
 *
 * return: 0 on success, negative error code on failure.
 */
int qsee_spcom_reset_sp_subsystem(void);

/*===========================================================================*/
/*                        IS LINK UP                                         */
/*===========================================================================*/

/**
 * spcom_is_sp_subsystem_link_up() - check if SPSS link is up.
 *
 * return: true if link is up, false if link is down.
 */
bool qsee_spcom_is_sp_subsystem_link_up(void);

/*===========================================================================*/
/*                           Client Send Message                             */
/*===========================================================================*/
/**
 * spcom_register_client() - register client for channel
 *
 * Only one client/Server can register on each side of a channel.
 * Server on remote side is expected to be running and connected,
 * therefore connection expected within the provided timeout.
 * Handle is returned even if timeout expired.
 * use spcom_client_is_server_connected() to check fully connected.
 *
 * @info:	Client configuration info (input).
 *
 * return: client handle on success, NULL on failure.
 */
struct spcom_client *qsee_spcom_register_client(struct spcom_client_info *info);

/**
 * spcom_unregister_client() - unregister client for channel
 *
 * @client:	Client Handle.
 *
 * return: 0 on success, negative error code on failure (see errno.h)
 */
int qsee_spcom_unregister_client(struct spcom_client *client);

/**
 * spcom_client_send_message_sync() - Send a synchronous request and response
 *
 * @client:	a pointer to spcom client
 * @req_ptr:	a pointer to the request C struct representation
 * @req_size:	size of the request C struct
 * @resp_ptr:	a pointer to the response C struct representation
 * @resp_size:  size of the response C struct
 * @timeout_msec: Timeout in msec between command and response, 0=no timeout.
 *
 * return: num of rx bytes on success, negative error code on failure (see errno.h)
 */
int qsee_spcom_client_send_message_sync(struct spcom_client	*client,
				   void			*req_ptr,
				   uint32_t		req_size,
				   void			*resp_ptr,
				   uint32_t		resp_size,
				   uint32_t		timeout_msec);

/**
 * spcom_client_is_server_connected() - Check if remote server connected.
 *
 * This API checks that the logical channel is fully connected between
 * the client and the server.
 * Normally, the server should be up first and connect first.
 *
 * @client:	a pointer to spcom client
 *
 * return: true if server connected, false otherwise.
 */
bool qsee_spcom_client_is_server_connected(struct spcom_client *client);

#endif /* _QSEE_SPCOM_H_ */
