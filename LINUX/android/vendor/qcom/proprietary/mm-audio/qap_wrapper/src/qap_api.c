/*============================================================================
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

#define LOG_TAG "qap_api"
//#define LOG_NDEBUG 0
#define LOG_NDDEBUG 0

#include <cutils/list.h>
#include <dlfcn.h>
#include <utils/Log.h>
#include <stdlib.h>
#include <stdbool.h>

#include "qap_api.h"
#include "qap_module.h"
#include "audio_ip_handler.h"
#define MS12_LIB_NAME "libdolby_ms12_wrapper.so"

typedef struct {
    void *lib_handle;
    void *ip_handle;
    void *audio_session_handle;
} qap_wrapper_lib_handle_t;

typedef struct {
    qap_session_t session_type;
    audio_module_lib_interface_t *audio_module;
    qap_session_handle_t session_handle;
    qap_wrapper_lib_handle_t *wrapper_lib_handle;
} qap_wrapper_session_t;

typedef struct {
    qap_module_handle_t module_handle;
    qap_wrapper_session_t *session_handle;
} qap_module_t;

uint32_t qap_get_version()
{
    return (uint32_t) QAP_MODULE_API_VERSION_CURRENT;
}

qap_lib_handle_t qap_load_library(const char* lib_name)
{
    qap_wrapper_lib_handle_t *qap_wrapper_hdl = NULL;
    int ret = 0;

    if (!lib_name) {
        ALOGE("%s:: Libname is null!", __func__);
        goto exit;
    }

    qap_wrapper_hdl = (qap_wrapper_lib_handle_t *) calloc(1, sizeof(qap_wrapper_lib_handle_t));
    if (qap_wrapper_hdl == NULL) {
        goto exit;
    }

    if (strstr(lib_name, MS12_LIB_NAME)) {
        ret = audio_ip_hdlr_init(&qap_wrapper_hdl->ip_handle, (char *)lib_name,
                                 &qap_wrapper_hdl->lib_handle);
        if (ret < 0) {
            ALOGE("%s: audio_ip_hdlr_init failed ret = %d", __func__, ret);
            goto exit;
        }
    } else {
        qap_wrapper_hdl->lib_handle = dlopen(lib_name, RTLD_NOW);
        if (!qap_wrapper_hdl->lib_handle) {
            const char *err_str = dlerror();
            ALOGE("%s:: DLOPEN failed for %s, %s", __func__,
                      lib_name, err_str?err_str:"unknown");
            goto exit;
        }
    }
    return (qap_lib_handle_t)qap_wrapper_hdl;

exit:
    free(qap_wrapper_hdl);
    qap_wrapper_hdl == NULL;
    return (qap_lib_handle_t)qap_wrapper_hdl;
}

qap_status_t qap_unload_library(qap_lib_handle_t lib_handle)
{
    int ret = 0;
    qap_wrapper_lib_handle_t *qap_wrapper_hdl;

    if (!lib_handle) {
        ALOGE("%s:: Lib handle is null!", __func__);
        return QAP_STATUS_ERR;
    }
    qap_wrapper_hdl = (qap_wrapper_lib_handle_t *)lib_handle;

    if (qap_wrapper_hdl->ip_handle)
        ret = audio_ip_hdlr_deinit(qap_wrapper_hdl->ip_handle);
    else
        ret = dlclose(qap_wrapper_hdl->lib_handle);

    free(qap_wrapper_hdl);

    return ((ret == 0)? QAP_STATUS_OK: QAP_STATUS_ERR);
}

qap_session_handle_t qap_session_open(qap_session_t session,
                                      qap_lib_handle_t lib_handle)
{
    qap_wrapper_session_t *qap_session_handle;
    const char *sym_str = AUDIO_MODULE_LIBRARY_INFO_SYM_AS_STR;
    qap_wrapper_lib_handle_t *qap_wrapper_hdl;
    int ret = 0;

    if (!lib_handle) {
        ALOGE("%s:: lib handle is null", __func__);
        goto exit;
    }
    qap_wrapper_hdl = (qap_wrapper_lib_handle_t *)lib_handle;
    qap_session_handle = (qap_wrapper_session_t *) calloc(1, sizeof(qap_wrapper_session_t));
    if (qap_session_handle == NULL) {
        ALOGE("%s:: Memory allocation failed for qap_wrapper_session_t", __func__);
        goto exit;
    }
    qap_session_handle->wrapper_lib_handle = qap_wrapper_hdl;
    qap_session_handle->audio_module = dlsym(qap_wrapper_hdl->lib_handle, sym_str);
    if (qap_session_handle->audio_module == NULL) {
        ALOGE("%s:: unable to find the symbol %s", __func__, sym_str);
        free(qap_session_handle);
        qap_session_handle = NULL;
        goto exit;
    }

    qap_session_handle->session_handle =
                       qap_session_handle->audio_module->audio_session_open(session,
                                                                  &qap_wrapper_hdl->audio_session_handle);

    if (qap_wrapper_hdl->ip_handle) {
        ret = audio_ip_hdlr_open(qap_wrapper_hdl->ip_handle, false, qap_wrapper_hdl->audio_session_handle);
        if (ret < 0) {
            ALOGE("%s: audio_ip_hdlr_open failed ret = %d", __func__, ret);
            free(qap_session_handle);
            qap_session_handle = NULL;
        }
    }

exit:
    return (qap_session_handle_t) qap_session_handle;
}

qap_status_t qap_session_close(qap_session_handle_t session)
{
    qap_wrapper_session_t *session_handle;
    qap_wrapper_lib_handle_t *qap_wrapper_hdl;

    if (!session) {
        ALOGE("%s:: session handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    session_handle = (qap_wrapper_session_t *) session;
    qap_wrapper_hdl = session_handle->wrapper_lib_handle;
    if (qap_wrapper_hdl->ip_handle)
        audio_ip_hdlr_close(qap_wrapper_hdl->ip_handle);

    session_handle->audio_module->audio_session_close(
                                          session_handle->session_handle);
    free(session_handle);

    return QAP_STATUS_OK;
}

qap_status_t qap_session_cmd(qap_session_handle_t session_handle,
                            qap_session_cmd_t cmd,
                            uint32_t cmd_size,
                            void *cmd_data,
                            uint32_t *reply_size,
                            void *reply_data)
{
    qap_wrapper_session_t *session = NULL;
    int *param_data = (int *) cmd_data;
    uint32_t param_id = 0;
    uint32_t data = 0;

    if (!session_handle) {
        ALOGE("%s:: session handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    session = (qap_wrapper_session_t *) session_handle;

    if ((cmd == QAP_SESSION_CMD_SET_PARAM) && param_data != NULL) {
        param_id = (uint32_t) *param_data;
        param_data++;
        ALOGV("%s param = %d", __func__, param_id);
    }

    return session->audio_module->audio_session_cmd(session->session_handle,
                                               cmd,
                                               param_id,
                                               cmd_size,
                                               param_data,
                                               data,
                                               reply_size,
                                               reply_data);
}

qap_status_t qap_session_set_callback(qap_session_handle_t session_handle,
                                      qap_callback_t cb)
{
    qap_wrapper_session_t *session = NULL;

    if (!session_handle) {
        ALOGE("%s:: session handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    session = (qap_wrapper_session_t *) session_handle;

    return session->audio_module->audio_session_set_callback(
                                                  session->session_handle, cb);
}

qap_status_t qap_module_init(qap_session_handle_t session_handle,
                            qap_module_config_t *cfg,
                            qap_module_handle_t *handle)
{
    qap_module_t *qap_mod_handle = NULL;
    qap_wrapper_session_t *session = NULL;
    qap_status_t status;

    if (!session_handle) {
        ALOGE("%s:: session handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    session = (qap_wrapper_session_t *) session_handle;
    qap_mod_handle = (qap_module_t *) malloc(sizeof(qap_module_t));
    if (qap_mod_handle == NULL) {
        return QAP_STATUS_ERR;
    }

    status = session->audio_module->audio_module_init(
                                        session->session_handle,
                                        cfg,
                                        &qap_mod_handle->module_handle);

    if (status != QAP_STATUS_OK) {
        free(qap_mod_handle);
        return status;
    }

    qap_mod_handle->session_handle = session_handle;
    *handle = (qap_module_handle_t *) qap_mod_handle;
    return status;
}

int qap_module_process(qap_module_handle_t module_handle,
                               qap_audio_buffer_t *buffer)
{
    qap_module_t *qap_handle;
    qap_wrapper_session_t *session = NULL;

    if (!module_handle) {
        ALOGE("%s:: module handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    qap_handle = (qap_module_t *) module_handle;
    session = (qap_wrapper_session_t *) qap_handle->session_handle;

    if (buffer == NULL) {
        ALOGE("%s:: buffer is null", __func__);
        return QAP_STATUS_ERR;
    }

    return session->audio_module->audio_module_process(
                                      qap_handle->module_handle, buffer);
}

qap_status_t qap_module_cmd(qap_module_handle_t module_handle,
                            qap_module_cmd_t cmd,
                            uint32_t cmd_size,
                            void *cmd_data,
                            uint32_t *reply_size,
                            void *reply_data)
{
    qap_module_t *qap_mod_handle;
    qap_wrapper_session_t *session = NULL;
    uint32_t param_id = 0;
    uint32_t data = 0;

    if (!module_handle) {
        ALOGE("%s:: module handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    qap_mod_handle = (qap_module_t *) module_handle;
    session = (qap_wrapper_session_t *) qap_mod_handle->session_handle;

    return session->audio_module->audio_module_cmd(
                                      qap_mod_handle->module_handle, cmd,
                                      param_id, cmd_size, cmd_data, data,
                                      reply_size, reply_data);
}

qap_status_t qap_module_set_callback(qap_module_handle_t module_handle __unused,
                                    qap_callback_t cb __unused)
{
    return QAP_STATUS_OK;
}

qap_status_t qap_module_deinit(qap_module_handle_t module_handle __unused)
{
    qap_module_t *qap_mod_handle;
    qap_wrapper_session_t *session = NULL;

    if (!module_handle) {
        ALOGE("%s module handle is null", __func__);
        return QAP_STATUS_ERR;
    }

    qap_mod_handle = (qap_module_t *) module_handle;
    session = (qap_wrapper_session_t *) qap_mod_handle->session_handle;

    return session->audio_module->audio_module_deinit(
                                      qap_mod_handle->module_handle);
}
#if 0
qap_module_descriptor_t* qap_module_get_descriptor(qap_module_type_t module __unused)
{
    return NULL;
}
#endif
