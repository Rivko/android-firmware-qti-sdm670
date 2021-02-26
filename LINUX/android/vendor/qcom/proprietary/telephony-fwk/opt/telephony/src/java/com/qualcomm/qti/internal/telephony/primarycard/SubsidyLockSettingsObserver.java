/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.internal.telephony.primarycard;

import java.util.HashSet;
import java.util.Set;

import org.codeaurora.internal.IExtTelephony;

import android.telephony.Rlog;
import android.content.ContentResolver;
import android.content.Context;
import android.content.SharedPreferences;
import android.database.ContentObserver;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.qualcomm.qti.internal.telephony.QtiUiccCardProvisioner;

import java.util.List;

/**
 * SubsidyLockSettingsObserver, registers a ContentObserver for
 * the subsidy lock settings changes.
 *
 */
public class SubsidyLockSettingsObserver extends ContentObserver {

    private static final String TAG = "SubsidyLockSettingsObserver";

    private Context mContext;

    public static final int PERMANENTLY_UNLOCKED = 100;
    public static final int SUBSIDY_LOCKED = 101;
    public static final int AP_LOCKED = 102;
    public static final int SUBSIDY_UNLOCKED = 103;

    private static final String SUBSIDY_LOCK_SETTINGS = "subsidy_status";
    private static final String SUBSIDY_LOCK_SYSTEM_PROPERY
            = "ro.radio.subsidylock";
    private static int mCurrentState = -1;
    private static final String SUBSIDY_STATE_KEY = "device_Subsidy_state";

    private SubsidySettingsHandler mSettingsHandler;
    private QtiCardInfoManager mCardInfoManager;
    private QtiPrimaryCardPriorityHandler mPriorityHandler;

    public SubsidyLockSettingsObserver(Context context) {
        super(null);
        mContext = context;
        mCurrentState = PreferenceManager.getDefaultSharedPreferences(mContext).getInt(
                SUBSIDY_STATE_KEY, mCurrentState);
        Rlog.d(TAG, " in constructor, context =  " + mContext + " device state " + mCurrentState);
    }

    public void observe(QtiCardInfoManager cardInfoManager,
            QtiPrimaryCardPriorityHandler cardPriorityHandler) {
        ContentResolver resolver = mContext.getContentResolver();
        mCardInfoManager = cardInfoManager;
        mPriorityHandler = cardPriorityHandler;
        resolver.registerContentObserver(
                Settings.Secure.getUriFor(SUBSIDY_LOCK_SETTINGS), false, this);
        registerAllCardsAvailableCallback();
    }

    private void registerAllCardsAvailableCallback() {
        if (mSettingsHandler == null) {
            mSettingsHandler = new SubsidySettingsHandler(Looper.getMainLooper());
            mCardInfoManager.registerAllCardsInfoAvailable(mSettingsHandler,
                    SubsidySettingsHandler.MSG_ALL_CARDS_READY, null);
        }
    }

    @Override
    public void onChange(boolean selfChange) {
        if (mPriorityHandler != null) {
            mPriorityHandler.reloadPriorityConfig();
            mPriorityHandler.loadCurrentPriorityConfigs(true);
        }
        HandlerThread thread = new HandlerThread("Subsidy Settings handler thread");
        thread.start();
        Handler handler = new SubsidySettingsHandler(thread.getLooper());
        if (isSubsidyLocked(mContext)) {
            handler.obtainMessage(SubsidySettingsHandler.MSG_LOCKED).sendToTarget();
        } else if (isPermanentlyUnlocked(mContext)) {
            handler.obtainMessage(SubsidySettingsHandler.MSG_UNLOCKED).sendToTarget();
        } else if(isSubsidyUnlocked(mContext)) {
            handler.obtainMessage(
                    SubsidySettingsHandler.MSG_RESTRICTED, SUBSIDY_UNLOCKED).sendToTarget();
        }
    }

