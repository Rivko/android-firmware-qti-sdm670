/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercaptureui;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Vector;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.PointF;
import android.graphics.RectF;
import android.graphics.drawable.AnimationDrawable;
import android.graphics.drawable.Drawable;
import android.media.MediaScannerConnection;
import android.media.MediaScannerConnection.MediaScannerConnectionClient;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.qualcomm.qti.clevercaptureui.R;
import com.qualcomm.qti.clevercapture.data.AppError;
import com.qualcomm.qti.clevercapture.hardware.CameraActor;
import com.qualcomm.qti.clevercapture.hardware.CameraActorCamera2;
import com.qualcomm.qti.clevercapture.hardware.CameraActorLegacy;
import com.qualcomm.qti.clevercapture.hardware.SensorFusion;
import com.qualcomm.qti.clevercapture.hardware.CleverCaptureActor;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceManager;
import com.qualcomm.qti.clevercaptureui.utils.Fps;
import com.qualcomm.qti.clevercaptureui.utils.ImageUtils;
import com.qualcomm.qti.clevercaptureui.utils.LogFileDump;
import com.qualcomm.qti.clevercaptureui.utils.Utils;

/**
 * Main Activity of the application.
 */
public class MainActivity extends Activity implements CleverCaptureActor.EventListener,
                                           CameraActor.OnKeyFrameCapturedListener,
                                           MediaScannerConnectionClient, OnTouchListener, OnClickListener {
    private static final String LOG_TAG = "MainActivity";

    private enum AppState {
        UNINITIALIZED, INITIALIZED, READY, CAPTURE, EXIT
    }

    private AppState applicationState = AppState.UNINITIALIZED;
    private CameraActor cameraActor;
    private CleverCaptureActor clevercaptureActor;
    private CameraPreviewView cameraPreview;

    public LogFileDump mLogFileDump = new LogFileDump();
    public String mBSDumpFolder = "/sdcard/MV_dump";

    private static final String ROOT_FOLDER = "/sdcard/Pictures/clevercapture/";
    private String picFolder;
    private String picName;
    private int burstIdx;
    private int burstNum;
    private int burstInterval;
    private int burstIntervalIdx;
    private int picNum;
    private int picNumTotal;
    private Vector<String> picPaths = new Vector<String>();
    private int picNumScanned;
    private boolean isPicturing;
    private boolean isCapture;

    private MediaScannerConnection mConnection;
    private SensorFusion mSensorFusion;
    private static boolean mSensorFusionInitialized = false;
    private volatile boolean mSensorHighMotion = false;

    private View mRoiView;
    private ROIDrawable mRoiDrawable;
    private boolean mRoiEnabled = false;
    private boolean mRoiRectUpdated = false;
    private int mScreenWidth;
    private int mScreenWidthDp;
    private int mScreenHeight;
    private int mScreenHeightDp;
    private enum TouchState {
        NONE, MOVE, DRAG, ZOOM, PINCH
    }
    private TouchState touchState = TouchState.NONE;
    private PointF startLocation = new PointF();
    private PointF moveFromLocation = new PointF();
    private float previousPinchDistance = 1.0f;
    private PointF pinchMidPoint = new PointF();
    private float minimumPinchZoom = 0;

    private int mThumbViewMaxPx = 200;
    private TextView mTextViewPicNum;
    private ImageView mThumbView_1;
    private ImageView mThumbView_2;
    private ImageView mThumbView_3;
    private ImageView mImageViewTranslate;
    private AnimatorSet mCaptureAnimator;
    public static final int WIDTH_THUMB = 640;
    public static final int HEIGHT_THUMB = 480;
    public static final int SHRINK_DURATION = 400;
    public static final int HOLD_DURATION = 2500;
    public static final int SLIDE_DURATION = 1100;
    private boolean mAnimation = false;
    private Bitmap mAnimationBitmap_1;
    private Bitmap mAnimationBitmap_2;
    private Bitmap mAnimationBitmap_3;

    private ImageView mArrowLeft;
    private ImageView mArrowRight;
    private ImageView mDivider;
    private RelativeLayout mTopper;

    private static final int MAX_THUMB_VIEW_NUM = 10;
    private static final int ALBUM_PAD_PX = 40;
    private static final int THUMB_ALL_PAD_PX = 6;
    private static final int THUMB_RIGHT_PAD_PX = 20;

    private int mThumbViewWidth;
    private int mThumbViewHeight;
    private int mArrowRightWidthPx;
    private int mArrowLeftWidthPx;
    private int mDividerWidthPx;
    private int mThumbViewMaxNum;
    private int mAlbumPadPx;
    private int mThumbViewIndex;

    private View[] mThumbViews;
    private ImageView[] mDividerViews;

    private boolean mLayoutNewCapture;
    private int mThumbViewBitmapNum;
    private int mThumbViewBitmapNumCur;
    private Bitmap[] mThumbViewBitmaps;
    private int[] mAlbumPicCounts;
    private String[] mAlbumFolders;
    private AnimationDrawable mAnimationDrawable;

    private TextView mFpsView;
    private Fps mFps = new Fps(new Fps.Listener() {
        @Override
        public void refreshUI(float fps) {
            Message msg = mHandler.obtainMessage(MSG_UPDATE_FPS, (int)(fps * 10), 0);
            mHandler.sendMessage(msg);
        }
    });

    public static final int MSG_TAKEPICTURE = 0xF001;
    public static final int MSG_UPDATE_FPS  = 0xF002;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch(msg.what) {
            case MSG_TAKEPICTURE:
                cameraActor.captureKeyFrame(MainActivity.this);
                break;
            case MSG_UPDATE_FPS:
                float fps_value = msg.arg1 / 10f;
                mFpsView.setText(String.format("%.1f", fps_value));
                break;
            }
            super.handleMessage(msg);
        }
    };

    /**
     * {@inheritDoc}
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(LOG_TAG, "onCreate");
        // Hide the window title.
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        if(getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
            setContentView(R.layout.activity_main);
        } else {
            setContentView(R.layout.activity_main_portrait);
        }

        if (initializeLibraries()) {
            applicationState = AppState.INITIALIZED;
        }

        cameraPreview = (CameraPreviewView)findViewById(R.id.camera_preview_view);
        cameraPreview.setOnTouchListener(this);
        cameraPreview.setFps(mFps);

        minimumPinchZoom = getResources().getDimension(R.dimen.min_pinch_limit);
        mRoiView = findViewById(R.id.roi_view);
        mRoiDrawable = new ROIDrawable(getResources());
        ((ImageView) mRoiView).setImageDrawable(mRoiDrawable);
        mRoiView.setVisibility(View.GONE);

        mFpsView = (TextView) findViewById(R.id.text_fps);

        updateScreenSize();

        createTopper();
        mThumbViewBitmaps = new Bitmap[100 * MAX_THUMB_VIEW_NUM * 3];
        mAlbumPicCounts = new int[100 * MAX_THUMB_VIEW_NUM];
        mAlbumFolders = new String[100 * MAX_THUMB_VIEW_NUM];
        mThumbViewBitmapNum = 0;
        mThumbViewBitmapNumCur = 0;
        mLayoutNewCapture = true;
    }

    /**
     * This method initializes the libraries
     *
     * @return true if initialized correctly, false otherwise
     */
    private boolean initializeLibraries() {
        try {
            System.loadLibrary("scveCommon");
            System.loadLibrary("scveCommon_stub");
            System.loadLibrary("scveMotionVector");
            System.loadLibrary("scveCleverCapture");
            System.loadLibrary("scveCleverCapture_stub");
        } catch (UnsatisfiedLinkError exp) {
            Log.d(LOG_TAG, "Loading libraries failed");
            handleError(AppError.LIBRARY_LOADING_FAILED);
            return false;
        }
        return true;
    }

    private void getDeviceRotation() {
        switch (getWindowManager().getDefaultDisplay().getRotation()) {
            case Surface.ROTATION_90:
            break;
            case Surface.ROTATION_180:
            break;
            case Surface.ROTATION_270:
            break;
            case Surface.ROTATION_0:
            default:
            break;
        }
    }

    /**
     * Push all the controls in front of the screen
     */
    private void bringControlsToFront() {
        View framesLayout = findViewById(R.id.frames);
        framesLayout.bringToFront();
    }

    public void startCaputure() {
        if(mLogFileDump != null) {
            mLogFileDump.init(cameraActor.getWidth(), cameraActor.getHeight(),
                    cameraActor.getPictureWidth(), cameraActor.getPictureHeight(),
                    cameraActor.getFocusMode());

            File bsDumpFolder = new File(mBSDumpFolder);
            if(bsDumpFolder != null && bsDumpFolder.exists()) {
                File[] fileList = bsDumpFolder.listFiles();

                for(File tmp : fileList) {
                    tmp.delete();
                }
            }
        }
        clevercaptureActor.setLogObject(mLogFileDump);
        clevercaptureActor.startCapture(this);

        picNum = 0;
        picNumTotal = 0;
        burstIdx = 0;
        isPicturing = false;
        burstNum = cameraActor.getBurstNum();
        burstInterval = cameraActor.getBurstInterval();
        burstIntervalIdx = burstInterval;
        SimpleDateFormat s = new SimpleDateFormat("yyyy.MM.dd_HH-mm-ss");
        picFolder = ROOT_FOLDER + s.format(new Date()) + "/";

        Log.d(LOG_TAG, "startCaputure burstNum=" + burstNum + " burstInterval=" + burstInterval);

        ImageView imageView = (ImageView) findViewById(R.id.btn_record);
        //imageView.setImageResource(R.drawable.btn_active);
        imageView.setImageResource(R.drawable.btn_active_animation);

        mLayoutNewCapture = true;
        mThumbViewBitmapNumCur = mThumbViewBitmapNum;
        Drawable current = (AnimationDrawable) imageView.getDrawable();
        if(current instanceof AnimationDrawable) {
            mAnimationDrawable = (AnimationDrawable) current;
            mAnimationDrawable.setOneShot(false);
            //mAnimationDrawable.setVisible(true, true);
            mAnimationDrawable.start();
        } else {
            mAnimationDrawable = null;

            imageView.setImageResource(R.drawable.btn_active);
        }

        //mSensorFusion = SensorFusion.getInstance(this);
        //mSensorFusion.registerCallbackCleverCapture(this);
        //mSensorFusion.onResume();
        //mSensorFusionInitialized = true;
        //mSensorHighMotion = false;

        applicationState = AppState.CAPTURE;
        resetAnimation();

        refreshTopper(mThumbViewBitmapNum, true, 0);

        mTextViewPicNum = (TextView) mThumbViews[3];
        mThumbView_1 = (ImageView) mThumbViews[2];
        mThumbView_2 = (ImageView) mThumbViews[1];
        mThumbView_3 = (ImageView) mThumbViews[0];

        cameraActor.setCaptureState(true);

        isCapture = true;
    }

    public void stopCapture() {
        isCapture = false;

        cameraActor.setCaptureState(false);

        //if(mSensorFusionInitialized) {
        //    mSensorFusion.deRegisterCallbackCleverCapture(this);
        //    mSensorFusion.onStop();
        //    mSensorFusion = null;
        //    mSensorFusionInitialized = false;
        //}

        completeCapture();

        if(mAnimationDrawable != null) {
            mAnimationDrawable.stop();
        }

        ImageView imageView = (ImageView) findViewById(R.id.btn_record);
        imageView.setImageResource(R.drawable.btn_normal);

        if(picNumTotal > 0) {
            if(mThumbViewBitmapNum < mThumbViewBitmaps.length) {
                mThumbViewBitmaps[mThumbViewBitmapNum++] = mAnimationBitmap_3;
                mThumbViewBitmaps[mThumbViewBitmapNum++] = mAnimationBitmap_2;
                mThumbViewBitmaps[mThumbViewBitmapNum++] = mAnimationBitmap_1;

                mAlbumPicCounts[mThumbViewBitmapNum/3 - 1] = picNumTotal;
                mAlbumFolders[mThumbViewBitmapNum/3 - 1] = picFolder;

                mThumbViewBitmapNumCur = mThumbViewBitmapNum;
            }

            if(mLogFileDump != null) {
                File bsDumpFolder = new File(mBSDumpFolder);
                if(bsDumpFolder != null && bsDumpFolder.exists()) {
                    File[] fileList = bsDumpFolder.listFiles();

                    for(File tmp : fileList) {
                        String dstName = picFolder +tmp.getName();
                        //Log.d(LOG_TAG, "dstname " + dstName);
                        File dst = new File(dstName);

                        tmp.renameTo(dst);
                    }
                }
            }
        }
            //Log.i(LOG_TAG, "stopCapture -- mThumbViewBitmapNumCur=" + mThumbViewBitmapNumCur);
    }

    /**
     * This method gets fires when user clicks on record button
     *
     * @param view
     *            View which gets clicked
     */
    public void onRecordButtonClick(View view) {
        if (applicationState != AppState.CAPTURE) {// this is start
            startCaputure();
        } else {// this is stop
            stopCapture();
        }
    }

    @Override
    public void onClick(View v) {
        if(isCapture) {
            Log.d(LOG_TAG, "Ignore thumbview clicking during capture mode");
            return;
        }

        int i;
        for(i = 0; i < mThumbViewMaxNum; i++) {
            if(v == mThumbViews[i * 4 + 3])
                break;
        }

        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setDataAndType(Uri.parse("file://" + mAlbumFolders[mThumbViewBitmapNumCur/3 - 1 - i]), "image/*");
        startActivity(intent);
    }

    public void onArrowLeftClick(View view) {
        if(mThumbViewBitmapNumCur > mThumbViewMaxNum * 3) {
            mThumbViewBitmapNumCur -= mThumbViewMaxNum * 3;
            refreshTopper(mThumbViewBitmapNumCur, false, 0);
        }
    }

    public void onArrowRightClick(View view) {
        if((mThumbViewBitmapNum - mThumbViewBitmapNumCur) > mThumbViewMaxNum * 3) {
            mThumbViewBitmapNumCur += mThumbViewMaxNum * 3;
            refreshTopper(mThumbViewBitmapNumCur, false, 0);
        } else {
            mThumbViewBitmapNumCur = mThumbViewBitmapNum;
            refreshTopper(mThumbViewBitmapNumCur, false, 0);
        }
    }

    public void createTopper() {
        int i, j;

        TextView tv;
        ImageView iv;
        RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.WRAP_CONTENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT);

        mThumbViewIndex = 0;
        mThumbViews = new View[MAX_THUMB_VIEW_NUM * 4];
        mDividerViews = new ImageView[MAX_THUMB_VIEW_NUM];

        mTopper = (RelativeLayout) findViewById(R.id.topper);
        mTopper.setVisibility(View.INVISIBLE);

        mArrowLeft = (ImageView) findViewById(R.id.btn_arrow_left);
        mArrowLeft.setVisibility(View.INVISIBLE);
        mArrowLeftWidthPx = 156;

        mArrowRight = (ImageView) findViewById(R.id.btn_arrow_right);
        mArrowRight.setVisibility(View.INVISIBLE);
        mArrowRightWidthPx = 156;

        mDivider = (ImageView) findViewById(R.id.ic_divider);
        mDividerWidthPx = 64;
        mDivider.setVisibility(View.INVISIBLE);

        Log.d(LOG_TAG, "mArrowLeftWidthPx=" + mArrowLeftWidthPx + " mArrowRightWidthPx=" + mArrowRightWidthPx
                + " mDividerWidthPx=" + mDividerWidthPx);

        mThumbViewMaxNum = (mScreenWidth - mArrowLeftWidthPx - mArrowRightWidthPx) /
                (mDividerWidthPx + mThumbViewMaxPx + ALBUM_PAD_PX * 2);
        mAlbumPadPx = ((mScreenWidth - mArrowLeftWidthPx - mArrowRightWidthPx -
                (mThumbViewMaxNum - 1) * mDividerWidthPx) / mThumbViewMaxNum - mThumbViewMaxPx) / 2;
        Log.d(LOG_TAG, "mThumbViewMaxNum=" + mThumbViewMaxNum + " mAlbumPadPx=" + mAlbumPadPx);

        for(j = 0; j < mThumbViewMaxNum; j++) {
            for(i = 0; i < 3; i++) {
                iv = new ImageView(this);
                iv.setBackgroundColor(Color.WHITE);
                iv.setPadding(THUMB_ALL_PAD_PX, THUMB_ALL_PAD_PX, THUMB_ALL_PAD_PX, THUMB_ALL_PAD_PX);

                lp = new RelativeLayout.LayoutParams(
                        mThumbViewMaxPx,
                        mThumbViewMaxPx * 3 / 4);
                lp.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
                lp.addRule(RelativeLayout.ALIGN_PARENT_TOP);
                lp.setMargins(0, i * THUMB_RIGHT_PAD_PX,
                        mArrowRightWidthPx +
                        j * (mDividerWidthPx + mThumbViewMaxPx + mAlbumPadPx * 2) +
                        i * THUMB_RIGHT_PAD_PX +
                        mAlbumPadPx -
                        THUMB_RIGHT_PAD_PX,
                        0);
                iv.setLayoutParams(lp);
                iv.setVisibility(View.INVISIBLE);

                mTopper.addView(iv);
                mThumbViews[mThumbViewIndex++] = iv;
            }

            tv = new TextView(this);
            tv.setBackgroundColor(Color.WHITE);
            tv.getBackground().setAlpha(50);
            tv.setPadding(THUMB_ALL_PAD_PX, THUMB_ALL_PAD_PX, THUMB_ALL_PAD_PX, THUMB_ALL_PAD_PX);
            tv.setTextColor(0xFFFFA500);
            tv.setTextSize(20);
            tv.setText("0");
            tv.setGravity(Gravity.CENTER);
            tv.setLayoutParams(lp);
            tv.setVisibility(View.INVISIBLE);
            tv.setOnClickListener(this);
            mThumbViews[mThumbViewIndex++] = tv;
            mTopper.addView(tv);

            if(j != 0) {
                iv = new ImageView(this);
                iv.setImageResource(R.drawable.ic_divider);

                lp = new RelativeLayout.LayoutParams(
                        RelativeLayout.LayoutParams.WRAP_CONTENT,
                        RelativeLayout.LayoutParams.WRAP_CONTENT);
                lp.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
                lp.addRule(RelativeLayout.CENTER_VERTICAL);
                lp.setMargins(0, 0,
                        mArrowRightWidthPx +
                        j * (mThumbViewMaxPx + mAlbumPadPx * 2) +
                        (j-1) * mDividerWidthPx,
                        0);
                iv.setLayoutParams(lp);
                iv.setVisibility(View.INVISIBLE);
                mDividerViews[j - 1] = iv;
                mTopper.addView(iv);
            }
        }
    }

    public void refreshTopper(int number, boolean isNewCapture, int picNum) {
        int i = 1;
        int index = number;
        TextView tv;
        ImageView iv;
        Bitmap bm;
        int maxAlbumNum;

        if(picNum != 0)
            mAlbumPicCounts[number / 3 - 1] = picNum;

        LayoutParams params;
        mLayoutNewCapture = isNewCapture;
        for(i = 0; i < mThumbViewMaxNum; i++) {
            ((TextView)mThumbViews[i * 4 + 3]).setVisibility(View.INVISIBLE);
            ((ImageView)mThumbViews[i * 4 + 2]).setVisibility(View.INVISIBLE);
            ((ImageView)mThumbViews[i * 4 + 1]).setVisibility(View.INVISIBLE);
            ((ImageView)mThumbViews[i * 4 + 0]).setVisibility(View.INVISIBLE);

            if(i != 0)
                mDividerViews[i-1].setVisibility(View.INVISIBLE);
        }

        if(isNewCapture) {
            i = 1;
            maxAlbumNum = mThumbViewMaxNum - 1;
        }
        else {
            i = 0;
            maxAlbumNum = mThumbViewMaxNum;
        }
        while(index > 0 && i < mThumbViewMaxNum) {
            //Log.d(LOG_TAG, "refreshTopper index=" + index);

            if(i > 0)
                mDividerViews[i-1].setVisibility(View.VISIBLE);

            tv = (TextView)mThumbViews[i * 4 + 3];
            params = (LayoutParams) tv.getLayoutParams();
            params.width = mThumbViewWidth;
            params.height = mThumbViewHeight;
            tv.setLayoutParams(params);
            tv.setVisibility(View.VISIBLE);
            tv.setText(String.format("%d", mAlbumPicCounts[index / 3 - 1]));
            //Log.d(LOG_TAG, "tv.setText " + mAlbumPicCounts[index / 3 - 1]);

            bm = mThumbViewBitmaps[--index];
            iv = (ImageView)mThumbViews[i * 4 + 2];
            params = (LayoutParams) iv.getLayoutParams();
            params.width = mThumbViewWidth;
            params.height = mThumbViewHeight;
            iv.setLayoutParams(params);
            if(bm == null)
                iv.setVisibility(View.INVISIBLE);
            else
                iv.setVisibility(View.VISIBLE);
            iv.setImageBitmap(bm);

            bm = mThumbViewBitmaps[--index];
            iv = (ImageView)mThumbViews[i * 4 + 1];
            params = (LayoutParams) iv.getLayoutParams();
            params.width = mThumbViewWidth;
            params.height = mThumbViewHeight;
            iv.setLayoutParams(params);
            if(bm == null)
                iv.setVisibility(View.INVISIBLE);
            else
                iv.setVisibility(View.VISIBLE);
            iv.setImageBitmap(bm);

            bm = mThumbViewBitmaps[--index];
            iv = (ImageView)mThumbViews[i * 4 + 0];
            params = (LayoutParams) iv.getLayoutParams();
            params.width = mThumbViewWidth;
            params.height = mThumbViewHeight;
            iv.setLayoutParams(params);
            if(bm == null)
                iv.setVisibility(View.INVISIBLE);
            else
                iv.setVisibility(View.VISIBLE);
            iv.setImageBitmap(bm);

            i++;
        }

        if(number / 3 > maxAlbumNum)
            mArrowLeft.setVisibility(View.VISIBLE);
        else
            mArrowLeft.setVisibility(View.INVISIBLE);

        if((mThumbViewBitmapNum - number) / 3 > 0)
            mArrowRight.setVisibility(View.VISIBLE);
        else
            mArrowRight.setVisibility(View.INVISIBLE);
    }

    public void onThumbClick(View view) {
        if(picNum > 0) {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setDataAndType(Uri.parse("file://" + picFolder), "image/*");
            startActivity(intent);
        }
    }

    public void onRoiButtonClick(View view) {
        if(mRoiEnabled) {
            mRoiEnabled = false;
            ImageView imageView = (ImageView) findViewById(R.id.btn_roi);
            imageView.setImageResource(R.drawable.btn_active_region_off);
            mRoiView.setVisibility(View.GONE);
            mRoiView.setOnTouchListener(null);
        } else {
            mRoiEnabled = true;
            ImageView imageView = (ImageView) findViewById(R.id.btn_roi);
            imageView.setImageResource(R.drawable.btn_active_region_on);
            mRoiView.setVisibility(View.VISIBLE);
            mRoiView.setOnTouchListener(this);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onStart() {
        super.onStart();

        Log.d(LOG_TAG, "onStart");

        //For release version, switch to legacy camera interface
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            cameraActor = new CameraActorCamera2(this);
        } else {
            cameraActor = new CameraActorLegacy(this);
        }
        clevercaptureActor = new CleverCaptureActor(cameraActor, cameraPreview);

        if (applicationState == AppState.INITIALIZED) {
            startCamera();
        }

        mConnection = new MediaScannerConnection(this, this);
        mConnection.connect();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onResume() {
        Log.d(LOG_TAG, "onResume");
        mSensorFusion = SensorFusion.getInstance(this);
        mSensorFusion.registerCallbackCleverCapture(this);
        mSensorFusion.onResume();
        mSensorFusionInitialized = true;
        mSensorHighMotion = false;

        super.onResume();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onPause() {
        Log.d(LOG_TAG, "onPause");
        if(mSensorFusionInitialized) {
            mSensorFusion.deRegisterCallbackCleverCapture(this);
            mSensorFusion.onStop();
            mSensorFusion = null;
            mSensorFusionInitialized = false;
        }

        super.onPause();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onStop() {
        Log.d(LOG_TAG, "onStop");
        super.onStop();
        if (applicationState != AppState.UNINITIALIZED &&
                applicationState != AppState.INITIALIZED) {
            clearCamera();
        }

        mConnection.disconnect();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBackPressed() {
        Log.d(LOG_TAG, "onBackPressed");
        if (applicationState == AppState.CAPTURE) {
            stopCapture();
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
        startActivity(intent);
    }

    /**
     * Invoked when the switch camera button is clicked
     *
     * @param view
     *            {@link View} that was clicked
     */
    public void onSwitchCameraButtonClick(View view) {
        int id = (CameraPreferenceManager.getSelectedCameraId(this) + 1) % 2;
        CameraPreferenceManager.setSelectedCameraId(this, id);
        updateCameraSettings();
    }

    private void updateCameraSettings() {
        recreate();
    }

    /**
     * Initialize the activity
     */
    private void startCamera() {
        if (!cameraActor.startCamera()) {
            Log.d(LOG_TAG, "Camera failed to get initialized");
            handleError(AppError.CAMERA_FAILED);
        }

        clevercaptureActor.init();

        getDeviceRotation();
        bringControlsToFront();

        applicationState = AppState.READY;
    }

    /**
     * Clear out the activity
     */
    private void clearCamera() {
        if (applicationState == AppState.CAPTURE) {
            cancelCapture();
        }

        applicationState = AppState.INITIALIZED;
        cameraActor.wrapUp();
        clevercaptureActor.release();
    }

    /**
     * Completes the ongoing capture process
     */
    private void completeCapture() {
        ImageView recordButton = (ImageView) findViewById(R.id.btn_record);
        recordButton.setImageResource(R.drawable.start_btn);
        clevercaptureActor.stopCapture();

        /*if(picNum > 0) {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setDataAndType(Uri.parse("file://" + picFolder), "image/*");
            startActivity(intent);
        }*/

        applicationState = AppState.READY;
    }

    /**
     * Cancels the ongoing capture process
     */
    private void cancelCapture() {
        ImageView recordButton = (ImageView) findViewById(R.id.btn_record);
        recordButton.setImageResource(R.drawable.start_btn);
        clevercaptureActor.stopCapture(true);

        applicationState = AppState.READY;
    }

    /**
     * Handle errors from various sources, and update the UI/app state as appropriate
     * @param applicationError {@link AppError} to handle
     */
    private void handleError(final AppError applicationError) {
        if (applicationState == AppState.EXIT) {
            return;
        }
        if (applicationError.isFatal()) {
            applicationState = AppState.EXIT;
        }

        String message = applicationError
                .getResourceString(MainActivity.this);
        if (applicationError.getErrorCode() != 0) {
            message = message
                    + " Error: 0x"
                    + Integer.toHexString(applicationError
                            .getErrorCode());
            String errInfo = applicationError.getErrorInfo(this);
            if (errInfo != null) {
                message = message + " - " + errInfo;
            }
        }

        /*
        if (applicationError.isImageRecoverable()) {
            completeCapture();
        } else {
            cancelCapture();
        }
        */

        // show error dialog
        String dialogTitle = getResources().getString(
                R.string.error_dialog_title);
        String positiveButton = getResources().getString(
                R.string.btn_ok);
        AlertDialog.Builder errorDialog = new AlertDialog.Builder(
                MainActivity.this);
        errorDialog.setTitle(dialogTitle);
        errorDialog.setMessage(message);

        errorDialog.setPositiveButton(positiveButton,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog,
                            int whichButton) {
                        if (applicationError.isFatal()) {
                            Log.d(LOG_TAG, "Closing application");
                            dialog.dismiss();
                            MainActivity.this.finish();
                            return;
                        } else if (applicationError
                                .isImageRecoverable()) {
                            ImageView recordButton = (ImageView) findViewById(R.id.btn_record);
                            recordButton
                                    .setImageResource(R.drawable.start_btn);
                        }
                    }
                });
        errorDialog.create();
        errorDialog.show();
    }

    @Override
    public void onCaptureStarted() {
        Log.d(LOG_TAG, "onCaptureStarted");
        picPaths.clear();
        picNumScanned = 0;
    }

    @Override
    public void onCaptureCompleted() {
        Log.d(LOG_TAG, "onCaptureCompleted");

        new Thread() {
            @Override
            public void run() {
                int maxTryTime = 100;
                int tryTime = 0;
                Log.i(LOG_TAG, "Post processing thread starts");
                while( picNumScanned != clevercaptureActor.getPostProcessingResult().size() &&
                       (tryTime < maxTryTime) ) {
                    try {
                        Thread.sleep(50, 0);
                    } catch (InterruptedException e) {
                        Log.e(LOG_TAG, "InterruptedException " + e);
                    }

                    tryTime++;
                }
                Log.i(LOG_TAG, "start to handle post processing result");
                Vector<Integer> presult = clevercaptureActor.getPostProcessingResult();
                picNumTotal = 0;

                for(int i = 0; i < presult.size(); i++) {
                    if(i >= picPaths.size())
                        break;

                    String fileName = picPaths.get(i);
                    if(presult.get(i) == 0) {
                        Log.i(LOG_TAG, "remove " + fileName);

                        File file = new File(fileName);
                        if(file.exists())
                            file.delete();
                        if(mConnection != null && mConnection.isConnected()) {
                        mConnection.scanFile(fileName, null);
                        }

                        //mConnection.scanFile(fileName, null);
                    } else {
                        picNumTotal++;
                    }
                }

                //Log.i(LOG_TAG, "onCaptureCompleted mThumbViewBitmapNumCur=" + mThumbViewBitmapNumCur
                //        + " picNumTotal=" + picNumTotal);
                //if(picNumTotal > 0 && mThumbViewBitmapNumCur/3 > 1) {
                //    mAlbumPicCounts[mThumbViewBitmapNumCur/3 - 1] = picNumTotal;
                //}

                if(mLogFileDump != null) {
                    mLogFileDump.setPostProcessingResult(presult);
                    mLogFileDump.writeFile(picFolder + "clevercapture.log");
                }

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        refreshTopper(mThumbViewBitmapNumCur, false, picNumTotal);
                    }
                });

                Log.i(LOG_TAG, "Post processing thread exits now");
            }
        }.start();
    }

    @Override
    public void onCaptureCancelled() {
        Log.d(LOG_TAG, "onCaptureCancelled");

    }

    @Override
    public void onError(AppError error) {
        Log.d(LOG_TAG, "onError");

    }

    @Override
    public void onPicture(int frameId, int reserved) {
        boolean motion = mSensorHighMotion;

        Log.d(LOG_TAG, "onPicture motion=" + motion);
        if(motion && CameraPreferenceManager.getMotionCfg(this) == 1) {
            Log.d(LOG_TAG, "Do not take picture during high hand motion");

            if(mLogFileDump != null) {
                mLogFileDump.getSkipReason().add(LogFileDump.SKIP_MOTION);
            }
            return;
        }

        if(!isPicturing) {
            isPicturing = true;
            mHandler.obtainMessage(MSG_TAKEPICTURE).sendToTarget();
            Log.d(LOG_TAG, "keypoint trigger take picture now");

            mAnimation = true;
             clevercaptureActor.submitForEval(frameId);
        } else {
            Log.d(LOG_TAG, "last picture not completed, ignore the new one");

            if(mLogFileDump != null) {
                mLogFileDump.getSkipReason().add(LogFileDump.SKIP_LAST_NOT_DONE);
            }
        }
    }

    @Override
    public void onKeyFrameCaptured(byte[] jpegData) {
        if(!isCapture) {
            Log.d(LOG_TAG, "clevercapture stopped already, ignore this pictures");
            return;
        }

        int picNumTmp = picNum;
        int intervalIdxTmp = burstIntervalIdx;

        burstIntervalIdx--;
        if(burstIntervalIdx == 0) {
            burstIntervalIdx = burstInterval;
        }

        if(intervalIdxTmp != burstInterval) {
            Log.d(LOG_TAG, "skip current keyframe");
            return;
        }

        burstIdx++;
        picNumTotal++;

        Log.d(LOG_TAG, "onKeyFrameCaptured mAnimation=" + mAnimation);
        if(burstIdx == burstNum) {
            picNum++;
            isPicturing = false;
            burstIdx = 0;
        }

        if(mTopper.getVisibility() != View.VISIBLE) {
            mTopper.setVisibility(View.VISIBLE);
        }

        if(mAnimation) {
            if(mAnimationBitmap_2 != null)
                mAnimationBitmap_3 = mAnimationBitmap_2;
            if(mAnimationBitmap_1 != null)
                mAnimationBitmap_2 = mAnimationBitmap_1;

            if(cameraActor.getThumbnailWidth() == 0 || cameraActor.getThumbnailHeight() == 0) {
                mAnimationBitmap_1 = BitmapFactory.decodeByteArray(jpegData, 0, jpegData.length);
            } else {//use thumbnail to animate to achieve good performance
                mAnimationBitmap_1 = ThumbnailUtils.extractThumbnail(
                        BitmapFactory.decodeByteArray(jpegData, 0, jpegData.length),
                        cameraActor.getThumbnailWidth(), cameraActor.getThumbnailHeight());
            }
            if(mAnimationBitmap_1.getWidth() * mAnimationBitmap_1.getHeight() > WIDTH_THUMB * HEIGHT_THUMB) {
                int width = mAnimationBitmap_1.getWidth();
                int height = mAnimationBitmap_1.getHeight();

                width /= 2;
                height /= 2;
                while(width * height > WIDTH_THUMB * HEIGHT_THUMB) {
                    width /= 2;
                    height /= 2;
                }
                mAnimationBitmap_1 = Bitmap.createScaledBitmap(mAnimationBitmap_1, width, height, false);
            }

            setThumbImage(mThumbView_1, mAnimationBitmap_1);
            mThumbViewWidth = mThumbView_1.getWidth();
            mThumbViewHeight = mThumbView_1.getHeight();
            if(mThumbView_1.getVisibility() != View.VISIBLE) {
                mThumbView_1.setVisibility(View.VISIBLE);
            }
            startCaptureAnimation(mThumbView_1);
            mAnimation = false;
        }
        picName = "picture_" + String.format("%03d_%02d", picNumTmp, (burstIdx + burstNum - 1) % burstNum) + ".jpg";
        Log.d(LOG_TAG, "keypoint data would be saved into " + picFolder + picName);
        picPaths.add(picFolder + picName);
        AsyncTask.execute(new SavePictureRunnable(picFolder, picName, jpegData, mConnection));

        mTextViewPicNum.setText(String.format("%d", picNumTotal));
        if(picNumTotal == 1) {
            ViewGroup.LayoutParams layoutParams = mThumbView_1.getLayoutParams();
            mTextViewPicNum.setLayoutParams(layoutParams);

            if(mTextViewPicNum.getVisibility() != View.VISIBLE) {
                mTextViewPicNum.setVisibility(View.VISIBLE);
            }
        }
    };

    private static class SavePictureRunnable implements Runnable {
        private final String folder;
        private final String name;
        private final byte[] jpegData;
        private final MediaScannerConnection connection;
        private final String mimeType = "image/jpeg";

        public SavePictureRunnable(String folder, String name, byte[] jpegData, MediaScannerConnection connection) {
            this.folder = folder;
            this.name = name;
            this.jpegData = jpegData;
            this.connection = connection;
        }

        @Override
        public void run() {
            ImageUtils.savePicture(folder, name, jpegData);
            if(connection != null && connection.isConnected()) {
            connection.scanFile(folder + name, mimeType);
            }
            //connection.scanFile(folder + name, mimeType);
            Log.d(LOG_TAG, "keypoint picture " + folder + name + " save done");
        }
    }

    @Override
    public void onMediaScannerConnected() {
        Log.d(LOG_TAG, "onMediaScannerConnected");
    }

    @Override
    public void onScanCompleted(String arg0, Uri arg1) {
        Log.d(LOG_TAG, "onScanCompleted arg0=" + arg0 + " arg1=" + arg1);
            if(arg1 != null)
            picNumScanned++;
    }

    @Override
    public void onHighMotion(boolean motion) {
        if (motion) {
            if (!mSensorHighMotion) {
                Log.d(LOG_TAG, "Motion Started");
            }
        } else {
            if (mSensorHighMotion) {
                Log.d(LOG_TAG, "Motion Stopped");
            }
        }
        mSensorHighMotion = motion;
    }

    @Override
    public boolean onTouch(View view, MotionEvent event) {
        //Log.d(LOG_TAG, "mydebug event=" + event);
        if(view.getId() == R.id.camera_preview_view) {
            if(mRoiView.getVisibility() != View.VISIBLE) {
                switch (event.getAction() & MotionEvent.ACTION_MASK) {
                case MotionEvent.ACTION_POINTER_DOWN:
                    previousPinchDistance = Utils.getSpacing(event);
                    if (previousPinchDistance > minimumPinchZoom) {
                        Utils.getMidPoint(pinchMidPoint, event);
                        touchState = TouchState.PINCH;
                    } else {
                        touchState = TouchState.NONE;
                    }
                    break;
                case MotionEvent.ACTION_POINTER_UP:
                    if (touchState == TouchState.NONE) {
                        break;
                    }
                    touchState = TouchState.NONE;
                    updateRoi();
                    break;
                case MotionEvent.ACTION_MOVE:
                    if (touchState == TouchState.PINCH) {
                        float distance = Utils.getSpacing(event);
                        if( Math.abs(distance - previousPinchDistance) > 20
                            && cameraActor.isZoomSupported()
                            && CameraPreferenceManager.getZoomCfg(this) == 1 ) {
                            if(!mRoiRectUpdated) {
                                removeRoi();
                            }

                            float zoom = cameraActor.getZoom();
                            zoom += (distance - previousPinchDistance) * cameraActor.getMaxZoom() / 2000;
                            cameraActor.setZoom(zoom);
                            previousPinchDistance = distance;

                            mRoiRectUpdated = true;
                        }
                    }
                }
            }
        } else if(view.getId() == R.id.roi_view) {
            switch (event.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_DOWN: {
                startLocation.set(event.getX(), event.getY());
                moveFromLocation.set(event.getX(), event.getY());

                int touchIndex = mRoiDrawable.updateStartPoint(event.getX(), event.getY());
                if(touchIndex == ROIDrawable.INNER) {
                    touchState = TouchState.MOVE;
                    mRoiDrawable.lockView();
                } else if(touchIndex == ROIDrawable.TOP || touchIndex == ROIDrawable.RIGHT ||
                          touchIndex == ROIDrawable.BOTTOM || touchIndex == ROIDrawable.LEFT) {
                    touchState = TouchState.DRAG;
                    mRoiDrawable.lockView();
                } else if(touchIndex == ROIDrawable.TOPLEFT || touchIndex == ROIDrawable.TOPRIGHT ||
                          touchIndex == ROIDrawable.BOTTOMRIGHT || touchIndex == ROIDrawable.BOTTOMLEFT) {
                    touchState = TouchState.ZOOM;
                    mRoiDrawable.lockView();
                } else {
                    touchState = TouchState.NONE;
                }
                break;
            }

            case MotionEvent.ACTION_POINTER_DOWN:
                previousPinchDistance = Utils.getSpacing(event);
                //Log.d(LOG_TAG, "previousPinchDistance=" + previousPinchDistance + " minimumPinchZoom=" + minimumPinchZoom);
                if (previousPinchDistance > minimumPinchZoom) {
                    Utils.getMidPoint(pinchMidPoint, event);
                    touchState = TouchState.PINCH;
                    mRoiDrawable.lockView();
                } else {
                    touchState = TouchState.NONE;
                }
                break;

            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                if (touchState == TouchState.NONE) {
                    break;
                }

                mRoiDrawable.unlockView();
                touchState = TouchState.NONE;
                updateRoi();
                break;

            case MotionEvent.ACTION_MOVE:
                if (touchState == TouchState.DRAG) {
                    if(Math.abs(event.getX() - moveFromLocation.x) > 20 || Math.abs(event.getY() - moveFromLocation.y) > 20) {
                        if(!mRoiRectUpdated) {
                            removeRoi();
                        }

                        mRoiDrawable.drag(startLocation, moveFromLocation, event.getX(), event.getY());
                        moveFromLocation.set(event.getX(), event.getY());

                        mRoiRectUpdated = true;
                    }

                } else if(touchState == TouchState.MOVE) {
                    if(Math.abs(event.getX() - moveFromLocation.x) > 20 || Math.abs(event.getY() - moveFromLocation.y) > 20) {
                        if(!mRoiRectUpdated) {
                            removeRoi();
                        }

                        mRoiDrawable.move(startLocation, moveFromLocation, event.getX(), event.getY());
                        moveFromLocation.set(event.getX(), event.getY());

                        mRoiRectUpdated = true;
                    }
                } else if (touchState == TouchState.ZOOM) {
                    if(Math.abs(event.getX() - moveFromLocation.x) > 20 || Math.abs(event.getY() - moveFromLocation.y) > 20) {
                        if(!mRoiRectUpdated) {
                            removeRoi();
                        }

                        mRoiDrawable.zoom(startLocation, moveFromLocation, event.getX(), event.getY());
                        moveFromLocation.set(event.getX(), event.getY());

                        mRoiRectUpdated = true;
                    }
                } else if (touchState == TouchState.PINCH && cameraActor.isZoomSupported()) {
                    float distance = Utils.getSpacing(event);
                    //Log.d(LOG_TAG, "previousPinchDistance=" + previousPinchDistance + " distance=" + distance);
                    if(Math.abs(distance - previousPinchDistance) > 20) {
                        if(!mRoiRectUpdated) {
                            removeRoi();
                        }

                        float zoom = cameraActor.getZoom();
                        zoom += (distance - previousPinchDistance) * cameraActor.getMaxZoom() / 1000;
                        cameraActor.setZoom(zoom);
                        previousPinchDistance = distance;

                        mRoiRectUpdated = true;
                    }
                }

                break;
            }
        }

        return true;
    }

    public void updateScreenSize() {
        Point size = new Point();
        Display display = getWindowManager().getDefaultDisplay();
        float density  = getResources().getDisplayMetrics().density;

        display.getSize(size);
        mScreenWidth = size.x;
        mScreenHeight = size.y;

        mScreenWidthDp = (int) (mScreenWidth / density);
        mScreenHeightDp = (int) (mScreenHeight / density);

        Log.d(LOG_TAG, "screen size [" + mScreenWidth + "x" + mScreenHeight +
                       "] in pixels, [" + mScreenWidthDp + "x" + mScreenHeightDp +
                       "] in dp");
    }

    public void updateRoi() {
        if(!mRoiRectUpdated) {
            Log.d(LOG_TAG, "ROI is not updated");
            return;
        }

        RectF roi = new RectF(0f, 0f, 1f, 1f);
        if(mRoiView.getVisibility() == View.VISIBLE) {
            roi = mRoiDrawable.copyFrameRectF();
            roi.left /= (float)mScreenWidth;
            roi.right /= (float)mScreenWidth;
            roi.top /= (float)mScreenHeight;
            roi.bottom /= (float)mScreenHeight;
        }
        Log.d(LOG_TAG, "updateRoi " + roi.toShortString());

        clevercaptureActor.updateRoi(roi);
        mRoiRectUpdated = false;
    }

    public void removeRoi() {
        clevercaptureActor.removeRoi();
    }

    public void setThumbImage(ImageView view, Bitmap bm) {
        int width = bm.getWidth();
        int height = bm.getHeight();
        int viewWidth = 0;
        int viewHeight = 0;

        if(width > height) {
            viewWidth = mThumbViewMaxPx;
            viewHeight = viewWidth * height / width;
        } else {
            viewHeight = mThumbViewMaxPx;
            viewWidth = viewHeight * width / height;
        }

        LayoutParams params = (LayoutParams) view.getLayoutParams();
        params.width = viewWidth;
        params.height = viewHeight;
        view.setLayoutParams(params);

        view.setImageBitmap(bm);
    }

    public void startCaptureAnimation(final View view) {
        Log.d(LOG_TAG, "startCaptureAnimation");

        if (mCaptureAnimator != null && mCaptureAnimator.isStarted()) {
            mCaptureAnimator.cancel();
        }

        //View parentView = (View) view.getParent();
        //float slideDistance = (float) (parentView.getWidth() - view.getLeft());
        //int parentViewWidth = view.getWidth() * 8;
        //int parentViewHeight = view.getHeight() * 8;

        //float scaleX = ((float) parentViewWidth) / ((float) view.getWidth());
        //float scaleY = ((float) parentViewHeight) / ((float) view.getHeight());
        float scale = 3;//scaleX > scaleY ? scaleX : scaleY;

        //int centerX = view.getLeft() + view.getWidth() / 2;
        //int centerY = view.getTop() + view.getHeight() / 2;

        //ObjectAnimator slide = ObjectAnimator.ofFloat(view, "translationX", 0f, slideDistance).setDuration(SLIDE_DURATION);
        //slide.setStartDelay(SHRINK_DURATION + HOLD_DURATION);

        mCaptureAnimator = new AnimatorSet();
        mCaptureAnimator.playTogether(
                ObjectAnimator.ofFloat(view, "scaleX", scale, 1f).setDuration(SHRINK_DURATION)
                ,ObjectAnimator.ofFloat(view, "scaleY", scale, 1f).setDuration(SHRINK_DURATION)
                ,ObjectAnimator.ofFloat(view, "translationX", -1 * mScreenWidth / 2, 0f).setDuration(SHRINK_DURATION)
                ,ObjectAnimator.ofFloat(view, "translationY", mScreenHeight / 2, 0f).setDuration(SHRINK_DURATION)
                /*,slide*/
                );
        mCaptureAnimator.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animator) {
                Log.d(LOG_TAG, "onAnimationStart");

                view.setClickable(false);
                view.setVisibility(View.VISIBLE);

                if(mAnimationBitmap_3 != null && mThumbView_3.getVisibility() != View.VISIBLE)
                    mThumbView_3.setVisibility(View.VISIBLE);
                if(mAnimationBitmap_2 != null && mThumbView_2.getVisibility() != View.VISIBLE)
                    mThumbView_2.setVisibility(View.VISIBLE);
                if(mAnimationBitmap_2 != null)
                    setThumbImage(mThumbView_2, mAnimationBitmap_2);
                if(mAnimationBitmap_3 != null)
                    setThumbImage(mThumbView_3, mAnimationBitmap_3);
            }

            @Override
            public void onAnimationEnd(Animator animator) {
                Log.d(LOG_TAG, "onAnimationEnd");

                view.setScaleX(1f);
                view.setScaleX(1f);
                view.setTranslationX(0f);
                view.setTranslationY(0f);
                view.setVisibility(View.VISIBLE);
                mCaptureAnimator.removeAllListeners();
                mCaptureAnimator = null;
            }

            @Override
            public void onAnimationCancel(Animator animator) {
                Log.d(LOG_TAG, "onAnimationCancel");
                //view.setVisibility(View.GONE);
            }

            @Override
            public void onAnimationRepeat(Animator animator) {
                Log.d(LOG_TAG, "onAnimationRepeat");
                // Do nothing.
            }
        });
        mCaptureAnimator.start();
    }

    public void resetAnimation() {
        Log.d(LOG_TAG, "resetAnimation");

        mAnimation = false;

        if (mCaptureAnimator != null && mCaptureAnimator.isStarted()) {
            mCaptureAnimator.cancel();
        }
        mCaptureAnimator = null;

        if(mTextViewPicNum != null)
            mTextViewPicNum.setVisibility(View.GONE);
        if(mThumbView_1 != null)
            mThumbView_1.setVisibility(View.GONE);
        if(mThumbView_2 != null)
            mThumbView_2.setVisibility(View.GONE);
        if(mThumbView_3 != null)
            mThumbView_3.setVisibility(View.GONE);
        if(mImageViewTranslate != null)
            mImageViewTranslate.setVisibility(View.GONE);

        mAnimationBitmap_1 = null;
        mAnimationBitmap_2 = null;
        mAnimationBitmap_3 = null;
    }
}
