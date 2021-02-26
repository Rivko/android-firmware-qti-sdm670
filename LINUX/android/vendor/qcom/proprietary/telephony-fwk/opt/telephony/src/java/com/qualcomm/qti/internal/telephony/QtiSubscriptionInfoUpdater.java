/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution, Apache license notifications and license are retained
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

package com.qualcomm.qti.internal.telephony;

import android.content.Context;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import android.os.AsyncResult;
import android.os.Message;
import android.os.SystemProperties;
import android.telephony.Rlog;
import android.telephony.SubscriptionInfo;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.os.Looper;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.SubscriptionController;
import com.android.internal.telephony.SubscriptionInfoUpdater;
import com.android.internal.telephony.uicc.IccCardApplicationStatus.AppType;
import com.android.internal.telephony.uicc.IccCardStatus.CardState;
import com.android.internal.telephony.uicc.UiccCard;
import com.android.internal.telephony.uicc.UiccController;
import com.android.internal.os.BackgroundThread;

import java.util.List;

/**
 * This class extends AOSP SubscriptionInfoUpdater class to achieve below support
 *
 * 1. When SIM card not provisioned(i.e SIM state in DETECTED) add a record
 *    in subInfo table by using the ICCID queried from RIL.
 * 2. To reduce delay in SubInfo records availability to clients, add subInfo record
 *    to table without waiting for SIM state moves to LOADED.
 */
public class QtiSubscriptionInfoUpdater extends SubscriptionInfoUpdater {
    private static final String LOG_TAG = "QtiSubscriptionInfoUpdater";

    private static final int EVENT_ADD_SUBINFO_RECORD = 100;
    private static final int EVENT_UPDATE_NV_RECORD = 101;
    private static final String ICCID_STRING_FOR_NO_SIM = "";
    private static final String ICCID_STRING_FOR_NV = "DUMMY_NV_ID";
    private static final String ACTION_ALL_ICC_QUERY_DONE =
            "org.codeaurora.intent.action.ALL_ICC_QUERY_DONE";
    private static final String ROAMING_SETTINGS_CONFIG = "persist.vendor.radio.roamingsettings";
    private static final String CT_IIN = "898603,898611,8985231,8985302,8985307";
    private boolean isNVSubAvailable = false;
    private boolean needEnableRoamingSettings = false;
    private static Context sContext = null;

    private static final int mNumPhones = TelephonyManager.getDefault().getPhoneCount();

