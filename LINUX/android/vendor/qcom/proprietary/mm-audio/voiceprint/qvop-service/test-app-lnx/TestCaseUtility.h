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

#ifndef TEST_CASE_UTILITY_H
#define TEST_CASE_UTILITY_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "logfile.h"
#include "IQvopCallback.h"
#include "QvopService.h"
#include "qahw_api.h"
#include "qahw_defs.h"

static char const* const QVOP_DIRECTORY = "/data/misc/qvop/";
static char const* const QVOP_LOG_FILE = "/data/misc/qvop/qvop-log.txt";

#define NUM_PKT_SAMPLES (10*16)
#define BLOCK_LENGTH NUM_PKT_SAMPLES

#define SOUNDFOCUS_PARAMS "SoundFocus.start_angles;SoundFocus.enable_sectors;" \
                          "SoundFocus.gain_step"
#define SOURCETRACK_PARAMS "SourceTrack.vad;SourceTrack.doa_speech;SourceTrack.doa_noise;"\
                           "SourceTrack.polar_activity;ssr.noise_level;ssr.noise_level_after_ns"

using namespace qvop;
using namespace std;

int sourcetrack_done;
int tests_running;
bool gerror;

typedef struct qvop_user_input_t {
    qvop_user_input_t(int st, int id, int ch, int fmt, int sr, int src, int lt) {
        source_tracking = st;
        input_device = id;
        channels = ch;
        format = fmt;
        sample_rate = sr;
        source = src;
        looptime = lt;
    }
    int source_tracking;
    int input_device;
    int channels;
    int format;
    uint32_t sample_rate;
    int source;
    int looptime;
} qvop_user_input;

struct audio_config_params {
    qahw_module_handle_t *qahw_mod_handle;
    audio_io_handle_t handle;
    audio_devices_t input_device;
    audio_config_t config;
    audio_input_flags_t flags;
    const char* kStreamName;
    audio_source_t kInputSource;
    char output_filename[256];
    double loopTime;
    char profile[50];
};

class TestCaseUtility : public IQvopCallback {
public:
    TestCaseUtility() {};
    ~TestCaseUtility() {};

    /*
    * borrowed from old test app
    */
    static QvopStatus send_pcm_file(QvopService& service, TestCaseCallbackProcessFrame& cb, const char *file_name)
    {
        struct qvop_pcm_packet pcmPkt;
        FILE *audio_fp;
        size_t blockIndex, num_audio_sample, numAudioBlock, file_size;
        size_t header_size_to_ignore = 0;
        uint64_t frameId = 0;
        uint64_t timestamp = 0;
        QvopStatus rv = QVOP_SUCCESS;
        int8_t run_adapt = 1;
        char header_check[4];
        const char *audio_file = file_name;
        int16_t *audio_buffer;
        size_t sample_size = AUDIO_SAMPLE_DEFAULT_SIZE;
        size_t block_len = BLOCK_LENGTH;

        pcmPkt.num_pcm_samples = NUM_PKT_SAMPLES;//30 ms sec worth of data
        //pcmPkt.pcm_data = (int16_t *)malloc(NUM_PKT_SAMPLES * sample_size);
        pcmPkt.eos_flag = QVOP_PCM_NO_EOS;

        audio_fp = fopen(audio_file, "rb");
        if (audio_fp == NULL) {
            logfile_print_e("Input audio file cannot be opened: %s", file_name);
            return (QVOP_FAILURE);
        }

        // check audio file size
        fseek(audio_fp, 0, SEEK_END);
        file_size = ftell(audio_fp);
        fseek(audio_fp, 0, SEEK_SET);

        //check for wav
        memset(header_check, 0, sizeof(char) * 4);
        header_size_to_ignore = 0;
        fread(header_check, 4, 1, audio_fp);
        fseek(audio_fp, 0, SEEK_SET);
        if (header_check[0] == 'R' && header_check[1] == 'I' && header_check[2] == 'F' && header_check[3] == 'F') {
            int nHeaderBlockSize;
            fseek(audio_fp, 16, SEEK_SET);
            fread(&nHeaderBlockSize, sizeof(int), 1, audio_fp);
            fseek(audio_fp, nHeaderBlockSize + 4, SEEK_CUR);
            //header_size_to_ignore = 44;
            //file_size -= header_size_to_ignore;
            fread(&file_size, sizeof(int), 1, audio_fp);
        }
        //fseek(audio_fp, header_size_to_ignore, SEEK_SET);

        // alloc audio buffer
        num_audio_sample = file_size / sample_size;
        audio_buffer = (int16_t*)malloc(sample_size* num_audio_sample);

        logfile_print_v("%s: reading audio sample", QVOP_FN);
        if (fread(audio_buffer, sample_size, num_audio_sample, audio_fp) < num_audio_sample) {
            logfile_print_e("Audio read fail!");
            return QVOP_FAILURE;
        }

        // frame based operation
        numAudioBlock = (num_audio_sample / block_len);   // numAudioSample / 160
        for (blockIndex = 0; blockIndex < numAudioBlock; blockIndex++) {
            logfile_print_v("%s: block number %d", QVOP_FN, blockIndex);
            if (blockIndex == numAudioBlock - 1){
                pcmPkt.eos_flag = QVOP_PCM_EOS;
            }
            else
            {
                pcmPkt.eos_flag = 0;
            }
            memcpy(pcmPkt.pcm_data, audio_buffer + blockIndex * block_len, block_len * sample_size);

            //call service interface for this test harness
            rv = service.processFrameWithId(cb, timestamp++, frameId++, block_len * sample_size, (const int8_t*)pcmPkt.pcm_data);

            //it is possible for the processFrame call to result in a match completion, success or failure
            if (cb.IsComplete()) {
                logfile_print_d("%s: IsComplete true %d", QVOP_FN, rv);
                if (cb.GetStatus() == CBSuccess) {
                    return cb.GetMatch();
                }
                else {
                    return QVOP_FAILURE;
                }
            }

            if (rv != QVOP_SUCCESS && rv != QVOP_PCM_DATA_REQUEST){
                logfile_print_e("%s: Call failed processFrameWithId %d", QVOP_FN, rv);
                break;
            }
            if (pcmPkt.eos_flag == QVOP_PCM_EOS)
            {
                logfile_print_v("%s: last packet has been sent", QVOP_FN);
                break;
            }
        }

        fclose(audio_fp);
        return rv;
    }

