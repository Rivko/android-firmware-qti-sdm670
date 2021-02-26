/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Sensor;

import android.content.Intent;
import android.os.Bundle;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import android.view.View;

public class SensorActivity extends BaseActivity {
    private indicationView mIndicationView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mTextView.setText(R.string.case_loading);

        mIndicationView = (indicationView) findViewById(R.id.indicationView);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.sensor_act;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void updateView(Bundle bundle) {
        String message = bundle.getString(Utils.BUNDLE_KEY_MESSAGE);
        String caseName = bundle.getString(Utils.BUNDLE_KEY_CASE_NAME);

        LogUtils.logd( "updateView for sensor case: " + caseName);
        LogUtils.logd( "update message : " + message);
        LogUtils.logd( "current sensor case: " + mTestCase.getName());

        if ( caseName != null && caseName.equals(mTestCase.getName())){
            int len = 0;
            String[] sensorInfoArray = message.split(":");
            mTextView.setText(sensorInfoArray[0]);
            len = sensorInfoArray.length;
            if(len == 5)
                mIndicationView.setValue(sensorInfoArray[1], sensorInfoArray[2],
                sensorInfoArray[3], sensorInfoArray[4]);
        }
    }
}
