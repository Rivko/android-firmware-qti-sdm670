/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.savedataui;

import java.util.ArrayList;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.qualcomm.qti.sta.data.CategoryData;
import com.qualcomm.qti.sta.data.FieldData;
import com.qualcomm.qti.sta.data.OCRIntentData;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.engine.JniManager;
import com.qualcomm.qti.sta.R;

/**
 * Base class for data activities
 */
public abstract class SaveDataActivity extends Activity {
    private static final String LOG_TAG = "SaveDataActivity";

    protected JniManager jniManager = null;
    protected FieldListHandler fieldListHandler = null;
    protected CategoryType categoryType = null;
    private int staDataId = 0;
    protected boolean exiting = false;

    protected String intentType = null;
    protected static final int INSTRUCTION_READ_TIME = 2000;

    protected Runnable exitRunnable = new Runnable() {
        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            setResult(RESULT_OK);
            finish();
        }
    };

    /**
     * initializes the base class data
     *
     * @param categoryType
     *            category type
     */
    protected void initializeBaseData(CategoryType categoryType) {
        jniManager = JniManager.getInstance();
        this.categoryType = categoryType;
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onResume() {
        super.onResume();

        // populate list
        ViewGroup primaryView = (ViewGroup) findViewById(R.id.upper_part);
        ViewGroup secondaryView = (ViewGroup) findViewById(R.id.lower_part);

        staDataId = jniManager.getDataId();
        CategoryData catageoryData = jniManager.getCategoryData();
        fieldListHandler = createListhandler(primaryView, secondaryView, catageoryData,
                jniManager.getDataList());

        // keep focus away from text view on resume
        View upperPartLayout = findViewById(R.id.upper_part_layout);
        upperPartLayout.requestFocus();

        Intent intent = getIntent();
        intentType = intent.getStringExtra(OCRIntentData.KEY_TYPE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu items for use in the action bar
        MenuInflater inflater = getMenuInflater();
        if (intentType != null
                && (intentType.equals(OCRIntentData.VALUE_TYPE_CONTACT) || intentType
                        .equals(OCRIntentData.VALUE_TYPE_CALENDAR))) {
            inflater.inflate(R.menu.done_data, menu);

        } else {

            inflater.inflate(R.menu.save_data, menu);
        }
        return super.onCreateOptionsMenu(menu);
    }

    /**
     * This function is called when user clicks on scan more button
     *
     * @param view
     *            view on which user clicked
     */
    public void onScanMore(View view) {
        if (!exiting) {
            Intent resultIntent = new Intent();
            resultIntent.putExtra(Utils.INTENT_KEY_FIND_MORE_TYPE, categoryType);
            setResult(RESULT_OK, resultIntent);
            finish();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onStop() {
        super.onStop();
        if (!exiting) {
            Log.d(LOG_TAG, "setting filled fields");
            ArrayList<FieldData> filledFields = fieldListHandler.getFilledFields();
            jniManager.setCategoryData(new CategoryData(filledFields, staDataId));
        } else {
            Log.d(LOG_TAG, "Cleared category data");
            jniManager.setCategoryData(null);
        }
        fieldListHandler.clearViews();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBackPressed() {
        super.onBackPressed();

        exiting = true;
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
     * This function is called when user clicks on Add field button
     *
     * @param view
     *            view on which user clicked
     */
    public void onAddField(View view) {
        if (exiting) {
            return;
        }
        final ArrayList<FieldType> fieldTypes = fieldListHandler.getAvailableFields();
        if (fieldTypes.size() > 0) {
            AlertDialog.Builder builder = new AlertDialog.Builder(this,
                    AlertDialog.THEME_HOLO_LIGHT);
            builder.setTitle(R.string.title_add_field);
            String[] fieldNames = new String[fieldTypes.size()];
            for (int i = 0; i < fieldTypes.size(); i++) {
                FieldType fieldType = fieldTypes.get(i);

                int resourceId = (fieldType.getNameId() != 0) ? fieldType.getNameId() : fieldType
                        .getCaptionId();
                fieldNames[i] = getResources().getString(resourceId);
            }

            builder.setItems(fieldNames, new DialogInterface.OnClickListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void onClick(DialogInterface dialog, int index) {
                    fieldListHandler.addField(fieldTypes.get(index));
                }
            });
            Dialog dialog = builder.create();
            dialog.show();

        } else {
            Toast.makeText(this, R.string.message_no_more_fields, Toast.LENGTH_LONG).show();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case android.R.id.home:
            exiting = true;
            finish();
            return true;

        case R.id.action_save:
            if (!exiting) {
                ArrayList<FieldData> fieldDataList = fieldListHandler.getFilledFields();
                onSavePressed(fieldDataList);
                return true;
            }
        case R.id.action_done:
            if (!exiting) {
                exiting = true;
                ArrayList<FieldData> fieldDataList = fieldListHandler.getFilledFields();
                onDonePressed(fieldDataList);
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    /**
     * Notification when user presses save button
     *
     * @param fieldDataList
     *            fields
     */
    protected abstract void onSavePressed(ArrayList<FieldData> fieldDataList);

    /**
     * Notification when user presses done button
     *
     * @param fieldDataList
     *            fields
     */
    protected abstract void onDonePressed(ArrayList<FieldData> fieldDataList);

    /**
     * creates appropriate list handler
     *
     * @param primaryView
     *            primary view
     * @param secondaryView
     *            secondary view
     * @param categoryData
     *            category data
     * @param staDataList
     *            ocr data list
     * @return newly created list handler
     */
    protected abstract FieldListHandler createListhandler(ViewGroup primaryView,
            ViewGroup secondaryView, CategoryData categoryData,
            ArrayList<RecognizedData> staDataList);
}
