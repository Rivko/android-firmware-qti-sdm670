/*
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.internal.telephony;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.Parcel;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.Rlog;
import android.text.TextUtils;
import android.util.Log;

import com.qualcomm.qti.internal.telephony.dataconnection.QtiApnProfileOmh;
import com.qualcomm.qti.internal.telephony.dataconnection.QtiApnSetting;
import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner.UiccProvisionStatus;
import com.qualcomm.qti.internal.telephony.uicccontact.QtiSimPhoneBookAdnRecord;
import com.android.internal.telephony.uicc.IccUtils;

import com.qualcomm.qcrilhook.OemHookCallback;
import com.qualcomm.qcrilhook.QcRilHook;
import com.qualcomm.qcrilhook.QcRilHookCallback;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

import org.codeaurora.internal.IDepersoResCallback;

/**
 *  Its an interface to OemHook library from Telephony framework.
 *  This class holds the library functions to frame data as expected
 *  by OEM HOOK framework and calls QcRilHook methods to communicate
 *  with RIL over qcrilmsgtunnel socket.
 */
public class QtiRilInterface implements BaseRilInterface{
    private final static String LOG_TAG = "QtiRilInterface";

    private final static int INT_SIZE = 4;
    private final static int BYTE_SIZE = 1;
    private final static int SHORT_SIZE = 2;

    private static final char NULL_TERMINATOR = '\0';

    private static final int NULL_TERMINATOR_LENGTH = BYTE_SIZE;

    private static QtiRilInterface sInstance = null;
    private QcRilHook mQcRilHook;
    private RegistrantList mServiceReadyRegistrantList;
    private RegistrantList mAdnInitDoneRegistrantList;
    private RegistrantList mAdnRecordsInfoRegistrantList;

    private static boolean mIsServiceReady = false;
    private String OMH_FAKE_QCRIL_HOOK_RESPONSE = "persist.test.omh.fakeprofile";
    private final String ACTION_ADN_INIT_DONE = "qualcomm.intent.action.ACTION_ADN_INIT_DONE";
    private final String ACTION_ADN_RECORDS_IND = "qualcomm.intent.action.ACTION_ADN_RECORDS_IND";

    /**
     * Called when connection to QcrilMsgTunnelService has been established.
     */
    private QcRilHookCallback mQcrilHookCb = new QcRilHookCallback() {
        public void onQcRilHookReady() {
            mIsServiceReady = true;
            logd("Service ready, notifying registrants");

            AsyncResult ar = new AsyncResult(null, mIsServiceReady, null);
            mServiceReadyRegistrantList.notifyRegistrants(ar);
        }

        public synchronized void onQcRilHookDisconnected() {
            mIsServiceReady = false;
            logd("Service disconnected, notifying registrants");

            AsyncResult ar = new AsyncResult(null, mIsServiceReady, null);
            mServiceReadyRegistrantList.notifyRegistrants(ar);
            sInstance = null;
        }
    };

    public static synchronized QtiRilInterface getInstance(Context context) {
        if (sInstance == null) {
            sInstance = new QtiRilInterface(context);
        } else {
            Log.d(LOG_TAG, "instance = " + sInstance);
        }

        return sInstance;
    }

