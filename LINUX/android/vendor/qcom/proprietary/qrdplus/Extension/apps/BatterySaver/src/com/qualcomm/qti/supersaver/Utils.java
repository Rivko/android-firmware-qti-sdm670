/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.supersaver;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.nfc.NfcAdapter;

import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import static com.qualcomm.qti.supersaver.SSMConstants.*;

public class Utils {

    private static TelephonyManager getTelephonyManager(Context context) {
        return (TelephonyManager) context.getSystemService(
                Context.TELEPHONY_SERVICE);
    }

    private static boolean isAirplaneModeOn(Context context) {
        return (Settings.Global.getInt(context.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) == 1);
    }

    public static int getDefaultDataPhoneId() {
        return SubscriptionManager.getPhoneId(SubscriptionManager.getDefaultDataSubscriptionId());
    }

    private static boolean hasIccCard(Context context) {
        if (getTelephonyManager(context).isMultiSimEnabled()) {
            return getTelephonyManager(context).hasIccCard(getDefaultDataPhoneId());
        } else {
            return getTelephonyManager(context).hasIccCard();
        }
    }

    public static boolean getMobileDataEnabled(Context context) {

        return !isAirplaneModeOn(context) && hasIccCard(context)
                && getTelephonyManager(context).getDataEnabled();
    }

    public static boolean canMobileDataEnabled(Context context) {
        return !isAirplaneModeOn(context) && hasIccCard(context);
    }

    public static boolean enableMobileData(Context context, boolean enabled) {
        boolean mobiledataEnable = getTelephonyManager(context).getDataEnabled();
        getTelephonyManager(context).setDataEnabled(enabled);
        return mobiledataEnable;
    }

