/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprinttest;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.util.Log;

public class VoicePrintTestLog {
    public static final String TAG = "VoicePrintTestLog";

    private String mLog;

    private int mNumTests;

    private int mNumPass;

    private File mFile;

    private FileOutputStream mFileOut;

    public VoicePrintTestLog(int numTests, String testName) {
        mLog = "---VoicePrintTestLog---\n";
        mNumTests = numTests;
        mNumPass = 0;
        File dir = new File(VoicePrintTestConst.LOG_FILE_LOC);
        if (!dir.exists()) {
            dir.mkdirs();
        }

        mFile = new File(VoicePrintTestConst.LOG_FILE_LOC, testName);
    }

    public String getTestLog() {
        return mLog;
    }

    public void writeTestLog(String log) {
        mLog += log;
    }

    public void saveTestLog() {
        try {
            mFileOut = new FileOutputStream(mFile);
            mFileOut.write(mLog.getBytes());
            mFileOut.close();
        } catch (FileNotFoundException e) {
            Log.e(TAG, e.getMessage());
            e.printStackTrace();
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
            e.printStackTrace();
        }

    }
}
