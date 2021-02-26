/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.smartassistant.wrapper;

import android.os.Build;

import com.qualcomm.qti.smartassistant.utils.LogUtils;

import java.lang.reflect.Method;

public class WindowManagerGlobalWrapper {

    private static final String TAG = "WindowManagerGlobalWrapper";

    public void dismissKeyguard() {
        try {
            Class windowManagerGlobal = Class.forName("android.view.WindowManagerGlobal");
            Method getWindowManagerService = windowManagerGlobal.
                    getMethod("getWindowManagerService");
            Object wmInstance = getWindowManagerService.invoke(windowManagerGlobal);
            if (wmInstance != null){
                Class IWindowManager = Class.forName("android.view.IWindowManager");
                if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O){
                    Method unlockMethod = IWindowManager.getMethod("dismissKeyguard");
                    unlockMethod.invoke(wmInstance);
                }else {
                    Class callback = Class.forName("com.android.internal.policy.IKeyguardDismissCallback");
                    Method unlockMethod = IWindowManager.getMethod("dismissKeyguard", callback);
                    Object param = null;
                    unlockMethod.invoke(wmInstance, param);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            LogUtils.e(TAG,"dismiss keyguard error "+e.toString());
        }
    }
}
