/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.MeshViewer.utils;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.util.DisplayMetrics;
import android.widget.Toast;

public class Helper {

    public static int getNotificationBarHeight(Context context) {
        int result = 0;
        int resourceId = context.getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = context.getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }

    public static void LaunchActivity(Context context, String packageName, String activityName) {
        Intent intent = new Intent();
        intent.setClassName(packageName, activityName);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);

        try {
            context.startActivity(intent);
        } catch(ActivityNotFoundException e)
        {
            Toast.makeText(context, "Activity " + intent.getClass().getCanonicalName() + " not found!", Toast.LENGTH_SHORT).show();
        }
    }

    public static int dpToPx(Context context, int dp) {
        DisplayMetrics displayMetrics = context.getResources().getDisplayMetrics();
        int px = Math.round(dp * (displayMetrics.densityDpi / DisplayMetrics.DENSITY_DEFAULT));
        return px;
    }
}
