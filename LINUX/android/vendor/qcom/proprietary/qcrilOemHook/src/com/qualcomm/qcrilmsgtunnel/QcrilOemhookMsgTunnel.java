/*
 * Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 */

/*
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

/*******************************************************************************
@file    QcrilOemhookMsgTunnel.java
@brief   HIDL Implementation between QCRIL and OEM specific reqs/responses
 ******************************************************************************/

package com.qualcomm.qcrilmsgtunnel;
import static com.android.internal.telephony.RILConstants.GENERIC_FAILURE;
import static com.android.internal.telephony.RILConstants.RADIO_NOT_AVAILABLE;
import static com.android.internal.telephony.RILConstants.RIL_REQUEST_OEM_HOOK_RAW;
import static com.android.internal.telephony.RILConstants.RIL_UNSOL_OEM_HOOK_RAW;
import static com.android.internal.telephony.RILConstants.RIL_ERRNO_INVALID_RESPONSE;
import static com.android.internal.telephony.RILConstants.SUCCESS;
import static android.Manifest.permission.READ_PHONE_STATE;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;

import android.app.ActivityManagerNative;
import android.content.Context;
import android.content.Intent;
import android.hardware.radio.V1_0.RadioError;
import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;
import android.net.LocalSocketAddress;
import android.telephony.SubscriptionManager;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HwBinder;
import android.os.Message;
import android.os.PowerManager;
import android.os.UserHandle;
import android.os.PowerManager.WakeLock;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Log;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;

import vendor.qti.hardware.radio.qcrilhook.V1_0.IQtiOemHook;
import vendor.qti.hardware.radio.qcrilhook.V1_0.IQtiOemHookResponse;
import vendor.qti.hardware.radio.qcrilhook.V1_0.IQtiOemHookIndication;

public class QcrilOemhookMsgTunnel {

    private static String TAG = "QcrilOemhookMsgTunnel";
    private static final boolean DBG = Log.isLoggable(TAG, Log.DEBUG);
    private static final String mOemIdentifier = "QOEMHOOK";
    public static final String sub_id = "sub_id";
    public static final String pdc_active = "active";
    public static final String pdc_error = "error";
    public static final String audio_state_changed_data = "audio_state_changed_data";
    static final int INT_SIZE = 4;
    private boolean mIsServiceConnected = false;
    private final String rat_info = "rat";
    private final String rac_change = "rac";
    final Object mLock = new Object();
    private final String adn_records = "adn_records";

    private final OemhookMessageHandler mHandler;
    WakeLock mWakeLock;
    Context mContext;

    int mWakeLockTimeout;
    // The number of requests sent out but waiting for response. It increases while
    // sending request and decreases while handling response. It should match
    // mRequestList.size() unless there are requests no replied while
    // WAKE_LOCK_TIMEOUT occurs.
    int mRequestMessagesWaiting;

    private Integer mInstanceId;

    // I'd rather this be LinkedList or something
    ArrayList<QcRilRequest> mRequestsList = new ArrayList<QcRilRequest>();

    private IQtiOemHook mIQtiOemHook;
    private IQtiOemHookResponse mIQtiOemHookResp;
    private IQtiOemHookIndication mIQtiOemHookInd;
    private QtiOemHookDeathRecipient mDeathRecipient;
    private ServiceNotificationCallback mServiceNotification;

    // ***** Events
    static final int EVENT_SEND = 1;
    static final int EVENT_WAKE_LOCK_TIMEOUT = 2;

    static final int RIL_MAX_COMMAND_BYTES = (8 * 1024);
    /**
     * Wake lock timeout should be longer than the longest timeout in the vendor ril.
     */
    private static final int DEFAULT_WAKE_LOCK_TIMEOUT = 60000;

    protected Registrant mUnsolOemHookRawRegistrant;
    protected Registrant mUnsolOemHookExtAppRegistrant;

    static String SERVICE_NAME[] = {"oemhook0", "oemhook1", "oemhook2"};

