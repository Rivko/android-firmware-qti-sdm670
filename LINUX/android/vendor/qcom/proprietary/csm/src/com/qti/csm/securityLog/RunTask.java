/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;

import java.util.Timer;
import java.util.TimerTask;

public class RunTask extends Service {
    private Timer mTimer;
    private TimerTask mTimerTask;
    private DataProc task;
    private Context mContextApp;
    private static final String TAG = "RunTask";

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

        if (DataProc.mSet > 0) {
            mContextApp = getApplicationContext();
            task = new DataProc(mContextApp);
            mTimer = new Timer();
            mTimerTask = new TimerTask() {
                @Override
                public void run() {
                    task.updateRec(mContextApp);
                }
            };
            mTimer.schedule(mTimerTask, 10000, DataProc.mSet);
        }
    }

    @Override
    public void onDestroy() {
        if (mTimer != null) {
            mTimer.cancel();
            mTimer.purge();
        }
        super.onDestroy();
    }
}