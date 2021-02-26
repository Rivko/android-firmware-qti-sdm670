/*
 *
 * This library contains the API to load sound models with
 * DSP and start/stop detection of associated key phrases.
 *
 * Copyright (c) 2013-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 *
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "sound_trigger_hw"
#define ATRACE_TAG (ATRACE_TAG_HAL)
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

#include <errno.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/trace.h>
#include <system/thread_defs.h>
#include <hardware/sound_trigger.h>

#include "st_common_defs.h"
#include "sound_trigger_platform.h"
#include "sound_trigger_hw.h"
#include "st_session.h"
#include "st_hw_common.h"

#define XSTR(x) STR(x)
#define STR(x) #x

/* count of sound trigger hal clients */
static unsigned int stdev_ref_cnt = 0;
static pthread_mutex_t stdev_init_lock;
static struct sound_trigger_device *stdev = NULL;

/* default properties which will later be updated based on platform configuration */
static struct sound_trigger_properties hw_properties = {
        "QUALCOMM Technologies, Inc", // implementor
        "Sound Trigger HAL", // description
        1, // version
        { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } }, // uuid
        1, // max_sound_models
        1, // max_key_phrases
        1, // max_users
        RECOGNITION_MODE_VOICE_TRIGGER | RECOGNITION_MODE_GENERIC_TRIGGER, // recognition_modes
        true, // capture_transition
        0, // max_capture_ms
        false, // concurrent_capture
        false, //trigger_in_event
        0 // power_consumption_mw
};

static void update_available_phrase_info(st_session_t *p_ses,
          struct sound_trigger_phrase_sound_model *phrase_sm,
          bool add);

static int stop_recognition_l(st_session_t *st_session)
{
    int status = 0;

    st_session_stop_lab(st_session);
    status = st_session_stop(st_session);

    pthread_mutex_lock(&st_session->lock);
    st_session->callback = NULL;

    if (st_session->rc_config) {
        free(st_session->rc_config);
        st_session->rc_config = NULL;
    }
    pthread_mutex_unlock(&st_session->lock);
    return status;
}

void update_hw_mad_exec_mode(st_exec_mode_t mode, st_profile_type_t profile_type)
{
    if (stdev->exec_mode != mode) {
        platform_stdev_connect_mad(stdev->platform, mode, profile_type);
        stdev->exec_mode = mode;
        ALOGV("%s: new exec_mode 0x%x", __func__, mode);
    }
}

unsigned int get_num_sessions()
{
    struct listnode *ses_node;
    st_session_t *p_ses;
    unsigned int num_sessions = 0;

    if ((list_head(&stdev->sound_model_list) == NULL) ||
                  (list_tail(&stdev->sound_model_list) == NULL)) {
        ALOGE("%s: sound model list is yet to be initialized", __func__);
        return num_sessions;
    }

    list_for_each(ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(ses_node, st_session_t, list_node);
            num_sessions++;
    }

    return num_sessions;
}

unsigned int get_num_sessions_in_exec_mode(st_exec_mode_t mode)
{
    struct listnode *ses_node;
    st_session_t *p_ses;
    unsigned int num_sessions = 0;

    if ((list_head(&stdev->sound_model_list) == NULL) ||
                  (list_tail(&stdev->sound_model_list) == NULL)) {
        ALOGE("%s: sound model list is yet to be initialized", __func__);
        return num_sessions;
    }

    list_for_each(ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(ses_node, st_session_t, list_node);
        if (p_ses->exec_mode == mode)
            num_sessions++;
    }

    return num_sessions;
}

static bool check_phrases_users_available(struct st_vendor_info *v_info,
    unsigned int num_phrases, unsigned int num_users,
    st_exec_mode_t exec_mode, bool transit)
{
    bool available = false;

    if (!v_info) {
        ALOGE("%s: NULL vendor info", __func__);
        return false;
    }

    if (!transit) {
        if (exec_mode == ST_EXEC_MODE_CPE) {
            if ((num_phrases <= v_info->avail_cpe_phrases) &&
                (num_users <= v_info->avail_cpe_users))
                available = true;
        }

        if (exec_mode == ST_EXEC_MODE_ADSP) {
            if ((num_phrases <= v_info->avail_ape_phrases) &&
                (num_users <= v_info->avail_ape_users))
                 available = true;
        }
    } else {
        if (exec_mode == ST_EXEC_MODE_CPE) {
            if ((num_phrases <= v_info->avail_transit_cpe_phrases) &&
                (num_users <= v_info->avail_transit_cpe_users))
                 available = true;
        }

        if (exec_mode == ST_EXEC_MODE_ADSP) {
            if ((num_phrases <= v_info->avail_transit_ape_phrases) &&
                (num_users <= v_info->avail_transit_ape_users))
                 available = true;
        }
    }

    ALOGV("%s: exec mode %d, transit %d, available %d",
           __func__, exec_mode, transit, available);
    return available;
}

static int parse_exec_mode_config(char *value,
                                  st_exec_mode_t *exec_mode,
                                  sound_model_handle_t *sm_handle)
{
    int ret = -EINVAL;
    char *id, *test_r;

    if (!value || !exec_mode || !sm_handle) {
        ALOGE("%s: Invalid params passed", __func__);
        return ret;
    }

    /*
     * <exec_mode>,<sound_model_handle>
     * sound model handle is optional. Valid set param values are:
     * 1. "0" -> exec mode is set and sound model handle to default value 0
     * 2. "1,2" -> exec mode and sound model handle are set to passed values
     * sound model handle as 0 is treated as a global set parameter.
     */

    id = strtok_r(value, ", ", &test_r);
    if (!id) {
        ALOGE("%s: incorrect exec mode passed", __func__);
        return ret;
    }
    *exec_mode = atoi(id);

    id = strtok_r(NULL, ", ", &test_r);
    if (!id) {
        ALOGD("%s: No session id passed, treat as global set param", __func__);
        *sm_handle = 0;
    } else {
        *sm_handle = atoi(id);
    }

    ALOGD("%s: mode %d, sm handle %d", __func__, *exec_mode, *sm_handle);
    return 0;
}

static bool is_any_session_active()
{
    struct listnode *p_ses_node;
    st_session_t *p_ses;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        if (st_session_is_active(p_ses)) {
            ALOGD("%s:[%d] session is active", __func__, p_ses->sm_handle);
            return true;
        }
    }
    return false;
}

static bool is_any_session_buffering()
{
    struct listnode *p_ses_node;
    st_session_t *p_ses;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        if (st_session_is_buffering(p_ses)) {
            ALOGD("%s:[%d] session is buffering", __func__, p_ses->sm_handle);
            return true;
        }
    }
    return false;
}

static bool is_any_session_ssr_state()
{
    struct listnode *p_ses_node;
    st_session_t *p_ses;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        if (st_session_is_ssr_state(p_ses)) {
            ALOGD("%s:[%d] session is in ssr state", __func__, p_ses->sm_handle);
            return true;
        }
    }
    return false;
}

