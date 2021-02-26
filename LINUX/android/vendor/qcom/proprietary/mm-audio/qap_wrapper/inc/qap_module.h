/*============================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#ifndef AUDIO_MODULE_H
#define AUDIO_MODULE_H

#include <errno.h>
#include <stdint.h>
#include <strings.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <cutils/bitops.h>
#include <qap_defs.h>

__BEGIN_DECLS

/* Name of the hal_module_info */
#define AUDIO_MODULE_LIBRARY_INFO_SYM         ACLI

/* Name of the hal_module_info as a string */
#define AUDIO_MODULE_LIBRARY_INFO_SYM_AS_STR  "ACLI"

#define AUDIO_MODULE_MAKE_API_VERSION(maj,min) \
            ((((maj) & 0xff) << 8) | ((min) & 0xff))

/* Module library interface version 1.0
   number can only be used for fully backwards compatible changes */
#define AUDIO_MODULE_LIBRARY_API_VERSION_1_0 AUDIO_MODULE_MAKE_API_VERSION(1,0)

#define AUDIO_MODULE_LIBRARY_TAG ((('A') << 24) | (('C') << 16) | (('L') << 8) | ('T'))

typedef struct audio_module_lib_interface_s {
    uint32_t tag;             /* tag must be initialized to AUDIO_MODULE_LIBRARY_TAG */
    uint32_t version;         /* Version of the module library API : 0xMMMMmmmm MMMM: Major, mmmm: minor */
    const char *name;         /* Name of this library */
    const char *implementor;  /* Author/owner/implementor of the library */

qap_session_handle_t (*audio_session_open)(qap_session_t session, void **audio_session_handle);

qap_status_t (*audio_session_close)(qap_session_handle_t session);

qap_status_t (*audio_session_cmd)(qap_session_handle_t session_handle,
                                  qap_session_cmd_t cmd,
                                  uint32_t param_id,
                                  uint32_t cmd_size,
                                  void *cmd_data,
                                  uint32_t data,
                                  uint32_t *reply_size,
                                  void *reply_data);

qap_status_t (*audio_session_set_callback)(qap_session_handle_t session_handle,
                                           qap_callback_t cb);

qap_status_t (*audio_module_init)(qap_session_handle_t session_handle,
                                 qap_module_config_t *cfg,
                                 qap_module_handle_t *handle);

int (*audio_module_process)(qap_module_handle_t module_handle,
                                    qap_audio_buffer_t *buffer);

qap_status_t (*audio_module_cmd)(qap_module_handle_t module_handle,
                                 qap_module_cmd_t cmd,
                                 uint32_t param_id,
                                 uint32_t cmd_size,
                                 void *cmd_data,
                                 uint32_t data,
                                 uint32_t *reply_size,
                                 void *reply_data);

qap_status_t (*audio_module_deinit)(qap_module_handle_t module_handle);

} audio_module_lib_interface_t;

__END_DECLS

#endif //AUDIO_MODULE_H
