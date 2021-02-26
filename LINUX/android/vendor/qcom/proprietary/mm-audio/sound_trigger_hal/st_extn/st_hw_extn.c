/* st_hw_extn.c
 * This file contains extended functionality of
 * sound_trigger_hw.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "sound_trigger_hw_extn"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <errno.h>
#include <stdlib.h>
#include <cutils/log.h>
#include <inttypes.h>

#include "st_session.h"
#include "st_hw_defs.h"

bool sthw_extn_check_process_det_ev_support()
{
    return true;
}

/* recognition event with timestamp */
int sthw_extn_process_detection_event_keyphrase(
    st_session_t *st_ses, uint64_t timestamp, int detect_status,
    void *payload, size_t payload_size,
    struct sound_trigger_phrase_recognition_event **event)
{
    st_hw_session_t *st_hw_ses = NULL;
    struct qsthw_phrase_recognition_event *local_event = NULL;
    struct st_vendor_info *v_info = st_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm= st_ses->sm_data;

    int status = 0;
    unsigned int i, j;
    qsthw_recognition_event_type_t event_type = QSTHW_RC_EVENT_TYPE_TIMESTAMP;

    st_hw_ses = st_ses->hw_ses_current;

    /* if smlib is present, get the event from it, else send the
     * DSP received payload as it is to App
     */
    if (v_info && v_info->smlib_handle) {
        status = v_info->generate_st_phrase_recognition_event_v3(
                                        phrase_sm,
                                        st_ses->rc_config,
                                        payload,
                                        payload_size,
                                        event_type,
                                        (void *)&local_event);

        if (status) {
            ALOGW("%s: smlib fill recognition event failed, status %d",
                  __func__, status);
            goto exit;
        }
    } else {
        local_event = calloc(1, sizeof(*local_event) + payload_size);
        if (!local_event) {
            ALOGE("%s: event allocation failed, size %zd", __func__,
                  payload_size);
            status = -ENOMEM;
            goto exit;
        }

        memcpy(local_event->phrase_event.phrase_extras, st_ses->rc_config->phrases,
               st_ses->rc_config->num_phrases *
               sizeof(struct sound_trigger_phrase_recognition_extra));

        local_event->phrase_event.num_phrases = st_ses->rc_config->num_phrases;
        local_event->phrase_event.common.data_offset = sizeof(*local_event);
        local_event->phrase_event.common.data_size = payload_size;
        memcpy((char *)local_event + local_event->phrase_event.common.data_offset,
               payload, payload_size);
    }

    /* fill the remaining recognition event parameters not specific
     * to soundmodel lib
     */
    local_event->timestamp = timestamp;

    local_event->phrase_event.common.status = detect_status;
    local_event->phrase_event.common.type = st_ses->sm_data->common.type;;
    local_event->phrase_event.common.model = st_ses->sm_handle;
    local_event->phrase_event.common.capture_available = st_ses->capture_requested;
    local_event->phrase_event.common.capture_delay_ms = 0;
    local_event->phrase_event.common.capture_preamble_ms = 0;
    local_event->phrase_event.common.audio_config.sample_rate =
                 SOUND_TRIGGER_SAMPLING_RATE_16000;
    local_event->phrase_event.common.audio_config.channel_mask =
                 AUDIO_CHANNEL_IN_MONO;
    local_event->phrase_event.common.audio_config.format =
                 AUDIO_FORMAT_PCM_16_BIT;

    for (i = 0; i < local_event->phrase_event.num_phrases; i++) {
        ALOGVV("%s: [%d] kw_id %d level %d", __func__,i,
              local_event->phrase_event.phrase_extras[i].id,
              local_event->phrase_event.phrase_extras[i].confidence_level);
        for (j = 0; j < local_event->phrase_event.phrase_extras[i].num_levels; j++) {
            ALOGVV("%s: [%d] user_id %d level %d ", __func__, i,
                  local_event->phrase_event.phrase_extras[i].levels[j].user_id,
                  local_event->phrase_event.phrase_extras[i].levels[j].level);
        }
    }

    ALOGI("%s:[%d] send keyphrase recognition event %d", __func__,
                     st_ses->sm_handle, detect_status);
    ALOGV("%s:[%d] status=%d, type=%d, model=%d, capture_avaiable=%d, "
           "num_phrases=%d, id=%d, timestamp = %" PRIu64,
           __func__, st_ses->sm_handle, local_event->phrase_event.common.status,
           local_event->phrase_event.common.type, local_event->phrase_event.common.model,
           local_event->phrase_event.common.capture_available, local_event->phrase_event.num_phrases,
           local_event->phrase_event.phrase_extras[0].id, local_event->timestamp);

    *event = &local_event->phrase_event;
exit:
    return status;
}