    static void *read_sourcetrack_data(void* data)
    {
        char kvpair_soundfocus[200] = SOUNDFOCUS_PARAMS;
        char kvpair_sourcetrack[200] = SOURCETRACK_PARAMS;
        char *string = NULL;
        char *token = NULL;
        char choice = '\0';
        int i = 0;
        qahw_module_handle_t *qawh_module_handle =
            (qahw_module_handle_t *)data;

        while (1) {
            string = qahw_get_parameters(qawh_module_handle, kvpair_soundfocus);
            if (!string) {
                logfile_print_e("Error.Failed Get SoundFocus Params\n");
            }
            else {
                token = strtok(string, "=");
                while (token) {
                    if (*token == 'S') {
                        choice = *(token + 11);
                        token = strtok(NULL, ",;");
                        i = 0;
                    }
                    switch (choice) {
                    case 'g':
                        logfile_print_v("\nSoundFocus.gain_step=%s", token);
                        break;
                    case 'e':
                        logfile_print_v("\nSoundFocus.enable_sectors[%d]=%s", i, token);
                        i++;
                        break;
                    case 's':
                        logfile_print_v("\nSoundFocus.start_angles[%d]=%s", i, token);
                        i++;
                        break;
                    }
                    token = strtok(NULL, ",;=");
                }
            }
            choice = '\0';
            string = qahw_get_parameters(qawh_module_handle, kvpair_sourcetrack);
            if (!string) {
                logfile_print_e("Error.Failed Get SourceTrack Params\n");
            }
            else {
                token = strtok(string, "=");
                while (token) {
                    if (*token == 'S') {
                        choice = *(token + 12);
                        if (choice == 'd')
                            choice = *(token + 16);
                        token = strtok(NULL, ",;");
                        i = 0;
                    }
                    switch (choice) {
                    case 'p':
                        logfile_print_v("\nSourceTrack.polar_activity=%s,", token);
                        choice = '\0';
                        break;
                    case 'v':
                        logfile_print_v("\nSourceTrack.vad[%d]=%s", i, token);
                        i++;
                        break;
                    case 's':
                        logfile_print_v("\nSourceTrack.doa_speech=%s", token);
                        break;
                    case 'n':
                        logfile_print_v("\nSourceTrack.doa_noise[%d]=%s", i, token);
                        i++;
                        break;
                    default:
                        logfile_print_v("%s,", token);
                        break;
                    }
                    token = strtok(NULL, ",;=");
                }
            }
            if (sourcetrack_done == 1)
                return NULL;
        }
    }

