/*
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.android.usbsecurity;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.hardware.usb.UsbManager;
import android.os.SystemProperties;
import android.util.Log;
import android.widget.Toast;
import android.util.ArrayMap;

import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.TelephonyIntents;

public class UsbSecurityReceiver extends BroadcastReceiver {
    private static final String TAG = "UsbSecurity";
    private static boolean mIsLastConnected = false;

    //need adapt usb default mode configuration per target in usb security feature.
    //new a ArrayMap to save Default usb persist configuration per target
    ArrayMap<String , String > DefaultUsbMode = new ArrayMap<String, String>();

    //save default usb persist configuration per target
    //the function must be updated with latest default USB persist configuration in init.qcom.usb.sh
    private void saveDefaultUsbMode(){
        DefaultUsbMode.put("msm8916", "diag,serial_smd,rmnet_bam,adb");
        DefaultUsbMode.put("msm8994", "diag,serial_smd,serial_tty,rmnet_ipa,mass_storage,adb");
        DefaultUsbMode.put("msm8992", "diag,serial_smd,serial_tty,rmnet_ipa,mass_storage,adb");
        DefaultUsbMode.put("msm8996", "diag,serial_cdev,serial_tty,rmnet_ipa,mass_storage,adb");
        DefaultUsbMode.put("msm8909", "diag,serial_smd,rmnet_qti_bam,adb");
        DefaultUsbMode.put("msm8937", "diag,serial_smd,rmnet_qti_bam,adb");
        DefaultUsbMode.put("msm8952", "diag,serial_smd,rmnet_ipa,adb");
        DefaultUsbMode.put("msm8953", "diag,serial_smd,rmnet_ipa,adb");
        DefaultUsbMode.put("msmcobalt", "diag,serial_cdev,rmnet_gsi,adb");//target8998
    }

    //set default usb persist configuration back per target if sim card enabled
    //the function must be updated with latest default USB persist configuration in init.qcom.usb.sh
    private void setDefaultUsbMode(){
        String Target = SystemProperties.get("ro.board.platform");
        Log.d(TAG, "Target is: " + Target);
        if ( DefaultUsbMode.get(Target) != null ){
            SystemProperties.set("persist.sys.usb.config", DefaultUsbMode.get(Target));
        }
        else {
            SystemProperties.set("persist.sys.usb.config", "diag,adb");
        }
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        saveDefaultUsbMode();
        String action = intent.getAction();
        if (UsbManager.ACTION_USB_STATE.equals(action)) {
            boolean isConnected = intent.getExtras().getBoolean(UsbManager.USB_CONNECTED, false);
            boolean isSimCardInserted = SystemProperties.getBoolean(
                    "persist.sys.sim.activate", false);
            boolean isUsbSecurityEnable = SystemProperties.getBoolean(
                    "persist.sys.usb.security", false);
            Log.d(TAG, "ACTION_USB_STATE" + !isSimCardInserted + isUsbSecurityEnable + isConnected
                    + mIsLastConnected);
            // 1 Pre-conditions: User never insert any sim card, user plug in usb cable.
            // 2 Expect result: Pop up a dialog to notify user.
            // 3 When persist.sys.usb.security is true, this feature is enable.
            // 4 When plug in usb cable, check and uncheck "Charge only", will cause DUT send
            // USB_CONNECTED intent, dialog will pop up again when user click OK. Use
            // mIsLastConnected to avoid it.
            if ((!isSimCardInserted) && isUsbSecurityEnable && isConnected && !mIsLastConnected) {
                Intent UsbSecurityActivityIntent = new Intent();
                UsbSecurityActivityIntent.setClass(context, UsbSecurityActivity.class);
                UsbSecurityActivityIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                context.startActivity(UsbSecurityActivityIntent);
            }
            mIsLastConnected = isConnected;
        } else if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(action)) {
            String stateExtra = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
            boolean isSimCardInserted = IccCardConstants.INTENT_VALUE_ICC_READY.equals(stateExtra)
                    || IccCardConstants.INTENT_VALUE_ICC_IMSI.equals(stateExtra)
                    || IccCardConstants.INTENT_VALUE_ICC_LOADED.equals(stateExtra);
            boolean isSimCardActivated = SystemProperties.getBoolean(
                    "persist.sys.sim.activate", false);
            if (isSimCardInserted && !isSimCardActivated) {
                SystemProperties.set("persist.sys.sim.activate", "true");
                setDefaultUsbMode();
            }
        }
    }
}