    static final String ACTION_SAFE_WIFI_CHANNELS_CHANGED =
            "qualcomm.intent.action.SAFE_WIFI_CHANNELS_CHANGED";
    static final String ACTION_INCREMENTAL_NW_SCAN_IND =
            "qualcomm.intent.action.ACTION_INCREMENTAL_NW_SCAN_IND";
    static final String ACTION_EM_DATA_RECEIVED =
            "qualcomm.intent.action.ACTION_EM_DATA_RECEIVED";
    static final String ACTION_PDC_DATA_RECEIVED =
            "qualcomm.intent.action.ACTION_PDC_DATA_RECEIVED";
    static final String ACTION_PDC_CONFIGS_CLEARED =
            "qualcomm.intent.action.ACTION_PDC_CONFIGS_CLEARED";
    static final String ACTION_PDC_CONFIG_LIST_RECEIVED =
            "qualcomm.intent.action.ACTION_PDC_CONFIG_LIST_RECEIVED";
    static final String ACTION_AUDIO_STATE_CHANGED =
            "qualcomm.intent.action.ACTION_AUDIO_STATE_CHANGED";
    static final String ACTION_PDC_CONFIGS_VALIDATION =
            "qualcomm.intent.action.ACTION_PDC_CONFIGS_VALIDATION";
    static final String ACTIN_PDC_VALIDATE_DUMPED =
            "qualcomm.intent.action.ACTIN_PDC_VALIDATE_DUMPED";
    static final String ACTION_CSG_ID_CHANGED_IND =
            "qualcomm.intent.action.ACTION_CSG_ID_CHANGED_IND";
    static final String ACTION_RAC_CHANGED =
            "qualcomm.intent.action.ACTION_RAC_CHANGED";
    static final String ACTION_SLOT_STATUS_CHANGED_IND =
            "qualcomm.intent.action.ACTION_SLOT_STATUS_CHANGED_IND";
    static final String ACTION_ADN_INIT_DONE =
            "qualcomm.intent.action.ACTION_ADN_INIT_DONE";
    static final String ACTION_ADN_RECORDS_IND =
            "qualcomm.intent.action.ACTION_ADN_RECORDS_IND";

    public QcrilOemhookMsgTunnel(Integer instanceId)
    {
        super();
        logi("Starting QcrilOemhookMsgTunnel");
        mContext = QcrilMsgTunnelService.mContext;
        PowerManager pm = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
        mWakeLock.setReferenceCounted(false);
        mWakeLockTimeout = SystemProperties.getInt(TelephonyProperties.PROPERTY_WAKE_LOCK_TIMEOUT,
                DEFAULT_WAKE_LOCK_TIMEOUT);
        mRequestMessagesWaiting = 0;
        mInstanceId = instanceId;
        mHandler = new OemhookMessageHandler();
        mDeathRecipient = new QtiOemHookDeathRecipient();
        initOemHook();
        registerForServiceNotifications();
    }

    /* Register with service manager to get notified
     * when the oemhook service is up
     */
    private void registerForServiceNotifications() {
        try {
            mServiceNotification = new ServiceNotificationCallback();
            boolean ret = IServiceManager.getService().registerForNotifications
                ("vendor.qti.hardware.radio.qcrilhook@1.0::IQtiOemHook",
                SERVICE_NAME[mInstanceId], mServiceNotification);
            logi("registerForServiceNotifications: ret=" + ret);
        } catch(Exception ex) {
            loge("registerForServiceNotifications: exception" + ex);
        }
    }

    /* Callback for oem hook response */
    class QtiOemHookResponse extends IQtiOemHookResponse.Stub {
        public void oemHookRawResponse(int serial, int error, ArrayList<Byte> data) {
            processOemHookResponse(serial, error, data);
            releaseWakeLockIfDone();
        }
    }

    /* Callback for oem hook indication */
    class QtiOemHookIndication extends IQtiOemHookIndication.Stub {
        public void oemHookRawIndication(ArrayList<Byte> data) {
            acquireWakeLock();
            processOemHookIndication(data);
            releaseWakeLockIfDone();
        }
    }

    /* Utility method to get part of byte array */
    private byte[] getArrayPart(byte[] buf, int index) {
        int bufLength = buf.length-index;
        if (bufLength <=0) {
            loge("Invalid payload length");
            return null;
        }
        byte[] res = new byte[bufLength];
        for(int i = 0; i < bufLength; i++) {
            res[i] = buf[index];
            index++;
        }
        return res;
    }

    /* Intitialize and bind to oemhook HIDL service */
    private void initOemHook() {
        try {
            synchronized(mLock) {
                // Get the service for a particular instanceId since each rild
                // will have its own oemhook HIDL service
                mIQtiOemHook = IQtiOemHook.getService(SERVICE_NAME[mInstanceId]);
                if (mIQtiOemHook == null) {
                    loge("IQtiOemHook is null");
                    return;
                }
                mIsServiceConnected = true;
                mIQtiOemHook.linkToDeath(mDeathRecipient, 0);
                mIQtiOemHookResp = new QtiOemHookResponse();
                mIQtiOemHookInd = new QtiOemHookIndication();
                mIQtiOemHook.setCallback(mIQtiOemHookResp, mIQtiOemHookInd);
                logi("initOemHook:setCallback " + mIQtiOemHookResp + " "
                    + mIQtiOemHookInd);
            }
        } catch (RemoteException | RuntimeException ex) {
            loge("initOemHook: exception" + ex);
        }
    }

    /*
     * Death recipient callback that is notified when oemhook HIDL
     * service dies
     */
    final class QtiOemHookDeathRecipient implements HwBinder.DeathRecipient {
        /**
         * Callback that gets called when the service has died
         */
        @Override
        public void serviceDied(long cookie) {
            loge("Oemhook HIDL service died");
            resetServiceAndRequestList();
        }
    }

