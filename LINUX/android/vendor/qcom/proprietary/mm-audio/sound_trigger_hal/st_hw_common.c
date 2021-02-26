/* st_hw_common.c
 *
 * This library contains common functionality between
 * sound trigger hw and sound trigger extension interface.

 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "sound_trigger_hw_common"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <cutils/log.h>
#include <hardware/sound_trigger.h>

#include "st_hw_common.h"

int stop_other_sessions(struct sound_trigger_device *stdev,
                         st_session_t *cur_ses)
{
    st_session_t *p_ses;
    struct listnode *p_ses_node;
    int status = 0;

    ALOGV("%s: list empty %s", __func__,
          list_empty(&stdev->sound_model_list) ? "true" : "false");

    if (stdev->audio_concurrency_active)
        return 0;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);

        /* Current session can already be in the list during SSR */
        if (p_ses == cur_ses)
            continue;

        status = st_session_pause(p_ses);
        if (status) {
            ALOGE("%s: error stopping session", __func__);
            return -EIO;
        }
    }
    return 0;
}

int start_other_sessions(struct sound_trigger_device *stdev,
                         st_session_t *cur_ses)
{
    st_session_t *p_ses;
    struct listnode *p_ses_node;
    int status = 0;

    ALOGV("%s: list empty %s", __func__,
       list_empty(&stdev->sound_model_list) ? "true" : "false");

    if (stdev->audio_concurrency_active)
        return 0;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);

        /* Current session can already be in the list during SSR */
        if (p_ses == cur_ses)
            continue;
        status = st_session_resume(p_ses);
        if (status) {
            ALOGE("%s: error restarting session", __func__);
            return -EIO;
        }
    }
    return 0;
}

st_session_t* get_sound_trigger_session(
                    struct sound_trigger_device *stdev,
                    sound_model_handle_t sound_model_handle)

{
    st_session_t *st_session = NULL;
    struct listnode *node;

    list_for_each(node, &stdev->sound_model_list) {
        st_session = node_to_item(node, st_session_t, list_node);
        if (st_session->sm_handle == sound_model_handle)
            return st_session;

    }
    return NULL;
}

