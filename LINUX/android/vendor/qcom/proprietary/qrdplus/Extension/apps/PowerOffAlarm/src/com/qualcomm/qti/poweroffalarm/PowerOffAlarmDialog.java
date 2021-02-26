/*
 * Copyright (c) 2013, 2016-2017 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution. Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


package com.qualcomm.qti.poweroffalarm;

import android.app.Activity;
import android.app.KeyguardManager;
import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;

public class PowerOffAlarmDialog  extends Activity{
    private KeyguardManager mKeyguardManager;

    private KeyguardManager.KeyguardLock mKeyguardLock;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        setContentView(R.layout.dialog_power_off_alarm);
        mKeyguardManager = (KeyguardManager) getSystemService(Context.KEYGUARD_SERVICE);
        Button BtnYes = (Button) findViewById(R.id.poweron_yes);
        BtnYes.requestFocus();
        BtnYes.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });

        Button BtnNo = (Button) findViewById(R.id.poweron_no);
        BtnNo.requestFocus();
        BtnNo.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                PowerOffAlarmUtils.powerOff(PowerOffAlarmDialog.this);
                finish();
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        disableKeyguard();
    }

    @Override
    protected void onStop() {
        super.onStop();
        enableKeyguard();

    }
    private synchronized void enableKeyguard() {
        if (mKeyguardLock != null) {
            mKeyguardLock.reenableKeyguard();
        }
    }

    private synchronized void disableKeyguard() {
        if (mKeyguardLock == null) {
            mKeyguardLock = mKeyguardManager.newKeyguardLock("PowerOffAlarm");
        }
        mKeyguardLock.disableKeyguard();
    }

    /**
     * ShutDown receiver,responsible for making sure that implement to power off
     * when power on due to alarm, and no user operation
     */
    public static class ShutDownReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(final Context context, Intent intent) {
            PowerOffAlarmUtils.powerOff(context);
        }
    }

}
