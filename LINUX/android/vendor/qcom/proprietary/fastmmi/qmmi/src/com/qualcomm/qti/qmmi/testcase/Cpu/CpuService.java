/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Cpu;

import android.content.Intent;
import android.os.CpuUsageInfo;
import android.os.HardwarePropertiesManager;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class CpuService extends BaseService {
    private HardwarePropertiesManager mHardwarePropertiesManager = null;


    public int onStartCommand(Intent intent, int flags, int startId) {
        mHardwarePropertiesManager = (HardwarePropertiesManager) getSystemService(
                HARDWARE_PROPERTIES_SERVICE);
        if (mHardwarePropertiesManager == null) {
            LogUtils.loge("No mHardwarePropertiesManager service here");
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

        LogUtils.logi("CpuService service run");
        CpuUsageInfo[] cpuUsageInfos = mHardwarePropertiesManager.getCpuUsages();
        float[] temperatures = mHardwarePropertiesManager.getDeviceTemperatures(HardwarePropertiesManager.DEVICE_TEMPERATURE_CPU, HardwarePropertiesManager.TEMPERATURE_CURRENT);

        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < cpuUsageInfos.length; i++) {
            sb.append("CPU" + i + ":" + cpuUsageInfos[i].getTotal() + "," + cpuUsageInfos[i].getActive() + "\n");
        }

        for (int i = 0; i < temperatures.length; i++) {
            sb.append("CPU" + i + " " + this.getResources().getString(R.string.cpu_temperature) + " : " + temperatures[i] + "\n");
        }

        LogUtils.logi("content :" + sb.toString());

        if (sb == null || sb.toString().equals("")){
            LogUtils.logi("Case State : Fail");
            updateView(testCase.getName(), this.getResources().getString(R.string.no_data));
            updateResultForCase(testCase.getName(), TestCase.STATE_FAIL);
        } else {
            LogUtils.logi("Case State : True");
            updateView(testCase.getName(), sb.toString());
            updateResultForCase(testCase.getName(), TestCase.STATE_PASS);
        }

        return 0;
    }

}
