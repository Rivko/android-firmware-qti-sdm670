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

package com.qualcomm.qti.simsettings;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.database.ContentObserver;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.preference.PreferenceFragment;
import android.preference.PreferenceCategory;
import android.provider.SearchIndexableResource;
import android.provider.Settings;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.telecom.PhoneAccountHandle;
import android.telecom.PhoneAccount;
import android.telecom.TelecomManager;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

import com.android.internal.logging.MetricsLogger;
//import com.android.internal.telephony.OperatorSimInfo;
import com.android.internal.telephony.PhoneConstants;
import org.codeaurora.internal.IExtTelephony;
import com.android.internal.telephony.TelephonyProperties;

import java.util.ArrayList;
import java.util.List;

public class SimSettings extends PreferenceFragment {
    private static final String TAG = "SimSettings";
    private static final boolean DBG = true;

    // These are the list of  possible values that
    // IExtTelephony.getCurrentUiccCardProvisioningStatus() can return
    private static final int PROVISIONED = 1;
    private static final int NOT_PROVISIONED = 0;
    private static final int INVALID_STATE = -1;
    private static final int CARD_NOT_PRESENT = -2;

    private static final String DISALLOW_CONFIG_SIM = "no_config_sim";
    private static final String SIM_CARD_CATEGORY = "sim_cards";
    private static final String KEY_CELLULAR_DATA = "sim_cellular_data";
    private static final String KEY_CALLS = "sim_calls";
    private static final String KEY_SMS = "sim_sms";
    public static final String EXTRA_SLOT_ID = "slot_id";
    private static final String SIM_ACTIVITIES_CATEGORY = "sim_activities";
    private static final String KEY_PRIMARY_SUB_SELECT = "select_primary_sub";
    private static final String CARRIER_MODE_CT_CLASS_A = "ct_class_a";

    // subsidy lock status for lock screen
    private static final int PERMANENTLY_UNLOCKED = 100;
    private static final int AP_LOCKED = 102;
    private static final int SUBSIDY_UNLOCKED = 103;
    private static final String SUBSIDY_LOCK_SETTINGS = "subsidy_status";
    private static final String SUBSIDY_LOCK_SYSTEM_PROPERY
            = "ro.radio.subsidylock";
    private SubsidySettingsObserver mSubsidySettingsObserver;
    /**
     * By UX design we use only one Subscription Information(SubInfo) record per SIM slot.
     * mAvalableSubInfos is the list of SubInfos we present to the user.
     * mSelectableSubInfos is the list of SubInfos that a user can select for data, calls, and SMS.
     */
    private List<SubscriptionInfo> mAvailableSubInfos = null;
    private List<SubscriptionInfo> mSelectableSubInfos = null;
    private PreferenceCategory mSimCards = null;
    private SubscriptionManager mSubscriptionManager;
    private int mNumSlots;
    private Context mContext;
    private boolean mPrimaryPrefRemoved = false;

    private AlertDialog mAlertDialog = null;
    private ProgressDialog mProgressDialog = null;
    private boolean needUpdate = false;
    private int mPhoneCount = TelephonyManager.getDefault().getPhoneCount();
    private int[] mUiccProvisionStatus = new int[mPhoneCount];
    private Preference mPrimarySubSelect = null;
    private int[] mCallState = new int[mPhoneCount];
    private PhoneStateListener[] mPhoneStateListener = new PhoneStateListener[mPhoneCount];
    // check current carrier is CT class A, C or not set
    private String mCarrierMode = SystemProperties.get("persist.radio.carrier_mode", "default");
    private boolean mIsCTClassA = mCarrierMode.equals(CARRIER_MODE_CT_CLASS_A);

    private static final String ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED =
            "org.codeaurora.intent.action.ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED";
    private static final String EXTRA_NEW_PROVISION_STATE = "newProvisionState";
    public static final String CONFIG_SUB_SELECT_MODE_MANUAL = "config_sub_select_mode_manual";
    public static final String CONFIG_PRIMARY_SUB_IS_SETABLE = "config_primary_sub_is_setable";
    public static final String CONFIG_CURRENT_PRIMARY_SUB = "config_current_primary_sub";
    // If this config set to '1' DDS option would be greyed out on UI.
    // For more info pls refere framework code.
    private static final String CONFIG_DISABLE_DDS_PREFERENCE = "config_disable_dds_preference";
    private static final String ACTION_OUTGOING_PHONE_ACCOUNT_CHANGED =
            "codeaurora.intent.action.DEFAULT_PHONE_ACCOUNT_CHANGED";

    public SimSettings() {
        super();
    }

