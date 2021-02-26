/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2010 The Android Open Source Project
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

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.wigig.WigigManager;

public class SoftAP extends WigigActivity {
    private static final String TAG = "SoftAP";

    private static final int OPEN_INDEX = 0;
    private static final int WPA_INDEX = 1;

    private static final int MIN_PW_LEN = 8;

    private View mView;
    private TextView mSsid;
    private int mSecurityTypeIndex = OPEN_INDEX;
    private EditText mPassword;

    private WifiConfiguration mWifiConfig;

    private AlertDialog mAlertDialog;
    private Context mContext;
    private Preference mApPreference;

    private String mModeChange = null;

    public SoftAP() {
        super(R.xml.pref_softap, new String[] {WigigManager.WIGIG_AP_STATE_CHANGED_ACTION,
                Intent.ACTION_AIRPLANE_MODE_CHANGED} );
    }

    @Override
    protected void handleEvent(Intent intent) {
        final String action = intent.getAction();
        if (action.equals(WigigManager.WIGIG_AP_STATE_CHANGED_ACTION)) {
            int apState = intent.getIntExtra(WigigManager.EXTRA_WIGIG_AP_STATE, -1);
            if (apState == WigigManager.WIGIG_AP_STATE_ENABLED) {
                onWigigFrameworkEnabled();
            } else if (apState == WigigManager.WIGIG_AP_STATE_DISABLED) {
                onWigigFrameworkDisabled();
            } else if (apState == WigigManager.WIGIG_AP_STATE_FAILED && !mReceiver.isInitialStickyBroadcast()) {
                onWigigFrameworkDisabled();
                int failReason = intent.getIntExtra(WigigManager.EXTRA_WIGIG_AP_FAILURE_REASON, -1);
                Toast.makeText(SoftAP.this, "Failed to enable Hotspot", Toast.LENGTH_SHORT).show();
                Log.e(TAG, "Failed to enable Hotspot because " + failReason);
            }
        } else if (action.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED)) {
            if (isAirplaneMode()) {
                Log.d(TAG, "Switching to network mode due to airplane mode");
                onModeChange(getString(R.string.network_mode));
            }
        }
    }

    private static int getSecurityTypeIndex(WifiConfiguration wifiConfig) {
        if (wifiConfig.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK)) {
            return WPA_INDEX;
        }
        return OPEN_INDEX;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!mLastMode.isEmpty()) {
            if (!mLastMode.equals(getResources().getString(R.string.hotspot_mode))) {
                Log.d(TAG, "Switching to previous persistent mode " + mLastMode);
                onModeChange(mLastMode);
            }
        } else if (isAirplaneMode()) {
            Log.d(TAG, "Switching to network mode due to airplane mode");
            onModeChange(getString(R.string.network_mode));
        }

        mContext = this;

        mApPreference = findPreference("softap_config");
        mApPreference.setOnPreferenceClickListener(sClickPreferenceListener);

        Bundle b = getIntent().getExtras();
        if(b != null) {
            if (b.getBoolean(ENABLE_WIGIG_STR, false)) {
                if (!isWigigApEnabled()) {
                    setWigigApEnabled(null, true);
                }
            }
            else /* If this is a persistence flow, the hostapd is ready to go */
                getWigigApConfiguration();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mWigigManager != null) {
            ListPreference modePreference = (ListPreference) findPreference("wigig_mode");
            modePreference.setValue(getResources().getString(R.string.hotspot_mode));
            modePreference.setSummary("%s");
            writeLastMode(getResources().getString(R.string.hotspot_mode));
        }
    }

    private void showSecurityFields() {
        if (mSecurityTypeIndex == OPEN_INDEX) {
            mView.findViewById(R.id.fields).setVisibility(View.GONE);
            return;
        }
        mView.findViewById(R.id.fields).setVisibility(View.VISIBLE);
    }

    private void validate() {
        if ((mSsid != null && mSsid.length() == 0) ||
                ((mSecurityTypeIndex == WPA_INDEX) &&
                        mPassword.length() < MIN_PW_LEN)) {
            mAlertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
        } else {
            mAlertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(true);
        }
    }

    private final Preference.OnPreferenceClickListener sClickPreferenceListener = new Preference.OnPreferenceClickListener() {
        @Override
        public boolean onPreferenceClick(Preference preference) {
            if (preference != null) {
                if (mAlertDialog != null && mAlertDialog.isShowing())
                    return true;

                mView = getLayoutInflater().inflate(R.layout.wifi_ap_dialog, null);

                AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
                builder.setView(mView);
                builder.setTitle(R.string.wifi_tether_configure_ap_text);
                builder.setPositiveButton(R.string.save, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        WifiConfiguration config = mWifiConfig = getConfig();
                        if (config != null) {
                            mApPreference.setSummary(config.SSID + " " + (mSecurityTypeIndex == OPEN_INDEX ?
                                    getResources().getString(R.string.wifi_security_none) :
                                    getResources().getString(R.string.wifi_security_wpa)));
                            if (isWigigApEnabled()) {
                                if (setWigigApEnabled(null, false))
                                    setWigigApEnabled(config, true);
                            } else {
                                if (!mWigigManager.setWigigApConfiguration(config)) {
                                    Toast.makeText(SoftAP.this, "Failed to set Wigig AP configuration!", Toast.LENGTH_SHORT).show();
                                    Log.e(TAG, "Failed to set Wigig AP configuration!");
                                }
                            }
                        }
                    }
                });
                builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.dismiss();
                    }
                });

                mAlertDialog = builder.create();
                mAlertDialog.show();

                Spinner mSecurity = (Spinner) mView.findViewById(R.id.security);
                mSecurity.setOnItemSelectedListener(mSecuritySelect);
                mSsid = (TextView) mView.findViewById(R.id.ssid);
                mSsid.addTextChangedListener(hotSpotTextWatcher);
                mPassword = (EditText) mView.findViewById(R.id.password);
                mPassword.addTextChangedListener(hotSpotTextWatcher);

                mView.findViewById(R.id.show_password).setOnClickListener(new View.OnClickListener() {
                    public void onClick(View view) {
                        mPassword.setInputType(
                                InputType.TYPE_CLASS_TEXT | (((CheckBox) view).isChecked() ?
                                        InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                                        InputType.TYPE_TEXT_VARIATION_PASSWORD));
                    }
                });

                if (mWifiConfig != null) {
                    mSsid.setText(mWifiConfig.SSID);
                    mSecurityTypeIndex = getSecurityTypeIndex(mWifiConfig);
                    mSecurity.setSelection(mSecurityTypeIndex);
                    if (mSecurityTypeIndex == WPA_INDEX) {
                        mPassword.setText(mWifiConfig.preSharedKey);
                    }
                }

                showSecurityFields();
                validate();
            }
            return true;
        }
    };

    private WifiConfiguration getConfig() {
        WifiConfiguration config = new WifiConfiguration();

        config.SSID = mSsid.getText().toString();

        switch (mSecurityTypeIndex) {
            case OPEN_INDEX:
                config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
                return config;

            case WPA_INDEX:
                config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
                if (mPassword.length() >= MIN_PW_LEN) {
                    config.preSharedKey = mPassword.getText().toString();
                }
                return config;
        }
        return null;
    }

    private final TextWatcher hotSpotTextWatcher = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
        }

        @Override
        public void afterTextChanged(Editable s) {
            validate();
        }
    };

    private final AdapterView.OnItemSelectedListener mSecuritySelect = new AdapterView.OnItemSelectedListener(){
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            mSecurityTypeIndex = position;
            showSecurityFields();
            validate();
        }

        @Override
        public void onNothingSelected (AdapterView < ? > parent){
        }
    };

    @Override
    protected void onWigigSwitchedOn() {
        super.onWigigSwitchedOn();
        setWigigApEnabled(null, true);
    }

    @Override
    protected void onWigigSwitchedOff() {
        super.onWigigSwitchedOff();
        setWigigApEnabled(null, false);
    }

    @Override
    protected boolean onModeChange(String mode) {
        Log.d(TAG, "Mode preference changed to " + mode);

        mModeChange = mode;

        if (isWigigApEnabled())
            setWigigApEnabled(null, false);
        else
            onWigigFrameworkDisabled();

        return true;
    }

    private void getWigigApConfiguration() {
        mWifiConfig = mWigigManager.getWigigApConfiguration();
        if (mWifiConfig != null) {
            mApPreference.setEnabled(true);
            mSecurityTypeIndex = getSecurityTypeIndex(mWifiConfig);
            mApPreference.setSummary(mWifiConfig.SSID + " " + (mSecurityTypeIndex == OPEN_INDEX ?
                    getResources().getString(R.string.wifi_security_none) :
                    getResources().getString(R.string.wifi_security_wpa)));
        }
    }

    @Override
    protected void onWigigFrameworkEnabled() {
        super.onWigigFrameworkEnabled();
        Log.d(TAG, "onWigigFrameworkEnabled()");
        getWigigApConfiguration();
    }

    @Override
    protected void onWigigFrameworkDisabled() {
        super.onWigigFrameworkDisabled();
        Log.d(TAG, "onWigigFrameworkDisabled()");

        if (mModeChange != null) {
            if (mModeChange.equals(getResources().getString(R.string.network_mode))) {
                switchWigigMode(SoftAP.this, WigigSettingsActivity.class, isAirplaneMode() ? false : true);
            } else if (mModeChange.equals(getResources().getString(R.string.p2p_mode))) {
                switchWigigMode(SoftAP.this, WigigWifiDirectSettings.class, true);
            } else if (mModeChange.equals(getResources().getString(R.string.ru_mode))) {
                switchWigigMode(SoftAP.this, WigigRateUpgradeSettings.class, true);
            }
            mModeChange = null;
        }
    }
}
