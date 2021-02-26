/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.framework;

public abstract class BaseRunnable implements Runnable {

    private boolean mRunning = false;

    public BaseRunnable() {

    }

    public void stop() {
        mRunning = false;
    }

    public boolean getRunning() {
        return mRunning;
    }

    public void setRunning(boolean running) {
        this.mRunning = running;
    }

}
