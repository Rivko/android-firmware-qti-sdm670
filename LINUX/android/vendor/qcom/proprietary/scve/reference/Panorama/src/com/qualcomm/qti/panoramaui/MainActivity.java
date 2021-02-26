/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import android.Manifest;
import android.animation.Animator;
import android.animation.AnimatorInflater;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.PointF;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.transition.TransitionInflater;
import android.transition.TransitionManager;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.panoramaui.CameraPreviewView.OutputFrame;
import com.qualcomm.qti.panoramaui.data.AppError;
import com.qualcomm.qti.panoramaui.data.CaptureDirection;
import com.qualcomm.qti.panoramaui.data.CaptureOrientation;
import com.qualcomm.qti.panoramaui.hardware.CameraActor;
import com.qualcomm.qti.panoramaui.hardware.CameraActorCamera2;
import com.qualcomm.qti.panoramaui.hardware.CameraActorLegacy;
import com.qualcomm.qti.panoramaui.hardware.PanoramaActor;
import com.qualcomm.qti.panoramaui.hardware.SensorActor;
import com.qualcomm.qti.panoramaui.utils.AnimationUtils;
import com.qualcomm.qti.panoramaui.utils.AppConfig;
import com.qualcomm.qti.panoramaui.utils.ImageUtils;

/**
 * Main Activity of the application.
 */
