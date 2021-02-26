/*
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
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
 *
 */

package org.codeaurora.ims;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;
import android.location.Address;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.HwBinder;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.RemoteException;
import android.os.PowerManager.WakeLock;
import android.os.SystemProperties;
import android.telephony.PhoneNumberUtils;

import com.android.ims.ImsCallForwardInfo;
import com.android.ims.ImsException;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsSsData;
import com.android.ims.ImsSsInfo;
import com.android.internal.telephony.CallForwardInfo;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.TelephonyProperties;
import com.android.internal.telephony.UUSInfo;
import com.android.internal.telephony.cdma.CdmaSmsBroadcastConfigInfo;
import com.android.internal.telephony.gsm.SmsBroadcastConfigInfo;
import com.qualcomm.ims.utils.Log;
import vendor.qti.hardware.radio.ims.V1_0.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.HashMap;
import java.util.List;

import org.codeaurora.ims.QtiCallConstants;


/**
 * {@hide}
 */
class IFRequest {

    // ***** Class Variables
    static int sNextSerial = 0;
    static Object sSerialMonitor = new Object();
    private static Object sPoolSync = new Object();
    private static IFRequest sPool = null;
    private static int sPoolSize = 0;
    private static final int MAX_POOL_SIZE = 4;

    // ***** Instance Variables
    int mSerial;
    int mRequest;
    Message mResult;
    // FIXME delete parcel
    // Parcel mp;
    IFRequest mNext;
    byte[] mData;

    /**
     * Retrieves a new IFRequest instance from the pool.
     *
     * @param request ImsQmiIF.MsgId.REQUEST_*
     * @param result sent when operation completes
     * @return a IFRequest instance from the pool.
     */
    static IFRequest obtain(int request, Message result) {
        IFRequest rr = null;

        synchronized (sPoolSync) {
            if (sPool != null) {
                rr = sPool;
                sPool = rr.mNext;
                rr.mNext = null;
                sPoolSize--;
            }
        }

        if (rr == null) {
            rr = new IFRequest();
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
     * Returns a IFRequest instance to the pool. Note: This should only be
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

    private IFRequest() {
    }

    static void resetSerial() {
        synchronized (sSerialMonitor) {
            sNextSerial = 0;
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
        RuntimeException ex;
        String errorMsg;

        if (error == ImsQmiIF.E_SUCCESS) {
            ex = null;
        } else {
            errorMsg = ImsSenderRxr.errorIdToString(error);
            ex = new ImsRilException(error, errorMsg);
        }

        Log.i(this, serialString() + "< "
                + ImsSenderRxr.msgIdToString(mRequest)
                + " error: " + error);

        if (mResult != null) {
            AsyncResult.forMessage(mResult, ret, ex);
            mResult.sendToTarget();
        }
    }
}

/**
 * IMS implementation of the CommandsInterface. {@hide}
 */
public class ImsSenderRxr extends ImsPhoneBaseCommands {
    static final String LOG_TAG = "ImsSenderRxr";

    /**
     * Wake lock timeout should be longer than the longest timeout in the vendor
     */
    private static final int DEFAULT_WAKE_LOCK_TIMEOUT = 60000;
    private static final int PDU_LENGTH_OFFSET = 4;
    private static final int MSG_TAG_LENGTH = 1;
    private static final int STATUS_INTERROGATE = 2;

    // ***** Events
    static final int EVENT_WAKE_LOCK_TIMEOUT = 1;

    // ***** Instance Variables
    WakeLock mWakeLock;
    int mWakeLockTimeout;

    final ImsRadioHandler mImsRadioHandler;

    // The number of requests sent out but waiting for response. It increases
    // while
    // sending request and decreases while handling response. It should match
    // mRequestList.size() unless there are requests no replied while
    // WAKE_LOCK_TIMEOUT occurs.
    int mRequestMessagesWaiting;

    /* Variable caching the Phone ID */
    private Integer mPhoneId;

    /* Variable caching the presence of UNSOL call list indication */
    private boolean mIsUnsolCallListPresent = false;

    // I'd rather this be LinkedList or something
    ArrayList<IFRequest> mRequestsList = new ArrayList<IFRequest>();

    // When we are testing emergency calls
    AtomicBoolean mTestingEmergencyCall = new AtomicBoolean(false);

    // The death recepient object which gets notified when IImsRadio service dies.
    final ImsRadioDeathRecipient mDeathRecipient;

    // Notification object used to listen to the start of the IImsRadio
    private final ImsRadioServiceNotification mServiceNotification =
        new ImsRadioServiceNotification();

    // ***** Constants
    static final int ZERO_SECONDS = 0;

    // Names of the Ims Radio services appended with slot id (for multiple sims)
    static final String[] IIMS_RADIO_SERVICE_NAME = {"imsradio0", "imsradio1", "imsradio2"};

    private RegistrantList mHandoverStatusRegistrants;
    private RegistrantList mRefreshConfInfoRegistrations;
    private RegistrantList mSrvStatusRegistrations;
    private RegistrantList mTtyStatusRegistrants;
    private RegistrantList mRadioStateRegistrations;
    private RegistrantList mGeolocationRegistrants;
    private RegistrantList mVoWiFiCallQualityRegistrants;
    private RegistrantList mSsaccRegistrants;
    private RegistrantList mVopsRegistrants;
    private RegistrantList mParticipantStatusRegistrants;
    private RegistrantList mImsSubConfigChangeRegistrants;
    private RegistrantList mRegistrationBlockStatusRegistrants;
    protected RegistrantList mModifyCallRegistrants;
    protected RegistrantList mMwiRegistrants;
    private RegistrantList mRttMessageRegistrants;
    private RegistrantList mRttModifyRegistrants;
    protected Registrant mSsnRegistrant;
    protected Registrant mSsIndicationRegistrant;

    private List<ImsRadioServiceListener> mListeners = new CopyOnWriteArrayList<>();

    /* Object of the type ImsRadioResponse which is registered with the IImsRadio
     * service for all callbacks to be routed back */
    private IImsRadioResponse mImsRadioResponse;

    /* Object of the type ImsRadioIndication which is registered with the IImsRadio
     * service for all unsolicited messages to be sent*/
    private IImsRadioIndication mImsRadioIndication;

    /*  Instance of the IImsRadio interface */
    private IImsRadio mImsRadio;

    public interface ImsRadioServiceListener {
        void onServiceUp(int phoneId);
        void onServiceDown(int phoneId);
    }

    /**
     * Registers listener.
     * @param listener Listener to be registered
     * @throws IllegalArgumentException Will throw an error if listener is null.
     * @see ImsSubController#Listener
     */
    public void registerListener(ImsRadioServiceListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener is null");
        }
        synchronized(mListeners) {
            if (!mListeners.contains(listener)) {
                mListeners.add(listener);

                if (mImsRadio != null) {
                    listener.onServiceUp(mPhoneId);
                } else {
                    listener.onServiceDown(mPhoneId);
                }
            } else {
                Log.e(this, "Duplicate listener " + listener);
            }
        }
    }

    /**
     * Unregisters listener.
     * @param listener Listener to unregister
     * @see ImsSubContrller#Listener
     * @throws IllegalArgumentException Will throw an error if listener is null.
     */
    public void unregisterListener(ImsRadioServiceListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener is null");
        }
        synchronized(mListeners) {
            if (mListeners.contains(listener)) {
                mListeners.remove(listener);
            } else {
                Log.e(this, "Listener not found " + listener);
            }
        }
    }

    private void notifyServiceUp(String instanceName) {
        Log.i(this, "onServiceUp : " + instanceName);
        synchronized(mListeners) {
            if (mImsRadio != null) {
                for (ImsRadioServiceListener l : mListeners) {
                    l.onServiceUp(mPhoneId);
                }
            } else {
                Log.i(this, "notifyServiceUp :: Can't call onServiceUp. mImsRadio=" + mImsRadio);
            }
        }
    }

    private void notifyServiceDown(String instanceName) {
        Log.i(this, "onServiceDown : " + instanceName);
        synchronized(mListeners) {
            for (ImsRadioServiceListener l : mListeners) {
                l.onServiceDown(mPhoneId);
            }
        }
    }

    /**
     * Class that implements the binder death recipeint to be notified when
     * IImsRadio service dies.
     */
    final class ImsRadioDeathRecipient implements HwBinder.DeathRecipient {
        /**
         * Callback that gets called when the service has died
         */
        @Override
        public void serviceDied(long cookie) {
            Log.e(this, " IImsRadio Died");
            resetServiceAndRequestList();
        }
    }

    private synchronized void resetServiceAndRequestList() {
        notifyServiceDown(IIMS_RADIO_SERVICE_NAME[mPhoneId]);
        clearRequestsList(ImsQmiIF.E_RADIO_NOT_AVAILABLE, false);
        mImsRadio = null;
        mImsRadioResponse = null;
        mImsRadioIndication = null;
    }

    /**
     * Returns true if we are connected to IImsRadio service, false otherwise.
     */
    private boolean isImsRadioServiceConnected() {
        return (mImsRadio != null);
    }

    /**
     * Class that implements the service notification which gets called once the
     * service with fully qualified name fqName has started
     */
    final class ImsRadioServiceNotification extends IServiceNotification.Stub {
        /**
         * Callback that gets called when the service has registered
         * @param fqName - Fully qualified name of the service
         * @param name - Name of the service
         * @param preexisting - if the registration is preexisting
         */
        @Override
        public void onRegistration(String fqName, String name, boolean preexisting) {
            Log.v(this, "Ims Radio interface service started " + fqName + " " + name +
                " preexisting =" + preexisting);
            if (!isImsRadioServiceConnected()) {
                initImsRadio();
            }
        }
    }

    /**
     * Register for notification when the
     * vendor.qti.hardware.radio.ims@1.0::IImsRadio is registered
     */
    private void registerForImsRadioServiceNotification() {
        try {
            final boolean ret = IServiceManager.getService()
                    .registerForNotifications("vendor.qti.hardware.radio.ims@1.0::IImsRadio",
                    IIMS_RADIO_SERVICE_NAME[mPhoneId], mServiceNotification);
            if (!ret) {
                Log.e(this, "Failed to register for service start notifications");
            }
        } catch (RemoteException ex) {
            Log.e(this, "Failed to register for service start notifications. Exception " + ex);
        }
    }

    /**
     * Initialize the instance of IImsRadio. Get the service and register the callback object
     * to be called for the responses to the solicited and unsolicited requests.
     */
    private synchronized void initImsRadio() {
        try {
            mImsRadio = IImsRadio.getService(IIMS_RADIO_SERVICE_NAME[mPhoneId]);
            if (mImsRadio == null) {
                Log.e(this, "initImsRadio: mImsRadio is null. Return");
                return;
            }
            Log.i(this, "initImsRadio: mImsRadio" + mImsRadio);
            mImsRadio.linkToDeath(mDeathRecipient, 0 /* Not Used */);
            mImsRadioResponse = new ImsRadioResponse();
            mImsRadioIndication = new ImsRadioIndication();
            mImsRadio.setCallback(mImsRadioResponse, mImsRadioIndication);
            notifyServiceUp(IIMS_RADIO_SERVICE_NAME[mPhoneId]);
        } catch (Exception ex) {
            Log.e(this, "initImsRadio: Exception: " + ex);
            resetServiceAndRequestList();
        }
    }

    public void registerForImsSubConfigChanged(Handler h, int what, Object obj) {
        mImsSubConfigChangeRegistrants.addUnique(h, what, obj);
    }

    public void deregisterForImsSubConfigChanged(Handler h) {
        mImsSubConfigChangeRegistrants.remove(h);
    }

    public void registerForSsacStatusChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mSsaccRegistrants.add(r);
    }

    public void unregisterForSsacStatusChanged(Handler h) {
        mSsaccRegistrants.remove(h);
    }

