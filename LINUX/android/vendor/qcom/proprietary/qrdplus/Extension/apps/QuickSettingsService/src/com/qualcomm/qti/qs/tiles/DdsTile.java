/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qs.tiles;

import android.content.Context;
import android.content.Intent;
import android.database.ContentObserver;
import android.net.ConnectivityManager;
import android.os.Handler;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.PhoneConstants;

import com.qualcomm.qti.qs.R;
import com.qualcomm.qti.qs.tiles.QSTile;

import org.codeaurora.internal.IExtTelephony;


public class DdsTile extends QSTile {
    private final String TAG = "DdsTile";
    private final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);
    private final DdsObserver mDdsObserver;
    private static final int PROVISIONED = 1;
    private static final int NOT_PROVISIONED = 0;
    private static final int INVALID_STATE = -1;
    private static final int CARD_NOT_PRESENT = -2;

    public DdsTile(Context context, String action) {
        super(context, action);
        mDdsObserver = new DdsObserver(mHandler);
        mDdsObserver.startObserving();
        mSubscriptionManager.addOnSubscriptionsChangedListener(mSubscriptionListener);
    }

    @Override
    public void handleClick() {
        switchDdsToNext();
    }

    @Override
    public void handleUpdateState() {
        Intent intent = new Intent(mAction);
        intent.putExtra(EXTRA_ICON_PACKAGE,PACKAGE_NAME);
        intent.putExtra(EXTRA_ON_CLICK_PENDING_INTENT, mOnClickIntent);
        intent.putExtra(EXTRA_VISIBLE, true);

        int dataPhoneId = PHONE_ID1;
        if (TelephonyManager.getDefault().isMultiSimEnabled()) {
            dataPhoneId = SubscriptionManager.getPhoneId(
                    SubscriptionManager.getDefaultDataSubscriptionId());
        }

        if (isDefaultDataEnabled() && !isAirplaneModeOn()&& hasIccCard()) {
            if (DEBUG) Log.d(TAG, "mobile data is on.");
            switch(dataPhoneId) {
                case PhoneConstants.SUB1:
                    intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_data_on_1);
                    break;
                case PhoneConstants.SUB2:
                    intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_data_on_2);
                    break;
                case PhoneConstants.SUB3:
                default:
                    intent.putExtra(EXTRA_ICON_ID, 0);
                    break;
            }
            intent.putExtra(EXTRA_LABEL, mContext.getString(
                        R.string.quick_settings_dds_sub, dataPhoneId + 1));
            intent.putExtra(EXTRA_CONTENT_DESCRIPTION,
                        mContext.getString(
                                R.string.quick_settings_dds_sub, dataPhoneId + 1));
        } else {
            if (DEBUG) Log.d(TAG, "mobile data is off.");
            switch(dataPhoneId) {
                case PhoneConstants.SUB1:
                    intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_data_off_1);
                    break;
                case PhoneConstants.SUB2:
                    intent.putExtra(EXTRA_ICON_ID, R.drawable.ic_qs_data_off_2);
                    break;
                case PhoneConstants.SUB3:
                default:
                    intent.putExtra(EXTRA_ICON_ID, 0);
                    break;
            }
            intent.putExtra(EXTRA_LABEL, mContext.getString(
                    R.string.quick_settings_data_off));
            intent.putExtra(EXTRA_CONTENT_DESCRIPTION,
                    mContext.getString(
                            R.string.quick_settings_data_off));
        }
        mContext.sendBroadcast(intent);
    }

    private boolean isDefaultDataEnabled() {
        return Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.MOBILE_DATA +
                SubscriptionManager.getDefaultDataSubscriptionId(), 0) != 0;
    }

    private void switchDdsToNext() {
        if (DEBUG) Log.d(TAG, "switchDdsToNext");

        if (!TelephonyManager.getDefault().isMultiSimEnabled() ||
                SubscriptionManager.from(mContext).getActiveSubscriptionInfoCount() < 2 ||
                getActiveSimCount() < 2) {
            return;
        }

        TelephonyManager tm = (TelephonyManager)
                mContext.getSystemService(Context.TELEPHONY_SERVICE);
        int dataPhoneId = SubscriptionManager.getPhoneId(
                SubscriptionManager.getDefaultDataSubscriptionId());
        int phoneCount = tm.getPhoneCount();
        int[] subIds = SubscriptionManager.getSubId((dataPhoneId + 1) % phoneCount);
        SubscriptionManager.from(mContext).setDefaultDataSubId(subIds[0]);
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

    private class DdsObserver extends ContentObserver {
        public DdsObserver(Handler handler) {
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange) {
            if (DEBUG) Log.d(TAG, "mobile data or data sub is changed.");
            handleUpdateState();
        }

        public void startObserving() {
            int phoneCount = TelephonyManager.getDefault().getPhoneCount();
            int[] subIds;
            for (int i = 0; i < phoneCount; i++) {
                subIds = SubscriptionManager.getSubId(i);
                mContext.getContentResolver().registerContentObserver(
                        Settings.Global.getUriFor(Settings.Global.MOBILE_DATA + subIds[0]),
                        false, this);
            }
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.getUriFor(
                    Settings.Global.MULTI_SIM_DATA_CALL_SUBSCRIPTION),
                    false, this);
            mContext.getContentResolver().registerContentObserver(
                    Settings.Global.getUriFor(Settings.Global.AIRPLANE_MODE_ON),
                    false, this);
        }

        public void endObserving() {
            mContext.getContentResolver().unregisterContentObserver(this);
        }
    }

    private boolean isAirplaneModeOn() {
        return (Settings.System.getInt(mContext.getContentResolver(),
                Settings.System.AIRPLANE_MODE_ON, 0) != 0);
    }

    @Override
    protected void updateObserver() {
        if (mDdsObserver!=null) {
            mDdsObserver.endObserving();
            mDdsObserver.startObserving();
        }
    }

    private int getActiveSimCount() {
        TelephonyManager tm = (TelephonyManager)
                mContext.getSystemService(Context.TELEPHONY_SERVICE);
        int numSlot = tm.getPhoneCount();
        int[] mUiccProvisionStatus = new int[numSlot];
        for(int i = 0; i < numSlot; i++) {
            mUiccProvisionStatus[i] = INVALID_STATE;
            try {
                IExtTelephony extTelephony =
                        IExtTelephony.Stub.asInterface(ServiceManager.getService("extphone"));
                //get current provision state of the SIM.
                mUiccProvisionStatus[i] =
                        extTelephony.getCurrentUiccCardProvisioningStatus(i);
            } catch (RemoteException ex) {
                if (DEBUG) Log.d(TAG, "Failed to get pref, slotId: "+ i +" Exception: " + ex);
            } catch (NullPointerException ex) {
                if (DEBUG) Log.d(TAG, "Failed to get pref, slotId: "+ i +" Exception: " + ex);
            }
        }

        int activeSimNum = 0;
        for(int i = 0; i <numSlot; i++) {
            if(mUiccProvisionStatus[i] == PROVISIONED) activeSimNum++;
        }

        return activeSimNum;
    }
}
