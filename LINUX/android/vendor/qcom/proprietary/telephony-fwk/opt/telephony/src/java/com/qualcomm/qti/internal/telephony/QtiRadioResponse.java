/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.hardware.radio.V1_0.RadioResponseInfo;
import android.os.AsyncResult;
import android.os.Message;
import android.os.SystemClock;
import android.telephony.Rlog;

import vendor.qti.hardware.radio.qtiradio.V1_0.QtiRadioResponseInfo;
import vendor.qti.hardware.radio.qtiradio.V1_0.QtiRadioError;
import vendor.qti.hardware.radio.qtiradio.V1_0.IQtiRadioResponse;

public class QtiRadioResponse extends IQtiRadioResponse.Stub {

    QtiRIL mRil;
    static final String QTI_RILJ_LOG_TAG = "QtiRadioResponse";

    public QtiRadioResponse(QtiRIL ril) {
        mRil = ril;
    }

    /**
     * Helper function to send response msg
     * @param msg Response message to be sent
     * @param ret Return object to be included in the response message
     */
    static void sendMessageResponse(Message msg, Object ret) {
        if (msg != null) {
            AsyncResult.forMessage(msg, ret, null);
            msg.sendToTarget();
        }
    }

    static RadioResponseInfo toRadioResponseInfo (
            QtiRadioResponseInfo qtiResponseInfo) {
        RadioResponseInfo responseInfo = new RadioResponseInfo();
        responseInfo.type = qtiResponseInfo.type;
        responseInfo.serial = qtiResponseInfo.serial;
        responseInfo.error = qtiResponseInfo.error;
        return responseInfo;
    }

    private void responseString(RadioResponseInfo responseInfo, String str) {
        Object request = mRil.qtiProcessResponse(responseInfo);
        Message result = mRil.qtiGetMessageFromRequest(request);
        if (result != null) {
            if (responseInfo.error == QtiRadioError.NONE) {
                sendMessageResponse(result, str);
            }
           mRil.qtiProcessResponseDone(request, responseInfo, str);
        }
    }

   /**
     * @param responseInfo Response info struct containing response type, serial no. and error
     * @param atr String containing the ATR
     */
    @Override
    public void getAtrResponse(QtiRadioResponseInfo qtiResponseInfo, String atr) {
        Rlog.d(QTI_RILJ_LOG_TAG, "getAtrResponse");
        RadioResponseInfo responseInfo= toRadioResponseInfo(
                qtiResponseInfo);
        responseString(responseInfo, atr);
    }
}
