/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.server.wigigapp;

import android.app.ActionBar;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WpsInfo;
import android.os.Bundle;
import android.preference.Preference;
import android.util.Log;
import android.view.MenuItem;

import com.qualcomm.qti.wigig.WigigManager;

public class AdvancedWifiSettings extends WigigActivity {

    private static final String TAG = "AdvancedWifiSettings";
    private static final String KEY_CURRENT_IP_ADDRESS = "current_ip_address";
    private static final String KEY_WPS_LABEL = "wps_label";
    private static final String KEY_WPS_PUSH = "wps_push_button";
    private static final String KEY_WPS_PIN = "wps_pin_entry";

    private Preference mWpsLabelPref;
    private Preference mWpsPushPref;
    private Preference mWpsPinPref;

    private WpsDialog mWpsDialog;

    public AdvancedWifiSettings() {
        super(R.xml.pref_general, new String[]{WigigManager.NETWORK_STATE_CHANGED_ACTION});
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "Enter onCreate");
        super.onCreate(savedInstanceState);
        getPreferenceScreen().removeAll(); //remove both generic preferences - enable_wigig_switch and wigig_mode
        addPreferencesFromResource(R.xml.wifi_advanced_settings);
        initPreferences();
        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
        }
    }

    @Override
    public void onResume() {
        Log.d(TAG, "Enter onResume");
        super.onResume();
        refreshWifiInfo();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        } else {
            return super.onOptionsItemSelected(item);
        }
    }

    private void initPreferences() {
        mWpsLabelPref = findPreference(KEY_WPS_LABEL);
        mWpsLabelPref.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
            public boolean onPreferenceClick(Preference arg0) {
                WpsFragment wpsFragment = new WpsFragment(AdvancedWifiSettings.this, WpsInfo.LABEL);
                wpsFragment.show(getFragmentManager(), KEY_WPS_LABEL);
                return true;
            }
        });

        mWpsPushPref = findPreference(KEY_WPS_PUSH);
        mWpsPushPref.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
            public boolean onPreferenceClick(Preference arg0) {
                WpsFragment wpsFragment = new WpsFragment(AdvancedWifiSettings.this, WpsInfo.PBC);
                wpsFragment.show(getFragmentManager(), KEY_WPS_PUSH);
                return true;
            }
        });

        mWpsPinPref = findPreference(KEY_WPS_PIN);
        mWpsPinPref.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
            public boolean onPreferenceClick(Preference arg0) {
                WpsFragment wpsFragment = new WpsFragment(AdvancedWifiSettings.this, WpsInfo.DISPLAY);
                wpsFragment.show(getFragmentManager(), KEY_WPS_PIN);
                return true;
            }
        });
    }

    private void refreshWifiInfo() {
        final Context context = this;
        Preference wifiIpAddressPref = findPreference(KEY_CURRENT_IP_ADDRESS);
        int ipAddress;
        String ipStr;

        WifiInfo wifiInfo = mWigigManager.getConnectionInfo();
        if (wifiInfo != null) {
            ipAddress = wifiInfo.getIpAddress();
            ipStr = String.format("%d.%d.%d.%d",
                    (ipAddress & 0xff),
                    (ipAddress >> 8 & 0xff),
                    (ipAddress >> 16 & 0xff),
                    (ipAddress >> 24 & 0xff));
        } else {
            ipAddress = 0;
            ipStr = null;
        }

        wifiIpAddressPref.setSummary(ipAddress == 0 ?
                context.getString(R.string.status_unavailable) : ipStr);
        wifiIpAddressPref.setSelectable(false);
    }

    public static class WpsFragment extends DialogFragment {
        private final int mWpsSetup;
        private final AdvancedWifiSettings mParent;

        public WpsFragment(AdvancedWifiSettings parent, int wpsSetup) {
            super();
            mWpsSetup = wpsSetup;
            mParent = parent;
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            if (mParent.mWpsDialog == null || !mParent.mWpsDialog.isShowing())
                mParent.mWpsDialog = new WpsDialog(getActivity(), mWpsSetup, mParent.mWigigManager);

            return mParent.mWpsDialog;
        }
    }

    @Override
    protected void onWigigFrameworkEnabled() {
        Log.d(TAG, "onWigigFrameworkEnabled()");
        mWpsLabelPref.setEnabled(true);
        mWpsPushPref.setEnabled(true);
        mWpsPinPref.setEnabled(true);
    }

    @Override
    protected void onWigigFrameworkDisabled() {
        Log.d(TAG, "onWigigFrameworkDisabled()");
        mWpsLabelPref.setEnabled(false);
        mWpsPushPref.setEnabled(false);
        mWpsPinPref.setEnabled(false);
    }

    @Override
    protected void onNetworkStateChanged(NetworkInfo.DetailedState ds, int networkId) {
        super.onNetworkStateChanged(ds, networkId);
        refreshWifiInfo();
        if ((ds == NetworkInfo.DetailedState.CONNECTED) && (mWpsDialog != null)){
            mWpsDialog.updateDialogOnComplete();
        }
    }
}
