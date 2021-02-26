/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.settings;

import java.util.Set;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.preference.PreferenceManager;
import android.util.Log;
import android.util.TypedValue;

import com.qualcomm.qti.sta.engine.JniConstants;
import com.qualcomm.qti.sta.R;

/**
 * Class for handling preferences data
 */
public class PreferenceData {
    private static final String LOG_TAG = "PreferenceData";

    /* package */static final String KEY_CAMERA_INSTRUCTIONS = "cameraInstructions";
    /* package */static final String KEY_CAMERA_STEADY_DURATION = "cameraSteadyDuration";
    /* package */static final String KEY_CAMERA_MOVE_THRESHOLD = "cameraMoveThreshold";
    /* package */static final String KEY_STA_DATA_TYPES = "STADataTypes";
    /* package */static final String KEY_STA_LOCALE_TYPE = "STALocaleType";
    /* package */static final String KEY_SCVE_MODE = "STAScveMode";
    /* package */static final String KEY_TEXT_RECORD_MODE = "STATextRecordMode";
    /* package */static final String KEY_IC_REGION_EXTRACTION_MODE = "ICRegionExMode";
    /* package */static final String KEY_IC_IMAGE_ENHANCED_MODE = "ICImageEnMode";

    /**
     * checks whether the camera instruction needs to be shown or not
     *
     * @param context
     *            Application context
     * @return true if needs to be shown, false otherwise
     */
    public static boolean isCameraInstruction(Context context) {
        return getBooleanPreference(context, KEY_CAMERA_INSTRUCTIONS, true);
    }

    /**
     * gets the camera move threshold parameter
     *
     * @param context
     *            Application context
     * @return camera move threshold parameter
     */
    public static float getCameraMoveThreshold(Context context) {
        return getFloatPreference(context, KEY_CAMERA_MOVE_THRESHOLD,
                R.dimen.default_camera_move_threshold, 0);
    }

    /**
     * gets the camera steady duration parameter
     *
     * @param context
     *            Application context
     * @return camera steady duration parameter
     */
    public static int getCameraSteadyDuration(Context context) {
        return getIntPreference(context, KEY_CAMERA_STEADY_DURATION,
                R.integer.default_camera_steady_duration, 0);
    }

    /**
     * gets the scve mode
     *
     * @param context
     *            Application context
     * @return scve mode
     */
    public static int getScveMode(Context context) {
        int defaultValue = Integer.parseInt(context.getResources().getString(
                R.string.settings_scve_mode_default));
        return getIntPreference(context, KEY_SCVE_MODE, 0, defaultValue);
    }

    /**
     * gets the IC region extraction mode
     *
     * @param context
     *            Application context
     * @return IC region extraction mode
     */
    public static int getICRegionExMode(Context context) {
        int defaultValue = Integer.parseInt(context.getResources().getString(
                R.string.setting_default_ic_region_ex_mode));
        return getIntPreference(context, KEY_IC_REGION_EXTRACTION_MODE, 0, defaultValue);
    }

    /**
     * gets the IC image enhanced mode
     *
     * @param context
     *            Application context
     * @return IC image enhanced mode
     */
    public static int getICImageEnMode(Context context) {
        int defaultValue = Integer.parseInt(context.getResources().getString(
                R.string.setting_default_ic_image_en_mode));
        return getIntPreference(context, KEY_IC_IMAGE_ENHANCED_MODE, 0, defaultValue);
    }

    /**
     * gets the ocr flags from the saved preferences
     *
     * @param context
     *            application context
     * @return ocr data type flags
     */
    public static int getOCRFlags(Context context) {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
        Set<String> flagValueTexts = preferences.getStringSet(KEY_STA_DATA_TYPES, null);
        int flag = 0;
        if (flagValueTexts != null) {
            for (String flagValueText : flagValueTexts) {
                flag |= Integer.parseInt(flagValueText);
            }
            if (flag > 0) {
                return flag;
            }
        }
        return JniConstants.DATA_TYPE_ALL;
    }