    private QtiRilInterface(Context context) {
        logd(" in constructor ");

        mServiceReadyRegistrantList = new RegistrantList();
        mAdnInitDoneRegistrantList = new RegistrantList();
        mAdnRecordsInfoRegistrantList = new RegistrantList();
        try {
            mQcRilHook = new QcRilHook(context, mQcrilHookCb);
        } catch(SecurityException se) {
            loge("SecurityException during QcRilHook init: " + se);
            mIsServiceReady = false;
        }

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_ADN_INIT_DONE);
        intentFilter.addAction(ACTION_ADN_RECORDS_IND);
        context.registerReceiver(mBroadcastReceiver, intentFilter);
    }

    public UiccProvisionStatus getUiccProvisionPreference(int phoneId) {
        UiccProvisionStatus provStatus = new UiccProvisionStatus();
        byte[] requestData = new byte[0];

        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_GET_UICC_PROVISION_PREFERENCE, requestData, phoneId);
        if ((ar.exception == null) && (ar.result != null)) {
            ByteBuffer byteBuf = ByteBuffer.wrap((byte[]) ar.result);
            byteBuf.order(ByteOrder.nativeOrder());

            logd("Data received: " + byteBuf.toString());

            provStatus.setUserPreference(byteBuf.getInt());
            provStatus.setCurrentState(byteBuf.getInt());
        }

        logi("get pref, phoneId " + phoneId + " " + provStatus + " exception " + ar.exception);
        return provStatus;
    }

    private ArrayList<QtiApnSetting> parseOmhProfiles(byte[] buffer) {
        ArrayList<QtiApnSetting> profilesList = new ArrayList<QtiApnSetting>();
        ByteBuffer byteBuf = ByteBuffer.wrap(buffer);
        if(byteBuf != null) {
            byteBuf.order(ByteOrder.nativeOrder());

            logi("Data received: " + byteBuf.toString());

            int nProfiles = byteBuf.getInt();

            int profileId = 0;
            int priority = 0;

            for (int i = 0; i < nProfiles; i++) {
                profileId = byteBuf.getInt();
                priority = byteBuf.getInt();
                QtiApnProfileOmh profile = new QtiApnProfileOmh(profileId, priority);
                logi("getOmhCallProfile " + profileId + ":" + priority);
                profilesList.add(profile);
            }
        }

        return profilesList;

    }

    private ArrayList<QtiApnSetting> getFakeOmhProfiles(int profileId) {

        int[] prioritySortedProfiles = {2, 32, 64, 1}; //MMS, SUPL, DUN, DEFAULT

        ArrayList<QtiApnSetting> profilesList = new ArrayList();
        for(int i = 0; i < prioritySortedProfiles.length; i++) {
            if (prioritySortedProfiles[i] == profileId) {
                QtiApnProfileOmh profile = new QtiApnProfileOmh(prioritySortedProfiles[i], i);
                logi("profile(id=" + profileId + ") =" + profile);
                profilesList.add(profile);
                break;
            }
        }
        return profilesList;
    }

    private class OmhCallProfileCallback extends OemHookCallback {
        Message mAppMessage;
        int mModemApnType;

        public OmhCallProfileCallback(int modemApnType, Message msg) {
            super(msg);
            mAppMessage = msg;
            mModemApnType = modemApnType;
        }

        @Override
        public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
            ArrayList<QtiApnSetting> profiles = new ArrayList<QtiApnSetting>();
            boolean fakeResponse = SystemProperties.getBoolean(OMH_FAKE_QCRIL_HOOK_RESPONSE,
                    false);

            if (fakeResponse) {
                logi("Getting fake omh profiles");
                profiles = getFakeOmhProfiles(mModemApnType);
                AsyncResult.forMessage(mAppMessage, profiles, null);
            } else {
                if (response != null) {
                    Log.d(LOG_TAG, "getOmhCallProfile: onOemHookResponse = " + response.toString());
                    profiles = parseOmhProfiles(response);
                    AsyncResult.forMessage(mAppMessage, profiles, null);
                } else {
                    AsyncResult.forMessage(mAppMessage, profiles,
                            new Exception("QCRIL_EVT_HOOK_GET_OMH_CALL_PROFILE failed"));
                }
            }
            mAppMessage.sendToTarget();
        }
    }

    public void getOmhCallProfile(int modemApnType, Message callbackMsg, int phoneId) {
        logi("getOmhCallProfile()");
        byte[] requestData = new byte[INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.putInt(modemApnType);
        OemHookCallback oemHookCb = new OmhCallProfileCallback(modemApnType, callbackMsg);

        mQcRilHook.sendQcRilHookMsgAsync(
                QcRilHook.QCRIL_EVT_HOOK_GET_OMH_CALL_PROFILE,
                requestData,
                oemHookCb,
                phoneId);
    }

    private class DepersoCallback extends OemHookCallback {
        IDepersoResCallback depersoCallBack;
        int SUCCESS = 0;
        int ERROR = 1;

        public DepersoCallback(IDepersoResCallback callback, Message msg) {
            super(msg);
            depersoCallBack = callback;
        }

        @Override
        public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
            if (response != null) {
                logd("DepersoResult SUCCESS");
                depersoCallBack.onDepersoResult(SUCCESS, phoneId);
            } else {
                logd("DepersoResult ERROR");
                depersoCallBack.onDepersoResult(ERROR, phoneId);
            }
        }

        @Override
        public void onOemHookException(int phoneId) throws RemoteException  {
            logd("DepersoResult ERROR");
            depersoCallBack.onDepersoResult(ERROR, phoneId);
        }
    }

    public void
    supplyIccDepersonalization(String netpin, String type, IDepersoResCallback callback,
            int phoneId) {
        logd("supplyDepersonalization: netpin = " + netpin + " type = " + type
                + "phoneId = " +phoneId);

        Message msg = Message.obtain();
        byte[] payload = null;
        // type + null character +
        // netpin + null character
        int payloadLength  = type.length() + NULL_TERMINATOR_LENGTH +
                (netpin == null ? NULL_TERMINATOR_LENGTH
                : netpin.length() + NULL_TERMINATOR_LENGTH);

        payload = new byte[payloadLength];
        ByteBuffer buf = mQcRilHook.createBufferWithNativeByteOrder(payload);
        // type
        buf.put(type.getBytes());
        buf.put((byte)NULL_TERMINATOR); // null character
        // pin
        if (netpin != null) buf.put(netpin.getBytes());
        buf.put((byte)NULL_TERMINATOR); // null character

        OemHookCallback oemHookCb = new DepersoCallback(callback, msg);
        mQcRilHook.sendQcRilHookMsgAsync(
                QcRilHook.QCRIL_EVT_HOOK_ENTER_DEPERSONALIZATION_CODE, payload, oemHookCb,
                phoneId);
    }

    public String getUiccIccId(int phoneId) {
        String iccId = null;
        byte[] requestData = new byte[INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.putInt(phoneId);
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_GET_UICC_ICCID, requestData, phoneId);
        if ((ar.exception == null) && (ar.result != null)) {
            iccId = new String((byte[])ar.result);
        }

        logi("getUiccIccId iccId[" + phoneId + "] = " + iccId + " exception: " + ar.exception);
        return iccId;
    }

    public int getMaxDataAllowed() {
        int maxData = 0;
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_GET_MAX_DATA_ALLOWED);
        if ((ar.exception == null) && (ar.result != null)) {
            byte[] response = (byte[]) ar.result;
            maxData = ByteBuffer.wrap(response).get();
        }

        logi("getMaxDataAllowed maxData = " + maxData + " exception: " + ar.exception);
        return maxData;
    }

    public boolean getLpluslSupportStatus() {
        boolean status = false;
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_REQ_HOOK_GET_L_PLUS_L_FEATURE_SUPPORT_STATUS_REQ);
        if ((ar.exception == null) && (ar.result != null)) {
            byte[] response = (byte[]) ar.result;
            status = (ByteBuffer.wrap(response).get() & 1) == 1;
        }

        logi("getLpluslSupportStatus: " + status + " exception: " + ar.exception);
        return status;
    }

    public void sendPhoneStatus(int isReady, int phoneId) {
        String iccId = null;
        byte[] requestData = new byte[BYTE_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.put((byte)isReady);
        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_SET_ATEL_UI_STATUS, requestData, phoneId);
    }


    public boolean setUiccProvisionPreference(int userPref, int phoneId) {
        boolean retval = false;
        byte[] requestData = new byte[2*INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);

        reqBuffer.putInt(userPref);
        reqBuffer.putInt(phoneId);

        AsyncResult ar = mQcRilHook.sendQcRilHookMsg(
                QcRilHook.QCRIL_EVT_HOOK_SET_UICC_PROVISION_PREFERENCE, requestData, phoneId);
        if (ar.exception == null) {
            retval = true;
        }

        logi("set provision userPref " + userPref + " phoneId " + phoneId
                + " exception: " + ar.exception);
        return retval;
    }

    private int[] parseInts(byte[] buffer, int numInts) {
        int response[];

        ByteBuffer byteBuf = ByteBuffer.wrap(buffer);
        byteBuf.order(ByteOrder.nativeOrder());

        logi("numInts: " + numInts);
        response = new int[numInts];
        for (int i = 0 ; i < numInts ; i++) {
            response[i] = byteBuf.getInt();
            logi("response[i]: " + response[i]);
        }

        return response;
    }

    protected class AdnOemHookCallback extends OemHookCallback {
        Message mAppMessage;
        int mRspLength;

        public AdnOemHookCallback(Message msg, int length) {
            super(msg);
            mAppMessage = msg;
            mRspLength = length;
        }

        @Override
        public void onOemHookResponse(byte[] response, int phoneId) throws RemoteException {
            if (response != null) {
                logd("AdnOemHookCallback: onOemHookResponse = " + response.toString());

                int[] values = parseInts(response, mRspLength);
                AsyncResult.forMessage(mAppMessage, values, null);
            } else {
                AsyncResult.forMessage(mAppMessage, null,
                        new Exception("QCRIL_EVT_HOOK_GET_ADN_RECORD failed"));
            }
            mAppMessage.sendToTarget();
        }

        @Override
        public void onOemHookException(int phoneId) throws RemoteException {
            logd("AdnOemHookCallback: onOemHookException");

            AsyncResult.forMessage(mAppMessage, null,
                    new Exception("com.android.internal.telephony.CommandException: MODEM_ERR"));
            mAppMessage.sendToTarget();
        }
    }

    public void getAdnRecord(Message callbackMsg, int phoneId) {
        byte[] requestData = new byte[INT_SIZE];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);
        final int rspLength = 10;

        reqBuffer.putInt(phoneId);
        AdnOemHookCallback oemHookCb = new AdnOemHookCallback(callbackMsg, rspLength);

        mQcRilHook.sendQcRilHookMsgAsync(
                QcRilHook.QCRIL_EVT_HOOK_GET_ADN_RECORD,
                requestData,
                oemHookCb,
                phoneId);

        logi("getAdnRecord()");
    }

    public void updateAdnRecord(
            QtiSimPhoneBookAdnRecord adnRecordInfo, Message callbackMsg, int phoneId) {
        int numEmails = adnRecordInfo.getNumEmails();
        int numAdNumbers = adnRecordInfo.getNumAdNumbers();
        String name = adnRecordInfo.getAlphaTag();
        String number = adnRecordInfo.getNumber();

        int dataSize = 5*SHORT_SIZE +
                + (TextUtils.isEmpty(name) ? 0 : (name.getBytes().length + NULL_TERMINATOR_LENGTH))
                + (TextUtils.isEmpty(number)
                        ? 0 : (number.getBytes().length + NULL_TERMINATOR_LENGTH));
        for (int i = 0 ; i < numEmails; i++) {
            dataSize += SHORT_SIZE;
            dataSize += adnRecordInfo.getEmails()[i].getBytes().length + NULL_TERMINATOR_LENGTH;
        }
        for (int j = 0 ; j < numAdNumbers; j++) {
            dataSize += SHORT_SIZE;
            dataSize += adnRecordInfo.getAdNumbers()[j].getBytes().length + NULL_TERMINATOR_LENGTH;
        }

        byte[] requestData = new byte[dataSize];
        ByteBuffer reqBuffer = mQcRilHook.createBufferWithNativeByteOrder(requestData);
        final int rspLength = 1;

        AdnOemHookCallback oemHookCb = new AdnOemHookCallback(callbackMsg, rspLength);
        //reqBuffer.putInt(phoneId);
        reqBuffer.putShort((short)adnRecordInfo.getRecordIndex());
        if(!TextUtils.isEmpty(name)) {
            reqBuffer.putShort((short)name.getBytes().length);
            try {
                reqBuffer.put(name.getBytes("UTF-8"));
                reqBuffer.put((byte)NULL_TERMINATOR);
            }
            catch (UnsupportedEncodingException usee) {
                 loge("Unsupport UTF-8 to parse name");
                 return;
            }
        } else {
            reqBuffer.putShort((short)0);
        }

        if(!TextUtils.isEmpty(number)) {
            reqBuffer.putShort((short)number.getBytes().length);
            try {
                reqBuffer.put(
                    QtiSimPhoneBookAdnRecord.ConvertToRecordNumber(number).getBytes("UTF-8"));
                reqBuffer.put((byte)NULL_TERMINATOR);
            }
            catch (UnsupportedEncodingException usee) {
                 loge("Unsupport UTF-8 to parse number");
                 return;
            }
        } else {
            reqBuffer.putShort((short)0);
        }

        reqBuffer.putShort((short)numEmails);
        for (int i = 0 ; i < numEmails; i++) {
            reqBuffer.putShort((short)adnRecordInfo.getEmails()[i].getBytes().length);
            try {
                reqBuffer.put(adnRecordInfo.getEmails()[i].getBytes("UTF-8"));
                reqBuffer.put((byte)NULL_TERMINATOR);
            }
            catch (UnsupportedEncodingException usee) {
                 loge("Unsupport UTF-8 to parse email");
                 return;
            }
        }

        reqBuffer.putShort((short)numAdNumbers);
        for (int j = 0 ; j < numAdNumbers; j++) {
            reqBuffer.putShort((short)adnRecordInfo.getAdNumbers()[j].getBytes().length);
            try {
                reqBuffer.put(
                    QtiSimPhoneBookAdnRecord.ConvertToRecordNumber(
                                adnRecordInfo.getAdNumbers()[j]).getBytes("UTF-8"));
                reqBuffer.put((byte)NULL_TERMINATOR);
            }
            catch (UnsupportedEncodingException usee) {
                 loge("Unsupport UTF-8 to parse anr");
                 return;
            }
        }

        mQcRilHook.sendQcRilHookMsgAsync(
                QcRilHook.QCRIL_EVT_HOOK_UPDATE_ADN_RECORD,
                requestData,
                oemHookCb,
                phoneId);

        logi("updateAdnRecord() with " + adnRecordInfo.toString());
    }

    public boolean isServiceReady() {
        return mIsServiceReady;
    }

    public void registerForUnsol(Handler handler, int event, Object obj) {
        mQcRilHook.register(handler, event, obj);
    }

    public void unRegisterForUnsol(Handler handler) {
        mQcRilHook.unregister(handler);
    }

    public void registerForServiceReadyEvent(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mServiceReadyRegistrantList.add(r);
        if (isServiceReady()) {
            r.notifyRegistrant(new AsyncResult(null, mIsServiceReady, null));
        }
    }

    public void registerForAdnInitDone(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mAdnInitDoneRegistrantList.add(r);
    }

    public void registerForAdnRecordsInfo(Handler h, int what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mAdnRecordsInfoRegistrantList.add(r);
    }

    public void qcRilSendDDSInfo(int ddsPhoneId, int reason, int rilId) {
        mQcRilHook.qcRilSendDDSInfo(ddsPhoneId, reason, rilId);
    }

    public void unRegisterForServiceReadyEvent(Handler h) {
        mServiceReadyRegistrantList.remove(h);
    }

    public void unregisterForAdnInitDone(Handler h) {
        mAdnInitDoneRegistrantList.remove(h);
    }

    public void unregisterForAdnRecordsInfo(Handler h) {
        mAdnRecordsInfoRegistrantList.remove(h);
    }

    public boolean setLocalCallHold(int phoneId, boolean enable) {
        return mQcRilHook.setLocalCallHold(phoneId, enable);
    }

    private Object responseAdnRecords(byte[] data) {
        ByteBuffer byteBuf = ByteBuffer.wrap(data);
        byteBuf.order(ByteOrder.nativeOrder());

        int numRecords = (int)byteBuf.getShort();

        QtiSimPhoneBookAdnRecord[] AdnRecordsInfoGroup =
                            new QtiSimPhoneBookAdnRecord[numRecords];

        for (int i = 0 ; i < numRecords ; i++) {
            AdnRecordsInfoGroup[i]= new QtiSimPhoneBookAdnRecord();

            AdnRecordsInfoGroup[i].mRecordIndex = byteBuf.getShort();

            int nameLength = byteBuf.getShort();
            if(nameLength > 0) {
                byte[] alphaTag = new byte[nameLength];
                byteBuf.get(alphaTag);
                try {
                    AdnRecordsInfoGroup[i].mAlphaTag = new String(alphaTag, "UTF-8");
                } catch (UnsupportedEncodingException e) {
                    loge("Unsupport UTF-8 to parse name");
                    break;
                }
            }

            int numberLength = byteBuf.getShort();
            if(numberLength > 0) {
                byte[] number = new byte[numberLength];
                byteBuf.get(number);
                try {
                    AdnRecordsInfoGroup[i].mNumber =
                        QtiSimPhoneBookAdnRecord.ConvertToPhoneNumber(new String(number, "UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    loge("Unsupport UTF-8 to parse number");
                    break;
                }
            }

            int numEmails = byteBuf.getShort();
            if(numEmails > 0) {
                AdnRecordsInfoGroup[i].mEmailCount = numEmails;
                AdnRecordsInfoGroup[i].mEmails = new String[numEmails];
                for (int j = 0 ; j < numEmails; j++) {
                    int emailLength = byteBuf.getShort();
                    if(emailLength > 0) {
                        byte[] email = new byte[emailLength];
                        byteBuf.get(email);
                        try {
                            AdnRecordsInfoGroup[i].mEmails[j] = new String(email, "UTF-8");
                        } catch (UnsupportedEncodingException e) {
                            loge("Unsupport UTF-8 to parse email");
                            break;
                        }
                    }
                }
            }

            int numAnrs = byteBuf.getShort();
            if(numAnrs > 0) {
                AdnRecordsInfoGroup[i].mAdNumCount = numAnrs;
                AdnRecordsInfoGroup[i].mAdNumbers = new String[numAnrs];
                for (int k = 0 ; k < numAnrs; k++) {
                    int anrLength = byteBuf.getShort();
                    if(anrLength > 0) {
                        byte[] anr = new byte[anrLength];
                        byteBuf.get(anr);
                        try {
                            AdnRecordsInfoGroup[i].mAdNumbers[k] =
                                QtiSimPhoneBookAdnRecord.ConvertToPhoneNumber(new String(anr, "UTF-8"));
                        } catch (UnsupportedEncodingException e) {
                            loge("Unsupport UTF-8 to parse anr");
                            break;
                        }
                    }
                }
            }
        }
        logd(Arrays.toString(AdnRecordsInfoGroup));

        return AdnRecordsInfoGroup;
    }

    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            logd("Received " + intent.getAction());
            if (ACTION_ADN_INIT_DONE.equals(intent.getAction())) {
                AsyncResult ar = new AsyncResult(null, null, null);
                mAdnInitDoneRegistrantList.notifyRegistrants(ar);

            } else if (ACTION_ADN_RECORDS_IND.equals(intent.getAction())) {
                byte[] data = intent.getByteArrayExtra("adn_records");

                AsyncResult ar = new AsyncResult(null, responseAdnRecords(data), null);
                mAdnRecordsInfoRegistrantList.notifyRegistrants(ar);
            }
        }
    };

    private void logd(String string) {
        Rlog.d(LOG_TAG, string);
    }

    private void logi(String string) {
        Rlog.i(LOG_TAG, string);
    }

    private void loge(String string) {
        Rlog.e(LOG_TAG, string);
    }
}
