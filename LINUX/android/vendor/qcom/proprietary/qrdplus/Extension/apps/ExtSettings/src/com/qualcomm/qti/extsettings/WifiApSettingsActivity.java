/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2014, The Linux Foundation. All Rights Reserved.
 */

package com.qualcomm.qti.extsettings;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.SystemProperties;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.preference.ListPreference;
import android.provider.Settings.System;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.NumberPicker;
import android.text.TextUtils;
import com.qualcomm.qti.extsettings.hotspot.HotspotService;
import com.qualcomm.qti.extsettings.hotspot.AccountCheck;
import com.qualcomm.qti.extsettings.hotspot.Res;
import com.qualcomm.qti.extsettings.hotspot.WifiHotspotDialogHelpActivity;
import com.qualcomm.qti.extsettings.sta.StaDevice;
import com.qualcomm.qti.extsettings.sta.StaMonitor;
import com.qualcomm.qti.extsettings.R;

import java.util.ArrayList;
import java.util.List;

/*
 * Displays preferences for Tethering.
 */
public class WifiApSettingsActivity extends PreferenceActivity implements
        DialogInterface.OnClickListener , Preference.OnPreferenceChangeListener{

    public static final String TAG = "HotspotSettings";
    private static final boolean DEBUG = true;

    private static final int PROVISION_REQUEST = 0;
    private static final int DIALOG_AP_SETTINGS = 1;
    private static final int CONFIG_SUBTEXT = R.string.wifiap_configure_subtext;
    private static final int MENU_HELP = Menu.FIRST;

    private static final String WIFI_AP_SSID_AND_SECURITY = "wifi_ap_ssid_and_security";
    private static final String AP_CONNECTED_STATE_CHANGED_ACTION =
            "codeaurora.net.conn.TETHER_CONNECT_STATE_CHANGED";
    private static final String KEY_AP_DEVICE_LIST = "ap_device_list";
    private static final String CONN_STATE = "ConnectedState";
    private static final String CONN_ADDR = "ConnectedAddress";
    private static final String CONN_NAME = "ConnectedName";
    private static final String CONN_COUNT = "ConnectedCount";

    private BroadcastReceiver mReceiver;
    private IntentFilter mFilter;
    private List<StaDevice> mTetherConnectedStaList = new ArrayList<StaDevice>();
    private List<String> mConnectedNameList = new ArrayList<String>();
    private List<String> mConnectedAddressList = new ArrayList<String>();
    private PreferenceCategory mWifiApListPrefCategory;
    private TextView mwifiApHint;
    private WifiApSwitch mWifiApSwitch;
    private ConnectivityManager mCm;
    private WifiApDialog mDialog;
    private WifiManager mWifiManager;
    private WifiConfiguration mWifiApConfig = null;
    private String[] mProvisionApp;
    private Preference mCreateNetworkPref;
    //maximun connection modification begin
    private PreferenceScreen mMaximumConnections = null;
    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;
    private int niSelectedMaximum = DEFAULT_MAXIMUN_VALUE;
    private String maximumConnectionsDialogTitle = null;
    private String maximumConnectionsPositive = null;
    private String maximumConnectionsNegative = null;
    private static final String MAXIMUM_CONNECTIONS_BUTTON = "maximum_connections";
    private static final String WIFI_HOTSPOT_MAX_CLIENT_NUM = "WIFI_HOTSPOT_MAX_CLIENT_NUM";
    private static final String KEY_MAXIMUN = "maximum";
    private static final int DEFAULT_MAXIMUN_VALUE = 8;
    //maximun connection modification end
    //hotspot working mode begin
    private static final String KEY_HOTSPOT_MODE = "wifi_hotspot_mode";
    private ListPreference mHotspotMode;
    //hotspot working mode end
    private boolean mIsShowhelp = false;
    private Context mContext = null;
    public static final int WIFI_TETHERING      = 0;

    public boolean AP_ENABLE_AND_CHANGE_CONFIG = false;

    private final StaMonitor.StaChangedListener mStaChangedListener =
            new StaMonitor.StaChangedListener() {
        @Override
        public void onChanged(List<StaDevice> staDevices) {
            constructConnectedDevices(staDevices);
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.wifi_ap_container);
        addPreferencesFromResource(R.xml.wifi_ap_settings);

        mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        mWifiApConfig = mWifiManager.getWifiApConfiguration();
        mProvisionApp = this.getResources().getStringArray(
                com.android.internal.R.array.config_mobile_hotspot_provision_app);

        mFilter = new IntentFilter();
        mFilter.addAction(WifiManager.WIFI_AP_STATE_CHANGED_ACTION);
        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                handleEvent(context, intent);
            }
        };

        ((ExtSettingsApplication)getApplication()).getStaMonitor()
                .addListener(mStaChangedListener);

        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setHomeButtonEnabled(true);

        mCreateNetworkPref = findPreference(WIFI_AP_SSID_AND_SECURITY);
        if (mWifiApConfig == null) {
            mCreateNetworkPref.setSummary(String.format(
                    Res.getSettingsString(this, Res.STRING_WIFIAP_CONFIGURE_SUBTEXT),
                    getString(R.string.wifiap_configure_ssid_default) + " ",
                    getSecurityTypeIndexToString()));
        } else {
            mCreateNetworkPref.setSummary(String.format(
                    Res.getSettingsString(this, Res.STRING_WIFIAP_CONFIGURE_SUBTEXT),
                    getApSsid() + " ", getSecurityTypeIndexToString()));
        }
        mCreateNetworkPref.setTitle(Res.getSettingsString(this, Res.STRING_CONFIGURE_AP_TEXT));

        PreferenceScreen root = getPreferenceScreen();
        mWifiApListPrefCategory = (PreferenceCategory) root.findPreference(KEY_AP_DEVICE_LIST);

        Switch actionBarSwitch = new Switch(this);
        final int padding = getResources().getDimensionPixelSize(
                R.dimen.action_bar_switch_padding);
        actionBarSwitch.setPaddingRelative(0, 0, padding, 0);
        getActionBar().setDisplayOptions(ActionBar.DISPLAY_SHOW_CUSTOM,
                ActionBar.DISPLAY_SHOW_CUSTOM);
        getActionBar().setCustomView(actionBarSwitch, new ActionBar.LayoutParams(
                ActionBar.LayoutParams.WRAP_CONTENT,
                ActionBar.LayoutParams.WRAP_CONTENT,
                Gravity.CENTER_VERTICAL | Gravity.END));
        getActionBar().setTitle(Res.getSettingsString(this, Res.STRING_WIFI_TETHER_TEXT));

        mWifiApSwitch = new WifiApSwitch(this, this, actionBarSwitch);
        mCm = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

        mwifiApHint = (TextView) findViewById(R.id.empty);
        mwifiApHint.setText(Res.getSettingsString(this, Res.STRING_WIFIAP_SETTINGS_DESC));
        //modify maximun connection number begin
        if (Res.getSettingsBoolean(this, Res.FLAG_SHOW_MAXIMUN_CONNECTION_ENABLE)) {
            sharedPreferences = this.getSharedPreferences(AccountCheck.MY_PREF_FILE, Activity.MODE_PRIVATE);
            editor = sharedPreferences.edit();
            mMaximumConnections = (PreferenceScreen) findPreference(MAXIMUM_CONNECTIONS_BUTTON);
            maximumConnectionsDialogTitle = Res.getSettingsString(this, Res.MAXIMUN_CONNECTION_DLG_TITLE);
            maximumConnectionsPositive =Res.getSettingsString(this, Res.MAXIMUN_CONNECTION_DLG_POSITIVE);
            maximumConnectionsNegative = Res.getSettingsString(this, Res.MAXIMUN_CONNECTION_DLG_NEGATIVE);
            mMaximumConnections.setSummary(""+(sharedPreferences.getInt(KEY_MAXIMUN,8)));
        } else {
            getPreferenceScreen().removePreference(findPreference(MAXIMUM_CONNECTIONS_BUTTON));
        }
        //modify maximun connection number end
        //select hotspot working mode begin
        mHotspotMode = (ListPreference) getPreferenceScreen().findPreference(KEY_HOTSPOT_MODE);
        if (Res.getSettingsBoolean(this, Res.FLAG_EDIT_HOTSPOT_TIMEOUT_ENABLE)) {
            mHotspotMode.setOnPreferenceChangeListener(this);
            String hotspotMode = HotspotService.getHotspotMode(this);
            mHotspotMode.setValue(hotspotMode);
            updateHotspotModeSummary(hotspotMode);
        } else {
            this.getPreferenceScreen().removePreference(mHotspotMode);
        }
        mContext = this;
        constructConnectedDevices();
        //select hotspot working mode end
    }

    @Override
    public void onResume() {
        super.onResume();
        log("onResume..");
        mWifiApSwitch.resume();
        this.registerReceiver(mReceiver, mFilter);
    }

    @Override
    public void onPause() {
        super.onPause();
        this.unregisterReceiver(mReceiver);
        mWifiApSwitch.pause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ((ExtSettingsApplication)getApplication()).getStaMonitor()
                .removeListener(mStaChangedListener);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.wifi_ap_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        int id = item.getItemId();
        if (id == R.id.action_help) {
            Intent intent = new Intent();
            intent.setClass(this, WifiApHelpActivity.class);
            this.startActivity(intent);
            return true;
        } else if (id == android.R.id.home) {
            finish();
        }
        return super.onOptionsItemSelected(item);
    }

    private String getSecurityTypeIndexToString() {
        String[] securityList = getResources().getStringArray(R.array.wifi_ap_security);
        if (mWifiApConfig == null) {
            return securityList[WifiApDialog.OPEN_INDEX];
        }
        return securityList[WifiApDialog.getSecurityTypeIndex(mWifiApConfig)];
    }

    private String getApSsid() {
        return mWifiApConfig.SSID;
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        ConnectivityManager cm =
                (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);

        if (preference == mCreateNetworkPref) {
            if (Res.getSettingsBoolean(this, Res.FLAG_SOW_HELP_FOR_FIRST_USING)) {
                AccountCheck.isWifiApBackKeyEnable(this, true);
            }
            showDialog(DIALOG_AP_SETTINGS);
        } else if (preference == mMaximumConnections) {
            showMaximumConnectionsDialog();
        }

        return super.onPreferenceTreeClick(screen, preference);
    }

    @Override
    public Dialog onCreateDialog(int id) {
        if (id == DIALOG_AP_SETTINGS) {
            mDialog = new WifiApDialog(this, this, mWifiApConfig);
            return mDialog;
        }
        return null;
    }

    @Override
    public void onClick(DialogInterface dialogInterface, int button) {
        if (button != DialogInterface.BUTTON_POSITIVE) {
             if (Res.getSettingsBoolean(this, Res.FLAG_SOW_HELP_FOR_FIRST_USING)
                    && mIsShowhelp) {
                mWifiApSwitch.setSwitchChecked(false);
            }
            return;
        }

        if(mWifiManager.getWifiApState() == WifiManager.WIFI_AP_STATE_ENABLED
                || mWifiManager.getWifiApState() == WifiManager.WIFI_AP_STATE_ENABLING) {
            AP_ENABLE_AND_CHANGE_CONFIG = true;
        } else {
            AP_ENABLE_AND_CHANGE_CONFIG = false;
        }

        mWifiApConfig = mDialog.getConfig();
        if (mWifiApConfig != null) {
            /**
             * if soft AP is stopped, bring up else restart with new config TODO: update config on a
             * running access point when framework support is added
             */
            if(AP_ENABLE_AND_CHANGE_CONFIG) {
                mWifiManager.setWifiApEnabled(null, false);
                mWifiManager.setWifiApConfiguration(mWifiApConfig);
            } else {
                if (!mIsShowhelp) {
                    mWifiManager.setWifiApConfiguration(mWifiApConfig);
                 } else {
                    AccountCheck.disableShowHelp(this);
                    mWifiManager.setWifiApEnabled(mWifiApConfig, true);
                    mIsShowhelp = false;
                 }
            }
            mCreateNetworkPref.setSummary(String.format(
                    Res.getSettingsString(this, Res.STRING_WIFIAP_CONFIGURE_SUBTEXT),
                    getApSsid(),
                    getSecurityTypeIndexToString()));
        }
    }

    private void handleEvent(Context context, Intent intent) {
        String action = intent.getAction();
        if (WifiManager.WIFI_AP_STATE_CHANGED_ACTION.equals(action)) {
            int state = intent.getIntExtra(WifiManager.EXTRA_WIFI_AP_STATE,
                    WifiManager.WIFI_AP_STATE_FAILED);
            switch (state) {
                case WifiManager.WIFI_AP_STATE_DISABLED:
                    clearApList();
                    mwifiApHint.setVisibility(View.VISIBLE);
                    break;
                case WifiManager.WIFI_AP_STATE_ENABLED:
                case WifiManager.WIFI_AP_STATE_ENABLING:
                case WifiManager.WIFI_AP_STATE_DISABLING:
                    mwifiApHint.setVisibility(View.GONE);
                default:
            }
            return;
        }
    }

    private void clearApList() {
        if (mWifiApListPrefCategory != null) {
            mWifiApListPrefCategory.removeAll();
        }
    }

    private void constructConnectedDevices() {
        List<StaDevice> devices = ((ExtSettingsApplication)getApplication())
                .getStaMonitor().getStaDevices();
        constructConnectedDevices(devices);
    }

    private void constructConnectedDevices(List<StaDevice> staDevices) {
        log("constructConnectedDevices..");
        mTetherConnectedStaList = staDevices;
        if (mTetherConnectedStaList == null || mTetherConnectedStaList.size() == 0) {
            log("ConnectedCount = 0");
            mWifiApListPrefCategory.removeAll();
            return;
        }
        mConnectedNameList.clear();
        mConnectedAddressList.clear();
        for (int i = 0; i < mTetherConnectedStaList.size(); i++) {
            StaDevice device = mTetherConnectedStaList.get(i);
            if (device.deviceState == StaDevice.CONNECTED) {
                mConnectedNameList.add(device.deviceName);
                mConnectedAddressList.add(device.deviceAddress);
            }
        }

        mWifiApListPrefCategory.removeAll();
        for (int index = 0; index < mConnectedAddressList.size(); ++index) {
            log("in construct pref addr = " + mConnectedAddressList.get(index));
            Preference pref = new Preference(this);
            if (mConnectedNameList != null && !mConnectedNameList.get(index).isEmpty()) {
                log("in construct pref.name = " + mConnectedNameList.get(index));
                pref.setTitle(mConnectedNameList.get(index));
            } else {
                pref.setTitle(R.string.wifiap_default_device_name);
            }
            pref.setSummary(mConnectedAddressList.get(index));
            mWifiApListPrefCategory.addPreference(pref);
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent intent) {
        super.onActivityResult(requestCode, resultCode, intent);
        if (requestCode == PROVISION_REQUEST && resultCode == Activity.RESULT_OK) {
            mWifiApSwitch.setSoftapEnabled(true);
        }
    }

    public void startProvisioningIfNecessary(int choice) {
        if (isProvisioningNeeded()) {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.setClassName(mProvisionApp[0], mProvisionApp[1]);
            this.startActivityForResult(intent, PROVISION_REQUEST);
        } else {
             boolean bShowFirstHelp = Res.getSettingsBoolean(this, Res.FLAG_SOW_HELP_FOR_FIRST_USING);
             if (bShowFirstHelp && AccountCheck.isNeedShowHelp(this)) {
                DialogInterface.OnClickListener okListener =
                        new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface arg0, int arg1) {
                        AccountCheck.isWifiApBackKeyEnable(mContext, false);
                        showDialog(DIALOG_AP_SETTINGS);
                    }
                };
                DialogInterface.OnClickListener laterListener =
                        new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface arg0, int arg1) {
                        AccountCheck.needShowHelpLater(mContext);
                        mWifiApSwitch.setSwitchChecked(false);
                    }
                };
                mIsShowhelp = true;
                AccountCheck.showHelpDialog(mContext, okListener, laterListener);
            } else {
                if (Res.getSettingsBoolean(this, Res.FLAG_USE_EMPTY_PASSWORD)) {
                    mWifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
                    if (AccountCheck.isPasswordEmpty(mWifiManager)) {
                        AccountCheck.showPasswordEmptyDialog(mContext);
                        mWifiApSwitch.setSwitchChecked(false);
                    }
                }
                mWifiApSwitch.setSoftapEnabled(true);
            }
        }
    }

    boolean isProvisioningNeeded() {
        if (SystemProperties.getBoolean("net.tethering.noprovisioning", false)) {
            return false;
        }
        return mProvisionApp.length == 2;
    }

    private void log(String msg) {
        if (DEBUG) {
            Log.d(TAG, msg);
        }
    }

    // Maximun connection number modification begin
    public void showMaximumConnectionsDialog() {
        niSelectedMaximum = sharedPreferences.getInt(KEY_MAXIMUN, DEFAULT_MAXIMUN_VALUE);

        final AlertDialog dialog = new AlertDialog.Builder(this).create();
        dialog.setTitle(maximumConnectionsDialogTitle);
        final NumberPicker catalogPick = new NumberPicker(this);
        dialog.setButton(maximumConnectionsPositive, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if(mWifiManager.getWifiApState() == WifiManager.WIFI_AP_STATE_ENABLED
                    || mWifiManager.getWifiApState() == WifiManager.WIFI_AP_STATE_ENABLING) {
                    AP_ENABLE_AND_CHANGE_CONFIG = true;
                } else {
                    AP_ENABLE_AND_CHANGE_CONFIG = false;
                }
                System.putInt(getContentResolver(),
                    WIFI_HOTSPOT_MAX_CLIENT_NUM, (catalogPick.getValue()));
                if (mWifiApConfig != null && mWifiManager.isWifiApEnabled()) {
                    mWifiManager.setWifiApEnabled(null, false);
                    mWifiManager.setWifiApConfiguration(mWifiApConfig);
                }
                dialog.dismiss();
                if (DEBUG)
                    Log.i("sysout","item:"+catalogPick.getValue());
                editor.putInt(KEY_MAXIMUN,catalogPick.getValue());
                editor.commit();
                mMaximumConnections.setSummary(""+(catalogPick.getValue()));
            }
        });

        dialog.setButton2(maximumConnectionsNegative, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });

        catalogPick.setMaxValue(DEFAULT_MAXIMUN_VALUE);
        catalogPick.setMinValue(1);
        catalogPick.setValue(niSelectedMaximum);// default set 8
        catalogPick.setDescendantFocusability(catalogPick.FOCUS_BLOCK_DESCENDANTS);
        dialog.setView(catalogPick);
        dialog.show();
    }
    //maximun connection number modifiction end
    //select hotspot working mode begin
    public boolean onPreferenceChange(Preference preference, Object value) {
        String key = preference.getKey();
        if (KEY_HOTSPOT_MODE.equals(key)) {
            String hotspotMode = (String)value;
            HotspotService.setHotspotMode(this, hotspotMode);
            mHotspotMode.setValue(hotspotMode);
            updateHotspotModeSummary(hotspotMode);
        }
        return false;
    }

    private String getSummaryByValue(String strValue) {
        if (!TextUtils.isEmpty(strValue)) {
            String[] entryValues = getResources().getStringArray(
                    R.array.wifi_hotspot_mode_entryValues);
            String[] summaries = getResources().getStringArray(
                    R.array.wifi_hotspot_mode_entries);
            int min = Math.min(entryValues.length, summaries.length);
            for (int index = 0; index < min; index++) {
                if (strValue.equals(entryValues[index])) {
                    return summaries[index];
                }
            }
        }
        return strValue;
    }

    private void updateHotspotModeSummary(String value) {
        String summary = getSummaryByValue(value);
        mHotspotMode.setSummary(summary);
    }
    //select hotspot working mode end
}
