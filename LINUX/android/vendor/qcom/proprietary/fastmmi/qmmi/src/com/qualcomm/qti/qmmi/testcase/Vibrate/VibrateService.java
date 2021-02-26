/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Vibrate;

import android.content.Intent;
import android.os.Vibrator;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.R;

public class VibrateService extends BaseService {

    private static Vibrator mVibrator = null;
    private final static long VIBRATOR_ON_TIME = 1000;
    private final static long VIBRATOR_OFF_TIME = 500;
    long[] pattern = {VIBRATOR_OFF_TIME, VIBRATOR_ON_TIME};

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi( "onStartCommand");
        mVibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        if (mVibrator == null) {
            LogUtils.loge( "No mVibrator service here");
        }

        return super.onStartCommand(intent, flags, startId);
    }


    @Override
    public void register() {

    }

    @Override
    public int stop(TestCase testCase) {
        mVibrator.cancel();
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        LogUtils.logi( "VibrateService service run");

        mVibrator.vibrate(pattern, 0);
        updateView(testCase.getName(), this.getResources().getString(R.string.vibrator_on));
        return 0;
    }

}
