/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import com.android.ims.ImsManager;
import com.qualcomm.ims.utils.Log;
import org.codeaurora.ims.R;
import android.app.AlertDialog;
import android.content.DialogInterface.OnClickListener;
import android.content.DialogInterface;
import android.view.WindowManager;

/**
 * This receiver is used to add Wifi calling enabled/disable
 * option in the Quick Settings.
 */
public class WifiCallingQuickSettingsReceiver extends BroadcastReceiver {
    /**
     * Action to send Wifi Calling Quick Settings details to IntentTile
     * This details include icon, label and visibility information
     */
    private static final String WIFICALLING_TILE_ACTION =
            "org.codeaurora.qs.tiles.wificallingtile";
    /**
     * Action gets triggered when the QS icon is clicked.
     */
    private static final String QS_ONCLICK_ACTION =
            "org.codeaurora.qs.tiles.wificallingtile.ONCLICK";

    /**
     * On Launch gets triggered when the Status bar is dragged
     * down to see the quick settings
     */
    private static final String QS_ONLAUNCH_ACTION =
            "org.codeaurora.qs.tiles.wificallingtile.QS_ONLAUNCH_ACTION";

    // Extra keys for Quick settings
    private static final String EXTRAKEY_ONCLICK = "onClick";
    private static final String EXTRAKEY_ONLONGCLICK = "onLongClick";
    private static final String EXTRAKEY_ONLAUNCH = "onLaunch";
    private static final String EXTRAKEY_ONCLICK_URI = "onClickUri";
    private static final String EXTRAKEY_ONLONGCLICK_URI = "onLongClickUri";
    private static final String EXTRAKEY_ONLAUNCH_URI = "onLaunchUri";

