/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.util.Log;
import android.os.Looper;
import android.os.Handler;
import android.os.IDeviceIdleController;
import android.os.PowerManager;
import android.os.ServiceManager;
import android.telephony.Rlog;

import com.android.ims.ImsManager;
import com.android.internal.telephony.cdma.CdmaSubscriptionSourceManager;
import com.android.internal.telephony.cdma.EriManager;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.telephony.imsphone.ImsPhone;
import com.android.internal.telephony.imsphone.ImsPhoneCallTracker;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneNotifier;
import com.android.internal.telephony.PhoneSwitcher;
import com.android.internal.telephony.RIL;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.uicc.IccCardProxy;

import com.android.internal.telephony.TelephonyComponentFactory;
import com.android.internal.telephony.*;
import com.qualcomm.qti.internal.telephony.dataconnection.QtiDcTracker;

import com.qualcomm.qti.internal.telephony.primarycard.QtiPrimaryCardController;

public class QtiTelephonyComponentFactory extends TelephonyComponentFactory {
    private static String LOG_TAG = "QtiTelephonyComponentFactory";

    private QtiSmsSecurityService mSmsSecurityService;

    public QtiTelephonyComponentFactory() {
    }

    @Override
    public GsmCdmaCallTracker makeGsmCdmaCallTracker(GsmCdmaPhone phone) {
        Rlog.d(LOG_TAG, "makeGsmCdmaCallTracker");
        return super.makeGsmCdmaCallTracker(phone);
    }

    @Override
    public SmsStorageMonitor makeSmsStorageMonitor(Phone phone) {
        Rlog.d(LOG_TAG, "makeSmsStorageMonitor");
        return super.makeSmsStorageMonitor(phone);
    }

    @Override
    public SmsUsageMonitor makeSmsUsageMonitor(Context context) {
        Rlog.d(LOG_TAG, "makeSmsUsageMonitor");
        return new QtiSmsUsageMonitor(context, getSmsSecurityService(context));
    }

    @Override
    public ServiceStateTracker makeServiceStateTracker(GsmCdmaPhone phone, CommandsInterface ci) {
        Rlog.d(LOG_TAG, "makeQtiServiceStateTracker");
        return new QtiServiceStateTracker(phone, ci);
    }

    @Override
    public DcTracker makeDcTracker(Phone phone) {
        Rlog.d(LOG_TAG, "makeQtiDcTracker");
        return new QtiDcTracker(phone);
    }

    @Override
    public IccPhoneBookInterfaceManager makeIccPhoneBookInterfaceManager(Phone phone) {
        Rlog.d(LOG_TAG, "makeQtiIccPhoneBookInterfaceManager");
        return new QtiIccPhoneBookInterfaceManager(phone);
    }

    @Override
    public IccSmsInterfaceManager makeIccSmsInterfaceManager(Phone phone) {
        Rlog.d(LOG_TAG, "makeIccSmsInterfaceManager");
        return super.makeIccSmsInterfaceManager(phone);
    }

    @Override
    public IccCardProxy makeIccCardProxy(Context context, CommandsInterface ci, int phoneId) {
        Rlog.d(LOG_TAG, "makeIccCardProxy");
        return super.makeIccCardProxy(context, ci, phoneId);
    }

    @Override
    public EriManager makeEriManager(Phone phone, Context context, int eriFileSource) {
        Rlog.d(LOG_TAG, "makeEriManager");
        return super.makeEriManager(phone, context, eriFileSource);
    }

    @Override
    public WspTypeDecoder makeWspTypeDecoder(byte[] pdu) {
        Rlog.d(LOG_TAG, "makeWspTypeDecoder");
        return super.makeWspTypeDecoder(pdu);
    }

    @Override
    public InboundSmsTracker makeInboundSmsTracker(byte[] pdu, long timestamp, int destPort,
            boolean is3gpp2, boolean is3gpp2WapPdu, String address, String displayAddr,
            String msgBody) {
        Rlog.d(LOG_TAG, "makeInboundSmsTracker");
        return super.makeInboundSmsTracker(pdu, timestamp, destPort, is3gpp2,
                is3gpp2WapPdu, address, displayAddr, msgBody);
    }

