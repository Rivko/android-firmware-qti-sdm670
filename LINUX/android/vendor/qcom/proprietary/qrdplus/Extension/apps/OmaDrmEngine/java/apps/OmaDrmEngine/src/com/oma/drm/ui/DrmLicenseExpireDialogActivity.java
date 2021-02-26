/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm.ui;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.drm.DrmInfoEvent;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;

import com.oma.drm.R;

public class DrmLicenseExpireDialogActivity extends Activity implements
        DialogInterface.OnCancelListener {

    public static final String KEY_EVENT_MESSAGE = "info_event_message";
    public static final String KEY_EVENT_TYPE = "info_event_type";

    private static final int REQUEST_WRITE_EXTERNAL_STORAGE = 10;
    private boolean hasUri = false;
    private String message;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        int type = getIntent().getIntExtra(KEY_EVENT_TYPE,
                DrmInfoEvent.TYPE_RIGHTS_REMOVED);
        message = getIntent().getStringExtra(KEY_EVENT_MESSAGE);
        if (!TextUtils.isEmpty(message)) {
            hasUri = true;
        }
        grantPermissions();
    }

    public void onRequestPermissionsResult(int requestCode,
            String[] permissions, int[] grantResults) {
        switch (requestCode) {
        case REQUEST_WRITE_EXTERNAL_STORAGE:
            if (grantResults.length > 0
                    && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                showLicenseDialog();
            } else {
                // permission denied, finish activity
                Toast.makeText(this, R.string.permission_desc_access_drm_data,
                        Toast.LENGTH_SHORT).show();
                finish();
            }
            break;
        }
    }

    /**
     * check for nongranted dangerous permissions and request
     */
    private void grantPermissions() {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[] { Manifest.permission.WRITE_EXTERNAL_STORAGE },
                    REQUEST_WRITE_EXTERNAL_STORAGE);
        }else{
            showLicenseDialog();
        }
    }

    private void showLicenseDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setIconAttribute(android.R.attr.alertDialogIcon)
                .setTitle(R.string.license_expired)
                .setMessage(R.string.license_expired_message)
                .setCancelable(false).setOnCancelListener(this);
        if (hasUri) {
            builder.setPositiveButton(R.string.buy_license,
                    new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            Intent intent = new Intent();
                            intent.setAction(Intent.ACTION_VIEW);
                            intent.setData(Uri.parse(message));
                            startActivity(intent);
                            onCancel(dialog);
                        }
                    });
            builder.setNegativeButton(android.R.string.cancel,
                    new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            onCancel(dialog);
                        }
                    });
        } else {
            builder.setNegativeButton(android.R.string.ok,
                    new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            onCancel(dialog);
                        }
                    });
        }

        builder.create().show();
    }

    @Override
    public void onCancel(DialogInterface dialog) {
        finish();
    }

}
