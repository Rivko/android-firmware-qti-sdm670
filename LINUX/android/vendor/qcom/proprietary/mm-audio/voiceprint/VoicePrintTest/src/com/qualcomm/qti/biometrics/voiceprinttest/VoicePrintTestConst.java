/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprinttest;

import android.os.Environment;

public class VoicePrintTestConst {
    public static final long MIC_REQUEST_TIMEOUT = 50000;

    public static final int COUNTDOWNLATCH_AWAIT_PERIOD = 1000;

    public static final int AUDIO_SAMPLE_RATE = 16000;

    public static final String TEST_NAME_KEY = "test_name";

    public static final String IDENTIFIER_KEY = "identifier";

    public static final String PHRASE_KEY = "phrase";

    public static final String ENROLL_PCM_FOLDER_KEY = "enroll_pcm_folder";

    public static final String MATCH_PCM_FILE_KEY = "match_pcm_file";

    public static final String LOG_FILE_LOC = Environment.getExternalStorageDirectory()
            .getAbsolutePath() + "/VoicePrintTest/TestLogs/";

    public static final String THRESHOLD_KEY = "threshold";
}
