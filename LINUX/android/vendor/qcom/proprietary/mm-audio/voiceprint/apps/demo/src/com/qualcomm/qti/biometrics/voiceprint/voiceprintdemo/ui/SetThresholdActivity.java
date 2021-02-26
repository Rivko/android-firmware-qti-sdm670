/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;
import com.qualcomm.qti.biometrics.voiceprint.sdk.VoicePrintManager;

import java.sql.SQLException;

public class SetThresholdActivity extends VoicePrintActivity implements CompoundButton.OnCheckedChangeListener {

    private RadioButton low;
    private RadioButton med;
    private RadioButton high;
    private TextView title;
    private TextView summary;
    private int mSavedThreshold;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.set_threshold);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        Button accept = (Button) findViewById(R.id.accept);
        accept.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onAccept();
            }
        });

        Button cancel = (Button) findViewById(R.id.cancel);
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onCancel();
            }
        });


        // Radio buttons
        low = (RadioButton) findViewById(R.id.low);
        med = (RadioButton) findViewById(R.id.med);
        high = (RadioButton) findViewById(R.id.high);

        low.setOnCheckedChangeListener(this);
        med.setOnCheckedChangeListener(this);
        high.setOnCheckedChangeListener(this);

        title = (TextView) findViewById(R.id.sensitivity_title);
        summary = (TextView) findViewById(R.id.sensitivity_summary);
        mSavedThreshold = getVoicePrintApp().getCurrentKeyPhrase().getThreshold();

        switch (mSavedThreshold) {
        case VoicePrintManager.THRESHOLD_LOW:
            low.setChecked(true);
            break;
        case VoicePrintManager.THRESHOLD_MED:
            med.setChecked(true);
            break;
        case VoicePrintManager.THRESHOLD_HIGH:
            high.setChecked(true);
            break;
        default:
            break;
        }
    }


    private void onCancel() {
        startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
    }

    private void onAccept() {

        try {
            if ( mSavedThreshold != getVoicePrintApp().getCurrentKeyPhrase().getThreshold()) {
                commitThreshold(mSavedThreshold);
            }
            startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    @Override
    public void onResume() {
        super.onResume();

    }

    @Override
    public void onPause() {

        super.onPause();
    }


    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        // Only handle valid isChecked cases.
        if (buttonView == null || isChecked == false) {
            return;
        }

        // Manually unset other radio buttons.
        // Don't want to use radio group since it's a linear layout.

        switch (buttonView.getId()) {
        case R.id.low:
            title.setText(getString(R.string.low_sensitivity));
            summary.setText(getString(R.string.low_sensitivity_summary));
            med.setChecked(false);
            high.setChecked(false);
            mSavedThreshold = VoicePrintManager.THRESHOLD_LOW;
            break;
        case R.id.med:
            title.setText(getString(R.string.med_sensitivity));
            summary.setText(getString(R.string.med_sensitivity_summary));
            low.setChecked(false);
            high.setChecked(false);
            mSavedThreshold = VoicePrintManager.THRESHOLD_MED;
            break;
        case R.id.high:
            title.setText(getString(R.string.high_sensitivity));
            summary.setText(getString(R.string.high_sensitivity_summary));
            low.setChecked(false);
            med.setChecked(false);
            mSavedThreshold = VoicePrintManager.THRESHOLD_HIGH;
            break;
        }
    }

    private void commitThreshold(int threshold) {
        try {
            // TODO error management
            mManager.setThreshold(getVoicePrintApp().getCurrentKeyPhrase().getId(),
                                  threshold);

            getVoicePrintApp().getCurrentKeyPhrase().setThreshold(threshold);

            VoicePrintDatabase.getInstance().setThreshold(
                getVoicePrintApp().getCurrentKeyPhrase().getId(),
                getVoicePrintApp().getCurrentKeyPhrase().getThreshold());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
