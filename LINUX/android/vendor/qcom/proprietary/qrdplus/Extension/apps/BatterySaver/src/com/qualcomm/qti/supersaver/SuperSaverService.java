/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.supersaver;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ContentResolver;
import android.os.AsyncTask;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.Handler;
import android.os.UserHandle;
import android.provider.Settings;
import android.net.wifi.WifiManager;
import android.nfc.NfcAdapter;
import android.database.ContentObserver;

import static android.net.wifi.WifiManager.WIFI_AP_STATE_DISABLED;
import static android.net.wifi.WifiManager.WIFI_AP_STATE_ENABLED;

import static com.qualcomm.qti.supersaver.Utils.*;
import static com.qualcomm.qti.supersaver.SSMConstants.*;
import static com.qualcomm.qti.supersaver.UtilsSharedPreferences.*;

public class SuperSaverService extends Service {

    private boolean mSaveModeOn;
    private boolean mIsSetting;
    private boolean mIsMobileDataChanged;        // position '0' for bitValue
    private boolean mIsGpsChanged;               // position '1' for bitValue
    private boolean mIsAgpsChanged;              // position '2' for bitValue
    private boolean mIsWifiChanged;              // position '3' for bitValue
    private boolean mIsWifiApChanged;            // position '4' for bitValue
    private boolean mIsBluetoothChanged;         // position '5' for bitValue
    private boolean mIsNfcChanged;               // position '6' for bitValue
    private boolean mIsHapticChanged;            // position '7' for bitValue
    private boolean mIsTimeoutChanged;           // position '8' for bitValue
    private boolean mIsBrightnessChanged;        // position '9' for bitValue
    private boolean mIsBrightnessModeChanged;    // position '10' for bitValue
    private int bitValue;
    private int mBrightnessLevel;
    private SettingsObserver mSettingsObserver;
    private Handler mHandler = new Handler();
    private final static int DELAY = 2000;
    private final BatterySaverReceiver mBatterySaverReceiver = new BatterySaverReceiver();

