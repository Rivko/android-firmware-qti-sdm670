/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;

public class RegistrationCompleteActivity extends VoicePrintActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.fragment_with_footer);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(false);

        Button actionButton = (Button) findViewById(R.id.button);

        if (savedInstanceState == null) {
            getFragmentManager().beginTransaction()
                    .add(R.id.container, new HintFragment()).commit();
            actionButton.setText(getString(R.string.cap_done));
            actionButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                    onDone();
                }
            });
        }

    }

    private void onDone() {
        startActivity(new Intent(this, MainActivity.class));
    }


    public static class HintFragment extends Fragment {

        public HintFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.register_phrase_complete,
                    container, false);

            TextView text = (TextView) rootView.findViewById(R.id.command);
            text.setText(Html.fromHtml(String.format(getString(R.string.phrase_registered),
                    ((RegistrationCompleteActivity) getActivity()).getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));
            return rootView;
        }
    }

}
