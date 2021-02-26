/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.effects;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public class EffectsIntroFragmentActivity extends FragmentActivity {

    public static final String prefName = "com.qualcomm.qti.qces.imageedit.effects";
    public static final String dontShowAutomatically = "DontShowHelpAutoEffects";

    SharedPreferences prefs;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.dialog_effects);

        prefs = getSharedPreferences(prefName, Context.MODE_PRIVATE);
        Button closeButton = (Button)findViewById(R.id.close_button);

        closeButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                finish();
            }
        });

        CheckBox mCheckBox = (CheckBox)findViewById(R.id.close_checkbox);
        mCheckBox.setChecked(getPref());
        mCheckBox.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                savePref(isChecked);
            }
        });

    }

    private boolean getPref() {
        return prefs.getBoolean(dontShowAutomatically, true);// Default is true/checked
    }

    private void savePref(boolean isChecked) {
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(dontShowAutomatically, isChecked);
        editor.commit();
    }
}
