/* qsthw_api.c
 *
 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "qsthw_api"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <cutils/list.h>
#include <tinyalsa/asoundlib.h>
#include <hardware/sound_trigger.h>

#include "qsthw_api.h"
#include "sound_trigger_prop_intf.h"

/* The current HAL API version */
#define QSTHW_MODULE_API_VERSION_CURRENT QSTHW_MODULE_API_VERSION_1_0

#define QSTHW_MAX_MODULE_ID_LENGTH 100

typedef struct {
    struct listnode list_node;
    sound_trigger_hw_device_t *sthw_device;
    unsigned int ref_cnt;
    char module_id[QSTHW_MAX_MODULE_ID_LENGTH];
    void *sthw_handle;
    sthw_extn_fptrs_t sthw_extn_fptrs;
    sthw_extn_get_fptrs_t sthw_extn_get_fptrs;
    pthread_mutex_t lock;
} qsthw_module_t;

/* count of qti sound trigger wrapper clients */
static unsigned int qsthw_client_cnt = 0;
static pthread_mutex_t qsthw_init_lock = PTHREAD_MUTEX_INITIALIZER;
static struct listnode qsthw_module_list;

static bool is_valid_qsthw_module(qsthw_module_t *qsthw_module)
{
    struct listnode *node, *temp_node;
    qsthw_module_t *module;
    bool is_valid = false;

    if (!qsthw_module) {
        ALOGE("%s: NULL module handle passed", __func__);
        return is_valid;
    }

    list_for_each_safe(node, temp_node, &qsthw_module_list) {
        module = node_to_item(node, qsthw_module_t, list_node);
        if (module == qsthw_module) {
            is_valid = true;
            break;
        }
    }

    return is_valid;
}

/* Called with qsthw_init_lock held */
static qsthw_module_t *get_qsthw_module(const char *module_id)
{
    struct listnode *node, *temp_node;
    qsthw_module_t *module;

    list_for_each_safe(node, temp_node, &qsthw_module_list) {
        module = node_to_item(node, qsthw_module_t, list_node);
        if (!strncmp(module_id, module->module_id, strlen(module_id)))
            return module;
    }
    return NULL;
}

static int qsthw_close(const qsthw_module_handle_t *mod_handle)
{
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    int status = 0;

    ALOGD("%s: count=%d", __func__, qsthw_client_cnt);
    pthread_mutex_lock(&qsthw_init_lock);

    if (!qsthw_module) {
        ALOGE("%s: ERROR module handle passed NULL", __func__);
        pthread_mutex_unlock(&qsthw_init_lock);
        return -EINVAL;
    }

    if (is_valid_qsthw_module(qsthw_module)) {
        pthread_mutex_lock(&qsthw_module->lock);
        if (--qsthw_module->ref_cnt != 0) {
            pthread_mutex_unlock(&qsthw_module->lock);
            goto exit;
        }

        status = sound_trigger_hw_device_close(qsthw_module->sthw_device);
        if (status)
            ALOGE("%s: sound_trigger_hw_device_close() failed, status %d", __func__, status);

        if (qsthw_module->sthw_handle)
           dlclose(qsthw_module->sthw_handle);

        list_remove(&qsthw_module->list_node);
        pthread_mutex_unlock(&qsthw_module->lock);

        pthread_mutex_destroy(&qsthw_module->lock);
        free(qsthw_module);
        qsthw_module = NULL;
    } else {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        status = -EINVAL;
    }

exit:
    qsthw_client_cnt--;
    pthread_mutex_unlock(&qsthw_init_lock);
    ALOGD("%s: Exit device=%p cnt=%d ", __func__,
           qsthw_module, qsthw_client_cnt);
    return status;
}

