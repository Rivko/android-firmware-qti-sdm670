/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings.hotspot;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiManager;
import android.os.Message;
import android.os.AsyncTask;
import android.os.UserHandle;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.CheckBox;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.CoreConnectionPNames;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;
import static android.content.Context.TELEPHONY_SERVICE;
import com.qualcomm.qti.extsettings.R;

public class AccountCheck  {
    public static final String TAG = "AccountCheck";
    private static final boolean DEBUG = false;
    public static final int INVALID             = -1;
    public static final int WIFI_TETHERING      = 0;
    public static final int USB_TETHERING       = 1;
    private static AccountCheck thisInstance = null;
    private static CheckBox mNotShowAgainCheckbox;
    public static final int DENY = 0;
    public static final int ACCEPT = 1;
    public static final int HTTP_TIMEOUT = 10000;
    public static final int MAX_SERVERS_NUMBER = 100;

    public static final String KEY_WIFIAP_BACKKEY_ENABLE = "WifiApBackKeyEnable";
    public static final String KEY_TURN_OFF_WIFI_SHOW_AGAIN = "TurnOffWifiShowAgain";
    public static final String KEY_FIRST_LAUNCH_HOTSPOT
            = "FirstLaunchHotspotTethering";
    public static final String KEY_FIRST_HOTSPOT_ACTIVATED = "FirstHotspotActivated";
    public static final String KEY_FIRST_USB_ACTIVATED = "FirstUSBActivated";

    public static final String MY_PREF_FILE = "MY_PERFS";
    public static final String INTENT_HOTSPOT_PRECONFIGURE_RESPONSE
            = "Hotspot_PreConfigure_Response";
    public static final String INTENT_EXTRA_KEY = "PreConfigure_result";
    private static final int WIFI_STATE_ENABLE_ENABLING = 1;
    private static final int WIFI_STATE_DISABLE_DISABLING = 0;
    public static boolean WifiStateBeforeTurnOn = false;

    public String mServerUrl = "";
    public String upsellUrl = "";
    public ProgressDialog accoutCheckDialog;
    public AlertDialog accountCheckAlertDialog;
    private static OnDismissListener mDismissListener = new OnDismissListener(){
        public void onDismiss(DialogInterface arg0) {
            Activity activity = ((AlertDialog)arg0).getOwnerActivity();
            if (activity != null) {
                activity.finish();
            }
        }
    };
    public static AccountCheck getInstance() {
        if(thisInstance == null) {
            thisInstance = new AccountCheck();
        }
        return thisInstance;
    }

    public AccountCheck() {
    }

    private void initAccoutCheckDialog(Context ctx) {
        mServerUrl = Res.getSettingsString(ctx, Res.VALUE_SERVER_URL);
        upsellUrl =  Res.getSettingsString(ctx, Res.VALUE_UP_SELL_URL);
        if(!Res.getSettingsBoolean(ctx, Res.FLAG_REGIONAL_ACCOUNT_CHECK_DIALOG_ENABLE)) {
            accoutCheckDialog = ProgressDialog.show(ctx,
                "", Res.getSettingsString(ctx, Res.STRING_CHECK_ACCOUNT_MSG), true, false);
        } else {
            showAccountCheckAlertDialog(ctx);
        }
    }

