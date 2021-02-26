/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "generic_effect"
#define LOG_NDDEBUG 0
/*#define LOG_NDEBUG 0*/

#include <qti_gef_api.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <cutils/log.h>
#include <cutils/trace.h>
#include <cutils/str_parms.h>
#include <cutils/properties.h>
#include <cutils/list.h>

#define XSTR(x) STR(x)
#define STR(x) #x

//ACDB fn pointers
typedef int (*audio_extn_gef_send_audio_cal_t)(void*, int,
    int, int, int, int, uint32_t, uint32_t, void*, int, bool);

typedef int (*audio_extn_gef_get_audio_cal_t)(void*, int,
    int, int, int, int, uint32_t, uint32_t, void*, int*, bool);

typedef int (*audio_extn_gef_store_audio_cal_t)(void*, int,
    int, int, int, int, uint32_t, uint32_t, void*, int);

typedef int (*audio_extn_gef_retrieve_audio_cal_t)(void*, int,
    int, int, int, int, uint32_t, uint32_t, void*, int*);

static uint64_t TRACE_TAG = ATRACE_TAG_NEVER;


typedef struct effect_private_data {
    struct listnode list;
    effect_uuid_t   uuid;
    bool            enable;
    gef_func_ptr    cb;
    void*           data;             //data given by effect
    void*           gef_private_data; //pointer to gef handle
} effect_private_data;

typedef struct gef_private_data {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int             num_effects;

    //hal handles and APIs
    void*           hal_handle;
    audio_extn_gef_send_audio_cal_t     send_cal;
    audio_extn_gef_get_audio_cal_t      get_cal;
    audio_extn_gef_store_audio_cal_t    store_cal;
    audio_extn_gef_retrieve_audio_cal_t retrieve_cal;

    //list of all effects
    struct listnode effect_list;
    void*           hal_data;
    int             debug_enable;
    bool            debug_func_entry;
    bool            is_gef_initialized;
    bool            is_hal_initialized;
} gef_private_data;


static gef_private_data gef_global_handle;

static pthread_once_t gef_lib_once = PTHREAD_ONCE_INIT;

static void check_and_enable_debug_logs(struct gef_private_data *my_data)
{
    if (!my_data)
        return;

    char val[PROPERTY_VALUE_MAX] = {0};
    if (property_get("vendor.audio.gef.debug.flags", val, "") &&
        !strncmp(val, "1", 1)) {
        my_data->debug_enable = 1;
        my_data->debug_func_entry = true;
    }
}

static void check_and_enable_traces()
{
    if (property_get_bool("vendor.audio.gef.enable.traces", false)) {
        TRACE_TAG = ATRACE_TAG_AUDIO;
    }
}

static void gef_lib_init()
{
    gef_private_data* my_data = &gef_global_handle;
    list_init(&my_data->effect_list);
    //initialize debug and tracing
    check_and_enable_debug_logs(my_data);
    check_and_enable_traces();

    pthread_mutex_init(&my_data->mutex, (const pthread_mutexattr_t *) NULL);
    pthread_cond_init(&my_data->cond, (const pthread_condattr_t *) NULL);

    my_data->num_effects = 0;
    my_data->is_gef_initialized = false;
    my_data->is_hal_initialized = false;
    my_data->hal_data = NULL;
}

static void gef_lib_init_once()
{
    pthread_once(&gef_lib_once, gef_lib_init);
}

