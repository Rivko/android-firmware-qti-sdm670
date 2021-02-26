/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.settings;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.preference.MultiSelectListPreference;
import android.preference.Preference;
import android.preference.PreferenceFragment;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;

import java.util.Set;

import qces.qualcomm.qti.com.videosummary.PolicyManager.FrequencyScheduleAlarmReceiver;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.VideoPlayer.TimelapseDialogPreference;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;


public class SettingsActivity extends AppCompatActivity {

    private static final String TAG = SettingsActivity.class.getSimpleName();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.settings_layout);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getFragmentManager().beginTransaction().replace(R.id.content_frame, new SettingsFragment()).commit();
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
    }

    /**
     * This fragment shows general preferences only. It is used when the
     * activity is showing a two-pane settings UI.
     */
    @TargetApi(Build.VERSION_CODES.HONEYCOMB)
    public static class SettingsFragment extends PreferenceFragment implements SharedPreferences.OnSharedPreferenceChangeListener {
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
            addPreferencesFromResource(R.xml.pref_settings);
            initLengthSettings();
            initTimelapseMultiSpeedSettings();
        }

        private void initLengthSettings() {
            Context context = getActivity().getApplicationContext();
            LengthDialogPreference.setEditVideoPrefLength(context, getPrefDefaultLength(context));
            LengthDialogPreference lengthDialogPreference = (LengthDialogPreference) getPreferenceManager().findPreference(getString(R.string.video_edit_pref_length));
            lengthDialogPreference.setSummary();
            lengthDialogPreference.setOnPreferenceChangeListener(mOnLengthPreferenceChangeListener);
        }

        private void initTimelapseMultiSpeedSettings() {
            Context context = getActivity().getApplicationContext();
            int speed = getPrefSpeed(context);
            boolean eventDetectEnabled = getPrefEventDetectionEnabled(context);
            int eventSpeed = getPrefEventSpeed(context);

            TimelapseDialogPreference.setEditVideoPrefSpeed(context, speed);
            TimelapseDialogPreference.setEditVideoPrefEventDetectionEnabled(context, eventDetectEnabled);
            TimelapseDialogPreference.setEditVideoPrefEventSpeed(context, eventSpeed);
            TimelapseDialogPreference.setEditVideoPrefMotionEventDetectionEnabled(context, getPrefMotionEventDetectionEnabled(context));
            TimelapseDialogPreference.setEditVideoPrefFaceEventDetectionEnabled(context, getPrefFaceEventDetectionEnabled(context));

            String summaryString = "Video " + speed + "x";
            if (eventDetectEnabled) {
                summaryString = summaryString + ", Event " + eventSpeed + "x";
            }
            TimelapseDialogPreference timelapseDialogPreference = (TimelapseDialogPreference) getPreferenceManager().findPreference(getString(R.string.video_edit_pref_speed));
            timelapseDialogPreference.setSummary(summaryString);
            timelapseDialogPreference.setOnPreferenceChangeListener(mOnPreferenceChangeListener);
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
            Log.i(TAG, "Key = " + key);
            if (key.equals(context.getString(R.string.pref_create_highlight_videos))
                    || key.equals(context.getString(R.string.pref_create_timelapse_videos))) {
                //irrespective, the auto create highlights/timelapse key is on or off
                // call setupAlarmForSelectedFrequency, which will take care for daily scoring
                // and/or auto highlights/timelapse
                FrequencyScheduleAlarmReceiver.setupAlarmForSelectedFrequency(context);
            } else if (key.equals(context.getString(R.string.pref_frequency)) &&
                    getPrefCreateHighlightVideos(context)) {
                Preference preference = findPreference(key);
                updateMultiSelectListPreferenceSummary(context, preference);
                FrequencyScheduleAlarmReceiver.setupAlarmForSelectedFrequency(context);
            } else if (key.equals(context.getString(R.string.pref_watch_dog))) {
                if (getPrefWatchDogEnabled(context)) {
                    Watchdog.enableDog();
                } else {
                    Watchdog.disableDog();
                }
            } else if (key.equals(context.getString(R.string.video_edit_pref_speed))) {
                setPrefSpeed(context, TimelapseDialogPreference.getEditVideoPrefSpeed(context));
                setPrefEventSpeed(context, TimelapseDialogPreference.getEditVideoPrefEventSpeed(context));
                setPrefEventDetectionEnabled(context, TimelapseDialogPreference.getEditVideoPrefEventDetectionEnabled(context));
                setPrefMotionEventDetectionEnabled(context, TimelapseDialogPreference.getEditVideoPrefMotionEventDetectionEnabled(context));
                setPrefFaceEventDetectionEnabled(context, TimelapseDialogPreference.getEditVideoPrefFaceEventDetectionEnabled(context));
            } else if (key.equals(context.getString(R.string.video_edit_pref_length))) {
                setPrefDefaultLength(context, LengthDialogPreference.getEditVideoPrefLength(context));
            }
        }
    }


    static void updateMultiSelectListPreferenceSummary(Context context, Preference preference) {

        MultiSelectListPreference mpref = (MultiSelectListPreference) preference;
        Set<String> selectedFreq = getPrefFrequency(context);
        StringBuilder frequencySummary = new StringBuilder();
        if (selectedFreq.contains("Daily")) {
            frequencySummary.append("Daily");
        }
        if (selectedFreq.contains("Weekly")) {
            if (frequencySummary.length() > 0) {
                frequencySummary.append(", ");
            }
            frequencySummary.append("Weekly");
        }
        if (selectedFreq.contains("Monthly")) {

            if (frequencySummary.length() > 0) {
                frequencySummary.append(", ");
            }
            frequencySummary.append("Monthly");
        }

        if (frequencySummary.length() == 0) {
            frequencySummary.append("None");
        }
        mpref.setSummary(frequencySummary);
    }

    /**
     * Returns whether to create Highlight Video or not from Settings i.e from shared prefs
     *
     * @param context
     * @return TRUE if enabled, FALSE otherwise
     */
    public static boolean getPrefCreateHighlightVideos(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_create_highlight_videos), context.getResources().getBoolean(R.bool.create_highlight_videos_default_val));
    }

    /**
     * setPrefCreateHighlightVideos will update the pref switch to the bool value passed
     *
     * @param context
     * @param value
     */
    public static void setPrefCreateHighlightVideos(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_create_highlight_videos), value);
        editor.commit();
    }

    /**
     * Returns the frequency value set in Settings i.e from shared prefs
     *
     * @param context
     * @return prefsValue, can have one of these values "Daily", "weekly", "Monthly"
     */
    public static Set<String> getPrefFrequency(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getStringSet(context.getResources().getString(R.string.pref_frequency),
                null);
    }

    /**
     * Returns whether to send notification about the highlight video created or not from Settings
     * i.e from shared prefs
     *
     * @param context
     * @return TRUE if enabled, FALSE otherwise
     */
    public static boolean getPrefSendHighlightsNotifications(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_highlights_send_notification_on_completion), context.getResources().getBoolean(R.bool.highlights_send_notification_default_val));
    }

    /**
     * Returns the default length of video to be created based on value in Settings i.e from shared
     * prefs
     *
     * @param context
     * @return video length, can have one of these values 1 (long/low compression), 2(short/high compression), 3(medium), 6 seconds (Vine), 15 seconds (Instagram), 30 seconds (Twitter)
     */
    public static int getPrefDefaultLength(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return Integer.parseInt(prefs.getString(context.getResources().getString(R.string.pref_default_length), context.getResources().getString(R.string.default_length_default_val)));
    }

    public static void setPrefDefaultLength(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(context.getResources().getString(R.string.pref_default_length), Integer.toString(value));
        editor.commit();
    }

    /**
     * Returns whether to create Timelapse Video or not from Settings i.e from shared
     * prefs
     *
     * @param context
     * @return TRUE if enabled, FALSE otherwise
     */
    public static boolean getPrefCreateTimelapseVideos(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_create_timelapse_videos), context.getResources().getBoolean(R.bool.create_timelapse_videos_default_val));
    }

    /**
     * setPrefCreateHighlightVideos will update the pref switch to the bool value passed
     *
     * @param context
     * @param value
     */
    public static void setPrefCreateTimelapseVideos(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_create_timelapse_videos), value);
        editor.commit();
    }

    /**
     * Returns the default speed of video based on value in Settings i.e from shared
     * prefs
     *
     * @param context
     * @return video playback speed, can have one of these values 1, 2, 4, 8, 16
     */
    public static int getPrefSpeed(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getInt(context.getResources().getString(R.string.pref_speed), context.getResources().getInteger(R.integer.pref_speed_default_val));
    }

    public static void setPrefSpeed(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putInt(context.getResources().getString(R.string.pref_speed), value);
        editor.commit();
    }

    public static int getPrefEventSpeed(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getInt(context.getResources().getString(R.string.pref_event_speed), context.getResources().getInteger(R.integer.pref_event_speed_default_val));
    }

    public static void setPrefEventSpeed(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putInt(context.getResources().getString(R.string.pref_event_speed), value);
        editor.commit();
    }

    public static boolean getPrefEventDetectionEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_event_detection_enabled), context.getResources().getBoolean(R.bool.pref_event_detection_enabled_default_val));
    }

    public static void setPrefEventDetectionEnabled(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_event_detection_enabled), value);
        editor.commit();
    }

    public static boolean getPrefMotionEventDetectionEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_motion_event_detection_enabled), context.getResources().getBoolean(R.bool.pref_motion_event_detection_enabled_default_val));
    }

    public static void setPrefMotionEventDetectionEnabled(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_motion_event_detection_enabled), value);
        editor.commit();
    }

    public static boolean getPrefFaceEventDetectionEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_face_event_detection_enabled), context.getResources().getBoolean(R.bool.pref_face_event_detection_enabled_default_val));
    }

    public static void setPrefFaceEventDetectionEnabled(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_face_event_detection_enabled), value);
        editor.commit();
    }

    /**
     * Returns whether to send notification about the timelapse video created or not from Settings
     * i.e from shared prefs
     *
     * @param context
     * @return TRUE if enabled, FALSE otherwise
     */
    public static boolean getPrefSendTimelapseNotifications(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_timelapse_send_notification_on_completion), context.getResources().getBoolean(R.bool.timelapse_send_notification_default_val));
    }

    /**
     * Debug options, for SCVE test mode.
     */
    public static int getPrefSCVETestMode(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return Integer.parseInt(prefs.getString(context.getResources().getString(R.string.scve_test_mode), "0"));
    }

    /**
     * Debug options
     */
    public static boolean getPrefExpandScrollView(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.expand_scroll_view), context
                .getResources().getBoolean(R.bool.expand_scroll_view_default_val));
    }

    /**
     * Debug options
     */
    public static boolean getPrefWatchDogEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_watch_dog), context
                .getResources().getBoolean(R.bool.pref_watch_dog_enabled_default_val));
    }

    /**
     * assemblyTableUpdateTimestamp pref is used to keep track of timestamp upto which the videos
     * were already summarized, this will help in scenarios when a user had turned off phone or
     * didnt connect charger for few days and summary needs to run for all those pending days
     *
     * @param context
     * @param timestampMillis
     */
    public static void setAssemblyTimelineTableUpdateTimestamp(Context context, long timestampMillis) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putLong("assemblyTableUpdateTimestamp", timestampMillis);
        editor.commit();
    }

    public static long getAssemblyTimelineTableUpdateTimestamp(Context context) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        return pref.getLong("assemblyTableUpdateTimestamp", 0);
    }


    /**
     * getPrefShowedOneTimeSplash will return True is splash screen is already showed once
     * otherwise false (default value is false)
     *
     * @param context
     */
    public static boolean getPrefShowedOneTimeSplash(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.pref_showed_one_time_splash), context
                .getResources().getBoolean(R.bool.showed_one_time_splash_default_val));
    }


    /**
     * setPrefShowedOneTimeSplash will update the onetimesplash pref flag to the bool value passed
     *
     * @param context
     * @param value
     */
    public static void setPrefShowedOneTimeSplash(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.pref_showed_one_time_splash), value);
        editor.commit();
    }

}
