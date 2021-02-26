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

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;

import java.util.List;

public class SetKeyPhraseNameActivity extends VoicePrintActivity {

    private EditText phraseName;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.set_key_phrase_name);

        // Set edit text bottom color
        phraseName = (EditText) findViewById(R.id.key_phrase_name);
        applyEditTextStyle(phraseName, new KeyboardDoneListener() {
            @Override
            public void onDone() {
                onNext();
            }
        });

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
        phraseName.requestFocus();
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
        getMenuInflater().inflate(R.menu.set_key_phrase_name, menu);
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

        String name = phraseName.getText().toString();

        // Phrase name cannot be empty
        if (name == null || name.isEmpty()) {
            Log.d(TAG, "Invalid name rejected.");
            showDialog(this, R.string.invalid_phrasename_title,
                    R.string.invalid_phrasename_msg,
                    R.string.cap_ok, null);

            return;
        }

        // Phrase name should be unique.
        List<KeyPhrase> list = VoicePrintDatabase.getInstance().getKeyPhrases();
        for (KeyPhrase kp : list) {
            if (kp.getName().equals(name)) {
                showDialog(this, R.string.invalid_phrasename_title,
                        R.string.duplicate_phrasename_msg,
                        R.string.cap_ok, null);
                return;
            }
        }

        Log.d(TAG, "Keyphrase Name=" + name);

        getVoicePrintApp().setKeyPhraseName(name);
        startActivity(new Intent(this, SetKeyPhraseActionActivity.class));
    }
}