    /* Callback that registers with ServiceManager to be notified of
     * service availability
     */
    private final class ServiceNotificationCallback extends IServiceNotification.Stub {
        @Override
        public void onRegistration(String fqName, String name, boolean preexisting) {
            logd("IServiceNotification.onRegistration " + fqName
                        + " " + name + " preexisting=" + preexisting);
            if (!mIsServiceConnected) initOemHook();
        }
    }

    private void resetServiceAndRequestList() {
        synchronized(mLock) {
            mIsServiceConnected = false;
            mIQtiOemHook = null;
            mIQtiOemHookResp = null;
            mIQtiOemHookInd = null;
        }
        clearRequestsList(RADIO_NOT_AVAILABLE);
    }

    class OemhookMessageHandler extends Handler {
        // ***** Handler implementation
        @Override
        public void handleMessage(Message msg) {
            QcRilRequest rr = (QcRilRequest) (msg.obj);
            QcRilRequest req = null;
            switch (msg.what) {
                case EVENT_WAKE_LOCK_TIMEOUT:
                    // Haven't heard back from the last request. Assume we're
                    // not getting a response and release the wake lock.
                    synchronized (mWakeLock) {
                        if (mWakeLock.isHeld()) {
                            // The timer of WAKE_LOCK_TIMEOUT is reset with each new send request.
                            // So when WAKE_LOCK_TIMEOUT occurs all requests in mRequestList
                            // already waited at least DEFAULT_WAKE_LOCK_TIMEOUT but no response.
                            // Reset mRequestMessagesWaiting to enable releaseWakeLockIfDone().
                            //
                            // Note: Keep mRequestList so that delayed response
                            // can still be handled when response finally comes.
                            if (mRequestMessagesWaiting != 0) {
                                logd("NOTE: mReqWaiting is NOT 0 but" +
                                        mRequestMessagesWaiting +
                                        " at TIMEOUT, reset!" +
                                        " There still msg waitng for response");

                                mRequestMessagesWaiting = 0;

                                synchronized (mRequestsList) {
                                    int count = mRequestsList.size();
                                    logd("WAKE_LOCK_TIMEOUT " +
                                            " mRequestList=" + count);

                                    for (int i = 0; i < count; i++) {
                                        rr = mRequestsList.get(i);
                                        logd(i + ": [" + rr.mSerial + "] " +
                                                requestToString(rr.mRequest));
                                    }
                                }
                            }
                            mWakeLock.release();
                        }
                    }
                    break;
            }
        }
    }

    private static void logd(String msg) {
        if (DBG) {
            Log.d(TAG, msg);
        }
    }

    private void logi(String msg) {
        Log.i(TAG, "[" + mInstanceId + "]" + msg);
    }

    private void loge(String msg) {
        Log.e(TAG, "[" + mInstanceId + "]" + msg);
    }

    static String requestToString(int request) {
        switch (request) {
            case RIL_REQUEST_OEM_HOOK_RAW:
                return "OEM_HOOK_RAW";
            case RIL_UNSOL_OEM_HOOK_RAW:
                return "UNSOL_OEM_HOOK_RAW";
            default:
                return "<unknown request>";
        }
    }

    private static String bytesToHexString(byte[] bytes, int length) {
        if (bytes == null || length == 0) return null;

        StringBuilder ret = new StringBuilder(2*length);

        for (int i = 0 ; i < length ; i++) {
            int b;

            b = 0x0f & (bytes[i] >> 4);
            ret.append("0123456789abcdef".charAt(b));

            b = 0x0f & bytes[i];
            ret.append("0123456789abcdef".charAt(b));
        }

        return ret.toString();
    }

    private void acquireWakeLock() {
        synchronized (mWakeLock) {
            mWakeLock.acquire();
            mHandler.removeMessages(EVENT_WAKE_LOCK_TIMEOUT);
            Message msg = mHandler.obtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
            mHandler.sendMessageDelayed(msg, mWakeLockTimeout);
        }
    }

    private void releaseWakeLockIfDone() {
        synchronized (mWakeLock) {
            if (mWakeLock.isHeld() &&
                    (mRequestMessagesWaiting == 0)) {
                mHandler.removeMessages(EVENT_WAKE_LOCK_TIMEOUT);
                mWakeLock.release();
            }
        }
    }

    private QcRilRequest findAndRemoveRequestFromList(int serial) {
        synchronized (mRequestsList) {
            for (int i = 0, s = mRequestsList.size(); i < s; i++) {
                QcRilRequest rr = mRequestsList.get(i);

                if (rr.mSerial == serial) {
                    mRequestsList.remove(i);
                    if (mRequestMessagesWaiting > 0)
                        mRequestMessagesWaiting--;
                    return rr;
                }
            }
        }
        return null;
    }

