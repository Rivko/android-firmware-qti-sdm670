/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprinttest;

import java.util.Random;

import junit.framework.TestSuite;
import android.os.Bundle;
import android.test.InstrumentationTestRunner;
import android.test.InstrumentationTestSuite;
import android.util.Log;

public class VoicePrintTestRunner extends InstrumentationTestRunner {
    public static final String TAG = "VoicePrintTestRunner";

    @Override
    public TestSuite getAllTests() {
        TestSuite suite = new InstrumentationTestSuite(this);

        suite.addTestSuite(VoicePrintTestBase.class);

        return suite;
    }

    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        Log.d(TAG, "onCreate");

        VoicePrintTestBase.test_name = (String) bundle.get(VoicePrintTestConst.TEST_NAME_KEY);
        if (VoicePrintTestBase.test_name == null) {
            VoicePrintTestBase.test_name = "DefaultTestName";
        }
        Log.d(TAG, "Test name is " + VoicePrintTestBase.test_name);

        VoicePrintTestBase.identifier = (String) bundle.get(VoicePrintTestConst.IDENTIFIER_KEY);
        if (VoicePrintTestBase.identifier == null) {
            VoicePrintTestBase.identifier = "DefaultID";
        }
        Log.d(TAG, "ID is " + VoicePrintTestBase.identifier);

        VoicePrintTestBase.phrase = (String) bundle.get(VoicePrintTestConst.PHRASE_KEY);
        if (VoicePrintTestBase.phrase == null) {
            VoicePrintTestBase.phrase = "";
        }
        Log.d(TAG, "Phrase is " + VoicePrintTestBase.identifier);

        VoicePrintTestBase.match_pcm_file = (String) bundle
                .get(VoicePrintTestConst.MATCH_PCM_FILE_KEY);
        if (VoicePrintTestBase.match_pcm_file == null) {
            Log.e(TAG, "No specified match pcm file");
            Log.d(TAG, "Match pcm file null");
        } else {
            Log.d(TAG, "Match pcm file " + VoicePrintTestBase.match_pcm_file);
        }

        VoicePrintTestBase.enroll_pcm_folder = (String) bundle
                .get(VoicePrintTestConst.ENROLL_PCM_FOLDER_KEY);
        if (VoicePrintTestBase.enroll_pcm_folder == null) {
            Log.e(TAG, "No specified enroll pcm folder");
            Log.d(TAG, "Enroll pcm folder null");
        } else {
            Log.d(TAG, "Enroll pcm folder " + VoicePrintTestBase.enroll_pcm_folder);
        }

        VoicePrintTestBase.threshold = (String) bundle.get(VoicePrintTestConst.THRESHOLD_KEY);
        if (VoicePrintTestBase.threshold == null) {
            Log.e(TAG, "No specified threshold value");
            VoicePrintTestBase.threshold = "";
        } else {
            Log.d(TAG, "Threshold value " + VoicePrintTestBase.threshold);
        }

        byte[] buf = new byte[1024];
        Random rand = new Random();
        rand.nextBytes(buf);

        VoicePrintTestBase.randomData = buf;
    }
}
