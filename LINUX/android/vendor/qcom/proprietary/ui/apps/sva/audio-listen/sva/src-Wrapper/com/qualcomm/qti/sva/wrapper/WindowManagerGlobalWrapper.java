/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.qti.sva.wrapper;

import android.os.Build;
import android.os.RemoteException;
import android.view.WindowManagerGlobal;
import android.view.IWindowManager;

public class WindowManagerGlobalWrapper {

    public static IWindowManager getWindowManagerService() {
        return WindowManagerGlobal.getWindowManagerService();
    }

    public void dismissKeyguard() {
        IWindowManager wm = getWindowManagerService();
        if (wm != null) {
            try {
                wm.dismissKeyguard(null);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }
}