static int check_and_transit_cpe_ses_to_ape(st_session_t *cur_ses)
{
    st_session_t *p_ses = NULL;
    struct listnode *node;
    unsigned int num_sessions = 0;
    struct st_vendor_info *ses_v_info = NULL;
    struct listnode transit_sound_model_list;
    int ret = -EINVAL;
    bool backend_cfg_change = false;

    /* Switch sessions from CPE to ADSP
     * 1. If cur_ses is NULL, all existing sessions are switched to ADSP.
     * 2. If cur_ses is valid, cur_ses is the session requested to transit.
     *    In such cases, switch sessions if they share same backend with
     *    cur_ses.
     */
    ALOGD("%s: enter: current ses %p", __func__, cur_ses);

    if (0 == get_num_sessions_in_exec_mode(ST_EXEC_MODE_CPE)) {
        ret = 0;
        goto exit;
    }

    if (cur_ses && (cur_ses->exec_mode != ST_EXEC_MODE_CPE)) {
        ALOGE("%s: Invalid mode set for current session %d",
               __func__, cur_ses->exec_mode);
        goto exit;
    }

    /* Initialize transit ape phrases/users for all CPE sessions */
    list_for_each(node, &stdev->sound_model_list) {
        p_ses = node_to_item(node, st_session_t, list_node);
        if (p_ses->exec_mode == ST_EXEC_MODE_CPE) {
            ses_v_info = p_ses->vendor_uuid_info;
            ses_v_info->avail_transit_ape_phrases = ses_v_info->avail_ape_phrases;
            ses_v_info->avail_transit_ape_users = ses_v_info->avail_ape_users;
        }
    }

    list_init(&transit_sound_model_list);
    /*
     * check if the existing CPE sessions can be moved to ADSP
     * Add sessions to be switched to transit list.
     */
    list_for_each(node, &stdev->sound_model_list) {
        p_ses = node_to_item(node, st_session_t, list_node);
        if (p_ses->exec_mode == ST_EXEC_MODE_CPE) {
            /* check for sessions that match backend with current session */
            if (cur_ses && (cur_ses != p_ses) &&
                !platform_stdev_check_backends_match(stdev->platform,
                                                  cur_ses->exec_mode,
                                  cur_ses->hw_ses_current->st_device,
                                                    p_ses->exec_mode,
                                   p_ses->hw_ses_current->st_device)) {
                ALOGV("%s:[%d] session not sharing backend",
                       __func__, p_ses->sm_handle);
                continue;
            }

            if (++num_sessions > stdev->max_ape_sessions) {
                ALOGE("%s: ERROR. sessions exceed max supported", __func__);
                goto exit;
            }

            ses_v_info = p_ses->vendor_uuid_info;
            if (ses_v_info->exec_mode_cfg != EXEC_MODE_CFG_DYNAMIC) {
                ALOGE("%s:[%d] ERROR. exec mode cfg not set to Dynamic",
                       __func__, p_ses->sm_handle);
                goto exit;
            }

            if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
                if (check_phrases_users_available(ses_v_info, p_ses->num_phrases,
                                  p_ses->num_users, ST_EXEC_MODE_ADSP, true)) {
                     ses_v_info->avail_transit_ape_phrases  -= p_ses->num_phrases;
                     ses_v_info->avail_transit_ape_users  -= p_ses->num_users;
                } else {
                    ALOGE("%s: ERROR. phrases/users exceed max supported", __func__);
                    goto exit;
                }
            }
            /* Add the session to transit sound models list */
            list_add_tail(&transit_sound_model_list,
                                   &p_ses->transit_list_node);
        }
    }

    ALOGV("%s: transit list empty: %s", __func__,
       list_empty(&transit_sound_model_list) ? "true" : "false");

    /*
     * During switching, below order is followed to ensure teardown
     * of all sessions happens first and then are started in new exec mode.
     * This helps avoid issues during multisession backend switching.
     * 1. Teardown the sessions by calling set_exec_mode with NONE.
     * 2. Bring up the sessions in ADSP mode.
     */

    /* Move sessions present in transit sound model list to NONE */
    list_for_each(node, &transit_sound_model_list) {
        p_ses = node_to_item(node, st_session_t, transit_list_node);

        if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
            update_available_phrase_info(p_ses, p_ses->sm_data, true);

        ALOGD("%s:[%d] switch session to NONE", __func__, p_ses->sm_handle);
        ret = st_session_set_exec_mode(p_ses, ST_EXEC_MODE_NONE);
        if (ret) {
            if (stdev->ssr_offline_received) {
                goto ssr_exit;
            } else {
                /* TODO: Handle error during transitions */
                ALOGE("%s:[%d] ERROR. could not set NONE", __func__, p_ses->sm_handle);
                goto exit;
            }
        }

        /*
         * Check and update codec backend config for each session.
         * Doing it here avoids restart of other active sessions in case of
         * config change as those sessions will also be moved to NONE.
         * Updated config will be picked when sessions move to ADSP below.
         */
        ret = platform_stdev_check_and_set_codec_backend_cfg(stdev->platform,
                             p_ses->vendor_uuid_info, &backend_cfg_change);
        if (ret) {
            ALOGE("%s:[%d] ERROR. codec backend config update failed",
                   __func__, p_ses->sm_handle);
            goto exit;
        }
    }

    /* Move sessions present in transit sound model list to ADSP */
    list_for_each(node, &transit_sound_model_list) {
        p_ses = node_to_item(node, st_session_t, transit_list_node);

        ALOGD("%s:[%d] switch session to ADSP mode", __func__, p_ses->sm_handle);
        ret = st_session_set_exec_mode(p_ses, ST_EXEC_MODE_ADSP);
        if (ret) {
            if (stdev->ssr_offline_received) {
                goto ssr_exit;
            } else {
                /* TODO: Handle error during transitions */
                ALOGE("%s:[%d] ERROR. could not set ADSP mode", __func__, p_ses->sm_handle);
                goto exit;
            }
        }

        if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
            update_available_phrase_info(p_ses, p_ses->sm_data, false);
    }

ssr_exit:
    /* Set target exec mode to be used during ssr online handling */
    list_for_each(node, &transit_sound_model_list) {
        p_ses = node_to_item(node, st_session_t, transit_list_node);
        p_ses->ssr_transit_exec_mode = ST_EXEC_MODE_ADSP;
    }

exit:
    ALOGD("%s: exit: ret %d", __func__, ret);
    return ret;
}

static int check_and_transit_ape_ses_to_cpe(st_session_t *cur_ses)
{
    st_session_t *p_ses = NULL;
    struct listnode *node;
    unsigned int num_sessions = 0, max_sessions;
    struct st_vendor_info *ses_v_info = NULL;
    struct listnode transit_sound_model_list;
    int ret = -EINVAL;

    /* Switch sessions from ADSP to CPE
     * 1. If cur_ses is NULL, all existing sessions are switched to CPE.
     * 2. If cur_ses is valid, cur_ses is the session requested to transit.
     *    In such cases, switch sessions if they share same backend with
     *    cur_ses.
     */
    ALOGD("%s: enter: current ses %p", __func__, cur_ses);

    if (0 == get_num_sessions_in_exec_mode(ST_EXEC_MODE_ADSP)) {
        ret = 0;
        goto exit;
    }

    if (cur_ses && (cur_ses->exec_mode != ST_EXEC_MODE_ADSP)) {
        ALOGE("%s: Invalid mode set for current session %d",
               __func__, cur_ses->exec_mode);
        goto exit;
    }

    max_sessions = stdev->is_gcs ?
                   stdev->max_wdsp_sessions : stdev->max_cpe_sessions;

    /* Initialize transit cpe phrases/users for all ADSP sessions */
    list_for_each(node, &stdev->sound_model_list) {
        p_ses = node_to_item(node, st_session_t, list_node);
        if (p_ses->exec_mode == ST_EXEC_MODE_ADSP) {
            ses_v_info = p_ses->vendor_uuid_info;
            ses_v_info->avail_transit_cpe_phrases = ses_v_info->avail_cpe_phrases;
            ses_v_info->avail_transit_cpe_users = ses_v_info->avail_cpe_users;
        }
    }

    list_init(&transit_sound_model_list);
    /*
     * check if the existing ADSP sessions can be moved to CPE
     * Add sessions to be switched to transit list.
     */
    list_for_each(node, &stdev->sound_model_list) {
        p_ses = node_to_item(node, st_session_t, list_node);
        if (p_ses->exec_mode == ST_EXEC_MODE_ADSP) {
            /* check for sessions that match backend with current session */
            if (cur_ses && (cur_ses != p_ses) &&
                !platform_stdev_check_backends_match(stdev->platform,
                                                  cur_ses->exec_mode,
                                  cur_ses->hw_ses_current->st_device,
                                                    p_ses->exec_mode,
                                   p_ses->hw_ses_current->st_device)) {
                ALOGV("%s:[%d] session not sharing backend",
                       __func__, p_ses->sm_handle);
                continue;
            }

            if (++num_sessions > max_sessions) {
                ALOGE("%s: ERROR. sessions exceed max supported", __func__);
                goto exit;
            }

            ses_v_info = p_ses->vendor_uuid_info;
            if (ses_v_info->exec_mode_cfg != EXEC_MODE_CFG_DYNAMIC) {
                ALOGE("%s: [%d] ERROR. exec mode cfg not set to Dynamic",
                       __func__, p_ses->sm_handle);
                goto exit;
            }

            if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE) {
                if (check_phrases_users_available(ses_v_info, p_ses->num_phrases,
                                  p_ses->num_users, ST_EXEC_MODE_CPE, true)) {
                     ses_v_info->avail_transit_cpe_phrases  -= p_ses->num_phrases;
                     ses_v_info->avail_transit_cpe_users  -= p_ses->num_users;
                } else {
                    ALOGE("%s: ERROR. phrases/users exceed max supported", __func__);
                    goto exit;
                }
            }
            /* Add the session to transit sound models list */
            list_add_tail(&transit_sound_model_list,
                                   &p_ses->transit_list_node);
        }
    }

    ALOGV("%s: transit list empty: %s", __func__,
       list_empty(&transit_sound_model_list) ? "true" : "false");

    /*
     * During switching, below order is followed to ensure teardown
     * of all sessions happens first and then are started in new exec mode.
     * This helps avoid issues during multisession backend switching.
     * 1. Teardown the sessions by calling set_exec_mode with NONE.
     * 2. Bring up the sessions in CPE mode.
     */

    /* Move sessions present in transit sound model list to NONE */
    list_for_each(node, &transit_sound_model_list) {
        p_ses = node_to_item(node, st_session_t, transit_list_node);

        if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
            update_available_phrase_info(p_ses, p_ses->sm_data, true);

        ALOGD("%s:[%d] switch session to NONE", __func__, p_ses->sm_handle);
        ret = st_session_set_exec_mode(p_ses, ST_EXEC_MODE_NONE);
        if (ret) {
            if (stdev->ssr_offline_received) {
                goto ssr_exit;
            } else {
                /* TODO: Handle error during transitions */
                ALOGE("%s:[%d] ERROR. could not set NONE", __func__, p_ses->sm_handle);
                goto exit;
            }
        }
    }

    /* Move sessions present in transit sound model list to CPE */
    list_for_each(node, &transit_sound_model_list) {
        p_ses = node_to_item(node, st_session_t, transit_list_node);

        ALOGD("%s:[%d] switch session to CPE mode", __func__, p_ses->sm_handle);
        ret = st_session_set_exec_mode(p_ses, ST_EXEC_MODE_CPE);
        if (ret) {
            if (stdev->ssr_offline_received) {
                goto ssr_exit;
            } else {
                /* TODO: Handle error during transitions */
                ALOGE("%s:[%d] ERROR. could not set CPE mode", __func__, p_ses->sm_handle);
                goto exit;
            }
        }

        if (p_ses->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
            update_available_phrase_info(p_ses, p_ses->sm_data, false);
    }

ssr_exit:
    /* Set target exec mode to be used during ssr online handling */
    list_for_each(node, &transit_sound_model_list) {
        p_ses = node_to_item(node, st_session_t, transit_list_node);
        p_ses->ssr_transit_exec_mode = ST_EXEC_MODE_CPE;
    }

exit:
    ALOGD("%s: exit: ret %d", __func__, ret);
    return ret;
}

