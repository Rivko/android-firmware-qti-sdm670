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

import java.util.ArrayList;
import java.util.List;

public class MsgShow extends PreferenceActivity implements AdapterView.OnItemClickListener {
    private GestureDetector mGestureDetector;
    private static final String REQ_INFO = "app_req";
    private static final String REQ_ID = "app_id";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Context context = getApplicationContext();
        PreferenceCategory mType;
        PreferenceCategory mId;

        Intent intent = getIntent();
        String mPackage = intent.getStringExtra("package");
        String mName = intent.getStringExtra("name");
        int mUid = intent.getIntExtra("uid", 0);

        if (this instanceof PreferenceActivity) {
            addPreferencesFromResource(R.layout.msg_info);
            PreferenceScreen root = getPreferenceScreen();
            mId = (PreferenceCategory) root.findPreference(REQ_ID);
            Preference prefInfo = new Preference(this);
            prefInfo.setTitle(mName);
            mId.addPreference(prefInfo);
            mType = (PreferenceCategory) root.findPreference(REQ_INFO);
            DataProc deReq = new DataProc(context);
            ArrayList<String> requests = deReq.getReqSm(context, mUid);
            List<Preference> recentPrefs = new ArrayList<>(requests.size());
            for (final String request : requests) {
                Preference pref = new Preference(this);
                pref.setTitle(request);
                recentPrefs.add(pref);
            }
            for (Preference entry : recentPrefs) {
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
