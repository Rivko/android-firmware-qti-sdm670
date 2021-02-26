/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.HeadsetKey;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.TextView;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.LogUtils;

public class HeadsetKeyActivity extends BaseActivity {
    AudioManager mAudioManager = null;
    TextView mHeadphone;
    TextView mMicro;

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context arg0, Intent arg1) {
            // TODO Auto-generated method stub
            String action = arg1.getAction();
            LogUtils.logi("onReceive: action: " + action);

            if (action.equals(Intent.ACTION_HEADSET_PLUG)) {
                int state = arg1.getIntExtra("state", 0);
                int hasMicro = arg1.getIntExtra("microphone", 0);
                LogUtils.logi("state: " + state + ",hasMicro:" + hasMicro);
                if (state == 1) {
                    setKeyPass(mHeadphone);
                    mTestCase.addTestData("headphone_insert", "detected");
                }
                if (state == 1 && hasMicro == 1) {
                    setKeyPass(mMicro);
                    mTestCase.addTestData("microphone_insert", "detected");
                }
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        mHeadphone = (TextView) findViewById(R.id.headphone_insert);
        mMicro = (TextView) findViewById(R.id.microphone_insert);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.headset_key_act;
    }

    @Override
    protected void onResume() {
        registerListener();
        super.onResume();
        mTestCase.addTestData("headphone_insert", "not detected");
        mTestCase.addTestData("microphone_insert", "not detected");
        mTestCase.addTestData("hangup", "not detected");
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        // TODO Auto-generated method stub
        LogUtils.logi("dispatchKeyEvent -- keyCode:" + event.getKeyCode());
        TextView keyText = null;
        switch (event.getKeyCode()) {
            case KeyEvent.KEYCODE_HEADSETHOOK:
                keyText = (TextView) findViewById(R.id.hangup);
                mTestCase.addTestData("hangup", "detected");
                break;
        }

        if (null != keyText) {
            setKeyPass(keyText);
            return true;
        }

        return super.dispatchKeyEvent(event);
    }

    private void setKeyPass(TextView keyText) {
        if (null != keyText) {
            keyText.setBackgroundResource(R.drawable.pass);
        }
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
    }

    @Override
    protected void onStop() {
        // TODO Auto-generated method stub
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        unRegisterListener();
        super.onDestroy();
    }

    private void registerListener() {
        IntentFilter filter = new IntentFilter(Intent.ACTION_HEADSET_PLUG);
        registerReceiver(mReceiver, filter);
    }

    private void unRegisterListener() {
        if (null != mReceiver) {
            unregisterReceiver(mReceiver);
        }
    }
}