static void handle_ssr_online(enum ssr_event_status event)
{
    struct listnode *p_ses_node;
    st_session_t *p_ses;

    ALOGD("%s: Enter", __func__);
    stdev->ssr_offline_received = false;
    ATRACE_BEGIN("sthal: handle_ssr_online");
    pthread_mutex_lock(&stdev->lock);

    /* Events allowed: SND_CARD ONLINE or CPE ONLINE */

    /* reload and reactive each previously active session */
    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        st_session_ssr_online(p_ses, event);
    }
    pthread_cond_broadcast(&stdev->cond);

    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    ALOGD("%s: Exit event %d", __func__, event);
}

static void handle_ssr_offline(enum ssr_event_status event)
{
    struct listnode *p_ses_node;
    st_session_t *p_ses;

    ALOGD("%s: Enter", __func__);
    stdev->ssr_offline_received = true;
    ATRACE_BEGIN("sthal: handle_ssr_offline");
    pthread_mutex_lock(&stdev->lock);

    /* Events allowed: SND_CARD OFFLINE or CPE OFFLINE */

    /* teardown each session */
    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        st_session_ssr_offline(p_ses, event);
    }

    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    ALOGD("%s: Exit event %d", __func__, event);
}

static void handle_audio_concurrency(audio_event_type_t event_type)
{
    struct listnode *p_ses_node;
    st_session_t *p_ses = NULL;
    bool session_allowed;
    unsigned int num_sessions;
    int status = 0;

    ALOGV("%s: Enter, event type = %d", __func__, event_type);

    /*
    UC1:
        1. start_recognition
        2. audio record_active
        3. audio_record_inactive
        4. stop_recognition
    UC1:
        1. start_recognition
        2. audio record_active
        3. stop_recognition
        4. audio_record_inactive
    UC2:
        1. audio_record_active
        2. start_recognition
        3. stop_recogntion
        4. audio_record_inactive
    UC3:
        1. audio_record_active
        2. start_recognition
        3. audio_record_inactive
        4. stop_recogntion
    */
    pthread_mutex_lock(&stdev->lock);

    num_sessions = get_num_sessions();
    session_allowed = platform_stdev_check_and_update_concurrency(stdev->platform,
        event_type, num_sessions);

    if (!num_sessions) {
       pthread_mutex_unlock(&stdev->lock);
       return;
    }

    if (stdev->transit_to_adsp_on_playback) {
        if (AUDIO_EVENT_PLAYBACK_STREAM_ACTIVE == event_type) {
            /* Transition to ADSP */
            if (is_any_session_buffering() ||
                is_any_session_ssr_state()) {
                stdev->transit_dir = TRANSIT_CPE_TO_APE;
                pthread_cond_signal(&stdev->transitions_cond);
            } else {
                status = check_and_transit_cpe_ses_to_ape(p_ses);
                if (status)
                    ALOGE("%s: Transition to ADSP failed, ignore", __func__);
            }
        } else if (AUDIO_EVENT_PLAYBACK_STREAM_INACTIVE == event_type) {
            stdev->transit_dir = TRANSIT_APE_TO_CPE;
            pthread_cond_signal(&stdev->transitions_cond);
        }
    }

    if (session_allowed && stdev->audio_concurrency_active) {
        stdev->audio_concurrency_active = false;

        list_for_each(p_ses_node, &stdev->sound_model_list) {
            p_ses = node_to_item(p_ses_node, st_session_t, list_node);
            st_session_resume(p_ses);
        }
    } else if (!session_allowed && !stdev->audio_concurrency_active) {
        stdev->audio_concurrency_active = true;

        list_for_each(p_ses_node, &stdev->sound_model_list) {
            p_ses = node_to_item(p_ses_node, st_session_t, list_node);
            st_session_pause(p_ses);
        }
    }

    pthread_mutex_unlock(&stdev->lock);
    ALOGV("%s: Exit", __func__);
}

static void *transitions_thread_loop(void * context __unused)
{
    int status = 0;

    /*
     *  This thread is used for audio playback transitions from ADSP
     *  to WDSP. This is needed for hotword detections on ADSP because
     *  the app pauses music playback during buffering and resumes when
     *  buffering stops. An audio playback inactive event sends the
     *  signal to this thread to begin looping while playback is paused.
     *  When both buffering and and audio playback are inactive, it is
     *  assumed to be a stoppage of audio playback NOT due to a hotword
     *  detection. This case will transition to WDSP.
     */
    pthread_mutex_lock(&stdev->lock);
    while (!stdev->stop_transitions_thread_loop) {
        pthread_cond_wait(&stdev->transitions_cond, &stdev->lock);
        if (stdev->stop_transitions_thread_loop) {
            pthread_mutex_unlock(&stdev->lock);
            return NULL;
        }

        if (stdev->transit_dir == TRANSIT_APE_TO_CPE) {
            while (!stdev->rx_concurrency_active) {
                pthread_mutex_unlock(&stdev->lock);
                sleep(stdev->transit_wait_time);
                pthread_mutex_lock(&stdev->lock);

                if (is_any_session_active() &&
                    !stdev->rx_concurrency_active &&
                    !is_any_session_ssr_state()) {
                    /* Transition to WDSP */
                    ATRACE_BEGIN("sthal: check_and_transit_ape_ses_to_cpe");
                    status = check_and_transit_ape_ses_to_cpe(NULL);
                    ATRACE_END();
                    if (status)
                        ALOGE("%s: Transition to WDSP failed, ignore", __func__);
                    break;
                }
            }
        } else if (stdev->transit_dir == TRANSIT_CPE_TO_APE) {
            while (is_any_session_buffering() ||
                   is_any_session_ssr_state()) {
                pthread_mutex_unlock(&stdev->lock);
                sleep(stdev->transit_wait_time);
                pthread_mutex_lock(&stdev->lock);

                if (!is_any_session_buffering() &&
                    !is_any_session_ssr_state()) {
                    /* Transition to ADSP */
                    ATRACE_BEGIN("sthal: check_and_transit_cpe_ses_to_ape");
                    status = check_and_transit_cpe_ses_to_ape(NULL);
                    ATRACE_END();
                    if (status)
                        ALOGE("%s: Transition to ADSP failed, ignore", __func__);
                    break;
                }
            }
        } else {
            ALOGE("%s: Invalid transit direction %d", __func__, stdev->transit_dir);
        }
    }
    pthread_mutex_unlock(&stdev->lock);
    return NULL;
}

static void init_transitions_thread()
{
    int status = 0;

    stdev->stop_transitions_thread_loop = false;
    status = pthread_create(&stdev->transitions_thread, NULL,
        transitions_thread_loop, NULL);
    if (status)
        ALOGE("%s: Error creating transitions thread. status = %d",
            __func__, status);
}

static void switch_device()
{
    struct listnode *p_ses_node;
    st_session_t *p_ses;
    unsigned int num_sessions;

    num_sessions = get_num_sessions();
    if (!num_sessions || stdev->audio_concurrency_active) {
        ALOGV("%s: num sessions %d, concurrency %s", __func__, num_sessions,
              stdev->audio_concurrency_active? "active": "inactive");
        return;
    }

    /* Start switching the device.
     * Disable all use cases followed by disabling the current device.
     * Enable new device followed by enabling all use cases .
     */
    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        st_session_disable_device(p_ses);
    }

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);
        st_session_enable_device(p_ses);
    }
}

static void handle_device_switch(bool connect, audio_event_info_t* config)
{
    audio_devices_t device = config->u.value;

    ALOGD("%s: device 0x%x %s", __func__, device,
          connect? "Connected":"Disconnected");
    ATRACE_BEGIN("sthal: handle_device_switch");

    if (!stdev->support_dev_switch || !stdev->is_gcs ||
        !ST_CAPTURE_DEVICE_IS_SUPPORTED(device)) {
        ATRACE_END();
        ALOGV("%s: unsupported", __func__);
        return;
    }

    pthread_mutex_lock(&stdev->lock);

    /* Update this device to available list of internal devices. At the time
     * of enabling the device for a use case, actual device will be selected
     * based on internal device selection policy
     */
    platform_stdev_update_avail_device(stdev->platform, device, connect);

    if ((connect && (stdev->capture_device == device)) ||
         (!connect && (stdev->capture_device != device))) {
        ATRACE_END();
        ALOGV("%s: device 0x%x already %s", __func__,
              device, connect ? "enabled" : "disabled");
        pthread_mutex_unlock(&stdev->lock);
        return;
    }

    if (is_any_session_buffering()) {
        /* If any session is buffering. Switch device after buffering is
         * completed.
         */
        ALOGV("%s: buffering in progress, device switch diferred", __func__);
    } else {
        switch_device();
    }
    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    ALOGV("%s: Exit", __func__);
}

