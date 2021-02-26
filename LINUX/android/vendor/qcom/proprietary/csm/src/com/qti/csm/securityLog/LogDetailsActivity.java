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
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;

import com.qti.csm.R;

import java.util.HashMap;
import java.util.Map;

public class LogDetailsActivity extends PreferenceActivity implements
        AdapterView.OnItemClickListener {
    private GestureDetector mGestureDetector;
    private ListPreference mPreference;

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
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        proUpdate();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view,
                            final int position, long id) {
    }

    private void proUpdate() {
        try {
            getActionBar().setDisplayShowHomeEnabled(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
        getActionBar().setDisplayShowHomeEnabled(true);

        Settings.confUpdate(this);
        Settings.runTask(this);

        if (this instanceof PreferenceActivity) {
            addPreferencesFromResource(R.layout.list_log);
            mPreference = (ListPreference) findPreference("preference");
            mPreference.setValue(Integer.toString(Settings.getSet(this)));
            mPreference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                @Override
                public boolean onPreferenceChange(Preference preference, Object objValue) {
                    int value = Integer.parseInt((String) objValue);
                    if (value == 0 || value == 60000 || value == 600000
                            || value == 1800000 || value == 3600000) {
                        Settings.setDef(LogDetailsActivity.this, value);
                        if (DataProc.mSet != value) {
                            DataProc.mSet = value;
                            Settings.updateTask(LogDetailsActivity.this);
                        }
                    }
                    return true;
                }
            });
        }
    }

    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
        }
    };
}
