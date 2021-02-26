/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

package com.qualcomm.qti;

import android.app.Service;
import android.content.Intent;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.os.Trace;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class PerfService extends IPerfManager.Stub {
    private static final String TAG = "PerfService";
    private static final boolean DEBUG =
            SystemProperties.getInt("debug.trace.perf", 0) == 1 ? true : false;
    private static final int REQUEST_FAILED = -1;
    private static final int REQUEST_SUCCEEDED = 0;
    private int mHandle = 0;

    static {
        try {
            System.loadLibrary("qti_performance");
        } catch (UnsatisfiedLinkError e) {
        }
    }

    public int perfLockAcquire(int duration, int[] list) {
        mHandle = native_perf_lock_acq(mHandle, duration, list);
        if (mHandle <= 0)
            return REQUEST_FAILED;
        else
            return mHandle;
    }

    public int perfHint(int hint, String userDataStr, int userData1, int userData2) {
        if (DEBUG)Trace.traceBegin(Trace.TRACE_TAG_ALWAYS, "[perfservice]perfHint:"
                                                           + hint + " " + userDataStr + " "
                                                           + userData1 + " " + userData2);
        mHandle = native_perf_hint(hint, userDataStr, userData1, userData2);
        if (DEBUG)Trace.traceEnd(Trace.TRACE_TAG_ALWAYS);
        if (mHandle <= 0)
            return REQUEST_FAILED;
        else
            return mHandle;
    }

    public int perfLockRelease() {
        int retValue = REQUEST_SUCCEEDED;
        if (DEBUG)Trace.traceBegin(Trace.TRACE_TAG_ALWAYS, "[perfservice]perfLockRelease handle:" + mHandle);
        retValue = native_perf_lock_rel(mHandle);
        if (DEBUG)Trace.traceEnd(Trace.TRACE_TAG_ALWAYS);
        mHandle = 0;
        return retValue;
    }

    public int perfLockReleaseHandler(int _handle) {
        int retValue = REQUEST_SUCCEEDED;
        if (DEBUG)Trace.traceBegin(Trace.TRACE_TAG_ALWAYS, "[perfservice]perfLockRelease _handle:" + _handle);
        retValue = native_perf_lock_rel(_handle);
        if (DEBUG)Trace.traceEnd(Trace.TRACE_TAG_ALWAYS);
        return retValue;
    }

    private native int  native_perf_lock_acq(int handle, int duration, int list[]);
    private native int  native_perf_hint(int hint, String userDataStr, int userData1, int userData2);
    private native int  native_perf_lock_rel(int handle);
}
