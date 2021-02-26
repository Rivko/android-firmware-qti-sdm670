/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import android.content.Context;

public class WakeupSessionHelper {
    private IWakeupSession mWakeupSessionImpl;

    public WakeupSessionHelper() {
        mWakeupSessionImpl = WakeupSessionImpl.getInstance();
    }

    public int establishSvaSession(String smName, Context context) {
        return mWakeupSessionImpl.establishSvaSession(smName, context);
    }

    public int terminateSvaSession(String smName) {
        return mWakeupSessionImpl.terminateSvaSession(smName);
    }

    public void releaseAllSvaSessions() {
        mWakeupSessionImpl.releaseAllSvaSessions();
    }

    public void addWakeupListener(IWakeupListener listener) {
        mWakeupSessionImpl.addWakeupListener(listener);
    }

    public void removeWakeupListener(IWakeupListener listener) {
        mWakeupSessionImpl.removeWakeupListener(listener);
    }

    public int restartRecognition(int smHandle) {
        return mWakeupSessionImpl.restartRecognition(smHandle);
    }

    SmSession querySessionInfo(String smName) {
        return mWakeupSessionImpl.querySessionInfo(smName);
    }
}
