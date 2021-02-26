/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.permission;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.ListView;
import android.widget.TextView;
import com.qti.csm.R;

public class BaseActivity extends Activity {
    TextView mTvBoot = null;
    TextView mTvAuto = null;
    TextView mTvNotice = null;
    PackageManager mPackageManager = null;
    ListView mListView = null;
    AlertDialog dialog;

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.permission);
        mTvBoot = (TextView) findViewById(R.id.tv_boot_mms);
        mTvAuto = (TextView) findViewById(R.id.tv_auto_data);
        mTvNotice = (TextView) findViewById(R.id.notice);
        mListView = (ListView) findViewById(R.id.list);
        mPackageManager = getPackageManager();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    };

    public void showDialog() {
        dialog = new AlertDialog.Builder(this).setTitle(R.string.dialog_title).setMessage(R.string.dialog_sum).show();
        dialog.setCanceledOnTouchOutside(false);
    }
}
