/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;

import com.qti.csm.R;
import com.qti.csm.utils.UtilsSystem;

import java.util.HashMap;
import java.util.Map;

import static com.qti.csm.encryption.system.ProcessInfo.logd;
public class EncryptOption extends PreferenceActivity implements
        AdapterView.OnItemClickListener {

    private Context mContext;
    GestureDetector mGestureDetector;

    DisplayMetrics displayMetrics = new DisplayMetrics();
    Map<String, ?> mMap = new HashMap<String, Integer>();

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (mGestureDetector != null) {
            if (mGestureDetector.onTouchEvent(event))
                return true;
            else
                return false;
        } else
            return false;
    }

    @Override
    protected void onResume() {
        logd("");
        refreshUI();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("");
        registerReceiver(mBroadcastReceiver, intentFilter);
        super.onResume();
    }

    @Override
    protected void onPause() {
        logd("");
        unregisterReceiver(mBroadcastReceiver);
        super.onPause();
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (UtilsSystem.isMonkeyRunning()) {
            return;
        }
        updateRes();
    }

    @Override
    protected void onDestroy() {
        logd("");
        super.onDestroy();
    };

    @Override
    public void onItemClick(AdapterView<?> parent, View view,
                            final int position, long id) {
    }


    void updateRes() {
        mContext = getApplicationContext();
        getActionBar().setDisplayShowHomeEnabled(true);

        if (this instanceof PreferenceActivity) {
            addPreferencesFromResource(R.xml.feature);
        }
    }


    private void refreshUI() {
        logd("");
    }

    BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            logd(action);
        };
    };
}