    // Extra keys to set content in Quick Settings
    private static final String EXTRAKEY_LABEL = "label";
    private static final String EXTRAKEY_ICONID = "iconId";
    private static final String EXTRAKEY_ICON_PACKAGE = "iconPackage";
    private static final String EXTRAKEY_CONTENT = "contentDescription";
    private static final String CONTENT_WIFI_CALLING_Tile =
            "Wifi Calling Tile";
    private static final String SSID_NONE = "<unknown ssid>";
    private static Context mContext;
    private boolean mWfcEnabled;

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        String action = intent.getAction();
        if (Intent.ACTION_BOOT_COMPLETED.equals(action)
                || QS_ONLAUNCH_ACTION.equals(action)) {

            sendIntentToUpdateQSTile(context);
        } else if (QS_ONCLICK_ACTION.equals(action)) {
            TelephonyManager tm = (TelephonyManager) context
                    .getSystemService(Context.TELEPHONY_SERVICE);
            if (tm != null && tm.getCallState() != TelephonyManager.CALL_STATE_IDLE) {
                Log.i(this, "Call is present so ignore the click");
                return;
            }

            if (!ImsManager.isWfcEnabledByPlatform(context)) {
                Log.i(this, "WFC is not enable by platform ignore the click");
                return;
            }

            mWfcEnabled = (ImsManager.isWfcEnabledByUser(context)
                    && ImsManager.isNonTtyOrTtyOnVolteEnabled(context));
            if(!mWfcEnabled){
                showVoWiFiDialog();
            }else{
                ImsManager.setWfcSetting(context, !mWfcEnabled);
                sendIntentToUpdateQSTile(context);
            }
        } else {
            Log.w(this, "Received Unexpected Intent " + intent.toString());
        }
    }

    /**
     * Sends intent to Intent tile with all the QS information
     * and corresponding pending Intents included in Extra keys.
     *
     * @param context on which this broadcast is sent
     */
    private void sendIntentToUpdateQSTile(Context context) {
        Intent intent = new Intent();
        intent.setAction(WIFICALLING_TILE_ACTION);
        intent.putExtra(EXTRAKEY_CONTENT, CONTENT_WIFI_CALLING_Tile);
        intent.putExtra(EXTRAKEY_ICON_PACKAGE, context.getPackageName());

        updateIntentWifiCallingStatus(context, intent);
        updatePendingIntents(context, intent);

        context.sendBroadcast(intent);
    }

    /**
     * Update information related to QS Tile to present in the QuickSettings
     * Here we will update label and icon in QS.
     * @param context on which this broadcast is sent
     * @param intent on which the extra keys has to be added
     */
    private void updateIntentWifiCallingStatus(Context context, Intent intent) {
        mWfcEnabled = (ImsManager.isWfcEnabledByUser(context)
                && ImsManager.isNonTtyOrTtyOnVolteEnabled(context));

        //SSID need to be displayed only if WFC is enabled
        String label = mWfcEnabled ? getConnectedWifiSSID(context) : null;
        int iconId = mWfcEnabled ? R.drawable.ic_qs_wifi_calling_on
                : R.drawable.ic_qs_wifi_calling_off;

        intent.putExtra(EXTRAKEY_LABEL, label);
        intent.putExtra(EXTRAKEY_ICONID, iconId);
    }

    private void updatePendingIntents(Context context, Intent intent) {
        intent.putExtra(EXTRAKEY_ONCLICK,
                getPendingIntent(context, QS_ONCLICK_ACTION));
        intent.putExtra(EXTRAKEY_ONCLICK_URI, QS_ONCLICK_ACTION);
        intent.putExtra(EXTRAKEY_ONLAUNCH,
                getPendingIntent(context, QS_ONLAUNCH_ACTION));
        intent.putExtra(EXTRAKEY_ONLAUNCH_URI, QS_ONLAUNCH_ACTION);
        // onClick and onLongClick does the same task of toggling
        // the WIFI calling settings
        intent.putExtra(EXTRAKEY_ONLONGCLICK,
                getPendingIntent(context, QS_ONCLICK_ACTION));
        intent.putExtra(EXTRAKEY_ONLONGCLICK_URI, QS_ONCLICK_ACTION);
    }

    /**
     * Returns pending intent based on the action created,
     */
    private PendingIntent getPendingIntent(Context context, String action) {
        // Create an intent that will be wrapped in PendingIntent
        Intent intent = new Intent();
        intent.setAction(action);

        PendingIntent pendingIntent =
                PendingIntent.getBroadcast(context, 1, intent, 0);

        return pendingIntent;
    }

    /**
     * Returns SSID from WifiManager if WifiInfo is valid other wise null.
     */
    private String getConnectedWifiSSID(Context context) {
        WifiManager wifiManager = (WifiManager) context
                .getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        String label = null;
        if (wifiInfo != null && !wifiInfo.getSSID().trim().equals(SSID_NONE)
                && !wifiInfo.getSSID().trim().equals("0x")) {
            label = wifiInfo.getSSID();
        }
        return label;
    }

    private void showVoWiFiDialog() {
        AlertDialog vowifiDilog = new AlertDialog.Builder(mContext)
        .setTitle(mContext.getString(R.string.wifi_calling_enable_dialog_title))
        .setMessage(mContext.getString(R.string.wifi_calling_enable_dialog_msg))
        .setPositiveButton(R.string.wifi_calling_enable_dialog_enable,
                new OnClickListener(){
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        ImsManager.setWfcSetting(mContext, !mWfcEnabled);
                        sendIntentToUpdateQSTile(mContext);
                    }
                })
        .setNegativeButton(R.string.wifi_calling_enable_dialog_cancel,null)
        .create();
        vowifiDilog.getWindow().setType(WindowManager.LayoutParams.TYPE_STATUS_BAR_PANEL);
        WindowManager.LayoutParams attrs = vowifiDilog.getWindow().getAttributes();
        attrs.setTitle(getClass().getSimpleName());
        vowifiDilog.getWindow().setAttributes(attrs);
        vowifiDilog.show();
    }
}