    static QvopStatus start_input(QvopService& service, TestCaseCallbackProcessFrame& cb, struct audio_config_params* params)
    {
        int rc = 0;
        uint64_t frameId = 0;
        uint64_t timestamp = 0;
        QvopStatus rv = QVOP_SUCCESS;
        time_t start_time = time(0);
        size_t bytes_read = -1;
        size_t total_bytes_read = 0;
        size_t sample_size = AUDIO_SAMPLE_DEFAULT_SIZE;
        if (params->config.format == AUDIO_FORMAT_PCM_24_BIT_PACKED) {
            //handle other bit depth
            sample_size = AUDIO_SAMPLE_SIZE_24;
        }
        else if (params->config.format == AUDIO_FORMAT_PCM_32_BIT) {
            //handle other bit depth
            sample_size = AUDIO_SAMPLE_SIZE_32;
        }

        qahw_module_handle_t *qahw_mod_handle = params->qahw_mod_handle;

        // Open audio input stream.
        qahw_stream_handle_t* in_handle = NULL;

        rc = qahw_open_input_stream(qahw_mod_handle,
            params->handle, params->input_device,
            &params->config, &in_handle,
            params->flags, params->kStreamName,
            params->kInputSource);
        if (rc) {
            logfile_print_e("ERROR :::: Could not open input stream.\n");
            gerror = true;
            return QVOP_FAILURE;
        }

        // Get buffer size to get upper bound on data to read from the HAL.
        size_t buffer_size;
        buffer_size = qahw_in_get_buffer_size(in_handle);
        char *buffer;
        buffer = (char *)calloc(1, buffer_size);
        if (buffer == NULL) {
            logfile_print_e("calloc failed!!\n");
            gerror = true;
            return QVOP_FAILURE;
        }

        logfile_print_v("input opened, buffer = %p, size %zun",
            buffer, buffer_size);

        int num_channels = audio_channel_count_from_in_mask(params->config.channel_mask);
        char param[100] = "audio_stream_profile=";
        qahw_in_buffer_t in_buf;

        // set profile for the recording session
        strlcat(param, params->profile, sizeof(param));
        qahw_in_set_parameters(in_handle, param);

        logfile_print_i("\nPlease speak into the microphone for %lf seconds.\n", params->loopTime);

        tests_running++;

        memset(&in_buf, 0, sizeof(qahw_in_buffer_t));

        //buffer to hold all of the PCM
        size_t szfmt = AUDIO_SAMPLE_DEFAULT_BIT_DEPTH; //assuming 16 bit
        //size /8 bit to bytes and *2 doubling necessary buffer size for padding.
        size_t lgsize = ((params->config.sample_rate * num_channels * szfmt * (size_t)params->loopTime)/8)*2;

        char* lgbuf = (char *)calloc(1, lgsize);
        if (lgbuf == NULL) {
            logfile_print_e("calloc failed!!\n");
            gerror = true;
            delete buffer;
            return QVOP_FAILURE;
        }
        //clear buffer
        memset(lgbuf, 0, lgsize);
        char* temp = lgbuf;

        //read in a tight loop & copy to large buffer
        while (true) {
            in_buf.buffer = buffer;
            in_buf.bytes = buffer_size;
            bytes_read = qahw_in_read(in_handle, &in_buf);

            //copy into large buffer
            memcpy(temp, in_buf.buffer, bytes_read);

            if (difftime(time(0), start_time) > params->loopTime) {
                logfile_print_i("PCM collection completed.\n");
                break;
            }

            //move temp ptr for next copy
            temp += bytes_read;
            total_bytes_read += bytes_read;
        }

        //move temp pointer back to beginning of lg buf
        temp = lgbuf;

        while (total_bytes_read > 0) {
            in_buf.buffer = buffer;
            //size of bytes to copy is buffer_size or remaining bytes from
            in_buf.bytes = buffer_size <= total_bytes_read ? buffer_size : total_bytes_read - buffer_size;
            memset(in_buf.buffer, 0, in_buf.bytes);
            memcpy(in_buf.buffer, temp, in_buf.bytes);

            //move pointer & adjust number of bytes read (counting down)
            temp += in_buf.bytes;
            total_bytes_read -= in_buf.bytes;

            rv = service.processFrameWithId(cb, timestamp++, frameId++, in_buf.bytes, (const int8_t*)in_buf.buffer);

            //it is possible for the processFrame call to result in a match completion, success or failure
            if (cb.IsComplete()) {
                logfile_print_d("%s: IsComplete is true, rv=%d", QVOP_FN, rv);
                delete buffer;
                delete lgbuf;
                if (cb.GetStatus() == CBSuccess) {
                    QvopStatus answer = cb.GetMatch();
                    logfile_print_d("%s: Match answer = %s", QVOP_FN, answer == QVOP_MATCH_SUCCESS ? "Match" : "No Match");
                    return answer;
                }
                else {
                    return QVOP_FAILURE;
                }
            }
        }

        logfile_print_v("closing input");

        // Close output stream and device.
        rc = qahw_in_standby(in_handle);
        if (rc) {
            logfile_print_e("out standby failed %d \n", rc);
            delete buffer;
            delete lgbuf;
            return QVOP_FAILURE;
        }

        rc = qahw_close_input_stream(in_handle);
        if (rc) {
            logfile_print_e("could not close input stream %d \n", rc);
            delete buffer;
            delete lgbuf;
            return QVOP_FAILURE;
        }

        // Print instructions to access the file.
        logfile_print_i("\nThe audio data has the "
            "following characteristics:\nsample rate: %u\nformat: %d\n"
            "num channels: %d\n",
            params->config.sample_rate,
            params->config.format, num_channels);

        tests_running--;
        return rv;
    }

