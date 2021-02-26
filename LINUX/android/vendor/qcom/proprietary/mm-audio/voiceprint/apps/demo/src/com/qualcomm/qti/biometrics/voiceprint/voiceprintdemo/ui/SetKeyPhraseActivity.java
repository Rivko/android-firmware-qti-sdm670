/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;

public class SetKeyPhraseActivity extends VoicePrintActivity {

    private ProgressBar phraseStrength;
    private EditText phrase;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.set_key_phrase);

        // Set edit text bottom color
        phrase = (EditText) findViewById(R.id.text1);
        applyEditTextStyle(phrase, new KeyboardDoneListener() {
            @Override
            public void onDone() {
                onNext();
            }
        });

        phraseStrength = (ProgressBar) findViewById(R.id.phraseStrength);

        Button next = (Button) findViewById(R.id.next);
        next.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onNext();
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();

        // Bring up keyboard
        phrase.requestFocus();
        forceShowKeyboard();
    }

    @Override
    public void onPause() {
        hideKeyboard();
        super.onPause();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.set_key_phrase, menu);
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


    private void onNext() {
        String name = phrase.getText().toString();
        if (name == null || name.isEmpty()) {
            Log.d(TAG, "Invalid phrase rejected.");
            showDialog(this, R.string.invalid_phrase_title,
                    R.string.invalid_phrase_msg,
                    R.string.cap_ok, null);

            return;
        }

        Log.d(TAG, "Old Keyphrase=" + getVoicePrintApp().getCurrentKeyPhrase().getPhrase());
        Log.d(TAG, "New Keyphrase=" + name);
        getVoicePrintApp().setKeyPhrase(name);
        startActivity(new Intent(this, SetKeyPhraseNameActivity.class));
    }
}
