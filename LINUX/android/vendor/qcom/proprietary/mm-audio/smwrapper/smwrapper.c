/*
 *
 * This library contains the soundmodel library wrapper APIs for
 * Sound trigger HAL
 *
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "smwrapper"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

#include <cutils/log.h>
#include <errno.h>
#include <stdlib.h>
#include <system/sound_trigger.h>

#ifdef SM_EXTN_ENABLED
#include "qsthw_defs.h"
#endif

/* This function fills the sound trigger recognition event from the
 *  payload received by DSP and the soundmodel data. The caller is
 *  responsible for freeing the out_rc_event. Return 0 for success
 *  and negative value for error.
 */
int generate_sound_trigger_phrase_recognition_event
(
    const struct sound_trigger_phrase_sound_model *phrase_sm,
    const struct sound_trigger_recognition_config *rc_config,
    const void *payload,
    unsigned int payload_size,
    struct sound_trigger_phrase_recognition_event **out_rc_event
)
{
    struct sound_trigger_phrase_recognition_event *event;
    unsigned int i, j, user_id;

    ALOGD("%s: Enter payload_size %d", __func__, payload_size);

    if(!payload || !phrase_sm || !rc_config || !out_rc_event) {
        ALOGE("%s: Null params", __func__);
        return -EINVAL;
    }

    *out_rc_event = NULL;
    event = calloc(1, sizeof(*event) + payload_size);
    if (!event) {
        ALOGE("%s: event allocation failed size %d", __func__, payload_size);
        return -ENODEV;
    }

    event->num_phrases = rc_config->num_phrases;
    event->common.data_offset = sizeof(*event);
    event->common.data_size = payload_size;
    memcpy((char *)event + event->common.data_offset, payload, payload_size);

    /* fill confidence levels */
    for (i = 0; i < rc_config->num_phrases; i++) {
        event->phrase_extras[i].id = rc_config->phrases[i].id;
        event->phrase_extras[i].recognition_modes =
                                 phrase_sm->phrases[0].recognition_mode;
        event->phrase_extras[i].confidence_level = ((char *)payload)[i];
        event->phrase_extras[i].num_levels =  rc_config->phrases[i].num_levels;
        for (j = 0; j < rc_config->phrases[i].num_levels; j++) {
            user_id = rc_config->phrases[i].levels[j].user_id;
            event->phrase_extras[i].levels[j].user_id = user_id;
            event->phrase_extras[i].levels[j].level =
                                             ((char *)payload)[user_id];
        }
    }

    *out_rc_event = event;
    return 0;
}

int generate_sound_trigger_phrase_recognition_event_v2
(
    const struct sound_trigger_phrase_sound_model *phrase_sm,
    const struct sound_trigger_recognition_config *rc_config,
    const void *payload,
    unsigned int payload_size,
    struct sound_trigger_phrase_recognition_event **out_rc_event
)
{
    unsigned char *payload_ptr = (unsigned char *)payload;
    unsigned int minor_version = 0, num_active_models = 0;

    if(!payload) {
        ALOGE("%s: Null params", __func__);
        return -EINVAL;
    }

    /* parse minor version and num_active_models */
    minor_version = *(payload_ptr++);
    num_active_models = *(payload_ptr++);

    ALOGD("%s: payload_size %d, minor_version = %d, num_active_models = %d",
       __func__, payload_size, minor_version, num_active_models);

    return generate_sound_trigger_phrase_recognition_event(phrase_sm,
       rc_config, payload_ptr, num_active_models, out_rc_event);
}

/*
* This function fills the confidence level payload to be sent to DSP
 * from the sound trigger recognition configuration object. Caller is
 * responsible to provide enough memory for storing the confidence levels
 * based on how many there are in the phrase_sm
 */
