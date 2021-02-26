/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2014 The Android Open Source Project
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

package com.qualcomm.qti.internal.telephony.dataconnection;

import com.android.internal.telephony.GsmCdmaPhone;
import com.android.internal.telephony.dataconnection.ApnContext;
import com.android.internal.telephony.dataconnection.ApnSetting;
import com.android.internal.telephony.dataconnection.DcTracker;
import com.android.internal.telephony.DctConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.uicc.IccRecords;
import com.android.internal.util.ArrayUtils;

import android.telephony.CarrierConfigManager;
import android.telephony.Rlog;

import android.content.Context;
import android.os.PersistableBundle;
import android.os.SystemProperties;

import android.database.Cursor;
import android.provider.Telephony;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import android.os.Message;

public final class QtiDcTracker extends DcTracker {
    private String LOG_TAG = "QtiDCT";
    private final String OMH_FEATURE_ENABLE_OVERRIDE = "persist.radio.omh.enable";
    private final String CONFIG_FETCH_APN_FROM_OMH_CARD = "config_fetch_apn_from_omh_card";

    /* Make sure QTI_DCT_EVENTS_BASE value is always
     * larger than DctConstants.java enum value
     **/
    private final int QTI_DCT_EVENTS_BASE = 1000;
    private final int EVENT_MODEM_DATA_PROFILE_READY = QTI_DCT_EVENTS_BASE + 1;

    private QtiCdmaApnProfileTracker mOmhApt;
    private HashSet<String> mIccidSet = new HashSet<String>();
    // Constructor
    public QtiDcTracker(Phone phone) {
        super(phone);
        if (phone.getPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
            LOG_TAG = "QtiGsmDCT";
        } else if (phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            LOG_TAG = "QtiCdmaDCT";
        } else {
            LOG_TAG = "DCT";
            loge("unexpected phone type [" + phone.getPhoneType() + "]");
        }
        if (DBG) log(LOG_TAG + ".constructor");

        if (phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            boolean fetchApnFromOmhCard = getConfigItem(CONFIG_FETCH_APN_FROM_OMH_CARD);
            log(LOG_TAG + " fetchApnFromOmhCard: " + fetchApnFromOmhCard);
            boolean featureOverride = SystemProperties.getBoolean(OMH_FEATURE_ENABLE_OVERRIDE,
                    false);
            if (featureOverride) {
                log(LOG_TAG + "OMH: feature-config override enabled");
                fetchApnFromOmhCard = featureOverride;
            }

            if (fetchApnFromOmhCard) {
                mOmhApt = new QtiCdmaApnProfileTracker(phone);
                mOmhApt.registerForModemProfileReady(this,
                        EVENT_MODEM_DATA_PROFILE_READY, null);
            }
        }
        fillIccIdSet();
    }

    @Override
    public void dispose() {
        super.dispose();
        if (mOmhApt != null) {
            mOmhApt.unregisterForModemProfileReady(this);
        }
    }

    protected void cleanUpConnection(boolean tearDown, ApnContext apnContext) {
        super.cleanUpConnection(tearDown, apnContext);

        if (mOmhApt != null) {
             mOmhApt.clearActiveApnProfile();
        }
    }

    private boolean getConfigItem(String key) {
        CarrierConfigManager carrierConfigManager = (CarrierConfigManager)
            mPhone.getContext().getSystemService(Context.CARRIER_CONFIG_SERVICE);
        if (carrierConfigManager == null) {
            loge("getConfigItem: No carrier config service found.");
            return false;
        }
        int subId = mPhone.getSubId();
        PersistableBundle carrierConfig = carrierConfigManager.getConfigForSubId(subId);
        if (carrierConfig == null) {
            loge("getConfigItem: Empty carrier config.");
           return false;
        }
        return carrierConfig.getBoolean(key);
    }

    private void onModemApnProfileReady() {
        if (mState == DctConstants.State.FAILED) {
            cleanUpAllConnections(false, Phone.REASON_PS_RESTRICT_ENABLED);
        }
        if (DBG) log("OMH: onModemApnProfileReady(): Setting up data call");

        DctConstants.State overallState = getOverallState();
        boolean isDisconnected = (overallState == DctConstants.State.IDLE ||
                overallState == DctConstants.State.FAILED);

        if (DBG) log("onModemApnProfileReady: createAllApnList and cleanUpAllConnections");
        createAllApnList();
        setInitialAttachApn();
        cleanUpConnectionsOnUpdatedApns(!isDisconnected, Phone.REASON_APN_CHANGED);

        // FIXME: See bug 17426028 maybe no conditional is needed.
        if (mPhone.getSubId() == SubscriptionManager.getDefaultDataSubscriptionId()) {
            setupDataOnConnectableApns(Phone.REASON_APN_CHANGED);
        }
    }

    private boolean isRecordsLoaded() {
        boolean recordsLoaded = false;

        IccRecords r = mIccRecords.get();
        if (r != null) {
            recordsLoaded = r.getRecordsLoaded();
        }
        return recordsLoaded;
    }

