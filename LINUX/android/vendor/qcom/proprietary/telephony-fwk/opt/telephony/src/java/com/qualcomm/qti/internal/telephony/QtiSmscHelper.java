/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.telephony.Rlog;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import java.util.concurrent.atomic.AtomicReferenceArray;

public class QtiSmscHelper extends Handler {
    private static final String LOG_TAG = "QtiSmscHelper";
    private static final int PHONE_COUNT = TelephonyManager.getDefault().getPhoneCount();
    private static final int EVENT_SET_SMSC = 1;
    private static final int EVENT_GET_SMSC = 2;

    private Object mSetLock = new Object();
    private Object mGetLock = new Object();

    private final Phone[] mPhones = PhoneFactory.getPhones();
    private final AtomicReferenceArray mSmscArray =
            new AtomicReferenceArray(new String[PHONE_COUNT]);
    private volatile boolean mSuccess = false;

    public QtiSmscHelper() {
        super(Looper.getMainLooper());
    }

    public void handleMessage(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;;
        switch (msg.what) {
            case EVENT_GET_SMSC:
                synchronized (mGetLock) {
                    if (ar != null && ar.exception == null) {
                        Rlog.d(LOG_TAG, "smsc = " + ar.result + " on phone = " + msg.arg1);
                        mSmscArray.set(msg.arg1, ar.result);
                    }
                    mGetLock.notifyAll();
                }
                break;
            case EVENT_SET_SMSC:
                synchronized (mSetLock) {
                    mSuccess = ar != null && ar.exception == null;
                    mSetLock.notifyAll();
                }
                break;
        }
    }

    public boolean setSmscAddress(final int slotId, final String smsc) {
        if (!isValidPhoneId(slotId) ) {
            Rlog.d(LOG_TAG, "Invalid phone id = " + slotId);
            return false;
        }
        synchronized (mSetLock) {
            if (TextUtils.equals((String)mSmscArray.get(slotId), smsc)) {
                Rlog.d(LOG_TAG, "the same smsc is there on phone = " + slotId);
                return true;
            }
            mPhones[slotId].setSmscAddress(smsc, obtainMessage(EVENT_SET_SMSC, slotId, -1));
            // Wait for the request to complete
            try {
                mSetLock.wait();
            } catch (InterruptedException e) {
                // Do nothing, go back and wait until the request is complete
            }

            if (mSuccess) mSmscArray.set(slotId, smsc);
            return mSuccess;
        }
    }

    public String getSmscAddress(final int slotId) {
        if (!isValidPhoneId(slotId) ) {
            Rlog.d(LOG_TAG, "Invalid phone id = " + slotId);
            return null;
        }
        synchronized (mGetLock) {
            mPhones[slotId].getSmscAddress(obtainMessage(EVENT_GET_SMSC, slotId, -1));
            // Wait for the request to complete
            try {
                mGetLock.wait();
            } catch (InterruptedException e) {
                // Do nothing, go back and wait until the request is complete
            }
            return (String)mSmscArray.get(slotId);
        }
    }

    private boolean isValidPhoneId(final int slotId) {
        return slotId >= 0 && slotId < PHONE_COUNT;
    }
}
