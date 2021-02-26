/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
* Copyright (c) 2016, The Linux Foundation. All rights reserved.
* Not a Contribution.
*
* Copyright (C) 2015 The Android Open Source Project *
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cutils/atomic.h>
#include <pthread.h>

#include "logfile.h"
#include "IQvopCallback.h"
#include "QvopService.h"
#include "qahw_api.h"
#include "qahw_defs.h"
#include "TestCase.h"
#include "TestCaseCallbacks.h"
#include "TestCaseUtility.h"
#include "logfile.h"
#include "qsthw_api.h"
#include "qsthw_defs.h"

using namespace std;
using namespace qvop;

#define OK 0
#define SM_MINOR_VERSION 1
#define SM_FILE_PATH_MAX_LENGTH 128
#define QVOP_MAX_BYTES_TO_COPY 320

static const sound_trigger_uuid_t qc_uuid =
    { 0x68ab2d40, 0xe860, 0x11e3, 0x95ef, { 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } };

struct sense_id_data {
    QvopService service;
    string user_id;
    string identifier;
    bool expected;
};

struct sm_session_data {
    sound_trigger_uuid_t vendor_uuid;
    string sm_file_path;
    sound_model_handle_t sm_handle;
    unsigned int num_of_keywords;
    unsigned int kw_conf_level;
    unsigned int num_of_users;
    unsigned int usr_conf_level;
    unsigned int kb_duration_ms;
    struct sense_id_data sense_id;
    int session_id;
    struct sound_trigger_recognition_config rc_config;
    struct qsthw_phrase_recognition_event qsthw_event;
};

struct keyword_buffer_config {
    int version;
    uint32_t kb_duration;
}__packed;

const qsthw_module_handle_t *st_mod_handle;
static struct sm_session_data sound_trigger_info;
static void *event_handler_thread(void *);
static  QvopStatus capture_match_kw_data(QvopService& service, struct qsthw_phrase_recognition_event *qsthw_event);

class TestCaseMatchSVAInputWithId : public TestCase {
public:
    TestCaseMatchSVAInputWithId() : user_id(DEFAULT_MATCH_USER_ID),
        identifier(DEFAULT_MATCH_IDENTIFIER),
        kb_duration_ms(DEFAULT_KB_DURATION),
        num_of_keywords(DEFAULT_NO_OF_KEYWORDS),
        kw_conf_level(DEFAULT_KW_CONF_LEVEL),
        num_of_users(DEFAULT_NO_OF_USERS),
        usr_conf_level(DEFAULT_USER_CONF_LEVEL),
        sm_file_path(DEFAULT_SOUND_MODEL_FILE) {};

