/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qs.tiles;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionManager.OnSubscriptionsChangedListener;
import android.util.Log;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Base quick-settings tile, extend this to create a new tile.
 */
public abstract class QSTile {
    private final String TAG = "QSTile";
    private final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);
    protected static final int PHONE_ID1 = 0;
    protected static final String EXTRA_VISIBLE = "visible";
    protected static final String EXTRA_CONTENT_DESCRIPTION = "contentDescription";
    protected static final String EXTRA_LABEL = "label";
    protected static final String EXTRA_ICON_ID = "iconId";
    protected static final String EXTRA_ICON_BITMAP = "iconBitmap";
    protected static final String EXTRA_ICON_PACKAGE = "iconPackage";
    protected static final String EXTRA_ON_CLICK_PENDING_INTENT = "onClick";
    protected static final String EXTRA_ON_CLICK_URI = "onClickUri";
    protected static final String EXTRA_ON_LONG_CLICK_PENDING_INTENT = "onLongClick";
    protected static final String EXTRA_ON_LONG_CLICK_URI = "onLongClickUri";
    protected static final String PACKAGE_NAME = "com.qualcomm.qti.qs";
    protected final Handler mHandler;
    protected final Context mContext;
    protected final String mAction;
    protected final String mOnClickAction;
    protected final PendingIntent mOnClickIntent;
    protected final SubscriptionManager mSubscriptionManager;
    protected List<Integer> mCurrentSubIds = new ArrayList<Integer>();

    protected QSTile(Context context, String action) {
        mContext = context;
        mAction = action;
        mOnClickAction = mAction.concat(".onclick");
        mOnClickIntent = PendingIntent.getBroadcast(mContext, 0,
                new Intent(mOnClickAction),
                PendingIntent.FLAG_CANCEL_CURRENT);
        mHandler = new Handler();
        mSubscriptionManager = SubscriptionManager.from(context);
    }

    abstract public void handleUpdateState();
    abstract public void handleClick();
    public String getAction() {
        return mAction;
    }
    public String getOnClickAction() {
        return mOnClickAction;
    }

    protected OnSubscriptionsChangedListener mSubscriptionListener =
            new OnSubscriptionsChangedListener() {
        @Override
        public void onSubscriptionsChanged() {
            if (DEBUG) Log.d(TAG,"onSubscriptionsChanged");
            List<SubscriptionInfo> subscriptions = mSubscriptionManager
                    .getActiveSubscriptionInfoList();
            if (hasCorrectSubId(subscriptions)) {
                return;
            }
            updateCurrentSubIds(subscriptions);
            updateObserver();
        }
    };

    protected void updateObserver() {
    }

    protected boolean hasCorrectSubId(List<SubscriptionInfo> subscriptions) {
        if (subscriptions == null) {
            subscriptions = Collections.emptyList();
        }
        if (subscriptions.size() != mCurrentSubIds.size()) {
            return false;
        }
        for (SubscriptionInfo info : subscriptions) {
            if (mCurrentSubIds.indexOf(info.getSubscriptionId()) == -1) {
                return false;
            }
        }
        return true;
    }

    protected void updateCurrentSubIds(List<SubscriptionInfo> subscriptions) {
        mCurrentSubIds.clear();
        if(subscriptions == null) {
            return;
        }
        int num = subscriptions.size();
        for (int i = 0; i < num; i++) {
            mCurrentSubIds.add(subscriptions.get(i).getSubscriptionId());
            if (DEBUG) Log.d(TAG,"mCurrentSubIds.get("+i+") = "+mCurrentSubIds.get(i));
        }
    }
}
