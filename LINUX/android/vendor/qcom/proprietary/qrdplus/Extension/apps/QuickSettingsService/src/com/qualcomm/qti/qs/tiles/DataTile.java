/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qs.tiles;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.database.ContentObserver;
import android.os.Handler;
import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qualcomm.qti.qs.R;
import com.qualcomm.qti.qs.tiles.QSTile;

import java.lang.reflect.Method;

public class DataTile extends QSTile {
    private final TelephonyManager mTelephonyManager;
    private final DataObserver mDataObserver;
    private final String TAG = "DataTile";
    private final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    public DataTile(Context context, String action) {
        super(context, action);
        mTelephonyManager = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
        mDataObserver = new DataObserver(mHandler);
        mDataObserver.startObserving();
        mSubscriptionManager.addOnSubscriptionsChangedListener(mSubscriptionListener);
    }

    @Override
    public void handleClick() {
        if (DEBUG) Log.d(TAG,"handle Click");
        setEnabled(!isDataOn());
    }

    private void setEnabled(boolean enabled) {
        if (DEBUG) Log.d(TAG,"setEnabled = "+enabled);
        if (Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) != 0 || !hasIccCard()) {
            return;
        }
        mTelephonyManager.setDataEnabled(
                SubscriptionManager.from(mContext).getDefaultDataSubscriptionId(), enabled);
    }

    @Override
    public void handleUpdateState() {
        final boolean dataOn = isDataOn();
        if (DEBUG) Log.d(TAG,"handleUpdateState dataOn = "+dataOn);
        Intent intent = new Intent(mAction);
        intent.putExtra(EXTRA_ICON_PACKAGE,PACKAGE_NAME);
        intent.putExtra(EXTRA_ON_CLICK_PENDING_INTENT, mOnClickIntent);
        intent.putExtra(EXTRA_VISIBLE, true);
        intent.putExtra(EXTRA_LABEL,
                   mContext.getString(R.string.quick_settings_mobile_data_label));

        if (dataOn) {
            intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_data_on);
            intent.putExtra(EXTRA_CONTENT_DESCRIPTION,
                       mContext.getString(
                           R.string.accessibility_quick_settings_data_on));
        } else {
            intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_data_off);
            intent.putExtra(EXTRA_CONTENT_DESCRIPTION,
                       mContext.getString(
                           R.string.accessibility_quick_settings_data_off));
        }
        mContext.sendBroadcast(intent);
    }

    private boolean isDataOn() {
        return isDefaultDataEnabled()
                && !isAirplaneModeOn() && hasIccCard();
    }
    private boolean isDefaultDataEnabled() {
        return Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.MOBILE_DATA +
                SubscriptionManager.from(mContext).getDefaultDataSubscriptionId(), 0) != 0;
    }

    /** ContentObserver to watch mobile data on/off **/
    private class DataObserver extends ContentObserver {
        public DataObserver(Handler handler) {
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange) {
            if (DEBUG) Log.d(TAG, "data state change");
            handleUpdateState();
        }

        public void startObserving() {
            int phoneCount = mTelephonyManager.getPhoneCount();
            for (int i = 0; i < phoneCount; i++) {
                int[] subIds = SubscriptionManager.getSubId(i);
                if (subIds != null && subIds.length > 0) {
                    if (DEBUG) Log.d(TAG,"startObserving subId:"+subIds[0]);
                    mContext.getContentResolver().registerContentObserver(
                            Settings.Global.getUriFor(Settings.Global.MOBILE_DATA + subIds[0]),
                            false, this);
                }
            }
            if (mTelephonyManager.isMultiSimEnabled()) {
                mContext.getContentResolver().registerContentObserver(
                        Settings.Global.getUriFor(
                        Settings.Global.MULTI_SIM_DATA_CALL_SUBSCRIPTION),
                        false, this);
            }
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.getUriFor(Settings.Global.AIRPLANE_MODE_ON),
                    false, this);
        }

        public void endObserving() {
            if (DEBUG) Log.d(TAG, "endObserving");
            mContext.getContentResolver().unregisterContentObserver(this);
        }
    }

    private boolean isAirplaneModeOn() {
        return (Settings.System.getInt(mContext.getContentResolver(),
                Settings.System.AIRPLANE_MODE_ON, 0) != 0);
    }

    private boolean hasIccCard() {
        if (mTelephonyManager.isMultiSimEnabled()) {
            int prfDataSlotId = SubscriptionManager.getSlotId(
                    SubscriptionManager.getDefaultDataSubscriptionId());
            int simState = mTelephonyManager.getSimState(prfDataSlotId);
            boolean active = (simState != TelephonyManager.SIM_STATE_ABSENT)
                    && (simState != TelephonyManager.SIM_STATE_UNKNOWN);
            return active && mTelephonyManager.hasIccCard(prfDataSlotId);
        } else {
            return mTelephonyManager.hasIccCard();
        }
    }

    @Override
    protected void updateObserver() {
        if (mDataObserver!=null) {
            mDataObserver.endObserving();
            mDataObserver.startObserving();
        }
    }

}
