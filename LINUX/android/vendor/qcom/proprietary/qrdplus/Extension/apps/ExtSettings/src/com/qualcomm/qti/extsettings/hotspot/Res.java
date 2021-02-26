/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings.hotspot;
import android.content.Context;
import android.text.TextUtils;
import android.util.Log;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import android.content.res.Resources;

public class Res {

    private Context mContext = null;
    private static Res thisInstance = null;
    // The following resource will be load from com.android.settings for overlay
    // here is for flags' resource name
    public static final String FLAG_SHOW_BROADCAST_SSID
            = "R.bool.config_regional_hotspot_show_broadcast_ssid_checkbox";
    public static final String FLAG_SHOW_ALERT_FOR_OPEN
            = "R.bool.hotspot_show_alert_for_open_security";
    public static final String FLAG_SHOW_TURN_WIFI_OFF
            = "R.bool.hotspot_show_turn_off_wifi_dialog";
    public static final String FLAG_SOW_HELP_FOR_FIRST_USING
            = "R.bool.tethering_show_help_for_first_using";
    public static final String FLAG_NEED_SHOW_ACTIVATED = "R.bool.tethering_need_show_activated";
    public static final String FLAG_EDIT_HOTSPOT_TIMEOUT_ENABLE
            = "R.bool.edit_hotspot_timeout_enable";
    public static final String FLAG_SHOW_MAXIMUN_CONNECTION_ENABLE
            = "R.bool.config_regional_hotspot_show_maximum_connection_enable";
    public static final String FLAG_REGIONAL_ACCOUNT_CHECK_DIALOG_ENABLE
            = "R.bool.config_regional_account_check_dialog_enable";
    public static final String FLAG_USE_EMPTY_PASSWORD = "R.bool.use_empty_password_default";
    public static final String FLAG_ACCOUNT_CHECK_ENABLE = "R.bool.hotspot_accout_check_enable";
    public static final String FLAG_WIFI_CALL_NOTIFICATION_ENABLE
            = "R.bool.config_regional_wifi_calling_notificaion_enable";

    public static final String SETTINGS_PACKAGE_NAME = "com.android.settings";

    // here is for strings' resource name
    public static String STRING_WIFIAP_CONFIGURE_SUBTEXT = "R.string.wifiap_configure_subtext";
    public static String STRING_WIFIAP_TURN_ON_HINT = "R.string.wifiap_turn_on_hint";
    public static String STRING_WIFIAP_SETTINGS_DESC = "R.string.wifiap_settings_desc";
    public static String STRING_WIFIAP_SETTINGS_INDICATION = "R.string.wifiap_settings_indication";
    public static String STRING_WIFIAP_SETTINGS_INDICATION_STEP1
            = "R.string.wifiap_settings_indication_step1";
    public static String STRING_WIFIAP_SETTINGS_INDICATION_STEP2
            = "R.string.wifiap_settings_indication_step2";

    public static final String STRING_CONFIGURE_AP_TEXT
            = "R.string.wifi_tether_configure_ap_text";
    public static final String STRING_WIFI_TETHER_TEXT = "R.string.wifi_tether_checkbox_text";
    public static final String STRING_WIFI_SAVE = "R.string.wifi_save";
    public static final String STRING_WIFI_CANCEL = "R.string.wifi_cancel";
    public static final String STRING_CHECK_ACCOUNT_MSG = "R.string.account_check_msg";
    public static final String STRING_SSID_DLG_TITLE = "R.string.ssid_broadcast_dialog_title";
    public static final String STRING_SSID_DLG_TEXT = "R.string.ssid_broadcast_dialog_text";
    public static final String STRING_OKAY = "R.string.okay";
    public static final String STRING_SERVER_UPGRADE_TITLE = "R.string.service_upgrade_title";
    public static final String STRING_SERVER_MSG = "R.string.service_msg";
    public static final String STRING_SERVER_UPGRADE = "R.string.service_upgrade";
    public static final String STRING_LATER = "R.string.later";
    public static final String STRING_NO_SIM_ALERT_TITLE
            = "R.string.tethering_no_sim_alert_title";
    public static final String STRING_NO_SIM_TEXT = "R.string.tethering_no_sim_card_text";
    public static final String ARRAY_CARRIER_OPERATOR_LIST = "R.array.carrier_operator_list";
    public static final String STRING_TURN_OFF_WIFI_DLG_TITLE
            = "R.string.turn_off_wifi_dialog_title";
    public static final String STRING_TURN_OFF_WIFI_DLG_TEXT
            = "R.string.turn_off_wifi_dialog_text";
    public static final String STRING_TETHER_SETTING_LAUNCH_TITLE
            = "R.string.tether_settings_launch_title";
    public static final String STRING_WIFI_TETHER_FIRST_USE
            = "R.string.wifi_tether_first_use_message";
    public static final String STRING_HOTSPOT_PASSWORD_EMPTY_TITLE
            = "R.string.hotspot_password_empty_title";
    public static final String STRING_HOTSPOT_PASSWORD_EMPTY_TEXT
            = "R.string.hotspot_password_empty_text";
    public static final String STRING_LEARN_USB_DLG_TITLE = "R.string.learn_usb_dialog_title";
    public static final String STRING_LEARN_USB_DLG_TEXT = "R.string.learn_usb_dialog_text";
    public static final String STRING_LEARN_HOTSPOT_DLG_TITLE
            = "R.string.learn_hotspot_dialog_title";
    public static final String STRING_LEARN_HOTSPOT_DLG_TEXT
            = "R.string.learn_hotspot_dialog_text";
    public static final String STRING_YES = "R.string.yes";
    public static final String STRING_SKIP = "R.string.skip_label";
    public static final String STRING_MOBILE_TETHER_HELP_DLG_TITLE
            = "R.string.mobile_tether_help_dialog_title";
    public static final String STRING_MOBILE_USB_HELP_DLG_TEXT
            = "R.string.mobile_usb_help_dialog_text";
    public static final String STRING_MOBILE_HOTSPOT_HELP_DLG_TEXT
            = "R.string.mobile_hotspot_help_dialog_text";
    public static final String MAXIMUN_CONNECTION_DLG_TITLE
            = "R.string.maximum_connections_dialog_title_text";
    public static final String MAXIMUN_CONNECTION_DLG_POSITIVE
            = "R.string.maximum_connections_dialog_positive_text";
    public static final String MAXIMUN_CONNECTION_DLG_NEGATIVE
            = "R.string.maximum_connections_dialog_negative_text";

