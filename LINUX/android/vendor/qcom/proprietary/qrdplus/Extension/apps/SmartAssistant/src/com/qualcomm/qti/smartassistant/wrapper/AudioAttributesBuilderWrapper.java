/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.smartassistant.wrapper;

import android.media.AudioAttributes;

public class AudioAttributesBuilderWrapper {
    private AudioAttributes.Builder mAudioAttBuilder;

    public AudioAttributesBuilderWrapper() {
        mAudioAttBuilder = new AudioAttributes.Builder();
    }

    public AudioAttributes.Builder getAudioAttributesBuilder() {
        return mAudioAttBuilder;
    }

    public AudioAttributes.Builder setUsage(int usage) {
        return mAudioAttBuilder.setUsage(usage);
    }


    /**
     * Sets the attribute describing the content type of the audio signal, such as speech,
     * or music.
     * @param contentType the content type values, one of
     *     {@link AudioAttributes#CONTENT_TYPE_MOVIE},
     *     {@link AudioAttributes#CONTENT_TYPE_MUSIC},
     *     {@link AudioAttributes#CONTENT_TYPE_SONIFICATION},
     *     {@link AudioAttributes#CONTENT_TYPE_SPEECH},
     *     {@link AudioAttributes#CONTENT_TYPE_UNKNOWN}.
     * @return the same Builder instance.
     */
    public AudioAttributes.Builder setContentType(int contentType) {
        return mAudioAttBuilder.setContentType(contentType);
    }

    /**
     * Same as {@link #setCapturePreset(int)} but authorizes the use of HOTWORD,
     * REMOTE_SUBMIX and RADIO_TUNER.
     * @param preset
     * @return the same Builder instance.
     */
    public AudioAttributes.Builder setInternalCapturePreset(int preset) {
        return mAudioAttBuilder.setInternalCapturePreset(preset);
    }

    /**
     * Combines all of the attributes that have been set and return a new
     * {@link AudioAttributes} object.
     * @return a new {@link AudioAttributes} object
     */
    public AudioAttributes build() {
        return mAudioAttBuilder.build();
    }
}
