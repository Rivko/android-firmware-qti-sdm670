/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.os.SystemClock;
import android.telephony.Rlog;

import vendor.qti.hardware.radio.qtiradio.V1_0.IQtiRadioIndication;

public class QtiRadioIndication extends IQtiRadioIndication.Stub {

    QtiRIL mRil;
    static final String QTI_RILJ_LOG_TAG = "QtiRadioIndication";

    public QtiRadioIndication(QtiRIL ril) {
        mRil = ril;
    }

    public void qtiRadioIndication(int type) {
        //Dummy function, if any UNSOL implemantion, meed to extend this file
    }
}
