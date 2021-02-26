/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.telephony.Rlog;
import android.util.Log;

import org.codeaurora.internal.IDepersoResCallback;

public class QtiDepersoSupplier {
    private static final String LOG_TAG = "QtiDepersoSupplier";
    private static QtiDepersoSupplier sInstance;
    private Context mContext;
    private QtiRilInterface mQtiRilInterface;

    public static QtiDepersoSupplier make(Context context) {
        if (sInstance == null) {
            sInstance = new QtiDepersoSupplier(context);
        } else {
            Log.wtf(LOG_TAG, "QtiDepersoSupplier.make() should be called once");
        }
        return sInstance;
    }

    public static QtiDepersoSupplier getInstance() {
        if (sInstance == null) {
            Log.e(LOG_TAG, "QtiDepersoSupplier.getInstance called before make");
        }
        return sInstance;
    }

    private QtiDepersoSupplier(Context context) {
        mContext = context;
        mQtiRilInterface = QtiRilInterface.getInstance(context);
    }

    public void
    supplyIccDepersonalization(String netpin, String type, IDepersoResCallback callback,
            int phoneId) {
        if (!mQtiRilInterface.isServiceReady()) {
            Log.d(LOG_TAG, "Oem hook service is not ready yet ");
            return;
        }
        Log.d(LOG_TAG, "supplyIccDepersonalization");
        mQtiRilInterface.supplyIccDepersonalization(netpin, type, callback, phoneId);
    }
}