    /**
     * Release each request in mRequestsList then clear the list
     *
     * @param error is the RIL_Errno sent back
     */
    private void clearRequestsList(int error) {
        QcRilRequest rr;
        synchronized (mRequestsList) {
            int count = mRequestsList.size();
            logd("mRequestList count=" + count);
            for (int i = 0; i < count; i++) {
                rr = mRequestsList.get(i);
                logd(i + ": [" + rr.mSerial + "] " +
                        requestToString(rr.mRequest));
                sendResponse(rr, error, null);
            }
            mRequestsList.clear();
            mRequestMessagesWaiting = 0;
        }
    }

    /* Utility method to convert byte array to ArrayList of Bytes */
    private static ArrayList<Byte> primitiveArrayToArrayList(byte[] arr) {
        if (arr == null) return null;
        ArrayList<Byte> arrayList = new ArrayList<>(arr.length);
        for (byte b : arr) {
            arrayList.add(b);
        }
        return arrayList;
    }

    /* Utility method to convert ArrayList of Bytes to byte array */
    private static byte[] arrayListToPrimitiveArray(ArrayList<Byte> bytes) {
        if (bytes == null) return null;
        byte[] ret = new byte[bytes.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = bytes.get(i);
        }
        return ret;
    }

    /* Utility method to send response for an oemhook request */
    private void sendResponse(QcRilRequest req, int error, Object ret) {
        if (req != null) {
            if (error != SUCCESS) {
                req.onError(error, ret);
                req.release();
            } else if (req.mResult != null) {
                logd(req.mResult.toString());
                AsyncResult.forMessage(req.mResult, ret, null);
                req.mResult.sendToTarget();
            }
        }
    }

    /* Utility method to add a new oemhook request to the
     * queue of requests waiting to be acknowledged from HIDL
     * service
     */
    private void queueRequest(QcRilRequest rr) {
        synchronized (mRequestsList) {
            mRequestsList.add(rr);
            mRequestMessagesWaiting++;
        }
    }

    /* Sends oemhook raw request to HIDL service
     *
     * @param data array of bytes
     * @param response response object to client
     * @return none
     */
    public void invokeOemRilRequestRaw(byte[] data, Message response) {
        acquireWakeLock();
        QcRilRequest rr
        = QcRilRequest.obtain(RIL_REQUEST_OEM_HOOK_RAW, response);

        logi("invokeOemRilRequestRaw: serial=" + rr.mSerial + " length=" + data.length);
        logd(rr.mSerial + " > " + requestToString(rr.mRequest) + "[" +
                bytesToHexString(data, data.length) + "]");

        if (mIQtiOemHook == null) {
            loge("oemHookService is not running.");
            sendResponse(rr, RADIO_NOT_AVAILABLE, null);
        } else {
            queueRequest(rr);
            try {
                logd("Calling HIDL oemHookRawRequest");
                mIQtiOemHook.oemHookRawRequest(rr.mSerial, primitiveArrayToArrayList(data));
            } catch (RemoteException ex) {
                loge("oemHookRawRequest remote error serial=" + rr.mSerial + " " + ex);
                QcRilRequest req = findAndRemoveRequestFromList(rr.mSerial);
                sendResponse(req, GENERIC_FAILURE, null);
            }
        }
    }

    /* Processes oem hook response received from HIDL service */
    private void processOemHookResponse(int serial, int error, ArrayList<Byte> data) {
        logi("processOemHookResponse serial no: " + serial + " error: "
            + error + "length=" + data.size());
        QcRilRequest rr = findAndRemoveRequestFromList(serial);
        int respError = SUCCESS;
        byte[] ret = arrayListToPrimitiveArray(data);
        if (rr == null) {
            loge("Unexpected serial number in solicited response!serial: "
                    + serial + " error: " + error);
            return;
        }

        if (error != 0) { // Send the response with error to client
            respError = error;
        } else if (data == null) { // No response to send to client
            logd(rr.serialString() + " < " + requestToString(rr.mRequest) +
                    " [null]");
        } else {
            // Send response array through client's callback
            logd(rr.serialString() + " < " + requestToString(rr.mRequest) +
                    " [" + ret.toString() + "]");
        }
        sendResponse(rr, respError, ret);
    }

    /* Processes oemhook indication received from HIDL service */
    private void processOemHookIndication(ArrayList<Byte> data) {
        logi("processOemHookIndication length=" + data.size());
        byte[] ret = arrayListToPrimitiveArray(data);
        ByteBuffer oemHookUnsolBuf = ByteBuffer.wrap(ret);
        oemHookUnsolBuf.order(ByteOrder.nativeOrder());
        int oemHookUnsolId = oemHookUnsolBuf.getInt();
        logd("processOemHookIndication unsolId=" + oemHookUnsolId);
        switch (oemHookUnsolId) {
            case RIL_UNSOL_OEM_HOOK_RAW:
                logd("Received RIL_UNSOL_OEM_HOOK_RAW message"
                      + "length=" + data.size() + " " + Arrays.toString(ret));

                if (isQcUnsolOemHookResp(oemHookUnsolBuf)) {
                    logd("OEM ID check Passed");
                    processUnsolOemhookResponse(oemHookUnsolBuf);
                } else if (mUnsolOemHookRawRegistrant != null) {
                    logd("External OEM message, to be notified");
                    mUnsolOemHookRawRegistrant.notifyRegistrant(
                        new AsyncResult(null, getArrayPart(ret, INT_SIZE), null));
                }
                break;
            default:
                loge("Invalid indication: " + oemHookUnsolId + ".Bail out");
        }
    }