static int try_and_init_hal_handles(gef_private_data* handle)
{
    int ret = 0;
    const char* error;
    char hal_lib_name[256];

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if (!handle) {
       ret = -EINVAL;
       goto ERROR_RETURN;
    }

#ifdef LINUX_ENABLED
    snprintf(hal_lib_name, sizeof(hal_lib_name),
        "/usr/lib/audio.primary.default.so");
#else
    snprintf(hal_lib_name, sizeof(hal_lib_name),
        "/vendor/lib/hw/audio.primary.%s.so",
        XSTR(GEF_PLATFORM_NAME));
#endif
    ret = access(hal_lib_name, R_OK);
    if (ret == 0) {
        //: check error for dlopen
        handle->hal_handle = dlopen(hal_lib_name, RTLD_LAZY);
        if (handle->hal_handle == NULL) {
            ALOGE("%s: DLOPEN failed for %s", __func__, hal_lib_name);
            ret = -EINVAL;
            goto ERROR_RETURN;
        } else {
             ALOGD_IF(gef_global_handle.debug_func_entry, "%s: DLOPEN successful"
                "for %s", __func__, hal_lib_name);

             //call dlerror to clear the current error
             error = dlerror();
             handle->send_cal = (audio_extn_gef_send_audio_cal_t)dlsym(handle->hal_handle,
                 "audio_extn_gef_send_audio_cal");
             //check error again
             error = dlerror();
             //dlsym can return NULL http://linux.die.net/man/3/dlerror
             //because of which it is expected that dlerror return has to be checked
             if (error != NULL) {
                  ALOGE("%s: dlsym failed for %s with error %s", __func__,
                      "audio_extn_gef_send_audio_cal", error);
                  ret = -EINVAL;
                  goto ERROR_RETURN;
             }

             //call dlerror to clear the current error
             error = dlerror();
             handle->get_cal = (audio_extn_gef_get_audio_cal_t)dlsym(handle->hal_handle,
                 "audio_extn_gef_get_audio_cal");
             //check error again
             error = dlerror();
             //dlsym can return NULL http://linux.die.net/man/3/dlerror
             //because of which it is expected that dlerror return has to be checked
             if (error != NULL) {
                  ALOGE("%s: dlsym failed for %s with error %s", __func__,
                      "audio_extn_gef_get_audio_cal", error);
                  ret = -EINVAL;
                  goto ERROR_RETURN;
             }

             //call dlerror to clear the current error
             error = dlerror();
             handle->store_cal = (audio_extn_gef_store_audio_cal_t)dlsym(handle->hal_handle,
                 "audio_extn_gef_store_audio_cal");
             //check error again
             error = dlerror();
             //dlsym can return NULL http://linux.die.net/man/3/dlerror
             //because of which it is expected that dlerror return has to be checked
             if (error != NULL) {
                  ALOGE("%s: dlsym failed for %s with error %s", __func__,
                      "audio_extn_gef_store_audio_cal", error);
                  ret = -EINVAL;
                  goto ERROR_RETURN;
             }

             //call dlerror to clear the current error
             error = dlerror();
             handle->retrieve_cal = (audio_extn_gef_retrieve_audio_cal_t)dlsym(handle->hal_handle,
                 "audio_extn_gef_retrieve_audio_cal");
             //check error again
             error = dlerror();
             //dlsym can return NULL http://linux.die.net/man/3/dlerror
             //because of which it is expected that dlerror return has to be checked
             if (error != NULL) {
                  ALOGE("%s: dlsym failed for %s with error %s", __func__,
                      "audio_extn_gef_retrieve_audio_cal", error);
                  ret = -EINVAL;
                  goto ERROR_RETURN;
             }
             handle->is_hal_initialized = true;
        }
    } else {
        ALOGE("opening %s failed with error %s", hal_lib_name, strerror(ret));
    }

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    if (ret) {
        if (handle && handle->hal_handle) {
            dlclose(handle->hal_handle);
            handle->hal_handle = NULL;
        }
    }

    return ret;
}

__attribute__ ((visibility ("default")))
gef_handle_t* gef_register_session(effect_uuid_t effectId)
{
    gef_private_data* my_data = &gef_global_handle;
    effect_private_data* effect_data = (effect_private_data*) NULL;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: enter with (%d, %d, %d)",
        __func__, effectId.timeLow, effectId.timeMid,
        effectId.timeHiAndVersion);

    gef_lib_init_once();
    pthread_mutex_lock(&my_data->mutex);

    if (!my_data->is_gef_initialized) {
        //allocate HAL handle
        try_and_init_hal_handles(my_data);
    }

    //store effect data
    //have to create dynamic memory and store in a list
    effect_data = (effect_private_data*)calloc(1, sizeof(effect_private_data));
    if (!effect_data) {
        goto ERROR_RETURN;
    }

    effect_data->uuid = effectId;
    effect_data->enable = false;
    effect_data->cb = (gef_func_ptr) NULL;
    effect_data->data = (void*) NULL;
    effect_data->gef_private_data = (void*)my_data;

    my_data->num_effects++;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "number of registered effects %d", my_data->num_effects);

    //add this to the list of nodes
    list_add_tail(&gef_global_handle.effect_list, &effect_data->list);