    /**
     * gets the ocr locale from the saved preferences
     *
     * @param context
     *            application context
     * @return ocr locale type
     */
    public static String getOCRLocale(Context context) {
        String defaultValue = context.getResources().getString(R.string.setting_default_sta_locale);
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
        return preferences.getString(KEY_STA_LOCALE_TYPE, defaultValue);
    }

    /**
     * gets the ocr text record mode from the saved preferences
     *
     * @param context
     *            application context
     * @return ocr text record mode
     */
    public static String getOCRTextRecordMode(Context context) {
        String defaultValue = context.getResources().getString(
                R.string.setting_default_text_record_mode);
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
        return preferences.getString(KEY_TEXT_RECORD_MODE, defaultValue);
    }

    /**
     * gets the ocr language from the saved preferences
     *
     * @param context
     *            application context
     * @return ocr language string
     */
    public static String getTextLanguage(Context context) {
        String defaultValue = context.getResources().getString(R.string.setting_default_sta_locale);
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
        String ocrLocale = preferences.getString(KEY_STA_LOCALE_TYPE, defaultValue);

        String usaLocale = context.getResources().getString(R.string.setting_sta_locale_usa);
        String chinaLocale = context.getResources().getString(R.string.setting_sta_locale_china);
        String koreaLocale = context.getResources().getString(R.string.setting_sta_locale_korea);

        if(ocrLocale.equals(usaLocale)) {
            return context.getResources().getString(R.string.setting_sta_language_usa);

        } else if(ocrLocale.equals(chinaLocale)){
            return context.getResources().getString(R.string.setting_sta_language_china);

        } else if(ocrLocale.equals(koreaLocale)){
            return context.getResources().getString(R.string.setting_sta_langauge_korea);
        }
        return null;
    }

    /**
     * Disables the camera instruction display
     *
     * @param context
     *            Application context
     */
    public static void disableCameraInstruction(Context context) {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
        Editor edit = preferences.edit();
        edit.putBoolean(KEY_CAMERA_INSTRUCTIONS, false);
        edit.apply();
    }

    private static boolean getBooleanPreference(Context context, String key, boolean defaultValue) {
        try {
            SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
            return preferences.getBoolean(key, defaultValue);

        } catch (ClassCastException exp) {
            Log.d(LOG_TAG, "Class cast exception while fetching: " + key);
        }
        return defaultValue;
    }

    private static float getFloatPreference(Context context, String key, int defaultResourceId,
            int defaultValue) {
        float defaultFloatValue = defaultValue;

        if (defaultResourceId > 0) {
            TypedValue typedValue = new TypedValue();
            context.getResources().getValue(defaultResourceId, typedValue, true);
            defaultFloatValue = typedValue.getFloat();
        }
        try {
            SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
            String val = preferences.getString(key, null);
            if (val != null) {
                return Float.parseFloat(val);
            }

        } catch (ClassCastException exp) {
            Log.d(LOG_TAG, "Class cast exception while fetching: " + exp.getMessage());
        } catch (NumberFormatException exp) {
            Log.d(LOG_TAG, "Number format exception while fetching: " + key);
        }
        return defaultFloatValue;
    }

    private static int getIntPreference(Context context, String key, int defaultResourceId,
            int defaultValue) {
        int defaultIntValue = (defaultResourceId > 0) ? context.getResources().getInteger(
                defaultResourceId) : defaultValue;
        try {
            SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);
            String val = preferences.getString(key, null);
            if (val != null) {
                return Integer.parseInt(val);
            }

        } catch (ClassCastException exp) {
            Log.d(LOG_TAG, "Class cast exception while fetching: " + exp.getMessage());
        } catch (NumberFormatException exp) {
            Log.d(LOG_TAG, "Number format exception while fetching: " + key);
        }
        return defaultIntValue;
    }
}