    public static boolean isSubsidyLocked(Context context) {
        int subsidyLock = Settings.Secure.getInt(
                context.getContentResolver(),
                SUBSIDY_LOCK_SETTINGS, -1);
        return subsidyLock == SUBSIDY_LOCKED || subsidyLock == AP_LOCKED;
    }

    public static boolean isSubsidyUnlocked(Context context) {
        int subsidyLock = Settings.Secure.getInt(
                context.getContentResolver(),
                SUBSIDY_LOCK_SETTINGS, -1);
        return subsidyLock == SUBSIDY_UNLOCKED;
    }

    public static boolean isPermanentlyUnlocked(Context context) {
        int subsidyLock = Settings.Secure.getInt(
                context.getContentResolver(),
                SUBSIDY_LOCK_SETTINGS, -1);
        return subsidyLock == PERMANENTLY_UNLOCKED;
    }

    private class SIMDeactivationRecords {
        private static final String PREFS_NAME = "deactivation_record";
        private static final String KEY_DEACTIVATION_RECORD = "key_deactivation_record";

        private Set<String> mSimRecords = null;
        private SharedPreferences mSharedPrefs;

        public SIMDeactivationRecords() {
            mSharedPrefs = mContext.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
            parse();
        }

        private void parse() {
            Set<String> simRecords =
                    mSharedPrefs.getStringSet(KEY_DEACTIVATION_RECORD, new HashSet<String>());
            mSimRecords = new HashSet<String>();
            mSimRecords.addAll(simRecords);
        }

        public void addRecord(String subId) {
            if (isDeactivated(subId)) return;
            mSimRecords.add(subId);
            mSharedPrefs.edit().putStringSet(KEY_DEACTIVATION_RECORD, mSimRecords).commit();
        }

        public void removeRecord(String subId) {
            if (mSimRecords.contains(subId)) {
                mSimRecords.remove(subId);
            }
            mSharedPrefs.edit().putStringSet(KEY_DEACTIVATION_RECORD, mSimRecords).commit();
        }

        public boolean isDeactivated(String subId) {
            return mSimRecords.contains(subId);
        }

    }

    private class SubsidySettingsHandler extends Handler {

        static final int SUCCESS = 0;

        static final int PROVISIONED = 1;

        public static final int MSG_LOCKED = 0;
        public static final int MSG_RESTRICTED = 1;
        public static final int MSG_UNLOCKED = 2;
        public static final int MSG_EXIT = 3;
        public static final int MSG_ALL_CARDS_READY = 4;
        public static final int MSG_SET_PRIMARY_CARD = 5;

        private int mNumSimSlots = 0;

        public SubsidySettingsHandler(Looper looper) {
            super(looper);
            mNumSimSlots = TelephonyManager.getDefault().getPhoneCount();
        }

