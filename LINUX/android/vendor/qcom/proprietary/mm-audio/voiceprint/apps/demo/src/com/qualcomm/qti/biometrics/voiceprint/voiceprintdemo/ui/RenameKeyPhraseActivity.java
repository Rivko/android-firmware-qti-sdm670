/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;

import java.sql.SQLException;

public class RenameKeyPhraseActivity extends VoicePrintActivity {

    private EditText name;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.set_key_phrase_name);

        // Change the text name
        TextView textView = (TextView) findViewById(R.id.key_phrase_name_text);
        textView.setText(R.string.new_key_phrase_name);

        // Set edit text bottom color
        name = (EditText) findViewById(R.id.key_phrase_name);
        applyEditTextStyle(name, new KeyboardDoneListener() {
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
        name.requestFocus();
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
        getMenuInflater().inflate(R.menu.rename_key_phrase, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks. The action bar will
        // automatically handle clicks on the Home/Up button,
        // parent activity specified in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_cancel) {
            startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
            return true;
        }
        return super.onOptionsItemSelected(item);
    }


    private void onNext() {
        final String oldKey = getVoicePrintApp().getCurrentKeyPhrase().getId();
        String oldName = getVoicePrintApp().getCurrentKeyPhrase().getName();

        // Update current object
        getVoicePrintApp().setKeyPhraseName(name.getText().toString());

        // pton TODO to SDK

        Log.d(TAG, "Old Keyphrase Name=" + oldName);
        Log.d(TAG, "New Keyphrase Name=" + getVoicePrintApp().getCurrentKeyPhrase().getName());
        Log.d(TAG, "New Keyphrase Id=" + getVoicePrintApp().getCurrentKeyPhrase().getId());
        try {

            mManager.rename(oldKey, getVoicePrintApp().getCurrentKeyPhrase().getId(), new OnResultListener() {
                @Override
                public void onResult(int code, Bundle extras) {
                    if (code == OnResultListener.SUCCESS) {

                        try {
                            // Update the database
                            VoicePrintDatabase.getInstance().deleteEntry(oldKey);
                            VoicePrintDatabase.getInstance().insertEntry(getVoicePrintApp().getCurrentKeyPhrase());
                        } catch (SQLException e) {
                            e.printStackTrace();
                        }

                        onRename(true);

                    } else {
                        onRename(false);
                    }
                }

                @Override
                public void onError(int code) {
                    onRename(false);
                }
            });


        } catch (Exception e) {
            Log.e(TAG, "Error renaming keyphrase: " + e.getMessage());
        }
    }

    private void onRename(boolean success) {
        if (success) {
            showDialog(this, R.string.rename_success_title,
                       R.string.rename_success_msg,
                       R.string.cap_ok,
            new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {

                }
            });
        } else {
            showDialog(this, R.string.rename_fail_title,
                       R.string.rename_fail_msg,
                       R.string.cap_ok,
            new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {

                }
            });
        }

        startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
    }
}
