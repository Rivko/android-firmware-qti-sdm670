/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigigapp;

import android.content.Intent;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.preference.ListPreference;
import android.util.Log;


public class WigigRateUpgradeSettings extends WigigActivity {
    private static final String TAG = "WigigRUSettings";

    public WigigRateUpgradeSettings() {
        super(R.xml.pref_ru, new String[] {});
    }

    @Override
    protected void handleEvent(Intent intent) {
        super.handleEvent(intent);
    }

    @Override
    protected void onWigigSwitchedOn() {
        setWigigRUEnabled(true);
    }

    @Override
    protected void onWigigSwitchedOff() {
        super.onWigigSwitchedOff();
        setWigigRUEnabled(false);
    }

    @Override
    protected boolean onModeChange(String mode) {
        Log.d(TAG, "Mode preference changed to " + mode);
        if (mode.equals(getResources().getString(R.string.p2p_mode)) ||
                mode.equals(getResources().getString(R.string.hotspot_mode)) ||
                mode.equals(getResources().getString(R.string.network_mode))) {
            modeSwitchDelayed = mode;
            setWigigRUEnabled(false);
            return false;
        }
        return true;
    }

    @Override
    protected void onWigigFrameworkEnabled() {
        super.onWigigFrameworkEnabled();
        Log.d(TAG, "onWigigFrameworkEnabled()");

    }

    @Override
    protected void onWigigFrameworkDisabled() {
        super.onWigigFrameworkDisabled();
        Log.d(TAG, "onWigigFrameworkDisabled()");
        if(!modeSwitchDelayed.isEmpty()) {
            if (modeSwitchDelayed.equals(getResources().getString(R.string.p2p_mode)))
                switchWigigMode(WigigRateUpgradeSettings.this, WigigWifiDirectSettings.class, true);
            else if (modeSwitchDelayed.equals(getResources().getString(R.string.hotspot_mode)))
                switchWigigMode(WigigRateUpgradeSettings.this, SoftAP.class, true);
            else if (modeSwitchDelayed.equals(getResources().getString(R.string.network_mode)))
                switchWigigMode(WigigRateUpgradeSettings.this, WigigSettingsActivity.class, true);
        }
    }

    @Override
    protected void onInitialStatesObtained(boolean[] states) {
        super.onInitialStatesObtained(states);
        Log.d(TAG, "onInitialStatesObtained()");
    }

    @Override
    protected void onNetworkStateChanged(NetworkInfo.DetailedState ds, int networkId) {
        Log.d(TAG, "onNetworkStateChanged() " + ds.toString());
        super.onNetworkStateChanged(ds, networkId);
    }

    @Override
    protected void onP2PStateChanged(NetworkInfo networkInfo) {
        super.onP2PStateChanged(networkInfo);
        Log.d(TAG, "onP2PStateChanged()");
    }

    @SuppressWarnings("deprecation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "WigigRUSettings onCreate()");
        super.onCreate(savedInstanceState);

        if (!mLastMode.isEmpty()) {
            if (!mLastMode.equals(getResources().getString(R.string.ru_mode))) {
                Log.d(TAG, "Switching to previous persistent mode " + mLastMode);
                    onModeChange(mLastMode);
            }
        }

        Bundle b = getIntent().getExtras();
        if(b != null) {
            if (b.getBoolean(ENABLE_WIGIG_STR, false)) {
                if (!isWigigRUEnabled()) {
                    setWigigRUEnabled(true);
                }
            }
        }
    }

    @SuppressWarnings("deprecation")
    @Override
    protected void onResume() {
        Log.d(TAG, "Enter onResume");
        super.onResume();
        if (mWigigManager != null) {
            ListPreference modePreference = (ListPreference) findPreference("wigig_mode");
            modePreference.setValue(getResources().getString(R.string.ru_mode));
            modePreference.setSummary("%s");
            writeLastMode(getResources().getString(R.string.ru_mode));
        }
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "Enter onPause");
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
    }

}
