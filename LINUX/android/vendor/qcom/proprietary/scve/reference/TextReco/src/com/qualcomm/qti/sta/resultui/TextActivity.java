/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import android.app.ActionBar;
import android.app.Activity;
import android.app.SearchManager;
import android.content.ActivityNotFoundException;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.engine.JniManager;
import com.qualcomm.qti.sta.settings.PreferenceData;

/**
 * An activity to show the general text
 */
public class TextActivity extends Activity {
    private static final String LOG_TAG = "TextActivity";

    private JniManager jniManager = null;
    private EditText editText = null;
    private int textIndex = -1;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_text);

        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        Intent activityIntent = getIntent();
        textIndex = activityIntent.getIntExtra(Utils.INTENT_KEY_TEXT_INDEX, -1);
        String text = activityIntent.getStringExtra(Utils.INTENT_KEY_TEXT);
        editText = (EditText) findViewById(R.id.general_text);
        editText.setText(text);
        editText.setSelection(text.length());

        jniManager = JniManager.getInstance();
    }
    private void setResultIntent() {
        String text = editText.getText().toString();
        Intent resultIntent = new Intent();
        resultIntent.putExtra(Utils.INTENT_KEY_TEXT, text);
        resultIntent.putExtra(Utils.INTENT_KEY_TEXT_INDEX, textIndex);
        setResult(RESULT_OK, resultIntent);
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            setResultIntent();
            finish();
            return true;
        }
        return false;
    }
    /**
     * {@inheritDoc}
     */
    @Override
    public void onBackPressed() {
        setResultIntent();
        finish();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onDestroy() {
        super.onDestroy();

        jniManager.release();
    }

    /**
     * This function gets called when user presses on search
     *
     * @param view
     *            the view on which user clicked
     */
    public void onActionSearch(View view) {
        Log.d(LOG_TAG, "User clicked on Search");
        String text = editText.getText().toString();
        try {
            Intent intent = new Intent(Intent.ACTION_WEB_SEARCH);
            intent.putExtra(SearchManager.QUERY, text);
            startActivity(intent);
        } catch (ActivityNotFoundException e) {
            String message = getResources().getString(R.string.message_no_matching_app);
            Toast.makeText(this, message + " " + text, Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }
    }

    /**
     * This function gets called when user presses on translate
     *
     * @param view
     *            the view on which user clicked
     */
    public void onActionTranslate(View view) {
        Log.d(LOG_TAG, "User clicked on Translate");
        String text = editText.getText().toString();
        try {
            String translateBaseString = getResources().getString(R.string.translate_url);
            String language = PreferenceData.getTextLanguage(this);
            String translateUrl = translateBaseString + language + "/" + text;

            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setData(Uri.parse(translateUrl));
            startActivity(intent);

        } catch (ActivityNotFoundException e) {
            String message = getResources().getString(R.string.message_no_matching_app);
            Toast.makeText(this, message + " " + text, Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }
    }

    /**
     * This function gets called when user presses on copy
     *
     * @param view
     *            the view on which user clicked
     */
    public void onActionCopy(View view) {
        String text = editText.getText().toString();
        ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        ClipData clip = ClipData.newPlainText("", text);
        clipboard.setPrimaryClip(clip);

        Toast.makeText(this, R.string.message_text_copied, Toast.LENGTH_SHORT).show();
    }
}
