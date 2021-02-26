/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
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
 */


package com.qualcomm.qti.internal.telephony;


import android.content.Context;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.telephony.PhoneNumberUtils;
import android.telephony.Rlog;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.cdma.CdmaSubscriptionSourceManager;
import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneNotifier;
import com.android.internal.telephony.RadioCapability;
import com.android.internal.telephony.TelephonyComponentFactory;

import com.qualcomm.qti.internal.telephony.primarycard.SubsidyLockSettingsObserver;

import com.qualcomm.qti.internal.telephony.QtiRadioCapabilityController;
import com.qualcomm.qti.internal.telephony.QtiSubscriptionInfoUpdater;


public class QtiGsmCdmaPhone extends GsmCdmaPhone {
    private static final String LOG_TAG = "QtiGsmCdmaPhone";
    private static final int PROP_EVENT_START = EVENT_LAST;
    private static final int EVENT_OEM_HOOK_SERVICE_READY = PROP_EVENT_START +1;

    private BaseRilInterface mQtiRilInterface;
    private boolean mIsPhoneReadySent = false;
    private boolean mIsPhoneReadyPending = false;
    private static int READY = 1;
    CdmaSubscriptionSourceManager mCdmaNVSSM;
    int mCdmaNVSubscriptionSource =
            CdmaSubscriptionSourceManager.SUBSCRIPTION_SOURCE_UNKNOWN;
    private static final int mNumPhones = TelephonyManager.getDefault().getPhoneCount();

    public QtiGsmCdmaPhone(Context context,
            CommandsInterface ci, PhoneNotifier notifier, int phoneId,
            int precisePhoneType, TelephonyComponentFactory telephonyComponentFactory) {
        this(context, ci, notifier, false, phoneId, precisePhoneType,
                telephonyComponentFactory);
    }

    public QtiGsmCdmaPhone(Context context,
            CommandsInterface ci, PhoneNotifier notifier, boolean unitTestMode, int phoneId,
            int precisePhoneType, TelephonyComponentFactory telephonyComponentFactory) {
        super(context, ci, notifier, unitTestMode, phoneId, precisePhoneType,
                telephonyComponentFactory);
        Rlog.d(LOG_TAG, "Constructor");
        mQtiRilInterface = getQtiRilInterface();
        mQtiRilInterface.registerForServiceReadyEvent(this, EVENT_OEM_HOOK_SERVICE_READY, null);
        mCdmaNVSSM = CdmaSubscriptionSourceManager.getInstance(context, mCi, null,
                -1, null);
    }

    private void handleCdmaNVSubscriptionSource(int newSubscriptionSource) {
        Rlog.d(LOG_TAG, " mCdmaNVSubscriptionSource:  " + mCdmaNVSubscriptionSource +
                " newSubscriptionSource:  " + newSubscriptionSource);
        if (newSubscriptionSource != mCdmaNVSubscriptionSource) {
            mCdmaNVSubscriptionSource = newSubscriptionSource;
            handleNVChange(newSubscriptionSource);
        }
    }

    private void handleNVChange(int newSubscriptionSource) {
        boolean isNVReady = newSubscriptionSource == CDMA_SUBSCRIPTION_NV;
        Rlog.e(LOG_TAG, " handleNVChanged: isNVReady: " + isNVReady);
        QtiSubscriptionInfoUpdater.getInstance().updateNVRecord(isNVReady, mPhoneId);
    }

    @Override
    public void setPreferredNetworkType(int networkType, Message response) {
        QtiRadioCapabilityController radioCapController =
                QtiRadioCapabilityController.getInstance();
        if (radioCapController != null) {
            radioCapController.setPreferredNetworkType(getPhoneId(), networkType, response);
        } else {
            Rlog.e(LOG_TAG, " Error: Received null QtiRadioCapabilityController instante ");
        }
    }

    private void updatePhoneReady(int phoneId) {
        if (!mIsPhoneReadySent &&
                SystemProperties.getInt("persist.vendor.radio.poweron_opt", 0) == 1) {
            if (!mQtiRilInterface.isServiceReady()) {
                mIsPhoneReadyPending = true;
                return;
            }
            logd("Sending Phone Ready to RIL.");
            mQtiRilInterface.sendPhoneStatus(READY, phoneId);
            mIsPhoneReadySent = true;
            mIsPhoneReadyPending = false;
        }
    }

