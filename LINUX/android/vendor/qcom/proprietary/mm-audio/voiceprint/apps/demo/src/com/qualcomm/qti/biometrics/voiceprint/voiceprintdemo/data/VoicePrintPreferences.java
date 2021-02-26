/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data;


import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

public class VoicePrintPreferences {

    private static final String PREFERENCE_NAME = "VoicePrintData";
    private static final String KEYPHRASES = "KEYPHRASES";
    private static final String LAST_KEYPHRASE_SELECTED = "LAST_KEYPHRASE_SELECTED";
    private static String DELIMITER = "=";
    private static final String TAG = "VoicePrintPreferences";
    private static final String SENTENCE_SCORE = "SENTENCE_SCORE";
    private static final String USER_SCORE = "USER_SCORE";
    private static final String SPOOF_SCORE = "SPOOF_SCORE";
    //private static final float DEFAULT_SENTENCE_SCORE = -0.6f;
    private static final float DEFAULT_SENTENCE_SCORE = -99999999f;
    private static final float DEFAULT_USER_SCORE = -2.0f;
    private static final float DEFAULT_SPOOF_SCORE = 0.2f;

    public static String getLastSelectedKeyPhrase(Context context) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        return settings.getString(LAST_KEYPHRASE_SELECTED, null);
    }

    public static void saveLastSelectedKeyPhrase(Context context, String id) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(LAST_KEYPHRASE_SELECTED, id).apply();
    }

    public static float getSentenceScore(Context context) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        float value = settings.getFloat(SENTENCE_SCORE, DEFAULT_SENTENCE_SCORE);
        return value;
    }

    public static void saveSentenceScore(Context context, float value) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(SENTENCE_SCORE, value).apply();
    }

    public static float getUserScore(Context context) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        float value = settings.getFloat(USER_SCORE, DEFAULT_USER_SCORE);
        return value;
    }

    public static void saveUserScore(Context context, float value) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(USER_SCORE, value).apply();
    }

    public static float getSpoofScore(Context context) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        float value = settings.getFloat(SPOOF_SCORE, DEFAULT_SPOOF_SCORE);
        return value;
    }

    public static void saveSpoofScore(Context context, float value) {
        SharedPreferences settings = PreferenceManager
                .getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(SPOOF_SCORE, value).apply();
    }
}
