/* Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CSD_HAL_H
#define CSD_HAL_H

#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "core_sound_driver_v01.h"
#include "qmi_csi.h"

#ifndef ANDROID
#define strlcat g_strlcat
#define strlcpy g_strlcpy
#define LOGI(...)      fprintf(stdout, __VA_ARGS__)
#define LOGE(...)      fprintf(stderr, __VA_ARGS__)
#define LOGV(...)      fprintf(stderr, __VA_ARGS__)
#define LOGD(...)      fprintf(stderr, __VA_ARGS__)
#endif

#define MAX_HAL_CSD_QMI_CLIENTS 2

enum {
    HAL_STATE_WAIT,
    HAL_STATE_PROCESS_CMD,
    HAL_STATE_EXIT_THREAD
}csd_hal_pthread_state;

enum {
    HAL_VOICE_MULTIMODE1_ID,
    HAL_VOICE_MULTIMODE2_ID,
}csd_hal_voice_modes;

typedef enum {
    HAL_VOICE_MUTE_MODE_STREAM,
    HAL_VOICE_MUTE_MODE_DEVICE,
    HAL_VOICE_MUTE_MODE_STREAM_DEVICE
}csd_hal_mute_mode;

typedef enum {
    HAL_VOICE_MUTE_DIR_TX,
    HAL_VOICE_MUTE_DIR_RX,
    HAL_VOICE_MUTE_DIR_TX_RX
}csd_hal_mute_direction;

struct csd_hal_dev_attribute
{
    uint32_t    sample_rate;
    uint32_t    bits_per_sample;
};

struct csd_hal_dev_entry {
    uint32_t                        dev_id;
    struct csd_hal_dev_attribute    dev_attrib;
};

struct csd_hal_devices_enable_info {
    struct csd_hal_dev_entry    *devs;
};

#define HAL_MAX_NO_ACTIVE_DEVICES 3
struct csd_hal_device_data{
    struct csd_hal_dev_entry    devices[HAL_MAX_NO_ACTIVE_DEVICES];
};

struct csd_hal_volume_data{
    uint16_t volume_index;
    uint16_t ramp_duration_in_ms;
};

struct csd_hal_voice_mute_data{
    csd_hal_mute_mode mode;
    uint8_t enable;
    csd_hal_mute_direction direction;
    uint16_t ramp_duration_in_ms;
};

struct csd_hal_command_data{
    uint32_t                    token;
    int32_t                     msg_id;
    qmi_req_handle              req_handle;
    uint32_t                    command_handle;
    struct csd_hal_client_data  *command_client_data;
};

struct csd_hal_common_data{
    const hw_module_t           *module;
    struct audio_hw_device      *device;
    audio_stream_out_t          *out_stream;
    pthread_t                   thread;
    int                         thread_state;
    struct csd_hal_command_data command;
    struct csd_hal_device_data  device_data;
    const char* session_id;
    struct csd_hal_volume_data volume_data;
    struct csd_hal_voice_mute_data mute_data;
};

int csd_hal_init(void);
int csd_hal_deinit(void);


#endif /*CSD_HAL_H*/
