/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo;

import android.media.MediaRecorder.AudioSource;
import android.view.Gravity;

/**
 * The Class Consts.
 */
public class Consts {


    /**
     * The Constant AUDIO_SAMPLE_RATE.
     */
    public final static String AUDIO_SAMPLE_RATE = "AUDIO_SAMPLE_RATE";

    /**
     * The Constant HEADSET.
     */
    public final static String HEADSET = "HEADSET";

    /**
     * The Constant NUMBER_OF_CHANNELS_INPUT.
     */
    public final static String NUMBER_OF_CHANNELS_INPUT = "NUMBER_OF_CHANNELS_INPUT";

    /**
     * The Constant NUMBER_OF_CHANNELS_OUTPUT.
     */
    public final static String NUMBER_OF_CHANNELS_OUTPUT = "NUMBER_OF_CHANNELS_OUTPUT";

    /**
     * The Constant MIC_GAIN_ENABLED.
     */
    public final static String MIC_GAIN_ENABLED = "MIC_GAIN_ENABLED";

    /**
     * The Constant DEFAULT_AUDIO_SAMPLE_RATE.
     */
    public final static int DEFAULT_AUDIO_SAMPLE_RATE = 16000;

    /**
     * The Constant DEFAULT_NUMBER_OF_CHANNELS.
     */
    public final static int DEFAULT_NUMBER_OF_CHANNELS = 1;

    /**
     * The Constant MAX_GAIN_BOUND.
     */
    public final static int MAX_GAIN_BOUND = 24;

    /**
     * The Constant MIN_GAIN_BOUND.
     */
    public final static int MIN_GAIN_BOUND = 0;

    /**
     * The Constant GAIN_INCREMENT.
     */
    public final static int GAIN_INCREMENT = 3;

    /**
     * The Constant DEFAULT_AUDIO_SOURCE.
     */
    public final static int DEFAULT_AUDIO_SOURCE = AudioSource.MIC;

    /**
     * The Constant SOURCE_TYPE.
     */
    public static final String SOURCE_TYPE = "SOURCE_TYPE";

    /**
     * The Constant EQUAL.
     */
    public static final String EQUAL = "=";

    /**
     * The Constant COMMA.
     */
    public static final String COMMA = ",";

    public static final int DEFAULT_GRAVITY = Gravity.BOTTOM;


    public static final int VERIFICATION = 1;
    public final static int RESULT_CLOSE_ALL = -1;

    public static final int RESULT_SUCCESS = 0;
    public static final int RESULT_FAILED = 1;
    public static final int RESULT_CANCELLED = 2;
    public static final int RESULT_TIMEOUT = 3;


    public final static int MSG_ENROLL_STEP_SUCCESS = 0;
    public final static int MSG_ENROLL_STEP_FAILED = 1;
    public final static int MSG_ENROLL_RESULT = 2;
    public final static int MSG_MATCH_RESULT = 3;
    public final static int MSG_ERROR = 4;

    public static String[] Sensitivity = {"", "HIGH", "MED", "LOW"};
}