    // check current carrier is CT class A, C or not set
    private static final String CARRIER_MODE_CT_CLASS_A = "ct_class_a";
    private String mCarrierMode = SystemProperties.get("persist.radio.carrier_mode", "default");
    private boolean mIsCTClassA = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_A);

    // SubInfo record can be added from below two places:
    // 1. From base class when SIM state internal intent received
    //    with LOADED/SIM_LOCKED state.
    // 2. This class adds when it receives request from UiccCardProvisioner.
    // Since the request can come both places, we are using this flag to do not
    // update the SubInfo record second time(by calling updateSubscriptionInfoByIccId()).
    // Initially it set to 'true', when the SubInfo updated from this class
    // this moves to 'false', so subsequent request will be ignored.
    // When SIM becomes absent this will be reset.
    private boolean[] mIsRecordUpdateRequired = new boolean[mNumPhones];
    private static QtiSubscriptionInfoUpdater sInstance = null;

    static QtiSubscriptionInfoUpdater init(Looper looper, Context context,
            Phone[] phone, CommandsInterface[] ci) {
        synchronized (QtiSubscriptionInfoUpdater.class) {
            if (sInstance == null) {
                sInstance = new QtiSubscriptionInfoUpdater(looper, context, phone, ci);
            } else {
                Log.wtf(LOG_TAG, "init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static QtiSubscriptionInfoUpdater getInstance() {
        if (sInstance == null) {
            Log.wtf(LOG_TAG, "getInstance null");
        }
        return sInstance;
    }

    private QtiSubscriptionInfoUpdater(Looper looper, Context context,
            Phone[] phone, CommandsInterface[] ci) {
        super(looper, context, phone, ci);
        sContext = context;
        for (int index = 0; index < mNumPhones; index++) {
            mIsRecordUpdateRequired[index] = false;
        }
    }

    @Override
    public void handleMessage(Message msg) {
        Rlog.d(LOG_TAG, " handleMessage: EVENT:  " + msg.what);
        switch (msg.what) {
            case EVENT_ADD_SUBINFO_RECORD:
                handleAddSubInfoRecordEvent(msg.arg1, (String) msg.obj);
                break;

            case EVENT_UPDATE_NV_RECORD:
                handleUpdateNVRecord(msg.arg1);
                break;

            case EVENT_SIM_LOCKED_QUERY_ICCID_DONE:
                AsyncResult ar = (AsyncResult)msg.obj;
                QueryIccIdUserObj uObj = (QueryIccIdUserObj) ar.userObj;
                int slotId = uObj.slotId;
                if ((mIccId[slotId] == null) || (mIccId[slotId] == ICCID_STRING_FOR_NO_SIM)) {
                    mIsRecordUpdateRequired[slotId] = true;
                }
                super.handleMessage(msg);
                break;

            default:
                super.handleMessage(msg);
                break;
        }
    }

    public void updateNVRecord(boolean isNVReady, int slotId) {
        Rlog.d(LOG_TAG, "updateNVRecord, isNVReady: " + isNVReady + " slotId: " + slotId);
        isNVSubAvailable = isNVReady;
        sendMessage(obtainMessage(EVENT_UPDATE_NV_RECORD, slotId, -1, null));
    }

    public void handleUpdateNVRecord(int slotId) {
        if (isNVSubAvailable) {
            mIsRecordUpdateRequired[slotId] = true;
            handleAddSubInfoRecordEvent(slotId, ICCID_STRING_FOR_NV);
        } else {
            List<SubscriptionInfo> subInfo =
                    SubscriptionController.getInstance().getSubInfoUsingSlotIndexWithCheck(slotId,
                    false, sContext.getOpPackageName());
            if (subInfo != null) {
                Rlog.d(LOG_TAG, "handleUpdateNVRecord, active IccID: "
                        + subInfo.get(0).getIccId());
                if (subInfo.get(0).getIccId().equals(ICCID_STRING_FOR_NV)) {
                    handleSimAbsentOrError(slotId, IccCardConstants.INTENT_VALUE_ICC_ABSENT);
                }
            }
        }
    }

    void addSubInfoRecord(int slotId, String iccId) {
         if ((iccId == null) || (slotId < 0 || slotId >= mNumPhones)) {
             Rlog.e(LOG_TAG, "addSubInfoRecord, invalid input IccId[" + slotId + "] = " + iccId);
             return;
         }
        // Post message to handler to handle all record update events in handler thread
        sendMessage(obtainMessage(EVENT_ADD_SUBINFO_RECORD, slotId, -1, iccId));
    }

    synchronized private void handleAddSubInfoRecordEvent(int slotId, String iccId) {
        if ((mIccId[slotId] != null) && (!mIccId[slotId].equals(ICCID_STRING_FOR_NO_SIM))) {
            Rlog.d(LOG_TAG, "Record already exists ignore duplicate update, existing IccId = "
                    + mIccId[slotId] + " recvd iccId[" + slotId + "] = " + iccId);
            return;
        }

        if ((mIccId[slotId] == null) || (mIccId[slotId] == ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }

        mIccId[slotId] = iccId;
        Rlog.d(LOG_TAG, " slotId = " + slotId + ", iccId = " + iccId +
            " needEnableRoamingSettings = " + needEnableRoamingSettings);
        if (!TextUtils.isEmpty(iccId) && isCtCard(iccId) && SystemProperties.getBoolean(
                ROAMING_SETTINGS_CONFIG, false)) {
            setRoamingSettingsState(sContext, true);
            needEnableRoamingSettings = true;
        } else if (!needEnableRoamingSettings){
            setRoamingSettingsState(sContext, false);
        }

        // in CT class A only slot 0 support CT card
        if (mIsCTClassA && slotId == PhoneConstants.SUB1) {
            checkUiccCard(iccId);
        }

        // If all SIMs IccId available add subInfo records.
        if (isAllIccIdQueryDone()) {
            updateSubscriptionInfoByIccId();
        }
    }

    private void sendBroadCastToApp () {
        Intent intent = new Intent();
        intent.setClassName("com.qualcomm.qti.networksetting",
                "com.qualcomm.qti.networksetting.SimAlertNotification");
        Rlog.d(LOG_TAG, "Sending broadcast to NetworkSetting" + intent);
        sContext.sendBroadcast(intent);
    }

    private void checkUiccCard(String iccId) {
        if (isCtCard(iccId)) {
            UiccCard uiccCard = UiccController.getInstance().getUiccCard(PhoneConstants.SUB1);
            if (uiccCard != null && uiccCard.getCardState() == CardState.CARDSTATE_PRESENT)  {
                boolean hasUiccApp = uiccCard.isApplicationOnIcc(AppType.APPTYPE_USIM)
                        && (uiccCard.isApplicationOnIcc(AppType.APPTYPE_CSIM)
                        || uiccCard.isApplicationOnIcc(AppType.APPTYPE_RUIM));
            if (!hasUiccApp) {
                Rlog.d(LOG_TAG, "This is a 3G CT card.");
                sendBroadCastToApp();
            }
            }
        } else {
            Rlog.d(LOG_TAG, "This is a non-CT card.");
            sendBroadCastToApp();
        }
    }

    private boolean isCtCard (String iccId) {
        for (String iin : CT_IIN.split(",")) {
            if (iccId.startsWith(iin)) {
                Rlog.d(LOG_TAG, "Iccid " + iccId + "is CT card");
                return true;
            }
        }
        return false;
    }

    private void setRoamingSettingsState(Context context, boolean install) {
        if (context == null) {
            Rlog.d(LOG_TAG,"setRoamingSettingsState, context null");
            return;
        }
        PackageManager pm = context.getPackageManager();
        if (pm == null) {
            Rlog.d(LOG_TAG,"setRoamingSettingsState, PackageManager null");
            return;
        }

        String packageName = "com.qualcomm.qti.roamingsettings";
        ComponentName cn = new ComponentName(packageName,
                "com.qualcomm.qti.roamingsettings.RoamingSettingsActivity");
        int state = install ? PackageManager.COMPONENT_ENABLED_STATE_ENABLED
                : PackageManager.COMPONENT_ENABLED_STATE_DISABLED;
        List<PackageInfo> packages = pm.getInstalledPackages(
                PackageManager.MATCH_UNINSTALLED_PACKAGES);
        for (PackageInfo pi : packages) {
            if (!TextUtils.isEmpty(pi.packageName) && packageName.equals(
                    pi.packageName)) {
                Rlog.d(LOG_TAG,"setRoamingSettings state = " + state);
                pm.setComponentEnabledSetting(cn, state, 0);
            }
        }

    }

    @Override
    protected void handleSimLoaded(int slotId) {
        if ((mIccId[slotId] == null) || (mIccId[slotId] == ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }
        super.handleSimLoaded(slotId);
    }

    @Override
    protected void handleSimAbsentOrError(int slotId, String simState) {
        if (isNVSubAvailable) {
            return;
        }

        if ((mIccId[slotId] == null) || (mIccId[slotId] != ICCID_STRING_FOR_NO_SIM)) {
            mIsRecordUpdateRequired[slotId] = true;
        }

        super.handleSimAbsentOrError(slotId, simState);
    }

    @Override
    synchronized protected void updateSubscriptionInfoByIccId() {
        boolean isUpdateRequired = false;
        for (int index = 0; index < mNumPhones; index++) {
            if (mIsRecordUpdateRequired[index] == true) {
                isUpdateRequired = true;
                break;
            }
        }

        if (isUpdateRequired) {
            super.updateSubscriptionInfoByIccId();
            Rlog.d(LOG_TAG, "SIM state changed, Updating user preference ");
            // Calling updateUserPreferences() here to update user preference
            // 1. During device power-up
            // 2. When SIM hot swap removal/insertion performed.
            if (QtiUiccCardProvisioner.getInstance().isAllCardProvisionInfoReceived()) {
                QtiSubscriptionController.getInstance().updateUserPreferences();
            }

            // Once subIfo records added/updated reset mIsRecordUpdateRequired flag
            for (int index = 0; index < mNumPhones; index++) {
                mIsRecordUpdateRequired[index] = false;
            }
        } else {
            Rlog.d(LOG_TAG, "Ignoring subscription update event");
        }
    }
}