    static int read_config_params_from_user(struct audio_config_params *thread_param, qvop_user_input& user_input, int rec_session) {
        int channels = 0, format = 0, source = 0, device = 0;

        thread_param->kStreamName = "input_stream";
        device = user_input.input_device;

        if (device & AUDIO_DEVICE_IN_BUILTIN_MIC)
            thread_param->input_device = AUDIO_DEVICE_IN_BUILTIN_MIC;
        else if (device & AUDIO_DEVICE_IN_WIRED_HEADSET)
            thread_param->input_device = AUDIO_DEVICE_IN_WIRED_HEADSET;
        channels = user_input.channels;
        if (channels == 1) {
            thread_param->config.channel_mask = AUDIO_CHANNEL_IN_MONO;
        }
        else if (channels == 2) {
            thread_param->config.channel_mask = AUDIO_CHANNEL_IN_STEREO;
        }
        else if (channels == 4) {
            thread_param->config.channel_mask = AUDIO_CHANNEL_INDEX_MASK_4;
        }
        else {
            gerror = true;
            logfile_print_e("\nINVALID channels");
            return -1;
        }

        format = user_input.format;
        if (format == 16) {
            thread_param->config.format = AUDIO_FORMAT_PCM_16_BIT;
        }
        else if (format == 24) {
            thread_param->config.format = AUDIO_FORMAT_PCM_24_BIT_PACKED;
        }
        else {
            gerror = true;
            logfile_print_e("\n INVALID format");
            return -1;
        }


        thread_param->config.sample_rate = user_input.sample_rate;
        thread_param->flags = (audio_input_flags_t)AUDIO_INPUT_FLAG_NONE;

        source = user_input.source;
        thread_param->kInputSource = (audio_source_t)source;

        if (rec_session == 1) {
            thread_param->handle = 0x999;
            strcpy(thread_param->output_filename, "/data/rec1.raw");
        }
        else if (rec_session == 2) {
            thread_param->handle = 0x998;
            strcpy(thread_param->output_filename, "/data/rec2.raw");
        }
        else if (rec_session == 3) {
            thread_param->handle = 0x997;
            strcpy(thread_param->output_filename, "/data/rec3.raw");
        }
        else if (rec_session == 4) {
            thread_param->handle = 0x996;
            strcpy(thread_param->output_filename, "/data/rec4.raw");
        }

        thread_param->loopTime = ((double)user_input.looptime/1000.0);
        return 0;
    }

    static QvopStatus beginVoiceInput(QvopService& service, TestCaseCallbackProcessFrame& cb, qvop_user_input& config) {
        int max_recordings_requested = 0, source_track = config.source_tracking;
        int thread_active[4] = { 0 };
        qahw_module_handle_t *qahw_mod_handle;
        const  char *mod_name = "audio.primary";

        qahw_mod_handle = qahw_load_module(mod_name);
        if (qahw_mod_handle == NULL) {
            logfile_print_v(" qahw_load_module failed");
            return QVOP_FAILURE;
        }

        max_recordings_requested = 1;
        struct audio_config_params thread1_params;

        thread1_params.qahw_mod_handle = qahw_mod_handle;
        read_config_params_from_user(&thread1_params, config, 1);
        thread_active[0] = 1;

        int ret = -1;

        QvopStatus rv = start_input(service, cb, &thread1_params);

        logfile_print_d("Done with hal record test \n");
        return rv;
    }
};


#endif //TEST_CASE_UTILITY_H
