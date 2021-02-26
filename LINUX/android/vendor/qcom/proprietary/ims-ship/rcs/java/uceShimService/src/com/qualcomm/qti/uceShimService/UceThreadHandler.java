/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/

package com.qualcomm.qti.uceShimService;
import java.lang.Thread;
import android.util.Log;

public class UceThreadHandler {
    private Thread t = null;
    private int serviceHandle = -1;
    private final static String logTAG = "AIDL-Shim:UceThreadHandler:";

    public void setThread(Thread t) {
        this.t = t;
    }
    public void suspend() {
        synchronized(this.t) {
            try {
                this.t.wait();
            }
            catch (InterruptedException e) {
                Log.d(logTAG, "InterruptedException: unable to hold thread");
            }
        }
    }
    public void resume() {
        synchronized(this.t) {
            this.t.notify();
        }
    }
    public void setHandle(int handle) {
        synchronized(this) {
            this.serviceHandle = handle;
        }
    }
    public int getHandle() {
        synchronized(this) {
            return this.serviceHandle;
        }
    }
}