    @Override
    public void onCreate() {
        logd("");
        super.onCreate();
        mBatterySaverReceiver.init();

        final boolean savermode = Settings.Global.getInt(getContentResolver(),
                                                            Settings.Global.LOW_POWER_MODE, 0) != 0;
        if (savermode) {
            mSaveModeOn = true;
            restoreBitValue();
        }

        PowerManager pm = (PowerManager)getSystemService(Context.POWER_SERVICE);
        int min = pm.getMinimumScreenBrightnessSetting();
        int max = pm.getMaximumScreenBrightnessSetting();
        mBrightnessLevel = 2 * (min + (max - min) / DEF_SSM_BKLT_DIV);

        mSettingsObserver = new SettingsObserver(mHandler);
        mSettingsObserver.observe();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void initVariables() {
        mIsMobileDataChanged = false;
        mIsGpsChanged = false;
        mIsAgpsChanged = false;
        mIsWifiChanged = false;
        mIsWifiApChanged = false;
        mIsBluetoothChanged = false;
        mIsNfcChanged = false;
        mIsHapticChanged = false;
        mIsTimeoutChanged = false;
        mIsBrightnessChanged = false;
        mIsBrightnessModeChanged = false;
    }

    private void saveBitValue() {
        bitValue = 0;
        if (mIsMobileDataChanged) bitValue |= 1 << 0;
        if (mIsGpsChanged) bitValue |= 1 << 1;
        if (mIsAgpsChanged) bitValue |= 1 << 2;
        if (mIsWifiChanged) bitValue |= 1 << 3;
        if (mIsWifiApChanged) bitValue |= 1 << 4;
        if (mIsBluetoothChanged) bitValue |= 1 << 5;
        if (mIsNfcChanged) bitValue |= 1 << 6;
        if (mIsHapticChanged) bitValue |= 1 << 7;
        if (mIsTimeoutChanged) bitValue |= 1 << 8;
        if (mIsBrightnessChanged) bitValue |= 1 << 9;
        if (mIsBrightnessModeChanged) bitValue |= 1 << 10;
        saveIntValue(SuperSaverService.this, KEY_SUPERSAVER_BIT_VALUE, bitValue);
    }

    private void restoreBitValue() {
        bitValue = getIntValueSaved(SuperSaverService.this, KEY_SUPERSAVER_BIT_VALUE, 0);
        if ((bitValue & (1 << 0)) != 0) mIsMobileDataChanged = true;
        if ((bitValue & (1 << 1)) != 0) mIsGpsChanged = true;
        if ((bitValue & (1 << 2)) != 0) mIsAgpsChanged = true;
        if ((bitValue & (1 << 3)) != 0) mIsWifiChanged = true;
        if ((bitValue & (1 << 4)) != 0) mIsWifiApChanged = true;
        if ((bitValue & (1 << 5)) != 0) mIsBluetoothChanged = true;
        if ((bitValue & (1 << 6)) != 0) mIsNfcChanged = true;
        if ((bitValue & (1 << 7)) != 0) mIsHapticChanged = true;
        if ((bitValue & (1 << 8)) != 0) mIsTimeoutChanged = true;
        if ((bitValue & (1 << 9)) != 0) mIsBrightnessChanged = true;
        if ((bitValue & (1 << 10)) != 0) mIsBrightnessModeChanged = true;
    }

    private final class BatterySaverReceiver extends BroadcastReceiver {

        public void init() {
            // Register for Intent broadcasts.
            IntentFilter filter = new IntentFilter();
            filter.addAction(PowerManager.ACTION_POWER_SAVE_MODE_CHANGING);
            filter.addAction(WifiManager.WIFI_AP_STATE_CHANGED_ACTION);
            filter.addAction(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
            filter.addAction(Intent.ACTION_SHUTDOWN);
            SuperSaverService.this.registerReceiver(this, filter);
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (PowerManager.ACTION_POWER_SAVE_MODE_CHANGING.equals(action)) {
                logd(":" + intent);
                boolean enabled = intent.getBooleanExtra(PowerManager.EXTRA_POWER_SAVE_MODE, false);
                if (enabled) {
                    if (mSaveModeOn) {
                        return;
                    }
                    mSaveModeOn = true;
                    logd(":Enter Super Saver Mode");
                } else {
                    if (!mSaveModeOn) {
                        return;
                    }
                    mSaveModeOn = false;
                    logd(":Exit Super Saver Mode");
                }
                new SuperSaverModeTask().execute();
            } else if (WifiManager.WIFI_AP_STATE_CHANGED_ACTION.equals(action)) {
                if (mSaveModeOn && !mIsSetting) {
                    int wifiApState = intent.getIntExtra(WifiManager.EXTRA_WIFI_AP_STATE,
                        WIFI_AP_STATE_DISABLED);
                    if (WIFI_AP_STATE_ENABLED == wifiApState) {
                        mIsWifiApChanged = true;
                    }
                }
            } else if (NfcAdapter.ACTION_ADAPTER_STATE_CHANGED.equals(action)) {
                if (mSaveModeOn && !mIsSetting) {
                    if (getNFCState(SuperSaverService.this)) {
                        mIsNfcChanged = true;
                    }
                }
            } else if (Intent.ACTION_SHUTDOWN.equals(action)) {
                if (mSaveModeOn) {
                    saveBitValue();
                }
            }
        }
    };

    class SettingsObserver extends ContentObserver {
        SettingsObserver(Handler handler) {
            super(handler);
        }

        void observe() {
            ContentResolver resolver = SuperSaverService.this.getContentResolver();

            // for GPS
            resolver.registerContentObserver(Settings.Secure.getUriFor(
                    Settings.Secure.LOCATION_MODE), false, this, UserHandle.USER_ALL);

            // for AGPS
            resolver.registerContentObserver(Settings.Global.getUriFor(
                    Settings.Global.ASSISTED_GPS_ENABLED), false, this, UserHandle.USER_ALL);

            // for bluetooth
            resolver.registerContentObserver(Settings.Global.getUriFor(
                    Settings.Global.BLUETOOTH_ON), false, this, UserHandle.USER_ALL);

            // for mobile data
            resolver.registerContentObserver(Settings.Global.getUriFor(
                    Settings.Global.MOBILE_DATA + getDefaultDataPhoneId()), false, this, UserHandle.USER_ALL);

            // for WIFI
            resolver.registerContentObserver(Settings.Global.getUriFor(
                    Settings.Global.WIFI_ON), false, this, UserHandle.USER_ALL);

            // for Haptic feedback
            resolver.registerContentObserver(Settings.System.getUriFor(
                    Settings.System.HAPTIC_FEEDBACK_ENABLED), false, this, UserHandle.USER_ALL);

            // for screen timeout
            resolver.registerContentObserver(Settings.System.getUriFor(
                    Settings.System.SCREEN_OFF_TIMEOUT), false, this, UserHandle.USER_ALL);

            // for brightness
            resolver.registerContentObserver(Settings.System.getUriFor(
                    Settings.System.SCREEN_BRIGHTNESS), false, this, UserHandle.USER_ALL);

            // for brightness mode
            resolver.registerContentObserver(Settings.System.getUriFor(
                    Settings.System.SCREEN_BRIGHTNESS_MODE), false, this, UserHandle.USER_ALL);
        }

        @Override
        public void onChange(boolean selfChange) {
            if (mSaveModeOn && !mIsSetting) {
                ContentResolver resolver = SuperSaverService.this.getContentResolver();

                // check GPS reseted or not by user
                int currentGpsState = Settings.Secure.getInt(resolver,
                        Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
                if (Settings.Secure.LOCATION_MODE_OFF != currentGpsState) {
                    mIsGpsChanged = true;
                }

                // check AGPS reseted or not by user
                int currentAgpsState = Settings.Global.getInt(resolver,
                                                Settings.Global.ASSISTED_GPS_ENABLED, 0);
                if (0 != currentAgpsState) {
                    mIsAgpsChanged = true;
                }

                // check bluetooth reseted or not by user
                int currentBtState = Settings.Global.getInt(resolver,
                                                Settings.Global.BLUETOOTH_ON, 0);
                if (0 != currentBtState) {
                    mIsBluetoothChanged = true;
                }

                // check mobile data reseted or not by user
                int currentMobileDataState = Settings.Global.getInt(resolver,
                                                Settings.Global.MOBILE_DATA + getDefaultDataPhoneId(), 0);
                if (0 != currentMobileDataState) {
                    mIsMobileDataChanged = true;
                }

                // check wifi reseted or not by user
                int currentWifiState = Settings.Global.getInt(resolver,
                                                Settings.Global.WIFI_ON, 0);
                if (0 != currentWifiState) {
                    mIsWifiChanged = true;
                }

                // check haptic feedback reseted or not by user
                int currentHapticState = Settings.System.getInt(resolver,
                                                Settings.System.HAPTIC_FEEDBACK_ENABLED, 0);
                if (0 != currentHapticState) {
                    mIsHapticChanged = true;
                }

                // check brightness reseted or not by user
                int currentBrightnessState = Settings.System.getInt(resolver,
                                                Settings.System.SCREEN_BRIGHTNESS, mBrightnessLevel);
                if (mBrightnessLevel != currentBrightnessState) {
                    mIsBrightnessChanged = true;
                }

                // check screen timeout reseted or not by user
                int currentTimeoutState = Settings.System.getInt(resolver,
                                                Settings.System.SCREEN_OFF_TIMEOUT, DEF_SSM_SCOFF_TIME_OUT);
                if (DEF_SSM_SCOFF_TIME_OUT != currentTimeoutState) {
                    mIsTimeoutChanged = true;
                }

                // check brightness mode reseted or not by user
                int currentBrightnessModeState = Settings.System.getInt(resolver,
                                                Settings.System.SCREEN_BRIGHTNESS_MODE, Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
                if (Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL != currentBrightnessModeState) {
                    mIsBrightnessModeChanged = true;
                }
            }
        }
    }

    private class SuperSaverModeTask extends AsyncTask<Void, Void, Void> {

        public SuperSaverModeTask() {
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();

            if (mSaveModeOn) {
                initVariables();
            }
            mIsSetting = true;
        }

        @Override
        protected Void doInBackground(Void... params) {

            if (mSaveModeOn) {
                // Disable mobile data
                if (canMobileDataEnabled(SuperSaverService.this)) {
                    boolean OldMobileDataEnabled = enableMobileData(SuperSaverService.this, false);
                    saveBooleanValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_MOBILEDATA_ENABLE, OldMobileDataEnabled);
                }

                // Disable GPS
                int OldGPSState = changeStateGPS(SuperSaverService.this, false, 0);
                saveIntValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_GPS_STATE, OldGPSState);

                // Disable AGPS
                int OldAGPSState = changeStateAGPS(SuperSaverService.this, false, 0);
                saveIntValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_AGPS_STATE, OldAGPSState);

                // Disable wifi
                int OldWifiState = enableWifi(SuperSaverService.this, false);
                saveIntValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_WIFI_STATE, OldWifiState);

                // Disable wifi ap
                int OldWifiAPState = enableWifiAp(SuperSaverService.this, false);
                saveIntValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_WIFI_AP_STATE, OldWifiAPState);

                // Disable bluetooth
                boolean OldBTEnable = enableBT(SuperSaverService.this, false);
                saveBooleanValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_BT_ENABLE, OldBTEnable);

                // Disable NFC
                boolean OldNFCEnable = enableNFC(SuperSaverService.this, false);
                saveBooleanValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_NFC_ENABLE, OldNFCEnable);

