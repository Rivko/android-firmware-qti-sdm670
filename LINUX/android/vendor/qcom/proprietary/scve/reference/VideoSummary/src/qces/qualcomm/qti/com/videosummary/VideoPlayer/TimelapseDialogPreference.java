/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPlayer;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.DialogPreference;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;

/**
 * Created by matthewf on 10/12/15.
 */
public class TimelapseDialogPreference extends DialogPreference {


    Context context;

    private SeekBar speedSeekBar;
    private SeekBar eventSpeedSeekBar;
    private Switch eventDetectionSwitch;
    private CheckBox motionDetectCheckbox;
    private CheckBox facesDetectCheckbox;
    private TextView eventSpeedTextView;
    private TextView detectEventTextView;
    private LinearLayout seekBarEventTicks;
    private OnPreferenceChangeListener mOnPreferenceChangeListener;

    public interface OnPreferenceChangeListener {
        /**
         * @param pref
         */
        public void onSharedPrefChanged(TimelapseDialogPreference pref);
    }

    public void setOnPreferenceChangeListener(OnPreferenceChangeListener listener) {
        mOnPreferenceChangeListener = listener;
    }

    public TimelapseDialogPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
    }

    public static int getEditVideoPrefSpeed(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getInt(context.getResources().getString(R.string.video_edit_pref_speed), context.getResources().getInteger(R.integer.pref_speed_default_val));
    }

    public static void setEditVideoPrefSpeed(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putInt(context.getResources().getString(R.string.video_edit_pref_speed), value);
        editor.commit();
    }

    public static int getEditVideoPrefEventSpeed(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getInt(context.getResources().getString(R.string.video_edit_pref_event_speed), context.getResources().getInteger(R.integer.pref_event_speed_default_val));
    }

    public static void setEditVideoPrefEventSpeed(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putInt(context.getResources().getString(R.string.video_edit_pref_event_speed), value);
        editor.commit();
    }

    public static boolean getEditVideoPrefEventDetectionEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.video_edit_pref_event_detection), context.getResources().getBoolean(R.bool.pref_event_detection_enabled_default_val));
    }

    public static void setEditVideoPrefEventDetectionEnabled(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.video_edit_pref_event_detection), value);
        editor.commit();
    }

    public static boolean getEditVideoPrefMotionEventDetectionEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.video_edit_pref_motion_event_detection), context.getResources().getBoolean(R.bool.pref_motion_event_detection_enabled_default_val));
    }

    public static void setEditVideoPrefMotionEventDetectionEnabled(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.video_edit_pref_motion_event_detection), value);
        editor.commit();
    }

    public static boolean getEditVideoPrefFaceEventDetectionEnabled(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return prefs.getBoolean(context.getResources().getString(R.string.video_edit_pref_face_event_detection), context.getResources().getBoolean(R.bool.pref_face_event_detection_enabled_default_val));
    }

    public static void setEditVideoPrefFaceEventDetectionEnabled(Context context, boolean value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(context.getResources().getString(R.string.video_edit_pref_face_event_detection), value);
        editor.commit();
    }

    @Override
    protected View onCreateDialogView() {
        return super.onCreateDialogView();
    }

    @Override
    protected void onBindDialogView(View view) {
        super.onBindDialogView(view);

        seekBarEventTicks = (LinearLayout) view.findViewById(R.id.seekBarEventTicks);

        speedSeekBar = (SeekBar) view.findViewById(R.id.seekBarSpeed);
        eventSpeedSeekBar = (SeekBar) view.findViewById(R.id.seekBarEventSpeed);
        eventDetectionSwitch = (Switch) view.findViewById(R.id.event_detect_switch);
        motionDetectCheckbox = (CheckBox) view.findViewById(R.id.motionCheckbox);
        facesDetectCheckbox = (CheckBox) view.findViewById(R.id.facesCheckbox);

        eventSpeedTextView = (TextView) view.findViewById(R.id.event_speed_textview);
        detectEventTextView = (TextView) view.findViewById(R.id.detect_events_textbox);

        int videoSpeed = getEditVideoPrefSpeed(context);
        int videoEventSpeed = getEditVideoPrefEventSpeed(context);

        speedSeekBar.setProgress(getTickFromSpeed(videoSpeed));
        eventSpeedSeekBar.setProgress(getTickFromSpeed(videoEventSpeed));

        boolean eventDetectionEnabled = getEditVideoPrefEventDetectionEnabled(context);
        changeEventDetectoinEnableUI(eventDetectionEnabled);

        eventDetectionSwitch.setChecked(eventDetectionEnabled);
        motionDetectCheckbox.setChecked(getEditVideoPrefMotionEventDetectionEnabled(context));
        facesDetectCheckbox.setChecked(getEditVideoPrefFaceEventDetectionEnabled(context));

        eventDetectionSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                changeEventDetectoinEnableUI(isChecked);
            }
        });

    }

    private void changeEventDetectoinEnableUI(boolean enableEvents) {

        motionDetectCheckbox.setEnabled(enableEvents);
        facesDetectCheckbox.setEnabled(enableEvents);
        eventSpeedSeekBar.setEnabled(enableEvents);

        float textAlpha = 1.0F;
        if (!enableEvents) {
            textAlpha = 0.3F;
            eventSpeedSeekBar.setThumb(getContext().getDrawable(R.drawable.speed_scrubber_disabled));
            eventSpeedSeekBar.setProgressDrawable(getContext().getDrawable(R.drawable.speed_progress_disabled));
        } else {
            eventSpeedSeekBar.setThumb(getContext().getDrawable(R.drawable.speed_scrubber));
            eventSpeedSeekBar.setProgressDrawable(getContext().getDrawable(R.drawable.speed_progress));
        }

        seekBarEventTicks.setAlpha(textAlpha);
        eventSpeedTextView.setAlpha(textAlpha);
        detectEventTextView.setAlpha(textAlpha);

    }

    private int getSpeedFromTick(int tick) {
        return (int) Math.pow(2, tick);
    }

    private int getTickFromSpeed(int speed) {
        int retVal = 0;
        switch (speed) {
            case 1:
                retVal = 0;
                break;
            case 2:
                retVal = 1;
                break;
            case 4:
                retVal = 2;
                break;
            case 8:
                retVal = 3;
                break;
            case 16:
                retVal = 4;
                break;

        }
        return retVal;
    }

    void onSharedPreferenceChanged() {
        mOnPreferenceChangeListener.onSharedPrefChanged(this);
    }

    public static int getTimelapseDialogPrefChangedSpeedOrLength(Context context) {
        int speed = getEditVideoPrefSpeed(context);
        boolean eventDetectEnabled = getEditVideoPrefEventDetectionEnabled(context);
        int eventSpeed = getEditVideoPrefEventSpeed(context);
        boolean motionEventDetectEnabled = getEditVideoPrefMotionEventDetectionEnabled(context);
        boolean faceEventDetectEnabled = getEditVideoPrefFaceEventDetectionEnabled(context);
        int newSpeedOrLength = HighlightsModel.getSpeedOrLength(speed, eventDetectEnabled, eventSpeed, motionEventDetectEnabled, faceEventDetectEnabled);
        return newSpeedOrLength;
    }

    void updatePrefs() {
        int speed = getSpeedFromTick(speedSeekBar.getProgress());
        int event_speed = getSpeedFromTick(eventSpeedSeekBar.getProgress());

        boolean event_detect = eventDetectionSwitch.isChecked();
        if (event_detect)
            super.setSummary("Video " + speed + "x, Events " + event_speed + "x");
        else
            super.setSummary("Video " + speed + "x");

        setEditVideoPrefSpeed(context, speed);
        setEditVideoPrefEventSpeed(context, event_speed);
        setEditVideoPrefEventDetectionEnabled(context, event_detect);
        setEditVideoPrefMotionEventDetectionEnabled(context, motionDetectCheckbox.isChecked());
        setEditVideoPrefFaceEventDetectionEnabled(context, facesDetectCheckbox.isChecked());
    }

    @Override
    protected void onDialogClosed(boolean positiveResult) {

        if (positiveResult) {
            updatePrefs();
            onSharedPreferenceChanged();
        }
        super.onDialogClosed(positiveResult);

    }

}