    public void registerForVopsStatusChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mVopsRegistrants.add(r);
    }

    public void unregisterForVopsStatusChanged(Handler h) {
        mVopsRegistrants.remove(h);
    }

    public void registerForHandoverStatusChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mHandoverStatusRegistrants.add(r);
    }

    public void unregisterForHandoverStatusChanged(Handler h) {
        mHandoverStatusRegistrants.remove(h);
    }

    public void registerForRefreshConfInfo(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mRefreshConfInfoRegistrations.add(r);
    }

    public void unregisterForRefreshConfInfo(Handler h) {
        mRefreshConfInfoRegistrations.remove(h);
    }

    public void registerForSrvStatusUpdate(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mSrvStatusRegistrations.add(r);
    }

    public void unregisterForSrvStatusUpdate(Handler h) {
        mSrvStatusRegistrations.remove(h);
    }

    public void registerForTtyStatusChanged(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mTtyStatusRegistrants.add(r);
    }

    public void unregisterForTtyStatusChanged(Handler h) {
        mTtyStatusRegistrants.remove(h);
    }

    public void registerForGeolocationRequest(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mGeolocationRegistrants.add(r);
    }

    public void unregisterForGeolocationRequest(Handler h) {
        mGeolocationRegistrants.remove(h);
    }

    public void registerForVoWiFiCallQualityUpdate(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mVoWiFiCallQualityRegistrants.add(r);
    }

    public void unregisterForVoWiFiCallQualityUpdate(Handler h) {
        mVoWiFiCallQualityRegistrants.remove(h);
    }

    public void registerForParticipantStatusInfo(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mParticipantStatusRegistrants.add(r);
    }

    public void unregisterForParticipantStatusInfo(Handler h) {
        mParticipantStatusRegistrants.remove(h);
    }

    public void registerForRegistrationBlockStatus(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mRegistrationBlockStatusRegistrants.add(r);
    }

    public void unregisterForRegistrationBlockStatus(Handler h) {
        mRegistrationBlockStatusRegistrants.remove(h);
    }

    public void setOnSuppServiceNotification(Handler h, int what, Object obj) {
        mSsnRegistrant = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
    }

    public void unSetOnSuppServiceNotification(Handler h) {
        mSsnRegistrant.clear();
    }

    public void registerForSuppServiceIndication(Handler h, int what, Object obj) {
        mSsIndicationRegistrant = new Registrant (h, what, obj);
    }

    public void unregisterForSuppServiceIndication(Handler h) {
        mSsIndicationRegistrant.clear();
    }

    class ImsRadioHandler extends Handler {
        // ***** Handler implementation
        @Override
        public void handleMessage(Message msg) {
            IFRequest rr = (IFRequest) (msg.obj);

            switch (msg.what) {
                case EVENT_WAKE_LOCK_TIMEOUT:
                    /**
                     * Haven't heard back from the last request. Assume we're not getting a
                     * response and release the wake lock.
                     * The timer of WAKE_LOCK_TIMEOUT is reset with each new send request. So when
                     * WAKE_LOCK_TIMEOUT occurs all requests in mRequestList already waited at
                     * least DEFAULT_WAKE_LOCK_TIMEOUT but no response.
                     * Reset mRequestMessagesWaiting to enable releaseWakeLockIfDone().
                     *
                     * Note: Keep mRequestList so that delayed response can still be handled when
                     * response finally comes.
                     */
                    synchronized (mWakeLock) {
                        if (mWakeLock.isHeld()) {

                            if (mRequestMessagesWaiting != 0) {
                                Log.i(this, "Number of messages still waiting for response "
                                        + mRequestMessagesWaiting + " at TIMEOUT. Reset to 0");

                                mRequestMessagesWaiting = 0;

                                synchronized (mRequestsList) {
                                    int count = mRequestsList.size();
                                    Log.i(this, "WAKE_LOCK_TIMEOUT " +
                                            " mRequestList=" + count);

                                    for (int i = 0; i < count; i++) {
                                        rr = mRequestsList.get(i);
                                        Log.i(this, i + ": [" + rr.mSerial + "] "
                                                + msgIdToString(rr.mRequest));
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

    /**
     * Class implementing all callbacks defined in IImsRadioResponse for getting responses from
     * the RIL for the requests sent.
     *
     */
    class ImsRadioResponse extends IImsRadioResponse.Stub {

        /**
         * Callback for getting the response to the DIAL request sent to the RIL via the
         * IImsRadio interface
         * @param token to match request/response. Response must include same token as request
         * @param errorCode of type ImsQmiIF.Error send back from RIL for the dial request
         *
         */
        @Override
        public void dialResponse(int token, int errorCode) {
            // TODO: Map proto error codes to IImsRadio error codes to be used by the interface.
            // Change usage of proto errors of type ImsQmiIF.Error to some proprietary error code
            // and return that to clients.
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void setConfigResponse(int token, int errorCode, ConfigInfo config) {
            Log.i(this, "Set config response received.");
            IFRequest rr = findAndRemoveRequestFromList(token);
            if (rr == null) {
                return;
            }

            Object ret = processConfigResponse(config);
            sendResponse(rr, errorCode, ret);
        }

        @Override
        public void getConfigResponse(int token, int errorCode, ConfigInfo config) {
            Log.i(this, "Get config response received.");
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }

            Object ret = processConfigResponse(config);
            sendResponse(rr, errorCode, ret);
        }

        @Override
        public void queryServiceStatusResponse(int token, int errorCode,
                ArrayList<ServiceStatusInfo> serviceStatusInfoList) {
            Log.i(this, "queryServiceStatus response received.");
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }

            Object ret = null;
            if(serviceStatusInfoList != null) {
                ret = ImsRadioUtils.handleSrvStatus(serviceStatusInfoList);
            }
            sendResponse(rr, errorCode, ret);
        }

        @Override
        public void setServiceStatusResponse(int token, int errorCode) {
            Log.i(this, "SetServiceStatus response received.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        private void sendSipErrorInfo(int token, int errorCode, SipErrorInfo errorInfo) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }

            ImsQmiIF.SipErrorInfo sipErrorInfo = ImsRadioUtils.sipErrorFromHal(errorInfo);
            Log.i(this, msgIdToString(rr.mRequest) + " response - sip error code: " +
                errorInfo.errorCode + " sip error string: " + errorInfo.errorString);
            sendResponse(rr, errorCode, sipErrorInfo);
        }

        @Override
        public void resumeResponse(int token, int errorCode, SipErrorInfo errorInfo) {
            sendSipErrorInfo(token, errorCode, errorInfo);
        }

        @Override
        public void holdResponse(int token, int errorCode, SipErrorInfo errorInfo) {
            sendSipErrorInfo(token, errorCode, errorInfo);
        }

        @Override
        public void hangupResponse(int token, int errorCode) {
            Log.i(this, "Got hangup response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void answerResponse(int token, int errorCode) {
            Log.i(this, "Got answer response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void requestRegistrationChangeResponse(int token, int errorCode) {
            Log.i(this, "Got registration change response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void getRegistrationResponse(int token, int errorCode,
                RegistrationInfo registration) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }

            ImsQmiIF.Registration regMessage = null;
            if (registration != null) {
                regMessage = ImsRadioUtils.registrationFromHal(registration);
            }
            sendResponse(rr, errorCode, regMessage);
        }

        @Override
        public void suppServiceStatusResponse(int token, int errorCode,
            SuppServiceStatus suppServiceStatus) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if (rr == null) {
                return;
            }

            Log.i(this, "Response to " + msgIdToString(rr.mRequest) + " status:" +
                suppServiceStatus.status + " facilityType:" + suppServiceStatus.facilityType +
                " failureCause:" + suppServiceStatus.failureCause + " errorCode:" +
                suppServiceStatus.errorDetails.errorCode);

            ImsQmiIF.SuppSvcResponse suppSvcResponse =
                ImsRadioUtils.suppSvcStatusResponseFromHal(suppServiceStatus);
            sendResponse(rr, errorCode, suppSvcResponse);
        }

        @Override
        public void conferenceResponse(int token, int errorCode, SipErrorInfo errorInfo) {
            sendSipErrorInfo(token, errorCode, errorInfo);
        }

        @Override
        public void getClipResponse(int token, int errorCode,
            ClipProvisionStatus clipProvisionStatus) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if (rr == null) {
                return;
            }

            ImsQmiIF.ClipProvisionStatus clipProvStatus = new ImsQmiIF.ClipProvisionStatus();

            final int clipStatus = clipProvisionStatus.clipStatus;
            if (clipStatus != ClipStatus.INVALID) {
                clipProvStatus.setClipStatus(ImsRadioUtils.clipStatusFromHal(
                    clipProvisionStatus.clipStatus));
                Log.i(this, "getClipResponse from ImsRadio. Clipstatus " + clipStatus);
            }

            if (clipProvisionStatus.hasErrorDetails) {
                clipProvStatus.setErrorDetails(ImsRadioUtils.sipErrorFromHal(
                    clipProvisionStatus.errorDetails));
                Log.i(this, "getClipResponse from ImsRadio. Errorcode " +
                    clipProvisionStatus.errorDetails.errorCode + "String " +
                    clipProvisionStatus.errorDetails.errorString);
            }

            sendResponse(rr, errorCode, clipProvStatus);
        }

        @Override
        public void getClirResponse(int token, int errorCode, ClirInfo clirInfo,
            boolean hasClirInfo) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if (rr == null) {
                return;
            }

            int[] response = null;

            if (hasClirInfo) {
                response = new int[2];
                if (clirInfo.paramN != Integer.MAX_VALUE) {
                    response[0] = clirInfo.paramN;
                }
                if (clirInfo.paramM != Integer.MAX_VALUE) {
                    response[1] = clirInfo.paramM;
                }
                Log.i(this, "getClirResponse from ImsRadio. param_m - " + clirInfo.paramM +
                    "param_n - " + clirInfo.paramN);
            }

            sendResponse(rr, errorCode, response);
        }

        @Override
        public void queryCallForwardStatusResponse(int token, int errorCode,
                ArrayList<vendor.qti.hardware.radio.ims.V1_0.CallForwardInfo> callForwardInfoList,
                SipErrorInfo errorDetails) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }

            Object ret = ImsRadioUtils.buildCFStatusResponseFromHal(callForwardInfoList);
            sendResponse(rr, errorCode, ret);
        }

        @Override
        public void getCallWaitingResponse(int token, int errorCode, int inServiceStatus,
                int serviceClass, SipErrorInfo errorDetails) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }
            int[] response = null;

            if (inServiceStatus != ServiceClassStatus.INVALID) {
                int outServiceStatus = ImsRadioUtils.serviceClassStatusFromHal(inServiceStatus);
                if (outServiceStatus == ImsQmiIF.DISABLED) {
                    response = new int[1];
                    response[0] = ImsQmiIF.DISABLED;
                } else {
                    response = new int[2];
                    response[0] = ImsQmiIF.ENABLED;
                    response[1] = serviceClass;
                }
            }

            sendResponse(rr, errorCode, response);
        }

        @Override
        public void setClirResponse(int token, int errorCode) {
            Log.i(this, "Got setClirResponse from ImsRadio. error " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void getColrResponse(int token, int errorCode, ColrInfo colrInfo) {
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }

            ImsQmiIF.Colr colrValue = new ImsQmiIF.Colr();

            final int presentation = ImsRadioUtils.ipPresentationFromHal(colrInfo.presentation);
            if (presentation != IpPresentation.IP_PRESENTATION_INVALID) {
                colrValue.setPresentation(presentation);
                Log.i(this, "getColrResponse from ImsRadio. presentation " + presentation);
            }

            if (colrInfo.hasErrorDetails) {
                colrValue.setErrorDetails(ImsRadioUtils.sipErrorFromHal(colrInfo.errorDetails));
                Log.i(this, "getColrResponse from ImsRadio. errorcode " +
                    colrInfo.errorDetails.errorCode + "string " +
                    colrInfo.errorDetails.errorString);
            }

            sendResponse(rr, errorCode, colrValue);
        }

        @Override
        public void exitEmergencyCallbackModeResponse(int token, int errorCode) {
            Log.i(this, "Got exitEmergencyCallbackModeResponse from ImsRadio error " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void sendDtmfResponse(int token, int errorCode) {
            Log.i(this, "Got sendDtmfResponse from ImsRadio error " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void startDtmfResponse(int token, int errorCode) {
            Log.i(this, "Got startDtmfResponse from ImsRadio error " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void stopDtmfResponse(int token, int errorCode) {
            Log.i(this, "Got stopDtmfResponse from ImsRadio error " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void setUiTTYModeResponse(int token, int errorCode) {
            Log.i(this, "Got setUiTTYModeResponse from ImsRadio error " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void modifyCallInitiateResponse(int token, int errorCode) {
            Log.i(this, "Got modify call initiate response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void cancelModifyCallResponse(int token, int errorCode) {
            Log.i(this, "Got cancel modify call response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void modifyCallConfirmResponse(int token, int errorCode) {
            Log.i(this, "Got modify call confirm response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void explicitCallTransferResponse(int token, int errorCode) {
            Log.i(this, "Got explicit call transfer response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void setSuppServiceNotificationResponse(int token, int errorCode,
                int serviceStatusClass) {
            Log.i(this, "Got set supp service notification response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void getRtpStatisticsResponse(int token, int errorCode,
                long packetCount) {
            Log.i(this, "Got getRtpStatisticsResponse from ImsRadio packetCount = " +
                     packetCount + " error " + errorCode);
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }
            sendResponse(rr, errorCode, packetCount);
        }

        @Override
        public void getRtpErrorStatisticsResponse(int token, int errorCode,
                long packetErrorCount) {
            Log.i(this, "Got getRtpErrorStatisticsResponse from ImsRadio packetErrorCount = " +
                    packetErrorCount + " error " + errorCode);
            IFRequest rr = findAndRemoveRequestFromList(token);
            if(rr == null) {
                return;
            }
            sendResponse(rr, errorCode, packetErrorCount);
        }

        @Override
        public void addParticipantResponse(int token, int errorCode) {
            // TODO: Map proto error codes to IImsRadio error codes to be used by the interface.
            // Change usage of proto errors of type ImsQmiIF.Error to some proprietary error code
            // and return that to clients.
            Log.i(this, "Add Participant response received. errorCode: " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void deflectCallResponse(int token, int errorCode) {
            Log.i(this, "Got deflect call response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void sendGeolocationInfoResponse(int token, int errorCode) {
            Log.i(this, "Received GeoLocationInfo response from ImsRadio.");
            removeFromQueueAndSendResponse(token, errorCode);
        }

        @Override
        public void getImsSubConfigResponse(int token, int errorCode,
                ImsSubConfigInfo subConfigInfo) {
            Log.i(this, "Received Subconfig response from ImsRadio.");
            IFRequest rr = findAndRemoveRequestFromList(token);
            if (rr == null) {
                return;
            }
            Object ret = ImsRadioUtils.imsSubconfigFromHal(subConfigInfo);
            sendResponse(rr, errorCode, ret);
        }

        public void sendRttMessageResponse(int token, int errorCode) {
            Log.i(this, "Send Rtt Message response received. errorCode: " + errorCode);
            removeFromQueueAndSendResponse(token, errorCode);
        }

    }

     /**
      * Class implementing all callbacks defined in IImsRadioIndication for getting indications
      * from RIL.
      *
      */
    class ImsRadioIndication extends IImsRadioIndication.Stub {

        @Override
        public void onCallStateChanged(ArrayList<CallInfo> callList) {

            if(callList == null) {
                Log.e(this, "Call list is null.");
                return;
            }

            ArrayList<DriverCallIms> response = new ArrayList<DriverCallIms>();
            int numOfCalls = callList.size();
            mIsUnsolCallListPresent = true;

            for(int i = 0; i < numOfCalls; ++i) {
                DriverCallIms dc = ImsRadioUtils.buildDriverCallImsFromHal(callList.get(i));
                // Check for an error message from the network.
                // If network sends a "Forbidden - Not authorized for service" string,
                // throw an intent. This intent is expected to be processed by OMA-DM
                // applications.
                if (dc.callFailCause.mExtraMessage != null &&
                        dc.callFailCause.mExtraMessage.equals(
                        ImsReasonInfo.EXTRA_MSG_SERVICE_NOT_AUTHORIZED)) {
                    log("Throwing ACTION_FORBIDDEN_NO_SERVICE_AUTHORIZATION intent.");
                    Intent intent = new Intent(
                            TelephonyIntents.ACTION_FORBIDDEN_NO_SERVICE_AUTHORIZATION);
                    intent.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
                    mContext.sendBroadcast(intent);
                }
                if (dc.isVoicePrivacy) {
                    mVoicePrivacyOnRegistrants.notifyRegistrants();
                    log("InCall VoicePrivacy is enabled");
                } else {
                    mVoicePrivacyOffRegistrants.notifyRegistrants();
                    log("InCall VoicePrivacy is disabled");
                }
                response.add(dc);
            }

            Collections.sort(response);
            unsljLogRet(ImsQmiIF.UNSOL_RESPONSE_CALL_STATE_CHANGED, response);
            mCallStateRegistrants.notifyRegistrants(new AsyncResult(null, response, null));
        }

        @Override
        public void onRing() {
            unsljLogRet(ImsQmiIF.UNSOL_CALL_RING, null);

            if (mRingRegistrant != null) {
                mRingRegistrant.notifyRegistrant(
                        new AsyncResult(null, null, null));
            }
        }

        @Override
        public void onRingbackTone(int tone) {
            int[] response = new int[1];
            log(" responseCallRingBack ");

            response[0] = ImsRadioUtils.ringbackToneFromHal(tone);
            log("responseCallRingBack " + response[0]);

            boolean playtone = false;
            unsljLogRet(ImsQmiIF.UNSOL_RINGBACK_TONE, response);
            if (response != null) playtone = (response[0] == 1);
            if (mRingbackToneRegistrants != null) {
                mRingbackToneRegistrants.notifyRegistrants(
                        new AsyncResult(null, playtone, null));
            }
        }

        @Override
        public void onRegistrationChanged(RegistrationInfo registration) {
            unsljLog(ImsQmiIF.UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED);
            ImsQmiIF.Registration regMessage = ImsRadioUtils.registrationFromHal(registration);

            mImsNetworkStateChangedRegistrants.notifyRegistrants(
                    new AsyncResult(null, regMessage, null));
        }

        @Override
        public void onHandover(HandoverInfo inHandover) {
            ImsQmiIF.Handover outHandover = ImsRadioUtils.handoverFromHal(inHandover);

            unsljLogRet(ImsQmiIF.UNSOL_RESPONSE_HANDOVER, outHandover);
            if (outHandover != null) {
                mHandoverStatusRegistrants.notifyRegistrants(
                        new AsyncResult(null, outHandover, null));
            }
        }

        @Override
        public void onServiceStatusChanged(ArrayList<ServiceStatusInfo> srvStatusList) {
            Object ret = ImsRadioUtils.handleSrvStatus(srvStatusList);
            unsljLogRet(ImsQmiIF.UNSOL_SRV_STATUS_UPDATE, ret);
            if (ret != null) {
                mSrvStatusRegistrations.notifyRegistrants(new AsyncResult(null, ret, null));
            }
        }

        @Override
        public void onRadioStateChanged(int radioState) {
            unsljLogRet(ImsQmiIF.UNSOL_RADIO_STATE_CHANGED, radioState);
            switch (radioState) {
                case vendor.qti.hardware.radio.ims.V1_0.RadioState.RADIO_STATE_OFF:
                    setRadioState(RadioState.RADIO_OFF);
                    break;
                case vendor.qti.hardware.radio.ims.V1_0.RadioState.RADIO_STATE_UNAVAILABLE:
                    setRadioState(RadioState.RADIO_UNAVAILABLE);
                    break;
                case vendor.qti.hardware.radio.ims.V1_0.RadioState.RADIO_STATE_ON:
                    setRadioState(RadioState.RADIO_ON);
                    break;
                default:
                    Log.e(this, "onRadioStateChanged: Invalid Radio State Change");
                    break;
            }
        }

        @Override
        public void onEnterEmergencyCallBackMode() {
            unsljLog(ImsQmiIF.UNSOL_ENTER_EMERGENCY_CALLBACK_MODE);
            if (mEmergencyCallbackModeRegistrant != null) {
                mEmergencyCallbackModeRegistrant.notifyRegistrant();
            }
        }

        @Override
        public void onExitEmergencyCallBackMode() {
            unsljLog(ImsQmiIF.UNSOL_EXIT_EMERGENCY_CALLBACK_MODE);
            if (mExitEmergencyCallbackModeRegistrants != null) {
                mExitEmergencyCallbackModeRegistrants.notifyRegistrants(
                    new AsyncResult(null, null, null));
            }
        }

        @Override
        public void onTtyNotification(TtyInfo ttyInfo) {
            int[] mode = null;
            if (ttyInfo != null) {
                mode = new int[1];
                mode[0] = ImsRadioUtils.ttyModeFromHal(ttyInfo.mode);
            }

            // Userdata is not being used currently
            byte[] ret;
            if (ttyInfo.userData != null && !ttyInfo.userData.isEmpty()) {
                ret = ImsRadioUtils.getByteArray(ttyInfo.userData);
            }
            unsljLogRet(ImsQmiIF.UNSOL_TTY_NOTIFICATION, mode);
            if (mode != null && mTtyStatusRegistrants != null) {
                mTtyStatusRegistrants.notifyRegistrants(new AsyncResult(null, mode, null));
            }
        }

        @Override
        public void onRefreshConferenceInfo(ConferenceInfo conferenceInfo) {
            if (conferenceInfo != null) {
                ImsQmiIF.ConfInfo info = new ImsQmiIF.ConfInfo();

                if (conferenceInfo.confInfoUri != null && !conferenceInfo.confInfoUri.isEmpty()) {
                    info.setConfInfoUri(ImsRadioUtils.getByteStringMicro(
                        conferenceInfo.confInfoUri));
                    Log.v(this, "onRefreshConferenceInfo: confUri = " +
                        conferenceInfo.confInfoUri);
                }

                if (conferenceInfo.conferenceCallState != ConferenceCallState.INVALID) {
                    info.setConfCallState(ImsRadioUtils.conferenceCallStateFromHal(
                    conferenceInfo.conferenceCallState));
                }
                Log.v(this, "onRefreshConferenceInfo: conferenceCallState = " +
                    conferenceInfo.conferenceCallState);
                unsljLogRet(ImsQmiIF.UNSOL_REFRESH_CONF_INFO, info);
                if (info != null) {
                    mRefreshConfInfoRegistrations.notifyRegistrants(
                        new AsyncResult(null, info, null));
                }
            }
        }

        @Override
        public void onRefreshViceInfo(ViceInfo viceInfo) {
            if (viceInfo != null && viceInfo.viceInfoUri != null) {
                ImsQmiIF.ViceInfo info = new ImsQmiIF.ViceInfo();
                // Notify the registrants in both the following cases :
                // 1) Valid Vice XML present
                // 2) Vice XML with 0 length. For ex: Modem triggers indication with 0 length
                // when APM toggle happens. Notify so that UI gets cleared
                if (viceInfo.viceInfoUri != null) {
                    Log.v(this, "onRefreshViceInfo: viceUri = " + viceInfo.viceInfoUri);
                    info.setViceInfoUri(ImsRadioUtils.getByteStringMicro(viceInfo.viceInfoUri));
                    unsljLogRet(ImsQmiIF.UNSOL_REFRESH_VICE_INFO, info);
                    if (info != null) {
                        mRefreshViceInfoRegistrants
                            .notifyRegistrants(new AsyncResult(null, info, null));
                    }
                }
            }
        }

        /**
         * ImsQmiIF.UNSOL_MODIFY_CALL
         *
         * @param CallModifyInfo call modify info.
         */
        @Override
        public void onModifyCall(CallModifyInfo callModifyInfo) {
            if (callModifyInfo == null) {
                unsljLogRet(ImsQmiIF.UNSOL_MODIFY_CALL, null);
                Log.e(this, "onModifyCall callModifyInfo is null");
                return;
            }
            CallModify ret = ImsRadioUtils.callModifyFromHal(callModifyInfo);
            unsljLogRet(ImsQmiIF.UNSOL_MODIFY_CALL, ret);
            mModifyCallRegistrants
                    .notifyRegistrants(new AsyncResult(null, ret, null));
        }

        @Override
        public void onSuppServiceNotification(SuppServiceNotification suppServiceNotification) {
            Object ret = ImsRadioUtils.suppServiceNotificationFromHal(suppServiceNotification);
            unsljLogRet(ImsQmiIF.UNSOL_SUPP_SVC_NOTIFICATION, ret);
            if (mSsnRegistrant != null) {
                mSsnRegistrant.notifyRegistrant(
                        new AsyncResult (null, ret, null));
            }
        }

        @Override
        public void onMessageWaiting(MessageWaitingIndication messageWaitingIndication) {
            ImsQmiIF.Mwi ret = ImsRadioUtils.messageWaitingIndicationFromHal(
                    messageWaitingIndication);
            unsljLogRet(ImsQmiIF.UNSOL_MWI, ret);
            if (ret != null) {
                mMwiRegistrants.notifyRegistrants(new AsyncResult (null, ret, null));
            } else {
                Log.e(this, "onMessageWaiting: Data is null");
            }
        }

        @Override
        public void onGeolocationInfoRequested(double lat, double lon) {
            ImsQmiIF.GeoLocationInfo ret =
                    ImsRadioUtils.geolocationIndicationFromHal(lat, lon);
            unsljLogRet(ImsQmiIF.UNSOL_REQUEST_GEOLOCATION, ret);
            if (ret != null) {
                mGeolocationRegistrants.notifyRegistrants(new AsyncResult (null, ret, null));
            } else {
                Log.e(this, "onGeolocationInfoRequested: Null location data!");
            }
        }

        public void onImsSubConfigChanged(ImsSubConfigInfo config) {
            ImsQmiIF.ImsSubConfig ret = ImsRadioUtils.imsSubconfigFromHal(config);
            unsljLogRet(ImsQmiIF.UNSOL_IMS_SUB_CONFIG_CHANGED, ret);
            if (ret != null) {
                mImsSubConfigChangeRegistrants
                    .notifyRegistrants(new AsyncResult(null, ret, null));
            }
        }

        @Override
        public void onParticipantStatusInfo(ParticipantStatusInfo participantStatusInfo) {
            ImsQmiIF.ParticipantStatusInfo ret =
                    ImsRadioUtils.participantStatusFromHal(participantStatusInfo);
            unsljLogRet(ImsQmiIF.UNSOL_PARTICIPANT_STATUS_INFO, ret);
            if (ret != null) {
                mParticipantStatusRegistrants
                    .notifyRegistrants(new AsyncResult(null, ret, null));
            }
        }

        @Override
        public void onRegistrationBlockStatus(boolean hasBlockStatusOnWwan,
                BlockStatus blockStatusOnWwan, boolean hasBlockStatusOnWlan,
                BlockStatus blockStatusOnWlan) {
            ImsQmiIF.RegistrationBlockStatus ret =
                    ImsRadioUtils.registrationBlockStatusFromHal(hasBlockStatusOnWwan,
                            blockStatusOnWwan, hasBlockStatusOnWlan, blockStatusOnWlan);
            unsljLogRet(ImsQmiIF.UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS, ret);
            if (ret != null) {
                mRegistrationBlockStatusRegistrants.notifyRegistrants(
                        new AsyncResult (null, ret, null));
            } else {
                Log.e(this, "onRegistrationBlockStatus: Data is null.");
            }
        }

        public void onRttMessageReceived(String msg) {
            if (msg != null) {
                Log.v(this, "onRttMessageReceived: msg = " + msg);
                unsljLogRet(ImsQmiIF.UNSOL_RESPONSE_RTT_MSG_RECEIVED, msg);
                mRttMessageRegistrants
                    .notifyRegistrants(new AsyncResult(null, msg, null));
            }
        }

        @Override
        public void onVoWiFiCallQuality(int voWiFiCallQuality) {
            unsljLogRet(ImsQmiIF.UNSOL_VOWIFI_CALL_QUALITY, voWiFiCallQuality);
            Object ret = ImsRadioUtils.voWiFiCallQualityFromHal(voWiFiCallQuality);
            if (mVoWiFiCallQualityRegistrants != null) {
                mVoWiFiCallQualityRegistrants.notifyRegistrants(
                        new AsyncResult (null, ret, null));
            }
        }

        @Override
        public void onSupplementaryServiceIndication(StkCcUnsolSsResult ss) {
            ImsSsData ssData = new ImsSsData();

            ssData.mServiceType = ImsRadioUtils.serviceTypeFromRILServiceType(ss.serviceType);
            ssData.mRequestType = ImsRadioUtils.requestTypeFromRILRequestType(ss.requestType);
            ssData.mTeleserviceType = ImsRadioUtils.teleserviceTypeFromRILTeleserviceType(
                    ss.teleserviceType);
            ssData.mServiceClass = ss.serviceClass; // This is service class sent in the SS request.
            ssData.mResult = ss.result; // This is the result of the SS request.

            int num;
            if (ssData.isTypeCF() && ssData.isTypeInterrogation()) {
                CfData cfData = ss.cfData.get(0);
                Object ret = ImsRadioUtils.buildCFStatusResponseFromHal(cfData.cfInfo);
                if (ret instanceof ImsCallForwardTimerInfo[]) {
                    Log.i(this, "onSupplementaryServiceIndication : Not handling CFUT SS");
                    unsljLogRet(ImsQmiIF.UNSOL_ON_SS, null);
                    return;
                } else {
                    num = cfData.cfInfo.size();
                    ssData.mCfInfo = new ImsCallForwardInfo[num];
                    for (int i = 0; i < num; i++) {
                        vendor.qti.hardware.radio.ims.V1_0.CallForwardInfo cfInfo = cfData.
                                cfInfo.get(i);
                        ssData.mCfInfo[i] = new ImsCallForwardInfo();

                        ssData.mCfInfo[i].mStatus = cfInfo.status;
                        ssData.mCfInfo[i].mCondition= ImsRadioUtils.
                                getUTInterfaceCFReasonFromCommandsInterfaceCFReason(cfInfo.reason);
                        ssData.mCfInfo[i].mServiceClass = cfInfo.serviceClass;
                        ssData.mCfInfo[i].mToA = cfInfo.toa;
                        ssData.mCfInfo[i].mNumber = cfInfo.number;
                        ssData.mCfInfo[i].mTimeSeconds = cfInfo.timeSeconds;

                        Log.i(this, "[SS Data] CF Info " + i + " : " +  ssData.mCfInfo[i]);
                    }
                }
            } else if (ssData.isTypeIcb() && ssData.isTypeInterrogation()) {
                num = ss.cbNumInfo.size();
                ssData.mImsSsInfo = new ImsSsInfo[num];
                for (int i = 0; i < num; i++) {
                    ssData.mImsSsInfo[i] = new ImsSsInfo();
                    ssData.mImsSsInfo[i].mStatus = ss.cbNumInfo.get(i).status;
                    ssData.mImsSsInfo[i].mIcbNum = ss.cbNumInfo.get(i).number;
                    Log.i(this, "[SS Data] ICB Info " + i + " : " +  ssData.mImsSsInfo[i]);
                }
            } else {
                SsInfoData ssInfoData = ss.ssInfoData.get(0);
                num = ssInfoData.ssInfo.size();
                ssData.mSsInfo = new int[num];
                for (int i = 0; i < num; i++) {
                    ssData.mSsInfo[i] = ssInfoData.ssInfo.get(i);
                    Log.i(this, "[SS Data] SS Info " + i + " : " +  ssData.mSsInfo[i]);
                }
            }

            unsljLogRet(ImsQmiIF.UNSOL_ON_SS, ssData);

            if (mSsIndicationRegistrant != null) {
                mSsIndicationRegistrant.notifyRegistrant(new AsyncResult(null, ssData, null));
            }
        }
    }

    /**
     * Removes the request matching the token id from the request list and sends the response
     * to the client
     *
     * @param token to match request/response. Response must include same token as request
     * @param errorCode of type ImsQmiIF.Error send back from RIL for the dial request
     *
     */
    private void removeFromQueueAndSendResponse(int token, int errorCode) {
        IFRequest rr = findAndRemoveRequestFromList(token);

        if (rr == null) {
            Log.w(this, "Unexpected solicited response or Invalid token id! token: "
                        + token + " error: " + errorCode);
            return;
        }
        sendResponse(rr, errorCode, null);
    }

    /**
     * Sends the response to the client corresponding to the IFRequest passed in as a param and
     * returns any error send to the client along with the result object
     *
     * @param rr IFRequest containing the token id, request type, request id, etc.
     * @param errorCode of type ImsQmiIF.Error send back from RIL for the dial request
     * @param ret Result object of the request to be passed back to the client
     *
     */
    private void sendResponse(IFRequest rr, int error, Object ret) {
        if (error != ImsQmiIF.E_SUCCESS) {
            rr.onError(error, ret);
            rr.release();
            releaseWakeLockIfDone();
            return;
        }

        log(rr.serialString() + "< " + msgIdToString(rr.mRequest)
                + " " + retToString(rr.mRequest, ret));

        if (rr.mResult != null) {
            AsyncResult.forMessage(rr.mResult, ret, null);
            rr.mResult.sendToTarget();
        }

        rr.release();
        releaseWakeLockIfDone();
    }

    public ImsSenderRxr(Context context, int phoneId)
    {
        super(context);

        mPhoneId = phoneId;
        initNotifyRegistrants();

        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, LOG_TAG);
        mWakeLock.setReferenceCounted(false);
        mWakeLockTimeout = SystemProperties.getInt(TelephonyProperties.PROPERTY_WAKE_LOCK_TIMEOUT,
                DEFAULT_WAKE_LOCK_TIMEOUT);
        mImsRadioHandler = new ImsRadioHandler();
        mRequestMessagesWaiting = 0;

        mDeathRecipient = new ImsRadioDeathRecipient();
        // register for ImsRadio service notification
        registerForImsRadioServiceNotification();
    }

    private void initNotifyRegistrants() {
        mHandoverStatusRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRefreshConfInfoRegistrations = new WakeLockRegistrantList(mNotifyWakeLock);
        mSrvStatusRegistrations = new WakeLockRegistrantList(mNotifyWakeLock);
        mTtyStatusRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRadioStateRegistrations = new WakeLockRegistrantList(mNotifyWakeLock);
        mGeolocationRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mVoWiFiCallQualityRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mSsaccRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mVopsRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mParticipantStatusRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mImsSubConfigChangeRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRegistrationBlockStatusRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mModifyCallRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mMwiRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRttMessageRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
        mRttModifyRegistrants = new WakeLockRegistrantList(mNotifyWakeLock);
    }

    /**
     * Holds a PARTIAL_WAKE_LOCK whenever a) There is outstanding request sent
     * to the interface and no replied b) There is a request pending to be sent
     * out. There is a WAKE_LOCK_TIMEOUT to release the lock, though it
     * shouldn't happen often.
     */

    private void acquireWakeLock() {
        synchronized (mWakeLock) {
            mWakeLock.acquire();

            mImsRadioHandler.removeMessages(EVENT_WAKE_LOCK_TIMEOUT);
            Message msg = mImsRadioHandler.obtainMessage(EVENT_WAKE_LOCK_TIMEOUT);
            mImsRadioHandler.sendMessageDelayed(msg, mWakeLockTimeout);
        }
    }

    private void releaseWakeLockIfDone() {
        synchronized (mWakeLock) {
            if (mWakeLock.isHeld() &&
                    (mRequestMessagesWaiting == 0)) {
                mWakeLock.release();
            }
        }
    }

    /**
     * Acquires the wake lock, queues the IFRequest in the request list so that when the response
     * comes we can match it with the request.
     *
     * @param rr IFRequest containing the token id, request type, request id, etc.
     */
    private void queueRequest(IFRequest rr) {
        acquireWakeLock();

       synchronized (mRequestsList) {
           mRequestsList.add(rr);
           mRequestMessagesWaiting++;
        }
    }

    /**
     * Release each request in mRequestsList then clear the list
     *
     * @param error is the ImsQmiIF.Error sent back
     * @param loggable true means to print all requests in mRequestslist
     */
    private void clearRequestsList(int error, boolean loggable) {
        IFRequest rr;
        synchronized (mRequestsList) {
            int count = mRequestsList.size();
            if (loggable) {
                Log.i(this,"clearRequestsList: mRequestList=" + count);
            }

            for (int i = 0; i < count; i++) {
                rr = mRequestsList.get(i);
                if (loggable) {
                    Log.i(this, i + ": [" + rr.mSerial + "] " +
                            msgIdToString(rr.mRequest));
                }
                rr.onError(error, null);
                rr.release();
            }
            mRequestsList.clear();
            mRequestMessagesWaiting = 0;
            releaseWakeLockIfDone();
        }

        /* Clear the existing calls also */
        if (mIsUnsolCallListPresent) {
            mIsUnsolCallListPresent = false;
            mCallStateRegistrants
                    .notifyRegistrants(new AsyncResult(null, null,
                    new RuntimeException(ImsSenderRxr.errorIdToString(error))));
        }
    }

    private IFRequest findAndRemoveRequestFromList(int serial) {
        synchronized (mRequestsList) {
            for (int i = 0, s = mRequestsList.size(); i < s; i++) {
                IFRequest rr = mRequestsList.get(i);

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

    private String retToString(int req, Object ret) {

        if (ret == null)
            return "";

        StringBuilder sb;
        String s;
        int length;
        if (ret instanceof int[]) {
            int[] intArray = (int[]) ret;
            length = intArray.length;
            sb = new StringBuilder("{");
            if (length > 0) {
                int i = 0;
                sb.append(intArray[i++]);
                while (i < length) {
                    sb.append(", ").append(intArray[i++]);
                }
            }
            sb.append("}");
            s = sb.toString();
        } else if (ret instanceof String[]) {
            String[] strings = (String[]) ret;
            length = strings.length;
            sb = new StringBuilder("{");
            if (length > 0) {
                int i = 0;
                sb.append(strings[i++]);
                while (i < length) {
                    sb.append(", ").append(strings[i++]);
                }
            }
            sb.append("}");
            s = sb.toString();
        } else if (req == ImsQmiIF.UNSOL_RESPONSE_CALL_STATE_CHANGED
                || req == ImsQmiIF.REQUEST_GET_CURRENT_CALLS ) {
            ArrayList<DriverCallIms> calls = (ArrayList<DriverCallIms>) ret;
            sb = new StringBuilder(" ");
            for (DriverCallIms dc : calls) {
                sb.append("[").append(dc).append("] ");
            }
            s = sb.toString();
        } else {
            s = ret.toString();
        }
        return s;
    }

    public void registerForModifyCall(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mModifyCallRegistrants.add(r);
    }

    public void unregisterForModifyCall(Handler h) {
        mModifyCallRegistrants.remove(h);
    }

    public void registerForMwi(Handler h, int what, Object obj) {
        Registrant r = new WakeLockRegistrant(h, what, obj, mNotifyWakeLock);
        mMwiRegistrants.add(r);
    }

    public void unregisterForMwi(Handler h) {
        mMwiRegistrants.remove(h);
    }

    static String errorIdToString(int request) {
        String errorMsg;
        switch (request) {
            case ImsQmiIF.E_SUCCESS:
                errorMsg = "SUCCESS";
                break;
            case ImsQmiIF.E_RADIO_NOT_AVAILABLE:
                errorMsg = "E_RADIO_NOT_AVAILABLE";
                break;
            case ImsQmiIF.E_GENERIC_FAILURE:
                errorMsg = "E_GENERIC_FAILURE";
                break;
            case ImsQmiIF.E_REQUEST_NOT_SUPPORTED:
                errorMsg = "E_REQUEST_NOT_SUPPORTED";
                break;
            case ImsQmiIF.E_CANCELLED:
                errorMsg = "E_CANCELLED";
                break;
            case ImsQmiIF.E_UNUSED:
                errorMsg = "E_UNUSED";
                break;
            case ImsQmiIF.E_INVALID_PARAMETER:
                errorMsg = "E_INVALID_PARAMETER";
                break;
            case ImsQmiIF.E_REJECTED_BY_REMOTE:
                errorMsg = "E_REJECTED_BY_REMOTE";
                break;
            case ImsQmiIF.E_NETWORK_NOT_SUPPORTED:
                errorMsg = "E_NETWORK_NOT_SUPPORTED";
                break;
            case ImsQmiIF.E_FDN_CHECK_FAILURE:
                errorMsg = "E_FDN_CHECK_FAILURE";
                break;
            case ImsQmiIF.E_SS_MODIFIED_TO_DIAL:
                errorMsg = "E_SS_MODIFIED_TO_DIAL";
                break;
            case ImsQmiIF.E_SS_MODIFIED_TO_USSD:
                errorMsg = "E_SS_MODIFIED_TO_USSD";
                break;
            case ImsQmiIF.E_SS_MODIFIED_TO_SS:
                errorMsg = "E_SS_MODIFIED_TO_SS";
                break;
            case ImsQmiIF.E_SS_MODIFIED_TO_DIAL_VIDEO:
                errorMsg = "E_SS_MODIFIED_TO_DIAL_VIDEO";
                break;
            default:
                errorMsg = "E_UNKNOWN";
                break;
        }
        return errorMsg;
    }

    static String msgIdToString(int request) {
        // TODO - check all supported messages are covered
        switch (request) {
            case ImsQmiIF.REQUEST_GET_CURRENT_CALLS:
                return "GET_CURRENT_CALLS";
            case ImsQmiIF.REQUEST_DIAL:
                return "DIAL";
            case ImsQmiIF.REQUEST_ANSWER:
                return "REQUEST_ANSWER";
            case ImsQmiIF.REQUEST_DEFLECT_CALL:
                return "REQUEST_DEFLECT_CALL";
            case ImsQmiIF.REQUEST_ADD_PARTICIPANT:
                return "REQUEST_ADD_PARTICIPANT";
            case ImsQmiIF.REQUEST_HANGUP:
                return "HANGUP";
            case ImsQmiIF.REQUEST_HANGUP_WAITING_OR_BACKGROUND:
                return "HANGUP_WAITING_OR_BACKGROUND";
            case ImsQmiIF.REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND:
                return "HANGUP_FOREGROUND_RESUME_BACKGROUND";
            case ImsQmiIF.REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE:
                return "ImsQmiIF.REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
            case ImsQmiIF.REQUEST_CONFERENCE:
                return "CONFERENCE";
            case ImsQmiIF.REQUEST_UDUB:
                return "UDUB";
            case ImsQmiIF.REQUEST_SEND_UI_TTY_MODE:
                return "REQUEST_SEND_UI_TTY_MODE";
            case ImsQmiIF.REQUEST_MODIFY_CALL_INITIATE:
                return "MODIFY_CALL_INITIATE";
            case ImsQmiIF.REQUEST_MODIFY_CALL_CONFIRM:
                return "MODIFY_CALL_CONFIRM";
            case ImsQmiIF.UNSOL_MODIFY_CALL:
                return "UNSOL_MODIFY_CALL";
            case ImsQmiIF.REQUEST_LAST_CALL_FAIL_CAUSE:
                return "LAST_CALL_FAIL_CAUSE";
            case ImsQmiIF.REQUEST_DTMF:
                return "DTMF";
            case ImsQmiIF.REQUEST_DTMF_START:
                return "DTMF_START";
            case ImsQmiIF.REQUEST_DTMF_STOP:
                return "DTMF_STOP";
            case ImsQmiIF.REQUEST_EXPLICIT_CALL_TRANSFER:
                return "ImsQmiIF.REQUEST_EXPLICIT_CALL_TRANSFER";
            case ImsQmiIF.REQUEST_EXIT_EMERGENCY_CALLBACK_MODE:
                return "ImsQmiIF.REQUEST_EXIT_EMERGENCY_CALLBACK_MODE";
            case ImsQmiIF.REQUEST_IMS_REGISTRATION_STATE:
                return "REQUEST_IMS_REGISTRATION_STATE";
            case ImsQmiIF.REQUEST_QUERY_CLIP:
                return "REQUEST_QUERY_CLIP";
            case ImsQmiIF.REQUEST_QUERY_SERVICE_STATUS:
                return "REQUEST_QUERY_SERVICE_STATUS";
            case ImsQmiIF.REQUEST_SET_SERVICE_STATUS:
                return "REQUEST_SET_SERVICE_STATUS";
            case ImsQmiIF.REQUEST_GET_CLIR:
                return "REQUEST_GET_CLIR";
            case ImsQmiIF.REQUEST_SET_CLIR:
                return "REQUEST_SET_CLIR";
            case ImsQmiIF.REQUEST_QUERY_CALL_FORWARD_STATUS:
                return "REQUEST_QUERY_CALL_FORWARD_STATUS";
            case ImsQmiIF.REQUEST_SET_CALL_FORWARD_STATUS:
                return "REQUEST_SET_CALL_FORWARD_STATUS";
            case ImsQmiIF.REQUEST_QUERY_CALL_WAITING:
                return "REQUEST_QUERY_CALL_WAITING";
            case ImsQmiIF.REQUEST_SET_CALL_WAITING:
                return "REQUEST_SET_CALL_WAITING";
            case ImsQmiIF.REQUEST_SET_SUPP_SVC_NOTIFICATION:
                return "REQUEST_SET_SUPP_SVC_NOTIFICATION";
            case ImsQmiIF.REQUEST_SUPP_SVC_STATUS:
                return "REQUEST_SUPP_SVC_STATUS";
            case ImsQmiIF.REQUEST_GET_RTP_STATISTICS:
                return "REQUEST_GET_RTP_STATISTICS";
            case ImsQmiIF.REQUEST_GET_RTP_ERROR_STATISTICS:
                return "REQUEST_GET_RTP_ERROR_STATISTICS";
            case ImsQmiIF.REQUEST_GET_WIFI_CALLING_STATUS:
                return "REQUEST_GET_WIFI_CALLING_STATUS";
            case ImsQmiIF.REQUEST_SET_WIFI_CALLING_STATUS:
                return "REQUEST_SET_WIFI_CALLING_STATUS";
            case ImsQmiIF.REQUEST_GET_COLR:
                return "REQUEST_GET_COLR";
            case ImsQmiIF.REQUEST_SET_COLR:
                return "REQUEST_SET_COLR";
            case ImsQmiIF.REQUEST_HOLD:
                return "REQUEST_HOLD";
            case ImsQmiIF.REQUEST_RESUME:
                return "REQUEST_RESUME";
            case ImsQmiIF.REQUEST_SET_IMS_CONFIG:
                return "REQUEST_SET_IMS_CONFIG";
            case ImsQmiIF.REQUEST_GET_IMS_CONFIG:
                return "REQUEST_GET_IMS_CONFIG";
            case ImsQmiIF.REQUEST_SEND_GEOLOCATION_INFO:
                return "REQUEST_SEND_GEOLOCATION_INFO";
            case ImsQmiIF.REQUEST_GET_VOPS_INFO:
                return "REQUEST_GET_VOPS_INFO";
            case ImsQmiIF.REQUEST_GET_SSAC_INFO:
                return "REQUEST_GET_SSAC_INFO";
            case ImsQmiIF.REQUEST_SET_VOLTE_PREF:
                return "REQUEST_SET_VOLTE_PREF";
            case ImsQmiIF.REQUEST_GET_VOLTE_PREF:
                return "REQUEST_GET_VOLTE_PREF";
            case ImsQmiIF.REQUEST_GET_HANDOVER_CONFIG:
                return "REQUEST_GET_HANDOVER_CONFIG";
            case ImsQmiIF.REQUEST_SET_HANDOVER_CONFIG:
                return "REQUEST_SET_HANDOVER_CONFIG";
            case ImsQmiIF.REQUEST_GET_IMS_SUB_CONFIG:
                return "REQUEST_GET_IMS_SUB_CONFIG";
            case ImsQmiIF.REQUEST_SEND_RTT_MSG:
                return "REQUEST_SEND_RTT_MSG";
            case ImsQmiIF.REQUEST_CANCEL_MODIFY_CALL:
                return "REQUEST_CANCEL_MODIFY_CALL";
            case ImsQmiIF.UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED:
                return "UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED";
            case ImsQmiIF.UNSOL_RESPONSE_CALL_STATE_CHANGED:
                return "UNSOL_RESPONSE_CALL_STATE_CHANGED";
            case ImsQmiIF.UNSOL_CALL_RING:
                return "UNSOL_CALL_RING";
            case ImsQmiIF.UNSOL_ENTER_EMERGENCY_CALLBACK_MODE:
                return "UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
            case ImsQmiIF.UNSOL_RINGBACK_TONE:
                return "UNSOL_RINGBACK_TONE";
            case ImsQmiIF.UNSOL_EXIT_EMERGENCY_CALLBACK_MODE:
                return "UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
            case ImsQmiIF.REQUEST_IMS_REG_STATE_CHANGE:
                return "REQUEST_IMS_REG_STATE_CHANGE";
            case ImsQmiIF.UNSOL_RESPONSE_HANDOVER:
                return "UNSOL_RESPONSE_HANDOVER";
            case ImsQmiIF.UNSOL_REFRESH_CONF_INFO:
                return "UNSOL_REFRESH_CONF_INFO";
            case ImsQmiIF.UNSOL_SRV_STATUS_UPDATE:
                return "UNSOL_SRV_STATUS_UPDATE";
            case ImsQmiIF.UNSOL_SUPP_SVC_NOTIFICATION:
                return "UNSOL_SUPP_SVC_NOTIFICATION";
            case ImsQmiIF.UNSOL_TTY_NOTIFICATION:
                return "UNSOL_TTY_NOTIFICATION";
            case ImsQmiIF.UNSOL_RADIO_STATE_CHANGED:
                return "UNSOL_RADIO_STATE_CHANGED";
            case ImsQmiIF.UNSOL_MWI:
                return "UNSOL_MWI";
            case ImsQmiIF.UNSOL_REQUEST_GEOLOCATION:
                return "UNSOL_REQUEST_GEOLOCATION";
            case ImsQmiIF.UNSOL_REFRESH_VICE_INFO:
                return "UNSOL_REFRESH_VICE_INFO";
            case ImsQmiIF.UNSOL_VOWIFI_CALL_QUALITY:
                return "UNSOL_VOWIFI_CALL_QUALITY";
            case ImsQmiIF.UNSOL_VOPS_CHANGED:
                return "UNSOL_VOPS_CHANGED";
            case ImsQmiIF.UNSOL_SSAC_CHANGED:
                return "UNSOL_SSAC_CHANGED";
            case ImsQmiIF.UNSOL_PARTICIPANT_STATUS_INFO:
                return "UNSOL_PARTICIPANT_STATUS_INFO";
            case ImsQmiIF.UNSOL_IMS_SUB_CONFIG_CHANGED:
                return "UNSOL_IMS_SUB_CONFIG_CHANGED";
            case ImsQmiIF.UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS:
                return "UNSOL_RESPONSE_REGISTRATION_BLOCK_STATUS";
            case ImsQmiIF.UNSOL_RESPONSE_RTT_MSG_RECEIVED:
                return "UNSOL_RESPONSE_RTT_MSG_RECEIVED";
            case ImsQmiIF.UNSOL_ON_SS:
                return "UNSOL_ON_SS";
            default:
                return "<unknown message>";
        }
    }

    public void log(String msg) {
        Log.i(this, msg + "[SUB" + mPhoneId + "]");
    }

    public void logv(String msg) {
        Log.v(this, msg + "[SUB" + mPhoneId + "]");
    }

    /**
     * Use this only for unimplemented methods. Prints stack trace if the
     * unimplemented method is ever called
     */
    public void logUnimplemented() {
        try {
            Exception e = new Exception();
            throw e;
        } catch (Exception e) {
            Log.i(this, "Unimplemented method. Stack trace: ");
            e.printStackTrace();
        }
    }

    public void unsljLog(int response) {
        log("[UNSL]< " + msgIdToString(response));
    }

    public void unsljLogMore(int response, String more) {
        log("[UNSL]< " + msgIdToString(response) + " " + more);
    }

    public void unsljLogRet(int response, Object ret) {
        log("[UNSL]< " + msgIdToString(response) + " " + retToString(response, ret));
    }

    public void unsljLogvRet(int response, Object ret) {
        logv("[UNSL]< " + msgIdToString(response) + " " + retToString(response, ret));
    }

    public void logSolicitedRequest(IFRequest rr) {
        log(rr.serialString() + "> " + msgIdToString(rr.mRequest) + " ");
    }

    @Override
    public void setPhoneType(int phoneType) { // Called by Phone constructor
        log("setPhoneType=" + phoneType + " old value=" + mPhoneType);
        mPhoneType = phoneType;
    }

    public void addParticipant(String address, int clirMode, CallDetails callDetails,
            Message result) {
        log("addParticipant address = " + Log.pii(address) + " clirMode = " + clirMode
                + " callDetails = " + callDetails);
        final int msgId = ImsQmiIF.REQUEST_ADD_PARTICIPANT;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        DialRequest dialRequest = ImsRadioUtils.buildDialRequest(address, clirMode,
            callDetails, false /*isEncrypted*/);
        try {
            logSolicitedRequest(rr);
            mImsRadio.addParticipant(rr.mSerial, dialRequest);
            Log.v(this, msgIdString + " request to IImsRadio - token = " + rr.mSerial +
                    " address = " + dialRequest.address + " callType = " +
                    dialRequest.callDetails.callType + " callDomain = " +
                    dialRequest.callDetails.callDomain + " isConferenceUri = " +
                    dialRequest.isConferenceUri + "isCallPull =" + dialRequest.isCallPull +
                    " isEncrypted = " + dialRequest.isEncrypted);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + " to IImsRadio: Exception: " + ex);
        }
    }

    /**
     * If the Ims Radio service is down, send the error to clients and release the IFRequest
     *
     * @param rr IFRequest containing the token id, request type, request id, etc.
     * @return boolean - true if error was sent on Ims Radio service down, false if Service is up.
     */
    private boolean sendErrorOnImsRadioDown(IFRequest rr, String msgIdString) {

        if (mImsRadio == null) {
            Log.e(this, "mImsRadio is null. Can't send" + msgIdString + " to QCRIL");
            rr.onError(ImsQmiIF.E_RADIO_NOT_AVAILABLE, null);
            rr.release();
            return true;
        }
        return false;
    }

    public void
    dial(String address, int clirMode, CallDetails callDetails,
            boolean isEncrypted, Message result) {
        log("Dial Request - address= " + Log.pii(address) + "clirMode= " + clirMode
                + " callDetails= " + callDetails + " isEncrypted= " + isEncrypted);
        final int msgId = ImsQmiIF.REQUEST_DIAL;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        DialRequest dialRequest = ImsRadioUtils.buildDialRequest(address, clirMode,
            callDetails, isEncrypted);
        try {
            logSolicitedRequest(rr);
            mImsRadio.dial(rr.mSerial, dialRequest);
            Log.v(this, msgIdString + " request to IImsRadio - token = " + rr.mSerial +
                "address = " + dialRequest.address + "callType =" +
                dialRequest.callDetails.callType + "callDomain =" +
                dialRequest.callDetails.callDomain + "isConferenceUri =" +
                dialRequest.isConferenceUri + "isCallPull =" + dialRequest.isCallPull +
                "isEncrypted =" + dialRequest.isEncrypted +
                "rttMode =" + dialRequest.callDetails.rttMode);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "to IImsRadio: Exception: " + ex);
        }
    }

    public void
    acceptCall(Message result, int callType) {
        acceptCall(result, callType, IpPresentation.IP_PRESENTATION_NUM_DEFAULT);
    }

    public void
    acceptCall(Message result, int callType, int mode) {
        final int msgId = ImsQmiIF.REQUEST_ANSWER;
        final String msgIdString = msgIdToString(msgId);

        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.answer(rr.mSerial, ImsRadioUtils.callTypeToHal(callType),
            IpPresentation.IP_PRESENTATION_NUM_DEFAULT, mode);
            Log.i(this, "rtt mode to HAL: " + mode);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void deflectCall(int index, String number, Message result) {
        logv("deflect call to: " + number + "connid:" + index);
        final int msgId = ImsQmiIF.REQUEST_DEFLECT_CALL;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        DeflectRequestInfo deflectRequestInfo = new DeflectRequestInfo();
        deflectRequestInfo.connIndex = index;
        deflectRequestInfo.number = number;
        try {
            logSolicitedRequest(rr);
            mImsRadio.deflectCall(rr.mSerial, deflectRequestInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }


    public synchronized void sendGeolocationInfo(double lat, double lon,
            Address address, Message result) {
        final int msgId = ImsQmiIF.REQUEST_SEND_GEOLOCATION_INFO;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);
        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }
        queueRequest(rr);

        String info;
        StringBuilder sb = new StringBuilder("sendGeolocationInfo [Lat="
                + lat + " | Lon=" + lon + "]");
        AddressInfo addressInfo = new AddressInfo();
        if (address != null) {
            info = address.getLocality();
            if (info != null) {
                addressInfo.city = info;
                sb.append(" Address=" + info);
            }
            info = address.getAdminArea();
            if (info != null) {
                addressInfo.state = info;
                sb.append(" State=" + info);
            }
            info = address.getCountryName();
            if (info != null) {
                addressInfo.country = info;
                sb.append(" Country=" + info);
            }
            info = address.getPostalCode();
            if (info != null) {
                addressInfo.postalCode = info;
                sb.append(" Postal Code=" + info);
            }
            info = address.getCountryCode();
            if (info != null) {
                addressInfo.countryCode = info;
                sb.append(" Country Code=" + info);
            }
        } else {
            sb.append(" Null Address!");
        }
        logv(sb.toString());

        try {
            logSolicitedRequest(rr);
            mImsRadio.sendGeolocationInfo(rr.mSerial, lat, lon, addressInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "to IImsRadio: Exception: " + ex);
        }
    }

    public void
    hangupConnection(int index, Message result) {
            hangupWithReason(index, null, null, false, Integer.MAX_VALUE, null, result);
    }

    public void
    hangupWithReason(int connectionId, String userUri, String confUri,
            boolean mpty, int failCause, String errorInfo, Message result) {
        final int msgId = ImsQmiIF.REQUEST_HANGUP;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        HangupRequestInfo hangup = ImsRadioUtils.buildHangupRequest(connectionId, userUri,
                confUri, mpty, failCause, errorInfo);
        try {
            logSolicitedRequest(rr);
            mImsRadio.hangup(rr.mSerial, hangup);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void queryServiceStatus(Message result) {
        final int msgId = ImsQmiIF.REQUEST_QUERY_SERVICE_STATUS;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.queryServiceStatus(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, "Serivce status query request to IImsRadio: Exception: " + ex);
        }
    }

    public void setServiceStatus(Message result, int srvType, int network, int enabled,
            int restrictCause) {
        final int msgId = ImsQmiIF.REQUEST_SET_SERVICE_STATUS;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            ServiceStatusInfo serviceStatusInfo = ImsRadioUtils.buildServiceStatusInfo(srvType,
                    network, enabled, restrictCause);
            logSolicitedRequest(rr);
            Log.i(this, msgIdString + " to ImsRadio: token -" + rr.mSerial +
                    " SrvType:" + srvType + " Network:" + network + " enabled:" + enabled +
                    " RestrictCause:" + restrictCause);
            mImsRadio.setServiceStatus(rr.mSerial, serviceStatusInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, "SetServiceStatus request to IImsRadio: Exception: " + ex);
        }
    }

    public void getImsRegistrationState(Message result) {
        final int msgId = ImsQmiIF.REQUEST_IMS_REGISTRATION_STATE;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getImsRegistrationState(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void sendImsRegistrationState(int imsRegState, Message result) {
        final int msgId = ImsQmiIF.REQUEST_IMS_REG_STATE_CHANGE;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            Log.i(this, msgIdString + " request to ImsRadio - token:" + rr.mSerial + " RegState" +
                    imsRegState);
            mImsRadio.requestRegistrationChange(rr.mSerial,
                    ImsRadioUtils.regStateToHal(imsRegState));
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
   }

    public void modifyCallInitiate(Message result, CallModify callModify) {
        logv("modifyCallInitiate callModify= " + callModify);
        final int msgId = ImsQmiIF.REQUEST_MODIFY_CALL_INITIATE;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.modifyCallInitiate(rr.mSerial,
                    ImsRadioUtils.buildCallModifyInfo(callModify));
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void cancelModifyCall(Message result, int callId) {
        logv("cancelModifyCall");
        final int msgId = ImsQmiIF.REQUEST_CANCEL_MODIFY_CALL;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            Log.i(this, msgIdString + " request to ImsRadio - " + rr.mSerial + " callId:" +
                    callId);
            mImsRadio.cancelModifyCall(rr.mSerial, callId);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void modifyCallConfirm(Message result, CallModify callModify) {
        logv("modifyCallConfirm callModify= " + callModify);
        final int msgId = ImsQmiIF.REQUEST_MODIFY_CALL_CONFIRM;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.modifyCallConfirm(rr.mSerial, ImsRadioUtils.buildCallModifyInfo(callModify));
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void hold(Message result, int callId) {
        final int msgId = ImsQmiIF.REQUEST_HOLD;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            Log.i(this, msgIdString + " request to ImsRadio - " + rr.mSerial + " callId:" +
                    callId);
            mImsRadio.hold(rr.mSerial, callId);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void resume(Message result, int callId) {
        final int msgId = ImsQmiIF.REQUEST_RESUME;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            Log.i(this, msgIdString + " request to ImsRadio - " + rr.mSerial + " callId:" +
                    callId);
            mImsRadio.resume(rr.mSerial, callId);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void conference(Message result) {
        final int msgId = ImsQmiIF.REQUEST_CONFERENCE;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.conference(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void explicitCallTransfer(int srcCallId, int type, String number,
            int destCallId, Message result) {
        logv("explicitCallTransfer srcCallId= " + srcCallId + " type= "+ type + " number= "+
                number + "destCallId = " + destCallId);
        final int msgId = ImsQmiIF.REQUEST_EXPLICIT_CALL_TRANSFER;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        ExplicitCallTransferInfo ectInfo = ImsRadioUtils.buildExplicitCallTransferInfo(srcCallId,
                type, number, destCallId);
        try {
            logSolicitedRequest(rr);
            mImsRadio.explicitCallTransfer(rr.mSerial, ectInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void sendConfigRequest(int requestType, int item, boolean boolValue,
        int intValue, String strValue, int errorCause, Message result) {
        final String msgIdString = msgIdToString(requestType);
        IFRequest rr = IFRequest.obtain(requestType, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            ConfigInfo configInfo = ImsRadioUtils.buildConfigInfo(item, boolValue,
                    intValue, strValue, errorCause);
            logSolicitedRequest(rr);
            Log.i(this, msgIdString + " request to ImsRadio: token -" + rr.mSerial +
                    " item:" + item + " boolValue:" + boolValue + " intValue:" + intValue +
                    " strValue:" + strValue + " errorCause:" + errorCause);
            if (requestType == ImsQmiIF.REQUEST_GET_IMS_CONFIG) {
                mImsRadio.getConfig(rr.mSerial, configInfo);
            } else {
                mImsRadio.setConfig(rr.mSerial, configInfo);
            }
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + " request to IImsRadio: Exception: " + ex);
        }
    }

    public void sendDtmf(char c, Message result) {
        final int msgId = ImsQmiIF.REQUEST_DTMF;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        DtmfInfo dtmfValue = new DtmfInfo();
        dtmfValue.dtmf = Character.toString(c);
        try {
            logSolicitedRequest(rr);
            mImsRadio.sendDtmf(rr.mSerial, dtmfValue);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "to IImsRadio: Exception: " + ex);
        }
    }

    public void startDtmf(char c, Message result) {
        final int msgId = ImsQmiIF.REQUEST_DTMF_START;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        DtmfInfo dtmfValue = new DtmfInfo();
        dtmfValue.dtmf = Character.toString(c);
        try {
            logSolicitedRequest(rr);
            mImsRadio.startDtmf(rr.mSerial, dtmfValue);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdToString(msgId) + "to IImsRadio: Exception: " + ex);
        }
    }

    public void stopDtmf(Message result) {
        final int msgId = ImsQmiIF.REQUEST_DTMF_STOP;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.stopDtmf(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdToString(msgId) + "to IImsRadio: Exception: " + ex);
        }
    }

    private Object processConfigResponse(ConfigInfo configInfo) {
         if (configInfo.hasBoolValue) {
             return configInfo.boolValue;
         } else if (configInfo.intValue != Integer.MAX_VALUE) {
             return configInfo.intValue;
         } else if (configInfo.stringValue != null) {
             return configInfo.stringValue;
         } else if (configInfo.errorCause != ConfigFailureCause.CONFIG_FAILURE_INVALID) {
             return ImsRadioUtils.configFailureCauseFromHal(configInfo.errorCause);
         }
         return ImsRadioUtils.configInfoFromHal(configInfo);
    }

    private void disableSrvStatus() {
        Log.v(this, "disableSrvStatus");
        if (mSrvStatusRegistrations != null) {
            ImsRilException ex = new ImsRilException(ImsQmiIF.E_RADIO_NOT_AVAILABLE, null);
            mSrvStatusRegistrations
                    .notifyRegistrants(new AsyncResult(null, null, ex));
        }
    }

    private void unpackAccTechStatus(ImsQmiIF.Info info, ServiceStatus srvSt) {
        int numAccessTechUpdate = 0;

        numAccessTechUpdate = info.getAccTechStatusCount();

        srvSt.accessTechStatus = new ServiceStatus.
                StatusForAccessTech[numAccessTechUpdate];
        for (int j = 0; j < numAccessTechUpdate; j++) {
            ImsQmiIF.StatusForAccessTech update = info.getAccTechStatus(j);
            srvSt.accessTechStatus[j] = new ServiceStatus.
                    StatusForAccessTech();
            srvSt.accessTechStatus[j].networkMode = update.getNetworkMode();
            srvSt.accessTechStatus[j].status = update.getStatus();
            srvSt.accessTechStatus[j].restrictCause = update.getRestrictionCause();
            if (update.getRegistered() != null) { // Registered is
                                                  // optional field
                srvSt.accessTechStatus[j].registered = update.getRegistered()
                        .getState();
            } else {
                srvSt.accessTechStatus[j].registered = ImsQmiIF.Registration.
                        NOT_REGISTERED;
                            Log.e(this, "Registered not sent");
            }
            log(" networkMode = " + srvSt.accessTechStatus[j].networkMode +
                    " status = " + srvSt.accessTechStatus[j].status +
                    " restrictCause = " + srvSt.accessTechStatus[j].restrictCause +
                    " registered = " + srvSt.accessTechStatus[j].registered);
       }
    }

    private ServiceStatus[] copySrvStatusList(ServiceStatus[] toList,
            ImsQmiIF.SrvStatusList fromList) {
        if (fromList != null) {
            toList = new ServiceStatus[fromList.getSrvStatusInfoCount()];
            Log.v(this, "Num of SrvUpdates = " + fromList.getSrvStatusInfoCount());
            for (int i = 0; i < fromList.getSrvStatusInfoCount(); i++) {
                ImsQmiIF.Info info = fromList.getSrvStatusInfo(i);
                if (info != null && toList != null) {
                    toList[i] = new ServiceStatus();
                    toList[i].isValid = info.getIsValid();
                    toList[i].type = info.getCallType();
                    if (info.getAccTechStatusCount() >= 1) {
                        unpackAccTechStatus(info, toList[i]);
                    } else {
                        toList[i].accessTechStatus = new ServiceStatus.StatusForAccessTech[1];
                        toList[i].accessTechStatus[0] = new ServiceStatus.StatusForAccessTech();
                        ServiceStatus.StatusForAccessTech act = toList[i].accessTechStatus[0];
                        act.networkMode = ImsQmiIF.RADIO_TECH_LTE;
                        act.status = info.getStatus();
                        act.restrictCause = info.getRestrictCause();
                    }
                    if (info.getStatus() == ImsQmiIF.STATUS_ENABLED &&
                            info.getRestrictCause() != CallDetails.CALL_RESTRICT_CAUSE_NONE) {
                        Log.v(this, "Partially Enabled Status due to Restrict Cause");
                        toList[i].status = ImsQmiIF.STATUS_PARTIALLY_ENABLED;
                    } else {
                        toList[i].status = info.getStatus();
                    }
                    if (info.getUserdata().size() > 0) {
                        toList[i].userdata = new byte[info.getUserdata().size()];
                        info.getUserdata().copyTo(toList[i].userdata, 0);
                    }
                    toList[i].rttMode = info.getRttMode();
                    Log.v(this, "RTT: copySrvStatusList rtt mode = " + info.getRttMode());
                } else {
                    Log.e(this, "Null service status in list");
                }
            }
        }
        return toList;
    }

    public void setSuppServiceNotifications(boolean enable, Message result) {
        logv("setSuppServiceNotifications enable = " + enable);
        final int msgId = ImsQmiIF.REQUEST_SET_SUPP_SVC_NOTIFICATION;
        final String msgIdString = msgIdToString(msgId);

        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            int status = enable ? ServiceClassStatus.ENABLED : ServiceClassStatus.DISABLED;
            logSolicitedRequest(rr);
            mImsRadio.setSuppServiceNotification(rr.mSerial, status);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void getCLIR(Message result) {
        logv("getCLIR");
        final int msgId = ImsQmiIF.REQUEST_GET_CLIR;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getClir(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, "GetClir request to IImsRadio: Exception: " + ex);
        }
    }

    public void setCLIR(int clirMode, Message result) {
        logv("setCLIR clirmode = " + clirMode);
        final int msgId = ImsQmiIF.REQUEST_SET_CLIR;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        ClirInfo clirValue = new ClirInfo();
        clirValue.paramN = clirMode;
        try {
            logSolicitedRequest(rr);
            mImsRadio.setClir(rr.mSerial, clirValue);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdToString(msgId) + "to IImsRadio: Exception: " + ex);
        }
    }

    public void queryCallWaiting(int serviceClass, Message response) {
        logv("queryCallWaiting serviceClass = " + serviceClass);
        final int msgId = ImsQmiIF.REQUEST_QUERY_CALL_WAITING;
        final String msgIdString = msgIdToString(msgId);

        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getCallWaiting(rr.mSerial, serviceClass);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void setCallWaiting(boolean enable, int serviceClass,
            Message response) {
        logv("setCallWaiting enable = " + enable + "serviceClass = "
                + serviceClass);
        final int msgId = ImsQmiIF.REQUEST_SET_CALL_WAITING;
        final String msgIdString = msgIdToString(msgId);

        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            int serviceStatus = enable ? ServiceClassStatus.ENABLED : ServiceClassStatus.DISABLED;
            logSolicitedRequest(rr);
            mImsRadio.setCallWaiting(rr.mSerial, serviceStatus, serviceClass);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void queryIncomingCallBarring(String facility, int serviceClass, Message response) {
        suppSvcStatus(ImsQmiIF.QUERY, facilityStringToInt(facility), null, serviceClass, response);
    }

    public void setIncomingCallBarring(int operation, String facility, String[] icbNum,
            int serviceClass, Message response) {
        suppSvcStatus(operation, facilityStringToInt(facility), icbNum, serviceClass, response);
    }

    public void setCallForward(int action, int cfReason, int serviceClass,
            String number, int timeSeconds, Message response) {
        logv("setCallForward cfReason= " + cfReason + " serviceClass = "
                + serviceClass + "number = " + number + "timeSeconds = "
                + timeSeconds);
        setCallForwardInternal(Integer.MAX_VALUE, Integer.MAX_VALUE, Integer.MAX_VALUE,
                Integer.MAX_VALUE, action, cfReason, serviceClass, number, timeSeconds, response);
    }

    public void setCallForwardUncondTimer(int startHour, int startMinute, int endHour,
            int endMinute, int action, int cfReason, int serviceClass, String number,
            Message response) {
        setCallForwardInternal(startHour, startMinute, endHour, endMinute, action, cfReason,
                serviceClass, number, ZERO_SECONDS, response);
    }

    private void setCallForwardInternal(int startHour, int startMinute, int endHour,
            int endMinute, int action, int cfReason, int serviceClass, String number,
            int timeSeconds, Message response) {
        logv("setCallForwardInternal cfReason= " + cfReason + " serviceClass = " +
                serviceClass + "number = " + number + "startHour = " + startHour +
                "startMinute = " + startMinute + "endHour = " + endHour + "endMin = " +
                endMinute);
        final int msgId = ImsQmiIF.REQUEST_SET_CALL_FORWARD_STATUS;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        vendor.qti.hardware.radio.ims.V1_0.CallForwardInfo cfInfo =
                ImsRadioUtils.buildCallForwardInfo(cfReason, serviceClass, number,
                action, timeSeconds);
        if (startHour != Integer.MAX_VALUE && startMinute != Integer.MAX_VALUE) {
            cfInfo.hasCallFwdTimerStart = true;
            ImsRadioUtils.buildCallFwdTimerInfo(cfInfo.callFwdTimerStart, startHour, startMinute);
        }

        if (endHour != Integer.MAX_VALUE && endMinute != Integer.MAX_VALUE) {
            cfInfo.hasCallFwdTimerEnd = true;
            ImsRadioUtils.buildCallFwdTimerInfo(cfInfo.callFwdTimerEnd, endHour, endMinute);
        }
        try {
            logSolicitedRequest(rr);
            mImsRadio.setCallForwardStatus(rr.mSerial, cfInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "to IImsRadio: Exception: " + ex);
        }
    }

    public void queryCallForwardStatus(int cfReason, int serviceClass,
            String number, Message response) {
        logv("queryCallForwardStatus cfReason= " + cfReason
                + " serviceClass = " + serviceClass + "number = " + number);
        final int msgId = ImsQmiIF.REQUEST_QUERY_CALL_FORWARD_STATUS;;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        vendor.qti.hardware.radio.ims.V1_0.CallForwardInfo cfInfo =
                ImsRadioUtils.buildCallForwardInfo(cfReason, serviceClass, number,
                STATUS_INTERROGATE, 0);
        try {
            logSolicitedRequest(rr);
            mImsRadio.queryCallForwardStatus(rr.mSerial, cfInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "to IImsRadio: Exception: " + ex);
        }
    }

    public void queryCLIP(Message response) {
        logv("queryClip");
        final int msgId = ImsQmiIF.REQUEST_QUERY_CLIP;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getClip(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, "GetClip request to IImsRadio: Exception: " + ex);
        }
    }

    public void setUiTTYMode(int uiTtyMode, Message response) {
        logv("setUiTTYMode uittyMode=" + uiTtyMode);
        final int msgId = ImsQmiIF.REQUEST_SEND_UI_TTY_MODE;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        TtyInfo info = new TtyInfo();
        info.mode = ImsRadioUtils.ttyModeToHal(uiTtyMode);
        try {
            logSolicitedRequest(rr);
            mImsRadio.setUiTtyMode(rr.mSerial, info);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdToString(msgId) + "to IImsRadio: Exception: " + ex);
        }
    }

    public void exitEmergencyCallbackMode(Message response) {
        logv("exitEmergencyCallbackMode");
        final int msgId = ImsQmiIF.REQUEST_EXIT_EMERGENCY_CALLBACK_MODE;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.exitEmergencyCallbackMode(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    @Override
    public void queryFacilityLock(String facility, String password,
            int serviceClass, Message response) {
        suppSvcStatus(ImsQmiIF.QUERY, facilityStringToInt(facility), response);
    }

    @Override
    public void setFacilityLock(String facility, boolean lockState,
            String password, int serviceClass, Message response) {
        int operation = lockState ? ImsQmiIF.ACTIVATE : ImsQmiIF.DEACTIVATE;
        suppSvcStatus(operation, facilityStringToInt(facility), response);
    }

    public void getSuppSvc(String facility, Message response) {
        suppSvcStatus(ImsQmiIF.QUERY, facilityStringToInt(facility), response);
    }

    public void setSuppSvc(String facility, boolean lockState, Message response) {
        int operation = lockState ? ImsQmiIF.ACTIVATE : ImsQmiIF.DEACTIVATE;
        suppSvcStatus(operation, facilityStringToInt(facility), response);
    }

    public void suppSvcStatus(int operationType, int facility, String[] icbNum,
            int serviceClassValue, Message response) {
        logv("suppSvcStatus operationType = " + operationType + " facility = "
                + facility + "serviceClassValue = " + serviceClassValue);
        suppSvcStatusInternal(operationType, facility, icbNum, serviceClassValue, response);
    }

    public void suppSvcStatus(int operationType, int facility, Message response) {
        logv("suppSvcStatus operationType = " + operationType + " facility = "
                + facility);
        suppSvcStatusInternal(operationType, facility, null, Integer.MAX_VALUE, response);
    }

    private void suppSvcStatusInternal(int operationType, int facility, String[] inCbNumList,
            int serviceClass, Message response) {
        final int msgId = ImsQmiIF.REQUEST_SUPP_SVC_STATUS;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        CbNumListInfo cbNumListInfo = ImsRadioUtils.buildCbNumListInfo(inCbNumList, serviceClass);
        try {
            logSolicitedRequest(rr);
            mImsRadio.suppServiceStatus(rr.mSerial, operationType,
                    ImsRadioUtils.facilityTypeToHal(facility), cbNumListInfo);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "to IImsRadio: Exception: " + ex);
        }
    }

    public void getCOLR(Message result) {
        logv("getCOLR");
        final int msgId = ImsQmiIF.REQUEST_GET_COLR;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getColr(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, "GetColr request to IImsRadio: Exception: " + ex);
        }
    }

    public void setCOLR(int presentationValue, Message result) {
        logv("setCOLR presentationValue = " + presentationValue);
        final int msgId = ImsQmiIF.REQUEST_SET_COLR;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, result);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        ColrInfo colrValue = new ColrInfo();
        colrValue.presentation = ImsRadioUtils.ipPresentationToHal(presentationValue);
        try {
            logSolicitedRequest(rr);
            mImsRadio.setColr(rr.mSerial, colrValue);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdToString(msgId) + "to IImsRadio: Exception: " + ex);
        }
    }

    static int facilityStringToInt(String sc) {
        if (sc == null) {
            throw new RuntimeException ("invalid supplementary service");
        }

        if (sc.equals("CLIP")) {
            return ImsQmiIF.FACILITY_CLIP;
        }
        else if (sc.equals("COLP")) {
            return ImsQmiIF.FACILITY_COLP;
        }
        return 0;
    }

    public void getPacketCount(Message response) {
        logv("getPacketCount");
        final int msgId = ImsQmiIF.REQUEST_GET_RTP_STATISTICS;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getRtpStatistics(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void getPacketErrorCount(Message response) {
        logv("getPacketErrorCount");
        final int msgId = ImsQmiIF.REQUEST_GET_RTP_ERROR_STATISTICS;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getRtpErrorStatistics(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void getImsSubConfig(Message response) {
        logv("getImsSubConfig");
        final int msgId = ImsQmiIF.REQUEST_GET_IMS_SUB_CONFIG;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.getImsSubConfig(rr.mSerial);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdString + "request to IImsRadio: Exception: " + ex);
        }
    }

    public void getWifiCallingPreference(Message response) {
        Log.i(this, "getWifiCallingPreference : Not supported");
    }

    public void setWifiCallingPreference(int wifiCallingStatus, int wifiCallingPreference,
            Message response) {
        Log.i(this, "setWifiCallingPreference : Not supported");
    }

    public void getHandoverConfig(Message response) {
        Log.i(LOG_TAG, "getHandoverConfig : Not supported");
    }

    public void setHandoverConfig(int hoConfig, Message response) {
        Log.i(LOG_TAG, "setHandoverConfig : Not supported");
    }

    public void queryVopsStatus(Message response) {
        Log.i(this, "queryVopsStatus : Not supported");
    }

    public void querySsacStatus(Message response) {
        Log.i(this, "querySsacStatus : Not supported");
    }

    public void updateVoltePref(int preference, Message response) {
        Log.i(this, "updateVoltePref : Not supported");
    }

    public void queryVoltePref(Message response) {
        Log.i(this, "queryVoltePref : Not supported");
    }

    /* ImsPhoneCommandsInterface API's */
    public void rejectCall(Message result) {
        Log.i(this, "rejectCall : Not supported");
    }

    public void
    getLastCallFailCause(Message result) {
        Log.i(this, "getLastCallFailCause : Not supported");
    }

    public void hangupWaitingOrBackground(Message result) {
        Log.i(this, "hangupWaitingOrBackground : Not supported");
    }

    public void getCurrentCalls(Message result) {
        Log.i(this, "getCurrentCalls : Not supported");
    }

    public void switchWaitingOrHoldingAndActive(Message result) {
        Log.i(this, "switchWaitingOrHoldingAndActive : Not supported");
    }

    public void explicitCallTransfer(Message result) {
        Log.i(this, "explicitCallTransfer : Not supported");
    }

    public void hangupForegroundResumeBackground(Message result) {
        Log.i(this, "hangupForegroundResumeBackground : Not supported");
    }


    public void registerForRttMessage(Handler h, int what, Object obj) {
        mRttMessageRegistrants.addUnique(h, what, obj);
    }

    public void deregisterForRttMessage(Handler h) {
        mRttMessageRegistrants.remove(h);
    }

    public void sendRttMessage(String message, Message response) {
        Log.i(this, "RTT: sendRttMessage msg = " + message);
        final int msgId = ImsQmiIF.REQUEST_SEND_RTT_MSG;
        final String msgIdString = msgIdToString(msgId);
        IFRequest rr = IFRequest.obtain(msgId, response);

        if (sendErrorOnImsRadioDown(rr, msgIdString)) {
            return;
        }

        queueRequest(rr);
        try {
            logSolicitedRequest(rr);
            mImsRadio.sendRttMessage(rr.mSerial, message);
        } catch (Exception ex) {
            removeFromQueueAndSendResponse(rr.mSerial, ImsQmiIF.E_GENERIC_FAILURE);
            Log.e(this, msgIdToString(msgId) + "to IImsRadio: Exception: " + ex);
        }
    }
}