public class MainActivity extends Activity implements SensorActor.RotationListener,
        PanoramaActor.EventListener {
    private static final String LOG_TAG = "MainActivity";
    private static final String TEMPORARY_FILE_NAME = "panorama.jpg";
    private static final int REQUEST_CAMERA_PERMISSION = 104;

    private static final float HIGH_SPEED_THRESHOLD = 0.03f;
    private static final float MEDIUM_SPEED_THRESHOLD = 0.01f;
    private static final float DIAL_ERROR_DEGREES_TO_LEVEL_SCALE = -120;
    private static final int OFFSET_LINE_MAX_ANGLE_DEGREES = 45;

    private enum AppState {
        UNINITIALIZED, INITIALIZED, READY, CAPTURE, PROCESSING, EXIT
    }

    private final class CameraTouchListener implements View.OnTouchListener {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            return gestureDetector.onTouchEvent(event);
        }

        private final GestureDetector gestureDetector = new GestureDetector(MainActivity.this,
                new GestureDetector.SimpleOnGestureListener() {
                    @Override
                    public boolean onDown(MotionEvent e) {
                        return true;
                    }

                    @Override
                    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,
                            float distanceY) {
                        return true;
                    }

                    @Override
                    public boolean onSingleTapUp(MotionEvent e) {
                        PointF touchPos = new PointF(e.getX(), e.getY());
                        cameraActor.setAeAfRegion(touchPos);
                        return true;
                    }
                });
    }

    private AppState applicationState = AppState.UNINITIALIZED;
    private boolean pendingDisplayPanoRequest;

    private SensorActor sensorActor;
    private CameraActor cameraActor;
    private PanoramaActor panoramaActor;

    // Motion parameters
    private float rotationX;
    private float rotationY;
    private float rotationZ;
    private float captureAxisSpeed; // this could be either vertical or horizontal speed
    private float captureReferenceRotationX;
    private float captureReferenceRotationY;
    private CaptureDirection captureDirection;
    private float offsetLineRateX; // display px per degree of rotation around the X axis
    private float offsetLineRateY; // display px per degree of rotation around the Y axis
    private float speedThresholdMed; // preview px change per frame threshold for medium speed
    private float speedThresholdHigh; // preview px change per frame threshold for high speed

    private ViewGroup sensorControlsHorizontal;
    private ViewGroup sensorControlsVertical;
    private View offsetLineView;
    private ImageView sensorDialImageView;
    private ImageView sensorArrowImageView;

    private ViewGroup filmStripContainerHorizontal;
    private ViewGroup filmStripContainerVertical;
    private ImageView filmStripImageView;
    private ImageView miniPreviewImage;
    private FilmStripDrawable filmStripDrawable;

    private TextView displayPanoTipViewHorizontal;
    private TextView displayPanoTipViewVertical;
    private TextView processingTip;

    private CaptureOrientation uiOrientation;
    private CameraPreviewView cameraPreview;
    private ViewGroup cameraPreviewParent;
    private ImageView recordButton;
    private View processingOverlay;

    private AeAfUIHandler aeAfUiHandler;
    private boolean cameraAccessDenied = false;

    /**
     * {@inheritDoc}
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Hide the window title.
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_main);

        Point displaySize = new Point();
        getWindowManager().getDefaultDisplay().getSize(displaySize);
        offsetLineRateX = (float) displaySize.y / (float) (2 * OFFSET_LINE_MAX_ANGLE_DEGREES);
        offsetLineRateY = (float) displaySize.x / (float) (2 * OFFSET_LINE_MAX_ANGLE_DEGREES);

        sensorControlsHorizontal = (ViewGroup) findViewById(R.id.sensorControlsHorizontal);
        sensorControlsVertical = (ViewGroup) findViewById(R.id.sensorControlsVertical);
        filmStripContainerHorizontal = (ViewGroup) findViewById(R.id.filmStripHorizontal);
        filmStripContainerVertical = (ViewGroup) findViewById(R.id.filmStripVertical);
        displayPanoTipViewHorizontal = (TextView) findViewById(R.id.displayPanoTipHorizontal);
        displayPanoTipViewVertical = (TextView) findViewById(R.id.displayPanoTipVertical);
        processingTip = (TextView) findViewById(R.id.processingTip);
        processingOverlay = findViewById(R.id.processingOverlay);
        recordButton = (ImageView) findViewById(R.id.btn_record);
        aeAfUiHandler = new AeAfUIHandler(getResources(),
                (ImageView) findViewById(R.id.camera_preview_focus_indicator), recordButton);

        String version = "";
        try {
            version = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        } catch (NameNotFoundException e) {
            Log.d(LOG_TAG, "Failed to get version");
            e.printStackTrace();
        }

        boolean legacyCamera = false;
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            legacyCamera = true;
        } else if (AppConfig.ZSL_ENABLED) {
            if (!CameraActorCamera2.isZSLSupported(this)) {
                legacyCamera = CameraActorLegacy.probeZSLSupported(this) ? true : false;
                Log.i(LOG_TAG, "ZSL support in legacy: " + legacyCamera);
            } else {
                Log.i(LOG_TAG, "ZSL support in Camera2");
            }
        }

        Log.d(LOG_TAG, "Version-" + version + ", camera2-" + (!legacyCamera) + ", zsl: "
                + AppConfig.ZSL_ENABLED + ", gpu: " + AppConfig.GPU_ENABLED);
        if (!legacyCamera) {
            cameraActor = new CameraActorCamera2(this, aeAfUiHandler);
        } else {
            cameraActor = new CameraActorLegacy(this, aeAfUiHandler);
        }

        sensorActor = new SensorActor(this);

        // These line need to be called before initializedLibraries()
        // If library failed to load, handleError need to be able to remove cameraPreview
        cameraPreview = (CameraPreviewView) findViewById(R.id.camera_preview_view);
        cameraPreviewParent = (ViewGroup) findViewById(R.id.camera_preview);

        if (initializeLibraries()) {
            applicationState = AppState.INITIALIZED;
        }

    }

    /**
     * This method initializes the libraries
     *
     * @return true if initialized correctly, false otherwise
     */
    private boolean initializeLibraries() {
        try {
            System.loadLibrary("scveBlobDescriptor");
            System.loadLibrary("scveCommon");
            if (AppConfig.GPU_ENABLED) {
                System.loadLibrary("scveBlobDescriptor_stub");
                System.loadLibrary("scveCommon_stub");
            }
            System.loadLibrary("scvePanorama");

        } catch (UnsatisfiedLinkError exp) {
            exp.printStackTrace();
            Log.d(LOG_TAG, "Loading libraries failed");
            handleError(AppError.LIBRARY_LOADING_FAILED);
            return false;
        }
        return true;
    }

    /**
     * This method gets fires when user clicks on record button
     *
     * @param view
     *            View which gets clicked
     */
    public void onRecordButtonClick(View view) {
        if (aeAfUiHandler.isTouchToFocusOn()) {
            Log.d(LOG_TAG, "Wait for touch to focus");
            return;
        }
        if (applicationState == AppState.PROCESSING) {
            Log.d(LOG_TAG, "Wait for processing");

            if (uiOrientation == CaptureOrientation.VERTICAL) {
                displayPanoTipViewVertical.setVisibility(View.GONE);
            } else {
                displayPanoTipViewHorizontal.setVisibility(View.GONE);
            }
            processingTip.setVisibility(View.VISIBLE);

        } else if (applicationState != AppState.CAPTURE) {
            startCapture();
        } else {
            completeCapture();
        }
    }

    /**
     * Starts the Image view activity with an intent
     */
    private void startImageActivity() {
        File outputFile = new File(getCacheDir(), TEMPORARY_FILE_NAME);
        if (outputFile.exists()) {
            Intent intent = new Intent(this, PanoramaViewer.class);
            intent.putExtra(PanoramaViewer.EXTRA_PANORAMA_PATH, outputFile.getAbsolutePath());
            startActivity(intent);
            overridePendingTransition(0, 0);
        } else {
            Toast.makeText(this, R.string.panorama_generate_result_failed, Toast.LENGTH_SHORT)
                    .show();
            resetFilmStrip();
        }
    }

    /**
     * This method resets the frames UI recorded earlier
     */
    private void resetFilmStrip() {
        filmStripDrawable.clearThumbnails();
        miniPreviewImage.setVisibility(View.VISIBLE);
        Bitmap placeholder = Bitmap.createBitmap(cameraActor.getWidth(), cameraActor.getHeight(),
                Config.ALPHA_8);
        placeholder.eraseColor(Color.BLACK);
        miniPreviewImage.setImageBitmap(placeholder);
    }
    private boolean checkCameraPermissions() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            Log.d(LOG_TAG, "Requesting camera access");
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA},
                    REQUEST_CAMERA_PERMISSION);
            return false;
        }
        return true;
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            if (grantResults.length == 0 || grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                cameraAccessDenied = true;
            } else {
                Log.i(LOG_TAG, "Camera access granted");
            }
        }
    }
    private void showPermissionError() {
        AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
        dialogBuilder.setTitle(R.string.error_dialog_title);
        dialogBuilder.setMessage(R.string.error_no_camera_access);

        dialogBuilder.setPositiveButton(R.string.btn_ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.dismiss();
                finish();
            }
        });
        Dialog activeDialog = dialogBuilder.create();
        activeDialog.setOnCancelListener(new OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialog) {
                dialog.dismiss();
                finish();
            }
        });
        activeDialog.show();
    }
    /**
     * {@inheritDoc}
     */
    @Override
    protected void onStart() {
        super.onStart();
        cameraAccessDenied = false;
    }
    @Override
    protected void onResume() {
        super.onResume();
        Log.d(LOG_TAG, "onResume");
        boolean permissions = true;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (cameraAccessDenied) {
                Log.d(LOG_TAG, "camera access already denied");
                showPermissionError();
                return;
            }
            permissions = checkCameraPermissions();
        }
        if (permissions) {
            if (applicationState != AppState.EXIT) {
                panoramaActor = new PanoramaActor(cameraActor, sensorActor, cameraPreview);
            }
        if (applicationState == AppState.INITIALIZED) {
            startCamera();
        }

        processingOverlay.setVisibility(View.GONE);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onPause() {
        super.onPause();
        cameraPreview.setOnFrameAvailableListener(null);
        if (applicationState != AppState.UNINITIALIZED && applicationState != AppState.INITIALIZED
                && applicationState != AppState.EXIT) {
            clearCamera();
        }
        processingOverlay.setVisibility(View.GONE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBackPressed() {
        if (applicationState == AppState.CAPTURE) {
            cancelCapture();
        } else {
            super.onBackPressed();
        }
    }

    /**
     * Invoked when the settings button is clicked
     *
     * @param view
     *            {@link View} that was clicked
     */
    public void onSettingsButtonClick(View view) {
        // N.B. Must shut down any active capture and close the camera before invoking settings
        clearCamera();

        Intent intent = new Intent(this, CameraSettingsActivity.class);
        boolean legacyCamera = (cameraActor instanceof CameraActorLegacy);
        intent.putExtra(CameraSettingsActivity.INTENT_KEY_LEGACY_CAMERA, legacyCamera);
        startActivity(intent);
    }

    private void resetSensorControls() {
        captureReferenceRotationX = 0f;
        captureReferenceRotationY = 0f;
        captureDirection = CaptureDirection.NOT_ESTABLISHED;
        updateSensorControls();

        cameraPreview.setOnTouchListener(new CameraTouchListener());
    }

    private void updateSensorControls() {

        int[] sensorArrowState = new int[3];

        // Set the capture feedback state
        sensorArrowState[0] = (applicationState == AppState.CAPTURE ? R.attr.state_capturing : 0);

        switch (captureDirection) {
        case LEFT:
        case DOWN:
            sensorArrowState[1] = R.attr.state_direction_negative;
            break;

        case RIGHT:
        case UP:
            sensorArrowState[1] = R.attr.state_direction_positive;
            break;

        case NOT_ESTABLISHED:
        default:
            sensorArrowState[1] = 0;
            break;
        }

        if (captureAxisSpeed >= speedThresholdHigh) {
            sensorArrowState[2] = R.attr.state_speed_fast;
        } else if (captureAxisSpeed >= speedThresholdMed) {
            sensorArrowState[2] = R.attr.state_speed_medium;
        } else {
            sensorArrowState[2] = R.attr.state_speed_normal;
        }

        sensorArrowImageView.setImageState(sensorArrowState, true);
        AnimationUtils.startAnimationsRecursively(sensorArrowImageView.getDrawable());

        // Set the error bars on the rotation dial and rotate the sensor controls
        int extraDialRotation = (uiOrientation == CaptureOrientation.VERTICAL ? -90 : 0);
        sensorDialImageView.setRotation(rotationZ + extraDialRotation);
        sensorArrowImageView.setRotation(rotationZ + extraDialRotation);
        int dialLevel = (int) (rotationZ * DIAL_ERROR_DEGREES_TO_LEVEL_SCALE) + 5000;
        if (dialLevel > 10000) {
            dialLevel = 10000;
        } else if (dialLevel < 0) {
            dialLevel = 0;
        }
        sensorDialImageView.setImageLevel(dialLevel);

        // Set the position of the angular offset bar
        float rotationDelta;
        switch (captureDirection) {
        case LEFT:
        case RIGHT:
            rotationDelta = rotationX - captureReferenceRotationX;
            if (rotationDelta > 180f) {
                rotationDelta = 360f - rotationDelta;
            } else if (rotationDelta < -180f) {
                rotationDelta = 360f + rotationDelta;
            }
            offsetLineView.setTranslationY(offsetLineRateX * rotationDelta);
            break;

        case UP:
        case DOWN:
            rotationDelta = rotationY - captureReferenceRotationY;
            if (rotationDelta > 180f) {
                rotationDelta = 360f - rotationDelta;
            } else if (rotationDelta < -180f) {
                rotationDelta = 360f + rotationDelta;
            }
            offsetLineView.setTranslationX(offsetLineRateY * rotationDelta);
            break;

        case NOT_ESTABLISHED:
        default:
            offsetLineView.setTranslationX(0);
            offsetLineView.setTranslationY(0);
            rotationDelta = 0f;
            break;
        }
    }

    private void updateUiOrientation(CaptureOrientation newOrientation) {
        if (uiOrientation == newOrientation) {
            return;
        }

        uiOrientation = newOrientation;

        ViewGroup activeSensorControls;
        ViewGroup activeFilmStripContainer;
        ViewGroup inactiveSensorControls;
        ViewGroup inactiveFilmStripContainer;

        if (uiOrientation == CaptureOrientation.VERTICAL) {
            activeSensorControls = sensorControlsVertical;
            activeFilmStripContainer = filmStripContainerVertical;
            inactiveSensorControls = sensorControlsHorizontal;
            inactiveFilmStripContainer = filmStripContainerHorizontal;
        } else {
            activeSensorControls = sensorControlsHorizontal;
            activeFilmStripContainer = filmStripContainerHorizontal;
            inactiveSensorControls = sensorControlsVertical;
            inactiveFilmStripContainer = filmStripContainerVertical;
        }

        TransitionManager.beginDelayedTransition(cameraPreviewParent, TransitionInflater.from(this)
                .inflateTransition(R.transition.orientation_change));
        inactiveSensorControls.setVisibility(View.GONE);
        inactiveFilmStripContainer.setVisibility(View.GONE);
        activeSensorControls.setVisibility(View.VISIBLE);
        activeFilmStripContainer.setVisibility(View.VISIBLE);

        offsetLineView = activeSensorControls.findViewById(R.id.offset_line);
        sensorDialImageView = (ImageView) activeSensorControls.findViewById(R.id.sensor_dial);
        sensorArrowImageView = (ImageView) activeSensorControls.findViewById(R.id.sensor_arrow);

        filmStripImageView = (ImageView) activeFilmStripContainer.findViewById(R.id.filmStrip);
        FilmStripDrawable newFilmStripDrawable = new FilmStripDrawable();
        newFilmStripDrawable.setFrameSize(cameraActor.getWidth(), cameraActor.getHeight());
        if (filmStripDrawable != null) {
            newFilmStripDrawable.copy1stThumbnail(filmStripDrawable, captureDirection);
        }
        filmStripDrawable = newFilmStripDrawable;
        filmStripImageView.setImageDrawable(filmStripDrawable);
        filmStripImageView.setOnClickListener(filmStripClickListener);
        miniPreviewImage = (ImageView) activeFilmStripContainer.findViewById(R.id.miniPreview);
        miniPreviewImage.setVisibility(View.INVISIBLE);

        displayPanoTipViewHorizontal.setVisibility(View.GONE);
        displayPanoTipViewVertical.setVisibility(View.GONE);
    }

    private void showDisplayPanoTip() {
        // N.B. animation resource handles view visibility
        Animator a = AnimatorInflater.loadAnimator(this, R.animator.display_pano_tip);
        a.setTarget(uiOrientation == CaptureOrientation.VERTICAL ? displayPanoTipViewVertical
                : displayPanoTipViewHorizontal);
        a.start();
    }

    @Override
    public void onRotationUpdate(float x, float y, float z) {
        if (applicationState != AppState.EXIT) {
            rotationX = x;
            rotationY = y;
            rotationZ = z;
            updateSensorControls();
        }
    }

    /**
     * Initialize the activity
     */
    private void startCamera() {

        if (!sensorActor.hasRequiredSensors()) {
            Log.d(LOG_TAG, "Sensor Actor intialization failed");
            handleError(AppError.SENSORS_FAILED);
            return;
        }
        sensorActor.registerRotationListener(this);
        sensorActor.start();

        AppError error = cameraActor.startCamera(AppConfig.ZSL_ENABLED);
        if (error != AppError.NONE) {
            handleError(error);
        }

        if (applicationState != AppState.EXIT) {
            // Now that frame size is known, calculate the camera motion thresholds
            int size = Math.max(cameraActor.getWidth(), cameraActor.getHeight());
            speedThresholdMed = MEDIUM_SPEED_THRESHOLD * size;
            speedThresholdHigh = HIGH_SPEED_THRESHOLD * size;

            panoramaActor.init();

            uiOrientation = null;
            updateUiOrientation((getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT) ?
                    CaptureOrientation.HORIZONTAL : CaptureOrientation.VERTICAL);
            resetSensorControls();
            resetFilmStrip();

            cameraPreview.setOnFrameAvailableListener(miniPreviewFrameListener);

            applicationState = AppState.READY;
        }
    }

    /**
     * Clear out the activity
     */
    private void clearCamera() {
        if (applicationState == AppState.CAPTURE) {
            cancelCapture();
        }

        applicationState = AppState.INITIALIZED;

        if (sensorActor != null) {
            sensorActor.stop();
            sensorActor.unregisterRotationListener(this);
        }

        panoramaActor.release();
        cameraActor.wrapUp();
    }

    /**
     * Start a new Panorama capture
     */
    private void startCapture() {
        miniPreviewImage.setVisibility(View.INVISIBLE);
        // Don't clear the CameraPreview.OnFrameAvailableListener here; this could introduce a race
        // where a pending stop/cancel of PanoramaActor could still need preview frames. It will
        // be cleared automatically when required.

        ImageUtils.cleanDebugImagePath();

        applicationState = AppState.CAPTURE;
        captureDirection = CaptureDirection.NOT_ESTABLISHED;
        filmStripDrawable.setCaptureDirection(CaptureDirection.NOT_ESTABLISHED);
        filmStripDrawable.setFrameSize(cameraActor.getWidth(), cameraActor.getHeight());
        filmStripDrawable.clearThumbnails();
        captureReferenceRotationX = rotationX;
        captureReferenceRotationY = rotationY;
        updateSensorControls();

        recordButton.setImageResource(R.drawable.stop_btn);

        cameraPreview.setOnTouchListener(null);
        panoramaActor.startCapture(this);
    }

    /**
     * Completes the ongoing capture process
     */
    private void completeCapture() {
        recordButton.setVisibility(View.GONE);
        showDisplayPanoTip();

        applicationState = AppState.PROCESSING;
        pendingDisplayPanoRequest = false;

        panoramaActor.stopCapture();

        resetSensorControls();
    }

    /**
     * Cancels the ongoing capture process
     */
    private void cancelCapture() {
        recordButton.setImageResource(R.drawable.start_btn);

        panoramaActor.stopCapture(true);

        applicationState = AppState.READY;
        resetSensorControls();
        resetFilmStrip();
    }

    /**
     * Handle errors from various sources, and update the UI/app state as appropriate
     *
     * @param applicationError
     *            {@link AppError} to handle
     */
    private void handleError(final AppError applicationError) {
        if (applicationState == AppState.EXIT) {
            return;
        }

        String message = applicationError.getResourceString(MainActivity.this);
        if (applicationError.getErrorCode() != 0) {
            message = message + " Error: 0x" + Integer.toHexString(applicationError.getErrorCode());
            String errInfo = applicationError.getErrorInfo(this);
            if (errInfo != null) {
                message = message + " - " + errInfo;
            }
        }

        // completeCapture && cancelCapture should be called if the app is in capturing state
        // Otherwise, panoranaActor is in a bad state will throw exception
        if (applicationState == AppState.CAPTURE) {
            if (applicationError.isImageRecoverable()) {
                completeCapture();
            } else {
                cancelCapture();
            }
        }

        if (applicationError.isFatal()) {
            applicationState = AppState.EXIT;
            // Remove the hardwired cameraPreviewView to prevent GL error
            // check crash app when camera failed
            cameraPreviewParent.removeView(cameraPreview);
        }

        // show error dialog
        String dialogTitle = getResources().getString(R.string.error_dialog_title);
        String positiveButton = getResources().getString(R.string.btn_ok);
        AlertDialog.Builder errorDialog = new AlertDialog.Builder(MainActivity.this);
        errorDialog.setTitle(dialogTitle);
        errorDialog.setMessage(message);

        errorDialog.setPositiveButton(positiveButton, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                if (applicationError.isFatal()) {
                    Log.d(LOG_TAG, "Closing application");
                    dialog.dismiss();
                    MainActivity.this.finish();
                    return;
                } else if (applicationError.isImageRecoverable()) {
                    recordButton.setImageResource(R.drawable.start_btn);
                }
            }
        });
        errorDialog.create();
        errorDialog.show();
    }

    /** {@inheritDoc} */
    @Override
    public void onCaptureStarted() {
        cameraActor.onCapture(true);
    }

    @Override
    public void onOrientationEstablished(CaptureOrientation orientation) {
    }
    private boolean handleWrongDirection(CaptureDirection direction) {
        if(uiOrientation == CaptureOrientation.HORIZONTAL) {
            if(direction == CaptureDirection.DOWN || direction == CaptureDirection.UP) {
                cancelCapture();
                showWrongDirectionMessage(R.string.err_portrait_wrong_direction);
                return true;
            }
        } else {
            if(direction == CaptureDirection.LEFT || direction == CaptureDirection.RIGHT) {
                cancelCapture();
                showWrongDirectionMessage(R.string.err_landscape_wrong_direction);
                return true;
            }
        }
        return false;
    }
    private void showWrongDirectionMessage(int messageId) {
        String dialogTitle = getResources().getString(R.string.error_dialog_title);
        String positiveButton = getResources().getString(R.string.btn_ok);
        AlertDialog.Builder errorDialog = new AlertDialog.Builder(MainActivity.this);
        errorDialog.setTitle(dialogTitle);
        errorDialog.setMessage(messageId);

        errorDialog.setPositiveButton(positiveButton, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.dismiss();
            }
        });
        errorDialog.create();
        errorDialog.show();
    }
    /** {@inheritDoc} */
    @Override
    public void onDirectionEstablished(CaptureDirection direction) {
        if (applicationState != AppState.EXIT
                && captureDirection == CaptureDirection.NOT_ESTABLISHED) {
            if (handleWrongDirection(direction)) {
                return;
            }
            // UI orientation should already be updated, but check that it is correct to be safe
            captureDirection = direction;
            updateUiOrientation(direction == CaptureDirection.LEFT
                    || direction == CaptureDirection.RIGHT ? CaptureOrientation.HORIZONTAL
                    : CaptureOrientation.VERTICAL);

            Log.i(LOG_TAG, "Establishing capture direction=" + direction);

            updateSensorControls();
            filmStripDrawable.setCaptureDirection(captureDirection);
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onSpeedUpdate(float speed) {
        if (applicationState != AppState.EXIT) {
            captureAxisSpeed = speed;
            updateSensorControls();
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onKeyFrame(Bitmap bitmap, int positionX, int positionY) {
        if (applicationState != AppState.EXIT && filmStripDrawable != null) {
            filmStripDrawable.addThumbnail(bitmap, -positionX, -positionY);
        }
    }

    /** {@inheritDoc} */
    @Override
    public void onCaptureCompleted(ByteBuffer jpegData) {
        // N.B. Leave the filmstrip displayed; the user will tap it to display the panorama

        new SavePanoramaTask(jpegData.asReadOnlyBuffer())
                .executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);

        recordButton.setImageResource(R.drawable.start_btn);
        recordButton.setVisibility(View.VISIBLE);
        processingTip.setVisibility(View.GONE);
        cameraActor.onCapture(false);
    }

    /** {@inheritDoc} */
    @Override
    public void onCaptureCancelled() {
        // If another capture has already been requested by the time we receive the complete
        // notification, don't restart the mini preview frame listener
        if (applicationState != AppState.CAPTURE) {
            cameraPreview.setOnFrameAvailableListener(miniPreviewFrameListener);
        }

        recordButton.setImageResource(R.drawable.start_btn);
        recordButton.setVisibility(View.VISIBLE);
        processingTip.setVisibility(View.GONE);
        cameraActor.onCapture(false);
    }

    /** {@inheritDoc} */
    @Override
    public void onError(AppError applicationError) {
        handleError(applicationError);
    }

    private final View.OnClickListener filmStripClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (applicationState == AppState.READY) {
                startImageActivity();
            } else if (applicationState == AppState.PROCESSING) {
                Log.i(LOG_TAG, "Request to view panorama while still processing; deferring");
                pendingDisplayPanoRequest = true;
                processingOverlay.setVisibility(View.VISIBLE);
            } else {
                Log.w(LOG_TAG, "Film strip clicked in invalid state: " + applicationState);
            }
        }
    };

    private CameraPreviewView.OnFrameAvailableListener miniPreviewFrameListener = new CameraPreviewView.OnFrameAvailableListener() {
        @Override
        public boolean onFrameAvailable(final OutputFrame frame) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (applicationState == AppState.READY && miniPreviewImage != null) {
                        miniPreviewImage.setImageBitmap(frame.thumb);
                    }
                    frame.sem.release();
                }
            });

            return true;
        }
    };

    private class SavePanoramaTask extends AsyncTask<Void, Void, Void> {
        private final File output = new File(getCacheDir(), TEMPORARY_FILE_NAME);
        private final File outputTmp = new File(getCacheDir(), TEMPORARY_FILE_NAME + ".tmp");
        private final ByteBuffer buffer;

        public SavePanoramaTask(ByteBuffer buffer) {
            this.buffer = buffer;
        }

        @Override
        protected void onPreExecute() {
            output.delete();
            outputTmp.delete();
        };

        @Override
        protected Void doInBackground(Void... params) {

            try {
                FileOutputStream outputStream = new FileOutputStream(outputTmp);
                outputStream.getChannel().write(buffer);
                outputStream.close();
                outputTmp.renameTo(output);
            } catch (IOException e) {
                Log.e(LOG_TAG, "Failed to write temporary panorama file", e);
            }

            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            if (applicationState == AppState.PROCESSING) {
                applicationState = AppState.READY;


                if (pendingDisplayPanoRequest) {
                    pendingDisplayPanoRequest = false;
                    startImageActivity();
                }
            }
        }
    };
}