    /* Checks whether a given indication is a QC inndication */
    private boolean isQcUnsolOemHookResp(ByteBuffer oemHookResponse) {
        int mHeaderSize = mOemIdentifier.length() + 2 * INT_SIZE;

        /* Check OEM ID in UnsolOemHook response */
        if (oemHookResponse.capacity() < mHeaderSize) {
            /*
             * size of UnsolOemHook message is less than expected, considered as
             * External OEM's message
             */
            logd("RIL_UNSOL_OEM_HOOK_RAW data size is " +
                    oemHookResponse.capacity() +
                    " assume external OEM message, not QOEMHOOK");

            return false;
        } else {
            byte[] oem_id_bytes = new byte[mOemIdentifier.length()];
            oemHookResponse.get(oem_id_bytes);
            String oem_id_str = new String(oem_id_bytes);
            logd("Oem ID in RIL_UNSOL_OEM_HOOK_RAW is " + oem_id_str);
            if (!oem_id_str.equals(mOemIdentifier)) {
                /* OEM ID not matched, considered as External OEM's message */
                logd("external OEM message, not QOEMHOOK");
                return false;
            }
        }
        return true;
    }

    private void processUnsolOemhookResponse(ByteBuffer oemHookResponse) {
        /** Starting number for QCRILHOOK request and response IDs */
        final int QCRILHOOK_BASE = 0x80000;

        /** qcrilhook unsolicited response IDs */
        final int QCRILHOOK_UNSOL_WIFI_SAFE_CHANNELS_CHANGED = QCRILHOOK_BASE + 1008;
        final int QCRILHOOK_UNSOL_INCREMENTAL_NW_SCAN_IND = QCRILHOOK_BASE + 1011;
        final int QCRILHOOK_UNSOL_ENGINEER_MODE = QCRILHOOK_BASE + 1012;
        final int QCRILHOOK_UNSOL_PDC_CONFIG = QCRILHOOK_BASE + 1014;
        final int QCRILHOOK_UNSOL_AUDIO_STATE_CHANGED = QCRILHOOK_BASE + 1015;
        final int QCRILHOOK_UNSOL_PDC_CLEAR_CONFIGS = QCRILHOOK_BASE + 1017;
        final int QCRILHOOK_UNSOL_PDC_VALIDATE_CONFIGS = QCRILHOOK_BASE + 1023;
        final int QCRILHOOK_UNSOL_PDC_VALIDATE_DUMPED = QCRILHOOK_BASE + 1024;
        final int QCRILHOOK_UNSOL_PDC_LIST_CONFIG = QCRILHOOK_BASE + 1032;
        final int QCRILHOOK_UNSOL_SLOT_STATUS_CHANGE_IND = QCRILHOOK_BASE + 1033;
        final int QCRILHOOK_UNSOL_CSG_ID_CHANGE_IND = QCRILHOOK_BASE + 1052;
        final int QCRIL_EVT_HOOK_UNSOL_RAT_RAC_CHANGE_IND = QCRILHOOK_BASE+1053;
        final int QCRILHOOK_UNSOL_ADN_INIT_DONE = QCRILHOOK_BASE + 1034;
        final int QCRILHOOK_UNSOL_ADN_RECORDS_IND = QCRILHOOK_BASE + 1035;

        int response_id = 0, response_size = 0;

        response_id = oemHookResponse.getInt();
        logd("Response ID in RIL_UNSOL_OEM_HOOK_RAW is " + response_id);

        response_size = oemHookResponse.getInt();
        if (response_size < 0 || response_size > RIL_MAX_COMMAND_BYTES) {
            loge("Response Size is Invalid " + response_size);
            return;
        }
        byte[] response_data = new byte[response_size];
        oemHookResponse.get(response_data, 0, response_size);

        switch (response_id) {
            case QCRILHOOK_UNSOL_WIFI_SAFE_CHANNELS_CHANGED:
                broadcastWifiChannelsChangedIntent(response_data);
                break;
            case QCRILHOOK_UNSOL_INCREMENTAL_NW_SCAN_IND:
                broadcastIncrNwScanInd(response_data);
                break;
            case QCRILHOOK_UNSOL_ENGINEER_MODE:
                broadcastEngineerMode(response_data);
                break;
            case QCRILHOOK_UNSOL_PDC_CONFIG:
                broadcastDeviceConfig(response_data);
                break;
            case QCRILHOOK_UNSOL_PDC_CLEAR_CONFIGS:
                broadcastClearConfigs(response_data);
                break;
            case QCRILHOOK_UNSOL_AUDIO_STATE_CHANGED:
                broadcastAudioStateChanged(response_data);
                break;
            case QCRILHOOK_UNSOL_PDC_VALIDATE_CONFIGS:
                broadcastValidateConfigs(response_data);
                break;
            case QCRILHOOK_UNSOL_PDC_VALIDATE_DUMPED:
                broadcastValidateDumped(response_data);
                break;
            case QCRILHOOK_UNSOL_PDC_LIST_CONFIG:
                broadcastPdcConfigsList(response_data);
                break;
            case QCRILHOOK_UNSOL_CSG_ID_CHANGE_IND:
                broadcastCsgChangedInd(response_data);
                break;
            case QCRIL_EVT_HOOK_UNSOL_RAT_RAC_CHANGE_IND:
                broadcastRacChange(response_data);
                break;
            case QCRILHOOK_UNSOL_SLOT_STATUS_CHANGE_IND:
                broadcastSlotStatusChange(response_data);
            case QCRILHOOK_UNSOL_ADN_INIT_DONE:
                broadcastAdnInitDone(response_data);
                break;
            case QCRILHOOK_UNSOL_ADN_RECORDS_IND:
                broadcastAdnRecordsInd(response_data);
                break;
            default:
                logd("Response ID " + response_id +
                            " is not served in this process.");
                logd("To broadcast an Intent via the notifier to" +
                            " external apps");

                if (mUnsolOemHookExtAppRegistrant != null) {
                    //TODO: Do we really need rewind here?
                    oemHookResponse.rewind();
                    byte[] origData = getArrayPart(oemHookResponse.array(), INT_SIZE);
                    logd("processUnsol: response length=" + origData.length);
                    // Response array reaches client through QcRilHook via the intent
                    // com.qualcomm.intent.action.ACTION_UNSOL_RESPONSE_OEM_HOOK_RAW
                    mUnsolOemHookExtAppRegistrant.notifyRegistrant(new AsyncResult(null, origData,
                            null));
                }
                break;
        }
    }