    @Override
    protected void phoneObjectUpdater(int newVoiceTech) {
        super.phoneObjectUpdater(newVoiceTech);
        updatePhoneReady(mPhoneId);
    }

    @Override
    public void radioCapabilityUpdated(RadioCapability rc) {
        // Called when radios first become available or after a capability switch
        // Update the cached value
        mRadioCapability.set(rc);

        QtiRadioCapabilityController radioCapController =
                QtiRadioCapabilityController.getInstance();
        if (radioCapController != null) {
            // update new radio capabilities to QtiRadioCapabilityController
            // This would be called in below scenarios
            // 1. After completion of flex map.
            // 2. When radio state moved from UNAVAILABLE to AVAILABLE
            // 3. Whenever phone object switch happens.
            radioCapController.radioCapabilityUpdated(getPhoneId(), rc);
        }
    }

    @Override
    public boolean getCallForwardingIndicator() {
        if (!isCurrentSubValid()) {
            return false;
        }
        return super.getCallForwardingIndicator();
    }

    private boolean isCurrentSubValid() {
        int provisionStatus = QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED;
        SubscriptionManager subscriptionManager = SubscriptionManager.from(mContext);
        try {
            provisionStatus = QtiUiccCardProvisioner.getInstance()
                    .getCurrentUiccCardProvisioningStatus(mPhoneId);
        } catch (NullPointerException ex) {
            provisionStatus = QtiUiccCardProvisioner.UiccProvisionStatus.NOT_PROVISIONED;
        }
        Rlog.d(LOG_TAG, "ProvisionStatus: " + provisionStatus + " phone id:" + mPhoneId);
        return subscriptionManager.isActiveSubId(getSubId()) &&
                (provisionStatus == QtiUiccCardProvisioner.UiccProvisionStatus.PROVISIONED);
    }

    public boolean setLocalCallHold(boolean enable) {
        if (!mQtiRilInterface.isServiceReady()) {
            Rlog.e(LOG_TAG, "mQtiRilInterface is not ready yet");
            return false;
        }
        return mQtiRilInterface.setLocalCallHold(mPhoneId, enable);
    }

    public void fetchIMEI() {
            Rlog.d(LOG_TAG, "fetching device id");
            mCi.getDeviceIdentity(obtainMessage(EVENT_GET_DEVICE_IDENTITY_DONE));
    }

    @Override
    public void dispose() {
        mQtiRilInterface.unRegisterForServiceReadyEvent(this);
        mQtiRilInterface = null;
        super.dispose();
    }

    @Override
    public void handleMessage(Message msg) {
        Rlog.d(LOG_TAG, "handleMessage: Event: " + msg.what);
        AsyncResult ar;
        switch(msg.what) {
            case EVENT_OEM_HOOK_SERVICE_READY:
                ar = (AsyncResult)msg.obj;
                if ((ar != null) && ar.result != null) {
                    boolean isServiceReady = (boolean)ar.result;
                    if (isServiceReady) {
                        if (mIsPhoneReadyPending) updatePhoneReady(mPhoneId);
                        Rlog.d(LOG_TAG, "EVENT_OEM_HOOK_SERVICE_READY received");
                    }
                } else {
                    Rlog.e(LOG_TAG, "Error: empty result, EVENT_OEM_HOOK_SERVICE_READY");
                }
                // Do not call super.handleMessage().
                // EVENT_OEM_HOOK_SERVICE_READY is non-aosp event.
                break;

            case EVENT_SIM_RECORDS_LOADED:
                if(isPhoneTypeGsm()) {
                    Rlog.d(LOG_TAG, "notify call forward indication, phone id:" + mPhoneId);
                    notifyCallForwardingIndicator();
                }

                super.handleMessage(msg);
                break;

            case EVENT_RADIO_AVAILABLE:
                mIsPhoneReadySent = false;
                updatePhoneReady(mPhoneId);
                super.handleMessage(msg);
                break;

            case EVENT_RIL_CONNECTED:
                mIsPhoneReadySent = false;
                super.handleMessage(msg);
                break;

            case EVENT_RADIO_ON:
            case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
                if (mNumPhones == 1) {
                    handleCdmaNVSubscriptionSource(mCdmaNVSSM.getCdmaSubscriptionSource());
                }
                super.handleMessage(msg);
                break;

            case EVENT_NV_READY:{
                Rlog.d(LOG_TAG, "Event EVENT_NV_READY Received");
                prepareEri();
                // Notify voicemails.
                Rlog.d(LOG_TAG, "notifyMessageWaitingChanged");
                mNotifier.notifyMessageWaitingChanged(this);
                updateVoiceMail();
                // Do not call super.handleMessage().
                // AOSP do not handle EVENT_NV_READY.
                break;
            }
            default: {
                super.handleMessage(msg);
            }

        }
    }

