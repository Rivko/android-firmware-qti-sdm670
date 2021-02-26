/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Sim;


import android.content.Intent;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseService;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class SimService extends BaseService {

    TelephonyManager mTelephonyManager = null;

    public int onStartCommand(Intent intent, int flags, int startId) {
        LogUtils.logi( "onStartCommand");
        mTelephonyManager = TelephonyManager.from(getApplicationContext());
        if (mTelephonyManager == null) {
            LogUtils.loge( "No mWifiManager service here");
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

        int sub = Integer.valueOf(testCase.getParameter().get("sub"));
        StringBuffer sb = new StringBuffer();
        String iccId = null;
        sb.append(this.getResources().getString(R.string.sim_list)).append("\n");

        try {
            if (mTelephonyManager.isMultiSimEnabled()) {
                int[] subId = SubscriptionManager.getSubId(sub);
                iccId = mTelephonyManager.getSimSerialNumber(subId[0]);
            } else {
                iccId = mTelephonyManager.getSimSerialNumber();
            }
            LogUtils.logi("iccId:" + iccId);
            if (iccId != null && !iccId.equals("")) {

                String simSlot = this.getResources().getString(R.string.sim_slot);

                sb.append(String.format(simSlot, sub+1))
                    .append(": ")
                    .append(this.getResources().getString(R.string.sim_deteched))
                    .append("\n");
                testCase.addTestData("SIM" + sub, "deteched");
                updateResultForCase(testCase.getName(), TestCase.STATE_PASS);
            }else{

                String simSlot = this.getResources().getString(R.string.sim_slot);

                sb.append(String.format(simSlot, sub+1))
                .append(": ")
                .append(this.getResources().getString(R.string.sim_not_deteched))
                .append("\n");
                testCase.addTestData("SIM" + sub, "not deteched");
                updateResultForCase(testCase.getName(), TestCase.STATE_FAIL);
            }
        }catch (SecurityException e){
            LogUtils.logi( "getSimSerialNumber error:" + e);
        }

        updateView(testCase.getName(), sb.toString());
        LogUtils.logi( "simservice run");
        return 0;
    }

}
