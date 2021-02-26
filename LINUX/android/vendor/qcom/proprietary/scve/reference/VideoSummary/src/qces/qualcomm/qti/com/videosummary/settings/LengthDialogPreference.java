/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.settings;

import android.app.AlertDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.preference.DialogPreference;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.view.View;
import android.widget.RadioGroup;

import java.util.Arrays;

import qces.qualcomm.qti.com.videosummary.R;

/**
 * Created by mithun on 10/28/15.
 */
public class LengthDialogPreference extends DialogPreference {
    private Context context;
    private OnPreferenceChangeListener mOnPreferenceChangeListener;
    String[] lengthArray = getContext().getResources().getStringArray(R.array.pref_default_length_values);

    public interface OnPreferenceChangeListener {
        /**
         * @param pref
         */
        public void onSharedPrefChanged(LengthDialogPreference pref);
    }

    public void setOnPreferenceChangeListener(OnPreferenceChangeListener listener) {
        mOnPreferenceChangeListener = listener;
    }

    public LengthDialogPreference(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context;
    }

    @Override
    protected void onPrepareDialogBuilder(AlertDialog.Builder builder) {
        builder.setTitle(R.string.length_settings_title);
        builder.setPositiveButton(null, null);
        super.onPrepareDialogBuilder(builder);
    }

    void onSharedPreferenceChanged() {
        mOnPreferenceChangeListener.onSharedPrefChanged(this);
    }

    @Override
    protected void onBindDialogView(View view) {
        //TODO: get access to ur view elements
        super.onBindDialogView(view);

        RadioGroup radioGroup = (RadioGroup) view.findViewById(R.id.radio_group_video_length);

        //initialize i.e select one of the radio button
        int defaultLenthValueIndex = Arrays.asList(lengthArray).indexOf(Integer.toString(getEditVideoPrefLength(context)));
        radioGroup.check(radioGroupIndexToId(defaultLenthValueIndex));

        radioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                int index = radioGroupIdToIndex(checkedId);
                int resultVal = Integer.parseInt(lengthArray[index]);
                setEditVideoPrefLength(context, resultVal);
                setSummary();
                onSharedPreferenceChanged();
                getDialog().cancel();

            }
        });
    }

    int radioGroupIndexToId(int index) {
        int id = -1;
        switch (index) {
            case 0:
                id = R.id.length_short;
                break;
            case 1:
                id = R.id.length_long;
                break;
            case 2:
                id = R.id.length_six_sec;
                break;
            case 3:
                id = R.id.length_fifteen_sec;
                break;
            case 4:
                id = R.id.length_thirty_sec;
                break;
            default:
                throw new ArrayIndexOutOfBoundsException("unknown index " + index + " to find the radio button id");
        }
        return id;
    }

    int radioGroupIdToIndex(int id) {
        int index = -1;
        switch (id) {
            case R.id.length_short:
                index = 0;
                break;
            case R.id.length_long:
                index = 1;
                break;
            case R.id.length_six_sec:
                index = 2;
                break;
            case R.id.length_fifteen_sec:
                index = 3;
                break;
            case R.id.length_thirty_sec:
                index = 4;
                break;
        }
        if (index < 0 || index > lengthArray.length) {
            throw new ArrayIndexOutOfBoundsException("length array index " + index + " is out of bounds, should be >=0 && < arraylength (" + lengthArray.length + ")");
        }
        return index;
    }

    public void setSummary() {
        int defaultLenthValueIndex = Arrays.asList(lengthArray).indexOf(Integer.toString(getEditVideoPrefLength(context)));
        String summaryString = context.getResources().getStringArray(R.array.pref_default_length_titles)[defaultLenthValueIndex];
        setSummary(summaryString);
    }

    public static int getEditVideoPrefLength(Context context) {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
        return Integer.parseInt(prefs.getString(context.getResources().getString(R.string.video_edit_pref_length), context.getResources().getString(R.string.default_length_default_val)));
    }

    public static void setEditVideoPrefLength(Context context, int value) {
        SharedPreferences pref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = pref.edit();
        editor.putString(context.getResources().getString(R.string.video_edit_pref_length), Integer.toString(value));
        editor.commit();
    }

}
