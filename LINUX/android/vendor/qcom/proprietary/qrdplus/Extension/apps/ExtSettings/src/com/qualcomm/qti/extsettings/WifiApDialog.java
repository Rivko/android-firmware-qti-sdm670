/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Not a Contribution.
Apache license notifications and license are retained
for attribution purposes only.

Copyright (C) 2010 The Android Open Source Project

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

package com.qualcomm.qti.extsettings;

import android.app.AlertDialog;
import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiConfiguration.AuthAlgorithm;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.view.MenuItem;
import android.view.KeyEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import com.qualcomm.qti.extsettings.hotspot.Res;
import java.nio.charset.Charset;
import com.qualcomm.qti.extsettings.hotspot.WifiHotspotDialogHelpActivity;
import com.qualcomm.qti.extsettings.hotspot.AccountCheck;

import android.util.Log;

/**
 * Dialog to configure the SSID and security settings
 * for Access Point operation
 */
public class WifiApDialog extends AlertDialog implements View.OnClickListener,
        TextWatcher, AdapterView.OnItemSelectedListener {

    static final int BUTTON_SUBMIT = DialogInterface.BUTTON_POSITIVE;

    private final DialogInterface.OnClickListener mListener ;

    public static final int OPEN_INDEX = 0;
    public static final int WPA2_INDEX = 1;

    private View mView;
    private TextView mSsid;
    private int mSecurityTypeIndex = OPEN_INDEX;
    private EditText mPassword;
    private int mBandIndex = OPEN_INDEX;

    WifiConfiguration mWifiConfig;
    WifiManager mWifiManager;
    private Context mContext;

    //hide SSID selectable begin
    private View mShowAgainView;
    private CheckBox mShowAgain;
    private CheckBox mBroadcastSsid;
    //hide SSID selectable end

    private static final String TAG = "WifiApDialog";
    private static final String KEY_SSID_BROADCAST = "SsidBroadcast";

    public WifiApDialog(Context context, DialogInterface.OnClickListener listener,
            WifiConfiguration wifiConfig) {
        super(context);
        mListener = listener;
        if (wifiConfig == null) {
            WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            if (wifiManager != null) {
                wifiConfig = wifiManager.getWifiApConfiguration();
            }
        }
        mWifiConfig = wifiConfig;
        if (wifiConfig != null) {
            mSecurityTypeIndex = getSecurityTypeIndex(wifiConfig);
        }
        mWifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        mContext =  context;
        if (context instanceof WifiHotspotDialogHelpActivity) {
            setOwnerActivity(((WifiHotspotDialogHelpActivity)context));
        }
    }

    public static int getSecurityTypeIndex(WifiConfiguration wifiConfig) {
        if (wifiConfig.allowedKeyManagement.get(KeyMgmt.WPA2_PSK)) {
            return WPA2_INDEX;
        }
        return OPEN_INDEX;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
         if (id == android.R.id.home && !canCancel()) {
             return false;
        }
        return super.onOptionsItemSelected(item);
    }

    private boolean canCancel() {
        final Context context = getContext();
        if (Res.getSettingsBoolean(context, Res.FLAG_SOW_HELP_FOR_FIRST_USING)) {
            SharedPreferences sharedPreferences = context.
                    getSharedPreferences(AccountCheck.MY_PREF_FILE, context.MODE_PRIVATE);
            return sharedPreferences.getBoolean(AccountCheck.KEY_WIFIAP_BACKKEY_ENABLE, true);
        }
        return true;
    }

    public WifiConfiguration getConfig() {
        WifiConfiguration config = new WifiConfiguration();

        /**
         * TODO: SSID in WifiConfiguration for soft ap
         * is being stored as a raw string without quotes.
         * This is not the case on the client side. We need to
         * make things consistent and clean it up
         */
        config.SSID = mSsid.getText().toString();

        config.apBand = mBandIndex;
        //hide SSID selectable begin
        if (Res.getSettingsBoolean(getContext(), Res.FLAG_SHOW_BROADCAST_SSID)) {
            config.hiddenSSID = !(mBroadcastSsid.isChecked());
         }
        //hide SSID selectable end
        switch (mSecurityTypeIndex) {
            case OPEN_INDEX:
                config.allowedKeyManagement.set(KeyMgmt.NONE);
                return config;

            case WPA2_INDEX:
                config.allowedKeyManagement.set(KeyMgmt.WPA2_PSK);
                config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
                if (mPassword.length() != 0) {
                    String password = mPassword.getText().toString();
                    config.preSharedKey = password;
                }
                return config;
        }
        return null;
    }
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && (!canCancel())) {
            return false;
        }
        return super.onKeyDown(keyCode,event);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        boolean mInit = true;
        mView = getLayoutInflater().inflate(R.layout.wifi_ap_dialog, null);
        Spinner mSecurity = ((Spinner) mView.findViewById(R.id.security));
        final Spinner mChannel = (Spinner) mView.findViewById(R.id.choose_channel);

        setView(mView);
        setInverseBackgroundForced(true);

        Context context = getContext();

        setTitle(Res.getSettingsString(getContext(), Res.STRING_CONFIGURE_AP_TEXT));
        mView.findViewById(R.id.type).setVisibility(View.VISIBLE);
        mSsid = (TextView) mView.findViewById(R.id.ssid);
        mPassword = (EditText) mView.findViewById(R.id.password);

        ArrayAdapter <CharSequence> channelAdapter;
        String countryCode = mWifiManager.getCountryCode();
        if (!mWifiManager.isDualBandSupported() || countryCode == null) {
            //If no country code, 5GHz AP is forbidden
            Log.i(TAG,(!mWifiManager.isDualBandSupported() ? "Device do not support 5GHz " :"")
                    + (countryCode == null ? " NO country code" :"") +  " forbid 5GHz");
            channelAdapter = ArrayAdapter.createFromResource(mContext,
                    R.array.wifi_ap_band_config_2G_only, android.R.layout.simple_spinner_item);
            mWifiConfig.apBand = 0;
        } else {
            channelAdapter = ArrayAdapter.createFromResource(mContext,
                    R.array.wifi_ap_band_config_full, android.R.layout.simple_spinner_item);
        }

        channelAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        setButton(BUTTON_SUBMIT, Res.getSettingsString(context, Res.STRING_WIFI_SAVE), mListener);
        setButton(DialogInterface.BUTTON_NEGATIVE,
        Res.getSettingsString(context, Res.STRING_WIFI_CANCEL), mListener);
        mBroadcastSsid = (CheckBox) mView.findViewById(R.id.broadcast_ssid);

        //hide SSID selectable begin
        if (Res.getSettingsBoolean(getContext(), Res.FLAG_SHOW_BROADCAST_SSID)) {
            mBroadcastSsid.setChecked(!mWifiConfig.hiddenSSID);
        }
        //hide SSID selectable end

        if (mWifiConfig != null) {
            mSsid.setText(mWifiConfig.SSID);
            if (mWifiConfig.apBand == 0) {
               mBandIndex = 0;
            } else {
               mBandIndex = 1;
            }

            mSecurity.setSelection(mSecurityTypeIndex);
            if (mSecurityTypeIndex == WPA2_INDEX) {
                mPassword.setText(mWifiConfig.preSharedKey);
            }
        }

        mChannel.setAdapter(channelAdapter);
        mChannel.setOnItemSelectedListener(
                new AdapterView.OnItemSelectedListener() {
                    boolean mInit = true;
                    @Override
                    public void onItemSelected(AdapterView<?> adapterView, View view, int position,
                                               long id) {
                        if (!mInit) {
                            mBandIndex = position;
                            mWifiConfig.apBand = mBandIndex;
                            Log.i(TAG, "config on channelIndex : " + mBandIndex + " Band: " +
                                    mWifiConfig.apBand);
                        } else {
                            mInit = false;
                            mChannel.setSelection(mBandIndex);
                        }

                    }

                    @Override
                    public void onNothingSelected(AdapterView<?> adapterView) {

                    }
                }
        );

        mSsid.addTextChangedListener(this);
        CheckBox showPassWord = (CheckBox) mView.findViewById(R.id.show_password);
        showPassWord.setChecked(true);
        showPassWord.setOnClickListener(this);
        mPassword.requestFocus();
        mPassword.setInputType(
                InputType.TYPE_CLASS_TEXT |
                (showPassWord.isChecked() ?
                InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                InputType.TYPE_TEXT_VARIATION_PASSWORD));
        mPassword.addTextChangedListener(this);
        mSecurity.setOnItemSelectedListener(this);

        super.onCreate(savedInstanceState);

        showSecurityFields();
        validate();

        //hide SSID selectable begin
        if (Res.getSettingsBoolean(getContext(), Res.FLAG_SHOW_BROADCAST_SSID)) {
            mBroadcastSsid.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    if (!isChecked) {
                        final Context context = getContext();
                        mShowAgainView = getLayoutInflater().inflate(R.layout.not_show_again, null);
                        mShowAgain = (CheckBox)mShowAgainView.findViewById(R.id.check);
                        SharedPreferences sharedpreferences = context.getSharedPreferences(
                                AccountCheck.MY_PREF_FILE, Context.MODE_PRIVATE);
                        boolean showagain = sharedpreferences.getBoolean(KEY_SSID_BROADCAST, true);
                        if (showagain) {
                            ShowAgainConfirm();
                        }
                    }
                }
            });
        }
        //hide SSID selectable end
    }

    public void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        mPassword.setInputType(
                InputType.TYPE_CLASS_TEXT |
                (((CheckBox) mView.findViewById(R.id.show_password)).isChecked() ?
                InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                InputType.TYPE_TEXT_VARIATION_PASSWORD));
    }

    private void validate() {
        String mSsidString = mSsid.getText().toString();
        if ((mSsid != null && mSsid.length() == 0)
                || ((mSecurityTypeIndex == WPA2_INDEX) && mPassword.length() < 8)
                || (mSsid != null &&
                Charset.forName("UTF-8").encode(mSsidString).limit() > 32)) {
            getButton(BUTTON_SUBMIT).setEnabled(false);
        } else {
            getButton(BUTTON_SUBMIT).setEnabled(true);
        }
    }

    public void onClick(View view) {
        mPassword.setInputType(
                InputType.TYPE_CLASS_TEXT | (((CheckBox) view).isChecked() ?
                InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                InputType.TYPE_TEXT_VARIATION_PASSWORD));
    }

    public void onTextChanged(CharSequence s, int start, int before, int count) {
    }

    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
    }

    public void afterTextChanged(Editable editable) {
        validate();
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        mSecurityTypeIndex = position;
        final Context context = getContext();
        if (Res.getSettingsBoolean(getContext(), Res.FLAG_SHOW_ALERT_FOR_OPEN)) {
            if (mSecurityTypeIndex == OPEN_INDEX) {
                AlertDialog.Builder alert = new AlertDialog.Builder(context);
                alert.setMessage(R.string.open_security_settings_note);
                alert.setPositiveButton("Ok", null);
                alert.show();
            }
        }
        showSecurityFields();
        validate();
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {
    }

    private void showSecurityFields() {
        if (!Res.getSettingsBoolean(getContext(), Res.FLAG_SHOW_BROADCAST_SSID)) {
            mView.findViewById(R.id.checkbox).setVisibility(View.GONE);
        } else {
            mView.findViewById(R.id.checkbox).setVisibility(View.VISIBLE);
        }
        if (mSecurityTypeIndex == OPEN_INDEX) {
            mView.findViewById(R.id.fields).setVisibility(View.GONE);
            return;
        }
        mView.findViewById(R.id.fields).setVisibility(View.VISIBLE);
    }

    //hide SSID selectable begin
    private void ShowAgainConfirm() {
        final Context context = getContext();
        AlertDialog.Builder alertdialog =  new AlertDialog.Builder(context);
        alertdialog.setTitle(Res.getSettingsString(context, Res.STRING_SSID_DLG_TITLE));
        alertdialog.setMessage(Res.getSettingsString(context, Res.STRING_SSID_DLG_TEXT));
        alertdialog.setView(mShowAgainView);
        alertdialog.setPositiveButton(Res.getSettingsString(context,
                Res.STRING_OKAY), new DialogInterface.OnClickListener() {
            @Override
            public void onClick( DialogInterface dialog, int which) {
                SharedPreferences sharedpreferences = context.getSharedPreferences(
                        AccountCheck.MY_PREF_FILE, Context.MODE_PRIVATE);
                Editor editor = sharedpreferences.edit();
                if (mShowAgain.isChecked()) {
                    editor.putBoolean(KEY_SSID_BROADCAST, false);
                } else {
                    editor.putBoolean(KEY_SSID_BROADCAST, true);
                }
                editor.commit();
            }
        });
        alertdialog.show();
    }
    //hide SSID selectable end
}
