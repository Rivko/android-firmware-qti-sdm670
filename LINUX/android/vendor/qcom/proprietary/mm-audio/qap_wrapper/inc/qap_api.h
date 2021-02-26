/*============================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#ifndef QAP_API_H
#define QAP_API_H

#include <errno.h>
#include <stdint.h>
#include <strings.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <cutils/bitops.h>
#include "qap_defs.h"

__BEGIN_DECLS
/*
 * Helper macros for module implementors.
 *
 * The derived modules should provide convenience macros for supported
 * versions so that implementations can explicitly specify module
 * versions at definition time.
 */

#define QAP_MODULE_MAKE_API_VERSION(maj,min) \
            ((((maj) & 0xff) << 8) | ((min) & 0xff))

/* First generation of Modules API had version hardcoded to 1.
 * All devices with versions < 2.0 will be considered of first generation API.
 */
#define QAP_MODULE_API_VERSION_1_0 QAP_MODULE_MAKE_API_VERSION(1, 0)

// Minimum Module API version that Module libraries must implement
#define QAP_MODULE_API_VERSION_MIN QAP_MODULE_API_VERSION_1_0

// Current version of Module APIs
#define QAP_MODULE_API_VERSION_CURRENT QAP_MODULE_API_VERSION_1_0

typedef void (*qap_callback_t)(qap_session_handle_t session_handle,
                              void* priv_data,
                              qap_callback_event_t event_id,
                              int size,
                              void* data);

/**************************************/
/* Module library specific APIs        */
/**************************************/

/*returns current Module API version */
uint32_t qap_get_version();

/* convenience API for opening and closing Module API library */
qap_lib_handle_t qap_load_library(const char* lib_name);

qap_status_t qap_unload_library(qap_lib_handle_t lib_handle);

/**************************************/
/* Session specific APIs              */
/**************************************/

/*
 * This method creates and opens the session.
 * Input:
 *     session - Input session type bases on which the decoder
 *               graph is created
 *     lib_handle - Library Handle (need this to create graph)
 * Output:
 *     qap_session_handle_t - handle to the session opened
 */
qap_session_handle_t qap_session_open(qap_session_t session,
                                     qap_lib_handle_t lib_handle);

/*
 * This method closes the session.
 * Input:
 *     session - Session handle of the session to be closed
 * Output:
 *     qap_status_t - returns success or failure.
 */
qap_status_t qap_session_close(qap_session_handle_t session);

/*
 * This method is used to set the session specific parameters
 * Input:
 *     session_handle - Session handle
 *     cmd - command id
 *     cmd_size - size of the command
 *     cmd_data - payload of the command
 *                first element in the payload is the param id
 *                followed by the data.
 *     reply_size - size of the reply
 *     reply_data - reply from the module for the command
 * Output:
 *     qap_status_t - returns success or failure.
 *                first element in the payload is the param id
 *                followed by the data.
 */
qap_status_t qap_session_cmd(qap_session_handle_t session_handle,
                                           qap_session_cmd_t cmd,
                                               uint32_t cmd_size,
                                                  void *cmd_data,
                                            uint32_t *reply_size,
                                                void *reply_data);

/*
 * Set the callback function for the session.
 * Input:
 *     session_handle - Session handle to which callback has to be set
 *     cb - callback function pointer
 * Output:
 *     qap_status_t - returns success or failure.
 */
qap_status_t qap_session_set_callback(qap_session_handle_t session_handle,
                                      qap_callback_t cb);

/**************************************/
/* Module specific APIs                */
/**************************************/

/*
 * Module Init function.
 * Input:
 *     session_handle - Session handle
 *     cfg - module config params
 *     handle - module handle
 * Output:
 *     qap_status_t - returns success or failure.
 */
qap_status_t qap_module_init(qap_session_handle_t session_handle,
                            qap_module_config_t *cfg,
                            qap_module_handle_t *handle);

/*
 * Process command for input buffer.
 * Input:
 *     module_handle - module handle
 *     buffer - input/output buffer pointer
 * Output:
 *     qap_status_t - returns success or failure.
 */
int qap_module_process(qap_module_handle_t module_handle,
                               qap_audio_buffer_t *buffer);

/*
 * Set module specific commands.
 * Input:
 *     module_handle - module handle
 *     cmd - command id
 *     cmd_size - size of the command
 *     cmd_data - payload of the command
 *                first element in the payload is the param id
 *                followed by the data.
 *     reply_size - size of the reply
 *     reply_data - reply from the module for the command
 * Output:
 *     qap_status_t - returns success or failure.
 *                first element in the payload is the param id
 *                followed by the data.
 */
qap_status_t qap_module_cmd(qap_module_handle_t module_handle,
                                         qap_module_cmd_t cmd,
                                            uint32_t cmd_size,
                                               void *cmd_data,
                                         uint32_t *reply_size,
                                            void *reply_data);

/*
 * Set the callback function for the module.
 * Input:
 *     module_handle - Module handle to which callback has to be set
 *     cb - callback function pointer
 * Output:
 *     qap_status_t - returns success or failure.
 */
qap_status_t qap_module_set_callback(qap_module_handle_t module_handle,
                                    qap_callback_t cb);

/*
 * Module de init function.
 * Input:
 *     module_handle - Module handle.
 * Output:
 *     qap_status_t - returns success or failure.
 */
qap_status_t qap_module_deinit(qap_module_handle_t module_handle);

/*
 * Get the module descriptor for current session.
 * Input:
 *     module - module id
 * Output:
 *    qap_module_descriptor_t - This holds the capabilities of the
 *              module.
 */
//qap_module_descriptor_t* qap_module_get_descriptor(qap_module_type_t module);

__END_DECLS

#endif //QAP_API_H
