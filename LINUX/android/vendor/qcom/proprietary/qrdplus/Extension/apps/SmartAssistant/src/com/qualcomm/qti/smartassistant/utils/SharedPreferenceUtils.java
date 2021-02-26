/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.preference.PreferenceManager;

import java.util.ArrayList;

public class SharedPreferenceUtils {
    private static final String SMART_ASSISTANT_ENABLED = "smart_assistant_eanbled";
    private static final String USER_VERIFICATION_ENABLED = "user_verification_enabled";
    private static final String LAB_ENABLED = "lab_enabled";
    private static final String SOUND_MODE_USERS = "sound_model_users";
    private static final String SOUND_MODE_USERS_NUM = "sound_model_users_num";

    /**
     * used for platform build compatible
     */
    private static boolean isAtLeastN() {
        return Build.VERSION.SDK_INT >= 24;
    }

    private static SharedPreferences getDefaultSharedPreferences(Context context) {
        final Context storageContext;
        if (isAtLeastN()) {
            // All N devices have split storage areas. Migrate the existing preferences into the
            // new device encrypted storage area if that has not yet occurred.
            final String name = PreferenceManager.getDefaultSharedPreferencesName(context);
            storageContext = context.createDeviceProtectedStorageContext();
            if (!storageContext.moveSharedPreferencesFrom(context, name)) {
                LogUtils.e("Failed to migrate shared preferences");
            }
        } else {
            storageContext = context;
        }
        return PreferenceManager.getDefaultSharedPreferences(storageContext);
    }

    public static boolean getAssistantEnabled(Context context) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        return prefs.getBoolean(SMART_ASSISTANT_ENABLED, false);
    }

    public static void setAssistantEnabled(Context context, boolean isEnabled) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(SMART_ASSISTANT_ENABLED, isEnabled);
        editor.apply();
    }

    public static boolean getIsUserVerificationEnabled(Context context) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        return prefs.getBoolean(USER_VERIFICATION_ENABLED, false);
    }

    public static void setIsUserVerificationEnabled(Context context, boolean isEnabled) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(USER_VERIFICATION_ENABLED, isEnabled);
        editor.apply();
    }

    public static boolean getIsLABEnabled(Context context) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        return prefs.getBoolean(LAB_ENABLED, false);
    }

    public static void setIsLABEnabled(Context context, boolean isLabEnabled) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(LAB_ENABLED, isLabEnabled);
        editor.apply();
    }

    public static ArrayList<String> getUserListData(Context context) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        ArrayList<String> users = new ArrayList<>();
        int num = prefs.getInt(SOUND_MODE_USERS_NUM, 0);
        for (int i = 0; i < num; i++) {
            String modelUser = prefs.getString(SOUND_MODE_USERS + i, null);
            if (null != modelUser) {
                users.add(modelUser);
            }
        }
        return users;
    }

    public static String getRealUserName(Context context, int index) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        return prefs.getString(SOUND_MODE_USERS + index, null);
    }

    public static void addUserListData(Context context, String user) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        int usernum = prefs.getInt(SOUND_MODE_USERS_NUM, 0);
        editor.putInt(SOUND_MODE_USERS_NUM, usernum + 1);
        editor.putString(SOUND_MODE_USERS + usernum, user);
        editor.putString(user, user);
        editor.apply();
    }

    public static void renameUserData(Context context, int index, String newUser) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        String user = prefs.getString(SOUND_MODE_USERS + index, null);
        if (null != user) {
            editor.putString(user, newUser);
            editor.apply();
        }

    }

    public static void clearUsers(Context context) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        int number = prefs.getInt(SOUND_MODE_USERS_NUM, 0);

        for(int i = 0; i < number; i++) {
            String rName = prefs.getString(SOUND_MODE_USERS + i, null);
            if (null != rName) {
                editor.remove(rName);
            }
            editor.remove(SOUND_MODE_USERS + i);
        }
        editor.putInt(SOUND_MODE_USERS_NUM, 0);
        editor.apply();
    }

    public static void removeUser(Context context, int index) {
        SharedPreferences prefs = getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = prefs.edit();
        String user = prefs.getString(SOUND_MODE_USERS + index, null);
        int number = prefs.getInt(SOUND_MODE_USERS_NUM, 0);
        ArrayList<String> tmpName = new ArrayList<>();
        for (int i = index + 1; i < number; i++) {
            tmpName.add(prefs.getString(SOUND_MODE_USERS + i, null));
        }

        if (null != user) {
            editor.remove(user);
        }

        int m = index;
        for (String u : tmpName) {
            editor.putString(SOUND_MODE_USERS + m, u);
            m++;
        }
        m = index;

        editor.remove(SOUND_MODE_USERS + (number - 1));
        number--;
        editor.putInt(SOUND_MODE_USERS_NUM, number >=0 ? number : 0);
        editor.apply();
    }
}