static int stdev_get_properties(const struct sound_trigger_hw_device *dev,
    struct sound_trigger_properties *properties)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;

    ALOGI("%s", __func__);
    if (properties == NULL) {
        ALOGE("%s: NULL properties", __func__);
        return -EINVAL;
    }

    if (stdev->run_on_ape) {
        stdev->hw_properties->max_sound_models = stdev->max_ape_sessions;
        stdev->hw_properties->max_key_phrases = stdev->avail_ape_phrases;
        stdev->hw_properties->max_users = stdev->avail_ape_users;
        stdev->hw_properties->max_buffer_ms = SOUND_TRIGGER_APE_BUFFER_DURATION_MS;
    } else if (stdev->is_gcs) {
        stdev->hw_properties->max_sound_models = stdev->max_wdsp_sessions;
        stdev->hw_properties->max_key_phrases = stdev->avail_cpe_phrases;
        stdev->hw_properties->max_users = stdev->avail_cpe_users;
        stdev->hw_properties->max_buffer_ms = ST_GRAPHITE_LAB_BUF_DURATION_MS;
        stdev->hw_properties->concurrent_capture = stdev->conc_capture;
    } else {
        stdev->hw_properties->max_sound_models = stdev->max_cpe_sessions;
        stdev->hw_properties->max_key_phrases = stdev->avail_cpe_phrases ;
        stdev->hw_properties->max_users = stdev->avail_cpe_users ;
        stdev->hw_properties->max_buffer_ms = SOUND_TRIGGER_CPE_LAB_DRV_BUF_DURATION_MS;
    }
    ALOGVV("%s recognition_modes=%d, capture_transition=%d,"
           "concurrent_capture=%d", __func__, stdev->hw_properties->recognition_modes,
           stdev->hw_properties->capture_transition, stdev->hw_properties->concurrent_capture);
    memcpy(properties, stdev->hw_properties, sizeof(struct sound_trigger_properties));
    return 0;
}

static st_exec_mode_t get_session_exec_mode
(
    st_session_t *p_ses,
    struct sound_trigger_sound_model *common_sm
)
{
    unsigned int num_sessions = 0;
    unsigned int num_users = 0;
    unsigned int num_phrases = 0;
    unsigned int i, j;
    st_exec_mode_t exec_mode = ST_EXEC_MODE_NONE;
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;
    struct sound_trigger_phrase_sound_model *phrase_sm = NULL;

    num_sessions = get_num_sessions();

    if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        phrase_sm = (struct sound_trigger_phrase_sound_model *)common_sm;
        num_phrases = phrase_sm->num_phrases;

        for (i = 0; i < phrase_sm->num_phrases; i++) {
            for (j = 0; j < phrase_sm->phrases[i].num_users; j++) {
                num_users++;
            }
        }
        p_ses->num_phrases = num_phrases;
        p_ses->num_users = num_users;

        ALOGD("%s: mode=%d NS=%d RKW=%u RU=%u VACKW=%u VACU=%u VADKW=%d, VADU=%d",
              __func__, stdev->run_on_ape, num_sessions, num_phrases, num_users,
              v_info ? v_info->avail_cpe_phrases : stdev->avail_cpe_phrases,
              v_info ? v_info->avail_cpe_users : stdev->avail_cpe_users,
              v_info ? v_info->avail_ape_phrases : stdev->avail_ape_phrases,
              v_info ? v_info->avail_ape_users : stdev->avail_ape_users);
    }

    /*
     * Each algorithm can be configured to either CPE, ADSP or DYNAMIC
     * in the vendor info, if vendor info is present use it to determine
     * exec_mode
     */
    if (v_info) {
        if (v_info->exec_mode_cfg == EXEC_MODE_CFG_CPE) {
            /* Algorithm configured for CPE only */
            if ((!stdev->is_gcs && (num_sessions < stdev->max_cpe_sessions)) ||
                (stdev->is_gcs && (num_sessions < stdev->max_wdsp_sessions))) {
                if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                    /* Keyphrase sound model */
                    if (check_phrases_users_available(v_info, num_phrases,
                                      num_users, ST_EXEC_MODE_CPE, false))
                        exec_mode = ST_EXEC_MODE_CPE;
                } else {
                    /* Generic sound model without keyphrases */
                    exec_mode = ST_EXEC_MODE_CPE;
                }
            }
        } else if (v_info->exec_mode_cfg == EXEC_MODE_CFG_APE) {
            if (num_sessions < stdev->max_ape_sessions) {
                if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                    if (check_phrases_users_available(v_info, num_phrases,
                                     num_users, ST_EXEC_MODE_ADSP, false))
                        exec_mode = ST_EXEC_MODE_ADSP;
                } else {
                    exec_mode = ST_EXEC_MODE_ADSP;
                }
            }
        } else if (v_info->exec_mode_cfg == EXEC_MODE_CFG_DYNAMIC) {
            /*
             * If execution type is DYNAMIC, there are two cases:
             * 1)  Exec mode is dictated by explicit commands from the app.
             *     This comes from stdev->client_req_exec_mode.
             * 2)  If the app is not giving explicit exec mode commands, exec
             *     mode is determined by the state of active audio playback in
             *     accordance with the playback transitions functionality.
             */
            if (stdev->client_req_exec_mode == ST_EXEC_MODE_CPE) {
                /*
                 * Check if the session can run on CPE, if client requested
                 * CPE mode for all sessions.
                 */
                ALOGV("%s: EXEC_MODE_CFG_DYNAMIC: req exec mode CPE", __func__);
                if ((!stdev->is_gcs && (num_sessions < stdev->max_cpe_sessions)) ||
                    (stdev->is_gcs && (num_sessions < stdev->max_wdsp_sessions))) {
                    if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                        /* Keyphrase sound model */
                        if (check_phrases_users_available(v_info, num_phrases,
                                          num_users, ST_EXEC_MODE_CPE, false))
                            exec_mode = ST_EXEC_MODE_CPE;
                    } else {
                        /* Generic sound model without keyphrases */
                        exec_mode = ST_EXEC_MODE_CPE;
                    }
                }
            } else if (stdev->client_req_exec_mode == ST_EXEC_MODE_ADSP) {
                /*
                 * Check if the session can run on ADSP, if client requested
                 * ADSP mode for all sessions.
                 */
                ALOGV("%s: EXEC_MODE_CFG_DYNAMIC: req exec mode ADSP", __func__);
                if (num_sessions < stdev->max_ape_sessions) {
                    if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                        if (check_phrases_users_available(v_info, num_phrases,
                                         num_users, ST_EXEC_MODE_ADSP, false))
                            exec_mode = ST_EXEC_MODE_ADSP;
                    } else {
                        exec_mode = ST_EXEC_MODE_ADSP;
                    }
                }
            } else {
                /*
                 * client_req_exec_mode is NONE.
                 * In this case, check if audio playback transitions are enabled
                 * and the if audio playback is currently active. If these
                 * criteria are satisfied, try on ADSP and error out if the new
                 * session cannot be started on it. If they are not satisfied, try
                 * on CPE and error out if the new session cannot be started on it.
                 *
                 * TODO -
                 * client requested exec mode can be NONE if transition was
                 * requested for a particular session i.e. not a global transition.
                 * In cases where transition was requested per session, two cases
                 * arise for a newly started session:
                 * 1. The new session’s backend does not match with the sessions
                 *    that have been switched and can run independent of them.
                 * 2. The session’s backend matches with the sessions that have
                 *    been switched above.
                 * Since st_device is not available at this point, backend_type
                 * cannot be obtained to determine if it falls in any of the above
                 * cases and exec mode cannot be determined.
                 */
                ALOGV("%s: EXEC_MODE_CFG_DYNAMIC: req exec mode NONE", __func__);
                if (stdev->transit_to_adsp_on_playback &&
                    (stdev->rx_concurrency_active > 0)) {
                    /* Load on ADSP */
                    if (num_sessions < stdev->max_ape_sessions) {
                        if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                            /* Keyphrase sound model */
                            if (check_phrases_users_available(v_info, num_phrases,
                                num_users, ST_EXEC_MODE_ADSP, false))
                                exec_mode = ST_EXEC_MODE_ADSP;
                        } else {
                            /* Generic sound model without keyphrases */
                            exec_mode = ST_EXEC_MODE_ADSP;
                        }
                    }
                } else {
                    /* Load on WDSP */
                    if ((!stdev->is_gcs &&
                        (num_sessions < stdev->max_cpe_sessions)) ||
                        (stdev->is_gcs &&
                        (num_sessions < stdev->max_wdsp_sessions))) {
                        if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE) {
                            /* Keyphrase sound model */
                            if (check_phrases_users_available(v_info, num_phrases,
                                num_users, ST_EXEC_MODE_CPE, false))
                                exec_mode = ST_EXEC_MODE_CPE;
                        } else {
                            /* Generic sound model without keyphrases */
                            exec_mode = ST_EXEC_MODE_CPE;
                        }
                    }
                }
            }
        } else if (v_info->exec_mode_cfg == EXEC_MODE_CFG_ARM) {
            if (num_sessions < stdev->max_arm_sessions)
                exec_mode = ST_EXEC_MODE_ARM;
        }
    } else {
        /* v_info not present, use static defaults */
        if (!stdev->run_on_ape) {
            if (!stdev->is_gcs) {
                if ((num_sessions < stdev->max_cpe_sessions) &&
                    (num_phrases <= stdev->avail_cpe_phrases) &&
                    (num_users <= stdev->avail_cpe_users)) {
                    exec_mode = ST_EXEC_MODE_CPE;
                }
            } else {
                if ((num_sessions < stdev->max_wdsp_sessions) &&
                    (num_phrases <= stdev->avail_cpe_phrases) &&
                    (num_users <= stdev->avail_cpe_users)) {
                    exec_mode = ST_EXEC_MODE_CPE;
                }
            }
        } else {
            if ((num_sessions < stdev->max_ape_sessions) &&
                (num_phrases <= stdev->avail_ape_phrases ) &&
                (num_users <= stdev->avail_ape_users )) {
                exec_mode = ST_EXEC_MODE_ADSP;
            }
        }
    }

    ALOGV("%s: Exit  exec_mode=%d", __func__, exec_mode);
    return exec_mode;
}