static qsthw_module_handle_t *qsthw_open(const char* qsthw_module_id)
{
    qsthw_module_handle_t *mod_handle;
    qsthw_module_t *qsthw_module = NULL;
    const hw_module_t *hw_mod;
    char *sthw_prefix = NULL;
    sound_trigger_hw_device_t *sthw_device;
    int status = 0;
    const char *error;

    ALOGD("%s: Enter", __func__);

    if (qsthw_module_id == NULL) {
        ALOGE("%s: ERROR module id passed NULL",__func__);
        goto err_exit;
    }

    if (!strcmp(qsthw_module_id, QSTHW_MODULE_ID_PRIMARY)) {
        sthw_prefix = "primary";
    } else {
        ALOGE("%s: ERROR. Invalid qti module id provided %s",
              __func__, qsthw_module_id);
        goto err_exit;
    }

    pthread_mutex_lock(&qsthw_init_lock);
    if (qsthw_client_cnt != 0) {
        qsthw_module = get_qsthw_module(qsthw_module_id);
        if (qsthw_module) {
            ALOGD("%s: returning existing qsthw module instance, exit", __func__);
            mod_handle = (void *)qsthw_module;
            goto exit;
        }
    }

    qsthw_module = calloc(1, sizeof(qsthw_module_t));
    if (!qsthw_module) {
        ALOGE("%s: ERROR. qsthw module alloc failed", __func__);
        goto err_exit;
    }

    status = hw_get_module_by_class(SOUND_TRIGGER_HARDWARE_MODULE_ID,
                                    sthw_prefix, &hw_mod);
    if (status) {
        ALOGE("%s:hw_get_module_by_class() failed with %d", __func__, status);
        goto err_exit;
    }

    status = sound_trigger_hw_device_open(hw_mod, &sthw_device);

    if (status || !sthw_device) {
        ALOGE("%s: sound_trigger_hw_device_open() failed with %d", __func__, status);
        goto err_exit;
    }

    qsthw_module->sthw_handle = hw_mod->dso;
    /* clear any existing errors */
    dlerror();
    qsthw_module->sthw_extn_get_fptrs =
       (sthw_extn_get_fptrs_t)dlsym(qsthw_module->sthw_handle,
                                             "sthw_extn_get_fptrs");
    if ((error = dlerror()) != NULL) {
        ALOGE("%s: dlsym error %s for sthw_extn_get_fptrs",
               __func__, error);
        goto err_fptr_exit;
    } else {
        qsthw_module->sthw_extn_get_fptrs(&qsthw_module->sthw_extn_fptrs);
    }

    if (!qsthw_client_cnt)
        list_init(&qsthw_module_list);

    pthread_mutex_init(&qsthw_module->lock, (const pthread_mutexattr_t *) NULL);
    qsthw_module->sthw_device = sthw_device;
    strlcpy(qsthw_module->module_id, qsthw_module_id,
            QSTHW_MAX_MODULE_ID_LENGTH);


    mod_handle = (void *)qsthw_module;
    /* Add module to global stdev module list */
    list_add_tail(&qsthw_module_list, &qsthw_module->list_node);

exit:
    qsthw_module->ref_cnt++;
    qsthw_client_cnt++;
    pthread_mutex_unlock(&qsthw_init_lock);
    ALOGD("%s: Exit: module handle %p", __func__, mod_handle);
    return mod_handle;

err_fptr_exit:
    sound_trigger_hw_device_close(qsthw_module->sthw_device);

err_exit:
    if (qsthw_module) {
        if (qsthw_module->sthw_handle)
            dlclose(qsthw_module->sthw_handle);
        free(qsthw_module);
        qsthw_module = NULL;
    }

    pthread_mutex_unlock(&qsthw_init_lock);
    return NULL;
}

int qsthw_get_version() {
    return QSTHW_MODULE_API_VERSION_CURRENT;
}

qsthw_module_handle_t *qsthw_load_module(const char *hw_module_id)
{
    return qsthw_open(hw_module_id);
}

int qsthw_unload_module(const qsthw_module_handle_t *mod_handle)
{
    return qsthw_close(mod_handle);
}

