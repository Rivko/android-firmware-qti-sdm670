/*
 * Copyright (c) 2015, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef _AV_METADATA_H_
#define _AV_METADATA_H_

#include <media/stagefright/MediaDefs.h>
#include <system/audio.h>

namespace android {

enum {
    kKeyEnableSetBuffers = 'sbuf',
};

enum {
    ENABLE_SET_BUFFERS = 0x1,
    ENABLE_AUDIO_BIG_BUFFERS = 0x2
};

AudioEncoding getPcmEncoding(uint32_t bitWidth) {

    AudioEncoding e = kAudioEncodingPcm16bit;
    switch(bitWidth) {
        case 8:
            e = kAudioEncodingPcm8bit;
            break;
        case 16:
            e = kAudioEncodingPcm16bit;
            break;
        case 24:
            e = kAudioEncodingPcm24bitPacked;
            break;
        Default:
            break;
    }
    return e;
}

audio_format_t getAudioFormat(AudioEncoding e) {
    audio_format_t format = AUDIO_FORMAT_INVALID;
    switch (e) {
        case kAudioEncodingPcm8bit:
            format = AUDIO_FORMAT_PCM_8_BIT;
            break;
        case kAudioEncodingPcm16bit:
            format = AUDIO_FORMAT_PCM_16_BIT;
            break;
        case kAudioEncodingPcmFloat:
            format = AUDIO_FORMAT_PCM_FLOAT;
            break;
        case kAudioEncodingPcm24bitPacked:
            format = AUDIO_FORMAT_PCM_24_BIT_PACKED;
            break;
        default:
            ALOGE("Invalid AudioEncoding %d",e);
            break;
    }
    return format;
}

}//namespace android

#endif // _AV_METADATA_H_