ERROR_RETURN:
    pthread_cond_signal(&my_data->cond);
    pthread_mutex_unlock(&my_data->mutex);

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with effect_handle %p", __func__, effect_data);
    return (gef_handle_t*)effect_data;
}

__attribute__ ((visibility ("default")))
int gef_deregister_session(gef_handle_t* handle)

{
    int ret = 0;
    effect_private_data* effect_data = (effect_private_data*)handle;
    gef_private_data* gp_data = (gef_private_data*) NULL;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if (!effect_data|| !effect_data->gef_private_data) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    gp_data = (gef_private_data*)effect_data->gef_private_data;
    if (!gp_data || gp_data != &gef_global_handle) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    pthread_mutex_lock(&gp_data->mutex);

    //remove the node from the list
    gp_data->num_effects--;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "number of registered effects %d", gp_data->num_effects);
    list_remove(&effect_data->list);
    free(effect_data);

    pthread_cond_signal(&gp_data->cond);
    pthread_mutex_unlock(&gp_data->mutex);

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

__attribute__ ((visibility ("default")))
int gef_enable_effect(gef_handle_t* handle)
{
    effect_private_data* effect_data = (effect_private_data*)handle;
    effect_private_data* fx_ctxt;
    event_type type = EFFECT_ENABLED;
    event_value value = {0, EFFECT_UUID_NULL_, 0, 0, 0, 0};
    struct listnode *node;
    int ret = 0;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if(!effect_data || !effect_data->gef_private_data ||
       effect_data->gef_private_data != &gef_global_handle) {
           ret = -EINVAL;
           goto ERROR_RETURN;
    }

    effect_data->enable = true;

    // notify enablement event to effect clients
    value.value = effect_data->enable;
    memcpy(&value.effect_uuid, &effect_data->uuid, sizeof(effect_uuid_t));

    list_for_each(node, &gef_global_handle.effect_list) {
        fx_ctxt = node_to_item(node,
                effect_private_data, list);
        if((fx_ctxt != effect_data) && fx_ctxt->cb) {
            fx_ctxt->cb(fx_ctxt, type, value);
        }
    }

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

__attribute__ ((visibility ("default")))
int gef_disable_effect(gef_handle_t* handle)
{
    effect_private_data* effect_data = (effect_private_data*)handle;
    effect_private_data* fx_ctxt;
    event_type type = EFFECT_ENABLED;
    event_value value = {0, EFFECT_UUID_NULL_, 0, 0, 0, 0};
    struct listnode *node;
    int ret = 0;

    ALOGD_IF(gef_global_handle.debug_func_entry,
         "%s: Enter", __func__);

    if(!effect_data || !effect_data->gef_private_data ||
       effect_data->gef_private_data != &gef_global_handle) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    effect_data->enable = false;

    // notify enablement event to effect clients
    value.value = effect_data->enable;
    memcpy(&value.effect_uuid, &effect_data->uuid, sizeof(effect_uuid_t));

    list_for_each(node, &gef_global_handle.effect_list) {
        fx_ctxt = node_to_item(node,
                effect_private_data, list);
        if((fx_ctxt != effect_data) && fx_ctxt->cb) {
            fx_ctxt->cb(fx_ctxt, type, value);
        }
    }

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

__attribute__ ((visibility ("default")))
int gef_query_version(int *majorVersion, int *minorVersion)
{

    int ret = 0;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if (!majorVersion || !minorVersion) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    *majorVersion = 0;
    *minorVersion = 1;

ERROR_RETURN:

    ALOGD_IF(gef_global_handle.debug_func_entry,
            "%s: Exit with error %d", __func__, ret);
    return ret;
}

/*
 * This function is to be called by vendor abstraction layer to GEF to
 * register a callback. This callback will be invoked by GEF
 * under these circumstances
 *
 * 1. Error when sending a calibration
 * 2. Another effect which was also registered through GEF got enabled
 * 3. A device is connected to notify which device is connected
 *   and the channel map associated
 *
 * handle: - Handle to GEF
 * cb:- pointer to callback
 * data:- data that will be sent in the callback
 *
 * returns:- 0: Operation is successful
 *      EINVAL: When the session is invalid/handle is null etc
 */
__attribute__ ((visibility ("default")))
int gef_register_callback(gef_handle_t* handle, gef_func_ptr cb, void* data)
{

    int ret = 0;
    effect_private_data* effect_data = (effect_private_data*)handle;
    gef_private_data* gp_data;
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if(!effect_data|| !effect_data->gef_private_data) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    gp_data = (gef_private_data*)effect_data->gef_private_data;

    if (!gp_data || gp_data != &gef_global_handle) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    pthread_mutex_lock(&gp_data->mutex);

    effect_data->cb = cb;
    effect_data->data = data;

    pthread_cond_signal(&gp_data->cond);
    pthread_mutex_unlock(&gp_data->mutex);

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

/*
 * This method sends the parameters that will be sent to the DSP
 * The parameters are automatically stored in ACDB cache.
 *
 * handle: - Handle to GEF
 * params:- Parameters of the effect
 * data:- data corresponding to the effect
 *
 * returns:- 0: Operation is successful
 *       EINVAL: When the session is invalid/handle is null etc
 *       ENODEV: When GEF has not been initialized
 *       ENOSYS: Sending parameters is not supported
 */
int gef_set_param_v1(gef_handle_t* handle,
        effect_config_params* params,
        effect_data_in* data)

{
    int ret = 0;
    effect_private_data* effect_data = (effect_private_data*)handle;
    gef_private_data* gp_data = (gef_private_data*)NULL;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if((!effect_data)||(!params)||(!data)) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    gp_data = (gef_private_data*)effect_data->gef_private_data;
    if (!gp_data || gp_data != &gef_global_handle) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    ret = -ENOSYS;

    if (gp_data->send_cal && gp_data->hal_data)
        ret = gp_data->send_cal(gp_data->hal_data, params->device_id,
            params->cal_type, params->app_type, params->topo_id,
            params->sample_rate, params->module_id, params->param_id,
            data->data, data->length, params->persist);

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}


/*
 * This api will retrieve the entire data binary from the DSP
 * for the module and parameter ids mentioned.
 *
 * Vendor abstraction layer is expected to allocate memory for
 * the buffer into which the data corresponding to the parameters
 * will be stored
 *
 * THIS API IS NOT SUPPORTED FOR NOW.
 * API will always return ENOSYS
 *
 * handle: - Handle to GEF
 * params:- Parameters of the effect
 * data:- data corresponding to the effect
 *
 * returns:- ENOSYS: Retrieving parameters is not supported
 */
int gef_get_param_v1(const gef_handle_t* handle,
        effect_config_params* params,
        effect_data_out* data)
{
    int ret = 0;
    effect_private_data* effect_data = (effect_private_data*)handle;
    gef_private_data* gp_data = (gef_private_data*)NULL;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if((!effect_data)||(!params)||(!data)) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    gp_data = (gef_private_data*)effect_data->gef_private_data;
    if (!gp_data || gp_data != &gef_global_handle) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    ret = -ENOSYS;

    if (gp_data->get_cal && gp_data->hal_data)
        ret = gp_data->get_cal(gp_data->hal_data, params->device_id,
            params->cal_type, params->app_type, params->topo_id,
            params->sample_rate, params->module_id, params->param_id,
            data->data, data->length, params->persist);

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

/*
 * This method sends the parameters that are to be stored in ACDB.
 *
 * handle: - Handle to GEF
 * params:- Parameters of the effect
 * data:- data corresponding to the effect
 *
 * returns:- 0: Operation is successful
 *      EINVAL: When the session is invalid/handle is null etc
 *      ENODEV: When GEF has not been initialized
 *      ENOSYS: Sending parameters is not supported
 */
int gef_store_cacheparam_v1(gef_handle_t* handle,
        effect_config_params* params,
        effect_data_in* data)
{
    int ret = 0;
    effect_private_data* effect_data = (effect_private_data*)handle;
    gef_private_data* gp_data = (gef_private_data*)NULL;

    if((!effect_data)||(!params)||(!data)) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    gp_data = (gef_private_data*)effect_data->gef_private_data;
    if (!gp_data || gp_data != &gef_global_handle) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    if (gp_data->store_cal && gp_data->hal_data)
        ret = gp_data->store_cal(gp_data->hal_data, params->device_id,
                params->cal_type, params->app_type, params->topo_id,
                params->sample_rate, params->module_id, params->param_id,
                data->data, data->length);

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

/*
 * This method retrieves the parameters that are stored in cache.
 * Vendor abstraction layer is expected to allocate memory for
 * the buffer into which the data corresponding to the parameters
 * will be stored
 *
 * THIS API IS NOT SUPPORTED FOR NOW
 * API will always return ENOSYS
 *
 * handle: - Handle to GEF
 * params:- Parameters of the effect
 * data:- data corresponding to the effect
 *
 * returns:- ENOSYS: Retrieving parameters is not supported
 */
int gef_retrieve_cacheparam_v1(const gef_handle_t* handle,
        effect_config_params* params,
        effect_data_out* data)
{

    int ret = 0;
    effect_private_data* effect_data = (effect_private_data*)handle;

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter", __func__);

    if((!effect_data)||(!params)||(!data)) {
        ret = -EINVAL;
        goto ERROR_RETURN;
    }

    ret = -ENOSYS;

ERROR_RETURN:
    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit with error %d", __func__, ret);
    return ret;
}

void* gef_init(void *data)
{
    gef_lib_init_once();

    ALOGD_IF(gef_global_handle.debug_func_entry, "%s: Enter", __func__);
    gef_private_data* my_data = &gef_global_handle;
    pthread_mutex_lock(&my_data->mutex);
    my_data->is_hal_initialized = false;
    my_data->hal_data = data;
    pthread_mutex_unlock(&my_data->mutex);

    return ((void*)my_data);
}

void gef_deinit(void *data)
{
    gef_private_data* handle = (gef_private_data*) data;
    ALOGD_IF(gef_global_handle.debug_func_entry, "%s: Enter", __func__);

    if (handle != NULL && handle == &gef_global_handle) {
        pthread_mutex_lock(&handle->mutex);
        if (handle->is_hal_initialized && handle->hal_handle) {
            dlclose(handle->hal_handle);
            handle->hal_handle = NULL;
        }
        handle->is_hal_initialized = false;
        handle->hal_data = NULL;
        pthread_mutex_unlock(&handle->mutex);
    }
}

void gef_device_config_cb(void* handle, audio_devices_t device,
        audio_channel_mask_t cmask, int sample_rate, int acdb_dev_id)
{
    struct listnode *node;
    gef_private_data* my_data = handle;
    event_type type = DEVICE_CONNECTED;
    event_value value =
    {
        .value        = 0,
        .effect_uuid  = EFFECT_UUID_NULL_,
        .device       = device,
        .channel_mask = cmask,
        .sample_rate  = sample_rate,
        .acdb_dev_id  = acdb_dev_id,
    };

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Enter with (device:%d, mask:%d, acdb_dev_id:%d)", __func__,
        device, cmask, acdb_dev_id);
    if (!my_data || my_data != &gef_global_handle)
    {
        return;
    }

    pthread_mutex_lock(&my_data->mutex);

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Number of callbacks: %d)", __func__, my_data->num_effects);

    if(!my_data->num_effects)
    {
        //no enabled effects, just return
        goto DONE;
    }

    //iterate through effect in list and post callback
    list_for_each(node, &gef_global_handle.effect_list) {
        effect_private_data *effect_data = node_to_item(node,
                effect_private_data, list);
        if(effect_data->cb) {
            memcpy(&value.effect_uuid, &effect_data->uuid, sizeof(effect_uuid_t));
            effect_data->cb(effect_data, type, value);
        }
    }

DONE:
    pthread_cond_signal(&my_data->cond);
    pthread_mutex_unlock(&my_data->mutex);

    ALOGD_IF(gef_global_handle.debug_func_entry,
        "%s: Exit", __func__);
}
