/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.Parcel;
import android.os.Registrant;
import android.os.RegistrantList;
import android.telephony.Rlog;
import android.util.Log;

import com.qualcomm.qcrilhook.OemHookCallback;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

public class SimulatedQtiRilInterface implements BaseRilInterface{
    private final static String LOG_TAG = "SimulatedQtiRilInterface";

    private static SimulatedQtiRilInterface sInstance = null;
    private QcRilHook mQcRilHook;
    private RegistrantList mServiceReadyRegistrantList;

    private static boolean mIsServiceReady = false;

    public static synchronized SimulatedQtiRilInterface getInstance(Context context) {
        if (sInstance == null) {
            sInstance = new SimulatedQtiRilInterface(context);
        } else {
            Log.wtf(LOG_TAG, "instance = " + sInstance);
        }

        return sInstance;
    }

    private SimulatedQtiRilInterface(Context context) {
        logd(" in constructor ");
        mServiceReadyRegistrantList = new RegistrantList();
    }

    public void getOmhCallProfile(int modemApnType, Message callbackMsg, int phoneId) {
        logd("getOmhCallProfile, modemApnType: " + modemApnType + ", phoneId: " + phoneId);
        Message msg = Message.obtain(callbackMsg);
        msg.obj = new AsyncResult(callbackMsg.obj, null , null);
        msg.sendToTarget();
    }

    public void sendPhoneStatus(int isReady, int phoneId) {
        logd("sendPhoneStatus, isReady: " + isReady + ", phoneId: " + phoneId);
    }

    public boolean isServiceReady() {
         return true;
    }

    public void registerForServiceReadyEvent(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mServiceReadyRegistrantList.add(r);
        if (isServiceReady()) {
            r.notifyRegistrant(new AsyncResult(null, mIsServiceReady, null));
        }
    }

    public void unRegisterForServiceReadyEvent(Handler h) {
        mServiceReadyRegistrantList.remove(h);
    }

    public boolean setLocalCallHold(int phoneId, boolean enable) {
        return true;
    }

    private void logd(String string) {
        Rlog.d(LOG_TAG, string);
    }
}
