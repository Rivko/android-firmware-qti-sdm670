/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.RectF;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.PopupMenu;
import android.widget.PopupMenu.OnDismissListener;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.OCRIntentData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.data.Utils.CategoryType;
import com.qualcomm.qti.sta.data.ViewSize;
import com.qualcomm.qti.sta.device.Camera2Impl;
import com.qualcomm.qti.sta.device.CameraHandler;
import com.qualcomm.qti.sta.device.CameraHandler.CameraListener;
import com.qualcomm.qti.sta.device.CameraImpl;
import com.qualcomm.qti.sta.device.CoordinateSpace;
import com.qualcomm.qti.sta.engine.JniConstants;
import com.qualcomm.qti.sta.engine.JniManager.FrameSession;
import com.qualcomm.qti.sta.mainui.CaptureUI.CaptureUIListener;
import com.qualcomm.qti.sta.mainui.gallery.GalleryImageActivity;
import com.qualcomm.qti.sta.mainui.snapshot.SnapshotUI;
import com.qualcomm.qti.sta.settings.PreferenceData;
import com.qualcomm.qti.sta.settings.SettingsActivity;

/**
 * Camera activity class
 */
public class CameraActivity extends CaptureActivity implements CameraListener, CaptureUIListener {
    private static final String LOG_TAG = "CameraActivity";

    private static final int REQUEST_GALLERY = 104;
    private static final String KEY_ACTIVITY_STATE = "activityState";
    private static final String KEY_FIND_MORE_CTG = "findMore";

    private static final int RESET_REASON_RESTART = 1;
    private static final int RESET_REASON_ABORT = 2;

    /**
     * enum for Capture type
     */
    public enum CaptureType {
        UNKNOWN, SNAPSHOT, AUTO_PREVIEW, BUTTON_PREVIEW;
    }

    private CaptureType previewCaptureType = CaptureType.AUTO_PREVIEW;
    private CaptureType captureType = CaptureType.UNKNOWN;
    private CaptureUI captureUI = null;
    private CameraHandler cameraHandler = null;

    // other activity parameters
    private enum ActivityState {
        NONE, INSTRUCTION, ROI_AND_CAPTURE, IMAGE_CORRECTION, ALGORITHM;
    }

    private ActivityState activityState = ActivityState.NONE;
    private FrameLayout frameLayout = null;
    // on/off views
    private View instructionView = null;
    private View previewFrameView = null;
    private View restartMessageView = null;
    // button/icons
    private View iconOptions = null;
    private View iconFlash = null;
    private View iconMacro = null;

    private View icIcon = null;
    private View icButtons = null;
    private boolean icEnable = false;

