/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.ui;

import android.app.Dialog;
import android.content.Context;

import com.qualcomm.qti.setupwizard.R;

public class SimProgressDialog extends Dialog {
    private static SimProgressDialog mSimProgressDialog;

    public SimProgressDialog(Context context) {
        super(context);
    }

    public SimProgressDialog(Context context, int themeResId) {
        super(context, themeResId);
    }

    protected SimProgressDialog(Context context, boolean cancelable,
             OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
    }

    public static SimProgressDialog createProgressDialog(Context context) {
        mSimProgressDialog = new SimProgressDialog(context);
        mSimProgressDialog.setContentView(R.layout.search_sim_progress_dialog);
        return mSimProgressDialog;
    }
}