                // Disable haptic feedback
                int OldValHapticfek = changeStateHapticfek(SuperSaverService.this, false, 0);
                saveIntValue(SuperSaverService.this,
                                           KEY_SUPERSAVER_HAPTICFEK, OldValHapticfek);

                // Change screen off timeout to ss mode
                long OldTimeout = changeScreenOffTimeout(SuperSaverService.this,
                        DEF_SSM_SCOFF_TIME_OUT);
                saveLongValue(SuperSaverService.this,
                        KEY_SUPERSAVER_BKL_SCREEN_OFF_TIMEOUT, OldTimeout);

                // change backlight level to low level
                int OldBacklight = changeBacklightLevel(SuperSaverService.this, mBrightnessLevel);
                saveIntValue(SuperSaverService.this,
                        KEY_SUPERSAVER_BKL_LEVEL, OldBacklight);

                // Change backlight mode to manual mode
                int OldMode = changeBacklightMode(SuperSaverService.this,
                        Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
                saveIntValue(SuperSaverService.this,
                               KEY_SUPERSAVER_BKL_MODE, OldMode);
            } else {

                if (!mIsMobileDataChanged) {
                    // Restore mobile data
                    if (canMobileDataEnabled(SuperSaverService.this)) {
                        boolean OldMobileDataEnabled = getBooleanValueSaved(SuperSaverService.this,
                                 KEY_SUPERSAVER_MOBILEDATA_ENABLE, false);
                        enableMobileData(SuperSaverService.this, OldMobileDataEnabled);
                    }
                }

                if (!mIsGpsChanged) {
                    // Restore GPS
                    int OldGPSState = getIntValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_GPS_STATE, 0);
                    changeStateGPS(SuperSaverService.this, true, OldGPSState);
                }

