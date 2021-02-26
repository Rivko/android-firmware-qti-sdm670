/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.

 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.resultui;

import android.animation.Animator;
import android.animation.Animator.AnimatorListener;
import android.animation.PropertyValuesHolder;
import android.animation.ValueAnimator;
import android.animation.ValueAnimator.AnimatorUpdateListener;
import android.app.ActionBar;
import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.DisplayMetrics;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.DecelerateInterpolator;
import android.widget.ImageView;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.RecognizedData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.FieldType;
import com.qualcomm.qti.sta.engine.JniManager;
import com.qualcomm.qti.sta.resultui.ActionListHandler.ListEventListener;
import com.qualcomm.qti.sta.savedataui.ContactActivity;
import com.qualcomm.qti.sta.savedataui.EventActivity;

/**
 * class for OCR Result Activity
 */
public class ResultActivity extends Activity implements ListEventListener {
    //private static final String LOG_TAG = "ResultActivity";

    private static final int REQUEST_CONTACT = 101;
    private static final int REQUEST_EVENT = 102;
    private static final int REQUEST_TEXT = 103;

    private static final float DURATION_STEP = 0.8f;
    private static final int SLIDE_LIST_DELAY = 350;

    private ResultImageHandler imageHandler = null;

    private ActionListHandler listHandler = null;
    private ActionListSlider listSlider = null;
    private JniManager jniManager = null;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_ocr_result);

        jniManager = JniManager.getInstance();
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        // create result image handler
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);

        ImageView resultBaseView = (ImageView) findViewById(R.id.ocr_bitmap_view);
        imageHandler = new ResultImageHandler(this, resultBaseView, jniManager.getDataList(),
                jniManager.getDataId(), metrics.widthPixels, metrics.heightPixels);

        // create list header
        ViewGroup viewGroup = (ViewGroup) findViewById(R.id.action_list_layout);
        listHandler = new ActionListHandler(this, viewGroup, jniManager.getDataList(), this);

        final int screenHeight = metrics.heightPixels;

        View actionListView = findViewById(R.id.ocr_actions_view);
        float midY = screenHeight - getResources().getDimension(R.dimen.actions_list_scroll);
        listSlider = new ActionListSlider(this, actionListView, midY);
        listSlider.enableVisibility(false);
        new Handler().postDelayed(new Runnable() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void run() {
                listSlider.enableVisibility(true);
                animateListViewOnLoad(screenHeight);
            }
        }, SLIDE_LIST_DELAY);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onContextItemSelected(MenuItem item) {
        if (item.getItemId() == ActionListHandler.MENU_COPY_ID) {
            RecognizedData data = listHandler.getSelectedActionData();

            if (data != null) {
                String text = data.getText();
                ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                ClipData clip = ClipData.newPlainText("", text);
                clipboard.setPrimaryClip(clip);

                Toast.makeText(this, R.string.message_text_copied, Toast.LENGTH_SHORT).show();
            }
            listHandler.clearSelectedActionItem();

        } else {
            //Log.d(LOG_TAG, "Edit pressed");
            listSlider.slideTop(new Runnable() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void run() {
                    listHandler.makeSelectedActionEditable();
                }
            });
        }
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu items for use in the action bar
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.ocr_result, menu);
        if (listHandler.isInEditTextMode()) {
            menu.getItem(0).setVisible(false);
            menu.getItem(1).setVisible(false);

        } else {
            boolean contactIcon = FieldType.areContactFieldsPresent(jniManager.getDataList());
            if (!contactIcon) {
                menu.getItem(0).setVisible(false);
            }

            boolean eventIcon = FieldType.areEventFieldsPresent(jniManager.getDataList());
            if (!eventIcon) {
                menu.getItem(1).setVisible(false);
            }
        }

        return super.onCreateOptionsMenu(menu);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case android.R.id.home:
            finish();
            return true;

        case R.id.action_add_contact: {
            Intent contactIntent = new Intent(getBaseContext(), ContactActivity.class);
            startActivityForResult(contactIntent, REQUEST_CONTACT);
        }
            return true;

        case R.id.action_add_event: {
            Intent eventIntent = new Intent(getBaseContext(), EventActivity.class);
            startActivityForResult(eventIntent, REQUEST_EVENT);
        }
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_TEXT) {
            if (data != null) {
                int textIndex = data.getIntExtra(Utils.INTENT_KEY_TEXT_INDEX, -1);
                String newText = data.getStringExtra(Utils.INTENT_KEY_TEXT);
                if (textIndex != -1 && newText != null) {
                    int parentIndex = listHandler.getParentIndex(textIndex);
                    String actionTitle = newText.replace('\n', ' ');
                    jniManager.updateDataText(parentIndex, actionTitle);

                    listHandler.updateText(textIndex, newText);
                }
            }
        } else if (resultCode == RESULT_OK && data != null) {
            CategoryType categoryType = (CategoryType) data
                    .getSerializableExtra(Utils.INTENT_KEY_FIND_MORE_TYPE);
            if(categoryType != null) {
            setResult(RESULT_OK, data);
            finish();
        }
    }
    }

    /**
     * This function gets executed when user clicks on scan more button
     *
     * @param view
     *            view on which user clicked
     */
    public void onScanMore(View view) {
        listHandler.clearEditModeIfAny();

        Intent resultIntent = new Intent();
        resultIntent.putExtra(Utils.INTENT_KEY_FIND_MORE_TYPE, CategoryType.ALL_RESULTS);
        setResult(RESULT_OK, resultIntent);
        finish();
    }

    private void animateListViewOnLoad(int screenheight) {
        int listFromY = screenheight;
        int listToY = (int) listSlider.getMidY();
        PropertyValuesHolder listYProperty = PropertyValuesHolder.ofInt("list_y", listFromY,
                listToY);

        float imageFromY = imageHandler.getImageStartY();
        float imageToY = 0;
        PropertyValuesHolder imageYProperty = PropertyValuesHolder.ofFloat("image_y", imageFromY,
                imageToY);

        // create animator
        ValueAnimator animation = ValueAnimator.ofPropertyValuesHolder(imageYProperty,
                listYProperty);
        int duration = (int) (Math.abs(listFromY - listToY) * DURATION_STEP);
        //Log.d(LOG_TAG, "List animation duration: " + duration);
        animation.setInterpolator(new DecelerateInterpolator());
        animation.setDuration(duration);
        animation.addUpdateListener(new AnimatorUpdateListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                int marginYValue = (Integer) animation.getAnimatedValue("list_y");
                listSlider.setListY(marginYValue);

                float imageYValue = (Float) animation.getAnimatedValue("image_y");
                imageHandler.setImageY(imageYValue);
            }
        });
        animation.addListener(new AnimatorListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationStart(Animator animation) {
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationRepeat(Animator animation) {
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationEnd(Animator animation) {
                listSlider.enableTouchEvents(true);

                View framelayoutView = findViewById(R.id.ocr_result_view);
                listSlider.updateMaxY(framelayoutView.getHeight());
                imageHandler.enableTouchEvents();
            }

            /**
             * {@inheritDoc}
             */
            @Override
            public void onAnimationCancel(Animator animation) {
            }
        });
        animation.start();
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
     * {@inheritDoc}
     */
    @Override
    public void onEditTextModeStart() {
        invalidateOptionsMenu();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onEditTextModeOver(int parentIndex, String newText) {
        invalidateOptionsMenu();
        listSlider.enableTouchEvents(true);

        if (newText != null) {
            jniManager.updateDataText(parentIndex, newText);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onDateClicked(int dataListIndex, int dateIndex) {
        Intent clickedIntent = new Intent(this, EventActivity.class);
        clickedIntent.putExtra(Utils.INTENT_KEY_DATETIME_AT, dataListIndex);
        clickedIntent.putExtra(Utils.INTENT_KEY_DATETIME_INDEX, dateIndex);
        startActivityForResult(clickedIntent, REQUEST_EVENT);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onTextClicked(int dataListIndex, String text) {
        Intent clickedIntent = new Intent(this, TextActivity.class);
        clickedIntent.putExtra(Utils.INTENT_KEY_TEXT, text);
        clickedIntent.putExtra(Utils.INTENT_KEY_TEXT_INDEX, dataListIndex);
        startActivityForResult(clickedIntent, REQUEST_TEXT);
    }
}
