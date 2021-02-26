/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;

public class VoicePrintNotAvailable extends VoicePrintActivity  {

    private MicFragment micFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.voiceprint_not_available);


        // Navigation back
        ActionBar actionBar = this.getActionBar();
        //actionBar.setDisplayHomeAsUpEnabled(true);

        // Recolor the action bar (error)
        actionBar.setBackgroundDrawable(new ColorDrawable(getResources().getColor(R.color.error_color)));

        final Intent intent = this.getIntent();
        Button done = (Button) findViewById(R.id.done);
        done.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setResult(Consts.RESULT_CLOSE_ALL, intent);
                finish();
            }
        });
    }


    @Override
    public void onResume() {
        super.onResume();

    }
}
