/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.sta.mainui.gallery;

import java.io.FileNotFoundException;
import java.io.IOException;

import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;

import com.qualcomm.qti.sta.R;
import com.qualcomm.qti.sta.data.ApplicationMessage;
import com.qualcomm.qti.sta.data.CanvasView;
import com.qualcomm.qti.sta.data.CanvasView.CanvasEventListener;
import com.qualcomm.qti.sta.data.CanvasView.ViewType;
import com.qualcomm.qti.sta.data.OCRIntentData;
import com.qualcomm.qti.sta.data.Utils;
import com.qualcomm.qti.sta.engine.JniConstants;
import com.qualcomm.qti.sta.mainui.CaptureActivity;
import com.qualcomm.qti.sta.settings.PreferenceData;

/**
 * Class for Gallery Image activity
 */
public class GalleryImageActivity extends CaptureActivity implements CanvasEventListener,
        OnTouchListener {
    private static final String LOG_TAG = "GalleryImageActivity";

    private static final String KEY_FILE_PATH = "file_path";
    private static final long MAX_IMAGE_SIZE = (12 * 1024 * 1024); // 12 MB
    private static final int REQUEST_NATIVE_GALLERY = 104;

    /**
     * Enum to track the touch state
     */
    protected enum TouchState {
        NONE, DRAG, ZOOM
    }

    protected float minimumPinchZoom = 0;
    protected TouchState touchState = TouchState.NONE;
    protected PointF startLocation = new PointF();
    protected PointF pinchMidPoint = new PointF();
    protected float previousPinchDistance = 1.0f;

    private enum ActivityState {
        IMAGE_NONE, IMAGE_FETCHED, IMAGE_FETCH_ERROR;
    }

    private ActivityState activityState = ActivityState.IMAGE_NONE;

    private String pathName = null;
    private ImageView imageView = null;

    private CanvasView canvasView = null;
    private GalleryImageDrawable imageDrawable = null;
    private ImageView startRecognizeButton = null;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_gallery_image);

        imageView = (ImageView) findViewById(R.id.image_view);
        startRecognizeButton = (ImageView) findViewById(R.id.btn_start_recognition);
        minimumPinchZoom = getResources().getDimension(R.dimen.min_pinch_limit);

        initializeBaseData();

        // read saved data
        fetchInstanceStateData(savedInstanceState);

        if (intentType != null) {
            Intent intent = getIntent();
            String intentImagePath = intent.getStringExtra(OCRIntentData.KEY_IMAGE);
            if (intentImagePath != null) {
                Log.d(LOG_TAG, "Got path from intent");
                pathName = intentImagePath;
            }
        }

        startRecognizeButton.setVisibility(View.GONE);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);

        savedInstanceState.putString(KEY_FILE_PATH, pathName);
    }

    private void fetchInstanceStateData(Bundle savedInstanceState) {
        if (savedInstanceState == null) {
            return;
        }
        pathName = savedInstanceState.getString(KEY_FILE_PATH);
        Log.d(LOG_TAG, "retrieved: " + pathName);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onResume() {
        super.onResume();

        if (activityState == ActivityState.IMAGE_FETCHED) {
            if (jniManager.isParserInitialized()) {
                startRecognizeButton.setVisibility(View.VISIBLE);
            } else {
                Log.d(LOG_TAG, "Resume: Wait for OCR Ready");
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onStart() {
        super.onStart();

        if (activityState == ActivityState.IMAGE_NONE || staBitmap == null) {
            if (pathName == null) {
                Log.d(LOG_TAG, "No Image, Fetch image first");
                Intent galleryIntent = new Intent(Intent.ACTION_PICK);
                galleryIntent.setType("image/*");
                startActivityForResult(galleryIntent, REQUEST_NATIVE_GALLERY);
            } else {
                Log.d(LOG_TAG, "Load image from path");
                cleanStaBitmap();
                staBitmap = decodeBitmapFromFile(pathName);
                try {
                    fetchImage();
                } catch (IOException e) {
                    e.printStackTrace();
                    activityState = ActivityState.IMAGE_FETCH_ERROR;
                }
            }
        }
        if (activityState == ActivityState.IMAGE_FETCHED) {
            canvasView = new CanvasView(staBitmap.getWidth(), staBitmap.getHeight(), this,
                    ViewType.CENTER);
            imageDrawable = new GalleryImageDrawable(staBitmap, canvasView);

            imageView.setOnTouchListener(this);
            imageView.setImageDrawable(imageDrawable);

            int scveMode = PreferenceData.getScveMode(this);
            initializeOCRParser(scveMode, JniConstants.PARSER_MODE_SNAPSHOT);

        } else if (activityState == ActivityState.IMAGE_FETCH_ERROR) {
            showApplicationMessage(ApplicationMessage.GALLERY_IMAGE_LOAD_FAILED);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        ImageView imageView = (ImageView) view;

        switch (event.getAction() & MotionEvent.ACTION_MASK) {
        case MotionEvent.ACTION_DOWN: {
            startLocation.set(event.getX(), event.getY());
            touchState = TouchState.DRAG;
            Log.d(LOG_TAG, "Steady: cancel callbacks - onTouch");
            uiHandler.removeCallbacksAndMessages(null);
            break;
        }

        case MotionEvent.ACTION_POINTER_DOWN:
            previousPinchDistance = Utils.getSpacing(event);
            if (previousPinchDistance > minimumPinchZoom) {
                Utils.getMidPoint(pinchMidPoint, event);
                canvasView.lockView();
                touchState = TouchState.ZOOM;
            }
            break;

        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_UP:
            if (touchState == TouchState.ZOOM) {
                canvasView.unlockView();
            }
            touchState = TouchState.NONE;
            break;

        case MotionEvent.ACTION_MOVE:
            boolean isMove = false;
            if (touchState == TouchState.DRAG) {
                isMove = canvasView.translateBy(event.getX() - startLocation.x, event.getY()
                        - startLocation.y);
                startLocation.set(event.getX(), event.getY());

            } else if (touchState == TouchState.ZOOM) {
                float newDistance = Utils.getSpacing(event);
                if (newDistance > minimumPinchZoom) {
                    float scale = newDistance / previousPinchDistance;
                    isMove = canvasView.scaleBy(scale, pinchMidPoint.x, pinchMidPoint.y);
                }
            }
            if (isMove) {
                imageView.invalidate();
            }
            break;
        }
        return true;
    }

    private void fetchImage() throws IOException {
        int angle = Utils.getExifRotation(pathName);
        if (angle != 0) {
            Matrix matrix = new Matrix();
            matrix.postRotate(angle);

            Bitmap newBitmap = Bitmap.createBitmap(staBitmap, 0, 0, staBitmap.getWidth(),
                    staBitmap.getHeight(), matrix, true);

            if(staBitmap != newBitmap) {
                staBitmap.recycle();
                staBitmap = newBitmap;
            }
        }
        activityState = ActivityState.IMAGE_FETCHED;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_NATIVE_GALLERY) {
            if (resultCode == RESULT_OK) {
                try {
                    Log.d(LOG_TAG, "Got Image");
                    Uri galleryImageUri = data.getData();
                    pathName = getGalleryPath(galleryImageUri);

                    staBitmap = decodeBitmapFromFile(pathName);
                    fetchImage();

                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                    activityState = ActivityState.IMAGE_FETCH_ERROR;
                } catch (IOException e) {
                    e.printStackTrace();
                    activityState = ActivityState.IMAGE_FETCH_ERROR;
                }
            } else {
                finish();
            }
        } else {
            handleBaseResults(requestCode, resultCode, data);
            if (resultCode == RESULT_OK) {
                if (findMoreCategoryType == null) {
                    finish();
                }
            }
        }
    }

    private static int calculateInSampleSize(BitmapFactory.Options options) {
        int height = options.outHeight;
        int width = options.outWidth;

        long imageSize = height * width;
        int inSampleSize = 1;

        while (imageSize > MAX_IMAGE_SIZE) {
            height /= 2;
            width /= 2;
            imageSize = height * width;

            inSampleSize *= 2;
        }

        return inSampleSize;
    }

    private static Bitmap decodeBitmapFromFile(String pathName) {
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(pathName, options);
        options.inSampleSize = calculateInSampleSize(options);
        Log.d(LOG_TAG, "Big image scaled down by: " + options.inSampleSize);
        options.inJustDecodeBounds = false;
        return BitmapFactory.decodeFile(pathName, options);
    }

    private String getGalleryPath(Uri contentUri) {
        String galleryPath = null;
        Cursor cursor = null;
        try {
            String[] projection = { MediaStore.Images.Media.DATA };
            cursor = getContentResolver().query(contentUri, projection, null, null, null);
            int index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            cursor.moveToFirst();
            galleryPath = cursor.getString(index);
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return galleryPath;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onScaleDone() {
        imageDrawable.invalidateSelf();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onScaleAnimationUpdate() {
        imageDrawable.invalidateSelf();
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
                resetProgressAnimation();
                if (jniManager.getDataList().size() == 0) {
                    jniManager.clearData();
                }
                imageView.setOnTouchListener(GalleryImageActivity.this);
                showApplicationMessage(errorMessage);
            }
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSTAReady() {
        runOnUiThread(new Runnable() {
            /**
             * {@inheritDoc}
             */
            public void run() {
                Log.d(LOG_TAG, "OCR Ready");
                startRecognizeButton.setVisibility(View.VISIBLE);
            }
        });
    }

    /**
     * This method is called when the use clicks on camera button
     *
     * @param view
     *            View on which user clicked
     */
    public void onStartRecognition(View view) {
        imageView.setOnTouchListener(null);
        float width = canvasView.getImageDisplayableWidth();
        float left = canvasView.toImageX(0);
        if (left < 0) {
            width = staBitmap.getWidth();
            left = 0;
        }

        float height = canvasView.getImageDisplayableHeight();
        float top = canvasView.toImageY(0);
        if (top < 0) {
            height = staBitmap.getHeight();
            top = 0;
        }

        Bitmap bitmap = Bitmap.createBitmap(staBitmap, (int) left, (int) top, (int) width,
                (int) height);

        showProgressAnimation(true);
        jniManager.parseImage(bitmap);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onSTAParsingInitialized() {
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onImageRectified(Bitmap bitmap) {
    }
}
