/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
 * Not a Contribution, Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2015 The Android Open Source Project
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

package com.qualcomm.qti.internal.telephony;

import static android.telephony.SubscriptionManager.INVALID_PHONE_INDEX;
import static android.telephony.SubscriptionManager.INVALID_SUBSCRIPTION_ID;
import static com.android.internal.telephony.DctConstants.APN_IMS_ID;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkCapabilities;
import android.net.NetworkFactory;
import android.net.NetworkRequest;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Registrant;
import android.os.RegistrantList;
import android.os.SystemProperties;
import android.telephony.Rlog;
import android.telephony.SubscriptionManager;
import android.telephony.ServiceState;
import android.text.TextUtils;
import android.util.LocalLog;
import android.util.Pair;


import com.android.internal.telephony.CallManager;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.dataconnection.DcRequest;
import com.android.internal.telephony.ITelephonyRegistry;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneSwitcher;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.uicc.IccCardStatus;

import com.qualcomm.qti.internal.telephony.QtiRilInterface;
import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner;
import com.qualcomm.qcrilhook.QcRilHook;

import java.lang.Integer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import android.net.StringNetworkSpecifier;
import android.net.NetworkSpecifier;

public class QtiPhoneSwitcher extends PhoneSwitcher {

    private final int MAX_CONNECT_FAILURE_COUNT = 5;
    private final int[] mRetryArray =  new int []{5,10,20,40,60};
    private int[] mAllowDataFailure;
    private CallManager mCm;
    private boolean mManualDdsSwitch = false;
    private boolean mSendDdsSwitchDoneIntent = false;
    private int mDefaultDataPhoneId = -1;
    private QtiRilInterface mQtiRilInterface;
    private String [] mSimStates;
    private List<Integer> mNewActivePhones;
    private boolean mWaitForDetachResponse = false;
    private QtiDdsSwitchController mQtiDdsSwitchController;
    public static boolean isLplusLSupported = false;
    private final int EVENT_ALLOW_DATA_FALSE_RESPONSE   = 112;
    private final int EVENT_ALLOW_DATA_TRUE_RESPONSE   = 113;
    private final int USER_INITIATED_SWITCH = 0;
    private final int NONUSER_INITIATED_SWITCH = 1;
    private final String PROPERTY_TEMP_DDSSWITCH = "persist.radio.enable_temp_dds";

    public QtiPhoneSwitcher(int maxActivePhones, int numPhones, Context context,
            SubscriptionController subscriptionController, Looper looper, ITelephonyRegistry tr,
            CommandsInterface[] cis, Phone[] phones) {
        super (maxActivePhones, numPhones, context, subscriptionController, looper,
                tr, cis, phones);
        mAllowDataFailure = new int[numPhones];
        mSimStates = new String[numPhones];
        mCm = CallManager.getInstance();
        mCm.registerForDisconnect(this, EVENT_VOICE_CALL_ENDED, null);
        mQtiRilInterface = QtiRilInterface.getInstance(context);
        if (mQtiRilInterface.isServiceReady()) {
            queryMaxDataAllowed();
            isLplusLSupported = mQtiRilInterface.getLpluslSupportStatus();
        } else {
            mQtiRilInterface.registerForServiceReadyEvent(this, EVENT_OEM_HOOK_SERVICE_READY, null);
        }
        mQtiRilInterface.registerForUnsol(this, EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED, null);
        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        mContext.registerReceiver(mSimStateIntentReceiver, filter);
        mQtiDdsSwitchController = new QtiDdsSwitchController(numPhones, context,
                subscriptionController, looper, phones, isLplusLSupported);
    }

