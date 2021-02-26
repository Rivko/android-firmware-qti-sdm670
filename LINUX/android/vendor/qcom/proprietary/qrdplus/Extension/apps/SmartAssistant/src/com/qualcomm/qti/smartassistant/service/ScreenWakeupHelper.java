/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.service;

import android.app.KeyguardManager;
import android.content.Context;
import android.os.PowerManager;
import android.util.Log;

import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.wrapper.WindowManagerGlobalWrapper;

import java.util.Timer;
import java.util.TimerTask;

public class ScreenWakeupHelper {

    private static final String TAG = "WakeupManager";
    private final static int DISPLAY_ON_DURATION = 5000;
    private final Context mContext;
    private PowerManager.WakeLock wakeLock = null;
    private Timer wakelockTimer = null;
    private KeyguardManager.KeyguardLock keyguardLock = null;

    public ScreenWakeupHelper(Context context){
        mContext =  context;
        // Create a wakeLock to turn the display on when detection occurs.
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        wakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK |
                PowerManager.ACQUIRE_CAUSES_WAKEUP, "smartassistantLock");
    }

    public void wakeup(){
        turnOnDisplay();
        dismissKeyguard();
    }

    private synchronized void dismissKeyguard() {
        WindowManagerGlobalWrapper windowManager = new WindowManagerGlobalWrapper();
        windowManager.dismissKeyguard();
    }

    private synchronized void turnOnDisplay() {
        LogUtils.v(TAG, "turnOnDisplay");

        // If the wakeLock isn't held, that means no other detections
        // have happened in the last DISPLAY_ON_DURATION milliseconds.
        // It's safe to acquire the wakeLock.
        if(false == wakeLock.isHeld()) {
            wakeLock.acquire();
        } else {
            PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
            if (false == pm.isInteractive()) {
                Log.e(TAG, "wake lock held but not in interactive, release and acquire to wake up device");
                wakeLock.release();
                wakeLock.acquire();
            }
        }

        // If no previous detection within 3 seconds, starts a new
        // timerTask and releases the wakeLock after 3 seconds.
        // If a timerTask already exists, cancels that task and
        // creates a new one to release the wakeLock after 3 seconds.
        startWakelockTimer();
    }

    // Turns off the display after DISPLAY_ON_DURATION milliseconds if no user interaction.
    private void startWakelockTimer() {
        Log.v(TAG, "startWakelockTimer");
        // To stop existing wakelockTimers
        stopWakelockTimer();
        wakelockTimer = new Timer();
        wakelockTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.v(TAG, "startWakelockTimer: run()- release wakelock");
                // To stop this wakelockTimer from repeating
                stopWakelockTimer();
                try {
                    if (wakeLock.isHeld()) {
                        wakeLock.release();
                    }
                } catch (RuntimeException re) {
                    Log.e(TAG, "WakeLock under-locked");
                }
            }
        }, DISPLAY_ON_DURATION);
    }

    private void stopWakelockTimer() {
        if (null != wakelockTimer) {
            // Cancel all scheduled tasks for the timer.
            wakelockTimer.cancel();
            // Remove all canceled tasks from the queue so they can be garbaged collected.
            int numPurgedTasks = wakelockTimer.purge();
            //wakelockTimer = null;
            Log.v(TAG, "stopWakelockTimer: numPurgedTasks= " + numPurgedTasks);
        }
    }
}