static int fill_sound_trigger_recognition_config_payload
(
   const struct sound_trigger_phrase_sound_model *phrase_sm,
   const struct sound_trigger_recognition_config *rc_config,
   unsigned char *conf_levels,
   unsigned int num_conf_levels,
   unsigned int total_num_users
)
{
    int status = 0;
    unsigned int user_level, user_id;
    unsigned int i, j;
    unsigned char *user_id_tracker;

    if (!phrase_sm || !rc_config || !conf_levels || !num_conf_levels) {
        ALOGE("%s: ERROR. Invalid inputs",__func__);
        return -EINVAL;
    }

    /* Example: Say the recognition structure has 3 keywords with users
       |kid|
        [0] k1 |uid|
                [0] u1 - 1st trainer
                [1] u2 - 4th trainer
                [3] u3 - 3rd trainer
        [1] k2
                [2] u2 - 2nd trainer
                [4] u3 - 5th trainer
        [2] k3
                [5] u4 - 6th trainer

      Output confidence level array will be
      [k1, k2, k3, u1k1, u2k1, u2k2, u3k1, u3k2, u4k3]
    */

    user_id_tracker = calloc(1, num_conf_levels);
    if (!user_id_tracker) {
        ALOGE("%s: failed to allocate user_id_tracker", __func__);
        return -ENOMEM;
    }

    for (i = 0; i < rc_config->num_phrases; i++) {
        ALOGV("%s: [%d] kw level %d", __func__, i,
                        rc_config->phrases[i].confidence_level);
        for (j = 0; j < rc_config->phrases[i].num_levels; j++) {
            ALOGV("%s: [%d] user_id %d level %d ", __func__, i,
                  rc_config->phrases[i].levels[j].user_id,
                  rc_config->phrases[i].levels[j].level);
        }
    }

    for (i = 0; i < rc_config->num_phrases; i++) {
        conf_levels[i] = rc_config->phrases[i].confidence_level;
        for (j = 0; j < rc_config->phrases[i].num_levels; j++) {
            user_level = rc_config->phrases[i].levels[j].level;
            user_id = rc_config->phrases[i].levels[j].user_id;
            if ((user_id < rc_config->num_phrases) ||
                (user_id >= num_conf_levels)) {
                ALOGE("%s: ERROR. Invalid params user id %d>%d",
                      __func__, user_id, total_num_users);
                status = -EINVAL;
                goto exit;
            } else {
                if (user_id_tracker[user_id] == 1) {
                    ALOGE("%s: ERROR. Duplicate user id %d",
                          __func__, user_id);
                    status = -EINVAL;
                    goto exit;
                }
                conf_levels[user_id] = (user_level < 100) ? user_level: 100;
                user_id_tracker[user_id] = 1;
                ALOGV("%s: user_conf_levels[%d] = %d", __func__,
                                    user_id, conf_levels[user_id]);
            }
        }
    }

exit:
    free(user_id_tracker);
    return status;
}

/* This function fills the confidence level payload to be sent to DSP
 * from the sound trigger recognition configuration object. Caller is
 * responsible for freeing the out_payload.Return 0 for success
 * and negative value for error.
 */
int generate_sound_trigger_recognition_config_payload
(
   const struct sound_trigger_phrase_sound_model *phrase_sm,
   const struct sound_trigger_recognition_config *rc_config,
   unsigned char **out_payload,
   unsigned int *out_payload_size
)
{
    int status = 0;
    unsigned int total_num_users = 0, num_conf_levels = 0;
    unsigned int user_level, user_id;
    unsigned int i = 0, j = 0;
    unsigned char *conf_levels = NULL;
    unsigned char *user_id_tracker;

    if (!phrase_sm || !rc_config || !out_payload || !out_payload_size) {
        ALOGE("%s: ERROR. Invalid inputs",__func__);
        status = -EINVAL;
        goto exit;
    }
    *out_payload = NULL;
    *out_payload_size = 0;

    if((rc_config->num_phrases == 0) ||
       (rc_config->num_phrases > phrase_sm->num_phrases)) {
        ALOGE("%s: ERROR. Invalid phrases %d!=%d",__func__,
              rc_config->num_phrases, phrase_sm->num_phrases);
        status = -EINVAL;
        goto exit;
    }
    for (i = 0; i < rc_config->num_phrases; i++) {
        for (j = 0; j < rc_config->phrases[i].num_levels; j++)
            total_num_users++;
    }

    num_conf_levels = total_num_users + rc_config->num_phrases;
    conf_levels = calloc(1, num_conf_levels);
    if (!conf_levels) {
        ALOGE("%s: ERROR. conf levels alloc failed",__func__);
        status = -ENOMEM;
        goto exit;
    }

    status = fill_sound_trigger_recognition_config_payload(phrase_sm, rc_config, conf_levels,
        num_conf_levels, total_num_users);
    if (status) {
        ALOGE("%s: fill config payload failed, error %d", __func__, status);
        goto exit;
    }

    *out_payload = conf_levels;
    *out_payload_size = num_conf_levels;

    return status;

exit:
    if (conf_levels)
        free(conf_levels);

    return status;
}

