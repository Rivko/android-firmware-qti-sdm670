/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.os.Handler;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;
import android.widget.Toast;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.ApplicationMessage.MessageType;
import com.qualcomm.qti.sta.data.DateTextHandler;
import com.qualcomm.qti.sta.data.OCRIntentData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.engine.JniManager;
import com.qualcomm.qti.sta.engine.JniManager.STAListener;
import com.qualcomm.qti.sta.mainui.ProgressAnimator.ProgressListener;
import com.qualcomm.qti.sta.resultui.ResultActivity;
import com.qualcomm.qti.sta.savedataui.ContactActivity;
import com.qualcomm.qti.sta.savedataui.EventActivity;
import com.qualcomm.qti.sta.settings.PreferenceData;

/**
 * The base class for the activities which parse images for the text recognition
 */
public abstract class CaptureActivity extends Activity implements STAListener, ProgressListener {
    private static final String LOG_TAG = "CaptureActivity";

    protected static final int TOAST_LONG_DELAY = 4000;
    protected static final int TOAST_SHORT_DELAY = 2500;

    private static final int REQUEST_ACTIONS = 101;
    private static final int REQUEST_CONTACT = 102;
    private static final int REQUEST_EVENT = 103;

    private Toast currentToast = null;

    protected JniManager jniManager = null;
    private ProgressAnimator progressAnimator = null;
    protected Handler uiHandler = new Handler();

    protected CategoryType findMoreCategoryType = null;
    protected String intentType = null;
    protected String intentLocale = null;
    protected int targetDataId = 0;
    protected Bitmap staBitmap = null;

    /**
     * Initializes the base class data
     */
    protected void initializeBaseData() {
        progressAnimator = new ProgressAnimator(getResources(),
                findViewById(R.id.progress_anim_layout), findViewById(R.id.parsing_anim_progress),
                findViewById(R.id.parsing_anim_done), this);
        jniManager = JniManager.getInstance();

        Intent intent = getIntent();
        intentType = intent.getStringExtra(OCRIntentData.KEY_TYPE);
        intentLocale = intent.getStringExtra(OCRIntentData.KEY_LOCALE);
        targetDataId = jniManager.getDataId() + 1;
        Log.d(LOG_TAG, "target dataId: " + targetDataId);
    }