    // In DSDA, char 'D' is used as DTMF char for playing supervisory tone for G/W.
    // For CDMA, '#' is used. A, B, C & D are also supported as DTMF digits for G/W networks.
    @Override
    public void startDtmf(char c) {
        if (!(PhoneNumberUtils.is12Key(c) || (c == 'D'))) {
            Rlog.e(LOG_TAG, "startDtmf called with invalid character '" + c + "'");
        } else {
            if (isPhoneTypeCdma() && c == 'D') {
                c = '#';
            }
            mCi.startDtmf(c, null);
        }
    }

    // For CDMA sendBurstDtmf is used, if dtmf char is 'D' then it with '#'
    // since 'D' is used for SCH tone and for CDMA it has to be '#'.
    @Override
    public void sendBurstDtmf(String dtmfString, int on, int off, Message onComplete) {
        Character c = dtmfString.charAt(0);
        if(dtmfString.length() == 1 && c == 'D') {
            dtmfString = c.toString();
        }
        super.sendBurstDtmf(dtmfString, on, off, onComplete);
    }

    // When OOS occurs, IMS registration may be still available so that IMS service
    // state is also in-service, then reports in-service to upper layer.
    // Add a precondition to merge IMS service so that notifies proper service state
    // after IMS changes RAT.
    @Override
    public ServiceState getServiceState() {
        if (mSST == null || mSST.mSS.getState() != ServiceState.STATE_IN_SERVICE) {
            // Ensure UE has IMS service capability, then merge IMS service state.
            // Video enabled includes WIFI video
            final boolean isImsEnabled = mImsPhone != null && (mImsPhone.isVolteEnabled()
                    || mImsPhone.isVideoEnabled()
                    || mImsPhone.isWifiCallingEnabled());
            if (isImsEnabled) {
                return ServiceState.mergeServiceStates(
                        ((mSST == null) ? new ServiceState() : mSST.mSS),
                        mImsPhone.getServiceState());
            }
        }

        if (mSST != null) {
            return mSST.mSS;
        } else {
            // avoid potential NPE in EmergencyCallHelper during Phone switch
            return new ServiceState();
        }
    }

    @Override
    public void sendSubscriptionSettings(boolean restoreNetworkSelection) {
         ExtTelephonyServiceImpl serviceImpl = ExtTelephonyServiceImpl.getInstance();

        // When subsidy lock feature enabled, device in subsidy locked/LOCKED state,
        // set network selection mode to AUTOMATIC for primary carrier SIM
        if (SubsidyLockSettingsObserver.isSubsidyLockFeatureEnabled() &&
                !SubsidyLockSettingsObserver.isSubsidyUnlocked(mContext) &&
                (serviceImpl != null & serviceImpl.isPrimaryCarrierSlotId(getPhoneId()))) {
            // Send settings down
            int type = PhoneFactory.calculatePreferredNetworkType(mContext, getSubId());
            setPreferredNetworkType(type, null);

            logd(" settings network selection mode to AUTO ");
            setNetworkSelectionModeAutomatic(null);
        } else {
            super.sendSubscriptionSettings(restoreNetworkSelection);
        }
    }

    private BaseRilInterface getQtiRilInterface() {
        BaseRilInterface qtiRilInterface;
        if (getUnitTestMode()) {
            logd("getQtiRilInterface, unitTestMode = true");
            qtiRilInterface = SimulatedQtiRilInterface.getInstance(mContext);
        } else {
            qtiRilInterface = QtiRilInterface.getInstance(mContext);
        }
        return qtiRilInterface;
    }

    private void logd(String msg) {
        Rlog.d(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }

    private void loge(String msg) {
        Rlog.e(LOG_TAG, "[" + mPhoneId +" ] " + msg);
    }
}