    @Override
    public InboundSmsTracker makeInboundSmsTracker(byte[] pdu, long timestamp, int destPort,
            boolean is3gpp2, String address, String displayAddr, int referenceNumber,
            int sequenceNumber, int messageCount, boolean is3gpp2WapPdu, String msgBody) {
        Rlog.d(LOG_TAG, "makeInboundSmsTracker");
        return super.makeInboundSmsTracker(pdu, timestamp, destPort, is3gpp2,
                address, displayAddr, referenceNumber, sequenceNumber, messageCount,
                is3gpp2WapPdu, msgBody);
    }

    @Override
    public ImsPhoneCallTracker makeImsPhoneCallTracker(ImsPhone imsPhone) {
        Rlog.d(LOG_TAG, "makeImsPhoneCallTracker");
        return super.makeImsPhoneCallTracker(imsPhone);
    }

    @Override
    public CdmaSubscriptionSourceManager
    getCdmaSubscriptionSourceManagerInstance(Context context, CommandsInterface ci, Handler h,
                                             int what, Object obj) {
        Rlog.d(LOG_TAG, "getCdmaSubscriptionSourceManagerInstance");
        return super.getCdmaSubscriptionSourceManagerInstance(context, ci, h, what, obj);
    }

    @Override
    public IDeviceIdleController getIDeviceIdleController() {
        Rlog.d(LOG_TAG, "getIDeviceIdleController");
        return super.getIDeviceIdleController();
    }

    @Override
    public Phone makePhone(Context context, CommandsInterface ci, PhoneNotifier notifier,
            int phoneId, int precisePhoneType,
            TelephonyComponentFactory telephonyComponentFactory) {
        Rlog.d(LOG_TAG, "makePhone");
        return new QtiGsmCdmaPhone(context, ci, notifier, phoneId, precisePhoneType,
                telephonyComponentFactory);
    }

    @Override
    public SubscriptionController initSubscriptionController(Context c, CommandsInterface[] ci) {
        Rlog.d(LOG_TAG, "initSubscriptionController");
        return QtiSubscriptionController.init(c, ci);
    }

    @Override
    public SubscriptionInfoUpdater makeSubscriptionInfoUpdater(Looper looper, Context context, Phone[] phones,
            CommandsInterface[] ci) {
        Rlog.d(LOG_TAG, "makeSubscriptionInfoUpdater");
        return QtiSubscriptionInfoUpdater.init(looper, context, phones, ci);
    }

    @Override
    public void makeExtTelephonyClasses(Context context,
            Phone[] phones, CommandsInterface[] commandsInterfaces) {
        Rlog.d(LOG_TAG, " makeExtTelephonyClasses ");
        QtiUiccCardProvisioner.make(context);
        QtiDepersoSupplier.make(context);
        QtiRadioCapabilityController.make(context, phones, commandsInterfaces);
        QtiPrimaryCardController.init(context, phones, commandsInterfaces);
        try {
            ExtTelephonyServiceImpl.init(context);
        } catch (NoClassDefFoundError e) {
            e.printStackTrace();
            Rlog.e(LOG_TAG, "Error creating ExtTelephonyServiceImpl");
        }
    }

    @Override
    public PhoneSwitcher makePhoneSwitcher(int maxActivePhones, int numPhones, Context context,
            SubscriptionController sc, Looper looper, ITelephonyRegistry tr,
            CommandsInterface[] cis, Phone[] phones) {
        Rlog.d(LOG_TAG, "makeQtiPhoneSwitcher");
        return new QtiPhoneSwitcher(maxActivePhones, numPhones, context, sc,
                looper, tr, cis, phones);
    }

    private synchronized QtiSmsSecurityService getSmsSecurityService(final Context context) {
        if (mSmsSecurityService == null) {
            mSmsSecurityService = new QtiSmsSecurityService(context);
            ServiceManager.addService(QtiSmsSecurityService.SERVICE_NAME, mSmsSecurityService);
        }
        return mSmsSecurityService;
    }

    @Override
    public RIL makeRIL(Context context, int preferredNetworkType,
            int cdmaSubscription, Integer instanceId) {
        Rlog.d(LOG_TAG, "makeQtiRIL");
        return new QtiRIL(context, preferredNetworkType, cdmaSubscription, instanceId);
    }

}
