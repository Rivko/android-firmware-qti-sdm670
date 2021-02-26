/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import static com.android.internal.telephony.RILConstants.*;
import static com.android.internal.util.Preconditions.checkNotNull;

import android.content.Context;
import android.hardware.radio.V1_0.RadioResponseInfo;
import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HwBinder;
import android.os.Message;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.WorkSource;
import android.telephony.Rlog;

import com.android.internal.telephony.RIL;
import com.android.internal.telephony.CommandException;

import vendor.qti.hardware.radio.qtiradio.V1_0.IQtiRadio;
import vendor.qti.hardware.radio.qtiradio.V1_0.QtiRadioResponseInfo;

import java.util.concurrent.atomic.AtomicLong;


/**
 * QtiRIL extends RIL.
 *
 * {@hide}
 */
public final class QtiRIL extends RIL {
    static final String TAG = "QTIRILJ";
    static final String[] QTI_HIDL_SERVICE_NAME = {"slot1", "slot2", "slot3"};
    QtiRadioResponse mQtiRadioResponse;
    QtiRadioIndication mQtiRadioIndication;
    private IQtiRadio mQtiRadio;
    int mQtiPhoneId = 0;
    final QtiRadioProxyDeathRecipient mDeathRecipient;
    final AtomicLong mQtiRadioProxyCookie = new AtomicLong(0);
    // Notification object used to listen to the start of the IQtiRadio
    private final QtiRadioServiceNotification mServiceNotification =
        new QtiRadioServiceNotification();

    final class QtiRadioProxyDeathRecipient implements HwBinder.DeathRecipient {
        @Override
        public void serviceDied(long cookie) {
            // Deal with service going away
            Rlog.d(TAG, "serviceDied");
            resetServiceAndRequestList();
        }
    }

    private void resetServiceAndRequestList() {
        resetProxyAndRequestList();
        mQtiRadio = null;
        mQtiRadioResponse = null;
        mQtiRadioIndication = null;
        mQtiRadioProxyCookie.incrementAndGet();
    }

    /**
     * Returns true if we are connected to IQtiRadio service, false otherwise.
     */
    private boolean isQtiRadioServiceConnected() {
        return (mQtiRadio != null);
    }

    /**
     * Class that implements the service notification which gets called once the
     * service with fully qualified name fqName has started
     */
    final class QtiRadioServiceNotification extends IServiceNotification.Stub {
        /**
         * Callback that gets called when the service has registered
         * @param fqName - Fully qualified name of the service
         * @param name - Name of the service
         * @param preexisting - if the registration is preexisting
         */
        @Override
        public void onRegistration(String fqName, String name, boolean preexisting) {
            Rlog.d(TAG, "QtiRadio interface service started " + fqName + " " + name +
                " preexisting =" + preexisting);
            if (!isQtiRadioServiceConnected()) {
                initQtiRadio();
            }
        }
    }

    /**
     * Register for notification when the com.qualcomm.qti.qtiradio@1.0::IQtiRadio is registered
     */
    private void registerForQtiRadioServiceNotification() {
        try {
            final boolean ret = IServiceManager.getService()
                    .registerForNotifications("vendor.qti.hardware.radio.qtiradio@1.0::IQtiRadio",
                    QTI_HIDL_SERVICE_NAME[mQtiPhoneId], mServiceNotification);
            if (!ret) {
                Rlog.e(TAG, "Failed to register for service start notifications");
            }
        } catch (RemoteException ex) {
            Rlog.e(TAG, "Failed to register for service start notifications. Exception " + ex);
        }
    }

    /**
     * Initialize the instance of IQtiRadio. Get the service and register the callback object
     * to be called for the responses to the solicited and unsolicited requests.
     */
    private synchronized void initQtiRadio() {
        try {
            mQtiRadio = IQtiRadio.getService(QTI_HIDL_SERVICE_NAME[mQtiPhoneId]);
            if (mQtiRadio == null) {
                Rlog.e(TAG, "initQtiRadio: mQtiRadio is null. Return");
                return;
            }
            Rlog.d(TAG, "initQtiRadio: mQtiRadio" + mQtiRadio);
            mQtiRadio.linkToDeath(mDeathRecipient,
                    mQtiRadioProxyCookie.incrementAndGet());
            mQtiRadioResponse = new QtiRadioResponse(this);
            mQtiRadioIndication = new QtiRadioIndication(this);
            mQtiRadio.setCallback(mQtiRadioResponse, mQtiRadioIndication);
        } catch (Exception ex) {
            Rlog.e(TAG, "initQtiRadio: Exception: " + ex);
            resetServiceAndRequestList();
        }
    }
    //***** Constructors

    public QtiRIL(Context context, int preferredNetworkType, int cdmaSubscription) {
        this(context, preferredNetworkType, cdmaSubscription, null);
    }

    public QtiRIL(Context context, int preferredNetworkType, int cdmaSubscription,
            Integer instanceId) {
        super(context, preferredNetworkType, cdmaSubscription, instanceId);
        mQtiPhoneId = instanceId;
        Rlog.d(TAG, "QtiRIL");
        mDeathRecipient = new QtiRadioProxyDeathRecipient();
        registerForQtiRadioServiceNotification();
    }

    private String convertNullToEmptyString(String string) {
        return string != null ? string : "";
    }

    @Override
    public void getAtr(Message result) {
        Rlog.d(TAG, "getAtr");
        int serial = obtainRequestSerial(RIL_REQUEST_SIM_QUERY_ATR, result,
                mRILDefaultWorkSource);
        try {
            mQtiRadio.getAtr(serial);
        } catch (RemoteException | RuntimeException e) {
            Rlog.e(TAG, "getAtr: Exception: " + e);
            resetServiceAndRequestList();
        }
    }

    /* package */
    Message qtiGetMessageFromRequest (Object request) {
        return getMessageFromRequest (request);
    }

    /* package */
    Object qtiProcessResponse(RadioResponseInfo responseInfo) {
        return processResponse(responseInfo);
    }

    void qtiProcessResponseDone(Object ret, RadioResponseInfo responseInfo, String str) {
        processResponseDone(ret, responseInfo, str);
    }
}