int generate_sound_trigger_recognition_config_payload_v2
(
   const struct sound_trigger_phrase_sound_model *phrase_sm,
   const struct sound_trigger_recognition_config *rc_config,
   unsigned char **out_payload,
   unsigned int *out_payload_size
)
{
    int status = 0;
    unsigned int total_num_users = 0, num_conf_levels = 0;
    unsigned int user_level, user_id;
    unsigned char *conf_levels = NULL;
    unsigned char *user_id_tracker;
    unsigned int i = 0, j = 0;

    ALOGV("%s: Enter...", __func__);

    if (!phrase_sm || !rc_config || !out_payload || !out_payload_size) {
        ALOGE("%s: ERROR. Invalid inputs",__func__);
        status = -EINVAL;
        goto exit;
    }
    *out_payload = NULL;
    *out_payload_size = 0;

    if((rc_config->num_phrases == 0) ||
       (rc_config->num_phrases > phrase_sm->num_phrases)) {
        ALOGE("%s: ERROR. Invalid phrases %d!=%d",__func__,
              rc_config->num_phrases, phrase_sm->num_phrases);
        status = -EINVAL;
        goto exit;
    }
    for (i = 0; i < rc_config->num_phrases; i++) {
        for (j = 0; j < rc_config->phrases[i].num_levels; j++)
            total_num_users++;
    }

    num_conf_levels = total_num_users + rc_config->num_phrases;
    /*
     * allocate dsp payload w/additional 2 bytes for minor_version and
     * num_active_models and additional num_conf_levels for KW enable
     * fields
     */
    conf_levels = calloc(1, 2 + 2 * num_conf_levels);
    if (!conf_levels) {
        ALOGE("%s: ERROR. conf levels alloc failed",__func__);
        status = -ENOMEM;
        goto exit;
    }

    conf_levels[0] = 1; /* minor version */
    conf_levels[1] = num_conf_levels; /* num_active_models */
    status = fill_sound_trigger_recognition_config_payload(phrase_sm, rc_config, conf_levels+2,
        num_conf_levels, total_num_users);
    if (status) {
        ALOGE("%s: fill config payload failed, error %d", __func__, status);
        goto exit;
    }

    /* set KW enable fields to 1 for now
     * TODO set appropriately based on what client is passing in rc_config
     */
    memset(&conf_levels[num_conf_levels+2], 0x1, num_conf_levels);
    ALOGV("%s: here", __func__);
    *out_payload = conf_levels;
    *out_payload_size = 2 + 2 * num_conf_levels; /* add size of minor version and num_active_models */

    return status;

exit:
    if (conf_levels)
        free(conf_levels);

    return status;
}

/* This function fills the sound trigger recognition event from the
 * payload received by DSP and the soundmodel data. The event type
 * passed by caller determines the recognition event struct.
 * ex: qsthw_phrase_recognition_event for event type
 * QSTHW_RC_EVENT_TYPE_TIMESTAMP.
 * The caller is responsible for freeing the out_rc_event.
 * Return 0 for success and negative value for error.
 */
#ifdef SM_EXTN_ENABLED
int generate_sound_trigger_phrase_recognition_event_v3
(
    const struct sound_trigger_phrase_sound_model *phrase_sm,
    const struct sound_trigger_recognition_config *rc_config,
    const void *payload,
    unsigned int payload_size,
    qsthw_recognition_event_type_t event_type,
    void **out_rc_event
)
{
    unsigned int i, j, user_id;
    int rc = 0;
    ALOGD("%s: Enter payload_size %d", __func__, payload_size);

    if (!payload || !phrase_sm || !rc_config || !out_rc_event) {
        ALOGE("%s: Null params", __func__);
        return -EINVAL;
    }

    *out_rc_event = NULL;

    switch (event_type) {
    case QSTHW_RC_EVENT_TYPE_TIMESTAMP: {
        struct qsthw_phrase_recognition_event *event;
        struct sound_trigger_phrase_recognition_event *phrase_event;
        event = calloc(1, sizeof(*event) + payload_size);
        if (!event) {
            ALOGE("%s: event allocation failed size %d",
                   __func__, payload_size);
            rc = -ENODEV;
            break;
        }

        phrase_event = &event->phrase_event;

        phrase_event->num_phrases = rc_config->num_phrases;
        phrase_event->common.data_offset = sizeof(*event);
        phrase_event->common.data_size = payload_size;

        /* fill confidence levels */
        for (i = 0; i < rc_config->num_phrases; i++) {
             phrase_event->phrase_extras[i].id = rc_config->phrases[i].id;
             phrase_event->phrase_extras[i].recognition_modes =
                                 phrase_sm->phrases[0].recognition_mode;
             phrase_event->phrase_extras[i].confidence_level =
                                 ((char *)payload)[i];
             phrase_event->phrase_extras[i].num_levels =
                                 rc_config->phrases[i].num_levels;
             for (j = 0; j < rc_config->phrases[i].num_levels; j++) {
                  user_id = rc_config->phrases[i].levels[j].user_id;
                  phrase_event->phrase_extras[i].levels[j].user_id = user_id;
                  phrase_event->phrase_extras[i].levels[j].level =
                                             ((char *)payload)[user_id];
             }
        }

        /* Copy payload to event using offset generated above */
        memcpy((char *)event + phrase_event->common.data_offset,
               payload, payload_size);

        *out_rc_event = (struct qsthw_phrase_recognition_event *)event;
        break;
    }
    default:
        ALOGE("%s: Invalid event type passed %d", __func__, event_type);
        rc = -EINVAL;
        break;
    }

    return rc;
}
#endif
