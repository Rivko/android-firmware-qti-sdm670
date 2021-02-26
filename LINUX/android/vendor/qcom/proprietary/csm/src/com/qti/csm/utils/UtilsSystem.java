/*
 * Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.utils;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.media.MediaFile;
import android.os.Process;
import android.os.SystemProperties;

public class UtilsSystem {
    private static final String PER_STR = "persist.sys.strict_op_enable";
    private static final String STR_STRICT_MODE = "persist.vendor.strict_op_enable";
    private static final String STR_POWER_TEST = "persist.vendor.power_test";

    public static boolean isMonkeyRunning() {
        return ActivityManager.isUserAMonkey();
    }

    public static String getPackageNameByPid(Context context, int uid) {

        String packageName = "unknown";

        if (uid < Process.FIRST_APPLICATION_UID) {
            packageName = "uid:" + uid;
            return packageName;
        }
        PackageManager packageManager = context.getPackageManager();
        String packages[] = packageManager.getPackagesForUid(uid);
        if (packages != null) {
            if (packages.length == 1) {
                packageName = packages[0];
            } else if (packages.length > 1) {
                packageName = "uid:" + uid;
            }
        }
        return packageName;
    }

    public static boolean isImageFile(String path) {
        MediaFile.MediaFileType mediaFileType = MediaFile.getFileType(path);
        int fileType = (mediaFileType == null ? 0 : mediaFileType.fileType);
        return MediaFile.isImageFileType(fileType);
    }

    public static boolean strictOpMode() {
        return SystemProperties.getBoolean(PER_STR, false);
    }

    public static String getAppName(Context context, int uid, String packageName) {
        String displayName = "unknown";
        if (uid < Process.FIRST_APPLICATION_UID) {
            displayName = "uid:" + uid;
            return displayName;
        }
        PackageManager packageManager = context.getPackageManager();
        ApplicationInfo applicationInfo = null;
        try {
            applicationInfo = packageManager.getApplicationInfo(packageName, 0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        if (applicationInfo != null) {
            displayName = (String) packageManager.getApplicationLabel(applicationInfo);
        }
        return displayName;
    }

    public static Drawable getAppIcon(Context context, int uid, String packageName) {

        if (uid >= Process.FIRST_APPLICATION_UID) {
            try {
                Drawable appIcon = context.getPackageManager().getApplicationIcon(packageName);
                return appIcon;
            } catch (PackageManager.NameNotFoundException e) {
                e.printStackTrace();
            }
        }
        return context.getResources().getDrawable(android.R.drawable.sym_def_app_icon);
    }

    public static boolean powerTest() {
        if(SystemProperties.getBoolean(STR_STRICT_MODE, false)){
            return false;
        }
        if(SystemProperties.getBoolean(PER_STR, false)){
            return false;
        }
        return SystemProperties.getBoolean(STR_POWER_TEST, false);
    }
}