/* qsthw_api.h
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <system/audio.h>
#include <system/sound_trigger.h>

#ifndef QTI_ST_HW_API_H
#define QTI_ST_HW_API_H


__BEGIN_DECLS

#define QSTHW_MAKE_API_VERSION(maj,min) \
            ((((maj) & 0xff) << 8) | ((min) & 0xff))

#define QSTHW_MODULE_API_VERSION(maj,min) \
            QSTHW_MAKE_API_VERSION(maj,min)

#define QSTHW_MODULE_API_VERSION_1_0 QSTHW_MODULE_API_VERSION(1, 0)


/*
 * List of known qti sound trigger modules.
 * This is the base name of the qti module that is used to load primary
 * sound trigger HAL.
 */

#define QSTHW_MODULE_ID_PRIMARY "soundtrigger.primary"

typedef void qsthw_module_handle_t;

typedef void (*qsthw_recognition_callback_t)(struct sound_trigger_recognition_event *event,
                                             void *cookie);
typedef void (*qsthw_sound_model_callback_t)(struct sound_trigger_model_event *event, void *cookie);

/*
 * Retrieve implementation properties.
 */
int (qsthw_get_properties)(const qsthw_module_handle_t *mod_handle,
                           struct sound_trigger_properties *properties);

/*
 * Load a sound model. Once loaded, recognition of this model can be started and stopped.
 * Only one active recognition per model at a time.
 * Can handle concurrent recognition requests by different clients on the same model.
 * The implementation returns a unique handle used by other functions (unload_sound_model(),
 * start_recognition(), etc...
 */
int (qsthw_load_sound_model)(const qsthw_module_handle_t *mod_handle,
                             struct sound_trigger_sound_model *sound_model,
                             qsthw_sound_model_callback_t callback,
                             void *cookie,
                             sound_model_handle_t *handle);

/*
 * Unload a sound model. A sound model can be unloaded to make room for a new one to overcome
 * implementation limitations.
 */
int (qsthw_unload_sound_model)(const qsthw_module_handle_t *mod_handle,
                               sound_model_handle_t handle);

/*
 * Start recognition on a given model. Only one recognition active at a time per model.
 * Once recognition succeeds of fails, the callback is called.
 */
int (qsthw_start_recognition)(const qsthw_module_handle_t *mod_handle,
                              sound_model_handle_t sound_model_handle,
                              const struct sound_trigger_recognition_config *config,
                              qsthw_recognition_callback_t callback,
                              void *cookie);

/*
 * Stop recognition on a given model.
 * The implementation does not have to call the callback when stopped via this method.
 */
int (qsthw_stop_recognition)(const qsthw_module_handle_t *mod_handle,
                             sound_model_handle_t sound_model_handle);

/*
 * Set parameters on a given model.
 * The function accepts a list of
 * parameter key value pairs in the form: key1=value1;key2=value2;...
 */
int (qsthw_set_parameters)(const qsthw_module_handle_t *mod_handle,
                           sound_model_handle_t sound_model_handle,
                           const char *kv_pairs);

/*
 * Get read buffer size.
 */
size_t (qsthw_get_buffer_size)(const qsthw_module_handle_t *mod_handle,
                               sound_model_handle_t sound_model_handle);

/*
 * Read buffered audio after keyword detection.
 * The buffered data can be processed by clients for further actions.
 */
int (qsthw_read_buffer)(const qsthw_module_handle_t *mod_handle,
                        sound_model_handle_t sound_model_handle,
                        unsigned char *buf,
                        size_t bytes);

/*
 * Stop buffering audio.
 * Called after sufficient buffered data is read to stop further buffering.
 */
int (qsthw_stop_buffering)(const qsthw_module_handle_t *mod_handle,
                           sound_model_handle_t sound_model_handle);
/*
 * Get param data on a given model based on input param.
 * Returns param data in payload along with param data size filled.
 */
int (qsthw_get_param_data)(const qsthw_module_handle_t *mod_handle,
                           sound_model_handle_t sound_model_handle,
                           const char *param,
                           void *payload,
                           size_t payload_size,
                           size_t *param_data_size);


/*returns current QTI HAL version */
int qsthw_get_version();

/* convenience API for loading and unloading qti sound trigger module */

qsthw_module_handle_t *qsthw_load_module(const char *hw_module_id);

int qsthw_unload_module(const qsthw_module_handle_t *mod_handle);

__END_DECLS

#endif  //QTI_ST_HW_API_H
