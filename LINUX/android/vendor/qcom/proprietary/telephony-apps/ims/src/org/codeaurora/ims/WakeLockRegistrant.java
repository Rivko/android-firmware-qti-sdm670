/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2006 The Android Open Source Project
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

package org.codeaurora.ims;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.Registrant;

import com.qualcomm.ims.utils.Log;

/**
 * {@hide}
 */
public class WakeLockRegistrant extends Registrant {

    private WakeLock mNotifyWakeLock;
    private long mWakeLockTimeOut;

    // Default wakelock time out for unsol responses in ms
    public static final long UNSOL_WAKELOCK_TIMEOUT_MS = 200;
    public static final String WAKELOCK_NAME = "NotifyRegistrantLock";

    public WakeLockRegistrant(Handler h, int what, Object obj, WakeLock wakeLock, long timeout) {
        super(h, what, obj);
        mNotifyWakeLock = wakeLock;
        mWakeLockTimeOut = timeout;
    }

    public WakeLockRegistrant(Handler h, int what, Object obj, WakeLock wakeLock) {
        this(h, what, obj, wakeLock, UNSOL_WAKELOCK_TIMEOUT_MS);
    }

    public WakeLockRegistrant(Handler h, int what, Object obj, Context context, long timeout) {
        super(h, what, obj);
        if (context != null) {
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            if (pm != null) {
                mNotifyWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, WAKELOCK_NAME);
                if (mNotifyWakeLock != null) {
                    mNotifyWakeLock.setReferenceCounted(false);
                }
            }
        }
        mWakeLockTimeOut = timeout;
    }

    public WakeLockRegistrant(Handler h, int what, Object obj, Context context) {
        this(h, what, obj, context, UNSOL_WAKELOCK_TIMEOUT_MS);
    }

    @Override
    public void notifyRegistrant() {
        acquireNotifyWakeLock();
        super.notifyRegistrant();
    }

    @Override
    public void notifyResult(Object result) {
        acquireNotifyWakeLock();
        super.notifyResult(result);
    }

    @Override
    public void notifyException(Throwable exception) {
        acquireNotifyWakeLock();
        super.notifyException(exception);
    }

    @Override
    public void notifyRegistrant(AsyncResult ar) {
        acquireNotifyWakeLock();
        super.notifyRegistrant(ar);
    }

    private void acquireNotifyWakeLock() {
        if (mNotifyWakeLock != null) {
            Log.v(this, "acquireNotifyWakeLock");
            mNotifyWakeLock.acquire(mWakeLockTimeOut);
        }
    }
}