    /**
     * initializes the OCR Parser
     */
    protected void initializeOCRParser(int scveMode, int textRecordMode) {
        String locale = intentLocale;
        if (locale == null) {
            locale = PreferenceData.getOCRLocale(this);
        }
        int flags = PreferenceData.getOCRFlags(this);

        String finalFile = Utils.getLocalImageFile(this);
        DateTextHandler dateTextHandler = new DateTextHandler(getResources());
        jniManager.setParserListener(this);
        jniManager.initializeParser(scveMode, textRecordMode, locale, flags, dateTextHandler, finalFile);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onResume() {
        super.onResume();

        jniManager.setParserListener(this);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onPause() {
        super.onPause();

        if (currentToast != null) {
            currentToast.cancel();
            currentToast = null;
        }

        uiHandler.removeCallbacksAndMessages(null);
        jniManager.setParserListener(null);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onStop() {
        super.onStop();

        progressAnimator.reset();
        lockCurrentOrientation(false);

        cleanStaBitmap();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();

        jniManager.release();
    }
    /**
     * clears the ROI bitmap image
     */
    protected void cleanStaBitmap() {
        if(staBitmap != null && !staBitmap.isRecycled()) {
            staBitmap.recycle();
            Log.d(LOG_TAG, "STA Bitmap recycled");
        }
        staBitmap = null;
    }

    protected void callNextActivity() {
        if (isNextActivity(CategoryType.CONTACT)) {
            Intent resultIntent = new Intent(getBaseContext(), ContactActivity.class);
            if (intentType != null) {
                resultIntent.putExtra(OCRIntentData.KEY_TYPE, intentType);
            }
            startActivityForResult(resultIntent, REQUEST_CONTACT);

        } else if (isNextActivity(CategoryType.EVENT)) {
            Intent resultIntent = new Intent(getBaseContext(), EventActivity.class);
            if (intentType != null) {
                resultIntent.putExtra(OCRIntentData.KEY_TYPE, intentType);
            }
            startActivityForResult(resultIntent, REQUEST_EVENT);

        } else {// all none
            Intent resultIntent = new Intent(getBaseContext(), ResultActivity.class);
            startActivityForResult(resultIntent, REQUEST_ACTIONS);
        }
    }

    private boolean isNextActivity(CategoryType categoryType) {
        if (findMoreCategoryType == categoryType) {
            return true;
        }
        if (categoryType == CategoryType.CONTACT && intentType != null
                && intentType.equals(OCRIntentData.VALUE_TYPE_CONTACT)) {
            return true;
        }
        if (categoryType == CategoryType.EVENT && intentType != null
                && intentType.equals(OCRIntentData.VALUE_TYPE_CALENDAR)) {
            return true;
        }
        return false;
    }

    /**
     * this method shows the Application messages
     *
     * @param message
     *            application message to be shown
     */
    protected void showApplicationMessage(final ApplicationMessage message) {
        String messageText = getResources().getString(message.getId());

        int errorCode = message.getErrorId();
        if (errorCode != 0) {
            String errorPrefix = getResources().getString(R.string.error_prefix);
            messageText = messageText + errorPrefix + Integer.toHexString(errorCode);
        }
        if (message.getType() == MessageType.TOAST_LONG
                || message.getType() == MessageType.TOAST_SHORT) {
            currentToast = Toast.makeText(this, messageText, Toast.LENGTH_LONG);
            currentToast.show();

            Runnable finishRunnable = message.getFinishCallback();
            if (finishRunnable != null) {
                long delay = (message.getType() == MessageType.TOAST_LONG) ? TOAST_LONG_DELAY
                        : TOAST_SHORT_DELAY;
                uiHandler.postDelayed(finishRunnable, delay);
                message.setFinishCallback(null);
            }
        } else {
            String positiveBtn = getResources().getString(R.string.error_dismiss_button);
            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this,
                    AlertDialog.THEME_HOLO_LIGHT);
            dialogBuilder
                    .setTitle((message.getType() == MessageType.INFO) ? R.string.title_app_message
                            : R.string.error_title);
            dialogBuilder.setMessage(messageText);

            dialogBuilder.setPositiveButton(positiveBtn, new DialogInterface.OnClickListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void onClick(DialogInterface dialog, int whichButton) {
                    dialog.dismiss();
                    dismissErrorMessage(message);
                }
            });
            Dialog dialog = dialogBuilder.create();
            dialog.setOnCancelListener(new OnCancelListener() {

                /**
                 * {@inheritDoc}
                 */
                @Override
                public void onCancel(DialogInterface dialog) {
                    dialog.dismiss();
                    dismissErrorMessage(message);
                }
            });
            dialog.show();
        }
    }

    private void dismissErrorMessage(ApplicationMessage message) {
        if (message.getType() == MessageType.FATAL) {
            finish();
        } else {
            Runnable finishRunnable = message.getFinishCallback();
            if (finishRunnable != null) {
                uiHandler.post(finishRunnable);
                message.setFinishCallback(null);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onAnimationComplete() {// already on ui thread
        Log.d(LOG_TAG, "Progress Animation done");
        callNextActivity();
    }

    /**
     * resets the progress animation
     */
    protected void resetProgressAnimation() {
        progressAnimator.reset();
        lockCurrentOrientation(false);
    }

    /**
     * Starts the progress animation
     */
    protected void showProgressAnimation(boolean show) {
        if (show) {
            progressAnimator.start();
            lockCurrentOrientation(true);

        } else {
            progressAnimator.startDoneAnimation();
        }
    }

    private void lockCurrentOrientation(boolean lock) {
        if (lock) {
            int ConfigOrientation = getResources().getConfiguration().orientation;
            int deviceOrientation;
            int displayRotation = ((WindowManager) getSystemService(Context.WINDOW_SERVICE))
                    .getDefaultDisplay().getRotation();

            if(ConfigOrientation == Configuration.ORIENTATION_LANDSCAPE) {
                if(displayRotation == 0 || displayRotation == 1) {
                    deviceOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
                    Log.d(LOG_TAG, "LOCK: config: " + ConfigOrientation + " display: " + displayRotation + " => landscape" );
                } else {
                    deviceOrientation = ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
                    Log.d(LOG_TAG, "LOCK: config: " + ConfigOrientation + " display: " + displayRotation + " => reverse landscape" );
                }

            } else {//if(ConfigOrientation == Configuration.ORIENTATION_PORTRAIT) {
                if(displayRotation == Surface.ROTATION_0 || displayRotation == 3) {
                    deviceOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
                    Log.d(LOG_TAG, "LOCK: config: " + ConfigOrientation + " display: " + displayRotation + " => portrait" );
                } else {
                    deviceOrientation = ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
                    Log.d(LOG_TAG, "LOCK: config: " + ConfigOrientation + " display: " + displayRotation + " => inverse portrait" );
                }
            }

            setRequestedOrientation(deviceOrientation);
        } else {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
            Log.d(LOG_TAG, "Unlocked Orientation");
        }
    }

    protected boolean handleBaseResults(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_ACTIONS || requestCode == REQUEST_CONTACT
                || requestCode == REQUEST_EVENT) {
            findMoreCategoryType = null;
            if (resultCode == RESULT_OK) {
                if (data != null) {
                    findMoreCategoryType = (CategoryType) data
                            .getSerializableExtra(Utils.INTENT_KEY_FIND_MORE_TYPE);

                    if (findMoreCategoryType == null && intentType != null) {
                        Log.d(LOG_TAG, "Sending data back to calling activity");
                        setResult(RESULT_OK, data);
                        finish();
                    }
                }
            }
            if (findMoreCategoryType == null) {// new start
                jniManager.clearData();
                targetDataId = 1;
            } else {
                targetDataId = jniManager.getDataId() + 1;
            }
            Log.d(LOG_TAG, "target dataId: " + targetDataId);
            return true;
        }
        return false;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSTAResult() {// on ocr thread
        runOnUiThread(new Runnable() {
            /**
             * {@inheritDoc}
             */
            public void run() {
                showProgressAnimation(false);
            }
        });
    }
}