    private BroadcastReceiver mSimStateIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action.equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                String value = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                        SubscriptionManager.INVALID_PHONE_INDEX);
                log("mSimStateIntentReceiver: phoneId = " + phoneId+" value = "+value);
                if (phoneId != SubscriptionManager.INVALID_PHONE_INDEX) {
                    mSimStates[phoneId] = value;
                }

                if (isSimReady(phoneId) && (getConnectFailureCount(phoneId) > 0)) {
                    resendDataAllowed(phoneId);
                }
            }
        }
    };

    private void queryMaxDataAllowed() {
        mMaxActivePhones = mQtiRilInterface.getMaxDataAllowed();
    }

    private void handleUnsolMaxDataAllowedChange(Message msg) {
        if (msg == null ||  msg.obj == null) {
            log("Null data received in handleUnsolMaxDataAllowedChange");
            return;
        }
        ByteBuffer payload = ByteBuffer.wrap((byte[])msg.obj);
        payload.order(ByteOrder.nativeOrder());
        int rspId = payload.getInt();
        if ((rspId == QcRilHook.QCRILHOOK_UNSOL_MAX_DATA_ALLOWED_CHANGED)) {
            int response_size = payload.getInt();
            if (response_size < 0 ) {
                log("Response size is Invalid " + response_size);
                return;
            }
            mMaxActivePhones = payload.get();
            log(" Unsol Max Data Changed to: " + mMaxActivePhones);
        }
    }

    @Override
    public void handleMessage(Message msg) {
        final int ddsSubId = mSubscriptionController.getDefaultDataSubId();
        final int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);

        log("handle event - " + msg.what);
        AsyncResult ar = null;
        switch (msg.what) {
            case EVENT_DEFAULT_SUBSCRIPTION_CHANGED:
                boolean isLplTempSwitch = SystemProperties.getBoolean(
                        PROPERTY_TEMP_DDSSWITCH, false);
                //Block dds switch request as it would fail in case of active call
                if (isAnyVoiceCallActiveOnDevice() && !isLplTempSwitch) {
                    log("Voice call active. Waiting for call end");
                    return;
                }
                onEvaluate(REQUESTS_UNCHANGED, "defaultChanged");
                break;
            case EVENT_OEM_HOOK_SERVICE_READY:
                ar = (AsyncResult)msg.obj;
                if (ar.result != null) {
                    boolean isServiceReady = (boolean)ar.result;
                    if (isServiceReady) {
                        queryMaxDataAllowed();
                        isLplusLSupported = mQtiRilInterface.getLpluslSupportStatus();
                        mQtiDdsSwitchController.updateLplusLStatus(isLplusLSupported);
                    }
                } else {
                    log("Error: empty result, EVENT_OEM_HOOK_SERVICE_READY");
                }
                break;
            case EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED:
                ar = (AsyncResult)msg.obj;
                if (ar.result != null) {
                    handleUnsolMaxDataAllowedChange((Message)ar.result);
                } else {
                    log("Error: empty result, EVENT_UNSOL_MAX_DATA_ALLOWED_CHANGED");
                }
                break;
            case EVENT_ALLOW_DATA_TRUE_RESPONSE:
                onAllowDataResponse(msg.arg1, (AsyncResult)msg.obj);
                break;
            case EVENT_SUBSCRIPTION_CHANGED:
                broadcastNetworkSpecifier();
                onEvaluate(REQUESTS_UNCHANGED, "subChanged");
                break;
            case EVENT_VOICE_CALL_ENDED:
                log("EVENT_VOICE_CALL_ENDED");
                if (!isAnyVoiceCallActiveOnDevice()) {
                    for (int i = 0; i < mNumPhones; i++) {
                        if ((getConnectFailureCount(i) > 0) &&
                                isPhoneIdValidForRetry(i)) {
                            resendDataAllowed(i);
                            break;
                        }
                    }
                }
                break;
            case EVENT_ALLOW_DATA_FALSE_RESPONSE:
                log("EVENT_ALLOW_DATA_FALSE_RESPONSE");
                mWaitForDetachResponse = false;
                informDdsToRil(ddsSubId);
                for (int phoneId : mNewActivePhones) {
                    activate(phoneId);
                }
                if (mNewActivePhones.contains(ddsPhoneId)) {
                    mManualDdsSwitch = false;
                }
                break;
            default:
                super.handleMessage(msg);
        }
    }

    /* Broadcast the subscription id to connectivityService which in turn
     * adds the relevant sub id to the network request
     */
    private void broadcastNetworkSpecifier() {
        ArrayList<Integer> subIdList = new ArrayList<Integer>();
        for (int i = 0; i < mNumPhones; ++i) {
            int[] subId = mSubscriptionController.getSubId(i);
            if (subId != null && subId.length > 0 &&
                    mSubscriptionController.isActiveSubId(subId[0]) &&
                    isUiccProvisioned(i)) {
                subIdList.add(subId[0]);
            }
        }

        if (subIdList.size() > 0) {
             Intent intent = new Intent(
                     "org.codeaurora.intent.action.ACTION_NETWORK_SPECIFIER_SET");
             intent.putIntegerArrayListExtra("SubIdList", subIdList);
             log("Broadcast network specifier set intent");
             mContext.sendBroadcast(intent);
        }
    }

    private boolean isSimReady(int phoneId) {
        if (phoneId == SubscriptionManager.INVALID_PHONE_INDEX) {
            return false;
        }

        if (IccCardConstants.INTENT_VALUE_ICC_READY.equals(mSimStates[phoneId]) ||
                IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(mSimStates[phoneId]) ||
                IccCardConstants.INTENT_VALUE_ICC_IMSI.equals(mSimStates[phoneId])) {
            log("SIM READY for phoneId: "+phoneId);
            return true;
        } else {
            return false;
        }
    }

    @Override
    protected void onEvaluate(boolean requestsChanged, String reason) {
        StringBuilder sb = new StringBuilder(reason);
        if (isEmergency()) {
            log("onEvalute aborted due to Emergency");
            return;
        }

        boolean diffDetected = requestsChanged;
        final int dataSubId = mSubscriptionController.getDefaultDataSubId();
        final int ddsPhoneId = mSubscriptionController.getPhoneId(dataSubId);
        if ((mSubscriptionController.isActiveSubId(dataSubId) &&
                dataSubId != mDefaultDataSubscription) ||
                (ddsPhoneId != INVALID_PHONE_INDEX &&
                ddsPhoneId != mDefaultDataPhoneId)) {
            sb.append(" default ").append(mDefaultDataSubscription).append("->").append(dataSubId);
            mManualDdsSwitch = true;
            mSendDdsSwitchDoneIntent = true;
            mDefaultDataSubscription = dataSubId;
            mDefaultDataPhoneId = ddsPhoneId;
            //For ims on non dds sub, active is enabled
            //Reset this here to block default requests processing on DDS change
            mPhoneStates[mDefaultDataPhoneId].active = false;
            diffDetected = true;
        }

        for (int i = 0; i < mNumPhones; i++) {
            int sub = mSubscriptionController.getSubIdUsingPhoneId(i);
            if (sub != mPhoneSubscriptions[i]) {
                sb.append(" phone[").append(i).append("] ").append(mPhoneSubscriptions[i]);
                sb.append("->").append(sub);
                mPhoneSubscriptions[i] = sub;
                diffDetected = true;
            }
        }

        if (diffDetected) {
            log("evaluating due to " + sb.toString());

            List<Integer> newActivePhones = new ArrayList<Integer>();

            for (DcRequest dcRequest : mPrioritizedDcRequests) {
                int phoneIdForRequest = phoneIdForRequest(dcRequest.networkRequest,
                        dcRequest.apnId);
                if (phoneIdForRequest == INVALID_PHONE_INDEX) continue;
                if (newActivePhones.contains(phoneIdForRequest)) continue;
                newActivePhones.add(phoneIdForRequest);
                if (newActivePhones.size() >= mMaxActivePhones) break;
            }

            if (VDBG) {
                log("default subId = " + mDefaultDataSubscription);
                for (int i = 0; i < mNumPhones; i++) {
                    log(" phone[" + i + "] using sub[" + mPhoneSubscriptions[i] + "]");
                }
                log(" newActivePhones:");
                for (Integer i : newActivePhones) log("  " + i);
            }

            mNewActivePhones = newActivePhones;
            for (int phoneId = 0; (phoneId < mNumPhones); phoneId++) {
                if (newActivePhones.contains(phoneId) == false) {
                    deactivate(phoneId);
                }
            }
            if (!mWaitForDetachResponse) {
                informDdsToRil(dataSubId);
                // only activate phones up to the limit
                for (int phoneId : newActivePhones) {
                    activate(phoneId);
                }
                if (mNewActivePhones.contains(ddsPhoneId)) {
                    mManualDdsSwitch = false;
                }
            }
        }
    }

    /* Determine the phone id on which PS attach needs to be done
     */
    protected int phoneIdForRequest(NetworkRequest netRequest, int apnid) {
        String specifier = null;
        int subId;
        NetworkSpecifier networkSpecifierObj = netRequest.networkCapabilities.getNetworkSpecifier();
        if(networkSpecifierObj != null && networkSpecifierObj instanceof StringNetworkSpecifier) {
        specifier = ((StringNetworkSpecifier) networkSpecifierObj).specifier;
        }

        if (TextUtils.isEmpty(specifier)) {
            subId = mDefaultDataSubscription;
        } else {
            if ((APN_IMS_ID == apnid) && mManualDdsSwitch) {
                subId = mDefaultDataSubscription;
            } else {
                try {
                    subId = Integer.parseInt(specifier);
                } catch (NumberFormatException e) {
                    Rlog.e(LOG_TAG, "NumberFormatException on " + specifier);
                    subId = INVALID_SUBSCRIPTION_ID;
                }
            }
        }
        int phoneId = INVALID_PHONE_INDEX;
        if (subId == INVALID_SUBSCRIPTION_ID) return phoneId;

        for (int i = 0 ; i < mNumPhones; i++) {
            if (mPhoneSubscriptions[i] == subId) {
                phoneId = i;
                break;
            }
        }
        return phoneId;
    }

    private boolean isUiccProvisioned(int phoneId) {
        QtiUiccCardProvisioner uiccProvisioner = QtiUiccCardProvisioner.getInstance();
        boolean status = (uiccProvisioner.getCurrentUiccCardProvisioningStatus(phoneId) > 0)?
                true : false;
        log("isUiccProvisioned = " + status);

        return status;
    }

    @Override
    protected void deactivate(int phoneId) {
        PhoneState state = mPhoneStates[phoneId];
        if (state.active == false) {
            return;
        }
        state.active = false;
        log("deactivate " + phoneId);
        state.lastRequested = System.currentTimeMillis();
        if (mSubscriptionController.isActiveSubId(mPhoneSubscriptions[phoneId])) {
            mCommandsInterfaces[phoneId].setDataAllowed(false,
                    obtainMessage(EVENT_ALLOW_DATA_FALSE_RESPONSE));
            mWaitForDetachResponse = true;
        }
        mActivePhoneRegistrants[phoneId].notifyRegistrants();
    }

    @Override
    protected void activate(int phoneId) {
        PhoneState state = mPhoneStates[phoneId];
        if ((state.active == true) && !mManualDdsSwitch &&
                (getConnectFailureCount(phoneId) == 0)) return;
        state.active = true;
        log("activate " + phoneId);
        state.lastRequested = System.currentTimeMillis();
        mCommandsInterfaces[phoneId].setDataAllowed(true,
                obtainMessage(EVENT_ALLOW_DATA_TRUE_RESPONSE, phoneId, 0));
    }

    @Override
    protected void onResendDataAllowed(Message msg) {
        final int phoneId = msg.arg1;
        mCommandsInterfaces[phoneId].setDataAllowed(mPhoneStates[phoneId].active,
                obtainMessage(EVENT_ALLOW_DATA_TRUE_RESPONSE, phoneId, 0));
    }

    private void resetConnectFailureCount(int phoneId) {
        mAllowDataFailure[phoneId] = 0;
    }

    private void incConnectFailureCount(int phoneId) {
        mAllowDataFailure[phoneId]++;
    }

    private int getConnectFailureCount(int phoneId) {
        return mAllowDataFailure[phoneId];
    }

    private void handleConnectMaxFailure(int phoneId) {
        resetConnectFailureCount(phoneId);
        int ddsSubId = mSubscriptionController.getDefaultDataSubId();
        int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);
        if ((ddsPhoneId > 0 && ddsPhoneId < mNumPhones) &&
                phoneId != ddsPhoneId) {
            log("ALLOW_DATA retries exhausted on phoneId = " + phoneId);
            enforceDds(ddsPhoneId);
        }
    }

    private void enforceDds(int phoneId) {
        int[] subId = mSubscriptionController.getSubId(phoneId);
        log("enforceDds: subId = " + subId[0]);
        mSubscriptionController.setDefaultDataSubId(subId[0]);
    }

    private boolean isAnyVoiceCallActiveOnDevice() {
        boolean ret = (mCm.getState() != PhoneConstants.State.IDLE);
        log("isAnyVoiceCallActiveOnDevice: " + ret);
        return ret;
    }

    private void onAllowDataResponse(int phoneId, AsyncResult ar) {
        if (ar.exception != null) {
            incConnectFailureCount(phoneId);
            log("Allow_data failed on phoneId = " + phoneId + ", failureCount = "
                    + getConnectFailureCount(phoneId));

            if (isAnyVoiceCallActiveOnDevice()) {
                /* Any DDS retry while voice call is active is in vain
                   Wait for call to get disconnected */
                log("Wait for call end indication");
                return;
            }

            if (!isSimReady(phoneId)) {
                /* If there is a attach failure due to sim not ready then
                hold the retry until sim gets ready */
                log("Wait for SIM to get READY");
                return;
            }

            int allowDataFailureCount = getConnectFailureCount(phoneId);
            if (allowDataFailureCount > MAX_CONNECT_FAILURE_COUNT) {
                handleConnectMaxFailure(phoneId);
            } else {
                int retryDelay = mRetryArray[allowDataFailureCount - 1] * 1000;
                log("Scheduling retry connect/allow_data after: " + retryDelay);
                postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        log("Running retry connect/allow_data");
                        if (isPhoneIdValidForRetry(phoneId)) {
                            resendDataAllowed(phoneId);
                        } else {
                            log("Abandon Retry");
                            resetConnectFailureCount(phoneId);
                        }
                    }}, retryDelay);
                }
        } else {
            log("Allow_data success on phoneId = " + phoneId);
            if (mSendDdsSwitchDoneIntent) {
                mSendDdsSwitchDoneIntent = false;
                Intent intent = new Intent(
                        "org.codeaurora.intent.action.ACTION_DDS_SWITCH_DONE");
                intent.putExtra(PhoneConstants.SUBSCRIPTION_KEY,
                        mSubscriptionController.getDefaultDataSubId());
                intent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
                log("Broadcast dds switch done intent");
                mContext.sendBroadcast(intent);
            }
            resetConnectFailureCount(phoneId);
            mActivePhoneRegistrants[phoneId].notifyRegistrants();
            if (mQtiDdsSwitchController.isTempDdsSwitchRequired()) {
                mQtiDdsSwitchController.resetTempDdsSwitchRequired();
            }
        }
    }

    private boolean isPhoneIdValidForRetry(int phoneId) {
        boolean isValid = false;
        int phoneIdForRequest = INVALID_PHONE_INDEX;
        int ddsPhoneId = mSubscriptionController.getPhoneId(
                mSubscriptionController.getDefaultDataSubId());
        if (ddsPhoneId != INVALID_PHONE_INDEX && ddsPhoneId == phoneId) {
            isValid = true;
        } else {
            if (mPrioritizedDcRequests.size() > 0) {
                for (int i = 0; i < mMaxActivePhones; i++) {
                    DcRequest dcRequest = mPrioritizedDcRequests.get(i);
                    if (dcRequest != null) {
                        phoneIdForRequest = phoneIdForRequest(dcRequest.networkRequest,
                                dcRequest.apnId);
                        if (phoneIdForRequest == phoneId) {
                            isValid = true;
                        }
                    }
                }
            }
        }
        return isValid;
    }

    private void informDdsToRil(int ddsSubId) {
        int ddsPhoneId = mSubscriptionController.getPhoneId(ddsSubId);

        if (!mQtiRilInterface.isServiceReady()) {
            log("Oem hook service is not ready yet");
            return;
        }

        for (int i = 0; i < mNumPhones; i++) {
            log("InformDdsToRil rild= " + i + ", DDS=" + ddsPhoneId);
            if (mQtiDdsSwitchController.isTempDdsSwitchRequired()) {
                mQtiRilInterface.qcRilSendDDSInfo(ddsPhoneId,
                        NONUSER_INITIATED_SWITCH, i);
            } else {
                mQtiRilInterface.qcRilSendDDSInfo(ddsPhoneId,
                        USER_INITIATED_SWITCH, i);
            }
        }
    }

}
