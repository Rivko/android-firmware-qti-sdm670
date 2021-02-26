/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.securityLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;

import com.qti.csm.R;
import com.qti.csm.securityLog.ProcessPosition;

import java.util.ArrayList;
import java.util.List;

public class PositionShow extends PreferenceActivity implements AdapterView.OnItemClickListener {
    private GestureDetector mGestureDetector;
    private static final String REQ_INFO = "app_req";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        PreferenceCategory mType;

        if (this instanceof PreferenceActivity) {
            addPreferencesFromResource(R.layout.pos_show);
            PreferenceScreen root = getPreferenceScreen();
            mType = (PreferenceCategory) root.findPreference(REQ_INFO);
            ProcessPosition doReq = new ProcessPosition(this);
            List<ProcessPosition.Request> appCall = doReq.getReq();
            List<Preference> recentLocationPrefs = new ArrayList<>(appCall.size());
            for (final ProcessPosition.Request request : appCall) {
                Preference pref = new Preference(this);
                pref.setIcon(request.icon);
                pref.setTitle(request.label);
                pref.setSummary(request.call);
                recentLocationPrefs.add(pref);
            }
            for (Preference entry : recentLocationPrefs) {
                mType.addPreference(entry);
            }

        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mGestureDetector != null) {
            return mGestureDetector.onTouchEvent(event);
        } else
            return false;
    }

    @Override
    protected void onResume() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("");
        registerReceiver(mBroadcastReceiver, intentFilter);
        super.onResume();
    }

    @Override
    protected void onPause() {
        unregisterReceiver(mBroadcastReceiver);
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view,
                            final int position, long id) {
    }

    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
        }
    };

}