    public static int getGpsState(Context context) {
        return Settings.Secure.getInt(context.getContentResolver(),
                Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
    }

    public static int changeStateGPS(Context context, boolean enable, int state) {
        if (!enable) {
            state = Settings.Secure.LOCATION_MODE_OFF;
        }
        int oldState = Settings.Secure.getInt(context.getContentResolver(), Settings.Secure.LOCATION_MODE, state);
        Settings.Secure.putInt(context.getContentResolver(), Settings.Secure.LOCATION_MODE, state);
        return oldState;
    }

    public static int changeStateAGPS(Context context, boolean enable, int state) {
        if (!enable) {
           state = 0;
        }
        int oldState = Settings.Global.getInt(context.getContentResolver(), Settings.Global.ASSISTED_GPS_ENABLED, state);
        Settings.Global.putInt(context.getContentResolver(), Settings.Global.ASSISTED_GPS_ENABLED, state);
        return oldState;
    }

    private static WifiManager getWifiManager(Context context) {
        return (WifiManager) context.getSystemService(
                Context.WIFI_SERVICE);
    }

    /*
     * @see #WIFI_STATE_DISABLED
     * @see #WIFI_STATE_DISABLING
     * @see #WIFI_STATE_ENABLED
     * @see #WIFI_STATE_ENABLING
     * @see #WIFI_STATE_UNKNOWN
    */
    public static int getWifiState(Context context) {
        WifiManager wifimanager = getWifiManager(context);
        if (null != wifimanager) {
            return wifimanager.getWifiState();
        }
        return WifiManager.WIFI_STATE_UNKNOWN;
    }

    public static int enableWifi(Context context, boolean enable) {
        WifiManager wifimanager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        int wifistate = wifimanager.getWifiState();
        wifimanager.setWifiEnabled(enable);
        return wifistate;
    }

    /*
     * @see #WIFI_AP_STATE_DISABLED
     * @see #WIFI_AP_STATE_DISABLING
     * @see #WIFI_AP_STATE_ENABLED
     * @see #WIFI_AP_STATE_ENABLING
     * @see #WIFI_AP_STATE_FAILED
    */
    public static int getWifiApState(Context context) {
        WifiManager wifimanager = getWifiManager(context);
        if (null != wifimanager) {
            return wifimanager.getWifiApState();
        }
        return WifiManager.WIFI_AP_STATE_FAILED;
    }

    public static int enableWifiAp(Context context, boolean enable) {
        WifiManager wifimanager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        int wifiapstate = wifimanager.getWifiApState();
        wifimanager.setWifiApEnabled(null, enable);
        return wifiapstate;
    }

    public boolean getBTState() {
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        return (null != btAdapter) && (btAdapter.isEnabled());
    }

    public static boolean enableBT(Context context, boolean enabled) {
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        boolean btEnable = btAdapter.isEnabled();
        if (enabled) {
            btAdapter.enable();
        } else {
            btAdapter.disable();
        }
        return btEnable;
    }

    public static boolean getNFCState(Context context) {
        NfcAdapter nfcAdapter = NfcAdapter.getDefaultAdapter(context);
        return (null != nfcAdapter) && (nfcAdapter.isEnabled());
    }

    public static boolean enableNFC(Context context, boolean enable) {
        NfcAdapter nfcAdp = NfcAdapter.getDefaultAdapter(context);
        if (nfcAdp == null) {
            logd("NfcAdapter is null");
            return false;
        }
        boolean nfcEnable = nfcAdp.isEnabled();
        if (enable) {
            nfcAdp.enable();
        } else {
            nfcAdp.disable();
        }
        return nfcEnable;
    }

    public static int getHapticfekState(Context context) {
        return Settings.System.getInt(context.getContentResolver(),
                Settings.System.HAPTIC_FEEDBACK_ENABLED, DEF_SSM_HAPTIC_FEEDBACK_MODE);
    }

    public static int changeStateHapticfek(Context context, boolean enable, int state) {
        if (!enable) {
            state = 0;
        }
        int oldState = Settings.System.getInt(context.getContentResolver(), Settings.System.HAPTIC_FEEDBACK_ENABLED, state);
        Settings.System.putInt(context.getContentResolver(), Settings.System.HAPTIC_FEEDBACK_ENABLED, state);
        return oldState;
    }

    public static long getScreenOffTimeout(Context context) {
        return Settings.System.getLong(context.getContentResolver(),
                Settings.System.SCREEN_OFF_TIMEOUT, DEF_SSM_SCOFF_TIME_OUT);
    }

    public static long changeScreenOffTimeout(Context context, long timeout) {
        long oldTimeout = Settings.System.getLong(context.getContentResolver(), Settings.System.SCREEN_OFF_TIMEOUT, timeout);
        Settings.System.putLong(context.getContentResolver(), Settings.System.SCREEN_OFF_TIMEOUT, timeout);
        return oldTimeout;
    }

    public static int getBacklightLevel(Context context) {
       return Settings.System.getInt(context.getContentResolver(),
                Settings.System.SCREEN_BRIGHTNESS, 25);
    }

    public static int changeBacklightLevel(Context context, int backlight) {
        int oldBacklight = Settings.System.getInt(context.getContentResolver(), Settings.System.SCREEN_BRIGHTNESS, backlight);
        Settings.System.putInt(context.getContentResolver(), Settings.System.SCREEN_BRIGHTNESS, backlight);
        return oldBacklight;
    }

    public static int getBacklightMode(Context context) {
        return Settings.System.getInt(context.getContentResolver(),
                Settings.System.SCREEN_BRIGHTNESS_MODE, DEF_SSM_BKLT_MODE);
    }

    public static int changeBacklightMode(Context context, int mode) {
        int oldMode = Settings.System.getInt(context.getContentResolver(), Settings.System.SCREEN_BRIGHTNESS_MODE, mode);
        Settings.System.putInt(context.getContentResolver(), Settings.System.SCREEN_BRIGHTNESS_MODE, mode);
        return oldMode;
    }

    public static void logd(Object dLogInfo) {
        if (!DEBUG) {
            return;
        }

        Thread mThread = Thread.currentThread();
        StackTraceElement[] stackTrace = mThread.getStackTrace();
        String methodName = stackTrace[3].getMethodName();

        dLogInfo = "[" + methodName + "] " + dLogInfo;
        Log.d(LOG_TAG, dLogInfo + "");
    }

}
