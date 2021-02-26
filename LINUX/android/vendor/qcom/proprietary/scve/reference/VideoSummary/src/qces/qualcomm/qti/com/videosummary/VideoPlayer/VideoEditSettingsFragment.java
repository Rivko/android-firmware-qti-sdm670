/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPlayer;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;
import qces.qualcomm.qti.com.videosummary.settings.LengthDialogPreference;

/**
 * Created by mithun on 10/6/15.
 */

/**
 * This fragment shows general preferences only. It is used when the
 * activity is showing a two-pane settings UI.
 */
@TargetApi(Build.VERSION_CODES.HONEYCOMB)
public class VideoEditSettingsFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {


    TimelapseDialogPreference.OnPreferenceChangeListener mOnPreferenceChangeListener = new TimelapseDialogPreference.OnPreferenceChangeListener() {

        @Override
        public void onSharedPrefChanged(TimelapseDialogPreference pref) {

            onSharedPreferenceChanged(pref.getSharedPreferences(), getResources().getString(R.string.video_edit_pref_speed));
        }
    };

    LengthDialogPreference.OnPreferenceChangeListener mOnLengthPreferenceChangeListener = new LengthDialogPreference.OnPreferenceChangeListener() {
        @Override
        public void onSharedPrefChanged(LengthDialogPreference pref) {
            onSharedPreferenceChanged(pref.getSharedPreferences(), getResources().getString(R.string.video_edit_pref_length));
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        HighlightsModel highlightsItem = getActivity().getIntent().getParcelableExtra(Constants.EXTRA_HIGHLIGHTS_MODEL);
        setEditVideoPrefTheme(getActivity().getApplicationContext(), highlightsItem.getTheme());
        if (highlightsItem.summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
            LengthDialogPreference.setEditVideoPrefLength(getActivity().getApplicationContext(), highlightsItem.getLength());
            addPreferencesFromResource(R.xml.video_edit_length_pref_settings);
            LengthDialogPreference lengthDialogPreference = (LengthDialogPreference) getPreferenceManager().findPreference(getString(R.string.video_edit_pref_length));
            lengthDialogPreference.setSummary();
            lengthDialogPreference.setOnPreferenceChangeListener(mOnLengthPreferenceChangeListener);
        } else {
            TimelapseDialogPreference.setEditVideoPrefSpeed(getActivity().getApplicationContext(), highlightsItem.getSpeed());
            TimelapseDialogPreference.setEditVideoPrefEventDetectionEnabled(getActivity().getApplicationContext(), highlightsItem.getEventDetectionEnabled());
            TimelapseDialogPreference.setEditVideoPrefEventSpeed(getActivity().getApplicationContext(), highlightsItem.getEventSpeed());
            TimelapseDialogPreference.setEditVideoPrefMotionEventDetectionEnabled(getActivity().getApplicationContext(), highlightsItem.getMotionEventDetectionEnabled());
            TimelapseDialogPreference.setEditVideoPrefFaceEventDetectionEnabled(getActivity().getApplicationContext(), highlightsItem.getFaceEventDetectionEnabled());
            addPreferencesFromResource(R.xml.video_edit_speed_pref_settings);
            String summaryString = "Video " + highlightsItem.getSpeed() + "x";
            if (highlightsItem.getEventDetectionEnabled()) {
                summaryString = summaryString + ", Event " + highlightsItem.getEventSpeed() + "x";
            }
            TimelapseDialogPreference timelapseDialogPreference = (TimelapseDialogPreference) getPreferenceManager().findPreference(getString(R.string.video_edit_pref_speed));
            timelapseDialogPreference.setSummary(summaryString);
            timelapseDialogPreference.setOnPreferenceChangeListener(mOnPreferenceChangeListener);
        }

    }

    @Override
    public void onResume() {
        super.onResume();
        getPreferenceScreen().getSharedPreferences()
                .registerOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        getPreferenceScreen().getSharedPreferences()
                .unregisterOnSharedPreferenceChangeListener(this);
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {

        Context context = getActivity().getApplicationContext();
        VideoPlayerActivity activity = (VideoPlayerActivity) getActivity();
        if (key.equals(context.getString(R.string.video_edit_pref_length))) {
            //For length, we will come here only if its value changed unlike timelapse-speed
            activity.highlightsItem.speedOrLength = HighlightsModel.updateLength(activity.highlightsItem.speedOrLength, LengthDialogPreference.getEditVideoPrefLength(context));
            activity.onSharedPrefChangedBitMask |= VideoPlayerActivity.LENGTH_CHANGED;
        } else if (key.equals(context.getString(R.string.video_edit_pref_theme))) {
            //For theme, we will come here only if its value changed unlike timelapse-speed
            activity.highlightsItem.speedOrLength = HighlightsModel.updateTheme(activity.highlightsItem.speedOrLength, getEditVideoPrefTheme(context));
            activity.onSharedPrefChangedBitMask |= VideoPlayerActivity.THEME_CHANGED;
        } else if (key.equals(context.getString(R.string.video_edit_pref_speed))) {
            int oldTimelapseSpeedSettings = activity.highlightsItem.speedOrLength;
            int newTimelapseSpeedSettings = TimelapseDialogPreference.getTimelapseDialogPrefChangedSpeedOrLength(context);
            if (oldTimelapseSpeedSettings != newTimelapseSpeedSettings) {
                activity.highlightsItem.speedOrLength = newTimelapseSpeedSettings;
                activity.onSharedPrefChangedBitMask |= VideoPlayerActivity.SPEED_CHANGED;
            }
        }
    }

    public static int getEditVideoPrefTheme(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return Integer.parseInt(prefs.getString(context.getResources().getString(R.string.video_edit_pref_theme), context.getResources().getString(R.string.video_edit_pref_theme_default_value)));
    }

    public static void setEditVideoPrefTheme(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(context.getResources().getString(R.string.video_edit_pref_theme), Integer.toString(value));
        editor.commit();
    }

}


