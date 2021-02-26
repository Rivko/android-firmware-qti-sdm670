/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Memory;


import android.app.ActivityManager;
import android.content.Intent;
import android.text.format.Formatter;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.NumericUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

public class MemoryService extends BaseService {
    private ActivityManager mActivityManager = null;

    public int onStartCommand(Intent intent, int flags, int startId) {
        // 得到ActivityManager
        mActivityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
        if (mActivityManager == null) {
            LogUtils.loge("No mActivityManager service here");
        }

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

        LogUtils.logi("MemoryService service run");
        // 创建ActivityManager.MemoryInfo对象
        ActivityManager.MemoryInfo mi = new ActivityManager.MemoryInfo();
        mActivityManager.getMemoryInfo(mi);
        Long totalMem = mi.totalMem;
        Long availMem = mi.availMem;

        StringBuffer sb = new StringBuffer();
        sb.append(this.getResources().getString(R.string.memory_info)).append("\n")
        .append(this.getResources().getString(R.string.memory_total))
        .append(Formatter.formatFileSize(this.getApplicationContext(), totalMem)).append("\n")
        .append(this.getResources().getString(R.string.memory_avail))
        .append(Formatter.formatFileSize(this.getApplicationContext(), availMem)).append("\n");

        testCase.addTestData("Total Memory", Formatter.formatFileSize(this.getApplicationContext(), totalMem));
        testCase.addTestData("Avail Memory", Formatter.formatFileSize(this.getApplicationContext(), availMem));

        boolean totalMemState = checkState(testCase, totalMem);
        boolean availMemState = checkState(testCase, availMem);

        if(totalMemState && availMemState){
            updateResultForCase(testCase.getName(), TestCase.STATE_PASS);
        }else{
            updateResultForCase(testCase.getName(), TestCase.STATE_FAIL);
        }

        updateView(testCase.getName(), sb.toString());
        return 0;
    }

    private boolean checkState(TestCase testCase, long size){
        float data = (float)(size)/Utils.GIGABYTE_BIT;
        String formattedData = NumericUtils.formatDecimals(data, 2);
        float twoDecimalData = Float.valueOf(formattedData);
        LogUtils.logi("Check size: " + twoDecimalData + " GB");
        return testCase.checkLimit(twoDecimalData, "value");
    }

}