    private void broadcastWifiChannelsChangedIntent(byte[] data) {
        Intent intent = new Intent(ACTION_SAFE_WIFI_CHANNELS_CHANGED);
        logd("WifiSafeChannels " + Arrays.toString(data));

        String s;
        try {
            s = new String(data, "US-ASCII");
        } catch (UnsupportedEncodingException e) {
            loge("Decoding failed: " + e);
            return;
        }

        logd("Decoded string " + s);

        // Channels info is divided by commas
        String[] channels = s.split(",");

        logd("Parsed channels " + Arrays.toString(channels));
        logd("Broadcasting intent ACTION_SAFE_WIFI_CHANNELS_CHANGED ");


        intent.putExtra("current_channel", Integer.parseInt(channels[0]));
        intent.putExtra("start_safe_channel", Integer.parseInt(channels[1]));
        intent.putExtra("end_safe_channel", Integer.parseInt(channels[2]));
        ActivityManagerNative.broadcastStickyIntent(intent, READ_PHONE_STATE,
                UserHandle.USER_ALL);
    }

    private void broadcastIncrNwScanInd(byte[] data) {
        logd("Incremental nw scan data " + Arrays.toString(data));
        ByteBuffer payload = ByteBuffer.wrap(data);
        payload.order(ByteOrder.nativeOrder());

        int scanResult = payload.get();
        // Each set of data has 4 strings, qcril sends number of sets
        // so multiply by 4 to get number of strings
        int numOfStrings = payload.get() * 4;
        logd("scanResult =" + scanResult + "numOfStrings = " + numOfStrings);
        if (numOfStrings < 0) {
            logd("Invalid number of strings" + numOfStrings);
            return;
        }
        String nwScanInfo[] = new String[numOfStrings];
        for (int i = 0; i < numOfStrings; i++) {
            short stringLen = payload.getShort();
            logd("stringLen =" + stringLen);
            byte bytes[] = new byte[stringLen];
            payload.get(bytes);
            nwScanInfo[i] = new String(bytes);
            logd("i = " + i + "String is " + nwScanInfo[i]);
        }
        Intent intent = new Intent(ACTION_INCREMENTAL_NW_SCAN_IND);
        intent.putExtra("scan_result", scanResult);
        intent.putExtra("incr_nw_scan_data", nwScanInfo);
        intent.putExtra("sub_id", mInstanceId);
        mContext.sendBroadcast(intent);
    }

