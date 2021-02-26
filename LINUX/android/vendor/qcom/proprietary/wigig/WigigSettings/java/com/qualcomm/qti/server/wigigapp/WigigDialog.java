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
import android.net.LinkAddress;
import android.os.Handler;
import android.text.Editable;
import android.text.InputType;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.net.wifi.WifiConfiguration;

import com.qualcomm.qti.wigig.WigigManager;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.regex.Pattern;

public class WigigDialog extends AlertDialog {

    private View mDialogView;
    private TextView mPasswordView;
    private static final String TAG = "WigigDialog";
    private final Context mContext;
    private final WigigManager mWigigManager;
    private AlertDialog mAlertDialog;
    private AccessPoint mAccessPoint;
    private EditText mPassword;
    private Spinner mIpSettingsSpinner;
    private TextView mIpAddressView;
    private TextView mGatewayView;
    private TextView mNetworkPrefixLengthView;
    private TextView mDns1View;
    private TextView mDns2View;
    private Spinner mSecuritySpinner;
    private TextView mSsidView;

    private final Handler mTextViewChangedHandler;

    private boolean mIsApSecured;

    private static final int MIN_PW_LEN = 8;

    /* This value comes from "wifi_ip_settings" resource array */
    private static final int DHCP = 0;
    private static final int STATIC_IP = 1;

    public class IpParams{
        LinkAddress ipAddress;
        InetAddress gateway;
        ArrayList<InetAddress> dnsServers = new ArrayList<>();
    }

    private final IpParams mIpParams = new IpParams();

    public WigigDialog(Context context, WigigManager wigigManager) {
        super(context);

        mTextViewChangedHandler = new Handler();
        mContext = context;
        mWigigManager = wigigManager;
    }

    /* show submit button if password, ip and proxy settings are valid */
    private void enableSubmitIfAppropriate() {
        boolean enabled;

        if ((mPasswordView != null && mIsApSecured && mPasswordView.length() < MIN_PW_LEN)  ||
                (mSsidView != null && mSsidView.length() == 0)) {
            enabled = false;
        } else
            enabled = (validateIpConfigFields(mIpParams) == 0);

        if ((mAccessPoint.GetApStatus() != AccessPoint.AP_NOT_CONNECTED) && mIsApSecured &&
                (mPasswordView.length() == 0) && (validateIpConfigFields(mIpParams) == 0))
            enabled = true;

        mAlertDialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(enabled);
    }

    private WifiConfiguration getConfig(){
        WifiConfiguration config;

        if (mAccessPoint.GetApStatus() == AccessPoint.AP_NOT_CONNECTED) {
            config = new WifiConfiguration();
            if (mSsidView != null && mSecuritySpinner != null) {
                config.SSID = mSsidView.getText().toString();
                config.hiddenSSID = true;
            } else
                config.SSID = mAccessPoint.ssid;

            if (mIsApSecured)
                config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            else
                config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
        } else {
            config = WigigSettingsActivity.getWifiConfigurationByAp(mWigigManager, mAccessPoint);
            if (config == null) {
                return null;
            }
        }

        if (mPassword != null && mPassword.length() != 0) {
            config.preSharedKey = '"' + mPassword.getText().toString() + '"';
        }

        setIpConfiguration(config);

        return config;
    }

    private static Object newInstance(String className) throws ClassNotFoundException, InstantiationException, IllegalAccessException, NoSuchMethodException, IllegalArgumentException, InvocationTargetException
    {
        return newInstance(className, new Class<?>[0], new Object[0]);
    }

    private static Object newInstance(String className, Class<?>[] parameterClasses, Object[] parameterValues) throws NoSuchMethodException, InstantiationException, IllegalAccessException, IllegalArgumentException, InvocationTargetException, ClassNotFoundException
    {
        Class<?> clz = Class.forName(className);
        Constructor<?> constructor = clz.getConstructor(parameterClasses);
        return constructor.newInstance(parameterValues);
    }