        @Override
        public void handleMessage(Message msg) {
            Rlog.d(TAG, " handleMessage, event  " + msg.what + " current state " + mCurrentState);
            switch (msg.what) {
                case MSG_ALL_CARDS_READY:
                    onChange(false);
                    break;
                case MSG_LOCKED:
                    final IExtTelephony mExtTelephony = IExtTelephony.Stub.asInterface(
                            ServiceManager.getService("extphone"));
                    try {
                        for (int i = 0; i < mNumSimSlots; i++) {
                            final SubscriptionInfo sir = SubscriptionManager.from(mContext)
                                    .getActiveSubscriptionInfoForSimSlotIndex(i);
                            if (sir == null || (sir.getMcc() == 0)) {
                                Rlog.e(TAG, "Invalid sub info for slot id: "+ i
                                        + ", not proceeding further.");
                                continue;
                            }
                            if (mExtTelephony.isPrimaryCarrierSlotId(i)) {
                                QtiUiccCardProvisioner.make(mContext).activateUiccCard(i);
                            } else {
                                if (mExtTelephony.getCurrentUiccCardProvisioningStatus(i)
                                        == PROVISIONED) {
                                    int result = QtiUiccCardProvisioner.make(mContext)
                                            .deactivateUiccCard(i);
                                    if (result == SUCCESS) {
                                        new SIMDeactivationRecords()
                                                .addRecord(String.valueOf(sir.getSubscriptionId()));
                                    }
                                }
                            }
                        }
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }
                    updateDeviceState(AP_LOCKED);
                    obtainMessage(MSG_EXIT).sendToTarget();
                    break;
                case MSG_RESTRICTED:
                    final IExtTelephony mExtTelephony1 = IExtTelephony.Stub.asInterface(
                            ServiceManager.getService("extphone"));
                    try {
                        for (int i = 0; i < mNumSimSlots; i++) {
                            final SubscriptionInfo sir = SubscriptionManager.from(mContext)
                                    .getActiveSubscriptionInfoForSimSlotIndex(i);
                            if (sir == null || (sir.getMcc() == 0)) {
                                Rlog.e(TAG, "Invalid subscription info for slot id: "+ i
                                        + ", not proceeding further.");
                                continue;
                            }
                            if (!mExtTelephony1.isPrimaryCarrierSlotId(i)) {
                                SIMDeactivationRecords records = new SIMDeactivationRecords();
                                if (records.isDeactivated(
                                        String.valueOf(sir.getSubscriptionId()))) {
                                    int result = mExtTelephony1.activateUiccCard(i);
                                    if (result == SUCCESS) {
                                        records.removeRecord(
                                                String.valueOf(sir.getSubscriptionId()));
                                    }
                                }
                            } else if (isSubsidyUnlocked(mContext)
                                        && mExtTelephony1.isPrimaryCarrierSlotId(i)) {
                                QtiUiccCardProvisioner.make(mContext).activateUiccCard(i);
                            }
                        }
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                    }

                    if (msg.obj != null) {
                        updateDeviceState((Integer) msg.obj);
                    }
                    obtainMessage(MSG_EXIT).sendToTarget();
                    break;
                case MSG_UNLOCKED:
                    obtainMessage(MSG_RESTRICTED, PERMANENTLY_UNLOCKED).sendToTarget();
                    break;
                case MSG_EXIT:
                    if (getLooper() != Looper.getMainLooper()) {
                        getLooper().quitSafely();
                    }
                    break;
                case MSG_SET_PRIMARY_CARD:
                    QtiPrimaryCardController.getInstance().trySetPrimarySub();
                    break;
            }
        }
    }

    private void updateDeviceState(int newState) {
        if (mCurrentState != newState) {
            Rlog.d(TAG, " updateDeviceState, new state  " + newState);

            if ((newState == PERMANENTLY_UNLOCKED) || (mCurrentState == PERMANENTLY_UNLOCKED)) {
                // 1. Re-load IIN config when device state changes from
                // APP_LOCKED/RESTRICTED to UNLOCKED and vice versa.
                // 2. When device factory powered-up with UNLOCKED
                // state perform re-load.
                if (mPriorityHandler != null) {
                    mPriorityHandler.reloadPriorityConfig();
                    mPriorityHandler.loadCurrentPriorityConfigs(true);
                }
            }
            if ((mCurrentState == PERMANENTLY_UNLOCKED) &&
                    (newState == AP_LOCKED || newState == SUBSIDY_UNLOCKED)) {
                // when device state changes from UNLOCKED to APP_LOCKED/RESTRICTED, need
                // to run Primary card algorithm.
                mSettingsHandler.obtainMessage(
                        SubsidySettingsHandler.MSG_SET_PRIMARY_CARD).sendToTarget();
            } else if (newState == PERMANENTLY_UNLOCKED) {
                QtiPrimaryCardController.getInstance().saveUserSelectionMode();
            }
            saveDeviceState(newState);
        }
    }

    private void saveDeviceState(int newState) {
        mCurrentState = newState;
        PreferenceManager.getDefaultSharedPreferences(mContext).edit()
                .putInt(SUBSIDY_STATE_KEY, newState).commit();
    }

    public static boolean isSubsidyLockFeatureEnabled() {
        int propVal = SystemProperties.getInt(SUBSIDY_LOCK_SYSTEM_PROPERY, 0);
        return (propVal == 1);
    }

}