int qsthw_get_properties(const qsthw_module_handle_t *mod_handle,
                         struct sound_trigger_properties *properties)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && sthw_dev->get_properties) {
        ret = sthw_dev->get_properties(sthw_dev, properties);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_load_sound_model(const qsthw_module_handle_t *mod_handle,
                           struct sound_trigger_sound_model *sound_model,
                           qsthw_sound_model_callback_t callback,
                           void *cookie,
                           sound_model_handle_t *handle)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    if (!handle || !sound_model) {
        ALOGE("%s: ERROR NULL arguments passed", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && sthw_dev->load_sound_model) {
        ret = sthw_dev->load_sound_model(sthw_dev, sound_model,
                                         callback, cookie, handle);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_unload_sound_model(const qsthw_module_handle_t *mod_handle,
                             sound_model_handle_t handle)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && sthw_dev->unload_sound_model) {
        ret = sthw_dev->unload_sound_model(sthw_dev, handle);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_start_recognition(const qsthw_module_handle_t *mod_handle,
                            sound_model_handle_t sound_model_handle,
                            const struct sound_trigger_recognition_config *config,
                            qsthw_recognition_callback_t callback,
                            void *cookie)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && sthw_dev->start_recognition) {
        ret = sthw_dev->start_recognition(sthw_dev, sound_model_handle,
                config, callback,
                cookie);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_stop_recognition(const qsthw_module_handle_t *mod_handle,
                           sound_model_handle_t sound_model_handle)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && sthw_dev->stop_recognition) {
        ret = sthw_dev->stop_recognition(sthw_dev, sound_model_handle);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_get_param_data(const qsthw_module_handle_t *mod_handle,
                         sound_model_handle_t sound_model_handle,
                         const char *param,
                         void *payload,
                         size_t payload_size,
                         size_t *param_data_size)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && qsthw_module->sthw_extn_fptrs.get_param_data) {
        ret = qsthw_module->sthw_extn_fptrs.get_param_data(sthw_dev,
                sound_model_handle, param, (void *)payload, payload_size,
                param_data_size);
        if (ret)
            ALOGE("%s: ERROR ret %d", __func__, ret);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_set_parameters(const qsthw_module_handle_t *mod_handle,
                         sound_model_handle_t sound_model_handle,
                         const char *kv_pairs)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && qsthw_module->sthw_extn_fptrs.set_parameters) {
        ret = qsthw_module->sthw_extn_fptrs.set_parameters(sthw_dev,
                sound_model_handle, kv_pairs);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

size_t qsthw_get_buffer_size(const qsthw_module_handle_t *mod_handle,
                             sound_model_handle_t sound_model_handle)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;

    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && qsthw_module->sthw_extn_fptrs.get_buffer_size) {
        ret = qsthw_module->sthw_extn_fptrs.get_buffer_size(sthw_dev,
                sound_model_handle);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}

int qsthw_read_buffer(const qsthw_module_handle_t *mod_handle,
                      sound_model_handle_t sound_model_handle,
                      unsigned char *buf,
                      size_t bytes)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;

    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && qsthw_module->sthw_extn_fptrs.read_buffer) {
        pthread_mutex_unlock(&qsthw_module->lock);
        ret = qsthw_module->sthw_extn_fptrs.read_buffer(sthw_dev,
                sound_model_handle,
                buf, bytes);
    } else {
        pthread_mutex_unlock(&qsthw_module->lock);
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }

exit:
    return ret;
}

int qsthw_stop_buffering(const qsthw_module_handle_t *mod_handle,
                         sound_model_handle_t sound_model_handle)
{
    int ret = 0;
    qsthw_module_t *qsthw_module = (qsthw_module_t *)mod_handle;
    sound_trigger_hw_device_t *sthw_dev;
    bool is_valid;

    pthread_mutex_lock(&qsthw_init_lock);
    is_valid = is_valid_qsthw_module(qsthw_module);
    pthread_mutex_unlock(&qsthw_init_lock);

    if (!is_valid) {
        ALOGE("%s: ERROR Invalid module handle", __func__);
        ret = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&qsthw_module->lock);
    sthw_dev = qsthw_module->sthw_device;
    if (sthw_dev && qsthw_module->sthw_extn_fptrs.stop_buffering) {
        ret = qsthw_module->sthw_extn_fptrs.stop_buffering(sthw_dev,
                sound_model_handle);
    } else {
        ret = -ENOSYS;
        ALOGW("%s not supported", __func__);
    }
    pthread_mutex_unlock(&qsthw_module->lock);

exit:
    return ret;
}