    @SuppressWarnings({ "unchecked", "rawtypes" })
    private static Object getEnumValue(String enumClassName, String enumValue) throws ClassNotFoundException
    {
        Class<Enum> enumClz = (Class<Enum>)Class.forName(enumClassName);
        return Enum.valueOf(enumClz, enumValue);
    }

    private static void setField(Object object, String fieldName, Object value) throws IllegalAccessException, IllegalArgumentException, NoSuchFieldException
    {
        Field field = object.getClass().getDeclaredField(fieldName);
        field.set(object, value);
    }

    private static <T> T getField(Object object, String fieldName, Class<T> type) throws IllegalAccessException, IllegalArgumentException, NoSuchFieldException
    {
        Field field = object.getClass().getDeclaredField(fieldName);
        return type.cast(field.get(object));
    }

    private static void callMethod(Object object, String methodName, String[] parameterTypes, Object[] parameterValues) throws ClassNotFoundException, IllegalAccessException, IllegalArgumentException, InvocationTargetException, NoSuchMethodException
    {
        Class<?>[] parameterClasses = new Class<?>[parameterTypes.length];
        for (int i = 0; i < parameterTypes.length; i++) {
            parameterClasses[i] = Class.forName(parameterTypes[i]);
        }

        Method method = object.getClass().getDeclaredMethod(methodName, parameterClasses);
        method.invoke(object, parameterValues);
    }

    private boolean _isIpStatic()
            throws IllegalAccessException, IllegalArgumentException, InvocationTargetException, NoSuchMethodException {
        WifiConfiguration wc = WigigSettingsActivity.getWifiConfigurationByAp(mWigigManager, mAccessPoint);
        if (wc != null) {
            Object ipAssignment = wc.getClass().getMethod("getIpAssignment").invoke(wc);

            return (ipAssignment.toString().equals("STATIC"));
        }

        return false;
    }

    private boolean isIpStatic() {
        try {
            return _isIpStatic();
        } catch (Exception e) {
            Log.e(TAG, "Exception at isIpStatic!");
        }
        return  false;
    }

    @SuppressWarnings("unchecked")
    private void _getIpConfiguration(WifiConfiguration config, IpParams ipParams)
            throws IllegalAccessException, IllegalArgumentException, InvocationTargetException,
            NoSuchMethodException, NoSuchFieldException
    {
        Object staticConf = config.getClass().getMethod("getStaticIpConfiguration").invoke(config);
        if (staticConf != null) {
            ipParams.ipAddress = (LinkAddress)staticConf.getClass().getField("ipAddress").get(staticConf);
            ipParams.gateway = (Inet4Address)staticConf.getClass().getField("gateway").get(staticConf);
            ipParams.dnsServers = (ArrayList<InetAddress>)staticConf.getClass().getField("dnsServers").get(staticConf);
        }
    }

    private boolean getIpConfiguration(WifiConfiguration config, IpParams ipParams) {
        if (mIpSettingsSpinner != null) {
            try {
                _getIpConfiguration(config, ipParams);
                return true;
            } catch (Exception e) {
                Log.e(TAG, "Exception at getIpConfiguration!");
            }
        }
        return false;
    }

    @SuppressWarnings("unchecked")
    private void _setIpConfiguration(boolean isStaticIp, WifiConfiguration config, IpParams ipParams)
            throws ClassNotFoundException, IllegalAccessException, IllegalArgumentException, InvocationTargetException,
            NoSuchMethodException, NoSuchFieldException, InstantiationException
    {
        if (isStaticIp) {
            // First set up IpAssignment to STATIC.
            Object ipAssignment = getEnumValue("android.net.IpConfiguration$IpAssignment", "STATIC");
            callMethod(config, "setIpAssignment", new String[]{"android.net.IpConfiguration$IpAssignment"}, new Object[]{ipAssignment});

            // Then set properties in StaticIpConfiguration.
            Object staticIpConfig = newInstance("android.net.StaticIpConfiguration");

            setField(staticIpConfig, "ipAddress", ipParams.ipAddress);
            setField(staticIpConfig, "gateway", ipParams.gateway);

            for (InetAddress ds : ipParams.dnsServers)
                getField(staticIpConfig, "dnsServers", ArrayList.class).add(ds);

            callMethod(config, "setStaticIpConfiguration", new String[]{"android.net.StaticIpConfiguration"}, new Object[]{staticIpConfig});
        } else {
            Object ipAssignment = getEnumValue("android.net.IpConfiguration$IpAssignment", "DHCP");
            callMethod(config, "setIpAssignment", new String[]{"android.net.IpConfiguration$IpAssignment"}, new Object[]{ipAssignment});
        }
    }

