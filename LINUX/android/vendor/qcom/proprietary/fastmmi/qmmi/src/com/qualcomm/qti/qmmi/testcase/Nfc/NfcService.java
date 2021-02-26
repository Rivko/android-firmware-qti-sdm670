/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Nfc;

import android.content.Intent;

import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class NfcService extends BaseService {

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi( "onStartCommand");
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void register() {
    }

    @Override
    public int stop(TestCase testCase) {
        return 0;
    }

    @Override
    public int run(TestCase testCase) {

        LogUtils.logi( " Run....");
        return 0;
    }

}
