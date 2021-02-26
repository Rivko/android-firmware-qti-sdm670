/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo;

import java.io.File;

import com.qualcomm.qti.objecttracker.aeafcontroller.AeAfController;
import com.qualcomm.qti.objecttracker.cameraengine.CameraManager;
import com.qualcomm.qti.objecttracker.cameraengine.FrameDispatcher;
import com.qualcomm.qti.objecttracker.cameraengine.CameraManager.Mode;
import com.qualcomm.qti.objecttracker.cameraengine.preferences.CameraPreferenceManager;
import com.qualcomm.qti.objecttracker.tracker.ObjectTracker;
import com.qualcomm.qti.objecttrackerdemo.R;
import com.qualcomm.qti.objecttrackerdemo.renderer.GLFrameTranslator;
import com.qualcomm.qti.objecttrackerdemo.renderer.ObjectTrackerRenderer;
import com.qualcomm.qti.objecttrackerdemo.renderer.RendererFactory;
import com.qualcomm.qti.objecttrackerdemo.renderer.SnapshotRenderer;
import com.qualcomm.qti.objecttrackerdemo.renderer.VideoRecorder;
import com.qualcomm.qti.objecttrackerdemo.ui.OnScreenDiagnosticsView;
import com.qualcomm.qti.objecttrackerdemo.ui.RoiContainer;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.provider.BaseColumns;
import android.provider.MediaStore;
import android.util.Log;
import android.view.ContextThemeWrapper;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Toast;

/**
 * This activity presents a simple camera viewfinder interface to the user. It is intended to
 * demonstrate how to use the com.qualcomm.objecttracker.* classes.
 */
@SuppressWarnings("deprecation")
public class CameraPreviewActivity extends Activity {
    private static final String TAG = "CameraPreviewActivity";
    private static final int REQUEST_SETTINGS = 100;
    private static final int HIDE_SNAPSHOT_THUMBNAIL_DELAY_MS = 6000;

    private Handler handler;
    private CameraManager cameraManager;
    private ObjectTracker tracker;
    private GLFrameTranslator rendererFrameSink;
    private RendererFactory rendererFactory;
    private AeAfController aeAfController;
    private Intent mediaLaunchIntent;
    private CaptureMode captureMode;

    private GLSurfaceView previewView;
    private RoiContainer roiContainer;
    private Spinner renderTypeSpinner;
    private ImageButton shutterReleaseButton;
    private ImageView snapshotThumbnail;
    private Spinner switchModeSpinner;
    private MenuItem switchCameraMenuItem;