    private boolean setIpConfiguration(WifiConfiguration config){
        if (mIpSettingsSpinner != null) {
            boolean isStaticIp = (mIpSettingsSpinner.getSelectedItemPosition() == STATIC_IP);
            if (!isStaticIp || validateIpConfigFields(mIpParams) == 0) {
                try {
                    _setIpConfiguration(isStaticIp, config, mIpParams);
                    return true;
                } catch (Exception e) {
                    Log.e(TAG, "Exception at setIpConfiguration!");
                }
            }
        }
        return false;
    }

    @SuppressWarnings("unchecked")
    private LinkAddress _setLinkAddress(Inet4Address inetAddr, int networkPrefixLength)
            throws ClassNotFoundException, IllegalAccessException, IllegalArgumentException, InvocationTargetException,
            NoSuchMethodException, InstantiationException
    {
        return (LinkAddress)newInstance("android.net.LinkAddress", new Class<?>[]{InetAddress.class, int.class}, new Object[]{inetAddr, networkPrefixLength});
    }

    private LinkAddress setLinkAddress(Inet4Address inetAddr, int networkPrefixLength) {
        try {
            return (_setLinkAddress(inetAddr, networkPrefixLength));
        } catch (Exception e) {
            Log.e(TAG, "Exception at setLinkAddress!");
        }
        return null;
    }

    private static final Pattern PATTERN = Pattern.compile(
            "^(([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");

    private Inet4Address getIPv4Address(String text) {
        Inet4Address inetAddr;
        if (PATTERN.matcher(text).matches()) {
            try {
                inetAddr = (Inet4Address) InetAddress.getByName(text);
                return inetAddr;
            } catch (java.net.UnknownHostException e){
                Log.e(TAG, "Exception at getIPv4Address!");
            }
        }
        return null;
    }

    private static InetAddress getNetworkPart(InetAddress address, int prefixLength) {
        if (address == null) {
                throw new RuntimeException("getNetworkPart doesn't accept null address");
        }
        byte[] array = address.getAddress();

        if (prefixLength < 0 || prefixLength > array.length * 8) {
            throw new RuntimeException("getNetworkPart - bad prefixLength");
        }

        int offset = prefixLength / 8;
        int reminder = prefixLength % 8;
        byte mask = (byte)(0xFF << (8 - reminder));

        if (offset < array.length) array[offset] = (byte)(array[offset] & mask);

        offset++;

        for (; offset < array.length; offset++) {
            array[offset] = 0;
        }

        InetAddress netPart;
        try {
            netPart = InetAddress.getByAddress(array);
        } catch (UnknownHostException e) {
            throw new RuntimeException("getNetworkPart error - " + e.toString());
        }
        return netPart;
    }

