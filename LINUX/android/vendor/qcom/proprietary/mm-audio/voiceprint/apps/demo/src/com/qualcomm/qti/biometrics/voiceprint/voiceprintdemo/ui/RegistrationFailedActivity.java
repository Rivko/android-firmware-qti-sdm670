/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.content.Intent;
import android.view.View;
import android.widget.Button;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.OnAudioStateChangeListener;

public class RegistrationFailedActivity extends VoicePrintActivity  {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.registration_failed);

        // Navigation back
        ActionBar actionBar = this.getActionBar();

        // Recolor the action bar (error)
        actionBar.setBackgroundDrawable(new ColorDrawable(getResources().getColor(R.color.error_color)));

        Button done = (Button) findViewById(R.id.done);
        done.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                onDone();
            }
        });
    }

    private void onDone() {
        startActivity(new Intent(this, MainActivity.class));
    }

}