    private boolean shouldRecreate = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview);

        final Resources res = getResources();

        handler = new Handler();

        // CameraManager manages the state of the camera, and all actions on the camera object
        cameraManager = new CameraManager(this);
        FrameDispatcher frameDispatcher = cameraManager.getFrameDispatcher();

        // ObjectTracker receives frames from the frame dispatcher, and provides an object tracking
        // service based on the contents of those frames
        tracker = new ObjectTracker(CameraPreferenceManager.getScveMode(this),
                CameraPreferenceManager.getScveObjectTrackerPrecision(this));
        frameDispatcher.addFrameSink(tracker);

        // The GLFrameTranslator receives frames from the frame dispatcher, translates them into
        // OpenGL ES textures, and dispatches them to all registered GL frame sinks
        rendererFrameSink = new GLFrameTranslator();
        frameDispatcher.addFrameSink(rendererFrameSink);

        // The ROI container handles regions of interest (ie tracked object indicators)
        roiContainer = (RoiContainer)getFragmentManager().findFragmentById(R.id.roi_container);
        roiContainer.init(tracker);

        // The RendererFactory is responsible for creating appropriate renderers based on app state
        rendererFactory = new RendererFactory(roiContainer);

        // The AeAfController is responsible for using the tracker results to control auto-exposure
        // and auto-focus.
        if (CameraPreferenceManager.isAeAfControllerEnabled(this)) {
            aeAfController = new AeAfController(cameraManager, tracker);
        }

        // The OnScreenDiagnosticsView displays useful diagnostic & debug information
        OnScreenDiagnosticsView osd = (OnScreenDiagnosticsView)findViewById(R.id.osd);
        if (CameraPreferenceManager.isDebugOnScreenDiagnosticsEnabled(this)) {
            osd.attachFrameDispatcher(frameDispatcher);
            if (aeAfController != null) {
                osd.attachAeAfController(aeAfController);
            }
        } else {
            osd.setVisibility(View.GONE);
        }

        // The ObjectTrackerRenderer receives frames from the GLFrameTranslator and displays them
        // in a GLSurfaceView
        previewView = (GLSurfaceView)findViewById(R.id.preview);
        ObjectTrackerRenderer.attachTo(previewView, rendererFrameSink, rendererFactory, tracker);

        renderTypeSpinner = (Spinner)findViewById(R.id.render_type);
        ArrayAdapter<CharSequence> renderTypeAdapter = ArrayAdapter.createFromResource(
                new ContextThemeWrapper(this, R.style.WhiteSpinnerTextTheme),
                R.array.render_mode_selector_entries, android.R.layout.simple_spinner_item);
        renderTypeAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        renderTypeSpinner.setAdapter(renderTypeAdapter);
        final String[] renderTypeValues = res.getStringArray(R.array.render_mode_selector_values);
        renderTypeSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                RendererFactory.Type renderType = RendererFactory.Type.valueOf(
                        renderTypeValues[position]);
                if (renderType != null) {
                    rendererFactory.setRenderType(renderType);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });

        shutterReleaseButton = (ImageButton)findViewById(R.id.shutter_release_button);
        shutterReleaseButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onShutterReleaseClicked();
            }
        });

        snapshotThumbnail = (ImageView)findViewById(R.id.snapshot_thumbnail);
        snapshotThumbnail.setOnClickListener(
                new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onSnapshotThumbnailClicked();
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();

        captureMode = null;
        switchCameraMode(CaptureMode.PHOTO);

        if (!cameraManager.open()) {
            displayCameraOpenFailedDialog();
            return;
        }

        roiContainer.setCameraCoordinateSpace(cameraManager.getPreviewFrameSpace());

        cameraManager.startPreview();
        if (aeAfController != null) {
            aeAfController.start();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();

        if (cameraManager.getMode() != Mode.CLOSED) {
            doStopPreview();
        }
    }

    private void doStopPreview() {
        if (aeAfController != null) {
            aeAfController.stop();
        }
        cameraManager.stopPreview();
        cameraManager.close();
    }

    @Override
    protected void onResume() {
        super.onResume();
        previewView.onResume();
        // ALEX - fixing crash in Android O when recreate gets called before onResume()
        if (shouldRecreate) {
           finish();
           startActivity(new Intent(this, CameraPreviewActivity.class));
           shouldRecreate = false;
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        previewView.onPause();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);

        getMenuInflater().inflate(R.menu.activity_camera_preview_menu, menu);

        switchCameraMenuItem = menu.findItem(R.id.menuitem_switch_camera);

        MenuItem switchMode = menu.findItem(R.id.menuitem_mode_selector);
        switchModeSpinner = (Spinner)switchMode.getActionView();
        TypedArray t = getResources().obtainTypedArray(R.array.camera_mode_selector_drawables);
        Integer[] imageResourceIds = new Integer[t.length()];
        for (int i = 0; i < imageResourceIds.length; i++) {
            imageResourceIds[i] = t.getResourceId(i, 0);
        }
        final String[] actions = getResources().getStringArray(R.array.camera_mode_selector_modes);
        t.recycle();
        switchModeSpinner.setAdapter(new ArrayAdapter<Integer>(this,
                R.layout.view_camera_mode_selector_item, imageResourceIds) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                ImageView view;
                if (convertView == null) {
                    view = (ImageView)getLayoutInflater().inflate(
                            R.layout.view_camera_mode_selector_item, parent, false);
                } else {
                    view = (ImageView)convertView;
                }

                view.setImageResource(getItem(position));

                return view;
            }

            @Override
            public View getDropDownView(int position, View convertView, ViewGroup parent) {
                return getView(position, convertView, parent);
            }
        });
        switchModeSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                CaptureMode mode = CaptureMode.valueOf(actions[position]);
                if (mode != null) {
                    switchCameraMode(mode);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
            }
        });

        // N.B. this is called after start(), so update the UI state as necessary
        updateUiState();

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        boolean handled = true;

        switch (item.getItemId()) {
        case R.id.menuitem_switch_camera:
            onSwitchCameraClicked();
            break;

        case R.id.menuitem_settings:
            if (cameraManager.getMode() != Mode.CLOSED) {
                doStopPreview();
            }

            Intent intent = new Intent(this, CameraSettingsActivity.class);
            startActivityForResult(intent, REQUEST_SETTINGS);
            break;

        default:
            handled = super.onOptionsItemSelected(item);
            break;
        }

        return handled;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
        case REQUEST_SETTINGS:
            // N.B. Must recreate ourselves here, since some members that are initialized in
            // onCreate need to be re-initialized. Defer the recreate until activity result
            // processing is complete.
            /*
            new Handler().post(new Runnable() {
                @Override
                public void run() {
                    recreate();
                }
            });
            break;
            */
           shouldRecreate = true;

        default:
            super.onActivityResult(requestCode, resultCode, data);
        }
    }

    private void onShutterReleaseClicked() {
        Mode mode = cameraManager.getMode();

        if (mode == Mode.CLOSED) {
            return;
        }

        switch (captureMode) {
        case PHOTO:
            boolean snapshotResult = false;

            if (mode == Mode.IDLE) {
                snapshotResult = cameraManager.takeSnapshot(new SnapshotRenderer(this,
                    cameraManager.getPreviewFrameSpace(), cameraManager.getPictureFrameSpace(),
                    getWindowManager().getDefaultDisplay(), rendererFactory, tracker,
                    new SnapshotRenderer.OnSnapshotRenderedListener() {
                        @Override
                        public void onSnapshotRendered(File snapshot) {
                            showMediaThumbnail(snapshot, 5);
                            updateUiState();
                        }

                        @Override
                        public void onSnapshotRenderingFailed() {
                            displaySnapshotFailedToast();
                            updateUiState();
                        }
                    }));
            }

            if (snapshotResult) {
                updateUiState();
            } else {
                displaySnapshotFailedToast();
            }
            break;

        case VIDEO:
            boolean videoRecordResult = false;

            if (mode == Mode.RECORDING_VIDEO) {
                cameraManager.stopVideoRecording();
                videoRecordResult = true;
            } else if (mode == Mode.IDLE) {
                VideoRecorderWrapper wrapper = new VideoRecorderWrapper(new VideoRecorder(this,
                        rendererFactory, tracker, getWindowManager().getDefaultDisplay(),
                        new VideoRecorder.RecordingEventListener() {
                            @Override
                            public void onRecordingStarted() {
                            }

                            @Override
                            public void onRecordingFinished(File video) {
                                showMediaThumbnail(video, 5);
                                updateUiState();
                            }

                            @Override
                            public void onRecordingError() {
                                displayVideoRecordingFailedToast();
                                updateUiState();
                            }
                        }));
                videoRecordResult = cameraManager.startVideoRecording(wrapper);
            }

            if (videoRecordResult) {
                updateUiState();
            } else {
                displayVideoRecordingFailedToast();
            }
            break;

        default:
            throw new RuntimeException("Unexpected camera mode");
        }
    }

    private void onSwitchCameraClicked() {
        int currentCameraId = CameraPreferenceManager.getSelectedCameraId(
                CameraPreviewActivity.this);
        CameraPreferenceManager.setSelectedCameraId(CameraPreviewActivity.this,
                (currentCameraId + 1) % Camera.getNumberOfCameras());
        recreate();
    }

    private void onSnapshotThumbnailClicked() {
        if (mediaLaunchIntent != null) {
            startActivity(mediaLaunchIntent);
        }
    }

    private void showMediaThumbnail(final File media, final int retryCount) {
        if (media == null) {
            Log.w(TAG, "Media content path is null");
            return;
        }

        int mediaId = -1;
        int mediaType = MediaStore.Files.FileColumns.MEDIA_TYPE_NONE;
        String mimeType = "image/*";
        Bitmap thumbnail = null;

        // Load thumbnail for media
        Cursor mediaCursor = getContentResolver().query(MediaStore.Files.getContentUri("external"),
                new String[] { BaseColumns._ID, MediaStore.Files.FileColumns.MEDIA_TYPE,
                    MediaStore.Files.FileColumns.MIME_TYPE },
                MediaStore.MediaColumns.DATA + " = ?",
                new String[] { media.getPath() },
                null);
        if (mediaCursor != null) {
            if (mediaCursor.moveToFirst()) {
                mediaId = mediaCursor.getInt(0);
                mediaType = mediaCursor.getInt(1);
                mimeType = mediaCursor.getString(2);
            }
            mediaCursor.close();
        }

        if (mediaId != -1) {
            switch (mediaType) {
            case MediaStore.Files.FileColumns.MEDIA_TYPE_IMAGE:
                thumbnail = MediaStore.Images.Thumbnails.getThumbnail(getContentResolver(),
                        mediaId, MediaStore.Images.Thumbnails.MINI_KIND, null);
                break;

            case MediaStore.Files.FileColumns.MEDIA_TYPE_VIDEO:
                thumbnail = MediaStore.Video.Thumbnails.getThumbnail(getContentResolver(),
                        mediaId, MediaStore.Video.Thumbnails.MINI_KIND, null);
                break;

            default:
                Log.w(TAG, "File '" + media.getPath() + "' is of unsupported type for thumbnail");
                break;
            }
        } else if (retryCount > 0) {
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    showMediaThumbnail(media, retryCount - 1);
                }
            }, 100);
            return; // N.B. Try again in a little while
        } else {
            Log.w(TAG, "File '" + media.getPath() + "' not found in media store");
        }

        snapshotThumbnail.setVisibility(View.VISIBLE);
        snapshotThumbnail.setImageBitmap(thumbnail);
        handler.removeCallbacks(hideMediaThumbnail);
        handler.postDelayed(hideMediaThumbnail, HIDE_SNAPSHOT_THUMBNAIL_DELAY_MS);

        mediaLaunchIntent = new Intent(Intent.ACTION_VIEW);
        mediaLaunchIntent.setDataAndType(Uri.fromFile(media), mimeType);
    }

    private final Runnable hideMediaThumbnail = new Runnable() {
        @Override
        public void run() {
            snapshotThumbnail.setVisibility(View.GONE);
        }
    };

    private void updateUiState() {
        Mode mode = cameraManager.getMode();
        boolean isRecording = (mode == Mode.RECORDING_VIDEO);

        shutterReleaseButton.setEnabled(cameraManager.getMode() != Mode.TAKING_SNAPSHOT);
        renderTypeSpinner.setEnabled(!isRecording);
        if (switchModeSpinner != null) { // created by onCreateOptionsMenu, may not exist right away
            switchModeSpinner.setEnabled(!isRecording);
        }
        if (switchCameraMenuItem != null) {
            switchCameraMenuItem.setEnabled(!isRecording);
        }

        setRequestedOrientation(isRecording ? ActivityInfo.SCREEN_ORIENTATION_LOCKED :
                ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);

        if (captureMode == CaptureMode.VIDEO) {
             if (isRecording) {
                 shutterReleaseButton.setImageState(
                         new int[] { R.attr.state_record_mode, R.attr.state_recording }, true);
             } else {
                 shutterReleaseButton.setImageState(new int[] { R.attr.state_record_mode }, true);
             }
        } else {
            shutterReleaseButton.setImageState(null, true);
        }
    }

    private void switchCameraMode(CaptureMode newMode) {
        if (newMode != captureMode) {
            captureMode = newMode;
            updateUiState();
        }
    }

    private void displayCameraOpenFailedDialog() {
        new AlertDialog.Builder(this)
                .setIconAttribute(android.R.attr.alertDialogIcon)
                .setTitle(android.R.string.dialog_alert_title)
                .setMessage(R.string.open_camera_failed)
                .setPositiveButton(android.R.string.ok, null)
                .setOnDismissListener(new DialogInterface.OnDismissListener() {
                    @Override
                    public void onDismiss(DialogInterface dialog) {
                        CameraPreviewActivity.this.finish();
                    }
                }).show();
    }

    private void displaySnapshotFailedToast() {
        Toast.makeText(this, R.string.snapshot_failed, Toast.LENGTH_SHORT).show();
    }

    private void displayVideoRecordingFailedToast() {
        Toast.makeText(this, R.string.video_recording_failed, Toast.LENGTH_SHORT).show();
    }

    private enum CaptureMode {
        PHOTO,
        VIDEO
    }

    private class VideoRecorderWrapper implements CameraManager.VideoRecorderControl {
        private final VideoRecorder recorder;

        public VideoRecorderWrapper(VideoRecorder recorder) {
            this.recorder = recorder;
        }

        @Override
        public void startRecording() {
            rendererFrameSink.addGLFrameSink(recorder);
            recorder.startRecording();
        }

        @Override
        public void stopRecording() {
            recorder.stopRecording();
            rendererFrameSink.removeGLFrameSink(recorder);
        }
    }
}