static void update_available_phrase_info
(
   st_session_t *p_ses,
   struct sound_trigger_phrase_sound_model *phrase_sm,
   bool add
)
{
    struct st_vendor_info *v_info = p_ses->vendor_uuid_info;

    if (add) {
        if (p_ses->exec_mode == ST_EXEC_MODE_CPE) {
            if (v_info) {
                v_info->avail_cpe_phrases  += phrase_sm->num_phrases;
                v_info->avail_cpe_users  += p_ses->num_users;
            } else {
                stdev->avail_cpe_phrases  += phrase_sm->num_phrases;
                stdev->avail_cpe_users  += p_ses->num_users;
            }
        } else {
            if (v_info) {
                v_info->avail_ape_phrases  += phrase_sm->num_phrases;
                v_info->avail_ape_users  += p_ses->num_users;
            } else {
                stdev->avail_ape_phrases  += phrase_sm->num_phrases;
                stdev->avail_ape_users  += p_ses->num_users;
            }
        }
    } else {
        if (p_ses->exec_mode == ST_EXEC_MODE_CPE) {
            if (v_info) {
                v_info->avail_cpe_phrases  -= phrase_sm->num_phrases;
                v_info->avail_cpe_users  -= p_ses->num_users;
            } else {
                stdev->avail_cpe_phrases  -= phrase_sm->num_phrases;
                stdev->avail_cpe_users  -= p_ses->num_users;
            }
        } else {
            if (v_info) {
                v_info->avail_ape_phrases  -= phrase_sm->num_phrases;
                v_info->avail_ape_users  -= p_ses->num_users;
            } else {
                stdev->avail_ape_phrases  -= phrase_sm->num_phrases;
                stdev->avail_ape_users  -= p_ses->num_users;
            }
        }
    }
}

static bool compare_recognition_config
(
   const struct sound_trigger_recognition_config *current_config,
   struct sound_trigger_recognition_config *new_config
)
{
    unsigned int i, j;

    /*
     * Sometimes if the number of user confidence levels is 0, the
     * sound_trigger_confidence_level struct will be different between the two
     * configs. So all the values must be checked instead of a memcmp of the
     * whole configs.
     */
    if ((current_config->capture_handle != new_config->capture_handle) ||
        (current_config->capture_device != new_config->capture_device) ||
        (current_config->capture_requested != new_config->capture_requested) ||
        (current_config->num_phrases != new_config->num_phrases) ||
        (current_config->data_size != new_config->data_size) ||
        (current_config->data_offset != new_config->data_offset)) {
        return false;
    } else {
        for (i = 0; i < current_config->num_phrases; i++) {
            if ((current_config->phrases[i].id !=
                 new_config->phrases[i].id) ||
                (current_config->phrases[i].recognition_modes !=
                 new_config->phrases[i].recognition_modes) ||
                (current_config->phrases[i].confidence_level !=
                 new_config->phrases[i].confidence_level) ||
                (current_config->phrases[i].num_levels !=
                 new_config->phrases[i].num_levels)) {
                return false;
            } else {
                for (j = 0; j < current_config->phrases[i].num_levels; j++) {
                    if ((current_config->phrases[i].levels[j].user_id !=
                         new_config->phrases[i].levels[j].user_id) ||
                        (current_config->phrases[i].levels[j].level !=
                         new_config->phrases[i].levels[j].level))
                        return false;
                }
            }
        }
        return true;
    }
}

static int load_audio_hal()
{
    int status = 0;
    char audio_hal_lib[100];
    void *sthal_prop_api_version;

    snprintf(audio_hal_lib, sizeof(audio_hal_lib), "%s/%s.%s.so",
             AUDIO_HAL_LIBRARY_PATH1, AUDIO_HAL_NAME_PREFIX,
             XSTR(SOUND_TRIGGER_PLATFORM));
    if (access(audio_hal_lib, R_OK)) {
        snprintf(audio_hal_lib, sizeof(audio_hal_lib), "%s/%s.%s.so",
                 AUDIO_HAL_LIBRARY_PATH2, AUDIO_HAL_NAME_PREFIX,
                 XSTR(SOUND_TRIGGER_PLATFORM));
        if (access(audio_hal_lib, R_OK)) {
            ALOGE("%s: ERROR. %s not found", __func__, audio_hal_lib);
            return -ENOENT;
        }
    }

    stdev->audio_hal_handle = dlopen(audio_hal_lib, RTLD_NOW);
    if (!stdev->audio_hal_handle) {
        ALOGE("%s: ERROR. %s", __func__, dlerror());
        return -ENODEV;
    }

    dlerror();
    DLSYM(stdev->audio_hal_handle, stdev->audio_hal_cb, audio_hw_call_back,
          status);
    if (status)
        goto error;

    DLSYM(stdev->audio_hal_handle, sthal_prop_api_version,
          sthal_prop_api_version, status);
    if (status) {
        stdev->sthal_prop_api_version = 0;
        status  = 0; /* passthru for backward compability */
    } else {
        stdev->sthal_prop_api_version = *(int*)sthal_prop_api_version;
        if (MAJOR_VERSION(stdev->sthal_prop_api_version) !=
            MAJOR_VERSION(STHAL_PROP_API_CURRENT_VERSION)) {
            ALOGE("%s: Incompatible API versions sthal:0x%x != ahal:0x%x",
                  __func__, STHAL_PROP_API_CURRENT_VERSION,
                  stdev->sthal_prop_api_version);
            goto error;
        }
        ALOGD("%s: ahal is using proprietary API version 0x%04x", __func__,
              stdev->sthal_prop_api_version);
    }
    return status;

error:
    dlclose(stdev->audio_hal_handle);
    stdev->audio_hal_handle = NULL;
    return status;
}