    public static final String WIFI_CALL_STATUS_DISABLED
            = "R.string.wifi_call_status_disabled";
    public static final String WIFI_CALL_STATUS_CELLULAR_PREFERRED
            = "R.string.wifi_call_status_cellular_preferred";
    public static final String WIFI_CALL_STATUS_WIFI_OFF
            = "R.string.wifi_call_status_wifi_off";
    public static final String WIFI_CALL_STATUS_NOT_CONNECTED_WIFI
            = "R.string.wifi_call_status_not_connected_wifi";
    public static final String WIFI_CALL_STATUS_ENABLING
            = "R.string.wifi_call_status_enabling";
    public static final String WIFI_CALL_STATUS_ERROR_UNKNOWN
            = "R.string.wifi_call_status_error_unknown";
    public static final String WIFI_CALL_STATUS_POOR_SIGNAL
            = "R.string.wifi_call_status_poor_wifi_signal";
    public static final String WIFI_CALL_STATUS_READY
            = "R.string.wifi_call_status_ready";

    public static final String STRING_WIFI_PREFERRED
            = "R.string.wifi_preferred";
    public static final String STRING_WIFI_ONLY
            = "R.string.wifi_only";
    public static final String STRING_CELLULAR_PREFERRED
            = "R.string.cellular_preferred";
    public static final String STRING_CELLULAR_ONLY
            = "R.string.cellular_only";
    public static final String STRING_WIFI_PREF_NONE
            = "R.string.wifi_pref_none";
    public static final String STRING_WIFI_HELP_TITLE
            = "R.string.wifi_help_title";
    public static final String STRING_WIFI_HELP_OPTION_TUTORIAL
            = "R.string.wifi_help_option_tutorial";
    public static final String STRING_WIFI_HELP_OPTION_TOP_QUESTIONS
            = "R.string.wifi_help_option_top_questions";

    public static final String STRING_WIFI_CALL_NOTIFICATION_TITLE
            = "R.string.wifi_calling_notification_title";
    public static final String STRING_WIFI_CALL_NOTIFICATION_SUBTITLE
            = "R.string.wifi_calling_notification_subtitle";

    public static final String STRING_WIFI_CALL_TUTORIAL_TITLE
            = "R.string.wifi_calling_tutorial_title";
    public static final String STRING_WIFI_CALL_QUESTION_TITLE
            = "R.string.wifi_calling_questions_title";

    public static final String STRING_NEXT_LABEL
            = "R.string.next_label";
    public static final String STRING_WIFI_CALL_WIZARD_CONTENT_STEP2
            = "R.string.wifi_calling_wizard_content_step_2";
    public static final String STRING_WIFI_DISPLAY_OPTION_DONE
            = "R.string.wifi_display_options_done";
    public static final String STRING_WIFI_CALL_WIZARD_CONTENT_STEP3
            = "R.string.wifi_calling_wizard_content_step_3";
    public static final String STRING_WIFI_CALL_
            = "";
    // here is for configure values' resource name
    public static final String VALUE_SERVER_URL = "R.string.hotspot_accout_check_server_url";
    public static final String VALUE_UP_SELL_URL = "R.string.hotspot_accout_up_sell_url";

