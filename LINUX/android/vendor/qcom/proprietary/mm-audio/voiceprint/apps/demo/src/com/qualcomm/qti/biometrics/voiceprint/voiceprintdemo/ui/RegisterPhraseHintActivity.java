/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.Fragment;
import android.content.Intent;
import android.os.Bundle;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;

import static com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R.string.register_phrase_hint;

public class RegisterPhraseHintActivity extends VoicePrintActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.fragment_with_footer);

        Button actionButton = (Button) findViewById(R.id.button);

        if (savedInstanceState == null) {
            getFragmentManager().beginTransaction()
                    .add(R.id.container, new HintFragment()).commit();
            actionButton.setText(getString(R.string.ready_to_record));
            actionButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                    nextStep();
                }
            });
        }

    }

    private void nextStep() {
        startActivity(new Intent(this, RegisterPhraseActivity.class));
    }


    public static class HintFragment extends Fragment {

        public HintFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.register_phrase_hint,
                    container, false);

            TextView text = (TextView) rootView.findViewById(R.id.command);
            text.setText(Html.fromHtml(String.format(getString(register_phrase_hint),
                    ((RegisterPhraseHintActivity) getActivity()).getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));
            return rootView;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.register_phrase, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks. The action bar will
        // automatically handle clicks on the Home/Up button,
        // parent activity specified in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_cancel) {
            startActivity(new Intent(this, MainActivity.class));
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