    private boolean waitForOcrReady = false;
    private PopupMenu popupMenu = null;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_camera);

        // load views
        frameLayout = (FrameLayout) findViewById(R.id.camera_layout);
        instructionView = findViewById(R.id.instruction_view);
        restartMessageView = findViewById(R.id.restart_message_layout);

        iconOptions = findViewById(R.id.icon_option);
        iconFlash = findViewById(R.id.icon_flash);
        iconMacro = findViewById(R.id.icon_macro);
        icIcon = findViewById(R.id.icon_ic);
        icButtons = findViewById(R.id.ic_button_layout);
        previewFrameView = findViewById(R.id.preview_frame);

        makeIconsFunctional();
        if (isLegacyCameraBetter()) {
            Log.d(LOG_TAG, "Using Camera API");
            cameraHandler = new CameraImpl(this, this);
        } else {
            Log.d(LOG_TAG, "Using Camera2 API");
            cameraHandler = new Camera2Impl(this, this);
        }
        fetchInstanceStateData(savedInstanceState);// read saved data
        initializeBaseData();

        try {
            String previewTypeKey = getResources().getString(R.string.preview_type_key);
            ActivityInfo ai = getPackageManager().getActivityInfo(getComponentName(),
                    PackageManager.GET_META_DATA);
            if (ai.metaData != null) {
                String previewType = ai.metaData.getString(previewTypeKey);
                if (previewType != null) {
                    String previewTypeButton = getResources().getString(
                            R.string.preview_type_button);
                    if (previewType.equals(previewTypeButton)) {
                        previewCaptureType = CaptureType.BUTTON_PREVIEW;
                    }
                }
            }
            Log.d(LOG_TAG, "Capture Type: " + previewCaptureType);
        } catch (NameNotFoundException e) {
            Log.d(LOG_TAG, "No meta data or associate keys found");
        }
    }

    private boolean isLegacyCameraBetter() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            Log.d(LOG_TAG, "API present < 21, better to use Camera1 api");
            return true;
        }
        if (!Camera2Impl.isFullySupported(this)) {
            Log.d(LOG_TAG, "Camera2 not fully supported, better to use Camera1 api");
            return true;
        }
        return false;
    }

    private void makeIconsFunctional() {
        boolean isFlashSupported = getPackageManager().hasSystemFeature(
                PackageManager.FEATURE_CAMERA_FLASH);
        if (!isFlashSupported) {
            iconFlash.setVisibility(View.GONE);
            iconFlash = null;
        } else {
            iconFlash.setOnClickListener(new OnClickListener() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void onClick(View v) {
                    if (cameraHandler != null && activityState != ActivityState.INSTRUCTION) {
                        cameraHandler.toggleFlash();
                        ((ImageView) iconFlash).setImageResource(cameraHandler.isFlashOn() ? R.drawable.btn_flash_on
                                : R.drawable.btn_flash_off);
                    }
                }
            });
        }
        iconMacro.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View v) {
                if (cameraHandler != null && activityState != ActivityState.INSTRUCTION) {
                    cameraHandler.toggleMacroFocusMode();
                    ((ImageView) iconMacro).setImageResource(cameraHandler.isMacroFocusMode() ? R.drawable.btn_macro_on
                            : R.drawable.btn_macro_off);
                }
            }
        });
        icIcon.setOnClickListener(new OnClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onClick(View v) {
                enableIC(!icEnable);
            }
        });
    }

    private void enableIC(boolean enable) {
        icEnable = enable;
        ((ImageView) icIcon).setImageResource((icEnable) ? R.drawable.btn_ic_on
                : R.drawable.btn_ic_off);
        captureUI.enableRoi(!icEnable, !icEnable);
    }

    private void resetViewVisibility() {
        instructionView.setVisibility(View.GONE);
        previewFrameView.setVisibility(View.GONE);
        restartMessageView.setVisibility(View.GONE);
        captureUI.hide();

        enableIcons(true);

        ((ImageView) iconFlash)
                .setImageResource(cameraHandler.isFlashOn() ? R.drawable.btn_flash_on
                        : R.drawable.btn_flash_off);

        ((ImageView) iconMacro)
                .setImageResource(cameraHandler.isMacroFocusMode() ? R.drawable.btn_macro_on
                        : R.drawable.btn_macro_off);
        icButtons.setVisibility(View.GONE);
        if (captureType == CaptureType.SNAPSHOT) {
            icEnable = false;
            ((ImageView) icIcon).setImageResource(R.drawable.btn_ic_off);
        } else {
            icIcon.setVisibility(View.GONE);
        }
    }

    private void enableIcons(boolean enable) {
        int visibility = (enable ? View.VISIBLE : View.GONE);
        iconOptions.setVisibility(visibility);
        if (iconFlash != null) {
            iconFlash.setVisibility(visibility);
        }
        iconMacro.setVisibility(visibility);
        if (captureType == CaptureType.SNAPSHOT) {
            icIcon.setVisibility(visibility);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);

        savedInstanceState.putSerializable(KEY_ACTIVITY_STATE, activityState);
        if (findMoreCategoryType != null) {
            savedInstanceState.putSerializable(KEY_FIND_MORE_CTG, findMoreCategoryType);
        }
    }

    private void fetchInstanceStateData(Bundle savedInstanceState) {
        if (savedInstanceState == null) {
            return;
        }
        ActivityState state = (ActivityState) savedInstanceState
                .getSerializable(KEY_ACTIVITY_STATE);
        if (state != null) {
            activityState = state;
        }

        CategoryType catgoryType = (CategoryType) savedInstanceState
                .getSerializable(KEY_FIND_MORE_CTG);
        if (catgoryType != null) {
            findMoreCategoryType = catgoryType;
        }
        Log.d(LOG_TAG, "retrieved: " + activityState);
    }

    private boolean initalizeCamera() {
        boolean isInitialized = cameraHandler.initialize(getWindowManager().getDefaultDisplay());
        if (!isInitialized) {
            showApplicationMessage(ApplicationMessage.CAMERA_INITIALIZATION_FAILED);
            return false;
        }
        View firstChild = frameLayout.getChildAt(0);
        View view = cameraHandler.getView();

        if (firstChild != view) {
            FrameLayout.LayoutParams layoutParameters = new FrameLayout.LayoutParams(
                    LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
            layoutParameters.gravity = Gravity.CENTER;
            view.setLayoutParams(layoutParameters);
            frameLayout.addView(view, 0);
        }
        return true;
    }

    private void loadStateUI(ActivityState state) {
        switch (state) {
        case NONE:
            if (PreferenceData.isCameraInstruction(this)) {
                loadInstruction();
            } else {
                loadROI();
            }
            break;

        case INSTRUCTION:
            loadInstruction();
            break;

        case ROI_AND_CAPTURE:
            loadROI();
            break;
        case IMAGE_CORRECTION:
            showICScreen();
            break;
        case ALGORITHM:
            loadROI();
            break;

        default:
            break;
        }
    }

    private void loadInstruction() {
        if (!cameraHandler.isCameraInitialized()) {
            if (!initalizeCamera()) {
                return;
            }
        } else {
            Log.d(LOG_TAG, "Load Instruction: Camera Already initialized");
        }
        activityState = ActivityState.INSTRUCTION;
        instructionView.setVisibility(View.VISIBLE);
    }

    private void loadROI() {
        if (!cameraHandler.isCameraInitialized()) {
            if (!initalizeCamera()) {
                return;
            }
        } else {
            Log.d(LOG_TAG, "Load ROI: Camera Already initialized");
        }

        activityState = ActivityState.ROI_AND_CAPTURE;

        captureUI.enableRoi(true, true);
        if (jniManager.isParserInitialized()) {
            captureUI.parserReady();
            waitForOcrReady = false;

        } else {
            waitForOcrReady = true;
            Log.d(LOG_TAG, "Load ROI: Wait for OCR Ready");
        }
    }

    private PopupMenu createOptionMenu(View view) {
        PopupMenu optionMenu = new PopupMenu(this, view);
        optionMenu.inflate(R.menu.camera_options);
        optionMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getItemId()) {
                case R.id.action_open_image:
                    Intent galleryActivityIntent = new Intent(getBaseContext(),
                            GalleryImageActivity.class);
                    if (intentType != null) {
                        galleryActivityIntent.putExtra(OCRIntentData.KEY_TYPE, intentType);
                        if (intentLocale != null) {
                            galleryActivityIntent.putExtra(OCRIntentData.KEY_LOCALE, intentLocale);
                        }
                    }
                    startActivityForResult(galleryActivityIntent, REQUEST_GALLERY);
                    break;

                case R.id.action_settings:
                    Intent settingsIntent = new Intent(getBaseContext(), SettingsActivity.class);
                    settingsIntent.putExtra(Utils.INTENT_KEY_LOCALE, intentLocale);
                    startActivity(settingsIntent);
                    break;
                }
                return true;
            }
        });
        optionMenu.setOnDismissListener(new OnDismissListener() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void onDismiss(PopupMenu menu) {
                captureUI.resume();
            }
        });
        return optionMenu;
    }

    /**
     * Notification when settings button gets clicked
     *
     * @param view
     *            view on which user clicked
     */
    public void onOptionsClicked(View view) {
        if (activityState == ActivityState.INSTRUCTION) {
            return;
        }

        // cancel if preview mode
        uiHandler.removeCallbacksAndMessages(null);
        captureStop(RESET_REASON_ABORT);

        if (popupMenu == null) {
            popupMenu = createOptionMenu(view);
        }
        popupMenu.show();
    }

    /**
     * This function gets called when user dismisses the instruction dialog by clicking ok button
     *
     * @param view
     *            view on which user clicked
     */
    public void onInstructionDismiss(View view) {
        CheckBox dontShowCheckbox = (CheckBox) findViewById(R.id.dont_show);
        if (dontShowCheckbox.isChecked()) {
            PreferenceData.disableCameraInstruction(this);
        }

        instructionView.setVisibility(View.GONE);
        loadStateUI(ActivityState.ROI_AND_CAPTURE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBackPressed() {
        uiHandler.removeCallbacksAndMessages(null);
        if (activityState == ActivityState.ALGORITHM
                || activityState == ActivityState.IMAGE_CORRECTION) {
            captureStop(RESET_REASON_RESTART);
            return;
        }

        jniManager.clearData();
        super.onBackPressed();
    }

    private void captureStop(int reason) {
        captureUI.pause(reason);
        cameraHandler.stopCapturing();
        resetParsing(reason);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onPause() {
        super.onPause();

        if (popupMenu != null) {
            popupMenu.dismiss();
        }
        wrapCamera();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onResume() {
        super.onResume();

        resetViewVisibility();
        loadStateUI(activityState);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onStart() {
        super.onStart();

        if (activityState == ActivityState.ALGORITHM) {
            Log.d(LOG_TAG, "Changed state: algorithm => ROI");
            activityState = ActivityState.ROI_AND_CAPTURE;
        }
        if (activityState == ActivityState.IMAGE_CORRECTION) {
            String finalImage = Utils.getLocalImageFile(this);
            staBitmap = BitmapFactory.decodeFile(finalImage);
        }
        String previewModeName = getResources()
                .getString(R.string.setting_text_record_preview_mode);
        String textRecordModeName = PreferenceData.getOCRTextRecordMode(this);

        captureType = CaptureType.SNAPSHOT;
        int textRecordMode = JniConstants.PARSER_MODE_SNAPSHOT;
        if (textRecordModeName.equals(previewModeName)) {
            textRecordMode = JniConstants.PARSER_MODE_PREVIEW;
            captureType = previewCaptureType;
        }
        captureUI = CaptureUI.getNewInstance(captureType, this, frameLayout, this,
                getWindowManager().getDefaultDisplay());
        int scveMode = PreferenceData.getScveMode(this);
        initializeOCRParser(scveMode, textRecordMode);
    }

    private void wrapCamera() {
        if (cameraHandler != null) {
            cameraHandler.wrapUp();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_GALLERY) {
            if (resultCode == RESULT_OK) {
                if (data != null && intentType != null) {
                    Log.d(LOG_TAG, "Sending data back to calling activity");
                    setResult(RESULT_OK, data);
                    finish();
                }
            }

        } else {
            handleBaseResults(requestCode, resultCode, data);
        }
    }

    private void loadAndStartAlgorithm(Bitmap roiBitmap) {
        // set UI
        enableIcons(false);
        ((SnapshotUI) captureUI).showCapturedImage(roiBitmap);
        activityState = ActivityState.ALGORITHM;

        // start parsing
        showProgressAnimation(true);
        jniManager.parseImage(roiBitmap);
    }

    private void loadAndStartIC(Bitmap snapshot) {
        // set UI
        enableIcons(false);

        ((SnapshotUI) captureUI).showCapturedImage(snapshot);
        activityState = ActivityState.IMAGE_CORRECTION;

        // start parsing
        showProgressAnimation(true);
        int icRegionExMode = PreferenceData.getICRegionExMode(this);
        int icImageEnMode = PreferenceData.getICImageEnMode(this);
        jniManager.rectifyImage(snapshot, icRegionExMode, icImageEnMode);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSnapshot(Bitmap snapshot) {
        if (!icEnable) {
            captureUI.enableRoi(false, false);
            if (snapshot != null) {
                cleanStaBitmap();
                ViewSize surfaceSize = cameraHandler.getSurfaceSize();
                CoordinateSpace previewSpace = cameraHandler.getPreviewFrameSpace();
                CoordinateSpace pictureSpace = cameraHandler.getPictureFrameSpace();
                float surfaceViewRatio = cameraHandler.getSurfaceViewRatio();

                staBitmap = captureUI.getRoiImage(snapshot, surfaceSize, surfaceViewRatio,
                        previewSpace, pictureSpace);
                if (staBitmap != null) {
                    wrapCamera();
                    Log.d(LOG_TAG,
                            "Cropped image: " + staBitmap.getWidth() + "," + staBitmap.getHeight());
                    loadAndStartAlgorithm(staBitmap);
                    return;
                }
            }
        } else if (snapshot != null) {
            wrapCamera();
            loadAndStartIC(snapshot);
            return;
        }

        // error case
        wrapCamera();
        ApplicationMessage message = ApplicationMessage.CAMERA_SNAPSHOT_FAILED;
        message.setFinishCallback(new Runnable() {
            /**
             * {@inheritDoc}
             */
            @Override
            public void run() {
                uiHandler.removeCallbacksAndMessages(null);
                resetParsing(RESET_REASON_RESTART);
            }
        });
        showApplicationMessage(message);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public FrameSession onFrameAvailable(byte[] data, int imageWidth, int imageHeight) {
        ViewSize surfaceSize = cameraHandler.getSurfaceSize();
        float surfaceViewRatio = cameraHandler.getSurfaceViewRatio();
        RectF previewRect = captureUI.getPreviewRect(surfaceSize, surfaceViewRatio);
        return jniManager.setNextFrame(data, imageWidth, imageHeight, previewRect,
                cameraHandler.getDisplayOrientation());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSTAReady() {// on ocr thread
        runOnUiThread(new Runnable() {
            /**
             * {@inheritDoc}
             */
            public void run() {
                Log.d(LOG_TAG, "STA Ready");
                if (activityState == ActivityState.ROI_AND_CAPTURE) {
                    if (waitForOcrReady) {
                        waitForOcrReady = false;
                        captureUI.parserReady();
                    }
                }
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSTAError(final ApplicationMessage errorMessage) {
        runOnUiThread(new Runnable() {
            /**
             * {@inheritDoc}
             */
            public void run() {
                if (activityState == ActivityState.ALGORITHM
                        || activityState == ActivityState.IMAGE_CORRECTION) {
                    resetProgressAnimation();

                    if (jniManager.getDataList().size() == 0) {
                        jniManager.clearData();
                    }
                    cameraHandler.stopCapturing();

                    if (errorMessage == ApplicationMessage.STA_API_PARSING_TIMEOUT) {
                        restartMessageView.setVisibility(View.VISIBLE);
                        restartMessageView.setOnClickListener(new OnClickListener() {
                            /**
                             * {@inheritDoc}
                             */
                            @Override
                            public void onClick(View v) {
                                resetParsing(RESET_REASON_RESTART);
                                restartMessageView.setVisibility(View.GONE);
                            }
                        });
                        return;
                    }
                    errorMessage.setFinishCallback(new Runnable() {
                        /**
                         * {@inheritDoc}
                         */
                        public void run() {
                            resetParsing(RESET_REASON_RESTART);
                        }
                    });
                }
                showApplicationMessage(errorMessage);
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onAnimationComplete() {// already on ui thread
        if (jniManager.getParserMode() == JniConstants.PARSER_MODE_PREVIEW) {
            // this resolves the black screen issue
            Bitmap lastPreviewFrame = jniManager.getPreviewFrameBitmap();
            if (lastPreviewFrame != null) {
                previewFrameView.setVisibility(View.VISIBLE);
                ((ImageView) previewFrameView).setImageBitmap(lastPreviewFrame);
            }
        }
        super.onAnimationComplete();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSTAParsingInitialized() {
        runOnUiThread(new Runnable() {
            /**
             * {@inheritDoc}
             */
            public void run() {
                cameraHandler.startSendingFrames();
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onImageRectified(final Bitmap bitmap) {
        runOnUiThread(new Runnable() {
            /**
             * {@inheritDoc}
             */
            public void run() {
                staBitmap = bitmap;
                showICScreen();
            }
        });
    }

    private void showICScreen() {
        if (staBitmap != null) {
            ((SnapshotUI) captureUI).showCapturedImage(staBitmap);
            resetProgressAnimation();
            icButtons.setVisibility(View.VISIBLE);
        } else {
            Log.d(LOG_TAG, "Image null for IC correction screen");
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onFrameSessionStarted() {
        Log.d(LOG_TAG, "Session started");
        activityState = ActivityState.ALGORITHM;
        jniManager.startPreviewSession();

        showProgressAnimation(true);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onFrameSessionCancelled() {
        Log.d(LOG_TAG, "Session cancelled");
        activityState = ActivityState.ROI_AND_CAPTURE;
        captureStop(RESET_REASON_ABORT);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onCaptureStart() {
        if (cameraHandler != null) {
            cameraHandler.startCapture(captureType);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onCaptureMessage(ApplicationMessage message) {
        showApplicationMessage(message);
    }

    private void resetParsing(int reason) {
        jniManager.resetParsing(targetDataId);
        if (reason == RESET_REASON_RESTART) {
            Log.d(LOG_TAG, "Reset Parsing reason: Restart");
            resetProgressAnimation();

            resetViewVisibility();
            loadROI();

        } else if (reason == RESET_REASON_ABORT) {
            Log.d(LOG_TAG, "Reset Parsing reason: Abort");
            resetProgressAnimation();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onRoiChanged() {
        cameraHandler.roiChanged();
    }

    private class saveImageTask extends AsyncTask<Void, Void, Void> {
        /**
         * {@inheritDoc}
         */
        @Override
        protected void onPostExecute(Void arg) {
            resetProgressAnimation();
            ApplicationMessage message = ApplicationMessage.IC_IMAGE_SAVED;
            message.setFinishCallback(new Runnable() {
                /**
                 * {@inheritDoc}
                 */
                @Override
                public void run() {
                    cleanStaBitmap();
                    resetParsing(RESET_REASON_RESTART);
                }
            });
            showApplicationMessage(message);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        protected void onPreExecute() {
            showProgressAnimation(true);
        }

        /**
         * {@inheritDoc}
         */
        @Override
        protected Void doInBackground(Void... params) {
            Utils.saveBitmap2Gallery(CameraActivity.this, staBitmap);
            return null;
        }
    }

    /**
     * Notification when user clicked on Save rectified image
     *
     * @param view
     *            view on which user clicked
     */
    public void onIcSave(View view) {
        icButtons.setVisibility(View.GONE);
        if (staBitmap != null) {
            new saveImageTask().execute();
        }
    }

    /**
     * Notification when user clicked on parse button
     *
     * @param view
     *            view on which user clicked
     */
    public void onIcParse(View view) {
        icButtons.setVisibility(View.GONE);
        if (staBitmap != null) {
            activityState = ActivityState.ALGORITHM;
            showProgressAnimation(true);
            jniManager.parseImage(staBitmap);
        }
    }
}
