/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

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

public class CloneIntroFragmentActivity extends FragmentActivity {

    public static final String prefName = "com.qualcomm.qti.qces.objectclone.CloneIntroFragmentActivity";
    public static final String dontShowAutomatically = "DontShowHelpAutoClone";

    SharedPreferences prefs;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.dialog_clone);

        prefs = getSharedPreferences(prefName, Context.MODE_PRIVATE);
        Button closeButton = (Button)findViewById(R.id.close_button);

        closeButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                finish();
            }
        });

        CheckBox cloneWalkthroughCheckBox = (CheckBox)findViewById(R.id.close_checkbox);
        cloneWalkthroughCheckBox.setChecked(getPref());
        cloneWalkthroughCheckBox.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                savePref(isChecked);
            }
        });

    }

    @Override
    protected void onStop() {
        super.onStop();
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
