/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;

import java.sql.SQLException;

public class DeletePhraseActivity extends VoicePrintActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.delete_phrase, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_cancel) {
            onCancel();
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onResume() {
        super.onResume();

        showDialog(this, getString(R.string.delete_phrase),
                String.format(getString(R.string.delete_phrase_msg),
                        getVoicePrintApp().getCurrentKeyPhrase().getPhrase()),
                R.string.cap_ok,
                R.string.cancel,
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        onDelete();
                    }
                },
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        onCancel();
                    }
                });
    }

    private void onCancel() {
        startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
    }

    private void onDelete() {
        try {
            mManager.delete(getVoicePrintApp().getCurrentKeyPhrase().getId(), new OnResultListener() {
                @Override
                public void onResult(int code, Bundle extras) {
                    if (code == OnResultListener.SUCCESS) {

                        try {
                            // Update the database
                            VoicePrintDatabase.getInstance().deleteEntry(getVoicePrintApp().getCurrentKeyPhrase());
                        } catch (SQLException e) {
                            e.printStackTrace();
                        }

                        onDelete(true);

                    } else {
                        onDelete(false);
                    }
                }

                @Override
                public void onError(int code) {
                    onDelete(false);
                }
            });

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void onDelete(boolean success) {
        if (success) {
            showDialog(this, R.string.delete_success_title,
                    R.string.delete_success_msg,
                    R.string.cap_ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                        }
                    });
        } else {
            showDialog(this, R.string.delete_fail_title,
                    R.string.delete_fail_msg,
                    R.string.cap_ok,
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                        }
                    });
        }

        startActivity(new Intent(this, EditKeyPhrasesListActivity.class));
    }

}