                if (!mIsAgpsChanged) {
                    // Restore AGPS
                    int OldAGPSState = getIntValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_AGPS_STATE, 0);
                    changeStateAGPS(SuperSaverService.this, true, OldAGPSState);
                }

                if (!mIsWifiChanged) {
                    // Restore wifi
                    int OldWifiState = getIntValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_WIFI_STATE, 0);
                    if (OldWifiState == WifiManager.WIFI_STATE_ENABLED ||
                        OldWifiState == WifiManager.WIFI_STATE_ENABLING) {
                        enableWifi(SuperSaverService.this, true);
                    }
                }

                if (!mIsWifiApChanged) {
                    // Restore wifi ap
                    int OldWifiAPState = getIntValueSaved(SuperSaverService.this,
                                                                KEY_SUPERSAVER_WIFI_AP_STATE, 0);
                    if (OldWifiAPState == WifiManager.WIFI_AP_STATE_ENABLED ||
                        OldWifiAPState == WifiManager.WIFI_AP_STATE_ENABLING) {
                        enableWifiAp(SuperSaverService.this, true);
                    }
                }

                if (!mIsBluetoothChanged) {
                    // Restore Bluetooth
                    boolean OldBTEnable = getBooleanValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_BT_ENABLE, false);
                    enableBT(SuperSaverService.this, OldBTEnable);
                }

                if (!mIsNfcChanged) {
                    // Restore NFC
                    boolean OldNfcEnable = getBooleanValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_NFC_ENABLE, false);
                    enableNFC(SuperSaverService.this, OldNfcEnable);
                }

                if (!mIsHapticChanged) {
                    // Restore Haptic feedback
                    int OldHapticState = getIntValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_HAPTICFEK, 0);
                    changeStateHapticfek(SuperSaverService.this, true, OldHapticState);
                }

                if (!mIsTimeoutChanged) {
                    // Restore screen off timeout to ss mode
                    long OldTimeout = getLongValueSaved(SuperSaverService.this,
                               KEY_SUPERSAVER_BKL_SCREEN_OFF_TIMEOUT, DEF_SSM_SCOFF_TIME_OUT);
                    changeScreenOffTimeout(SuperSaverService.this, OldTimeout);
                }

                if (!mIsBrightnessChanged) {
                    // Restore backlight level to low level
                    int OldBacklight = getIntValueSaved(SuperSaverService.this,
                                KEY_SUPERSAVER_BKL_LEVEL, mBrightnessLevel);
                    changeBacklightLevel(SuperSaverService.this, OldBacklight);
                }

                if (!mIsBrightnessModeChanged) {
                    // Restore backlight mode to manual mode
                    int OldMode = getIntValueSaved(SuperSaverService.this,
                            KEY_SUPERSAVER_BKL_MODE,
                            Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);
                    changeBacklightMode(SuperSaverService.this, OldMode);
                }
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            logd(":Handle Super Saver Mode Completed");

            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mIsSetting = false;
                }
            }, DELAY);
        }
    }
}