static int stdev_load_sound_model(const struct sound_trigger_hw_device *dev,
                                  struct sound_trigger_sound_model *sound_model,
                                  sound_model_callback_t callback __unused,
                                  void *cookie __unused,
                                  sound_model_handle_t *handle)
{
    int status = 0;
    st_exec_mode_t exec_mode;
    st_session_t *st_session = NULL;
    struct sound_trigger_phrase_sound_model *phrase_sm = NULL;
    struct sound_trigger_sound_model *common_sm = NULL;
    unsigned int sm_size = 0;
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    pthread_mutexattr_t attr;
    bool concurrency_allowed = false;
    bool backend_cfg_change = false;

    ALOGD("%s", __func__);
    ATRACE_BEGIN("sthal: stdev_load_sound_model");

    pthread_mutex_lock(&stdev->lock);

    if (handle == NULL || sound_model == NULL) {
        status = -EINVAL;
        goto exit;
    }

    concurrency_allowed = platform_stdev_is_session_allowed(stdev->platform, get_num_sessions());

    /*
     * for swmad case dont load sm and return failure to client
     */
    if (!concurrency_allowed && stdev->sw_mad) {
        ALOGW("%s: session prevented by concurrency", __func__);
        status = -ENODEV;
        goto exit;
    }

    if (sound_model->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        phrase_sm = (struct sound_trigger_phrase_sound_model*)sound_model;
        if ((phrase_sm->common.data_size == 0) ||
            (phrase_sm->common.data_offset < sizeof(*phrase_sm)) ||
            (phrase_sm->common.type != SOUND_MODEL_TYPE_KEYPHRASE) ||
            (phrase_sm->num_phrases == 0)) {
            ALOGE("%s: Invalid sound model params data size=%d, data offset=%d, "
                  "type=%d phrases=%d", __func__, phrase_sm->common.data_size,
                  phrase_sm->common.data_offset, phrase_sm->common.type,
                  phrase_sm->num_phrases);
            status = -EINVAL;
            goto exit;
        }
        common_sm = &phrase_sm->common;
        sm_size = sizeof(*phrase_sm) + phrase_sm->common.data_size;
    } else if (sound_model->type == SOUND_MODEL_TYPE_GENERIC) {
        if (!sound_model->data_size ||
            (sound_model->data_offset < sizeof(*common_sm))) {
            ALOGE("%s: Invalid Generic sound model params data size=%d, data offset=%d",
                  __func__, sound_model->data_size, sound_model->data_offset);
            status = -EINVAL;
            goto exit;
        }
        common_sm = sound_model;
        sm_size = sizeof(*common_sm) + common_sm->data_size;
    } else {
        ALOGE("%s: Unknown sound model type %d", __func__, sound_model->type);
        status = -EINVAL;
        goto exit;
    }

    ALOGV("%s: sm_size=%d data_size=%d", __func__, sm_size, common_sm->data_size);
    st_session = calloc(1, sizeof(st_session_t));
    if (!st_session) {
        status = -ENOMEM;
        goto exit;
    }

    /* CPE takes time to become online, so parse for the pcm devices
       here instead during boot time */
    if (!CHECK_BIT(stdev->hw_type,
            ST_DEVICE_HW_APE|ST_DEVICE_HW_CPE|ST_DEVICE_HW_ARM)) {
        status = platform_stdev_get_hw_type(stdev->platform);
        if (status)
            goto exit;
    }

    /* Use cases:
       1. No vendor uuids defined in platform file                                                    .
            - Use default non topology based SVA solution.
       2. vendor uuids defined in platform file and vendor uuid doesn't match                         .
            - Return error.
       3. vendor uuids defined in platform file and vendor uuid matches                               .
            - Use topology based solution.
     */

    /* TODO: Google search app doesn't pass the vendor_uuid yet.
     * Update vendor_uuid accordingly to maintain backward compatibility with
     * old platform file hosting vendor uuid. With multisession support App
     * has to pass the sound model uuid matching the uuid under updated platform
     * file
     */
    sound_trigger_uuid_t temp_uuid;
    memset(&temp_uuid, 0, sizeof(temp_uuid));
    if (!memcmp(&common_sm->vendor_uuid, &temp_uuid, sizeof(sound_trigger_uuid_t))) {
        ALOGV("%s: no valid vendor_uuid present, using SVA uuid", __func__);
        common_sm->vendor_uuid = hw_properties.uuid;
    }

    st_session->vendor_uuid_info = platform_stdev_get_vendor_info(
                                      stdev->platform,
                                      &common_sm->vendor_uuid);

    if (!st_session->vendor_uuid_info) {
        ALOGE("%s: no matching vendor uuid found", __func__);
        status = -EINVAL;
        goto exit;
    }

    exec_mode = get_session_exec_mode(st_session, common_sm);
    if (exec_mode == ST_EXEC_MODE_NONE) {
        status = -EINVAL;
        goto exit;
    }

    *handle = android_atomic_inc(&stdev->session_id);

    ALOGD("%s: calling st_session_init with st_session ptr %p", __func__, st_session);
    status = st_session_init(st_session, stdev, exec_mode, *handle);
    if (status) {
        ALOGE("%s: failed to initialize st_session with error %d", __func__, status);
        goto exit_1;
    }

    /* Store the sound model information for handling SSR
       and interaction with smlib */
    st_session->sm_data = calloc(1, sm_size);
    if (!st_session->sm_data) {
        status = -ENOMEM;
        goto exit_2;
    }

    if (sound_model->type == SOUND_MODEL_TYPE_KEYPHRASE) {
        memcpy(st_session->sm_data, (char *)phrase_sm, sizeof(*phrase_sm));
        memcpy((char *)st_session->sm_data + phrase_sm->common.data_offset,
               (char *)phrase_sm + phrase_sm->common.data_offset,
               phrase_sm->common.data_size);
        /* TODO: SVA doesn't support per keyword recognition mode.
         * So use the first phrase recognition mode. App is supposed
         * to set the proper recognition mode in the first phrase
         */
        st_session->recognition_mode = phrase_sm->phrases[0].recognition_mode;
        ALOGD("%s: sm magic number 0x%x rm %d", __func__,
              ((int *)((char *)st_session->sm_data + phrase_sm->common.data_offset))[0],
              phrase_sm->phrases[0].recognition_mode);
    } else {
        memcpy(st_session->sm_data, (char *)common_sm, sizeof(*common_sm));
        memcpy((char *)st_session->sm_data + common_sm->data_offset,
               (char *)common_sm + common_sm->data_offset,
               common_sm->data_size);
        ALOGD("%s: sm magic number 0x%x", __func__,
              ((int *)((char *)st_session->sm_data + common_sm->data_offset))[0]);
    }

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&st_session->lock, (const pthread_mutexattr_t *)&attr);
    st_session->sm_type = sound_model->type;

    /* CPE DRAM can only be accessed by single client, i.e. Apps or CPE,
     * but not both in parallel. Sending params here can access DRAM.
     * If another session is already active, stop it and restart after
     * sending this session params.
     * note: TODO: remove checking of "is_gcs" flag here
     */
    if ((ST_EXEC_MODE_CPE == st_session->exec_mode) && !stdev->is_gcs)
        stop_other_sessions(stdev, st_session);


    if (ST_EXEC_MODE_ADSP == st_session->exec_mode ||
        ST_EXEC_MODE_ARM == st_session->exec_mode) {
        status = platform_stdev_check_and_set_codec_backend_cfg(stdev->platform,
                             st_session->vendor_uuid_info, &backend_cfg_change);
        if (status) {
            ALOGE("%s: ERROR. codec backend config update failed", __func__);
            goto exit_2;
        }

        if (backend_cfg_change) {
           /* if backend config change because of this session, stop and start
            * existing sessions.
            * This will ensure existing sessions are routed with same updated
            * codec backend configuration.
            */
            ALOGD("%s: backend config change", __func__);
            stop_other_sessions(stdev, st_session);
            start_other_sessions(stdev, st_session);
        }
    }

    /* No need for session lock as session is not pushed to list and
       the ssr/concurrencies are guarded with dev lock */
    status = st_session_load_sm(st_session);
    if (status) {
        pthread_mutex_destroy(&st_session->lock);
        goto exit_2;
    }

    if (!concurrency_allowed) {
        status = st_session_pause(st_session);
        if (status) {
            pthread_mutex_destroy(&st_session->lock);
            goto exit_2;
        }
    }

    if ((ST_EXEC_MODE_CPE == st_session->exec_mode) && !stdev->is_gcs)
        start_other_sessions(stdev, st_session);

    /* Keyphrase, user information is valid only for keyphrase sound models */
    if (common_sm->type == SOUND_MODEL_TYPE_KEYPHRASE)
        update_available_phrase_info(st_session, phrase_sm, false);

    /* Add the session to the list of registered sound models list */
    list_add_tail(&stdev->sound_model_list, &st_session->list_node);

    pthread_mutex_unlock(&stdev->lock);

    ATRACE_END();
    ALOGD("%s: success, sound_model_handle %d", __func__, st_session->sm_handle);
    return 0;

exit_2:
    if (st_session->sm_data != NULL)
        free(st_session->sm_data);
    st_session_deinit(st_session);

exit_1:
    android_atomic_dec(&stdev->session_id);

exit:
    if (st_session != NULL)
        free(st_session);
    if (handle != NULL)
        *handle = -1;
    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    return status;
}

static int stdev_unload_sound_model(const struct sound_trigger_hw_device *dev,
                                    sound_model_handle_t handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    int status = 0, ret = 0;

    ALOGD("%s: Enter sound model %d", __func__, handle);
    ATRACE_BEGIN("sthal: stdev_unload_sound_model");

    pthread_mutex_lock(&stdev->lock);

    st_session = get_sound_trigger_session(stdev, handle);
    if (!st_session) {
        ALOGE("%s: Could not find sound model %d", __func__, handle);
        ret = -EINVAL;
        goto exit;
    }

    if (st_session->callback) {
        status = stop_recognition_l(st_session);
        if (status) {
            ALOGE("%s:[%d] Failed to stop recognition, status %d", __func__,
                handle, status);
            ret = status;
        }
    }

    status = st_session_unload_sm(st_session);
    if (status) {
        ALOGE("%s:[%d] Failed to unload sound model, status %d", __func__,
            handle, status);
        ret = status;
    }

    list_remove(&st_session->list_node);

    if (st_session->sm_type == SOUND_MODEL_TYPE_KEYPHRASE)
        update_available_phrase_info(st_session, st_session->sm_data, true);

    if (!get_num_sessions())
        stdev->exec_mode = ST_EXEC_MODE_NONE;

    pthread_mutex_lock(&st_session->lock);
    free(st_session->sm_data);
    pthread_mutex_unlock(&st_session->lock);

    st_session_deinit(st_session);
    pthread_mutex_destroy(&st_session->lock);

    free(st_session);

exit:
    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    ALOGD("%s: Exit status %d", __func__, ret);
    return ret;
}