    private int validateIpConfigFields(IpParams ipParams) {
        if (mIpSettingsSpinner!= null && mIpSettingsSpinner.getSelectedItemPosition() == STATIC_IP) {

            if (mIpAddressView == null) return 0;

            String ipAddr = mIpAddressView.getText().toString();
            if (TextUtils.isEmpty(ipAddr)) return R.string.wifi_ip_settings_invalid_ip_address;

            Inet4Address inetAddr = getIPv4Address(ipAddr);
            if (inetAddr == null) {
                return R.string.wifi_ip_settings_invalid_ip_address;
            }

            String networkPrefixLengthStr = mNetworkPrefixLengthView.getText().toString();
            String dns = mDns1View.getText().toString();

            boolean initialConfiguration = false;

            if (TextUtils.isEmpty(networkPrefixLengthStr) && TextUtils.isEmpty(dns))
                initialConfiguration = true;

            int networkPrefixLength = Integer.parseInt(mContext.getString(R.string.wifi_network_prefix_length_hint));
            try {
                networkPrefixLength = Integer.parseInt(networkPrefixLengthStr);
                if (networkPrefixLength < 0 || networkPrefixLength > 32) {
                    return R.string.wifi_ip_settings_invalid_network_prefix_length;
                }
                ipParams.ipAddress = setLinkAddress(inetAddr, networkPrefixLength);

            } catch (NumberFormatException e) {
                if (initialConfiguration)
                    // Set the hint as default after user types in ip address
                    mNetworkPrefixLengthView.setText(mContext.getString(R.string.wifi_network_prefix_length_hint));
                else
                    return R.string.wifi_ip_settings_invalid_network_prefix_length;
            }

            String gateway = mGatewayView.getText().toString();
            if (TextUtils.isEmpty(gateway)) {
                try {
                    //Extract a default gateway from IP address
                    InetAddress netPart = getNetworkPart(inetAddr, networkPrefixLength);
                    byte[] addr = netPart.getAddress();
                    addr[addr.length-1] = 1;
                    mGatewayView.setText(InetAddress.getByAddress(addr).getHostAddress());
                } catch (RuntimeException | java.net.UnknownHostException e) {
                    Log.e(TAG, "Exception at validateIpConfigFields!");
                    return R.string.wifi_ip_settings_invalid_gateway;

                }
            } else {
                InetAddress gatewayAddr = getIPv4Address(gateway);
                if (gatewayAddr == null) {
                    return R.string.wifi_ip_settings_invalid_gateway;
                }
                ipParams.gateway = gatewayAddr;
            }

            ipParams.dnsServers.clear();

            InetAddress dnsAddr;
            if (TextUtils.isEmpty(dns)) {
                if (initialConfiguration)
                    //If everything else is valid, provide hint as a default option
                    mDns1View.setText(mContext.getString(R.string.wifi_dns1_hint));
                else
                    return R.string.wifi_ip_settings_invalid_dns;
            } else {
                dnsAddr = getIPv4Address(dns);
                if (dnsAddr == null) {
                    return R.string.wifi_ip_settings_invalid_dns;
                }
                ipParams.dnsServers.add(dnsAddr);
            }

            if (mDns2View.length() > 0) {
                dns = mDns2View.getText().toString();
                dnsAddr = getIPv4Address(dns);
                if (dnsAddr == null) {
                    return R.string.wifi_ip_settings_invalid_dns;
                }
                ipParams.dnsServers.add(dnsAddr);
            }
        }

        return 0;
    }

    private final TextWatcher ipSettingsTextWatcher = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {
        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
        }

