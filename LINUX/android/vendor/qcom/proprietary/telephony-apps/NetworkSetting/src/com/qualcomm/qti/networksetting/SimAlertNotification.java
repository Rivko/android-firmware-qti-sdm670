/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.networksetting;

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.res.Resources;
import android.text.TextUtils;
import android.util.Log;
import android.view.WindowManager;

public class SimAlertNotification extends BroadcastReceiver implements OnClickListener,
        OnDismissListener {
    private static final String TAG = "SimAlertNotification";
    private static final boolean DEBUG = true;

    private AlertDialog mNotUiccCardDialog = null;
    private Context mContext;

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        alertNotUiccCard();
    }

    private void alertNotUiccCard() {
        logd("alertNotUiccCard");

        if ((mNotUiccCardDialog != null && mNotUiccCardDialog.isShowing())) {
            return;
        }
        AlertDialog.Builder builder = new AlertDialog
            .Builder(mContext,R.style.CustomAlertDialogBackground);
        mNotUiccCardDialog = builder.setTitle(R.string.sim_info)
                .setMessage(R.string.alert_not_ct_uicc_card)
                .setNegativeButton(R.string.close, this).create();
        mNotUiccCardDialog.getWindow().setType(
                WindowManager.LayoutParams.TYPE_SYSTEM_DIALOG);
        mNotUiccCardDialog.setOnDismissListener(this);
        mNotUiccCardDialog.show();
    }

    @Override
    public void onDismiss(DialogInterface dialog) {
        if (mNotUiccCardDialog == (AlertDialog) dialog) {
            mNotUiccCardDialog = null;
        }
    }

     @Override
    public void onClick(DialogInterface dialog, int which) {

    }

    void logd(String msg) {
        if (DEBUG) {
            Log.d(TAG, msg);
        }
    }

}
