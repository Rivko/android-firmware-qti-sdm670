/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


package com.qualcomm.qti.qs.tiles;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.os.Handler;
import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;

import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;

import com.qualcomm.qti.qs.R;
import com.qualcomm.qti.qs.tiles.QSTile;

public class RoamingTile extends QSTile {
    private final RoamingObserver mRoamingObserver;
    private boolean mIsForeignState = false;
    private final String TAG = "RoamingTile";
    private final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);
    private int mDataPhoneId = PHONE_ID1;

    public RoamingTile(Context context, String action) {
        super(context, action);
        mRoamingObserver = new RoamingObserver(mHandler);
        mRoamingObserver.startObserving();
        final IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SERVICE_STATE_CHANGED);
        mContext.registerReceiver(mReceiver, filter);
    }

    private void setEnabled(boolean enabled) {
        setDataRoaming(enabled, mDataPhoneId);
    }

    @Override
    public void handleClick() {
        setEnabled(!getDataRoaming(mDataPhoneId) && hasIccCard());
    }

    @Override
    public void handleUpdateState() {
        if (TelephonyManager.getDefault().isMultiSimEnabled()) {
            mDataPhoneId = SubscriptionManager.getPhoneId(
                    SubscriptionManager.getDefaultDataSubscriptionId());
        }
        final boolean roaming = getDataRoaming(mDataPhoneId);
        Intent intent = new Intent(mAction);
        intent.putExtra(EXTRA_VISIBLE, true);
        intent.putExtra(EXTRA_ICON_PACKAGE,PACKAGE_NAME);
        intent.putExtra(EXTRA_LABEL, mContext.getString(
                  R.string.quick_settings_roaming_label));
        intent.putExtra(EXTRA_ON_CLICK_PENDING_INTENT, mOnClickIntent);
        if (roaming && !isAirplaneModeOn() && hasIccCard()) {
                intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_roaming_on);
                intent.putExtra(EXTRA_CONTENT_DESCRIPTION, mContext.getString(
                    R.string.accessibility_quick_settings_roaming_on));
        } else {
            intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_roaming_off);
            intent.putExtra(EXTRA_CONTENT_DESCRIPTION, mContext.getString(
                      R.string.accessibility_quick_settings_roaming_off));
        }
        mContext.sendBroadcast(intent);
    }

    private boolean getDataRoaming(int phoneId) {
        String val = TelephonyManager.getDefault().isMultiSimEnabled()
                ? String.valueOf(phoneId) : "";
        boolean enabled = Settings.Global.getInt(mContext.getContentResolver(),
                    Settings.Global.DATA_ROAMING + val, 0) != 0;
        return enabled;
    }

    private void setDataRoaming(boolean enabled, int phoneId) {
        if (TelephonyManager.getDefault().isMultiSimEnabled()) {
            Settings.Global.putInt(mContext.getContentResolver(),
                    Settings.Global.DATA_ROAMING + phoneId, enabled ? 1 : 0);
            if (phoneId == SubscriptionManager.getPhoneId(
                    SubscriptionManager.getDefaultDataSubscriptionId())) {
                Settings.Global.putInt(mContext.getContentResolver(),
                        Settings.Global.DATA_ROAMING, enabled ? 1 : 0);
            }
        } else {
            Settings.Global.putInt(mContext.getContentResolver(),
                    Settings.Global.DATA_ROAMING, enabled ? 1 : 0);
        }
    }

    private class RoamingObserver extends ContentObserver {
        public RoamingObserver(Handler handler) {
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange) {
            if (DEBUG) Log.d(TAG, "roaming is changed.");
            handleUpdateState();
        }

        public void startObserving() {
            int phoneCount = TelephonyManager.getDefault().getPhoneCount();
            for (int i = 0; i < phoneCount; i++) {
                mContext.getContentResolver().registerContentObserver(
                        Settings.Global.getUriFor(Settings.Global.DATA_ROAMING + i),
                        false, this);
            }
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.getUriFor(
                    Settings.Global.MULTI_SIM_DATA_CALL_SUBSCRIPTION),
                    false, this);
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.CONTENT_URI,
                    false, this);
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.getUriFor(Settings.Global.DATA_ROAMING),
                    false, this);
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.getUriFor(Settings.Global.AIRPLANE_MODE_ON),
                    false, this);
        }

        public void endObserving() {
            mContext.getContentResolver().unregisterContentObserver(this);
        }
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (TelephonyIntents.ACTION_SERVICE_STATE_CHANGED.equals(intent.getAction())) {
                int sub = intent.getIntExtra(
                        PhoneConstants.SUBSCRIPTION_KEY, PHONE_ID1);
                if (sub == PHONE_ID1) {
                    onRoamingVisibleChanged();
                }
            }
        }
    };

    private void onRoamingVisibleChanged() {
        final String CHINA_MCC = "460";
        final String MACAO_MCC = "455";

        String numeric = "";
        boolean isForeign;

        int mPhoneCount = TelephonyManager.getDefault().getPhoneCount();
        if (mPhoneCount > 1) {
            int[] subId = SubscriptionManager.getSubId(PHONE_ID1);
            if (subId != null && subId.length >= 1) {
                numeric = TelephonyManager.getDefault().getNetworkOperator(subId[0]);
            }
        } else {
            numeric = TelephonyManager.getDefault().getNetworkOperator();
        }

        if (!isValidNumeric(numeric)) {
            return;
        }

        if (numeric.startsWith(CHINA_MCC) || numeric.startsWith(MACAO_MCC)) {
            isForeign = false;
        } else {
            isForeign = true;
        }

        if (isForeign != mIsForeignState) {
            handleUpdateState();
            mIsForeignState = isForeign;
        }
    }

    private boolean isAirplaneModeOn() {
        return (Settings.System.getInt(mContext.getContentResolver(),
                Settings.System.AIRPLANE_MODE_ON, 0) != 0);
    }

    private boolean isValidNumeric(String numeric) {
        if (TextUtils.isEmpty(numeric)
                || numeric.equals("null") || numeric.equals("00000")) {
            return false;
        }
        return true;
    }

    private boolean hasIccCard() {
        if (TelephonyManager.getDefault().isMultiSimEnabled()) {
            TelephonyManager tm =
                (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
            int prfDataPhoneId = SubscriptionManager.getPhoneId(
                    SubscriptionManager.getDefaultDataSubscriptionId());
            int simState = tm.getSimState(prfDataPhoneId);
            boolean active = (simState != TelephonyManager.SIM_STATE_ABSENT)
                    && (simState != TelephonyManager.SIM_STATE_UNKNOWN);
            return active && tm.hasIccCard(prfDataPhoneId);
        } else {
            TelephonyManager tm =
                (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
            return tm.hasIccCard();
        }
    }
}