        @Override
        public void afterTextChanged(Editable s) {
            mTextViewChangedHandler.post(new Runnable() {
                public void run() {
                    enableSubmitIfAppropriate();
                }
            });
        }
    };

    private void showSecurityFields() {
        if (!mIsApSecured) {
            mDialogView.findViewById(R.id.security_fields).setVisibility(View.GONE);
            return;
        }
        mDialogView.findViewById(R.id.security_fields).setVisibility(View.VISIBLE);

        if (mPasswordView == null) {
            mPasswordView = (TextView) mDialogView.findViewById(R.id.password);
            mPasswordView.addTextChangedListener(ipSettingsTextWatcher);
            ((CheckBox) mDialogView.findViewById(R.id.show_password)).setOnCheckedChangeListener(sAdvancedOptions);
        }

        if (mAccessPoint.GetApStatus() != AccessPoint.AP_NOT_CONNECTED)
            mPasswordView.setHint(R.string.wifi_unchanged);
    }

    private void showIpConfigFields() {
        if (mIpSettingsSpinner.getSelectedItemPosition() == STATIC_IP) {
            mDialogView.findViewById(R.id.staticip).setVisibility(View.VISIBLE);
            mIpAddressView = (TextView) mDialogView.findViewById(R.id.ipaddress);
            mIpAddressView.addTextChangedListener(ipSettingsTextWatcher);
            mGatewayView = (TextView) mDialogView.findViewById(R.id.gateway);
            mGatewayView.addTextChangedListener(ipSettingsTextWatcher);
            mNetworkPrefixLengthView = (TextView) mDialogView.findViewById(
                    R.id.network_prefix_length);
            mNetworkPrefixLengthView.addTextChangedListener(ipSettingsTextWatcher);
            mDns1View = (TextView) mDialogView.findViewById(R.id.dns1);
            mDns1View.addTextChangedListener(ipSettingsTextWatcher);
            mDns2View = (TextView) mDialogView.findViewById(R.id.dns2);
            mDns2View.addTextChangedListener(ipSettingsTextWatcher);

            if (isIpStatic()) {
                WifiConfiguration staticConfig = WigigSettingsActivity.getWifiConfigurationByAp(
                        mWigigManager, mAccessPoint);
                getIpConfiguration(staticConfig, mIpParams);
                if (staticConfig != null) {
                    if (mIpParams.ipAddress != null) {
                        mIpAddressView.setText(mIpParams.ipAddress.getAddress().getHostAddress());
                        mNetworkPrefixLengthView.setText(Integer.toString(mIpParams.ipAddress.getPrefixLength()));
                    }
                    if (mIpParams.gateway != null)
                        mGatewayView.setText(mIpParams.gateway.getHostAddress());

                    Iterator<InetAddress> dnsIterator = mIpParams.dnsServers.iterator();
                    if (dnsIterator.hasNext())
                        mDns1View.setText(dnsIterator.next().getHostAddress());
                    if (dnsIterator.hasNext())
                        mDns2View.setText(dnsIterator.next().getHostAddress());
                }
            }
        } else {
            mDialogView.findViewById(R.id.staticip).setVisibility(View.GONE);
        }
    }

    private final AdapterView.OnItemSelectedListener mIpSettingsSpinnerSelect = new AdapterView.OnItemSelectedListener(){
        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            if (parent == mSecuritySpinner) {
                mIsApSecured = position == AccessPoint.SECURITY_PSK;
                showSecurityFields();
            } else if (parent == mIpSettingsSpinner) {
                showIpConfigFields();
            }
            enableSubmitIfAppropriate();
        }

        @Override
        public void onNothingSelected (AdapterView < ? > parent){
        }
    };

    private final CompoundButton.OnCheckedChangeListener sAdvancedOptions = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton view, boolean isChecked) {
            if (view.getId() == R.id.show_password) {
                int pos = mPasswordView.getSelectionEnd();
                mPasswordView.setInputType(
                        InputType.TYPE_CLASS_TEXT | (isChecked ?
                                InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                                InputType.TYPE_TEXT_VARIATION_PASSWORD));
                if (pos >= 0) {
                    ((EditText) mPasswordView).setSelection(pos);
                }
            } else if (view.getId() == R.id.wifi_advanced_togglebox) {
                int visibility = isChecked ? View.VISIBLE : View.GONE;
                mDialogView.findViewById(R.id.wifi_advanced_fields).setVisibility(visibility);
            }
        }
    };

    protected void CreateDialog(AccessPoint accessPoint, boolean edit) {
        Spinner mProxySettingsSpinner;
        mAccessPoint = accessPoint;
        mSsidView = null;
        mSecuritySpinner = null;

        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);

        mDialogView = getLayoutInflater().inflate(R.layout.wifi_dialog, null);

        if (edit) {
            mIpSettingsSpinner = (Spinner) mDialogView.findViewById(R.id.ip_settings);
            mIpSettingsSpinner.setOnItemSelectedListener(mIpSettingsSpinnerSelect);
            mProxySettingsSpinner = (Spinner) mDialogView.findViewById(R.id.proxy_settings);
            mProxySettingsSpinner.setEnabled(false);

            if (accessPoint == null) {
                mAccessPoint = new AccessPoint(mContext, mContext.getString(R.string.menu_add_network), 0, 1);
                mDialogView.findViewById(R.id.type).setVisibility(View.VISIBLE);

                mSsidView = (EditText) mDialogView.findViewById(R.id.ssid);
                mSsidView.addTextChangedListener(ipSettingsTextWatcher);
                mSecuritySpinner = (Spinner) mDialogView.findViewById(R.id.security);
                mSecuritySpinner.setOnItemSelectedListener(mIpSettingsSpinnerSelect);
            }

            ((CheckBox)mDialogView.findViewById(R.id.wifi_advanced_togglebox)).setOnCheckedChangeListener(sAdvancedOptions);

            showIpConfigFields();

            mPassword = (EditText) mDialogView.findViewById(R.id.password);

            mIsApSecured = mAccessPoint.security == AccessPoint.SECURITY_PSK;
            mPasswordView = null;

            if (mAccessPoint.GetApStatus() != AccessPoint.AP_NOT_CONNECTED) {
                boolean isIpStat = isIpStatic();
                mIpSettingsSpinner.setSelection(isIpStat ? STATIC_IP : DHCP);
                mDialogView.findViewById(R.id.wifi_advanced_fields).setVisibility(isIpStat ? View.VISIBLE : View.GONE);
                ((CheckBox)mDialogView.findViewById(R.id.wifi_advanced_togglebox)).setChecked(isIpStat);
            }

        } else {
            builder.setNeutralButton(R.string.forget, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    WifiConfiguration config = WigigSettingsActivity.getWifiConfigurationByAp(mWigigManager, mAccessPoint);
                    if (config != null) {
                        ((WigigSettingsActivity) mContext).forget(config.networkId);
                    }
                }
            });
            mDialogView.findViewById(R.id.wifi_dialog).setVisibility(View.GONE);
        }

        builder.setView(mDialogView);
        builder.setTitle(mAccessPoint.ssid);

        if (mAccessPoint.GetApStatus() != AccessPoint.AP_SAVED_AWAY || edit) {
            final int positiveButton = (mAccessPoint.GetApStatus() == AccessPoint.AP_NOT_CONNECTED &&
                    (mSsidView == null && mSecuritySpinner == null)) || !edit ?
                    R.string.connect : R.string.save;
            builder.setPositiveButton(positiveButton, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    WifiConfiguration config = getConfig();
                    if (config != null) {
                        if (mSsidView != null && mSecuritySpinner != null) { //when adding manually, check for existing ap
                            boolean isApSecured = config.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK);
                            if (WigigSettingsActivity.getWifiConfigurationByParams(mWigigManager, config.SSID, isApSecured) != null) {
                                Log.i(TAG, "The added network already exists");
                                return;
                            }
                        }

                        if (positiveButton == R.string.connect) {
                            if (mAccessPoint.GetApStatus() == AccessPoint.AP_NOT_CONNECTED)
                                ((WigigSettingsActivity) mContext).connect(config);
                            else
                                ((WigigSettingsActivity) mContext).connect(config.networkId);
                        } else
                            ((WigigSettingsActivity) mContext).save(config);
                    }
                }
            });
        }

        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                dialog.dismiss();
            }
        });

        mAlertDialog = builder.create();
        mAlertDialog.show();

        if (edit) {
            showSecurityFields();
            enableSubmitIfAppropriate();
        }
    }

    public boolean isShowing() {
        return mAlertDialog != null && mAlertDialog.isShowing();
    }
}