    private void showAccountCheckAlertDialog(Context ctx) {
        AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
        builder.setTitle("");
        builder.setMessage(Res.getSettingsString(ctx, Res.STRING_CHECK_ACCOUNT_MSG));
        builder.setNegativeButton(Res.getSettingsString(ctx, Res.STRING_WIFI_CANCEL),
            new android.content.DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        builder.setCancelable(false);
        accountCheckAlertDialog = builder.create();
        accountCheckAlertDialog.show();
    }

    private void dismissAccoutCheckDialog()  {
        if(accoutCheckDialog != null && accoutCheckDialog.isShowing()) {
            accoutCheckDialog.dismiss();
        } else if(accountCheckAlertDialog != null && accountCheckAlertDialog.isShowing()) {
            accountCheckAlertDialog.dismiss();
        }
    }

    private boolean handleHttpResponse(int statusCode, String responeEntity) {
        if(DEBUG) {
            Log.d(TAG,"statusCode:"+statusCode);
        }
        if(statusCode == 200) {
            if(DEBUG) {
                Log.d(TAG,"strResult:"+responeEntity);
             }
             return (!((responeEntity != null) && responeEntity.equalsIgnoreCase("No")));
        } else {
            //according to requiremet id 59643, it must turn on usb/hotspot tethering
            //for status code 4xx or 5xx from server
            return statusCode / 100 == 4 || statusCode / 100 == 5;
        }
    }

    private boolean getHttpResponse() {
        if(TextUtils.isEmpty(mServerUrl)) {
            return true;
        }
        HttpClient client = new DefaultHttpClient();
        try {
            HttpGet httpRequest = new HttpGet(mServerUrl);
            HttpConnectionParams.setConnectionTimeout(client.getParams(), HTTP_TIMEOUT);
            HttpConnectionParams.setSoTimeout(client.getParams(), HTTP_TIMEOUT);
            HttpResponse response = client.execute(httpRequest);
            int statusCode = response.getStatusLine().getStatusCode();
            String strResult = EntityUtils.toString(response.getEntity());
            return handleHttpResponse(statusCode, strResult);
        } catch (ClientProtocolException err) {
            if(DEBUG) {
                Log.w(TAG,"ClientProtocolException error!");
            }
        } catch (IOException err) {
            if(DEBUG) {
                Log.w(TAG, "IOException error!");
            }
        } catch (Exception e) {
            if(DEBUG) {
                Log.w(TAG,"Exception occur!");
            }
        } finally {
            client.getConnectionManager().shutdown();
        }
        return true;
    }

    public void checkAccount(Context ctx, Message message) {
        AccoutCheckTask accCheckTask = new AccoutCheckTask(ctx,message);
        accCheckTask.execute();
    }

    private void initServiceUpgradeDialog(final Context ctx) {
        AlertDialog.Builder builder = new AlertDialog.Builder(ctx);
        builder.setTitle(Res.getSettingsString(ctx, Res.STRING_SERVER_UPGRADE_TITLE));
        builder.setMessage(Res.getSettingsString(ctx, Res.STRING_SERVER_MSG));
        builder.setPositiveButton(Res.getSettingsString(ctx, Res.STRING_SERVER_UPGRADE),
            new android.content.DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                try {
                    Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(upsellUrl));
                    ctx.startActivity(intent);
                    dialog.dismiss();
                } catch (Exception err) {
                    if(DEBUG) {
                        Log.w(TAG,"can not open url:"+upsellUrl);
                    }
                    err.printStackTrace();
                }
            }
        });
        builder.setNegativeButton(Res.getSettingsString(ctx, Res.STRING_LATER),
            new android.content.DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        builder.setCancelable(false);
        if (ctx instanceof WifiHotspotDialogHelpActivity) {
            builder.setOnDismissListener(mDismissListener);
            AlertDialog dialog = builder.create();
            dialog.setOwnerActivity(((WifiHotspotDialogHelpActivity)ctx));
            dialog.show();
            return;
        }
        builder.create().show();
    }

    public static boolean showNoSimCardDialog(Context ctx) {
        TelephonyManager tm = (TelephonyManager) ctx.getSystemService(TELEPHONY_SERVICE);
        if (!isSimCardReady(tm)) {
            AlertDialog.Builder alert = new AlertDialog.Builder(ctx);
            alert.setTitle(Res.getSettingsString(ctx, Res.STRING_NO_SIM_ALERT_TITLE));
            alert.setMessage(Res.getSettingsString(ctx, Res.STRING_NO_SIM_TEXT));
            alert.setPositiveButton(Res.getSettingsString(ctx, Res.STRING_OKAY), null);
            if (ctx instanceof WifiHotspotDialogHelpActivity) {
                alert.setOnDismissListener(mDismissListener);
                AlertDialog dialog = alert.create();
                dialog.setOwnerActivity(((WifiHotspotDialogHelpActivity)ctx));
                dialog.show();
                return true;
            }
            alert.show();
            return true;
        }
        return false;
    }

    private static boolean isSimCardReady(
        TelephonyManager telephonyManager) {
        return (telephonyManager.getSimState()
            == TelephonyManager.SIM_STATE_READY);
    }
    public static boolean isCarrierSimCard(Context ctx) {
        boolean isCarrierSimCard = false;
        String[] carrierMccMncs = Res.getSettingsStringArray(ctx, Res.ARRAY_CARRIER_OPERATOR_LIST);
        TelephonyManager tm = (TelephonyManager)ctx.getSystemService(
                Context.TELEPHONY_SERVICE);
        String simOperator = tm.getSimOperator();
        if (DEBUG) Log.d(TAG,
            "carrier sim card check: sim operator is " + simOperator);
        if (simOperator != null) {
            if (Arrays.asList(carrierMccMncs).contains(simOperator)) {
                isCarrierSimCard = true;
            } else {
                for(String s: Arrays.asList(carrierMccMncs)) {
                    if (simOperator.indexOf(s) >= 0) {
                        isCarrierSimCard = true;
                        break;
                    }
                }
            }
        }
        if (DEBUG) {
            Log.d(TAG,"is home Carrier SIM Card? " + isCarrierSimCard);
        }
        return isCarrierSimCard;
    }

    public static boolean checkWifiState(final Context ctx) {
        WifiStateBeforeTurnOn = false;
        ConnectivityManager connMgr = (ConnectivityManager) ctx.getSystemService(
                Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = connMgr == null ? null : connMgr.getNetworkInfo(
                ConnectivityManager.TYPE_WIFI);
        WifiStateBeforeTurnOn = (info != null && info.isConnected());
        return WifiStateBeforeTurnOn;
    }

    public static void showTurnOffWifiDialog(final Context ctx) {
        if (!Res.getSettingsBoolean(ctx, Res.FLAG_SHOW_TURN_WIFI_OFF) || !checkWifiState(ctx)) {
            if(ctx instanceof WifiHotspotDialogHelpActivity) {
                ((WifiHotspotDialogHelpActivity)ctx).finish();
            }
            return;
        }
        LayoutInflater inflater = (LayoutInflater)ctx.getSystemService(
                Context.LAYOUT_INFLATER_SERVICE);
        View showAgainView = inflater.inflate(R.layout.not_show_again, null);
        mNotShowAgainCheckbox = (CheckBox)showAgainView.findViewById(R.id.check);
        final SharedPreferences sharedpreferences = ctx.getSharedPreferences(
                MY_PREF_FILE, Context.MODE_PRIVATE);
        boolean showAgain = sharedpreferences.getBoolean(KEY_TURN_OFF_WIFI_SHOW_AGAIN, true);
        if (!showAgain) {
            if(ctx instanceof WifiHotspotDialogHelpActivity) {
                ((WifiHotspotDialogHelpActivity)ctx).finish();
            }
            return;
        }
        if (showAgain) {
            AlertDialog.Builder alert = new AlertDialog.Builder(ctx)
                    .setTitle(Res.getSettingsString(ctx, Res.STRING_TURN_OFF_WIFI_DLG_TITLE))
                    .setMessage(Res.getSettingsString(ctx, Res.STRING_TURN_OFF_WIFI_DLG_TEXT))
                    .setView(showAgainView)
                    .setPositiveButton(Res.getSettingsString(ctx,
                        Res.STRING_OKAY), new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Editor editor = sharedpreferences.edit();
                    editor.putBoolean(KEY_TURN_OFF_WIFI_SHOW_AGAIN,
                            !mNotShowAgainCheckbox.isChecked());
                    editor.commit();
                }
            });
            alert.setCancelable(false);
            if (ctx instanceof WifiHotspotDialogHelpActivity) {
                alert.setOnDismissListener(mDismissListener);
                AlertDialog dialog = alert.create();
                dialog.setOwnerActivity(((WifiHotspotDialogHelpActivity)ctx));
                dialog.show();
                return;
            }
            alert.show();
        }
    }

    public static void disableShowHelp(final Context ctx) {
        SharedPreferences sharedPreferences = ctx.getSharedPreferences(
                MY_PREF_FILE, Activity.MODE_PRIVATE);
        String strKey = KEY_FIRST_LAUNCH_HOTSPOT;
        boolean isFirstUse = sharedPreferences.getBoolean(strKey, true);
        if(isFirstUse) {
            Editor editor = sharedPreferences.edit();
            editor.putBoolean(strKey, false);
            editor.apply();
        }
    }

    public static boolean isNeedShowHelp(final Context ctx) {
        SharedPreferences sharedPreferences = ctx.getSharedPreferences(
                MY_PREF_FILE, Activity.MODE_PRIVATE);
        String strKey =  KEY_FIRST_LAUNCH_HOTSPOT;
        Editor editor = sharedPreferences.edit();
        boolean isFirstUse = sharedPreferences.getBoolean(strKey, true);
        if (isFirstUse) {
            editor.putBoolean(strKey, false);
            editor.apply();
        }
        return isFirstUse;
    }

    public static void needShowHelpLater(final Context ctx) {
        SharedPreferences sharedPreferences = ctx.getSharedPreferences(
                MY_PREF_FILE, Activity.MODE_PRIVATE);
        String strKey = KEY_FIRST_LAUNCH_HOTSPOT;
        Editor editor = sharedPreferences.edit();
        editor.putBoolean(strKey, true);
        editor.apply();
    }

    public static void showHelpDialog(final Context ctx,
            DialogInterface.OnClickListener okListener,
            DialogInterface.OnClickListener laterListener) {
        final AlertDialog.Builder build = new AlertDialog.Builder(ctx);
        build.setTitle(Res.getSettingsString(ctx, Res.STRING_TETHER_SETTING_LAUNCH_TITLE));
        build.setMessage(Res.getSettingsString(ctx, Res.STRING_WIFI_TETHER_FIRST_USE));
        build.setNegativeButton(Res.getSettingsString(ctx, Res.STRING_LATER), laterListener);
        build.setPositiveButton(Res.getSettingsString(ctx, Res.STRING_OKAY), okListener);
        build.setCancelable(false);
        build.show();
    }

    public static void isWifiApBackKeyEnable(Context ctx, boolean enable) {
        SharedPreferences sharedPreferences =
                ctx.getSharedPreferences(MY_PREF_FILE, ctx.MODE_PRIVATE);
        Editor editor = sharedPreferences.edit();
        editor.putBoolean(KEY_WIFIAP_BACKKEY_ENABLE, enable);
        editor.apply();
    }

    public static void showPasswordEmptyDialog(final Context ctx) {
        final AlertDialog.Builder build = new AlertDialog.Builder(ctx);
        build.setTitle(Res.getSettingsString(ctx, Res.STRING_HOTSPOT_PASSWORD_EMPTY_TITLE));
        build.setMessage(Res.getSettingsString(ctx, Res.STRING_HOTSPOT_PASSWORD_EMPTY_TEXT));
        build.setPositiveButton(Res.getSettingsString(ctx, Res.STRING_OKAY), null);
        if (ctx instanceof WifiHotspotDialogHelpActivity) {
            build.setOnDismissListener(mDismissListener);
            AlertDialog dialog = build.create();
            dialog.setOwnerActivity(((WifiHotspotDialogHelpActivity)ctx));
            dialog.show();
            return;
        }
        build.show();
    }

    public static boolean isNeedShowActivated (Context ctx, final int type) {
        SharedPreferences sharedPreferences = ctx.getSharedPreferences(
                MY_PREF_FILE, Activity.MODE_PRIVATE);
        Editor editor = sharedPreferences.edit();
        String strKey = (type == 0)? KEY_FIRST_HOTSPOT_ACTIVATED : KEY_FIRST_USB_ACTIVATED;
        boolean isFirstUse = sharedPreferences.getBoolean(strKey,true);
        if (isFirstUse) {
            editor = sharedPreferences.edit();
            editor.putBoolean(strKey, false);
            editor.commit();
        }
        return isFirstUse;
    }

    public static boolean showActivatedDialog(final Context ctx, final int type) {
        if (!isNeedShowActivated(ctx, type)) {
            return false;
        }
        final AlertDialog.Builder build = new AlertDialog.Builder(ctx);
        build.setTitle(Res.getSettingsString(ctx, (USB_TETHERING == type) ?
                Res.STRING_LEARN_USB_DLG_TITLE : Res.STRING_LEARN_HOTSPOT_DLG_TITLE));
        build.setMessage(Res.getSettingsString(ctx, (USB_TETHERING == type) ?
                Res.STRING_LEARN_USB_DLG_TEXT: Res.STRING_LEARN_HOTSPOT_DLG_TEXT));
        build.setPositiveButton(Res.getSettingsString(ctx, Res.STRING_YES),
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface arg0, int arg1) {
                        showMoreAboutActivation(ctx,type);
                    }
                });
        build.setNegativeButton(Res.getSettingsString(ctx, Res.STRING_SKIP),
                new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface arg0, int arg1) {
                    showTurnOffWifiDialog(ctx);
                }
            }
        );
        build.setCancelable(false);
        build.show();
        return true;
    }

    public static void showMoreAboutActivation(final Context ctx,int type) {
        final AlertDialog.Builder build = new AlertDialog.Builder(ctx);
        build.setTitle(Res.getSettingsString(ctx, Res.STRING_MOBILE_TETHER_HELP_DLG_TITLE));
        build.setMessage(Res.getSettingsString(ctx, (USB_TETHERING == type) ?
                Res.STRING_MOBILE_USB_HELP_DLG_TEXT : Res.STRING_MOBILE_HOTSPOT_HELP_DLG_TEXT));
        build.setPositiveButton(Res.getSettingsString(ctx, Res.STRING_OKAY),
                new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface arg0, int arg1) {
                    showTurnOffWifiDialog(ctx);
                }
            }
        );
        build.show();
    }

    public static boolean isPasswordEmpty(WifiManager wifiManager) {
        if (wifiManager == null) {
            return false;
        }
        WifiConfiguration wifiAPConfig = wifiManager.getWifiApConfiguration();
        if (wifiAPConfig == null) {
            return false;
        }
        if(TextUtils.isEmpty(wifiAPConfig.preSharedKey) && (
                wifiAPConfig.allowedKeyManagement.get(KeyMgmt.WPA2_PSK))) {
            return true;
        }
        return false;
    }

    private class AccoutCheckTask extends AsyncTask<Void, Integer, Boolean>{
        private final Context mContext;
        private Message mCurrentMsg = null;
        public AccoutCheckTask(Context context,Message currentMsg) {
            mContext = context;
            mCurrentMsg = currentMsg;
        }
         @Override
         protected void onPreExecute() {
             super.onPreExecute();
             initAccoutCheckDialog(mContext);
         }

         @Override
         protected void onPostExecute(Boolean result) {
             super.onPostExecute(result);
             dismissAccoutCheckDialog();
             mCurrentMsg.arg1 =result ? ACCEPT : DENY;
             mCurrentMsg.sendToTarget();
             if(!result) {
                 initServiceUpgradeDialog(mContext);
             }
         }

         @Override
         protected void onProgressUpdate(Integer... values) {
             super.onProgressUpdate(values);
         }

         @Override
         protected void onCancelled(Boolean result) {
             super.onCancelled(result);
         }

         @Override
         protected void onCancelled() {
             super.onCancelled();
         }

         @Override
         protected Boolean doInBackground(Void... params) {
            return getHttpResponse();
        }
    }

    public static boolean isHotspotAutoTurnOffEnabled(Context ctx) {
        boolean isHotspotAutoTurnOffEnabled = false;
        if (ctx != null) {
            isHotspotAutoTurnOffEnabled = Res.getSettingsBoolean(ctx,
                    Res.FLAG_EDIT_HOTSPOT_TIMEOUT_ENABLE);
        }
        return isHotspotAutoTurnOffEnabled;
    }

    public static boolean isServiceRunning(Context context, String className) {
        boolean isRunning = false;
        if (context != null && (!TextUtils.isEmpty(className))) {
            ActivityManager activityManager = (ActivityManager)
                    context.getSystemService(Context.ACTIVITY_SERVICE);
            List<ActivityManager.RunningServiceInfo> serviceList = activityManager
                    .getRunningServices(MAX_SERVERS_NUMBER);
            if (serviceList != null) {
                for (int i = 0; i < serviceList.size(); i++) {
                    if (serviceList.get(i).service.getClassName().equals(className)) {
                        isRunning = true;
                        break;
                    }
                }
            }
        }
        return isRunning;
    }

    public static void broadCastResponse(Context ctx, int result) {
        Intent hotspot_preConfigure_intent = new Intent(INTENT_HOTSPOT_PRECONFIGURE_RESPONSE);
        hotspot_preConfigure_intent.putExtra(INTENT_EXTRA_KEY, result == ACCEPT);
        ctx.sendBroadcastAsUser(hotspot_preConfigure_intent,UserHandle.ALL);
    }

    public static void handWifiState(Context context, boolean bShowTurnOff) {
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        if(DEBUG) {
            Log.d(TAG, "bShowTurnOff:"+bShowTurnOff + " Wifi State:"
                    + wifiManager.getWifiState());
        }
        if (bShowTurnOff) {
            if (wifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLING ||
                    wifiManager.getWifiState() == WifiManager.WIFI_STATE_ENABLED) {
                /**
                 * Disable Wifi if enabling tethering
                 */
                wifiManager.setWifiEnabled(false);
                setWifiSavedState(context, WIFI_STATE_ENABLE_ENABLING);
            }
        } else {
            if (getWifiSavedState(context) == WIFI_STATE_ENABLE_ENABLING) {
                wifiManager.setWifiEnabled(true);
                setWifiSavedState(context, WIFI_STATE_DISABLE_DISABLING);
            }
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
}
