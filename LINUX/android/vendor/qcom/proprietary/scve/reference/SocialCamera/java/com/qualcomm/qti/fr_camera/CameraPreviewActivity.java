/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetFileDescriptor;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.SurfaceTexture;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.media.ImageNV21;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.TextureView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.fr_camera.camera.CameraActor;
import com.qualcomm.qti.fr_camera.camera.JpegPictureWriter;
import com.qualcomm.qti.fr_camera.camera.preferences.CameraPreferenceManager;
import com.qualcomm.qti.fr_camera.camera.preferences.CameraSelectionManager;
import com.qualcomm.qti.fr_camera.camera.preferences.FlashMode;
import com.qualcomm.qti.fr_camera.coordinate.CoordinateSpace;
import com.qualcomm.qti.fr_camera.coordinate.CoordinateTranslator;
import com.qualcomm.qti.fr_camera.coordinate.Orientation;
import com.qualcomm.qti.fr_camera.fr.Face;
import com.qualcomm.qti.fr_camera.fr.FaceRecognizerWorker;
import com.qualcomm.qti.fr_camera.fr.UserDatabase;
import com.qualcomm.qti.fr_camera.ui.AutoFocusIndicatorController;
import com.qualcomm.qti.fr_camera.ui.FaceLayoutManager;
import com.qualcomm.qti.fr_camera.ui.MaskDrawable;
import com.qualcomm.qti.fr_camera.ui.ShutterReactionOverlayView;

import java.io.File;
import java.io.FileDescriptor;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * This {@link Activity} provides Camera preview and picture capture capabilities
 */
public class CameraPreviewActivity extends Activity {
    private static final String TAG = "SocialCamera";

    private TextureView previewTextureView;
    private ImageButton toggleFlashButton;
    private ImageButton shutterButton;
    private TextView faceCountView;
    private Button quickSendButton;
    private ImageView reviewImageView;
    private ShutterReactionOverlayView shutterReactionOverlay;

    private boolean active;
    private CameraPreferenceManager cameraPreferenceManager;
    private AutoFocusIndicatorController autoFocusIndicatorController;
    private FaceRecognizerWorker faceRecognizer;
    private FaceLayoutManager faceLayoutManager;
    private FaceLayoutManager.SelectedUser[] reviewImageUsers;
    private Uri reviewImageUri;

    // Shared camera state fields
    private String cameraId;
    private CameraActor cameraActor;
    private SurfaceTexture previewSurfaceTexture;
    private CoordinateSpace displaySpace;
    private CoordinateTranslator displayToPreviewTranslator;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Disable the rotation animation
        Window window = getWindow();
        WindowManager.LayoutParams wlp = window.getAttributes();
        wlp.rotationAnimation = WindowManager.LayoutParams.ROTATION_ANIMATION_CROSSFADE;
        window.setAttributes(wlp);

