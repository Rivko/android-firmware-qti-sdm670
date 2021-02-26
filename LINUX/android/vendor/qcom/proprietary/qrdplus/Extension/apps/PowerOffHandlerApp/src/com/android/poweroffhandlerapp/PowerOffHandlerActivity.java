/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.android.poweroffhandlerapp;

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.util.Log;
import android.view.IWindowManager;
import android.view.Window;
import android.view.WindowManager;

import java.io.File;

public class PowerOffHandlerActivity extends Activity {

    private static final String TAG = "PowerOffHandlerActivity";
    private static final String OEM_SHUTDOWNANIMATION_FILE
            = "/oem/media/shutdownanimation.zip";
    private static final String SYSTEM_SHUTDOWNANIMATION_FILE
            = "/system/media/shutdownanimation.zip";
    private static final String SYSTEM_ENCRYPTED_SHUTDOWNANIMATION_FILE
            = "/system/media/shutdownanimation-encrypted.zip";
    private static final String SHUTDOWN_HANDLER_FLAG = "shutdown_handler";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        boolean isShutdown = getIntent().getBooleanExtra(SHUTDOWN_HANDLER_FLAG, false);
        if (ActivityManager.isUserAMonkey() || !isShutdown) {
            System.exit(0);
            return;
        }

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_FULLSCREEN);

        if (checkAnimationFileExist()) {
            lockDevice();
            showShutdownAnimation();
        }
    }

    // lock device because we only support orientation
    private static void lockDevice() {
        IWindowManager wm = IWindowManager.Stub
                .asInterface(ServiceManager.getService(Context.WINDOW_SERVICE));
        try {
            wm.updateRotation(false, false);
        } catch (RemoteException e) {
            Log.w(TAG, "boot animation can not lock device!");
        }
    }

    // call poweroffhandler service to play music and animation
    private static void showShutdownAnimation() {
        SystemProperties.set("ctl.start", "poweroffhandler");
    }

    private boolean checkAnimationFileExist() {
        if (new File(OEM_SHUTDOWNANIMATION_FILE).exists()
                || new File(SYSTEM_SHUTDOWNANIMATION_FILE).exists()
                || new File(SYSTEM_ENCRYPTED_SHUTDOWNANIMATION_FILE).exists()) {
            return true;
        } else {
            Log.w(TAG, "Shutdown animation file not find!");
            return false;
        }
    }
}