    private void broadcastCsgChangedInd(byte[] data) {
        int csgId = 0;
        logd("CSG ID Changed data " + Arrays.toString(data));
        ByteBuffer payload = ByteBuffer.wrap(data);
        payload.order(ByteOrder.nativeOrder());
        csgId = payload.get();
        logd("csgId =" + csgId);
        Intent intent = new Intent(ACTION_CSG_ID_CHANGED_IND);
        intent.putExtra("csgId", csgId);
        mContext.sendBroadcast(intent);
    }

    private void broadcastValidateDumped(byte[] data) {
        Intent intent = new Intent(ACTIN_PDC_VALIDATE_DUMPED);
        logd("PDC Validate Dumped " + Arrays.toString(data));
        logd("Broadcasting intent ACTION_PDC_VALIDATE_DUMPED");

        intent.putExtra("dump_file", data);
        mContext.sendBroadcast(intent);
    }

    private void broadcastValidateConfigs(byte[] data) {
        logd("PDC Validate Configs " + Arrays.toString(data));
        ByteBuffer payload = ByteBuffer.wrap(data);
        payload.order(ByteOrder.nativeOrder());

        // get the result
        int result = payload.getInt();
        // get the index
        int index = payload.getInt();
        // get the nv item len
        int nvItemLen = payload.getInt();
        // get the ref valalue len
        int nvRefValLen = payload.getInt();
        // get the device value len
        int nvCurValLen= payload.getInt();

        Intent intent = new Intent(ACTION_PDC_CONFIGS_VALIDATION);
        intent.putExtra("result", result);
        intent.putExtra("index", index);

        logd("result:" + result +  " index:" + index);


        if ((nvItemLen != 0) && (result == 0)) {
            // get NV item Info
            byte bytes[] = new byte[nvItemLen];
            payload.get(bytes);
            String nvItemInfo = new String(bytes);
            logd("nvItemInfo:" + nvItemInfo);
            intent.putExtra("nv_item", nvItemInfo);

            // get Ref value
            if (nvRefValLen == 0) {
                intent.putExtra("nv_item", "");
            } else {
                bytes = new byte[nvRefValLen];
                payload.get(bytes);
                String nvRefVal = new String(bytes);
                logd("nvRefVal:" + nvRefVal);
                intent.putExtra("ref_value", nvRefVal);
            }

            // get Device Value
            if (nvCurValLen == 0) {
                 intent.putExtra("cur_value", "");
            } else {
                bytes = new byte[nvCurValLen];
                payload.get(bytes);
                String nvCurVal = new String(bytes);
                logd("nvCurVal:" + nvCurVal);
                intent.putExtra("cur_value", nvCurVal);
            }
        }
        // broadcast result
        mContext.sendBroadcast(intent);
    }

    private void broadcastPdcConfigsList(byte[] data) {
        Intent intent = new Intent(ACTION_PDC_CONFIG_LIST_RECEIVED);
        intent.putExtra("list_data", data);

        logd("Broadcasting intent ACTION_CONFIG_LIST_RECEIVED");
        mContext.sendBroadcast(intent);
    }

    private void broadcastEngineerMode(byte[] data) {
        Intent intent = new Intent(ACTION_EM_DATA_RECEIVED);
        logd("EM data: " + Arrays.toString(data));
        intent.putExtra("sub_id", mInstanceId);
        intent.putExtra("em_data", data);
        logd("Broadcasting intent ACTION_EM_DATA_RECEIVED");
        mContext.sendBroadcast(intent);
    }

    private void broadcastDeviceConfig(byte[] data) {
        Intent intent = new Intent(ACTION_PDC_DATA_RECEIVED);
        logd("DeviceConfig (PDC) data: " + Arrays.toString(data));
        intent.putExtra(sub_id, mInstanceId);
        intent.putExtra(pdc_active, data);
        intent.putExtra(pdc_error, 0);
        logd("Broadcasting intent ACTION_PDC_DATA_RECEIVED");
        mContext.sendBroadcast(intent);
    }

    private void broadcastClearConfigs(byte[] data) {
        Intent intent = new Intent(ACTION_PDC_CONFIGS_CLEARED);
        logd("ClearConfig (PDC) data: " + Arrays.toString(data));
        intent.putExtra(sub_id, mInstanceId);
        intent.putExtra(pdc_active, data);
        intent.putExtra(pdc_error, 0);
        logd("Broadcasting intent ACTION_PDC_CONFIGS_CLEARED");
        mContext.sendBroadcast(intent);
    }

    private void broadcastAudioStateChanged(byte[] data) {
        Intent intent = new Intent(ACTION_AUDIO_STATE_CHANGED);
        logd("AudioState data received: " + new String(data));
        intent.putExtra(audio_state_changed_data, data);
        logd("Broadcasting intent ACTION_AUDIO_STATE_CHANGED");
        mContext.sendBroadcast(intent);
    }

