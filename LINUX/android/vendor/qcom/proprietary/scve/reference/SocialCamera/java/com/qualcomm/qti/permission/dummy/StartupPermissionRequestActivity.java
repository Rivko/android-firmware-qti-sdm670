/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

/**
 * DUMMY, DO NOT CHECK IN
 */
public class StartupPermissionRequestActivity extends Activity {
    private static final String TAG = "SocialCamera";

    private ComponentName nextActivity;

    @Override
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

        // N.B. Permissions request code was here, removed

        startNextActivity();
    }

    private void startNextActivity() {
        startActivity(new Intent().setComponent(nextActivity));
        finish();
        overridePendingTransition(0, 0);
    }
}
