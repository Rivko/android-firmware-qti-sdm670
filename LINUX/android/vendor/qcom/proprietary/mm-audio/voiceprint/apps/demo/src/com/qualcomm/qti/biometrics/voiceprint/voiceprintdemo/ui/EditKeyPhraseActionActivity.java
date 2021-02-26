/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.Consts;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhrase;

public class EditKeyPhraseActionActivity extends VoicePrintActivity
        implements KeyPhraseSelectAdapter.OnKeyPhraseSelectionChange {

    private ListView listView;
    private ArrayAdapter<String> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.edit_key_phrase_action_list);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        setTitle(getVoicePrintApp().getCurrentKeyPhrase().getPhrase());

        listView = (ListView) findViewById(R.id.list);


        final String[] actions =
                {
                        getString(R.string.edit_action),
                        getString(R.string.threshold),
                        getString(R.string.rename),
                        getString(R.string.delete_phrase)
                };
        final Class<?>[] invoker = {
                ChangeKeyPhraseActionActivity.class,
                SetThresholdActivity.class,
                RenameKeyPhraseActivity.class,
                DeletePhraseActivity.class,
        };


        listView.setClickable(true);

        adapter = new ArrayAdapter<String>(this, R.layout.two_item_row,
                actions) {
            public View getView(final int position, View convertView, ViewGroup parent) {
                LayoutInflater inflater = (LayoutInflater)
                        getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                // TODO PM team to provide threshold string info.

                String[] substrings = {
                        getVoicePrintApp().getCurrentActionName(),
                        Consts.Sensitivity[getVoicePrintApp().getCurrentKeyPhrase().getThreshold()],
                        getVoicePrintApp().getCurrentKeyPhrase().getName(),
                        getVoicePrintApp().getCurrentKeyPhrase().getPhrase()
                };


                // Inflate a custom row view.
                View rowView = inflater
                        .inflate(R.layout.two_item_row, parent, false);

                // Add padding from start to text
                TextView text1 = (TextView) rowView.findViewById(R.id.text1);
                text1.setText(actions[position]);
                // text1.setPadding(15, 0, 15, 0);

                TextView text2 = (TextView) rowView.findViewById(R.id.text2);
                text2.setText(substrings[position]);
                // text2.setPadding(15, 0, 15, 0);

                return rowView;
            }
        };
        listView.setAdapter(adapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, final int position, long id) {
                //     Log.d(TAG, "setOnItemClickListener item " + position);

                listView.requestFocusFromTouch();
                listView.smoothScrollToPosition(position);
                listView.setSelectionFromTop(position, listView.getHeight() / 2);

                // Handle the action
                startActivity(new Intent(getVoicePrintApp(), invoker[position]));
            }
        });


        listView.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                Log.d(TAG, "setOnItemSelectedListener: " + position);
                adapter.notifyDataSetChanged();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            // Empty
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.edit_key_phrase_action, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    // Handle menu selection
        int id = item.getItemId();
        if (id == R.id.action_cancel) {
            startActivity(new Intent(this, MainActivity.class));
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onSelectionChange(int position, KeyPhrase newSelected, boolean isChecked) {

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check for request code and handle
        if (requestCode == Consts.VERIFICATION) {
            // Make sure the request was successful
            if (resultCode == Consts.RESULT_SUCCESS) {
                startActivity(new Intent(this, NewKeyPhraseActivity.class));
            } else {
                Log.d(TAG, "PIN verification failed. Reason code=" + resultCode);
            }
        }
    }
}
