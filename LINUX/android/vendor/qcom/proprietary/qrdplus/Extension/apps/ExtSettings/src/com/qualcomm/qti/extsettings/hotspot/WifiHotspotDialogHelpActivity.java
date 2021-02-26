/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.extsettings.hotspot;

import android.app.Activity;
import android.app.ActionBar;
import android.content.Context;
import android.os.Bundle;
import android.view.MenuItem;
import android.net.wifi.WifiManager;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.net.wifi.WifiConfiguration;
import android.util.Log;
import com.qualcomm.qti.extsettings.WifiApSettingsActivity;
import com.qualcomm.qti.extsettings.hotspot.AccountCheck;
import com.qualcomm.qti.extsettings.WifiApDialog;

public class WifiHotspotDialogHelpActivity extends Activity {

    private WifiApDialog mDialog = null;
    private Context mContext;
    private WifiManager mWifiManager;
    private WifiConfiguration mWifiApConfig;
    public static final String INTENT_HOTSPOT_PRECONFIGURE = "Hotspot_PreConfigure";
    private static final String INTENT_HOTSPOT_POSTCONFIGURE = "Hotspot_PostConfigure";
    private static final String INTENT_EXTRA_CHOICE = "choice";
    private static final String INTENT_EXTRA_VALUE = "value";
    private static final String INTENT_EXTRA_NEED_SHOW_HELP_LATER = "needShowHelpLater";
    private int mChoice = -1;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message message) {
            if(message.arg1 == AccountCheck.ACCEPT) {
                if (mChoice == AccountCheck.WIFI_TETHERING) {
                    handleFirstHelp();
                } else {
                    AccountCheck.broadCastResponse(mContext, AccountCheck.ACCEPT);
                    finish();
                }
            } else {
                AccountCheck.broadCastResponse(mContext, AccountCheck.DENY);
            }
        }
    };

    private DialogInterface.OnClickListener mListener = new DialogInterface.OnClickListener() {
        public void onClick(DialogInterface dialogInterface, int button) {
            if (button != DialogInterface.BUTTON_POSITIVE) {
                AccountCheck.broadCastResponse(mContext, AccountCheck.DENY);
            } else {
                AccountCheck.broadCastResponse(mContext, AccountCheck.ACCEPT);
            }
            finish();
        }
    };

    private DialogInterface.OnClickListener mWifiAPListener = new DialogInterface.OnClickListener()
    {
        public void onClick(DialogInterface dialogInterface, int button) {
            if (button != DialogInterface.BUTTON_POSITIVE) {
                AccountCheck.broadCastResponse(mContext, AccountCheck.DENY);
            } else {
                mWifiApConfig = mDialog.getConfig();
                if (mWifiApConfig != null) {
                    if (mWifiManager==null) {
                        mWifiManager= (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
                        mWifiManager.setWifiEnabled(false);
                    }
                    mWifiManager.setWifiApEnabled(mWifiApConfig, true);
                }
                AccountCheck.broadCastResponse(mContext, AccountCheck.ACCEPT);
            }
            finish();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActionBar actionBar = getActionBar();
        actionBar.hide();
        mContext = this;
        Intent broadcast = getIntent();
        boolean bEnabled = broadcast.getBooleanExtra(INTENT_EXTRA_VALUE, false);
        String action = broadcast.getAction();
        mChoice = broadcast.getIntExtra(INTENT_EXTRA_CHOICE, 0);
        if(broadcast.getBooleanExtra(INTENT_EXTRA_NEED_SHOW_HELP_LATER, false)) {
            AccountCheck.needShowHelpLater(this);
        }
        if (action.equals(INTENT_HOTSPOT_PRECONFIGURE)) {
            if (AccountCheck.showNoSimCardDialog(mContext)) {
                AccountCheck.broadCastResponse(mContext, AccountCheck.DENY);
                return;
            }
            if (AccountCheck.isCarrierSimCard(mContext)) {
                AccountCheck.getInstance().checkAccount(mContext, mHandler.obtainMessage(1));
                return;
            }
            //for other carrier's SIM, fall below
            if (mChoice == AccountCheck.WIFI_TETHERING) {
                handleFirstHelp();
            } else if (mChoice == AccountCheck.USB_TETHERING) {
                AccountCheck.broadCastResponse(mContext, AccountCheck.ACCEPT);
                finish();
            }
        } else if (action.equals(INTENT_HOTSPOT_POSTCONFIGURE)) {
            if (mChoice == AccountCheck.WIFI_TETHERING) {
                if (AccountCheck.isHotspotAutoTurnOffEnabled(mContext)) {
                    String hotspotServiceClassName = HotspotService.class.getName();
                    if (!AccountCheck.isServiceRunning(mContext, hotspotServiceClassName)) {
                        Intent intent = new Intent().setClassName(mContext,
                                hotspotServiceClassName);
                        mContext.startService(intent);
                    }
                }
            }
            if (AccountCheck.showActivatedDialog(mContext, mChoice)) {
                return;
            }
            boolean showTurnOffWifiDialog = (mChoice != AccountCheck.USB_TETHERING || bEnabled);
            if (showTurnOffWifiDialog) {
                AccountCheck.showTurnOffWifiDialog(mContext);
            }
            if (mChoice == AccountCheck.USB_TETHERING) {
                AccountCheck.handWifiState(mContext, bEnabled);
            }
            if (!showTurnOffWifiDialog) {
                finish();
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    private void handleFirstHelp() {
         if(showFirstHelp(mContext)) {
             return;
         }
         mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
         if (AccountCheck.isPasswordEmpty(mWifiManager)) {
             AccountCheck.showPasswordEmptyDialog(mContext);
             AccountCheck.broadCastResponse(mContext,AccountCheck.DENY);
             return;
         }
         AccountCheck.broadCastResponse(mContext,AccountCheck.ACCEPT);
         finish();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return super.onOptionsItemSelected(item);
    }

    private boolean showFirstHelp(Context ctx) {
        if (AccountCheck.isNeedShowHelp(ctx)) {
            DialogInterface.OnClickListener okListener =
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface arg0, int arg1) {
                           AccountCheck.isWifiApBackKeyEnable(mContext, false);
                           mDialog = new WifiApDialog(mContext, mWifiAPListener, null);
                           mDialog.show();
                        }
                    };
            DialogInterface.OnClickListener laterListener =
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface arg0, int arg1) {
                           AccountCheck.needShowHelpLater(mContext);
                           AccountCheck.broadCastResponse(mContext,AccountCheck.DENY);
                           Intent intent = new Intent(WifiHotspotDialogHelpActivity.this,
                                   WifiApSettingsActivity.class);
                           startActivity(intent);
                           finish();
                        }
                    };
            AccountCheck.showHelpDialog(ctx, okListener, laterListener);
            return true;
        }
        return false;
    }
}