    @Override
    protected void onRecordsLoadedOrSubIdChanged() {
        if (DBG) log("onRecordsLoaded: createAllApnList");
        mAutoAttachOnCreationConfig = mPhone.getContext().getResources()
                .getBoolean(com.android.internal.R.bool.config_auto_attach_data_on_creation);

        if (mOmhApt != null) {
            log("OMH: onRecordsLoaded(): calling loadProfiles()");
            // query for data profiles stored in the modem //
            mOmhApt.loadProfiles();
            if (mPhone.mCi.getRadioState().isOn()) {
                if (DBG) log("OMH: onRecordsLoaded: notifying data availability");
                notifyOffApnsOfAvailability(Phone.REASON_SIM_LOADED);
            }
        } else {
            createAllApnList();
            if (isRecordsLoaded()) {
                setInitialAttachApn();
            }
            if (mPhone.mCi.getRadioState().isOn()) {
                if (DBG) log("onRecordsLoaded: notifying data availability");
                notifyOffApnsOfAvailability(Phone.REASON_SIM_LOADED);
            }
            setupDataOnConnectableApns(Phone.REASON_SIM_LOADED);
        }
    }

    @Override
    protected void createAllApnList() {
        mMvnoMatched = false;
        mAllApnSettings = new ArrayList<>();
        String operator = mPhone.getOperatorNumeric();
        int radioTech = mPhone.getServiceState().getRilDataRadioTechnology();

        /**
         * 1. Get the cached list of OMH profiles from OmhProfileTracker.
         * 2. If OMH profiles are not available then query DB for mcc-mnc(which record?)
         *    and add them to the list.
         * 3. If emergency APN is available then add it to the list.
         * 4. Remove duplicates by merging duplicate records.
         * 5. If list is empty add dummy apn.
         *
         */
        if (mOmhApt != null && (ServiceState.isCdma(radioTech) &&
                ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD != radioTech)) {
            ArrayList<QtiApnSetting> mOmhApnsList = new ArrayList<QtiApnSetting>();
            mOmhApnsList = mOmhApt.getOmhApnProfilesList();
            if (!mOmhApnsList.isEmpty()) {
                if (DBG) log("createAllApnList: Copy Omh profiles");
                mAllApnSettings.addAll(mOmhApnsList);
            }
        }

        if (mAllApnSettings.isEmpty()) {
            if (operator != null && !operator.isEmpty()) {
                String selection = Telephony.Carriers.NUMERIC + " = '" + operator + "'";
                // query only enabled apn.
                // carrier_enabled : 1 means enabled apn, 0 disabled apn.
                // selection += " and carrier_enabled = 1";
                if (DBG) log("createAllApnList: selection=" + selection);

                Cursor cursor = mPhone.getContext().getContentResolver().query(
                        Telephony.Carriers.CONTENT_URI, null, selection, null,
                        Telephony.Carriers._ID);

                if (cursor != null) {
                    if (cursor.getCount() > 0) {
                        mAllApnSettings = createApnList(cursor);
                    }
                    cursor.close();
                }
            }
        }

        addEmergencyApnSetting();

        dedupeApnSettings();

        if (mAllApnSettings.isEmpty()) {
            if (DBG) log("createAllApnList: No APN found for carrier: " + operator);
            mPreferredApn = null;
            // TODO: What is the right behavior?
            //notifyNoData(DataConnection.FailCause.MISSING_UNKNOWN_APN);
        } else {
            mPreferredApn = getPreferredApn();
            if (mPreferredApn != null && !mPreferredApn.numeric.equals(operator)) {
                mPreferredApn = null;
                setPreferredApn(-1);
            }
            if (DBG) log("createAllApnList: mPreferredApn=" + mPreferredApn);
        }
        if (DBG) log("createAllApnList: X mAllApnSettings=" + mAllApnSettings);

        setDataProfilesAsNeeded();
    }

    /* Return true for non-dds sub.*/
    @Override
    protected boolean getAttachedStatus() {
        int dataSub = SubscriptionManager.getDefaultDataSubscriptionId();
        /* Make sure that the subId is valid and belongs to non-dds */
        if (SubscriptionManager.isUsableSubIdValue(dataSub)) {
            if (dataSub != mPhone.getSubId()) {
                /* Return hard-coded true value for PS ATTACH for non-dds sub.
                 * SETUP_DATA_CALL would be really attempted only if other
                 * coditions like sim-records, mobile-data are in favour.
                 * We are just removing the PS ATTACH from the equation for
                 * non-dds on-demand data call attempts.*/
                return true;
            }
        }
        return super.getAttachedStatus();
    }

    @Override
    protected boolean allowInitialAttachForOperator() {
        IccRecords r = mIccRecords.get();
        String iccId = (r != null) ? r.getIccId() : "";
        if (iccId != null) {
            Iterator<String> itr = mIccidSet.iterator();
            while (itr.hasNext()) {
                if (iccId.contains(itr.next())) {
                    return false;
                }
            }
        }
        return true;
    }

    private void fillIccIdSet() {
        mIccidSet.add("8991840");
        mIccidSet.add("8991854");
        mIccidSet.add("8991855");
        mIccidSet.add("8991856");
        mIccidSet.add("8991857");
        mIccidSet.add("8991858");
        mIccidSet.add("8991859");
        mIccidSet.add("899186");
        mIccidSet.add("8991870");
        mIccidSet.add("8991871");
        mIccidSet.add("8991872");
        mIccidSet.add("8991873");
        mIccidSet.add("8991874");
    }

    @Override
    public void handleMessage (Message msg) {
        if (DBG) log("QtiDcTracker handleMessage msg=" + msg);

        switch (msg.what) {
            case EVENT_MODEM_DATA_PROFILE_READY:
                onModemApnProfileReady();
                break;

            default:
                super.handleMessage(msg);
                break;
        }
    }

    @Override
    protected void log(String s) {
        Rlog.d(LOG_TAG, "[" + mPhone.getPhoneId() + "]" + s);
    }
}
