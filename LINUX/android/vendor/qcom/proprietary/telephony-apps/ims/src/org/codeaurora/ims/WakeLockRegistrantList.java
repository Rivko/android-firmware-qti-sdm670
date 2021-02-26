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
import android.os.RegistrantList;

import com.qualcomm.ims.utils.Log;

/**
 * {@hide}
 */
public class WakeLockRegistrantList extends RegistrantList {

    private WakeLock mNotifyWakeLock;
    private long mWakeLockTimeOut;

    public WakeLockRegistrantList(WakeLock wakeLock, long timeout) {
        mNotifyWakeLock = wakeLock;
        mWakeLockTimeOut = timeout;
    }

    public WakeLockRegistrantList(WakeLock wakeLock) {
        this(wakeLock, WakeLockRegistrant.UNSOL_WAKELOCK_TIMEOUT_MS);
    }

    public WakeLockRegistrantList(Context context, long timeout) {
        if (context != null) {
            PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
            if (pm != null) {
                mNotifyWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,
                        WakeLockRegistrant.WAKELOCK_NAME);
                if (mNotifyWakeLock != null) {
                    mNotifyWakeLock.setReferenceCounted(false);
                }
            }
        }
        mWakeLockTimeOut = timeout;
    }

    public WakeLockRegistrantList(Context context) {
        this(context, WakeLockRegistrant.UNSOL_WAKELOCK_TIMEOUT_MS);
    }

    @Override
    public void notifyRegistrants() {
        acquireNotifyWakeLock();
        super.notifyRegistrants();
    }

    @Override
    public void notifyException(Throwable exception) {
        acquireNotifyWakeLock();
        super.notifyException(exception);
    }

    @Override
    public void notifyResult(Object result) {
        acquireNotifyWakeLock();
        super.notifyResult(result);
    }

    @Override
    public void notifyRegistrants(AsyncResult ar) {
        acquireNotifyWakeLock();
        super.notifyRegistrants(ar);
    }

    private void acquireNotifyWakeLock() {
        if (mNotifyWakeLock != null) {
            Log.v(this, "acquireNotifyWakeLock");
            mNotifyWakeLock.acquire(mWakeLockTimeOut);
        }
    }
}