    // here is for array resource name
    public static final String ARRAY_WIFI_CALL_PREFERENCES_ENTRIES_TITLE
            = "R.array.wifi_call_preferences_entries_title";
    public static final String ARRAY_WIFI_CALL_PREFERENCES_ENTRIES_SUMMARY
            = "R.array.wifi_call_preferences_entries_summary";
    public static final String ARRAY_WIFI_CALL_PREFERENCES_ENTRIES
            = "R.array.wifi_call_preferences_entries";

    // here is for drawable resource
    public static final String DRAWABLE_WIFI_CALL_ON_NOTIFICATION
            = "R.drawable.wifi_calling_on_notification";
    public static final String DRAWABLE_WIFI_CALL_NOTI_ERROR
            = "R.drawable.wifi_calling_noti_error";
    public static final String DRAWABLE_WIFI_CALL_INCALL_NOTI
            = "R.drawable.wifi_calling_incall_noti";

    public static final String SUPPRESS_WIFI_CALL_ERROR_NOTIFICATION
            = "R.bool.config_regional_suppress_wifi_call_error_notification";

    public static final String WIFI_CALL_ONLY_IN_ROAMING
            = "R.bool.config_regional_wifi_call_only_in_roaming";

    public static final String HOTSPOT_ON_10MINS_DEFAULT
            = "R.bool.config_keep_hotspot_on_10mins_default";

    public Res(Context ctx, String strPackage) {
        try {
            mContext = ctx.createPackageContext(strPackage,
                    Context.CONTEXT_IGNORE_SECURITY);
        } catch (Exception err) {
            err.printStackTrace();
        }
    }
    public Context getContext() {
        return mContext;
    }

    public int getID(String strResName) {
        if (mContext == null) return 0;
        String[] attrs = decode(strResName);
        Resources res=mContext.getResources();
        if (TextUtils.isEmpty(attrs[0])) {
            return res.getIdentifier(attrs[2],attrs[1],mContext.getPackageName());
        } else {
            return res.getIdentifier(attrs[2],attrs[1],attrs[0]);
        }
    }

    public static int getResID(Context ctx, String strResName) {
        if (thisInstance == null) {
            thisInstance = new Res(ctx, SETTINGS_PACKAGE_NAME);
        }
        return thisInstance.getID(strResName);
    }

    private String[] decode(String strResID) {
        int iPackageLen = strResID.indexOf("R.");
        String strField = strResID.substring(iPackageLen+2,strResID.length());
        String[] attrs = strField.split("\\.");
        String strPackage = strResID.substring(0, iPackageLen);
        if (iPackageLen >0)
            strPackage = strResID.substring(0, iPackageLen-1);
        return new String[] {strPackage,attrs[0],attrs[1]};
    }

    public static boolean getSettingsBoolean(Context ctx, String strResName) {
        if (thisInstance == null) {
            thisInstance = new Res(ctx, SETTINGS_PACKAGE_NAME);
        }
        try {
            Context settingCtx = thisInstance.getContext();
            return settingCtx.getResources().getBoolean(thisInstance.getID(strResName));
        } catch (Exception err) {
           err.printStackTrace();
           return false;
        }
    }

    public static String getSettingsString(Context ctx, String strResName) {
        if (thisInstance == null) {
            thisInstance = new Res(ctx, SETTINGS_PACKAGE_NAME);
        }
        try {
            Context settingCtx = thisInstance.getContext();
            return settingCtx.getResources().getString(thisInstance.getID(strResName));
        } catch (Exception err) {
           err.printStackTrace();
           return "";
        }
    }

    public static String getSettingsString(Context ctx, int iResID) {
        if (thisInstance == null) {
            thisInstance = new Res(ctx, SETTINGS_PACKAGE_NAME);
        }
        try {
            Context settingCtx = thisInstance.getContext();
            return settingCtx.getResources().getString(iResID);
        } catch (Exception err) {
           err.printStackTrace();
           return "";
        }
    }

    public static int getSettingsInt(Context ctx, String strResName) {
        if (thisInstance == null) {
            thisInstance = new Res(ctx, SETTINGS_PACKAGE_NAME);
        }
        try {
            Context settingCtx = thisInstance.getContext();
            return settingCtx.getResources().getInteger(thisInstance.getID(strResName));
        }  catch (Exception err) {
           err.printStackTrace();
           return -1;
        }
    }

    public static String[] getSettingsStringArray(Context ctx, String strResName) {
        if (thisInstance == null) {
            thisInstance = new Res(ctx, SETTINGS_PACKAGE_NAME);
        }
        try {
            Context settingCtx = thisInstance.getContext();
            return settingCtx.getResources().getStringArray(thisInstance.getID(strResName));
        }  catch (Exception err) {
           err.printStackTrace();
           return null;
        }
    }
}