    @Override
    public void onCreate(final Bundle bundle) {
        super.onCreate(bundle);
        mContext = getActivity();
        mSubscriptionManager = SubscriptionManager.from(getActivity());
        final TelephonyManager tm =
                (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
        addPreferencesFromResource(R.xml.sim_settings);

        mPrimarySubSelect = (Preference) findPreference(KEY_PRIMARY_SUB_SELECT);
        mNumSlots = tm.getSimCount();
        mSimCards = (PreferenceCategory)findPreference(SIM_CARD_CATEGORY);
        mAvailableSubInfos = new ArrayList<SubscriptionInfo>(mNumSlots);
        mSelectableSubInfos = new ArrayList<SubscriptionInfo>();
        SimSelectNotification.cancelNotification(getActivity());

        IntentFilter intentFilter = new IntentFilter(ACTION_UICC_MANUAL_PROVISION_STATUS_CHANGED);
        intentFilter.addAction(ACTION_OUTGOING_PHONE_ACCOUNT_CHANGED);
        intentFilter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        mContext.registerReceiver(mReceiver, intentFilter);
        mContext.getContentResolver().registerContentObserver(Settings.Global
                .getUriFor(CONFIG_CURRENT_PRIMARY_SUB), false, mPrimaryCardObserver);

        if (isSubSidyLockFeatureEnabled()) {
            mSubsidySettingsObserver = new SubsidySettingsObserver();
            mContext.getContentResolver().registerContentObserver(
                    Settings.Secure.getUriFor(SUBSIDY_LOCK_SETTINGS),
                    false, mSubsidySettingsObserver);
        }
    }

    private class SubsidySettingsObserver extends ContentObserver {

        public SubsidySettingsObserver() {
            super(null);
        }

        @Override
        public void onChange(boolean selfChange) {
            mSubsidySettingsHandler.sendEmptyMessage(0);
        }
    }

    private Handler mSubsidySettingsHandler = new Handler() {
        public void handleMessage(Message msg) {
            updateSubscriptions();
            updateCellularDataValues();
        }
    };

    @Override
    public void onDestroy() {
        mContext.unregisterReceiver(mReceiver);
        mContext.getContentResolver().unregisterContentObserver(mPrimaryCardObserver);
        Log.d(TAG,"on onDestroy");
        super.onDestroy();
        if (mSubsidySettingsObserver != null) {
            mContext.getContentResolver().unregisterContentObserver(mSubsidySettingsObserver);
        }
    }

    private final SubscriptionManager.OnSubscriptionsChangedListener mOnSubscriptionsChangeListener
            = new SubscriptionManager.OnSubscriptionsChangedListener() {
        @Override
        public void onSubscriptionsChanged() {
            if (DBG) log("onSubscriptionsChanged:");
            updateSubscriptions();
        }
    };

    private final ContentObserver mPrimaryCardObserver = new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange, Uri uri) {
            Log.d(TAG,"primary slot changed");
            initLTEPreference();
        }
    };

    private void updateSubscriptions() {
        for (int i = 0; i < mNumSlots; ++i) {
            Preference pref = mSimCards.findPreference("sim" + i);
            if (pref instanceof SimPreference) {
                mSimCards.removePreference(pref);
            }
        }
        mAvailableSubInfos.clear();
        mSelectableSubInfos.clear();

        for (int i = 0; i < mNumSlots; ++i) {
            final SubscriptionInfo sir = mSubscriptionManager
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            SimPreference simPreference = new SimEnablerPreference(mContext, sir, i);
            simPreference.setOrder(i-mNumSlots);
            mSimCards.addPreference(simPreference);
            mAvailableSubInfos.add(sir);
            if (sir != null && (isSubProvisioned(i))) {
                mSelectableSubInfos.add(sir);
            }
        }
        updateAllOptions();
        initLTEPreference();

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
        log(" simOperator " + simOperator + ", isCTCard " + isCTCard);
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
        updateSimSlotValues();
        updateActivitesCategory();
    }

    private void updateSimSlotValues() {
        final int prefSize = mSimCards.getPreferenceCount();
        for (int i = 0; i < prefSize; ++i) {
            Preference pref = mSimCards.getPreference(i);
            if (pref instanceof SimPreference) {
                ((SimPreference)pref).update();
            }
        }
    }

    private void updateActivitesCategory() {
        updateCellularDataValues();
        updateCallValues();
        updateSmsValues();
    }

    private void updateSmsValues() {
        final Preference simPref = findPreference(KEY_SMS);
        simPref.setTitle(R.string.sms_messages_title);
        boolean isSMSPrompt = false;
        SubscriptionInfo sir = mSubscriptionManager.getActiveSubscriptionInfo(
                mSubscriptionManager.getDefaultSmsSubscriptionId());
        try {
            IExtTelephony extTelephony = IExtTelephony.Stub.
                    asInterface(ServiceManager.getService("extphone"));
            isSMSPrompt = extTelephony.isSMSPromptEnabled();
        } catch (RemoteException ex) {
            loge("RemoteException @isSMSPromptEnabled" + ex);
        } catch (NullPointerException ex) {
            loge("NullPointerException @isSMSPromptEnabled" + ex);
        }
        log("[updateSmsValues] isSMSPrompt: " + isSMSPrompt);
        if (isSMSPrompt || sir == null) {
            simPref.setSummary(mContext.getResources().getString(
                    R.string.sim_calls_ask_first_prefs_title));
        } else {
            simPref.setSummary(sir.getDisplayName());
        }
        simPref.setEnabled(mSelectableSubInfos.size() > 1  && !isAirplaneModeOn());
    }

    private void updateCellularDataValues() {
        final Preference simPref = findPreference(KEY_CELLULAR_DATA);
        final SubscriptionInfo sir = mSubscriptionManager.getDefaultDataSubscriptionInfo();
        simPref.setTitle(R.string.cellular_data_title);
        if (DBG) log("[updateCellularDataValues] sir=" + sir);

        if (sir != null) {
            simPref.setSummary(sir.getDisplayName());
        } else if (sir == null) {
            simPref.setSummary(R.string.sim_selection_required_pref);
        }

        boolean callStateIdle = isCallStateIdle();
        final boolean ecbMode = SystemProperties.getBoolean(
                TelephonyProperties.PROPERTY_INECM_MODE, false);
        // Enable data preference in msim mode and call state idle
        simPref.setEnabled((mSelectableSubInfos.size() > 1) && !disableDds()
                && callStateIdle && !ecbMode  && !isAirplaneModeOn() && !isSubsidyUnlocked());
    }

    private boolean isManualSubSelectRequired() {
        if (!isSubSidyLockFeatureEnabled()) {
            return true;
        }
        IExtTelephony extTelephony =
                IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
        boolean required = false;
        int primaryCarrierSlotCount = 0;
        for (int i = 0; i < mNumSlots; ++i) {
            try {
                if (extTelephony.isPrimaryCarrierSlotId(i)) {
                    primaryCarrierSlotCount = primaryCarrierSlotCount + 1;
                }
            } catch (RemoteException ex) {
                ex.printStackTrace();
            } catch (NullPointerException ex) {
                ex.printStackTrace();
            }
        }
        if (primaryCarrierSlotCount > 1) {
            required = true;
        }
        return required;
    }

    private void updateCallValues() {
        final Preference simPref = findPreference(KEY_CALLS);
        final TelecomManager telecomManager = TelecomManager.from(mContext);
        final PhoneAccountHandle phoneAccount =
            telecomManager.getUserSelectedOutgoingPhoneAccount();
        final List<PhoneAccountHandle> allPhoneAccounts =
            telecomManager.getCallCapablePhoneAccounts();

        simPref.setTitle(R.string.calls_title);
        if (phoneAccount == null) {
            simPref.setSummary(mContext.getResources().getString(
                    R.string.sim_calls_ask_first_prefs_title));
        } else {
            PhoneAccount phoneaccount = telecomManager
                    .getPhoneAccount(phoneAccount);
            if (phoneaccount != null) {
                simPref.setSummary((String) phoneaccount.getLabel());
            } else {
                simPref.setSummary(mContext.getResources().getString(
                        R.string.sim_calls_ask_first_prefs_title));
            }
        }
        simPref.setEnabled(allPhoneAccounts.size() > 1  && !isAirplaneModeOn());
    }

    private boolean isAirplaneModeOn() {
        return (Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) != 0);
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

        for (int i = 0; i < mSimCards.getPreferenceCount(); ++i) {
            Preference pref = mSimCards.getPreference(i);
            if (pref instanceof SimEnablerPreference) {
                // Calling cleanUp() here to dismiss/cleanup any pending dialog exists.
                ((SimEnablerPreference)pref).cleanUpPendingDialogs();
            }
        }
    }

    @Override
    public boolean onPreferenceTreeClick(final PreferenceScreen preferenceScreen,
            final Preference preference) {
        final Context context = mContext;
        Intent intent = new Intent(context, SimDialogActivity.class);

        if (preference instanceof SimPreference) {
/*            //Sim Icon Customisation feature change
            OperatorSimInfo operatorSimInfo = new OperatorSimInfo(context);SAND
            boolean isCustomSimFeatureEnabled = operatorSimInfo.
                    isOperatorFeatureEnabled();
            if (!isCustomSimFeatureEnabled) {*/
                Intent newIntent = new Intent(context, SimPreferenceDialog.class);
                newIntent.putExtra(EXTRA_SLOT_ID, ((SimPreference)preference).getSlotId());
                startActivity(newIntent);
//            }
        } else if (findPreference(KEY_CELLULAR_DATA) == preference) {
            if (isCallStateIdle()) {
                intent.putExtra(SimDialogActivity.DIALOG_TYPE_KEY, SimDialogActivity.DATA_PICK);
                context.startActivity(intent);
            }
        } else if (findPreference(KEY_CALLS) == preference) {
            intent.putExtra(SimDialogActivity.DIALOG_TYPE_KEY, SimDialogActivity.CALLS_PICK);
            context.startActivity(intent);
        } else if (findPreference(KEY_SMS) == preference) {
            intent.putExtra(SimDialogActivity.DIALOG_TYPE_KEY, SimDialogActivity.SMS_PICK);
            context.startActivity(intent);
        } else if (preference == mPrimarySubSelect) {
            Intent newIntent = new Intent(context, PrimarySubSetting.class);
            startActivity(newIntent);
        }

        return true;
    }
    private void loge(String msg) {
        Log.e(TAG + "message", msg);
    }

    private void simEnablerUpdate() {
        if (isAdded()) {
            updateAllOptions();
        } else {
            needUpdate = true;
        }
    }

    private class SimPreference extends Preference {
        SubscriptionInfo mSubInfoRecord;
        int mSlotId;
        Context mContext;
        public SimPreference(Context context, SubscriptionInfo subInfoRecord, int slotId) {
            super(context);

            mContext = context;
            mSubInfoRecord = subInfoRecord;
            mSlotId = slotId;
            setKey("sim" + mSlotId);
            update();
        }
        public SimPreference (Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
        }

        public void update() {
            final Resources res = mContext.getResources();

            setTitle(String.format(mContext.getResources()
                    .getString(R.string.sim_editor_title), (mSlotId + 1)));
            if (mSubInfoRecord != null) {
                if (TextUtils.isEmpty(getPhoneNumber(mSubInfoRecord))) {
                    setSummary(mSubInfoRecord.getDisplayName());
                } else {
                    setSummary(mSubInfoRecord.getDisplayName() + " - " +
                            getPhoneNumber(mSubInfoRecord));
                    setEnabled(true);
                }
                setIcon(new BitmapDrawable(res, (mSubInfoRecord.createIconBitmap(mContext))));
            } else {
                setSummary(R.string.sim_slot_empty);
                setFragment(null);
                setEnabled(false);
            }
        }

        private int getSlotId() {
            return mSlotId;
        }

        @Override
        protected void onAttachedToActivity() {
            super.onAttachedToActivity();
            if (needUpdate) {
                needUpdate = false;
                updateAllOptions();
            }
        }
    }

    // This is to show SIM Enable options on/off on UI for user selection.
    //  User can activate/de-activate through SIM on/off options.
    private class SimEnablerPreference extends SimPreference implements OnCheckedChangeListener {

        private String TAG = "SimEnablerPreference";
        private static final boolean DBG = true;

        private static final int EVT_UPDATE = 1;
        private static final int EVT_SHOW_RESULT_DLG = 2;
        private static final int EVT_SHOW_PROGRESS_DLG = 3;
        private static final int EVT_PROGRESS_DLG_TIME_OUT = 4;

        private static final int CONFIRM_ALERT_DLG_ID = 1;
        private static final int ERROR_ALERT_DLG_ID = 2;
        private static final int RESULT_ALERT_DLG_ID = 3;

        private static final int REQUEST_SUCCESS = 0;
        private static final int GENERIC_FAILURE = -1;
        private static final int INVALID_INPUT = -2;
        private static final int REQUEST_IN_PROGRESS = -3;



        private static final String DISPLAY_NUMBERS_TYPE = "display_numbers_type";

        private SubscriptionInfo mSir;
        private boolean mCurrentUiccProvisionState;
        private boolean mIsChecked;

        private boolean mCmdInProgress = false;
        private int mSwitchVisibility = View.VISIBLE;
        private CompoundButton mSwitch;
        //Delay for progress dialog to dismiss
        private static final int PROGRESS_DLG_TIME_OUT = 30000;
        private static final int MSG_DELAY_TIME = 2000;


        public SimEnablerPreference(Context context, SubscriptionInfo sir, int slotId) {
            super(context, (AttributeSet)null,
                    com.android.internal.R.attr.checkBoxPreferenceStyle);
            logd("Contructor..Enter");
            mContext = context;
            mSlotId = slotId;
            mSir = sir;
            mSubInfoRecord = sir;
            if (mContext.getResources().getBoolean(R.bool.config_custom_multi_sim_checkbox)) {
                setWidgetLayoutResource(R.layout.custom_sim_checkbox);
            } else {
                setWidgetLayoutResource(R.layout.custom_sim_switch);
            }

            setSwitchVisibility(View.VISIBLE);
            setKey("sim" + mSlotId);
            update();
        }

        private void sendMessage(int event, Handler handler, int delay) {
            Message message = handler.obtainMessage(event);
            handler.sendMessageDelayed(message, delay);
        }

        private void sendMessage(int event, Handler handler, int delay, int arg1, int arg2) {
            Message message = handler.obtainMessage(event, arg1, arg2);
            handler.sendMessageDelayed(message, delay);
        }

        private boolean hasCard() {
            return TelephonyManager.getDefault().hasIccCard(mSlotId);
        }

        private int getProvisionStatus(int slotId) {
            return mUiccProvisionStatus[slotId];
        }

        @Override
        protected void onBindView(View view) {
            super.onBindView(view);
            logd("onBindView....");
            mSwitch = (CompoundButton) view.findViewById(R.id.sub_switch_widget);
            mSwitch.setOnCheckedChangeListener(this);
            update();
            // now use other config screen to active/deactive sim card\
            mSwitch.setVisibility(mSwitchVisibility);

            // Disable manual provisioning option to user when
            // Subscription is not valid.
            if (!isCurrentSubValid()) {
                mSwitch.setEnabled(false);
            } else if (isSubSidyLockFeatureEnabled()
                        && isSubsidyUnlocked() && isCurrentSubValid()) {
                boolean isPrimaryCarrierSlot = false;
                try {
                    IExtTelephony extTelephony =
                            IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
                    isPrimaryCarrierSlot = extTelephony
                            .isPrimaryCarrierSlotId(mSir.getSimSlotIndex());
                } catch (RemoteException ex) {
                    ex.printStackTrace();
                } catch (NullPointerException ex) {
                    ex.printStackTrace();
                }
                mSwitch.setEnabled(!isPrimaryCarrierSlot);
            } else {
                mSwitch.setEnabled(true);
            }
        }

        @Override
        public void update() {
            final Resources res = mContext.getResources();
            logd("update()" + mSir);
            try {
                IExtTelephony extTelephony =
                        IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
                //get current provision state of the SIM.
                mUiccProvisionStatus[mSlotId] =
                        extTelephony.getCurrentUiccCardProvisioningStatus(mSlotId);
            } catch (RemoteException ex) {
                mUiccProvisionStatus[mSlotId] = INVALID_STATE;
                loge("Failed to get pref, slotId: "+ mSlotId +" Exception: " + ex);
            } catch (NullPointerException ex) {
                mUiccProvisionStatus[mSlotId] = INVALID_STATE;
                loge("Failed to get pref, slotId: "+ mSlotId +" Exception: " + ex);
            }

            boolean isSubValid = isCurrentSubValid();
            boolean enable = true;
            if (isSubSidyLockFeatureEnabled() && isSubsidyUnlocked() && isSubValid) {
                boolean isPrimaryCarrierSlotId = false;
                try {
                    IExtTelephony extTelephony =
                            IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
                    isPrimaryCarrierSlotId = extTelephony
                            .isPrimaryCarrierSlotId(mSir.getSimSlotIndex());
                } catch (RemoteException ex) {
                    ex.printStackTrace();
                } catch (NullPointerException ex) {
                    ex.printStackTrace();
                }
                enable = !isPrimaryCarrierSlotId;
            }
            setEnabled(isSubValid && enable);

            logd("update: isSubValid "  + isSubValid + " provision status["
                    + mSlotId + "] = " + mUiccProvisionStatus[mSlotId]);
            setTitle(res.getString(R.string.sim_card_number_title, mSlotId + 1));
            if (isSubValid) {
                updateSummary();
                setIcon(new BitmapDrawable(res, (mSir.createIconBitmap(mContext))));
            } else {
                setSummary(res.getString(R.string.sim_slot_empty));
            }
        }

        // This method returns true if SubScription record corresponds to this
        // Preference screen has a valid SIM and slot index/SubId.
        private boolean isCurrentSubValid() {
            boolean isSubValid = false;
            if (!isAirplaneModeOn() && hasCard()) {
                SubscriptionInfo sir = mSubscriptionManager.
                        getActiveSubscriptionInfoForSimSlotIndex(mSlotId);
                if (sir != null ) {
                    mSir = sir;
                    if (SubscriptionManager.isValidSubscriptionId(mSir.getSubscriptionId()) &&
                            mSir.getSimSlotIndex() >= 0 &&
                            getProvisionStatus(mSir.getSimSlotIndex()) >= 0) {
                        isSubValid = true;
                    }
                }
            }
            return isSubValid;
        }

        public void setSwitchVisibility (int visibility) {
            mSwitchVisibility = visibility;
        }

        // Based on the received SIM provision state this method
        // sets the check box on Sim Preference UI and updates new
        // state to mCurrentUiccProvisionState.
        private void setChecked(boolean uiccProvisionState) {
            logd("setChecked: uiccProvisionState " + uiccProvisionState + "sir:" + mSir);
            if (mSwitch != null) {
                mSwitch.setOnCheckedChangeListener(null);
                // Do not update update checkstatus again in progress
                if (!mCmdInProgress) {
                    mSwitch.setChecked(uiccProvisionState);
                }
                mSwitch.setOnCheckedChangeListener(this);
                mCurrentUiccProvisionState = uiccProvisionState;
            }
        }

        private void updateSummary() {
            Resources res = mContext.getResources();
            String summary;
            boolean isActivated = (getProvisionStatus(mSir.getSimSlotIndex()) == PROVISIONED);
            logd("updateSummary: subId " + mSir.getSubscriptionId() + " isActivated = "
                    + isActivated + " slot id = " + mSlotId);

            String displayName = mSir == null ? "SIM" : (String)mSir.getDisplayName();
            if (isActivated) {
                summary = displayName;
                if (!TextUtils.isEmpty(mSir.getNumber())) {
                    summary = displayName + " - " + mSir.getNumber();
                }
            } else {
                summary = displayName + mContext.getString(R.string.sim_enabler_summary,
                        res.getString(hasCard() ? R.string.sim_disabled : R.string.sim_missing));
            }

            setSummary(summary);
            setChecked(isActivated);
        }


        /**
        * get number of Subs provisioned on the device
        * @param context
        * @return
        */
        public int getNumOfSubsProvisioned() {
            int activeSubInfoCount = 0;
            List<SubscriptionInfo> subInfoLists =
                    mSubscriptionManager.getActiveSubscriptionInfoList();
            if (subInfoLists != null) {
                for (SubscriptionInfo subInfo : subInfoLists) {
                    if (getProvisionStatus(subInfo.getSimSlotIndex())
                            == PROVISIONED) activeSubInfoCount++;
                }
            }
            return activeSubInfoCount;
        }

        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            mIsChecked = isChecked;
            logd("onClick: " + isChecked);

            handleUserRequest();
        }

        // This internal method called when user changes preference from UI
        // 1. For activation/deactivation request from User, if device in APM mode
        //    OR if voice call active on any SIM it dispay error dialog and returns.
        // 2. For deactivation request it returns error dialog if only one SUB in
        //    active state.
        // 3. In other cases it sends user request to framework.
        synchronized private void handleUserRequest() {
            if (isAirplaneModeOn()) {
                // do nothing but warning
                logd("APM is on, EXIT!");
                showAlertDialog(ERROR_ALERT_DLG_ID, R.string.sim_enabler_airplane_on);
                return;
            }
            for (int i = 0; i < mPhoneCount; i++) {
                int[] subId = SubscriptionManager.getSubId(i);
                //when voice call in progress, subscription can't be activate/deactivate.
                if (subId != null && subId.length > 0) {
                    if (TelephonyManager.getDefault().getCallState(subId[0])
                        != TelephonyManager.CALL_STATE_IDLE) {
                        logd("Call state for phoneId: " + i + " is not idle, EXIT!");
                        showAlertDialog(ERROR_ALERT_DLG_ID, R.string.sim_enabler_in_call);
                        return;
                    }
                }
            }

            if (!mIsChecked) {
                if (getNumOfSubsProvisioned() > 1) {
                    logd("More than one sub is active, Deactivation possible.");
                    showAlertDialog(CONFIRM_ALERT_DLG_ID, 0);
                } else {
                    logd("Only one sub is active. Deactivation not possible.");
                    showAlertDialog(ERROR_ALERT_DLG_ID, R.string.sim_enabler_both_inactive);
                    return;
                }
            } else {
                logd("Activate the sub");
                sendUiccProvisioningRequest();
            }
        }

        private void sendUiccProvisioningRequest() {
            if (!mSwitch.isEnabled()) {
                return;
            }
            new SimEnablerDisabler().execute();
        }

        private class SimEnablerDisabler extends AsyncTask<Void, Void, Integer> {

            int newProvisionedState = NOT_PROVISIONED;

            @Override
            protected void onPreExecute() {
                super.onPreExecute();
                mCmdInProgress = true;
                showProgressDialog();
                setEnabled(false);
            }

            @Override
            protected Integer doInBackground(Void... params) {
                int result = -1;
                newProvisionedState = NOT_PROVISIONED;
                try {
                    IExtTelephony extTelephony =
                            IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
                    if (mIsChecked) {
                        result = extTelephony.activateUiccCard(mSir.getSimSlotIndex());
                        newProvisionedState = PROVISIONED;
                    } else {
                        result = extTelephony.deactivateUiccCard(mSir.getSimSlotIndex());
                    }
                } catch (RemoteException ex) {
                    loge("Activate  sub failed " + result + " phoneId " + mSir.getSimSlotIndex());
                } catch (NullPointerException ex) {
                    loge("Failed to activate sub Exception: " + ex);
                }
                return result;
            }

            @Override
            protected void onPostExecute(Integer result) {
                processSetUiccDone(result.intValue(), newProvisionedState);
            }
        }

        private void processSetUiccDone(int result, int newProvisionedState) {
            sendMessage(EVT_UPDATE, mHandler, MSG_DELAY_TIME);
            sendMessage(EVT_SHOW_RESULT_DLG, mHandler, MSG_DELAY_TIME, result, newProvisionedState);
            mCmdInProgress = false;
        }

        private void showAlertDialog(int dialogId, int msgId) {

            String title = mSir == null ? "SUB" : mSir.getDisplayName().toString();
            // Confirm only one AlertDialog instance to show.
            dismissDialog(mAlertDialog, false);
            dismissDialog(mProgressDialog, false);
            // Create dialog only if Fragment is visible
            if (isVisible()) {
                AlertDialog.Builder builder = new AlertDialog.Builder(
                        mContext).setIcon(
                        android.R.drawable.ic_dialog_alert).setTitle(
                        title);

                switch (dialogId) {
                case CONFIRM_ALERT_DLG_ID:
                    String message;
                    if (mContext.getResources().getBoolean(
                            R.bool.confirm_to_switch_data_service)) {
                        if (SubscriptionManager.getDefaultDataSubscriptionId() == mSir
                                .getSubscriptionId()) {
                            message = mContext
                                    .getString(
                                            R.string.sim_enabler_need_switch_data_service,
                                            getProvisionedSlotId(mContext));
                        } else {
                            message = mContext
                                    .getString(R.string.sim_enabler_need_disable_sim);
                        }
                        builder.setTitle(R.string.sim_enabler_will_disable_sim_title);
                    } else {
                        message = mContext
                                .getString(R.string.sim_enabler_need_disable_sim);
                    }
                    builder.setMessage(message);
                    builder.setPositiveButton(android.R.string.ok,
                            mDialogClickListener);
                    builder.setNegativeButton(android.R.string.no,
                            mDialogClickListener);
                    builder.setOnCancelListener(mDialogCanceListener);
                    break;

                case ERROR_ALERT_DLG_ID:
                    builder.setMessage(mContext.getString(msgId));
                    builder.setNeutralButton(android.R.string.ok,
                            mDialogClickListener);
                    builder.setCancelable(false);
                    break;

                case RESULT_ALERT_DLG_ID:
                    String msg = mCurrentUiccProvisionState ? mContext
                            .getString(R.string.sub_activate_success)
                            : mContext
                                    .getString(R.string.sub_deactivate_success);
                    builder.setMessage(msg);
                    builder.setNeutralButton(android.R.string.ok, null);
                    break;
                default:
                    break;
                }

                mAlertDialog = builder.create();
                mAlertDialog.setCanceledOnTouchOutside(false);
                mAlertDialog.show();
            }
        }

        private int getProvisionedSlotId(Context context) {
            int activeSlotId = -1;
            List<SubscriptionInfo> subInfoLists =
                    mSubscriptionManager.getActiveSubscriptionInfoList();
            if (subInfoLists != null) {
                for (SubscriptionInfo subInfo : subInfoLists) {
                    if (getProvisionStatus(subInfo.getSimSlotIndex()) == PROVISIONED
                            && subInfo.getSubscriptionId() != mSir.getSubscriptionId())
                        activeSlotId = subInfo.getSimSlotIndex() + 1;
                }
            }
            return activeSlotId;
        }

        private void showProgressDialog() {
            String title = mSir == null ? "SUB" : mSir.getDisplayName().toString();

            String msg = mContext.getString(mIsChecked ? R.string.sim_enabler_enabling
                    : R.string.sim_enabler_disabling);
            dismissDialog(mProgressDialog, false);
            // Create dialog only if Fragment is visible
            if (isVisible()) {
                mProgressDialog = new ProgressDialog(mContext);
                mProgressDialog.setIndeterminate(true);
                mProgressDialog.setTitle(title);
                mProgressDialog.setMessage(msg);
                mProgressDialog.setCancelable(false);
                mProgressDialog.setCanceledOnTouchOutside(false);
                mProgressDialog.show();

                sendMessage(EVT_PROGRESS_DLG_TIME_OUT, mHandler,
                        PROGRESS_DLG_TIME_OUT);
            }
        }

        private void dismissDialog(Dialog dialog, boolean updateRequired) {
            if((dialog != null) && (dialog.isShowing())) {
                if (updateRequired) {
                    update();
                }
                dialog.dismiss();
                dialog = null;
            }
        }

        public void cleanUpPendingDialogs() {
            dismissDialog(mProgressDialog, false);
            dismissDialog(mAlertDialog, true);
        }

        private DialogInterface.OnClickListener mDialogClickListener = new DialogInterface
                .OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        if (which == DialogInterface.BUTTON_POSITIVE) {
                            dismissDialog(mAlertDialog, false);
                            sendUiccProvisioningRequest();
                        } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                            update();
                        } else if (which == DialogInterface.BUTTON_NEUTRAL) {
                            update();
                        }
                    }
                };

        private DialogInterface.OnCancelListener mDialogCanceListener = new DialogInterface
                .OnCancelListener() {
                    public void onCancel(DialogInterface dialog) {
                        update();
                    }
                };


        private Handler mHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {

                    switch(msg.what) {
                        case EVT_UPDATE:
                            simEnablerUpdate();

                        case EVT_SHOW_RESULT_DLG:
                            int result = msg.arg1;
                            int newProvisionedState = msg.arg2;
                            logd("EVT_SHOW_RESULT_DLG result: " + result +
                                    " new provisioned state " + newProvisionedState);
                            update();
                            if (result != REQUEST_SUCCESS) {
                                int msgId = (newProvisionedState == PROVISIONED) ?
                                        R.string.sub_activate_failed :
                                        R.string.sub_deactivate_failed;
                                showAlertDialog(ERROR_ALERT_DLG_ID, msgId);
                            } else {
                                showAlertDialog(RESULT_ALERT_DLG_ID, 0);
                            }
                            mHandler.removeMessages(EVT_PROGRESS_DLG_TIME_OUT);
                            break;

                        case EVT_SHOW_PROGRESS_DLG:
                            logd("EVT_SHOW_PROGRESS_DLG");
                            showProgressDialog();
                            break;

                        case EVT_PROGRESS_DLG_TIME_OUT:
                            logd("EVT_PROGRESS_DLG_TIME_OUT");
                            dismissDialog(mProgressDialog, false);
                            // Must update UI when time out
                            update();
                            break;

                        default:
                        break;
                    }
                }
            };

        private void logd(String msg) {
            if (DBG) Log.d(TAG + "(" + mSlotId + ")", msg);
        }

        private void loge(String msg) {
            Log.e(TAG + "(" + mSlotId + ")", msg);
        }
    }

    // Returns the line1Number. Line1number should always be read from TelephonyManager since it can
    // be overridden for display purposes.
    private String getPhoneNumber(SubscriptionInfo info) {
        final TelephonyManager tm =
            (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        return tm.getLine1Number(info.getSubscriptionId());
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
            } else if (ACTION_OUTGOING_PHONE_ACCOUNT_CHANGED.equals(action)) {
                // When default outgoing phone account changed
                // refresh voice call preference
                updateCallValues();
            } else if (Intent.ACTION_AIRPLANE_MODE_CHANGED.equals(action)) {
                updateSubscriptions();
                Log.d(TAG, "Intent received: isAirplaneModeOn()?  " + isAirplaneModeOn());
            }
        }
    };

    // When primarycard feature enabled this provides menu option for user
    // to view/select current primary slot.
    private void initLTEPreference() {
        boolean isPrimarySubFeatureEnable =
                SystemProperties.getBoolean("persist.radio.primarycard", false);
        boolean primarySetable = Settings.Global.getInt(mContext.getContentResolver(),
                 CONFIG_PRIMARY_SUB_IS_SETABLE, 0) == 1;

        log("isPrimarySubFeatureEnable :" + isPrimarySubFeatureEnable +
                " primarySetable :" + primarySetable +
                " mPrimaryPrefRemoved = " + mPrimaryPrefRemoved);

        if (!isPrimarySubFeatureEnable || !primarySetable) {
            final PreferenceCategory simActivities =
                    (PreferenceCategory) findPreference(SIM_ACTIVITIES_CATEGORY);
            if (!mPrimaryPrefRemoved) {
                simActivities.removePreference(mPrimarySubSelect);
                mPrimaryPrefRemoved = true;
            }
            return;
        } else if (mPrimaryPrefRemoved == true) {
            final PreferenceCategory simActivities =
                    (PreferenceCategory) findPreference(SIM_ACTIVITIES_CATEGORY);
            simActivities.addPreference(mPrimarySubSelect);
            mPrimaryPrefRemoved = false;
        }
        int currentPrimarySlot = Settings.Global.getInt(mContext.getContentResolver(),
                 CONFIG_CURRENT_PRIMARY_SUB, SubscriptionManager.INVALID_SIM_SLOT_INDEX);
        log("init LTE primary slot : " + currentPrimarySlot);

        if (SubscriptionManager.isValidSlotIndex(currentPrimarySlot)) {
            final SubscriptionInfo subInfo = mSubscriptionManager
                    .getActiveSubscriptionInfoForSimSlotIndex(currentPrimarySlot);
            CharSequence lteSummary = (subInfo == null ) ? null : subInfo.getDisplayName();
            mPrimarySubSelect.setSummary(lteSummary);
        } else {
            mPrimarySubSelect.setSummary("");
        }
        updatePrimarySub();
    }

    private void updatePrimarySub() {
        if (!mPrimaryPrefRemoved) {
            boolean isManualMode = Settings.Global.getInt(mContext.getContentResolver(),
                    CONFIG_SUB_SELECT_MODE_MANUAL, 0) == 1;
            log("updatePrimarySub isManualMode :" + isManualMode);
            mPrimarySubSelect.setEnabled(isManualMode && mSelectableSubInfos.size() > 1 &&
                    isCallStateIdle() && !isAirplaneModeOn()
                    && (isManualSubSelectRequired() || !isSubsidyUnlocked()));
        }
    }

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
                updatePrimarySub();
            }
        };
        return mPhoneStateListener[phoneId];
    }

    private boolean isSubsidyUnlocked() {
        int subsidyStatus  = Settings.Secure.getInt(
                mContext.getContentResolver(),
                SUBSIDY_LOCK_SETTINGS, AP_LOCKED);
        boolean subsidyUnLocked = (subsidyStatus == SUBSIDY_UNLOCKED);
        return isSubSidyLockFeatureEnabled() && subsidyUnLocked;
    }

    public static boolean isPermanentlyUnlocked(Context context) {
        int unlocked  = Settings.Secure.getInt(
                context.getContentResolver(),
                SUBSIDY_LOCK_SETTINGS, AP_LOCKED);
        boolean permanentlyUnlocked = (unlocked == PERMANENTLY_UNLOCKED);
        return isSubSidyLockFeatureEnabled() && permanentlyUnlocked;
    }

    public static boolean isSubSidyLockFeatureEnabled() {
        int prop = SystemProperties.getInt(SUBSIDY_LOCK_SYSTEM_PROPERY, 0);
        return (prop == 1);
    }

    private boolean isCallStateIdle() {
        boolean callStateIdle = true;
        for (int i = 0; i < mCallState.length; i++) {
            int[] subId = SubscriptionManager.getSubId(i);
            if (subId != null && subId.length > 0) {
                mCallState[i] = TelephonyManager.getDefault().getCallState(subId[0]);
                if (TelephonyManager.CALL_STATE_IDLE != mCallState[i]) {
                    callStateIdle = false;
                }
            }
        }
        Log.d(TAG, "isCallStateIdle " + callStateIdle);
        return callStateIdle;
    }
}