        setupActivityContent();
    }

    @Override
    protected void onResume() {
        super.onResume();

        active = true;

        cameraId = new CameraSelectionManager(this).getCameraId();

        cameraPreferenceManager = new CameraPreferenceManager(this, cameraId);
        cameraPreferenceManager.setEventListener(cameraPreferenceManagerEventListener);

        clearReviewImage();
        reviewImageUsers = null;
        updateCameraUiElements();

        if (previewSurfaceTexture != null) {
            startPreview();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        active = false;

        if (cameraActor != null) {
            stopPreview();
            cameraActor.close();
            cameraActor = null;
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        if (cameraActor != null) {
            stopPreview();
        }

        setupActivityContent();

        updateCameraUiElements();

        // Camera preview will be restarted by onSurfaceTextureAvailable
    }

    private void setupActivityContent() {
        setContentView(R.layout.activity_camera_preview);

        previewTextureView = (TextureView)findViewById(R.id.camera_preview_texture);
        previewTextureView.setSurfaceTextureListener(previewSurfaceTextureListener);
        previewTextureView.setOnTouchListener(new PreviewTextureViewTouchListener());
        previewTextureView.setOpaque(false);

        findViewById(R.id.camera_preview_settings).setOnClickListener(onSettingsClickedListener);

        shutterButton = (ImageButton)findViewById(R.id.camera_preview_shutter_release);
        shutterButton.setOnClickListener(onShutterClickedListener);

        faceCountView = (TextView)findViewById(R.id.camera_preview_face_count);

        quickSendButton = (Button)findViewById(R.id.camera_preview_send);
        quickSendButton.setOnClickListener(onSendClickedListener);

        toggleFlashButton = (ImageButton)findViewById(R.id.camera_preview_flash_toggle);
        toggleFlashButton.setOnClickListener(onToggleFlashClickedListener);

        shutterReactionOverlay = (ShutterReactionOverlayView)findViewById(
                R.id.camera_preview_shutter_reaction_overlay);

        autoFocusIndicatorController = new AutoFocusIndicatorController(
                (ImageView)findViewById(R.id.camera_preview_focus_indicator));

        reviewImageView = (ImageView)findViewById(R.id.camera_preview_review);
        reviewImageView.setOnClickListener(onReviewImageClickedListener);

        faceLayoutManager = new CameraPreviewFaceLayoutManager(this,
                (FrameLayout)findViewById(R.id.camera_preview_face_container));
    }

    private void updateCameraUiElements() {
        FlashMode flashMode = cameraPreferenceManager.getFlashMode();
        if (flashMode == FlashMode.NOT_SUPPORTED) {
            toggleFlashButton.setVisibility(View.GONE);
        } else {
            toggleFlashButton.setVisibility(View.VISIBLE);

            switch (flashMode) {
                case ON:
                    toggleFlashButton.setImageState(new int[] { R.attr.state_flash_on }, true);
                    break;

                case AUTO:
                    toggleFlashButton.setImageState(new int[] { R.attr.state_flash_auto }, true);
                    break;

                default:
                    toggleFlashButton.setImageState(null, true);
                    break;
            }
        }

        reviewImageView.setVisibility(reviewImageUri != null ? View.VISIBLE : View.GONE);

        if (reviewImageUri != null) {
            if (reviewImageUsers.length > 0) {
                faceCountView.setVisibility(View.GONE);
                quickSendButton.setVisibility(View.VISIBLE);
                quickSendButton.setText(getString(R.string.camera_preview_send_faces,
                        reviewImageUsers.length));
            } else {
                faceCountView.setVisibility(View.VISIBLE);
                quickSendButton.setVisibility(View.GONE);
            }
        } else {
            faceCountView.setVisibility(View.VISIBLE);
            quickSendButton.setVisibility(View.GONE);
        }
    }

    // N.B. reviewImageUsers must be set before calling this method
    private void setReviewImage(Uri uri) {
        reviewImageUri = uri;

        Drawable d;
        try (
            AssetFileDescriptor fd = getContentResolver().openAssetFileDescriptor(
                    reviewImageUri, "r")
        ) {
            assert fd != null;
            d = createReviewDrawable(fd.getFileDescriptor());
        } catch (IOException e) {
            Log.e(TAG, "Failed loading drawable from [" + reviewImageUri + "]", e);
            d = null;
        }

        reviewImageView.setImageDrawable(d);
        reviewImageView.setImageLevel(reviewImageUsers.length);
    }

    private void clearReviewImage() {
        reviewImageUri = null;
        reviewImageView.setImageDrawable(null);
    }

    private Drawable createReviewDrawable(FileDescriptor fd) {
        Resources res = getResources();
        final int targetSize = res.getDimensionPixelSize(R.dimen.camera_preview_review_size);

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFileDescriptor(fd, null, options);

        int minDimension = Math.min(options.outWidth, options.outHeight);
        options.inJustDecodeBounds = false;
        options.inSampleSize = 1;
        while (minDimension > 2 * targetSize) {
            minDimension /= 2;
            options.inSampleSize *= 2;
        }

        Bitmap review = BitmapFactory.decodeFileDescriptor(fd, null, options);
        if (review == null) {
            return null;
        }

        BitmapDrawable reviewDrawable = new BitmapDrawable(res, review);
        reviewDrawable.setFilterBitmap(true);

        LayerDrawable ld = (LayerDrawable)getDrawable(R.drawable.camera_preview_review);
        //noinspection ConstantConditions
        ld.mutate();
        //noinspection ConstantConditions
        ld.setDrawableByLayerId(R.id.camera_preview_review_masked_image,
                new MaskDrawable(getDrawable(R.drawable.camera_preview_review_mask),
                        reviewDrawable));

        return ld;
    }

    private void startPreview() {
        if (cameraActor == null) {
            cameraActor = new CameraActor(this);
            cameraActor.open(cameraId);
        } else {
            stopPreview();
        }

        cameraActor.setPreviewSurfaceTexture(previewSurfaceTexture);
        cameraActor.setRequestedPreviewCoordinateSpace(displaySpace);
        cameraActor.setEventListener(cameraActorEventListener);
        cameraActor.setAutoFocusEventListener(autoFocusEventListener);
        cameraActor.setPreviewCaptureCallback(previewFrameCaptureCallback);

        faceRecognizer = new FaceRecognizerWorker(this, faceRecognizerListener);

        cameraActor.startPreview();

        autoFocusIndicatorController.autoFocusPosition(new PointF(displaySpace.width / 2f,
                displaySpace.height / 2f));
    }

    private void stopPreview() {
        if (cameraActor != null) {
            cameraActor.stopPreview();
        }

        if (faceRecognizer != null) {
            faceRecognizer.stop();
            faceRecognizer = null;
        }

        faceLayoutManager.reset();
        faceCountView.setText(null);
    }

    private final View.OnClickListener onSettingsClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            startActivity(new Intent(CameraPreviewActivity.this, CameraSettingsActivity.class));
        }
    };

    private final View.OnClickListener onToggleFlashClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            cameraPreferenceManager.toggleFlashMode();
        }
    };

    private final View.OnClickListener onShutterClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            shutterButton.setEnabled(false);
            reviewImageUsers = faceLayoutManager.getSelectedUsers();
            cameraActor.takePicture(stillImageCaptureCallback);
        }
    };

    private final View.OnClickListener onSendClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            SharePicture.send(CameraPreviewActivity.this, reviewImageUri, reviewImageUsers);
            clearReviewImage();
            reviewImageUsers = null;
            updateCameraUiElements();
        }
    };

    private final View.OnClickListener onReviewImageClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (reviewImageUri == null) {
                Log.w(TAG, "No review image URI, ignoring click");
                return;
            }

            Intent intent = new Intent(CameraPreviewActivity.this, PictureReviewActivity.class);
            intent.setData(reviewImageUri);
            intent.putParcelableArrayListExtra("Faces", faceLayoutManager.getFaces());
            intent.putExtra("PreviewResolutionWidth", cameraActor.getPreviewResolution().getWidth());
            intent.putExtra("PreviewResolutionHeight", cameraActor.getPreviewResolution().getHeight());
            intent.putExtra("PictureResolutionWidth", cameraActor.getPictureResolution().getWidth());
            intent.putExtra("PictureResolutionHeight", cameraActor.getPictureResolution().getHeight());
            intent.putExtra("IsPortrait", cameraActor.isPortrait());
            startActivity(intent);

            clearReviewImage();
            updateCameraUiElements();
        }
    };

    private final TextureView.SurfaceTextureListener previewSurfaceTextureListener =
            new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            previewSurfaceTexture = surface;

            displaySpace = CoordinateSpace.forDisplay(width, height,
                    getWindowManager().getDefaultDisplay());

            if (active) {
                startPreview();
            }
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            onSurfaceTextureAvailable(surface, width, height);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            previewSurfaceTexture = null;
            return true;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        }
    };

    private final CameraActor.EventListener cameraActorEventListener =
            new CameraActor.EventListener() {
        private CoordinateSpace previewSpace;
        private Orientation surfaceTextureOrientation;

        @Override
        public void onCameraError(int error) {
            Toast.makeText(CameraPreviewActivity.this, getResources().getString(
                            R.string.camera_preview_error_camera_error, Integer.toString(error)),
                    Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onCameraDisconnected() {
            Toast.makeText(CameraPreviewActivity.this,
                    R.string.camera_preview_error_camera_disconnected, Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onPreviewConfigured(CoordinateSpace previewSpace,
                                        Orientation surfaceTextureOrientation) {
            this.previewSpace = previewSpace;
            this.surfaceTextureOrientation = surfaceTextureOrientation;
            displayToPreviewTranslator = new CoordinateTranslator(displaySpace, previewSpace);
            calculatePreviewTextureViewTransform();
            faceLayoutManager.setCoordinateTranslator(new CoordinateTranslator(
                    previewSpace, displaySpace));

        }

        // Calculate a transform to map the preview SurfaceTexture onto the TextureView. By
        // default, the TextureView already applies a transform that scales the SurfaceTexture
        // to fill its bounds. Apply an additional transform for rotation (if required) and
        // aspect ratio correction.
        private void calculatePreviewTextureViewTransform() {
            Orientation relativeOrientation = surfaceTextureOrientation.subtract(
                    displaySpace.orientation);

            Matrix transform = new Matrix();
            transform.setTranslate(-displaySpace.width / 2f, -displaySpace.height / 2f);

            transform.postRotate(relativeOrientation.degrees);
            if (relativeOrientation.isRightAngle) {
                transform.postScale((float)displaySpace.width / (float)displaySpace.height,
                        (float)displaySpace.height / (float)displaySpace.width);
            }

            if (previewSpace.aspect >= displaySpace.aspect) {
                transform.postScale(1f, displaySpace.aspect / previewSpace.aspect);
            } else {
                transform.postScale(previewSpace.aspect / displaySpace.aspect, 1f);
            }

            transform.postTranslate(displaySpace.width / 2f, displaySpace.height / 2f);
            previewTextureView.setTransform(transform);
        }
    };

    private final class PreviewTextureViewTouchListener implements View.OnTouchListener {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            return gestureDetector.onTouchEvent(event);
        }

        private final GestureDetector gestureDetector = new GestureDetector(
                CameraPreviewActivity.this, new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onDown(MotionEvent e) {
                return true;
            }

            @Override
            public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
                return true;
            }

            @Override
            public boolean onSingleTapUp(MotionEvent e) {
                PointF touchPos = new PointF(e.getX(), e.getY());

                autoFocusIndicatorController.autoFocusPosition(touchPos);

                displayToPreviewTranslator.mapPoint(touchPos, touchPos);
                cameraActor.setAeAfRegion(new RectF(touchPos.x, touchPos.y,
                        touchPos.x, touchPos.y));
                return true;
            }
        });
    }

    private CameraActor.AutoFocusEventListener autoFocusEventListener =
            new CameraActor.AutoFocusEventListener() {
        @Override
        public void onAutoFocusStarted() {
            autoFocusIndicatorController.autoFocusStarted();
        }

        @Override
        public void onAutoFocusCancelled() {
            autoFocusIndicatorController.autoFocusCancelled();
        }

        @Override
        public void onAutoFocusComplete() {
            autoFocusIndicatorController.autoFocusComplete(true);
        }

        @Override
        public void onAutoFocusFailed() {
            autoFocusIndicatorController.autoFocusComplete(false);
        }
    };

    private final CameraPreferenceManager.EventListener cameraPreferenceManagerEventListener =
            new CameraPreferenceManager.EventListener() {
        @Override
        public void onCaptureSessionPreferenceChanged() {
            updateCameraUiElements();
        }

        @Override
        public void onPictureCaptureRequestPreferenceChanged() {
            updateCameraUiElements();
        }

        @Override
        public void onCaptureRequestPreferenceChanged() {
            updateCameraUiElements();
        }
    };

    private final CameraActor.PreviewFrameCaptureCallback previewFrameCaptureCallback =
            new CameraActor.PreviewFrameCaptureCallback() {
        @Override
        public void onCaptured(ImageNV21 image) {
            if (!active) {
                Log.w(TAG, "Received preview frame when not active; discarding");
                image.close();
                return;
            }
            faceRecognizer.process(image, true);
        }
    };

    private final FaceRecognizerWorker.EventListener faceRecognizerListener =
            new FaceRecognizerWorker.EventListener() {
        @Override
        public void onFaceRecognitionResults(Face[] faces, boolean wasRecognitionRun) {
            if (!active) {
                Log.w(TAG, "Received recognition results when not running; discarding");
                return;
            }

            faceLayoutManager.update(faces);
        }
    };

    private final CameraActor.StillImageCaptureCallback stillImageCaptureCallback =
            new CameraActor.StillImageCaptureCallback() {
        @Override
        public void onCaptureAborted() {
            shutterButton.setEnabled(true);
            Toast.makeText(CameraPreviewActivity.this,
                    R.string.camera_preview_error_still_image_capture_failed, Toast.LENGTH_SHORT)
                    .show();
        }

        @Override
        public void onCaptureStarting() {
            shutterReactionOverlay.doShutterReaction();
        }

        @Override
        public void onCaptured(ByteBuffer jpeg, boolean isFacingFront) {
            shutterButton.setEnabled(true);
            new PictureWriter(CameraPreviewActivity.this, jpeg, isFacingFront);
        }

        @Override
        public void onCaptureFailed() {
            shutterButton.setEnabled(true);
            Toast.makeText(CameraPreviewActivity.this,
                    R.string.camera_preview_error_still_image_capture_failed, Toast.LENGTH_SHORT)
                    .show();
        }
    };

    private class CameraPreviewFaceLayoutManager extends FaceLayoutManager {
        public CameraPreviewFaceLayoutManager(Context context, FrameLayout layout) {
            super(layout, Mode.PREVIEW, UserDatabase.getInstance(context));
        }

        @Override
        protected void onSelectedFacesChanged() {
            final int recognizedFaceCount = faceLayoutManager.getNumSelectedUsers();
            faceCountView.setText(recognizedFaceCount > 0 ?
                    Integer.toString(recognizedFaceCount) : null);
        }
    }

    private class PictureWriter extends JpegPictureWriter {
        public PictureWriter(Context context, ByteBuffer jpegData, boolean horizontalInvert) {
            super(context, jpegData, horizontalInvert);
        }

        @Override
        public void onWriteComplete(File path, Uri uri) {
            setReviewImage(uri);
            updateCameraUiElements();
        }

        @Override
        public void onWriteFailed() {
            Toast.makeText(CameraPreviewActivity.this,
                    R.string.camera_preview_error_still_image_save_failed, Toast.LENGTH_SHORT)
                    .show();
        }
    }
}
