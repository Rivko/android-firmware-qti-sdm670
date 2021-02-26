/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PermissionInfo;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import java.util.ArrayList;

/**
 * This class handles Android M permission requests for demo apps. It explicitly requests all
 * dangerous permissions listed in the app manifest that have not already been granted. In this
 * sense, it emulates the permissions behaviour of previous versions of Android; all permissions
 * will always be available, or the app will not start. This is acceptable for our demo apps, as we
 * don't want permission popups interrupting the flow. In practice, these apps would be installed
 * as system apps, and thus would be granted all their permissions automatically anyway. After
 * handling permissions, it will start the activity specified by the nextActivityName metadata
 * element specified in the application manifest for this activity.
 */
public class StartupPermissionRequestActivity extends Activity {
    private static final String TAG = "SocialCamera";

    private ComponentName nextActivity;

    @Override
    @TargetApi(Build.VERSION_CODES.M)
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final PackageManager pm = getPackageManager();

        final ActivityInfo ai;
        try {
            ai = pm.getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
        } catch (PackageManager.NameNotFoundException e) {
            Log.e(TAG, "Failed getting activity info for component " + getComponentName(), e);
            Toast.makeText(this, R.string.startup_permission_request_error_failed,
                    Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        try {
            String nextActivityName = ai.metaData.getString("nextActivityName", "");
            if (nextActivityName.charAt(0) == '.') {
                nextActivityName = ai.packageName + nextActivityName;
            }
            nextActivity = new ComponentName(this, nextActivityName);
            pm.getActivityInfo(nextActivity, 0);
        } catch (IndexOutOfBoundsException | PackageManager.NameNotFoundException e) {
            Log.e(TAG, "Failed getting activity info for next component " + nextActivity, e);
            Toast.makeText(this, R.string.startup_permission_request_error_failed,
                    Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            Log.i(TAG, "Android version is prior to " + Build.VERSION_CODES.M +
                    ", runtime permission requests not required");
            startNextActivity();
            return;
        }

        final PackageInfo pi;
        try {
            pi = pm.getPackageInfo(getPackageName(), PackageManager.GET_PERMISSIONS);
        } catch (PackageManager.NameNotFoundException e) {
            Log.e(TAG, "Failed getting package info for package " + getPackageName(), e);
            Toast.makeText(this, R.string.startup_permission_request_error_failed,
                    Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        ArrayList<String> permissions = new ArrayList<>();

        for (String permission : pi.requestedPermissions) {
            try {
                PermissionInfo info = pm.getPermissionInfo(permission, 0);
                if (info.protectionLevel == PermissionInfo.PROTECTION_DANGEROUS &&
                        checkSelfPermission(permission) != PackageManager.PERMISSION_GRANTED) {
                    permissions.add(info.name);
                }
            } catch (PackageManager.NameNotFoundException e) {
                Log.e(TAG, "Failed getting permission info for permission " + permission, e);
                Toast.makeText(this, R.string.startup_permission_request_error_failed,
                        Toast.LENGTH_SHORT).show();
                finish();
                return;
            }
        }

        if (!permissions.isEmpty()) {
            Log.i(TAG, "Requesting permissions: " + permissions);
            requestPermissions(permissions.toArray(new String[permissions.size()]), 0);
        } else {
            startNextActivity();
        }
    }

    @Override
    @SuppressWarnings("NullableProblems")
    public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                           int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        boolean granted = true;
        for (int i = 0; i < grantResults.length; i++) {
            if (grantResults[i] != PackageManager.PERMISSION_GRANTED) {
                Log.e(TAG, "Permission [" + permissions[i] + "] not granted");
                granted = false;
            }
        }

        if (!granted) {
            Log.e(TAG, "One or more permissions not granted, exiting");
            Toast.makeText(this, R.string.startup_permission_request_error_failed,
                    Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        startNextActivity();
    }

    private void startNextActivity() {
        startActivity(new Intent().setComponent(nextActivity));
        finish();
        overridePendingTransition(0, 0);
    }
}