    virtual ~TestCaseMatchSVAInputWithId() { }

static struct sm_session_data *get_sound_trigger_info(sound_model_handle_t sm_handle)
{
    if (sound_trigger_info.sm_handle == sm_handle)
         return &sound_trigger_info;
    return NULL;
}

static void eventCallback(struct sound_trigger_recognition_event *event, void *sessionHndl)
{
    int rc;
    pthread_attr_t attr;
    pthread_t callback_thread;
    sound_model_handle_t sm_handle = event->model;
    struct qsthw_phrase_recognition_event *qsthw_event;
    uint64_t event_timestamp;

    logfile_print_d("[%d] Callback event received: %d\n", event->model, event->status);
    qsthw_event = (struct qsthw_phrase_recognition_event *)event;
    event_timestamp = qsthw_event->timestamp;
    logfile_print_d("[%d] Event detection timestamp %llu\n", sm_handle, event_timestamp);
    rc = pthread_attr_init(&attr);
    if (rc != 0) {
        logfile_print_e("pthread attr init failed %d\n",rc);
        return;
    }
    struct sm_session_data *sm_data = get_sound_trigger_info(sm_handle);
    if (sm_data == NULL) {
        logfile_print_e("Error: Invalid sound model handle %d\n", sm_handle);
        return;
    }
    memcpy(&sm_data->qsthw_event, qsthw_event,
        sizeof(struct qsthw_phrase_recognition_event));

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    rc = pthread_create(&callback_thread, &attr,
                        event_handler_thread, sm_data);
    if (rc != 0)
        logfile_print_e("event_handler_thread create failed %d\n",rc);
    pthread_attr_destroy(&attr);
}

static void *event_handler_thread(void *context)
{
    QvopStatus status;
    QvopService service;
    struct sm_session_data *sm_data = (struct sm_session_data *) context;
    if (!sm_data) {
        logfile_print_e("Error: context is null\n");
        return NULL;
    }
    service = sm_data->sense_id.service;
    struct sound_trigger_recognition_config *rc_config = &sm_data->rc_config;
    struct qsthw_phrase_recognition_event *qsthw_event =
                                                       &sm_data->qsthw_event;
    sound_model_handle_t sm_handle = sm_data->sm_handle;
    logfile_print_v("[%d] session params %p, %d\n", sm_handle,
                        rc_config, sound_trigger_info.kb_duration_ms);

    if (qsthw_event && qsthw_event->phrase_event.common.capture_available) {
        status =  capture_match_kw_data(service, qsthw_event);
        if (status == QVOP_FAILURE)
            logfile_print_e("capturing or matching of keyword data failed");
    }

    /* ignore error */
    qsthw_start_recognition(st_mod_handle, sm_handle,
                            rc_config, eventCallback, NULL);
    logfile_print_e("proceed with utterance or command \n");
    logfile_print_e("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\n");
    return NULL;
}

static QvopStatus capture_match_kw_data(QvopService& service, struct qsthw_phrase_recognition_event *qsthw_event)
{
    int ret = 0;
    uint64_t frameId = 0;
    uint64_t timestamp = 0, status = 0;
    void *buffer;
    char kw_capture_file[128] = "";
    QvopStatus rv = QVOP_SUCCESS;
    unsigned int period_size;
    size_t bytes, bytes_to_copy, written;
    size_t cur_bytes_read = 0;
    size_t total_bytes_to_read;
    FILE *fp;
    struct stat st = {0};
    struct sound_trigger_phrase_recognition_event phrase_event =
                                                  qsthw_event->phrase_event;
    char* large_buf;
    char* temp1;
    sound_model_handle_t sm_handle = phrase_event.common.model;
    audio_config_t *audio_config = &phrase_event.common.audio_config;
    uint32_t sample_rate = audio_config->sample_rate;
    uint32_t channels =
             audio_channel_count_from_in_mask(audio_config->channel_mask);
    audio_format_t format = audio_config->format;
    size_t samp_sz = audio_bytes_per_sample(format);
    int read = 0;
    int bytes_read, sm_data_size;

    bytes = qsthw_get_buffer_size(st_mod_handle, sm_handle);
    if (bytes <= 0) {
        logfile_print_e("Invalid buffer size returned\n");
        return QVOP_FAILURE;
    }

    /* total bytes to read = bytes to read per sec * duration where
     * audio configuration passed as part of recognition event is
     * used to obtain bytes per sec.
     */

    total_bytes_to_read = ((sample_rate * channels * samp_sz) * sound_trigger_info.kb_duration_ms)/1000;
    large_buf = (char *)calloc(1, total_bytes_to_read);
    if (large_buf == NULL) {
        logfile_print_e("calloc failed!!\n");
        return QVOP_FAILURE;
    }
    char* temp = large_buf;
    logfile_print_v("sample rate %d, channels %d, samp sz %d, duration %d\n, total_bytes_to_read %d",
              sample_rate, channels, samp_sz, sound_trigger_info.kb_duration_ms, total_bytes_to_read);
    buffer = calloc(1, bytes);
    if (buffer == NULL) {
        logfile_print_e("Could not allocate memory for capture buffer\n");
        return QVOP_FAILURE;
    }

    if (stat("/data/SVA", &st) == -1)
        mkdir("/data/SVA", S_IRWXU | S_IRWXG | S_IRWXO);
    snprintf(kw_capture_file, sizeof(kw_capture_file),
             "/data/SVA/kw_capture_file_%d", sm_handle);
    fp = fopen(kw_capture_file, "wb");
    if (fp == NULL) {
        logfile_print_e("Could not open keyword capture file : %s\n", kw_capture_file);
        free(buffer);
        return QVOP_FAILURE;
    }

    logfile_print_e("keyword capture file : %s\n", kw_capture_file);
    memset(large_buf, 0, total_bytes_to_read);
    memset(buffer, 0, bytes);
    logfile_print_d("currnt = %d, total = %d", cur_bytes_read, total_bytes_to_read);
    while (cur_bytes_read < total_bytes_to_read) {
        read = qsthw_read_buffer(st_mod_handle, sm_handle, (unsigned char *) buffer, bytes);
        written = fwrite(buffer, 1, bytes, fp);
        if (written != bytes) {
            logfile_print_v("written %d, bytes %d\n", written, bytes);
            if (ferror(fp)) {
                logfile_print_e("Error writing Keyword capture data into file %s\n",strerror(errno));
                break;
            }
        }
        if ((total_bytes_to_read - cur_bytes_read) < bytes)
            bytes_to_copy = (total_bytes_to_read - cur_bytes_read);
        else
            bytes_to_copy = bytes;
        memcpy(temp, buffer, bytes_to_copy);
        cur_bytes_read += bytes_to_copy;
        temp += bytes_to_copy;
        memset(buffer, 0, bytes);
    }
    qsthw_stop_buffering(st_mod_handle, sm_handle);
    temp = large_buf;
    free(buffer);

    /* calling matchWithId initializes the state of the secure module to accept
    *   the match request against a particular identifier for the user.
    *   A failed call to matchWithId is a test case failure
    *   because the secure environment would not be ready to accept the
    *   audio data for the match processing.
    *   The secure module is typically an environment like Trustzone, but could be
    *   something else.
    */
    TestCaseCallbackMatch cb(true);
    if (service.matchWithId(cb, sound_trigger_info.sense_id.user_id.c_str(),
            sound_trigger_info.sense_id.identifier.c_str()) != QVOP_SUCCESS ||
                  cb.GetStatus() == CBFailure) {
        logfile_print_e("%s: failed to call matchWithId", QVOP_FN);
        return QVOP_FAILURE;
    }

    TestCaseCallbackProcessFrame pfcb(true);
    buffer = calloc(1, QVOP_MAX_BYTES_TO_COPY);
    while (total_bytes_to_read > 0) {
        bytes_to_copy = (bytes <= total_bytes_to_read) ? bytes : total_bytes_to_read;
        //TODO: need to revisit this once it is clear why qvop_service does not accept larger buffer size
        if (bytes_to_copy > QVOP_MAX_BYTES_TO_COPY)
            bytes_to_copy = QVOP_MAX_BYTES_TO_COPY;
        memset(buffer, 0, QVOP_MAX_BYTES_TO_COPY);
        memcpy(buffer, temp, bytes_to_copy);
        temp += bytes_to_copy;
        total_bytes_to_read -= bytes_to_copy;
        logfile_print_v(" bytes: %d, bytes_to_copy: %d, total_bytes: %d",
                                     bytes, bytes_to_copy, total_bytes_to_read);
        rv = service.processFrameWithId(pfcb, timestamp++, frameId++, bytes_to_copy, (const int8_t*)buffer);
        //it is possible for the processFrame call to result in a match completion, success or failure
        if (pfcb.IsComplete()) {
            logfile_print_d("%s: IsComplete is true, rv=%d", QVOP_FN, rv);
            if (pfcb.GetStatus() == CBSuccess) {
                QvopStatus answer = pfcb.GetMatch();
                logfile_print_d("%s: Match answer = %s", QVOP_FN, answer == QVOP_MATCH_SUCCESS ?
                            "Match" : "No Match");
                rv = answer;
            }
            else {
                rv = QVOP_FAILURE;
            }
            break;
        }
    }

    free(buffer);
    free(large_buf);
    fclose(fp);

    if (rv != QVOP_SUCCESS && rv != QVOP_MATCH_SUCCESS &&
                    rv != QVOP_MATCH_FAILURE) {
        logfile_print_i("%s: voice capture returned status: %d", QVOP_FN, rv);
        return QVOP_FAILURE;
    }

    if (rv == QVOP_MATCH_SUCCESS || rv == QVOP_MATCH_FAILURE) {
        logfile_print_i("%s: match Complete - result: %s expected: %s", QVOP_FN,
                 rv == QVOP_MATCH_SUCCESS ? "QVOP_MATCH_SUCCESS" : "QVOP_MATCH_FAILURE",
                 sound_trigger_info.sense_id.expected ? "Match" : "NoMatch");
        if ((sound_trigger_info.sense_id.expected && (rv == QVOP_MATCH_SUCCESS)) ||
            (!sound_trigger_info.sense_id.expected && (rv == QVOP_MATCH_FAILURE)))
            return QVOP_SUCCESS;
       else
            return QVOP_FAILURE;
    }

    if (service.matchCaptureComplete(cb) != QVOP_SUCCESS) {
        logfile_print_e("%s: failed to call matchCaptureComplete", QVOP_FN);
        return QVOP_FAILURE;
    }

    return QVOP_SUCCESS;
}

virtual TestCaseResult Run(QvopService& service, vector<string> const& params) {
    int status = 0, j, i, sound_model_size = 0, sm_data_size  = 0;
    int id = num_of_keywords; //user_id should start from num_kws
    bool user_verification = false;
    struct sound_trigger_recognition_config *rc_config = NULL;
    uint32_t rc_config_size, opaque_data_size = 0;
    struct sound_trigger_phrase_sound_model *sound_model = NULL;
    FILE *fp = NULL;
    const char *sm_file;
    char command[128];
    int bytes_read;

    if (ConvertParams(params) == false) {
        logfile_print_e("%s: failed to convert params ", QVOP_FN);
        return TCBadParams;
    }
    sound_trigger_info.sense_id.user_id = user_id;
    sound_trigger_info.sense_id.identifier = identifier;
    sound_trigger_info.sense_id.expected = expected;
    sound_trigger_info.sense_id.service = service;
    sound_trigger_info.sm_file_path = sm_file_path.c_str();
    sound_trigger_info.num_of_keywords = num_of_keywords;
    sound_trigger_info.kw_conf_level = kw_conf_level;
    sound_trigger_info.num_of_users = num_of_users;
    sound_trigger_info.usr_conf_level = usr_conf_level;
    sound_trigger_info.kb_duration_ms = kb_duration_ms;
    sound_trigger_info.vendor_uuid = qc_uuid;
    logfile_print_e("kb_duration: %d", kb_duration_ms);

    sm_file = sound_trigger_info.sm_file_path.c_str();
    struct keyword_buffer_config kb_config;
    st_mod_handle = qsthw_load_module(QSTHW_MODULE_ID_PRIMARY);
    if (NULL == st_mod_handle) {
        logfile_print_e("qsthw_load_module() failed\n");
        return TCFailure;
    }
    unsigned int num_of_keywords = sound_trigger_info.num_of_keywords;
    unsigned int num_of_users =
               user_verification ? sound_trigger_info.num_of_users : 0;
    sound_model_handle_t sm_handle = 0;

    fp = fopen(sm_file, "rb");
    if (fp == NULL) {
        logfile_print_e("Could not open sound model file\n");
        goto error_1;
    }
    fseek( fp, 0, SEEK_END);
    sm_data_size  = ftell(fp);
    fseek( fp, 0, SEEK_SET);
    sound_model_size = sizeof(struct sound_trigger_phrase_sound_model) + sm_data_size;
    sound_model = (struct sound_trigger_phrase_sound_model *)calloc(1, sound_model_size);
    if (sound_model == NULL) {
        logfile_print_e("Could not allocate memory for sound model");
        goto error_1;
    }
    sound_model->common.type = SOUND_MODEL_TYPE_KEYPHRASE;
    sound_model->common.data_size = sm_data_size;
    sound_model->common.data_offset = sizeof(*sound_model);
    sound_model->num_phrases = num_of_keywords;
    for (i = 0; i < num_of_keywords; i++) {
        sound_model->phrases[i].num_users = num_of_users;
        if (user_verification)
            sound_model->phrases[i].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER |
                                  RECOGNITION_MODE_USER_IDENTIFICATION;
        else
            sound_model->phrases[i].recognition_mode = RECOGNITION_MODE_VOICE_TRIGGER;
    }
    bytes_read = fread((char*)sound_model+sound_model->common.data_offset , 1, sm_data_size , fp);
    logfile_print_e("bytes from the file %d\n", bytes_read);
    if (bytes_read != sm_data_size) {
        logfile_print_e("Something wrong while reading data from file: bytes_read %d file_size %d", bytes_read, sm_data_size);
        goto error_1;
    }
    memcpy(&sound_model->common.vendor_uuid, &sound_trigger_info.vendor_uuid,
             sizeof(sound_trigger_uuid_t));
    logfile_print_e("sound model data_size %d data_offset %d\n", sm_data_size, sound_model->common.data_offset);
    status = qsthw_load_sound_model(st_mod_handle, &sound_model->common, NULL, NULL, &sm_handle);
    if (OK != status) {
        logfile_print_e("load_sound_model failed\n");
        goto error_1;
    }

    opaque_data_size = sizeof(struct keyword_buffer_config);
    rc_config_size = sizeof(struct sound_trigger_recognition_config) + opaque_data_size;
    rc_config = (struct sound_trigger_recognition_config *)calloc(1, rc_config_size);
    if (rc_config == NULL) {
        logfile_print_e("Could not allocate memory for recognition config");
        goto error_2;
    }
    rc_config->capture_handle = AUDIO_IO_HANDLE_NONE;
    rc_config->capture_device = AUDIO_DEVICE_NONE;
    rc_config->capture_requested = true;
    rc_config->num_phrases = num_of_keywords;

    kb_config.version = SM_MINOR_VERSION;
    kb_config.kb_duration = sound_trigger_info.kb_duration_ms;
    memcpy((char *) rc_config + rc_config->data_offset, &kb_config, sizeof(struct keyword_buffer_config));
    rc_config->data_size = sizeof(struct keyword_buffer_config);
    rc_config->data_offset = sizeof(struct sound_trigger_recognition_config);

    for (i = 0; i < num_of_keywords; i++) {
        rc_config->phrases[i].id = i;
        rc_config->phrases[i].confidence_level = sound_trigger_info.kw_conf_level;
        rc_config->phrases[i].num_levels = num_of_users;
        for (j = 0; j < num_of_users; j++) {
            rc_config->phrases[i].levels[j].level = sound_trigger_info.usr_conf_level;
            rc_config->phrases[i].levels[j].user_id = id++;
        }
        if (user_verification)
            rc_config->phrases[i].recognition_modes = RECOGNITION_MODE_VOICE_TRIGGER |
                    RECOGNITION_MODE_USER_IDENTIFICATION;
        else
            rc_config->phrases[i].recognition_modes = RECOGNITION_MODE_VOICE_TRIGGER;
    }
    sound_trigger_info.sm_handle = sm_handle;
    memcpy(&sound_trigger_info.rc_config, rc_config, sizeof(*rc_config));
    logfile_print_e("session params %p, %p, %d\n", &sound_trigger_info, rc_config, sm_handle);

    status = qsthw_start_recognition(st_mod_handle, sm_handle,
                 rc_config, eventCallback, NULL);
    if (OK != status) {
        logfile_print_e("start_recognition failed\n");
        goto error_3;
    }

    do {
        logfile_print_e("enter command exit");
        fgets(command, 128, stdin);
        if (!strncmp(command, "exit", 4))
            break;
    } while(true);

error_3:
    status = qsthw_stop_recognition(st_mod_handle, sm_handle);
    if (OK != status)
        logfile_print_e("stop_recognition failed\n");
error_2:
    status = qsthw_unload_sound_model(st_mod_handle, sm_handle);
    if (OK != status)
        logfile_print_e("unload_sound_model failed\n");
error_1:
    status = qsthw_unload_module(st_mod_handle);
    if (OK != status)
        logfile_print_e("qsthw_unload_module failed, status %d\n", status);

    if (rc_config)
        free(rc_config);
    if (sound_model)
        free(sound_model);
    if (fp)
        fclose(fp);

    return TCSuccess;
}

protected:
    /*
    * KEY_MATCH_USERID : --user_id
    *                  : name of user
    * KEY_MATCH_IDENTIFIER : --identifier
    *                      : name of identifier
    * KEY_KB_DURATION : --kb_duration
    *                 : number of milliseconds for keyword to get buffered
    * KEY_MATCH_EXPECTED || KEY_MATCH_NOT_EXPECTED
    */

virtual bool ConvertParams(vector<string> const& params)
{
    logfile_print_e("%s: Convert params entered ", QVOP_FN);
    for (auto it = begin(params); it != end(params); ++it) {
        if (it->compare(KEY_IDENTIFIER) == 0) {
            //value is in next item
            if (++it != end(params)) {
                if (it->find(PARAM_INDICATOR) == string::npos)
                    identifier = (it->c_str());
            }
        }
        else if (it->compare(KEY_USERID) == 0) {
            //value is in next item
            if (++it != end(params))
                user_id = (it->c_str());
        }
        else if (it->compare(KEY_KB_DURATION) == 0) {
            //value is in next item
            if (++it != end(params))
                kb_duration_ms = atoi(it->c_str());
        }
        else if (it->compare(KEY_NUM_OF_KEYWORDS) == 0) {
            //value is in next item
            if (++it != end(params))
                num_of_keywords = atoi(it->c_str());
        }
        else if (it->compare(KEY_KW_CONF_LEVEL) == 0) {
            //value is in next item
            if (++it != end(params))
                kw_conf_level = atoi(it->c_str());
        }
        else if (it->compare(KEY_MATCH_EXPECTED) == 0) {
            logfile_print_i("%s: expected result:KEY_EXPECTED", QVOP_FN);
            expected = true;
        }
        else if (it->compare(KEY_MATCH_NOT_EXPECTED) == 0) {
            logfile_print_i("%s: expected result:KEY_NOT_EXPECTED", QVOP_FN);
            expected = false;
        }
        else if (it->compare(KEY_NO_OF_USERS) == 0) {
            //value is in next item
            if (++it != end(params))
                num_of_users = atoi(it->c_str());
        }
        else if (it->compare(KEY_USER_CONF_LEVEL) == 0) {
            //value is in next item
            if (++it != end(params))
                usr_conf_level = atoi(it->c_str());
        }
        else if (it->compare(KEY_SM_FILE_PATH)) {
            if (++it != end(params))
                sm_file_path = (it->c_str());
        }
        else {
            logfile_print_w("%s: unknown parameter: %s", QVOP_FN, it->c_str());
        }
        //since the code above inc/dec the it pointer, there is a need
        //for an end check here before the for loop increments it again.
        if (it == end(params)){
            break;
        }
    }
    logfile_print_e("%s: user_id:%s identifier: %s kb_duration: %d, sm_file_path: %s",
          QVOP_FN, user_id.c_str(), identifier.c_str(), kb_duration_ms, sm_file_path.c_str());

    if (user_id == string(DEFAULT_MATCH_USER_ID) ||
                 identifier == string(DEFAULT_MATCH_IDENTIFIER) ) {
        logfile_print_e("%s: need to init params", QVOP_FN);
        return false;
    }

    return true;
}
protected:
    string user_id;
    string identifier;
    string sm_file_path;
    unsigned int num_of_keywords;
    unsigned int kw_conf_level;
    unsigned int num_of_users;
    unsigned int usr_conf_level;
    unsigned int kb_duration_ms;
    bool expected;
};