static int stdev_start_recognition
(
   const struct sound_trigger_hw_device *dev,
   sound_model_handle_t sound_model_handle,
   const struct sound_trigger_recognition_config *config,
   recognition_callback_t callback,
   void *cookie
)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    st_session_t *st_session = NULL;
    int status = 0;

    ALOGD("%s:[%d] Enter", __func__, sound_model_handle);
    ATRACE_BEGIN("sthal: stdev_start_recognition");

    pthread_mutex_lock(&stdev->lock);

    if (!callback || !config) {
        ALOGE("%s: ERROR. NULL params", __func__);
        status = -EINVAL;
        goto exit;
    }

    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (!st_session) {
        ALOGE("%s: ERROR. Could not find session for sound model %d", __func__,
           sound_model_handle);
        status = -EINVAL;
        goto exit;
    }

    ALOGV("%s:[%d] About to take session lock", __func__, sound_model_handle);
    /* lock the session as we are about to change its
        stored parameters */
    pthread_mutex_lock(&st_session->lock);

    /* check if there is any change in config, if not issue restart event
       to state machine */
    if (st_session->rc_config &&
        compare_recognition_config(config, st_session->rc_config)) {

        ALOGV("%s: received identical params for session %d, restarting",
           __func__, st_session->sm_handle);

        status = st_session_restart(st_session);
        if (status)
            ALOGE("%s: failed to restart session", __func__);

    } else {

        ALOGV("%s: received new params for session %d", __func__,
            st_session->sm_handle);

        /* Store the recogntion configuration for sending opaque data
         * and for SSR, passing to sounmodel wrapper library.
         */
        if (st_session->rc_config)
            free(st_session->rc_config);

        st_session->rc_config = calloc(1, sizeof(*config) + config->data_size);
        if (!st_session->rc_config) {
            ALOGE("%s: No memory for rc_config", __func__);
            status = -ENOMEM;
            goto cleanup;
        }

        memcpy(st_session->rc_config, (char *)config, sizeof(*config));
        memcpy((char *)st_session->rc_config + st_session->rc_config->data_offset,
           (char *)config + config->data_offset, config->data_size);

        ALOGVV("%s: num_phrases=%d, id=%d", __func__, st_session->rc_config->num_phrases,
               st_session->rc_config->phrases[0].id);
        st_session->callback = callback;
        st_session->cookie = cookie;
        /* capture_handle will be shared with AHAL for reading LAB data */
        st_session->capture_handle = config->capture_handle;
        st_session->capture_requested = config->capture_requested;

        status = st_session_start(st_session);
        if (status)
            ALOGE("%s: failed to start session", __func__);
    }

cleanup:
    pthread_mutex_unlock(&st_session->lock);

exit:
    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    ALOGD("%s:[%d] Exit", __func__, sound_model_handle);
    return status;
}

static int stdev_stop_recognition(const struct sound_trigger_hw_device *dev,
                                 sound_model_handle_t sound_model_handle)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)dev;
    struct st_session *st_session = NULL;
    int status = 0;

    ALOGD("%s:[%d] Enter", __func__, sound_model_handle);
    ATRACE_BEGIN("sthal: stdev_stop_recognition");

    pthread_mutex_lock(&stdev->lock);

    st_session = get_sound_trigger_session(stdev, sound_model_handle);
    if (st_session == NULL) {
        ATRACE_END();
        ALOGE("%s: Could not find sound model %d", __func__, sound_model_handle);
        status = -EINVAL;
        pthread_mutex_unlock(&stdev->lock);
        return status;
    }

    /* If ftrt processing loop in progress, indicate to exit
       before acquiring the lock here */
    status = stop_recognition_l(st_session);

    pthread_mutex_unlock(&stdev->lock);
    ATRACE_END();
    ALOGD("%s:[%d] Exit status %d", __func__, sound_model_handle, status);
    return status;
}

static int stdev_close(hw_device_t *device)
{
    struct sound_trigger_device *stdev = (struct sound_trigger_device *)device;
    st_session_t *st_session = NULL;
    struct listnode *node = NULL, *tmp_node = NULL;
    int status = 0;

    ALOGD("%s: count=%d", __func__, stdev_ref_cnt);
    ATRACE_BEGIN("sthal: stdev_close");

    pthread_mutex_lock(&stdev_init_lock);
    if (!stdev || (--stdev_ref_cnt != 0)) {
        goto exit;
    }

    pthread_mutex_lock(&stdev->lock);
    platform_stdev_deinit(stdev->platform);
    free(stdev->arm_pcm_use_cases);
    if (!stdev->is_gcs)
        free(stdev->cpe_pcm_use_cases);
    free(stdev->ape_pcm_use_cases);
    free(stdev->dev_ref_cnt);
    list_for_each_safe(node, tmp_node, &stdev->sound_model_list) {
        st_session = node_to_item(node, st_session_t, list_node);
        list_remove(node);
        st_session_stop_lab(st_session);
        st_session_stop(st_session);
        st_session_deinit(st_session);
        free(st_session->sm_data);
        free(st_session->rc_config);
        free(st_session);
    }

    pthread_mutex_unlock(&stdev->lock);

    if (stdev->transit_to_adsp_on_playback) {
        stdev->stop_transitions_thread_loop = true;
        pthread_cond_signal(&stdev->transitions_cond);
        status = pthread_join(stdev->transitions_thread, NULL);
        if (status)
            ALOGE("%s: Error joining transitions thread. status = %d",
                __func__, status);
    }

    pthread_mutex_destroy(&stdev->lock);
    pthread_mutex_destroy(&stdev->ref_cnt_lock);
    free(device);
    stdev = NULL;

exit:
    pthread_mutex_unlock(&stdev_init_lock);
    ATRACE_END();
    ALOGD("%s: Exit device=%p cnt=%d ", __func__, stdev, stdev_ref_cnt);
    return 0;
}

static int stdev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    int status = 0;

    ALOGD("%s: Enter", __func__);
    ATRACE_BEGIN("sthal: stdev_open");

    if (strcmp(name, SOUND_TRIGGER_HARDWARE_INTERFACE) != 0) {
        ALOGE("%s: ERROR. wrong interface", __func__);
        status = -EINVAL;
        goto exit;
    }

    pthread_mutex_lock(&stdev_init_lock);
    if (stdev_ref_cnt != 0) {
        *device = &stdev->device.common;
        stdev_ref_cnt++;
        ATRACE_END();
        ALOGD("%s: returning existing stdev instance, exit", __func__);
        pthread_mutex_unlock(&stdev_init_lock);
        return status;
    }

    stdev = calloc(1, sizeof(struct sound_trigger_device));
    if (!stdev) {
        ALOGE("%s: ERROR. stdev alloc failed", __func__);
        status = -ENOMEM;
        goto exit;
    }

    stdev->hw_properties = &hw_properties;

    status = load_audio_hal();
    if (status)
        goto exit;

    stdev->platform = platform_stdev_init(stdev);
    if (!stdev->platform) {
        ALOGE("%s: ERROR. platform init failed", __func__);
        status = -ENODEV;
        goto exit_1;
    }

    stdev->ape_pcm_use_cases =
        calloc(stdev->max_ape_sessions, sizeof(struct use_case_info));

    if (!stdev->ape_pcm_use_cases) {
        ALOGE("%s: ERROR. Mem alloc failed for ape use cases", __func__);
        status = -ENODEV;
        goto exit_1;
    }

    if (!stdev->is_gcs) {
        stdev->cpe_pcm_use_cases =
            calloc(stdev->max_cpe_sessions, sizeof(struct use_case_info));

        if (!stdev->cpe_pcm_use_cases) {
            ALOGE("%s: ERROR. Mem alloc failed for cpe use cases", __func__);
            status = -ENODEV;
            goto exit_1;
        }
    }

    /* Each arm session is associated with corresponding ec ref session */
    stdev->arm_pcm_use_cases =
        calloc(stdev->max_arm_sessions * 2, sizeof(struct use_case_info));

    if (!stdev->arm_pcm_use_cases) {
        ALOGE("%s: ERROR. Mem alloc failed for capture use cases", __func__);
        status = -ENODEV;
        goto exit_1;
    }

    stdev->dev_ref_cnt =
        calloc(ST_EXEC_MODE_MAX * ST_DEVICE_MAX, sizeof(int));

    if (!stdev->dev_ref_cnt) {
        ALOGE("%s: ERROR. Mem alloc failed dev ref cnt", __func__);
        status = -ENOMEM;
        goto exit_1;
    }

    stdev->device.common.tag = HARDWARE_DEVICE_TAG;
    stdev->device.common.version = SOUND_TRIGGER_DEVICE_API_VERSION_1_0;
    stdev->device.common.module = (struct hw_module_t *) module;
    stdev->device.common.close = stdev_close;
    stdev->device.get_properties = stdev_get_properties;
    stdev->device.load_sound_model = stdev_load_sound_model;
    stdev->device.unload_sound_model = stdev_unload_sound_model;
    stdev->device.start_recognition = stdev_start_recognition;
    stdev->device.stop_recognition = stdev_stop_recognition;
    stdev->session_id = 1;
    stdev->gcs_token = 1;
    stdev->exec_mode = ST_EXEC_MODE_MAX;
    stdev->capture_device = AUDIO_DEVICE_IN_BUILTIN_MIC;
    stdev->available_devices = AUDIO_DEVICE_IN_BUILTIN_MIC;
    stdev->client_req_exec_mode = ST_EXEC_MODE_NONE;
    stdev->ec_ref_dev = AUDIO_DEVICE_OUT_SPEAKER;

    pthread_mutex_init(&stdev->lock, (const pthread_mutexattr_t *) NULL);
    pthread_mutex_init(&stdev->ref_cnt_lock, (const pthread_mutexattr_t*)NULL);
    list_init(&stdev->sound_model_list);

    if (stdev->transit_to_adsp_on_playback)
        init_transitions_thread();
    dbg_trace_parse_max_lab_reads(); /*For trace log control*/
    *device = &stdev->device.common;
    stdev_ref_cnt++;
    pthread_mutex_unlock(&stdev_init_lock);

    ATRACE_END();
    return 0;

