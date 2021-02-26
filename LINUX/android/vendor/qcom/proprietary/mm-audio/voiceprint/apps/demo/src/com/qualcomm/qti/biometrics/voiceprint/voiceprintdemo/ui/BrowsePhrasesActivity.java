/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseList;

public class BrowsePhrasesActivity extends VoicePrintActivity implements KeyPhraseSelectAdapter.OnKeyPhraseSelectionChange {


    private PhraseListView phraseListView;
    private Button actionButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.browse_phrases);

        phraseListView = (PhraseListView) findViewById(R.id.phraseList);
        KeyPhraseList list = new KeyPhraseList(this);

        if (list.size() == 0) {
            String[] default_phrases = getResources().getStringArray(R.array.default_phrases);
            for (int i = 0; i < default_phrases.length; ++i) {
                list.add(null, default_phrases[i]);
            }
        }

        phraseListView.setPhraseList(list);

        phraseListView.setOnSelectionChangeListener(this);

        actionButton = (Button) findViewById(R.id.button);
        actionButton.setText(getString(R.string.use_selected_phrase));
        actionButton.setEnabled(false);
        actionButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                nextStep();
            }
        });
    }

    private void nextStep() {
        getVoicePrintApp().setKeyPhrase(phraseListView.getSelected());
        startActivity(new Intent(this, SetKeyPhraseNameActivity.class));
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

    @Override
    public void onSelectionChange(int position, KeyPhrase newSelected, boolean isChecked) {
        actionButton.setEnabled(phraseListView.hasSelection());
    }
}
