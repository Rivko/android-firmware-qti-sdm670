/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qs.tiles;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.Settings;
import android.provider.Telephony;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.qualcomm.qti.qs.R;
import com.qualcomm.qti.qs.tiles.QSTile;

import java.util.ArrayList;
import java.util.HashMap;

public class ApnTile extends QSTile {
    private final String TAG = "ApnTile";
    private final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    private static final Uri URI_PHONE_FEATURE =
            Uri.parse("content://com.qualcomm.qti.phonefeature.FEATURE_PROVIDER");
    private static final String METHOD_SET_NEXT_APN_AS_PREF = "set_next_apn_as_pref";
    private static final String EXTRA_CURRENT_APN = "current_apn";
    private final Uri PREFERAPN_URI = Uri.parse("content://telephony/carriers/preferapn");
    private final String[] PROJECTION = new String[] {"_id", "name", "apn", "type"};
    private final int INDEX_ID = 0;
    private final int INDEX_NAME = 1;
    private final int INDEX_APN = 2;
    private final int INDEX_TYPE = 3;
    private final ApnObserver mApnObserver;
    private Apn mCurrentApn = null;
    private ArrayList<Integer> mApnIcons = null;
    private Integer mCurrentIcon = null;
    private String mCurrentApnName = "";

    public ApnTile(Context context, String action) {
        super(context, action);
        mApnIcons = new ArrayList<Integer>();
        mApnIcons.add(R.drawable.ic_qs_apn1);
        mApnIcons.add(R.drawable.ic_qs_apn2);
        mCurrentIcon = mApnIcons.get(0);
        mCurrentApn = new Apn();
        mApnObserver = new ApnObserver(mHandler);
        mApnObserver.startObserving();
    }

    @Override
    public void handleClick() {
        long curId = 0;
        try {
            curId = Long.parseLong(getCurrentApnId());
        } catch (Exception e) {
        }
        if (DEBUG) Log.d(TAG, "Current apn id is: " + curId);
        setPrefApn(curId);
    }

    @Override
    public void handleUpdateState() {
        updateCurrentApn();
        Intent intent = new Intent(mAction);
        intent.putExtra(EXTRA_ICON_PACKAGE,PACKAGE_NAME);
        intent.putExtra(EXTRA_ON_CLICK_PENDING_INTENT, mOnClickIntent);

        int dataPhoneId = PHONE_ID1;
        if (TelephonyManager.getDefault().isMultiSimEnabled()) {
            dataPhoneId = (int) SubscriptionManager.from(mContext).getDefaultDataPhoneId();
        }
        intent.putExtra(EXTRA_VISIBLE, hasIccCard() && !isAirplaneModeOn());
        String apnName = getCurrentApnName();
        if (!apnName.isEmpty() && !apnName.equals(mCurrentApnName)) {
            updateApnIcon();
            mCurrentApnName = apnName;
        }

        intent.putExtra(EXTRA_ICON_ID, mCurrentIcon);
        intent.putExtra(EXTRA_LABEL, apnName.toUpperCase());
        intent.putExtra(EXTRA_CONTENT_DESCRIPTION, apnName);
        mContext.sendBroadcast(intent);

    }

    private Bundle callBinder(String method, Bundle extras) {
        if (!isPhoneFeatureEnabled()) {
            return null;
        }
        return mContext.getContentResolver().call(URI_PHONE_FEATURE, method, null, extras);
    }

    private boolean isPhoneFeatureEnabled() {
        return mContext.getContentResolver().acquireProvider(URI_PHONE_FEATURE) != null;
    }

    private void setPrefApn(long curId) {
        Bundle params = new Bundle();
        params.putLong(EXTRA_CURRENT_APN, curId);
        callBinder(METHOD_SET_NEXT_APN_AS_PREF, params);
    }

    private void updateCurrentApn() {
        Cursor cursor = null;
        try {
            cursor = mContext.getContentResolver().query(getUri(PREFERAPN_URI), PROJECTION,
                    null, null, Telephony.Carriers.DEFAULT_SORT_ORDER);
            if (cursor != null && cursor.getCount() > 0) {
                cursor.moveToFirst();
                mCurrentApn.id = cursor.getString(INDEX_ID);
                mCurrentApn.name = cursor.getString(INDEX_NAME);
                mCurrentApn.apn = cursor.getString(INDEX_APN);
                mCurrentApn.type = cursor.getString(INDEX_TYPE);
                if (DEBUG) {
                    Log.d(TAG, "updateCurrentApn: " + "id= " + mCurrentApn.id
                            + " name= " + mCurrentApn.name
                            + " apn= " + mCurrentApn.apn
                            + " type= " + mCurrentApn.type);
                }
            }else{
                mCurrentApn.id = "";
                mCurrentApn.name = "";
                mCurrentApn.apn = "";
                mCurrentApn.type = "";
            }
        } catch(Exception e) {
        } finally {
            if (cursor != null) {
                cursor.close();
                cursor = null;
            }
        }
    }

    private void updateApnIcon() {
        Integer icon = null;

        int iconId = mApnIcons.indexOf(mCurrentIcon);
        icon = mApnIcons.get((iconId + 1) % mApnIcons.size());

        if (icon != null) {
            mCurrentIcon = icon;
        }
    }

    private String getCurrentApnId() {
        String id = "";
        if (mCurrentApn != null && mCurrentApn.id != null) {
            id = mCurrentApn.id;
        }
        return id;
    }

    private String getCurrentApnName() {
        String apn = "";
        if (mCurrentApn != null && mCurrentApn.apn != null) {
            apn = mCurrentApn.apn;
        }
        return apn;
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

    private Uri getUri(Uri uri) {
        int subId = SubscriptionManager.getDefaultDataSubscriptionId();
        return Uri.withAppendedPath(uri, "/subId/" + subId);
    }

    private boolean isAirplaneModeOn() {
        return (Settings.System.getInt(mContext.getContentResolver(),
                Settings.System.AIRPLANE_MODE_ON, 0) != 0);
    }

    /** ContentObserver to watch apn switch **/
    private class ApnObserver extends ContentObserver {
        public ApnObserver(Handler handler) {
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            if (DEBUG) Log.d(TAG, "preferred apn changed.");
            handleUpdateState();
        }

        public void startObserving() {
            final ContentResolver cr = mContext.getContentResolver();
            cr.registerContentObserver(PREFERAPN_URI, true, this);
            cr.registerContentObserver(Settings.Global.getUriFor(
                    Settings.Global.MULTI_SIM_DATA_CALL_SUBSCRIPTION),
                    false, this);
            cr.registerContentObserver(Settings.Global.getUriFor(
                    Settings.Global.AIRPLANE_MODE_ON),
                    false, this);
        }

        public void endObserving() {
            mContext.getContentResolver().unregisterContentObserver(this);
        }
    };

    private class Apn {
        String id;
        String name;
        String apn;
        String type;
    }

}