exit_1:
    if (stdev->arm_pcm_use_cases)
        free(stdev->arm_pcm_use_cases);

    if (stdev->cpe_pcm_use_cases)
        free(stdev->cpe_pcm_use_cases);

    if (stdev->ape_pcm_use_cases)
        free(stdev->ape_pcm_use_cases);

    if (stdev->audio_hal_handle)
        dlclose(stdev->audio_hal_handle);

    if (stdev->platform)
       platform_stdev_deinit(stdev->platform);

exit:

    if (stdev)
        free(stdev);
    stdev = NULL;
    *device = NULL;

    pthread_mutex_unlock(&stdev_init_lock);
    ATRACE_END();
    ALOGD("%s: Exit status %d", __func__, status);
    return status;
}

/* Audio hal calls this callback for notifying Subsystem restart,
   lab stop and concurrency events */
int sound_trigger_hw_call_back(audio_event_type_t event,
                               audio_event_info_t* config)
{
    struct listnode *p_ses_node;
    st_session_t *p_ses = NULL;
    int ret = 0;
    size_t bytes_read = 0;
    st_exec_mode_t exec_mode;
    sound_model_handle_t sm_handle;

    if (!stdev)
        return -ENODEV;

    switch (event) {
    case AUDIO_EVENT_CAPTURE_DEVICE_INACTIVE:
    case AUDIO_EVENT_CAPTURE_DEVICE_ACTIVE:
    case AUDIO_EVENT_PLAYBACK_STREAM_INACTIVE:
    case AUDIO_EVENT_PLAYBACK_STREAM_ACTIVE:
        ATRACE_BEGIN("sthal: handle_audio_concurrency");
        handle_audio_concurrency(event);
        ATRACE_END();
        break;

    case AUDIO_EVENT_CAPTURE_STREAM_INACTIVE:
    case AUDIO_EVENT_CAPTURE_STREAM_ACTIVE:
        if (!config) {
            ALOGE("%s: NULL capture stream event config", __func__);
            ret = -EINVAL;
        } else {
            if (!platform_stdev_check_concurrent_capture(stdev->platform,
                                                 config->u.usecase.type))
                handle_audio_concurrency(event);
        }
        break;

    case AUDIO_EVENT_STOP_LAB:
        if (!config || !config->u.ses_info.p_ses) {
            ALOGE("%s: NULL params for stop lab", __func__);
            ret = -EINVAL;
            break;
        }
        ATRACE_BEGIN("sthal: event_stop_lab");
        p_ses = config->u.ses_info.p_ses;
        pthread_mutex_lock(&stdev->lock);

        /* check capture_handle to ensure pointer sanity */
        list_for_each(p_ses_node, &stdev->sound_model_list) {
            p_ses = node_to_item(p_ses_node, st_session_t, list_node);
            if (p_ses->capture_handle == config->u.ses_info.capture_handle) {
                st_session_stop_lab(p_ses);
                break;
            }
        }
        /*
         * If no other session is buffering and device switch is pending,
         * switch the device
         */
        if (!is_any_session_buffering() &&
            (platform_stdev_get_capture_device(p_ses->stdev->platform) !=
             stdev->capture_device)) {
            ALOGD("%s: switching device after buffering is completed", __func__);
            switch_device();
        }
        pthread_mutex_unlock(&stdev->lock);
        ATRACE_END();
        break;

    case AUDIO_EVENT_SSR:
        if (!config) {
            ALOGE("%s: NULL config for SSR", __func__);
            ret = -EINVAL;
            break;
        }

        if (config->u.status == SND_CARD_STATUS_OFFLINE ||
            config->u.status == CPE_STATUS_OFFLINE)
            handle_ssr_offline(config->u.status);

        if (config->u.status == SND_CARD_STATUS_ONLINE ||
            config->u.status == CPE_STATUS_ONLINE)
            handle_ssr_online(config->u.status);

        break;
    case AUDIO_EVENT_NUM_ST_SESSIONS:
        if (!config) {
            ALOGE("%s: NULL config for AUDIO_EVENT_NUM_ST_SESSIONS", __func__);
            ret = -EINVAL;
            break;
        }
        pthread_mutex_lock(&stdev->lock);
        ALOGV("%s: num sessions configured %d", __func__, config->u.value);
        stdev->num_sessions_configured = config->u.value;
        pthread_mutex_unlock(&stdev->lock);
        break;

    case AUDIO_EVENT_READ_SAMPLES:
        if (!config) {
            ALOGE("%s: Invalid config", __func__);
            ret = -EINVAL;
            break;
        }
        p_ses = (st_session_t*)config->u.aud_info.ses_info->p_ses;
        ret = st_session_read_pcm(p_ses, config->u.aud_info.buf,
            config->u.aud_info.num_bytes,
            &bytes_read);
        break;

    case AUDIO_EVENT_DEVICE_CONNECT:
        if (!config) {
            ALOGE("%s: NULL config for DEVICE_CONNECT", __func__);
            ret = -EINVAL;
            break;
        }
        handle_device_switch(true, config);
        break;

    case AUDIO_EVENT_DEVICE_DISCONNECT:
        if (!config) {
            ALOGE("%s: NULL config for DEVICE_DISCONNECT", __func__);
            ret = -EINVAL;
            break;
        }
        handle_device_switch(false, config);
        break;

    case AUDIO_EVENT_SVA_EXEC_MODE:
        pthread_mutex_lock(&stdev->lock);
        if (!config) {
            ALOGE("%s: NULL config for AUDIO_EVENT_SVA_EXEC_MODE", __func__);
            ret = -EINVAL;
            goto error;
        }
        ret = parse_exec_mode_config(config->u.str_value, &exec_mode, &sm_handle);
        if (ret) {
            ALOGE("%s: Failed to parse exec mode config", __func__);
            goto error;
        }

        if (sm_handle) {
            p_ses = get_sound_trigger_session(stdev, sm_handle);
            if (p_ses == NULL) {
                ALOGE("%s: Could not find sound model %d", __func__, sm_handle);
                ret = -EINVAL;
                goto error;
            }
        }
        ALOGV("%s:[%d] set exec mode %d", __func__, sm_handle, exec_mode);
        if (exec_mode == ST_EXEC_MODE_ADSP) {
            ret = check_and_transit_cpe_ses_to_ape(p_ses);
            if (!sm_handle)
                stdev->client_req_exec_mode = exec_mode;
        } else if (exec_mode == ST_EXEC_MODE_CPE) {
            ret = check_and_transit_ape_ses_to_cpe(p_ses);
            if (!sm_handle)
                stdev->client_req_exec_mode = exec_mode;
        } else {
            ALOGE("%s: Invalid exec mode %d", __func__, exec_mode);
            ret = -EINVAL;
        }

    error:
        /* store exec mode callback status to return when queried */
        stdev->client_req_exec_mode_status = ret;
        pthread_mutex_unlock(&stdev->lock);
        break;

    case AUDIO_EVENT_SVA_EXEC_MODE_STATUS:
        if (!config) {
            ALOGE("%s: NULL config for AUDIO_EVENT_SVA_EXEC_MODE", __func__);
            ret = -EINVAL;
            break;
        }
        config->u.value = stdev->client_req_exec_mode_status;
        ALOGV("%s: exec mode status %d", __func__, config->u.value);
        break;

    default:
        ALOGW("%s: Unknown event %d", __func__, event);
        break;
    }

    return ret;
}

/*
 * Current proprietary API version used by STHAL. Queried by AHAL
 * for compatibility check with STHAL
 */
const unsigned int sthal_prop_api_version = STHAL_PROP_API_CURRENT_VERSION;

static struct hw_module_methods_t hal_module_methods = {
    .open = stdev_open,
};

struct sound_trigger_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SOUND_TRIGGER_MODULE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SOUND_TRIGGER_HARDWARE_MODULE_ID,
        .name = "Sound trigger HAL",
        .author = "QUALCOMM Technologies, Inc",
        .methods = &hal_module_methods,
    },
};