    private void broadcastRacChange(byte[] data) {
        ByteBuffer payload = ByteBuffer.wrap(data);
        payload.order(ByteOrder.nativeOrder());
        int ratInfo = payload.get();
        int racChange = payload.get();

        Intent intent = new Intent(ACTION_RAC_CHANGED);
        logd("Rac Change (PDC) data: " + Arrays.toString(data));
        intent.putExtra(sub_id, mInstanceId);
        intent.putExtra(rat_info, ratInfo);
        intent.putExtra(rac_change, racChange);
        logd("Broadcasting intent ACTION_RAC_CHANGED");
        mContext.sendBroadcast(intent);
    }

    /* Duplicate slot status change indications will go in case of
       multiple RILs, application need to discard duplicate status */
    private void broadcastSlotStatusChange(byte[] data) {
        Intent intent = new Intent(ACTION_SLOT_STATUS_CHANGED_IND);
        intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        intent.putExtra("slot_status", data);
        logd("Broadcasting intent ACTION_SLOT_STATUS_CHANGED_IND");
        mContext.sendBroadcast(intent);
    }

    private void broadcastAdnInitDone(byte[] data) {
        Intent intent = new Intent(ACTION_ADN_INIT_DONE);
        intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND |
                        Intent.FLAG_RECEIVER_REPLACE_PENDING);
        SubscriptionManager.putPhoneIdAndSubIdExtra(intent, mInstanceId);
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
        logd("Broadcasting intent ACTION_ADN_INIT_DONE");
    }

    private void broadcastAdnRecordsInd(byte[] data) {
        Intent intent = new Intent(ACTION_ADN_RECORDS_IND);
        intent.putExtra(adn_records, data);
        logd("Broadcasting intent ACTION_ADN_RECORDS_IND");
        mContext.sendBroadcast(intent);
    }

    public void setOnUnsolOemHookRaw(Handler h, int what, Object obj) {
        mUnsolOemHookRawRegistrant = new Registrant(h, what, obj);
    }

    public void unSetOnUnsolOemHookRaw(Handler h) {
        mUnsolOemHookRawRegistrant.clear();
    }

    public void setOnUnsolOemHookExtApp(Handler h, int what, Object obj) {
        mUnsolOemHookExtAppRegistrant = new Registrant(h, what, obj);
    }

    public void unSetOnUnsolOemHookExtApp(Handler h) {
        mUnsolOemHookExtAppRegistrant.clear();
    }
}

class QcRilRequest {
    static final String TAG = "QcRilRequest";

    // ***** Class Variables
    static int sNextSerial = 1000;
    static Object sSerialMonitor = new Object();
    private static Object sPoolSync = new Object();
    private static QcRilRequest sPool = null;
    private static int sPoolSize = 0;
    private static final int MAX_POOL_SIZE = 4;

    // ***** Instance Variables
    int mSerial;
    int mRequest;
    Message mResult;
    QcRilRequest mNext;

    /**
     * Retrieves a new QcRilRequest instance from the pool.
     *
     * @param request RIL_REQUEST_*
     * @param result sent when operation completes
     * @return a RILRequest instance from the pool.
     */
    static QcRilRequest obtain(int request, Message result) {
        QcRilRequest rr = null;

        synchronized (sPoolSync) {
            if (sPool != null) {
                rr = sPool;
                sPool = rr.mNext;
                rr.mNext = null;
                sPoolSize--;
            }
        }

        if (rr == null) {
            rr = new QcRilRequest();
        }

        synchronized (sSerialMonitor) {
            rr.mSerial = sNextSerial++;
        }
        rr.mRequest = request;
        rr.mResult = result;

        if (result != null && result.getTarget() == null) {
            throw new NullPointerException("Message target must not be null");
        }
        return rr;
    }

    /**
     * Returns a QcRilRequest instance to the pool. Note: This should only be
     * called once per use.
     */
    void release() {
        synchronized (sPoolSync) {
            if (sPoolSize < MAX_POOL_SIZE) {
                this.mNext = sPool;
                sPool = this;
                sPoolSize++;
                mResult = null;
            }
        }
    }

    private QcRilRequest() {
    }

    static void resetSerial() {
        synchronized (sSerialMonitor) {
            sNextSerial = 1000;
        }
    }

    String serialString() {
        // Cheesy way to do %04d
        StringBuilder sb = new StringBuilder(8);
        String sn;

        sn = Integer.toString(mSerial);

        // sb.append("J[");
        sb.append('[');
        for (int i = 0, s = sn.length(); i < 4 - s; i++) {
            sb.append('0');
        }

        sb.append(sn);
        sb.append(']');
        return sb.toString();
    }

    void onError(int error, Object ret) {
        CommandException ex;

        ex = CommandException.fromRilErrno(error);

        Log.e(TAG, serialString() + " < "
                + QcrilOemhookMsgTunnel.requestToString(mRequest)
                + " error: " + ex);

        if (mResult != null) {
            AsyncResult.forMessage(mResult, ret, ex);
            mResult.sendToTarget();
        }
    }
}
