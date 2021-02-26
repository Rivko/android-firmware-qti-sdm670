/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
/*
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

package com.qualcomm.qti.setupwizard.sim;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.FragmentManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceCategory;
import android.preference.PreferenceFragment;
import android.provider.Settings;
import android.telecom.PhoneAccountHandle;
import android.telecom.PhoneAccount;
import android.telecom.TelecomManager;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyProperties;
import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.ui.DropDownPreference;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;
import com.qualcomm.qti.setupwizard.ui.SimInfoPreference;
import org.codeaurora.internal.IExtTelephony;

import java.util.ArrayList;
import java.util.List;

public class SimSettingFragment extends PreferenceFragment {
    private static final String TAG = "SimSettingFragment";
    private static final boolean DBG = false;

    private static final String SIM_SETTING = "sim_setting";
    private FragmentCallbacks mCallbacks;
    private FragmentManager mFragmentManager;

    // These are the list of  possible values that
    // IExtTelephony.getCurrentUiccCardProvisioningStatus() can return
    private static final int PROVISIONED = 1;
    private static final int NOT_PROVISIONED = 0;
    private static final int INVALID_STATE = -1;

    private static final String SIM_INFO_LAYOUT = "sim_info_layout";
    private static final String SIM_CARD_CATEGORY = "sim_cards";
    private static final String KEY_CELLULAR_DATA = "sim_cellular_data";
    private static final String KEY_CALLS = "sim_calls";
    private static final String KEY_SMS = "sim_sms";
    public static final String EXTRA_SLOT_ID = "slot_id";
    private static final String SIM_ACTIVITIES_CATEGORY = "sim_activities";
    private static final String KEY_PRIMARY_SUB_SELECT = "select_primary_sub";
    private static final String CARRIER_MODE_CT_CLASS_A = "ct_class_a";
    private static final String SETTING_USER_PREF_DATA_SUB = "user_preferred_data_sub";

    private IExtTelephony mExtTelephony = IExtTelephony.Stub.
            asInterface(ServiceManager.getService("extphone"));

    /**
     * By UX design we use only one Subscription Information(SubInfo) record per SIM slot.
     * mAvalableSubInfos is the list of SubInfos we present to the user.
     * mSubInfoList is the list of all SubInfos.
     * mSelectableSubInfos is the list of SubInfos that a user can select for data, calls, and SMS.
     */
    private List<SubscriptionInfo> mAvailableSubInfos = null;
    private List<SubscriptionInfo> mSelectableSubInfos = null;
    private PreferenceCategory mSimCards = null;
    private SubscriptionManager mSubscriptionManager;
    private int mNumSlots;
    private Context mContext;
    private SimInfoPreference mSimInfoPreference;

    private AlertDialog mAlertDialog = null;
    private boolean needUpdate = false;
    private int mPhoneCount = TelephonyManager.getDefault().getPhoneCount();
    private int[] mUiccProvisionStatus = new int[mPhoneCount];
    private Preference mPrimarySubSelect = null;
    private int[] mCallState = new int[mPhoneCount];
    private PhoneStateListener[] mPhoneStateListener = new PhoneStateListener[mPhoneCount];
    // check current carrier is CT class A, C or not set
    private String mCarrierMode = SystemProperties.get("persist.carrier.mode", "default");
    private boolean mIsCTClassA = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_A);

    private static final String ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED =
            "org.codeaurora.intent.action.ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED";
    private static final String EXTRA_NEW_PROVISION_STATE = "newProvisionState";
    private static final String CONFIG_LTE_SUB_SELECT_MODE = "config_lte_sub_select_mode";
    private static final String CONFIG_PRIMARY_SUB_SETABLE = "config_primary_sub_setable";
    private static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";
    // If this config set to '1' DDS option would be greyed out on UI.
    // For more info pls refere framework code.
    private static final String CONFIG_DISABLE_DDS_PREFERENCE = "config_disable_dds_preference";

    private static final int NO_SIM = 0;
    private static final int ASK_VALUE = -1;
    private static final int CARD_1 = 0;
    private static final int CARD_2 = 1;
    private CharSequence mFirstSelectableSubInfo;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mContext = getActivity();
        mFragmentManager = getActivity().getFragmentManager();
        mSubscriptionManager = SubscriptionManager.from(mContext);
        final TelephonyManager tm =
                (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
        addPreferencesFromResource(R.xml.setup_sim_preference);
        onBackLabel(SIM_SETTING);

        mSimInfoPreference = (SimInfoPreference) findPreference(SIM_INFO_LAYOUT);
        mPrimarySubSelect = findPreference(KEY_PRIMARY_SUB_SELECT);
        mNumSlots = tm.getSimCount();
        mSimCards = (PreferenceCategory)findPreference(SIM_CARD_CATEGORY);
        mAvailableSubInfos = new ArrayList<SubscriptionInfo>(mNumSlots);
        mSelectableSubInfos = new ArrayList<SubscriptionInfo>();

        IntentFilter intentFilter = new IntentFilter(ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED);
        mContext.registerReceiver(mReceiver, intentFilter);
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof FragmentCallbacks) {
            mCallbacks = (FragmentCallbacks) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        mSubscriptionManager.addOnSubscriptionsChangedListener(mOnSubscriptionsChangeListener);
        updateSubscriptions();
        listen();
    }

    @Override
    public void onPause() {
        super.onPause();
        mSubscriptionManager.removeOnSubscriptionsChangedListener(mOnSubscriptionsChangeListener);
        unRegisterPhoneStateListener();
    }

    @Override
    public void onDestroy() {
        mContext.unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    private final SubscriptionManager.OnSubscriptionsChangedListener mOnSubscriptionsChangeListener
            = new SubscriptionManager.OnSubscriptionsChangedListener() {
        @Override
        public void onSubscriptionsChanged() {
            if (DBG) log("onSubscriptionsChanged:");
            updateSubscriptions();
        }
    };

    private void updateSubscriptions() {
        final List<SubscriptionInfo> mSubInfoList =
                mSubscriptionManager.getActiveSubscriptionInfoList();
        SubscriptionInfo sim1Info = mSubscriptionManager
                .getActiveSubscriptionInfoForSimSlotIndex(0);
        SubscriptionInfo sim2Info = mSubscriptionManager
                .getActiveSubscriptionInfoForSimSlotIndex(1);
        if (mSubInfoList == null) {
            updateRes(NO_SIM);
            mFragmentManager.beginTransaction()
                    .replace(R.id.base_sim, new NoSimCardFragment())
                    .commit();
        }
        if (sim1Info != null) {
            mSimInfoPreference.setSim1NameText(sim1Info.getDisplayName());
        } else {
            mSimInfoPreference.setSim1NameText("SIM 1");
        }
        if (sim2Info != null) {
            mSimInfoPreference.setSim2NameText(sim2Info.getDisplayName());
        } else {
            mSimInfoPreference.setSim2NameText("SIM 2");
        }

        mAvailableSubInfos.clear();
        mSelectableSubInfos.clear();

        for (int i = 0; i < mNumSlots; ++i) {
            final SubscriptionInfo sir = mSubscriptionManager
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            try {
                //get current provision state of the SIM.
                mUiccProvisionStatus[i] =
                        mExtTelephony.getCurrentUiccCardProvisioningStatus(i);
            } catch (RemoteException ex) {
                mUiccProvisionStatus[i] = INVALID_STATE;
                Log.e(TAG, "Failed to get pref, slotId: "+ i +" Exception: " + ex);
            } catch (NullPointerException ex) {
                mUiccProvisionStatus[i] = INVALID_STATE;
                Log.e(TAG, "Failed to get pref, slotId: "+ i +" Exception: " + ex);
            }
            mAvailableSubInfos.add(sir);
            if (sir != null && (isSubProvisioned(i))) {
                mSelectableSubInfos.add(sir);
            }
        }
        if (mSelectableSubInfos.size() > 0) {
            mFirstSelectableSubInfo = mSelectableSubInfos.get(CARD_1).getDisplayName();
        }
        updateAllOptions();

        // in CT class A only slot 0 support CT card, alert user if CT card in slot 1
        if (mIsCTClassA && isCTCardForSimSlotIndex(PhoneConstants.SUB2)) {
            alertRestrictCTCardDialog();
        }
    }

    private boolean isCTCardForSimSlotIndex(int slotIdx) {
        String simOperator = TelephonyManager.getDefault().getSimOperatorNumericForPhone(slotIdx);
        boolean isCTCard = false;
        if(!TextUtils.isEmpty(simOperator)) {
            if (simOperator.equals("46003") || simOperator.equals("46011") ||
                    simOperator.equals("46012") || simOperator.equals("20404") ||
                    simOperator.equals("45502") || simOperator.equals("45507") ||
                    simOperator.equals("45403") || simOperator.equals("45404")) {
                isCTCard = true;
            }
        }
        if (DBG) log(" simOperator " + simOperator + ", isCTCard " + isCTCard);
        return isCTCard;
    }

    private void alertRestrictCTCardDialog() {
        // Confirm only one AlertDialog instance to show.
        dismissDialog(mAlertDialog);
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(mContext);
        alertDialogBuilder.setTitle(R.string.alert_ct_in_wrong_slot_title);
        alertDialogBuilder.setMessage(R.string.alert_ct_in_wrong_slot_message);
        alertDialogBuilder.setNeutralButton(android.R.string.ok, null);

        mAlertDialog = alertDialogBuilder.create();
        mAlertDialog.show();
    }

    private void dismissDialog(Dialog dialog) {
        if((dialog != null) && (dialog.isShowing())) {
            dialog.dismiss();
            dialog = null;
        }
    }

    private void updateAllOptions() {
        updateActivitesCategory();
    }

    private void updateActivitesCategory() {
        createDropDown((DropDownPreference) findPreference(KEY_CELLULAR_DATA));
        createDropDown((DropDownPreference) findPreference(KEY_CALLS));
        createDropDown((DropDownPreference) findPreference(KEY_SMS));
        updateCellularDataValues();
        updateCallValues();
        updateSmsValues();
    }

    private void updateSmsValues() {
        final Preference simPref = findPreference(KEY_SMS);
        simPref.setTitle(R.string.sms_messages_title);
        boolean isSMSPrompt = false;
        SubscriptionInfo sir = mSubscriptionManager.getActiveSubscriptionInfo(
                SubscriptionManager.getDefaultSmsSubscriptionId());
        try {
            isSMSPrompt = mExtTelephony.isSMSPromptEnabled();
        } catch (RemoteException ex) {
            loge("RemoteException @isSMSPromptEnabled" + ex);
        } catch (NullPointerException ex) {
            loge("NullPointerException @isSMSPromptEnabled" + ex);
        }
        Log.d(TAG, "[updateSmsValues] isSMSPrompt: " + isSMSPrompt);
        if (isSMSPrompt || sir == null) {
            simPref.setSummary(mContext.getResources().getString(
                    R.string.sim_calls_ask_first_prefs_title));
            ((DropDownPreference) simPref).setSelectedValue(ASK_VALUE, false);
        } else {
            simPref.setSummary(sir.getDisplayName());
            ((DropDownPreference) simPref).setSelectedValue(sir.getSimSlotIndex(), false);
        }
        simPref.setEnabled(mSelectableSubInfos.size() > 1);
    }

    private void updateCellularDataValues() {
        final Preference simPref = findPreference(KEY_CELLULAR_DATA);
        final SubscriptionInfo sir = mSubscriptionManager.getDefaultDataSubscriptionInfo();
        simPref.setTitle(R.string.cellular_data_title);

        if (sir != null && sir.getSubscriptionId() == getUserPrefDataSubIdInDb()) {
            simPref.setSummary(sir.getDisplayName());
            ((DropDownPreference) simPref).setSelectedValue(sir.getSimSlotIndex(), false);
        } else if (sir == null) {
            simPref.setSummary(R.string.sim_selection_required_pref);
        }

        boolean callStateIdle = isCallStateIdle();
        final boolean ecbMode = SystemProperties.getBoolean(
                TelephonyProperties.PROPERTY_INECM_MODE, false);
        // Enable data preference in msim mode and call state idle
        simPref.setEnabled((mSelectableSubInfos.size() > 1) && !disableDds()
                && callStateIdle && !ecbMode);
    }

    private void updateCallValues() {
        final Preference simPref = findPreference(KEY_CALLS);
        final TelecomManager telecomManager = TelecomManager.from(mContext);
        final PhoneAccountHandle phoneAccount =
                telecomManager.getUserSelectedOutgoingPhoneAccount();
        final List<PhoneAccountHandle> allPhoneAccounts =
                telecomManager.getCallCapablePhoneAccounts();

        simPref.setTitle(R.string.calls_title);
        if (phoneAccount == null || allPhoneAccounts.size() < 2) {
            simPref.setSummary(mContext.getResources().getString(
                    R.string.sim_calls_ask_first_prefs_title));
            ((DropDownPreference) simPref).setSelectedValue(ASK_VALUE, false);
        } else {
            PhoneAccount phoneaccount = telecomManager
                    .getPhoneAccount(phoneAccount);
            PhoneAccount allPhoneaccount = telecomManager
                    .getPhoneAccount(allPhoneAccounts.get(CARD_1));
            if (phoneaccount != null) {
                simPref.setSummary(phoneaccount.getLabel());
                if (mSelectableSubInfos.size() > 1
                        && allPhoneaccount != null
                        && mFirstSelectableSubInfo.toString().equals(allPhoneaccount
                            .getLabel())) {
                    if (mFirstSelectableSubInfo.toString()
                            .equals(phoneaccount.getLabel())) {
                        ((DropDownPreference) simPref).setSelectedValue(CARD_1, false);
                    } else {
                        ((DropDownPreference) simPref).setSelectedValue(CARD_2, false);
                    }
                }
            } else {
                simPref.setSummary(mContext.getResources().getString(
                        R.string.sim_calls_ask_first_prefs_title));
                ((DropDownPreference) simPref).setSelectedValue(ASK_VALUE, false);
            }
        }
        simPref.setEnabled(allPhoneAccounts.size() > 1);
    }

    private void loge(String msg) {
        if (DBG) Log.e(TAG + "msg", msg);
    }

    public void createDropDown(DropDownPreference preference) {
        final DropDownPreference simPref = preference;
        final String keyPref = simPref.getKey();

        //If Fragment not yet attached to Activity, return
        if (!isAdded()) {
            Log.d(TAG,"Fragment not yet attached to Activity, EXIT!!" );
            return;
        }
        simPref.clearItems();

        if (mSelectableSubInfos == null) {
            return;
        }

        final int subSelectableSize = mSelectableSubInfos.size();
        if (subSelectableSize > 1 && !keyPref.equals(KEY_CELLULAR_DATA)) {
            simPref.addItem(getResources().getString(
                    R.string.sim_calls_ask_first_prefs_title), ASK_VALUE);
        }

        for (int i = 0; i < subSelectableSize; ++i) {
            final SubscriptionInfo sir = mSelectableSubInfos.get(i);
            if (sir != null) {
                simPref.addItem(sir.getDisplayName().toString(), sir.getSimSlotIndex());
            }
        }

        simPref.setCallback(new DropDownPreference.Callback() {
            @Override
            public boolean onItemSelected(int pos, Object value) {
                final int subId = (Integer) value == ASK_VALUE ? ASK_VALUE :
                        mSubscriptionManager.getSubId((Integer) value)[0];
                Log.d(TAG, "Preference[" + simPref.getKey() + "]select value :" + value
                        + "with subId(" + subId + ")");
                if (simPref.getKey().equals(KEY_CELLULAR_DATA)) {
                    final List<SubscriptionInfo> subInfoList =
                            mSubscriptionManager.getActiveSubscriptionInfoList();
                    final SubscriptionInfo sir = subInfoList.get(pos);
                    final int preferredSubID = sir.getSubscriptionId();
                    int defaultDataSubId = SubscriptionManager.getDefaultDataSubscriptionId();
                    Log.d(TAG, "DefDataId [" + defaultDataSubId + "]");
                    if (defaultDataSubId != preferredSubID && subSelectableSize > 1) {
                        mSubscriptionManager.setDefaultDataSubId(preferredSubID);
                        setUserPrefDataSubIdInDb(preferredSubID);
                    }
                } else if (simPref.getKey().equals(KEY_CALLS)) {
                    //subId 0 is meant for "Ask First"/"Prompt" option as per AOSP
                    final TelecomManager telecomManager = TelecomManager.from(mContext);
                    final List<PhoneAccountHandle> phoneAccountsList =
                            telecomManager.getCallCapablePhoneAccounts();
                    Log.d(TAG, "DefVoiceId ["
                            + SubscriptionManager.getDefaultVoiceSubscriptionId() + "]");
                    if (phoneAccountsList.size() > 1
                            && mFirstSelectableSubInfo.toString().equals(telecomManager
                            .getPhoneAccount(phoneAccountsList.get(CARD_1)).getLabel())) {
                        setUserSelectedOutgoingPhoneAccount(
                                pos < 1 ? null : phoneAccountsList.get(pos - 1));
                    }
                } else if (simPref.getKey().equals(KEY_SMS)) {
                    Log.d(TAG, "DefSMSId [" + SubscriptionManager.getDefaultSmsSubscriptionId() + "]");
                    int defaultSmsSubId = SubscriptionManager.getDefaultSmsSubscriptionId();
                    boolean isSmsPrompt = false;
                    if ((defaultSmsSubId != subId) && (subSelectableSize > 1)) {
                        if (subId == ASK_VALUE) {
                            isSmsPrompt = true;
                        }
                        mSubscriptionManager.setDefaultSmsSubId(subId);
                    }
                    Log.d(TAG, "isSmsPrompt: " + isSmsPrompt);
                    try {
                        mExtTelephony.setSMSPromptEnabled(isSmsPrompt);
                    } catch (RemoteException ex) {
                        Log.e(TAG, "RemoteException @setSMSPromptEnabled" + ex);
                    } catch (NullPointerException ex) {
                        Log.e(TAG, "NullPointerException @setSMSPromptEnabled" + ex);
                    }
                }

                return true;
            }
        });
    }

    private void setUserPrefDataSubIdInDb(long subId) {
        android.provider.Settings.Global.putLong(mContext.getContentResolver(),
                SETTING_USER_PREF_DATA_SUB, subId);
        Log.e(TAG, "updating data subId: " + subId + " in DB");
    }

    private Long getUserPrefDataSubIdInDb() {
        return android.provider.Settings.Global.getLong(mContext.getContentResolver(),
                SETTING_USER_PREF_DATA_SUB, -1);
    }

    private void setUserSelectedOutgoingPhoneAccount(PhoneAccountHandle phoneAccount) {
        final TelecomManager telecomManager = TelecomManager.from(mContext);
        telecomManager.setUserSelectedOutgoingPhoneAccount(phoneAccount);
    }

    private void log(String s) {
        Log.d(TAG, s);
    }

    // Internal utility, returns true if Uicc card
    // corresponds to given slotId is provisioned.
    private boolean isSubProvisioned(int slotId) {
        boolean retVal = false;

        if (mUiccProvisionStatus[slotId] == PROVISIONED) retVal = true;
        return retVal;
    }

    private int getProvisionStatus(int slotId) {
        return mUiccProvisionStatus[slotId];
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "Intent received: " + action);
            if (ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED.equals(action)) {
                int phoneId = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                        SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                int newProvisionedState = intent.getIntExtra(EXTRA_NEW_PROVISION_STATE,
                        NOT_PROVISIONED);
                updateSubscriptions();
                Log.d(TAG, "Received ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED on phoneId: "
                        + phoneId + " new sub state " + newProvisionedState);
            }
        }
    };

    private boolean disableDds() {
        boolean disableDds = Settings.Global.getInt(mContext.getContentResolver(),
                CONFIG_DISABLE_DDS_PREFERENCE, 0) == 1;

        log(" config disable dds =  " + disableDds);
        // in CT class A disable DDS option in UI
        return disableDds || mIsCTClassA;
    }

    private void listen() {
        TelephonyManager tm =
                (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
        if (mSelectableSubInfos.size() > 1) {
            Log.d(TAG, "Register for call state change");
            for (int i = 0; i < mPhoneCount; i++) {
                int subId = mSelectableSubInfos.get(i).getSubscriptionId();
                tm.listen(getPhoneStateListener(i, subId),
                        PhoneStateListener.LISTEN_CALL_STATE);
            }
        }
    }

    private void unRegisterPhoneStateListener() {
        TelephonyManager tm =
                (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
        for (int i = 0; i < mPhoneCount; i++) {
            if (mPhoneStateListener[i] != null) {
                tm.listen(mPhoneStateListener[i], PhoneStateListener.LISTEN_NONE);
                mPhoneStateListener[i] = null;
            }
        }
    }

    private PhoneStateListener getPhoneStateListener(int phoneId, int subId) {
        // Disable Sim selection for Data when voice call is going on as changing the default data
        // sim causes a modem reset currently and call gets disconnected
        // ToDo : Add subtext on disabled preference to let user know that default data sim cannot
        // be changed while call is going on
        final int i = phoneId;
        mPhoneStateListener[phoneId]  = new PhoneStateListener(subId) {
            @Override
            public void onCallStateChanged(int state, String incomingNumber) {
                if (DBG) log("PhoneStateListener.onCallStateChanged: state=" + state);
                mCallState[i] = state;
                updateCellularDataValues();
            }
        };
        return mPhoneStateListener[phoneId];
    }

    private boolean isCallStateIdle() {
        boolean callStateIdle = true;
        for (int i = 0; i < mCallState.length; i++) {
            if (TelephonyManager.CALL_STATE_IDLE != mCallState[i]) {
                callStateIdle = false;
            }
        }
        Log.d(TAG, "isCallStateIdle " + callStateIdle);
        return callStateIdle;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(String currentPage) {
        mCallbacks.onFragmentInteraction(currentPage);
    }

    public void updateRes(int count) {
        mCallbacks.updateSetupRes(count);
    }
}
