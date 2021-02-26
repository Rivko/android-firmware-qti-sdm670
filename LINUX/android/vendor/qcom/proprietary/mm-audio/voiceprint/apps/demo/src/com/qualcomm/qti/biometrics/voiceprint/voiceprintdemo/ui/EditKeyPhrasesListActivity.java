/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseList;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;

public class EditKeyPhrasesListActivity extends VoicePrintActivity implements KeyPhraseSelectAdapter.OnKeyPhraseSelectionChange {


    private PhraseListView phraseListView;
    private Button actionButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edit_key_phrases);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        phraseListView = (PhraseListView) findViewById(R.id.phraseList);

    }

    @Override
    protected void onResume() {
        super.onResume();

        // Rebuild each time
        KeyPhraseList list = new KeyPhraseList(getVoicePrintApp(),
                VoicePrintDatabase.getInstance().getKeyPhrases());
        phraseListView.setPhraseList(list);

        phraseListView.setOnSelectionChangeListener(this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.edit_key_phrases, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks. The action bar will
        // automatically handle clicks on the Home/Up button,
        // parent activity specified in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_add) {
            // User must have verified PIN before coming to this screen.
            startActivity(new Intent(this, NewKeyPhraseActivity.class));
        } else if (id == R.id.action_cancel) {
            startActivity(new Intent(this, MainActivity.class));
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onSelectionChange(int position, KeyPhrase newSelected, boolean isChecked) {
        getVoicePrintApp().setKeyPhrase(phraseListView.getSelected());

        startActivity(new Intent(this, EditKeyPhraseActionActivity.class));
    }
}
