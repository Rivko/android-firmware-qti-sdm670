/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Copyright (c) 2014, The Linux Foundation. All Rights Reserved.
 */

package com.qualcomm.qti.extsettings;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.provider.Settings;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.os.Handler;
import android.os.Message;
import com.qualcomm.qti.extsettings.hotspot.AccountCheck;
import com.qualcomm.qti.extsettings.hotspot.Res;
import com.qualcomm.qti.extsettings.hotspot.HotspotService;

import java.util.ArrayList;

public class WifiApSwitch implements CompoundButton.OnCheckedChangeListener {

    private static final String TAG = "WifiApSwitch";

    private static final int WIFI_STATE_ENABLE_ENABLING = 1;
    private static final int WIFI_STATE_DISABLE_DISABLING = 0;
    private static final int WIFI_TETHERING = 0;
    private static final int PROVISION_REQUEST = 0;

    private boolean mStateMachineEvent;
    private WifiManager mWifiManager;
    private Switch mSwitch;
    ConnectivityManager mCm;
    private String[] mWifiRegexs;
    /* Indicates if we have to wait for WIFI_STATE_CHANGED intent */
    private boolean mWaitForWifiStateChange;

    private final Context mContext;
    private final WifiApSettingsActivity mParent;
    private final IntentFilter mIntentFilter;
    private Handler accountHandler;
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (WifiManager.WIFI_AP_STATE_CHANGED_ACTION.equals(action)) {
                handleWifiApStateChanged(intent.getIntExtra(
                        WifiManager.EXTRA_WIFI_AP_STATE, WifiManager.WIFI_AP_STATE_FAILED));
            } else if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)) {
                if (mWaitForWifiStateChange) {
                    handleWifiStateChanged(intent.getIntExtra(
                            WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN));
                }
            } else if (Intent.ACTION_AIRPLANE_MODE_CHANGED.equals(action)) {
                enableWifiCheckBox();
            }
        }
    };

    public WifiApSwitch(Context context, WifiApSettingsActivity parent, Switch switch_) {
        mContext = context;
        mParent = parent;
        mSwitch = switch_;
        mWaitForWifiStateChange = true;

        mWifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        mCm = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);

        mWifiRegexs = mCm.getTetherableWifiRegexs();

        mIntentFilter = new IntentFilter(WifiManager.WIFI_AP_STATE_CHANGED_ACTION);
        mIntentFilter.addAction(ConnectivityManager.ACTION_TETHER_STATE_CHANGED);
        mIntentFilter.addAction(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        mIntentFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        if (Res.getSettingsBoolean(mContext,
                 "R.bool.hotspot_accout_check_enable")) {
            accountHandler = new AccountHandler();
        }

    }

    public void resume() {
        mContext.registerReceiver(mReceiver, mIntentFilter);
        mSwitch.setOnCheckedChangeListener(this);
        enableWifiCheckBox();
    }

    public void pause() {
        mContext.unregisterReceiver(mReceiver);
        mSwitch.setOnCheckedChangeListener(null);
    }

    private void enableWifiCheckBox() {
        boolean isAirplaneMode = Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) != 0;
        mSwitch.setEnabled(!isAirplaneMode);
    }

    public void setSoftapEnabled(boolean enable) {
        if (enable) {
            if (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLING ||
                    mWifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED) {
                /**
                 * Disable Wifi if enabling tethering
                 */
                mWifiManager.setWifiEnabled(false);
                setWifiSavedState(mContext, WIFI_STATE_ENABLE_ENABLING);
            }
            if (mWifiManager.setWifiApEnabled(null, enable)) {
                /* Disable here, enabled on receiving success broadcast */
                mSwitch.setEnabled(false);
            }
        } else {
            /**
             * Check if we have to wait for the WIFI_STATE_CHANGED intent before we re-enable the
             * Checkbox.
             */
            mWaitForWifiStateChange = false;

            if (mWifiManager.setWifiApEnabled(null, enable)) {
                /* Disable here, enabled on receiving success broadcast */
                mSwitch.setEnabled(false);
            }

            /**
             * If needed, restore Wifi on tether disable
             */
            if (getWifiSavedState(mContext) == WIFI_STATE_ENABLE_ENABLING) {
                mWaitForWifiStateChange = true;
                mWifiManager.setWifiEnabled(true);
                setWifiSavedState(mContext, WIFI_STATE_DISABLE_DISABLING);
            }
        }
    }

    public void setSwitch(Switch switch_) {
        if (mSwitch == switch_) {
            return;
        }
        mSwitch.setOnCheckedChangeListener(null);
        mSwitch = switch_;
        mSwitch.setOnCheckedChangeListener(this);
    }

    private void handleWifiApStateChanged(int state) {
        switch (state) {
            case WifiManager.WIFI_AP_STATE_ENABLING:
                mSwitch.setEnabled(false);
                break;
            case WifiManager.WIFI_AP_STATE_ENABLED:
                /**
                 * Summary on enable is handled by tether broadcast notice
                 */
                if (AccountCheck.isHotspotAutoTurnOffEnabled(mContext)) {
                    String hotspotServiceClassName = HotspotService.class.getName();
                    if (!AccountCheck.isServiceRunning(mContext, hotspotServiceClassName)) {
                        Intent intent = new Intent().setClassName(mContext,
                                hotspotServiceClassName);
                        mContext.startService(intent);
                    }
                }
                if (Res.getSettingsBoolean(mContext, Res.FLAG_NEED_SHOW_ACTIVATED)) {
                    AccountCheck.showActivatedDialog(mContext, WIFI_TETHERING);
                }
                setSwitchChecked(true);
                /* Doesnt need the airplane check */
                mSwitch.setEnabled(true);
                break;
            case WifiManager.WIFI_AP_STATE_DISABLING:
                mSwitch.setEnabled(false);
                break;
            case WifiManager.WIFI_AP_STATE_DISABLED:
                setSwitchChecked(false);
                mSwitch.setEnabled(true);
                if (mWaitForWifiStateChange == false) {
                    enableWifiCheckBox();
                }
                if(mParent.AP_ENABLE_AND_CHANGE_CONFIG) {
                    mParent.AP_ENABLE_AND_CHANGE_CONFIG = false;
                    mParent.startProvisioningIfNecessary(WIFI_TETHERING);
                }
                break;
            default:
                setSwitchChecked(false);
                enableWifiCheckBox();
        }
    }

    private void handleWifiStateChanged(int state) {
        switch (state) {
            case WifiManager.WIFI_STATE_ENABLED:
            case WifiManager.WIFI_STATE_UNKNOWN:
                enableWifiCheckBox();
                break;
            default:
        }
    }

    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        // Do nothing if called as a result of a state machine event
        if (mStateMachineEvent) {
            return;
        }

        if (AccountCheck.showNoSimCardDialog(mContext)) {
            mSwitch.setChecked(false);
            return;
        }

        if (Res.getSettingsBoolean(mContext, Res.FLAG_ACCOUNT_CHECK_ENABLE)) {
            if (isChecked) {
                if (AccountCheck.showNoSimCardDialog(mContext)) {
                    mSwitch.setChecked(false);
                    return;
                }
                if (AccountCheck.isCarrierSimCard(mContext)) {
                    AccountCheck.getInstance().checkAccount(mContext,
                            accountHandler.obtainMessage(1));
                    return;
                }
            }
        }

        WifiConfiguration config = mWifiManager.getWifiApConfiguration();
        boolean isNotNoneSecurity = config.getAuthType() > WifiConfiguration.KeyMgmt.NONE;
        if(isNotNoneSecurity) {
            boolean mHaveWifiApConfig = config.preSharedKey != null &&
                    !config.preSharedKey.isEmpty();
            if(!mHaveWifiApConfig) {
                AccountCheck.needShowHelpLater(mContext);
            } else {
                AccountCheck.disableShowHelp(mContext);
            }
        }
        AccountCheck.showTurnOffWifiDialog(mContext);

        if (isChecked) {
            mParent.startProvisioningIfNecessary(WIFI_TETHERING);
        } else {
            setSoftapEnabled(false);
        }
    }

    public void setSwitchChecked(boolean checked) {
        if (checked != mSwitch.isChecked()) {
            mStateMachineEvent = true;
            mSwitch.setChecked(checked);
            mStateMachineEvent = false;
        }
    }

    public static int getWifiSavedState(Context context) {
        return Settings.Global.getInt(context.getContentResolver(),
                Settings.Global.WIFI_SAVED_STATE, WIFI_STATE_DISABLE_DISABLING);
    }

    public static void setWifiSavedState(Context context, int state) {
        Settings.Global.putInt(context.getContentResolver(),
                Settings.Global.WIFI_SAVED_STATE, state);
    }
    private class AccountHandler extends Handler {
        @Override
        @SuppressWarnings("unchecked")
        public void handleMessage(Message message) {
            if(message.arg1 == AccountCheck.ACCEPT) {
                mParent.startProvisioningIfNecessary(WIFI_TETHERING);
            } else {
                setSwitchChecked(false);
            }
        }
    }
}
