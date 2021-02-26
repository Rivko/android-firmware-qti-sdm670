/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.simsettings;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout.LayoutParams;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.PhoneConstants;
import org.codeaurora.internal.IExtTelephony;

public class PrimarySubSetting extends Activity implements View.OnClickListener {
    private static final String TAG = "PrimarySubSetting";
    private TextView mRecognizeText;
    private RadioGroup mRadioGroup;
    private Button mOKbutton;
    private CheckBox mDdsChecBox;
    private ProgressDialog mProgressDialog;
    private Context mContext;

    private static int mPhoneCount = TelephonyManager.getDefault().getPhoneCount();
    private int mUserSelPrimarySlot = -1;
    private boolean mPrimaryCardRequestSent = false;
    private SubsidySettingsObserver mSubsidySettingsObserver;

    private static final int SET_LTE_SUB_MSG = 1;
    private static final int CARDSTATE_ABSENT = 2;
    private static final String CONFIG_ACTION = "codeaurora.intent.action.ACTION_LTE_CONFIGURE";

    private static final String ACTION_SET_PRIMARY_CARD_DONE =
            "org.codeaurora.intent.action.ACTION_SET_PRIMARY_CARD_DONE";
    private static final String PRIMARY_CARD_RESULT = "result";
    private static final String SUBSIDY_LOCK_SETTINGS = "subsidy_status";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.primary_sub_select);
        mContext = getApplication();
        mRecognizeText = (TextView) findViewById(R.id.recognize_text);
        mRadioGroup = (RadioGroup) findViewById(R.id.radiogroup);
        for (int i = 0; i < mPhoneCount; i++) {
            RadioButton radioButton = new RadioButton(this);
            mRadioGroup.addView(radioButton, new LayoutParams(LayoutParams.WRAP_CONTENT,
                    LayoutParams.WRAP_CONTENT));
            radioButton.setEnabled(getPrimaryCardSetable());
            radioButton.setTag(i);
            radioButton.setText(getSimName(i));
            radioButton.setOnClickListener(this);
        }

        if (!CONFIG_ACTION.equals(getIntent().getAction())) {
            setTitle(R.string.lte_select_title);
            mRecognizeText.setVisibility(View.GONE);
        } else {
            setTitle(R.string.lte_recognition_title);
        }

        mOKbutton = (Button) findViewById(R.id.select_ok_btn);
        mOKbutton.setOnClickListener(this);

        mProgressDialog = new ProgressDialog(this);
        mProgressDialog.setMessage(this.getString(R.string.lte_setting));
        mProgressDialog.setCancelable(false);

        logd("action= " + getIntent().getAction() + " Can enable UI? " + getPrimaryCardSetable());
        if (SimSettings.isSubSidyLockFeatureEnabled()) {
            mSubsidySettingsObserver = new SubsidySettingsObserver();
            getContentResolver().registerContentObserver(
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
            boolean unlocked = SimSettings
                    .isPermanentlyUnlocked(getApplicationContext());
            if (!unlocked) {
                logd("SubsidySettingsHandler: finishing PrimarySubSetting Activity ");
                finish();
            }
        }
    };

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "Intent received: " + action);
            if (ACTION_SET_PRIMARY_CARD_DONE.equals(action)) {
                int primarySlot = intent.getIntExtra(PhoneConstants.PHONE_KEY,
                        SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                int success = intent.getIntExtra(PRIMARY_CARD_RESULT, 0);
                Log.d(TAG, "Received ACTION_SET_PRIMARY_CARD_DONE for primarySlot: "
                        + primarySlot + " SUCCESS :" + success);
                if (mPrimaryCardRequestSent) {
                    mHandler.sendMessage(mHandler.obtainMessage(
                            SET_LTE_SUB_MSG, primarySlot, success));
                    mPrimaryCardRequestSent = false;
                }
            }
        }
    };

    @Override
    protected void onResume() {
        super.onResume();
        int primarySlot = getPrimarySlotFromDB();
        if (getPrimaryCardSetable()) {
            if (primarySlot != -1) {
                mOKbutton.setTag(primarySlot);
            } else {
                mOKbutton.setEnabled(false);
            }
        } else {
            mOKbutton.setEnabled(false);
        }
        logd(" onResume, primary slot " + primarySlot);
        getApplicationContext().registerReceiver(mSimStateReceiver,
               new IntentFilter(TelephonyIntents.ACTION_SIM_STATE_CHANGED));
        updateState();
    }

    @Override
    protected void onPause() {
        super.onPause();
        getApplicationContext().unregisterReceiver(mSimStateReceiver);
    }

    @Override
    protected void onDestroy() {
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            mProgressDialog.dismiss();
        }
        if (mSubsidySettingsObserver != null) {
            getContentResolver().unregisterContentObserver(mSubsidySettingsObserver);
        }
        super.onDestroy();
    }

    private BroadcastReceiver mSimStateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            logd("Recieved intent " + action);
            if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(action)) {
                final String stateExtra = intent
                        .getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                logd("Intent action " + stateExtra);
                if (IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(stateExtra)) {
                    for (int i = 0; i < mRadioGroup.getChildCount(); i++) {
                        RadioButton radioButton = (RadioButton) mRadioGroup.getChildAt(i);
                        final String currentSpn = radioButton.getText().toString();
                        final String newSpn = getSimName(i);
                        logd("current spn " + currentSpn + " new spn " + newSpn);
                        if (!currentSpn.equals(newSpn)) {
                            radioButton.setText(newSpn);
                        }
                    }
                } else if (IccCardConstants.INTENT_VALUE_ICC_ABSENT.equals(stateExtra)) {
                    logd("CARDSTATE_ABSENT");
                    mHandler.sendMessage(mHandler.obtainMessage(CARDSTATE_ABSENT));
                }
            }
        }
    };

    private void updateState() {
        int primarySlot = getPrimarySlotFromDB();
        mRadioGroup.clearCheck();
        for (int i = 0; i < mRadioGroup.getChildCount(); i++) {
            RadioButton radioButton = (RadioButton) mRadioGroup.getChildAt(i);
            radioButton.setChecked(primarySlot == i);
        }
        if (!getPrimaryCardSetable()) {
            Toast.makeText(this, getString(R.string.lte_switch_unavailable),
                   Toast.LENGTH_LONG).show();
        }
        logd("is primary sub enabled " + getPrimaryCardSetable() + " primaryslot = " + primarySlot);
    }

    @Override
    public boolean onNavigateUp() {
        finish();
        return true;
    }

    public void onClick(View v) {
        if (v instanceof RadioButton) {
            int slot = (Integer) v.getTag();
            mOKbutton.setTag(slot);
            mOKbutton.setEnabled(true);
        } else if (v == mOKbutton) {
            int slotId = (Integer) mOKbutton.getTag();
            int targetSub = (Integer) mOKbutton.getTag();
            android.util.Log.d(TAG, "Set primary card to : " + targetSub);
            try {
                IntentFilter intentFilter = new IntentFilter(ACTION_SET_PRIMARY_CARD_DONE);
                mContext.registerReceiver(mReceiver, intentFilter);
                IExtTelephony extTelephony =
                        IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
                extTelephony.setPrimaryCardOnSlot(targetSub);
                mPrimaryCardRequestSent = true;
                mProgressDialog.show();
            } catch (RemoteException ex) {
                logd("RemoteException @setPrimaryCardOnSlot" + ex);
            } catch (NullPointerException ex) {
                logd("NullPointerException @setPrimaryCardOnSlot" + ex);
            }
        }
    }

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case SET_LTE_SUB_MSG:
                    int targetSub = (Integer) mOKbutton.getTag();
                    int primarySlot = msg.arg1;
                    int success = msg.arg2;
                    logd(" primary slot = " + primarySlot + " user set sub = " + targetSub +
                            " success = " + success);
                    mContext.unregisterReceiver(mReceiver);
                    if (targetSub != primarySlot || success != 1) {
                        showFailedDialog(targetSub);
                        updateState();
                        if (mProgressDialog != null && mProgressDialog.isShowing() &&
                                !isFinishing() && !isDestroyed()) {
                            mProgressDialog.dismiss();
                        }
                    } else {
                        //Set Primary card is completed Successfully, save the value to DB and exit
                        savePrimarySlotToDB(primarySlot);
                        updateState();
                        if (mProgressDialog != null && mProgressDialog.isShowing() &&
                                !isFinishing() && !isDestroyed()) {
                            mProgressDialog.dismiss();
                        }
                        finish();
                    }
                    break;
                case CARDSTATE_ABSENT:
                    if (mProgressDialog != null && mProgressDialog.isShowing() &&
                            !isFinishing() && !isDestroyed()) {
                        mProgressDialog.dismiss();
                    }
                    finish();
                    break;
                default:
                    break;
            }
        }
    };

    private int getPrimarySlotFromDB() {
        return Settings.Global.getInt(mContext.getContentResolver(), SimSettings
                .CONFIG_CURRENT_PRIMARY_SUB, SubscriptionManager.INVALID_SIM_SLOT_INDEX);
    }

    // This method saves current configured primary slot to DB.
    public void savePrimarySlotToDB(int primarySlot) {
        Settings.Global.putInt(mContext.getContentResolver(), SimSettings
                .CONFIG_CURRENT_PRIMARY_SUB, primarySlot);
    }

    private boolean getPrimaryCardSetable() {
        boolean isPrimarySubFeatureEnable =
                        SystemProperties.getBoolean("persist.radio.primarycard", false);
        boolean isManualMode = Settings.Global.getInt(mContext.getContentResolver(),
                 SimSettings.CONFIG_SUB_SELECT_MODE_MANUAL, 0) == 1;

        return isPrimarySubFeatureEnable && isManualMode && mPhoneCount > 1;
    }

    private String getSimName(int slot) {
        SubscriptionInfo subInfo = SubscriptionManager.from(mContext)
                            .getActiveSubscriptionInfoForSimSlotIndex(slot);
        String simName = (subInfo == null ) ? mContext.getResources().getString(
                R.string.sim_card_number_title,slot + 1) : subInfo.getDisplayName().toString();
        return simName;
    }

    private void showFailedDialog(int sub) {
        if (!CONFIG_ACTION.equals(getIntent().getAction())) {
            AlertDialog alertDialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.reg_failed)
                    .setMessage(getString(R.string.reg_failed_msg, getSimName(sub)))
                    .setNeutralButton(R.string.select_ok, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    })
                    .create();
            alertDialog.show();
        } else {
            AlertDialog alertDialog = new AlertDialog.Builder(this)
                    .setTitle(R.string.reg_failed)
                    .setMessage(getString(R.string.reg_failed_msg, getSimName(sub)))
                    .setNeutralButton(R.string.lte_set, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            String action = "com.android.settings.sim.SIM_SUB_INFO_SETTINGS";
                            Intent intent = new Intent(action);
                            startActivity(intent);
                            finish();
                        }
                    })
                    .setNegativeButton(R.string.select_cancel,
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int which) {
                                    finish();
                                }
                            })
                    .create();
            alertDialog.show();
        }
    }

    private void logd(String message) {
        Log.d(TAG, message);
    }
}
