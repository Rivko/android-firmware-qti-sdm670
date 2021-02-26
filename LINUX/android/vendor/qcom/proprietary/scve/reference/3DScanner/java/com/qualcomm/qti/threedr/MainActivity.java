/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;


import android.app.Activity;
import android.app.DialogFragment;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.hardware.SensorManager;
import android.media.AudioAttributes;
import android.media.SoundPool;
import android.opengl.GLSurfaceView;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.text.Html;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.threedr.CannedDataTest.CannedDataIO;
import com.qualcomm.qti.threedr.CannedDataTest.CannedDataStream;
import com.qualcomm.qti.threedr.MeshViewer.MeshViewerUtils;
import com.qualcomm.qti.threedr.mmrd.sensorfusioncv.MotionSensorManager;
import com.qualcomm.qti.threedr.ui3d.GLGuidanceRenderer;
import com.qualcomm.qti.threedr.ui3d.TargetIndicatorController;
import com.qualcomm.qti.threedr.widget.HintDialogFragment;

import org.openni.Device;
import org.openni.DeviceInfo;
import org.openni.OpenNI;
import org.openni.SensorType;
import org.openni.android.OpenNIHelper;

import java.io.IOException;
import java.util.List;

public class MainActivity extends Activity implements OpenNIHelper.DeviceOpenListener {

    private static final int BUTTON_STATE_DOWN = 1;
    private static final int BUTTON_STATE_UP = 0;

    private static final String TAG = "Scan3D";

    private static final float SCAN_DISTANCE_TOO_CLOSE = 0.5f;
    private static final float SCAN_DISTANCE_TOO_FAR = 0.9f;

    private static final int IMAGE_640_480_RES_WIDTH = 640;
    private static final int IMAGE_640_480_RES_HEIGHT = 480;

    private static final int IMAGE_1280_720_RES_WIDTH = 1280;
    private static final int IMAGE_1280_720_RES_HEIGHT = 720;

    private static final int IMAGE_432_244_RES_WIDTH = 432;
    private static final int IMAGE_432_244_RES_HEIGHT = 244;

    public static int ColorImageWidth = -1;
    public static int ColorImageHeight = -1;

    public static int DepthImageWidth = -1;
    public static int DepthImageHeight = -1;

    public static boolean isOrbbecSensor = true;//this flag is used only to account the div 10 for orbbec sensor depth in 3DR_APIs.cpp

    // modules
    private MotionSensorManager mMotionSensor;

    // state variables
    private boolean mIsInted = false;

    private boolean vo_config[];

    private int[] m_pointerArray;

    public boolean ASOpen;
    public boolean scanShutdownStarted = false;

    Handler UIHandler;

    public static Activity currentActivity;

    public TextView infoTextView = null;
    public TextView debugTextView = null;

    int xAngle = 0;
    int yAngle = 0;

    public MultiStateButton toggleButton = null;
    TextView header;
    private GLGuidanceRenderer glGuidanceRenderer;
    private GLSurfaceView glSurfaceViewSSDAndGuidance;

    private boolean mGuidanceEnabled;

    public float initialSensorPitchDeg = 0;
    public float initialSensorRollDeg = 0;

    private boolean modulesDestroyed = false;

    private SeekBar sizeSliderBB;

    private ImageButton imageButtonRotateTop;
    private ImageButton imageButtonRotateFront;
    private ImageButton imageButtonRotateSide;

    long mPrevTS_Depth = 0;
    long ctr = 0;

    private SoundPool soundPool;
    private int soundEffectId;

    private TargetIndicatorController targetHitIndicatorController;

    private OpenNIHelper mOpenNIHelper;
    private boolean mDeviceOpenPending = false;
    private Device mDevice;
    private List<SensorType> mDeviceSensors;
    private int mActiveDeviceID = -1;

    private OrbecStream orbecStream;
    private boolean autoExposureEnabled = true;
    private boolean dumpFramesEnabled = false;

    private CannedDataStream cannedDataStream;

    public static CannedDataIO.CannedDataSetType cannedDataSetType = CannedDataIO.CannedDataSetType.SET3_DATASET;


    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        currentActivity = this;

        //String ver = FastCVJNILib.getFastCVVersion();

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN
        );

        //Do this first
        VOConfig voConfig = Preferences.getVOConfig(this);
        cannedDataSetType = CannedDataIO.CannedDataSetType.values()[Preferences.getPrefCannedDatasetType(this)];
        vo_config = voConfig.ToArray();
        initializeImageResoultion();

        // set view
        initScannerViews();

        try {
            mOpenNIHelper = new OpenNIHelper(this);
        } catch (IOException e) {
            e.printStackTrace();
        }


        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            OpenNI.setLogAndroidOutput(true);
            OpenNI.setLogMinSeverity(0);
            OpenNI.initialize();
        }

    }

    private void initializeImageResoultion() {
        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING] || cannedDataSetType == CannedDataIO.CannedDataSetType.ORBBEC_SENSOR) {
            //orbbec
            ColorImageWidth = IMAGE_640_480_RES_WIDTH;
            ColorImageHeight = IMAGE_640_480_RES_HEIGHT;

            DepthImageWidth = IMAGE_640_480_RES_WIDTH;
            DepthImageHeight = IMAGE_640_480_RES_HEIGHT;

            isOrbbecSensor = true;
        } else {

            switch (cannedDataSetType) {
                case RABBIT_DATASET:
                case SHANDON_DATASET:
                case SET3_DATASET: {
                    //canned data for old sensor i.e Shandon Dataset
                    ColorImageWidth = IMAGE_1280_720_RES_WIDTH;
                    ColorImageHeight = IMAGE_1280_720_RES_HEIGHT;

                    DepthImageWidth = IMAGE_432_244_RES_WIDTH;
                    DepthImageHeight = IMAGE_432_244_RES_HEIGHT;
                    isOrbbecSensor = false;
                    break;
                }
                default:
                    throw new RuntimeException("unkown cannedDatasettype :" + cannedDataSetType.toString());
            }
        }
    }

    public void targetHit() {
        soundPool.play(soundEffectId, 1f, 1f, 1, 0, 1);
        targetHitIndicatorController.targetHitStarted();
    }

    public void showHintDialog() {
        HintDialogFragment fragment = new HintDialogFragment();
        fragment.show(getFragmentManager(), "hint");
    }

    public void dismissHint() {
        DialogFragment df = (DialogFragment) getFragmentManager().findFragmentByTag("hint");
        if (df != null && df.getDialog() != null) {
            df.getDialog().dismiss();
        }
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause");
        super.onPause();

        if (mDeviceOpenPending) {
            return;
        }
        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            stopOrbbecStreams();
        } else {
            cannedDataStream.stop();
        }

        destroyModules(false);
        finish();
    }

    private void stopOrbbecStreams() {
        // onPause() is called just before the USB permission dialog is opened, in which case, we don't
        // want to shutdown OpenNI
        if (mDeviceOpenPending)
            return;

        if (orbecStream != null)
            orbecStream.stop();

        if (mDevice != null) {
            mDevice.close();
            mDevice = null;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        mOpenNIHelper.shutdown();
        OpenNI.shutdown();
    }

    private static float BB_LOW_RANGE = 0.36f / 2.0f;
    private static float BB_DEFAULT_SIZE = 0.48f / 2.0f;
    private static float BB_HIGH_RANGE = 0.64f / 2.0f;
    private static float slope = (BB_HIGH_RANGE - BB_LOW_RANGE) / 100;

    private float sliderRangeToBBFloat(int sliderValue) {
        return (slope * sliderValue) + BB_LOW_RANGE;
    }

    private int BBFloatToSliderValue(float desiredBB) {
        return (int) ((desiredBB - BB_LOW_RANGE) / slope);
    }

    private void clearRotationButtons() {
        imageButtonRotateFront.setSelected(false);
        imageButtonRotateSide.setSelected(false);
        imageButtonRotateTop.setSelected(false);
    }

    private void initScannerViews() {

        setContentView(R.layout.activity_vo_scanner);

        targetHitIndicatorController = new TargetIndicatorController(
                (ImageView) findViewById(R.id.targetHitImageView));

        infoTextView = (TextView) findViewById(R.id.infoTextView);
        debugTextView = (TextView) findViewById(R.id.debugTextView);

        //TextView waitTitle = (TextView) findViewById(R.id.textViewWaiting);
        //waitTitle.setText(R.string.action_init_camera);
        findViewById(R.id.waiting).setVisibility(View.GONE);

        header = (TextView) findViewById(R.id.uiHeader);
        header.setText(Html.fromHtml(getResources().getString(R.string.previewHeader)));

        VOConfig voConfig = Preferences.getVOConfig(this);
        boolean vo_config[] = voConfig.ToArray();


        glSurfaceViewSSDAndGuidance = (GLSurfaceView) findViewById(R.id.glSurfaceViewSSDandGuidance);
        View pantouchArea = (View) findViewById(R.id.panTouchArea);
        pantouchArea.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                return onPanAreaTouchEvent(event);
            }
        });


        glGuidanceRenderer = new GLGuidanceRenderer(this, SCAN_DISTANCE_TOO_CLOSE,
                SCAN_DISTANCE_TOO_FAR);

        mGuidanceEnabled = vo_config[VO_Configuration.ConfigName.USE_ENABLE_GUIDANCE];
        glGuidanceRenderer.guidanceEnabled = mGuidanceEnabled;

        glSurfaceViewSSDAndGuidance.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glSurfaceViewSSDAndGuidance.setEGLContextClientVersion(2);
        glSurfaceViewSSDAndGuidance.setRenderer(glGuidanceRenderer);


        imageButtonRotateTop = (ImageButton) findViewById(R.id.imageButtonTop);
        imageButtonRotateSide = (ImageButton) findViewById(R.id.imageButtonSide);
        imageButtonRotateFront = (ImageButton) findViewById(R.id.imageButtonFront);

        findViewById(R.id.imageButtonTop).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!v.isSelected()) {
                    clearRotationButtons();
                    v.setSelected(true);
                    GL2JNILib.setViewerTargetAngles(-xAngle, yAngle - 90);
                    xAngle = 0;
                    yAngle = -90;
                }
            }
        });

        findViewById(R.id.imageButtonFront).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!v.isSelected()) {
                    clearRotationButtons();
                    v.setSelected(true);
                    GL2JNILib.setViewerTargetAngles(-xAngle, -yAngle);
                    xAngle = yAngle = 0; //reset
                }
            }
        });

        findViewById(R.id.imageButtonSide).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!v.isSelected()) {
                    clearRotationButtons();
                    v.setSelected(true);
                    GL2JNILib.setViewerTargetAngles(xAngle - 90, -yAngle);
                    xAngle = -90;
                    yAngle = 0;
                }
            }
        });

        findViewById(R.id.seekBar).setVisibility(View.GONE);
        sizeSliderBB = (SeekBar) findViewById(R.id.seekBar);

        sizeSliderBB.setMax(100);
        sizeSliderBB.setProgress(50);
        sizeSliderBB.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                GL2JNILib.setBBSize(sliderRangeToBBFloat(i));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        toggleButton = (MultiStateButton) findViewById(R.id.startStopGen);
        toggleButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(final View v) {
                v.setEnabled(false);
                MultiStateButton msb = (MultiStateButton) (v);
                switch (msb.getState()) {
                    case 0:
                        onButtonStartScan(v);
                        break;
                    case 1:
                        dismissHint();
                        onButtonStopScanning();
                        break;
                    case 2:
                        onButtonGenMesh(v);
                }

            }
        });
        ASOpen = false;

        // set handler
        UIHandler = new Handler();

        m_pointerArray = new int[10];
        for (int i = 0; i < 10; i++) {
            m_pointerArray[i] = -1;
        }

        //load assets for sound clip
        AudioAttributes audioAttr = new AudioAttributes.Builder()
                .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                .setUsage(AudioAttributes.USAGE_GAME)
                .build();

        soundPool = new SoundPool.Builder()
                .setMaxStreams(1)
                .setAudioAttributes(audioAttr)
                .build();

        soundEffectId = soundPool.load(this, R.raw.alert, 1);

    }

    private void initOrbbec() {

        // onResume() is called after the USB permission dialog is closed, in which case, we don't want
        // to request device permissions again
        if (mDeviceOpenPending) {
            return;
        }

        // Request opening the first OpenNI-compliant device found
        String uri;

        List<DeviceInfo> devices = OpenNI.enumerateDevices();
        if (devices.isEmpty()) {
            Log.d(TAG, "No OpenNI-compliant device found.");
            return;
        }

        uri = devices.get(devices.size() - 1).getUri();

        mDeviceOpenPending = true;
        mOpenNIHelper.requestDeviceOpen(uri, this);
    }

    @Override
    protected void onResume() {
        super.onResume();

        GL2JNILib.setDispFlag(false); //Switch to splatting

        for (int i = 0; i < 10; i++) {
            m_pointerArray[i] = -1;
        }

        if (!mIsInted)
            startModules();

        setJava();

        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            initOrbbec();
        } else {
            initCannedData();
        }

    }


    void initCannedData() {
        CannedDataIO.initCannedData();
        final Context context = this;
        Thread SensorThread = new Thread() {
            @Override
            public void run() {
                cannedDataStream = new CannedDataStream(context);
            }
        };

        SensorThread.start();
    }

    protected void DestroyActivityBecauseError() {
        destroyModules(true);
        finish();
    }


    class doneScanToCropModeAsyncTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected void onPreExecute() {
            super.onPreExecute();

            findViewById(R.id.seekBar).setVisibility(View.VISIBLE);
            findViewById(R.id.donut).setVisibility(View.GONE);

            // stop motion sensor
            if (vo_config[VO_Configuration.ConfigName.USE_MOTION_SENSOR]) {
                mMotionSensor.stop();
            }

            findViewById(R.id.editControls).setVisibility(View.VISIBLE);
            header.setText(Html.fromHtml(getResources().getString(R.string.editHeader)));

        }

        @Override
        protected void onPostExecute(Void aVoid) {
            super.onPostExecute(aVoid);
            glGuidanceRenderer.scanningStop = true;
            findViewById(R.id.waiting).setVisibility(View.GONE);
            findViewById(R.id.infoTextView).setVisibility(View.GONE);

            delayEnableMultiStateButton();

            int sliderVal = BBFloatToSliderValue(BB_DEFAULT_SIZE);
            sizeSliderBB.setProgress(sliderVal);
            imageButtonRotateFront.setSelected(true);

            GL2JNILib.setBBCenter(glGuidanceRenderer.originPointOfScan.x,
                    glGuidanceRenderer.originPointOfScan.y,
                    glGuidanceRenderer.originPointOfScan.z);

        }

        @Override
        protected Void doInBackground(Void... voids) {

            GL2JNILib.stopScan(initialSensorPitchDeg, initialSensorRollDeg);

            return null;
        }
    }

    public void onButtonStopScanning() {
        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            stopOrbbecStreams();
        } else {
            cannedDataStream.stop();
        }

        Log.i(TAG, "onButtonStopScanning()");
        TextView waitTitle = (TextView) findViewById(R.id.textViewWaiting);
        waitTitle.setText(R.string.action_refining_model);
        findViewById(R.id.waiting).setVisibility(View.VISIBLE);

        new doneScanToCropModeAsyncTask().execute();
    }

    public static void MeshReady(String modelID) {

        Log.i(TAG, "MeshReady() called, received modelID from native code = " + modelID);
        MeshViewerUtils.LauncherModelViewer(currentActivity, modelID);
        currentActivity.finish();
    }

    public void onButtonGenMesh(View view) {

        findViewById(R.id.waiting).setVisibility(View.VISIBLE);
        TextView waitTitle = (TextView) findViewById(R.id.textViewWaiting);
        waitTitle.setText(R.string.action_building_model);

        Log.i(TAG, "MESH INTEGRATION");
        GL2JNILib.tsdf();

    }

    public void onButtonStartScan(View view) {

        findViewById(R.id.uiHeader).setVisibility(View.VISIBLE);
        header.setText(Html.fromHtml(getResources().getString(R.string.scanHeader)));

        //Log.i(TAG, "destroy preview modules");
        //GL2JNILib.destroyPreview();

        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            orbecStream.mGrab = false;

            Log.i(TAG, "q/c before wait " + orbecStream.queueCnt + " / " + orbecStream.procCnt);
            while (orbecStream.queueCnt != orbecStream.procCnt) ;
            Log.i(TAG, "q/c after wait " + orbecStream.queueCnt + " / " + orbecStream.procCnt);

            orbecStream.queueCnt = 0;
            orbecStream.procCnt = 0;
        }

        GL2JNILib.init(getFilesDir().getPath(),
                "/sdcard/3DR",
                getResources().getAssets());

        glGuidanceRenderer.scanningStarted = true;

        delayEnableMultiStateButton();

        if (vo_config[VO_Configuration.ConfigName.USE_MOTION_SENSOR])
            mMotionSensor.start();

        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            orbecStream.mGrab = true;
            orbecStream.setScan();
        }


    }

    public void delayEnableMultiStateButton() {
        final Handler handler = new Handler();

        final Runnable r = new Runnable() {
            public void run() {
                toggleButton.setEnabled(true);

            }
        };

        handler.postDelayed(r, 1000);
    }

    public void onButtonSaveModel(View view) {
        GL2JNILib.saveModel();
        Log.i(TAG, "save model is returned");
    }

    public void onButtonCaptureKeyFrame(View view) {
        GL2JNILib.sendHighResKeyFrame();
    }

    public void onButtonToggleExposureLock(View view) {
        Button exposureBtn = (Button) view;

        autoExposureEnabled = !autoExposureEnabled;
        orbecStream.setAutoExposureEnabled(autoExposureEnabled);

        if (autoExposureEnabled)
            exposureBtn.setText("Auto Exp Enabled");
        else
            exposureBtn.setText("Auto Exp Disabled");
    }

    public void onButtonDumpFrame(View view) {

        Button dumpFramesBtn = (Button) view;

        dumpFramesEnabled = !dumpFramesEnabled;
        orbecStream.dumpFrames = dumpFramesEnabled;

        if (dumpFramesEnabled) {
            dumpFramesBtn.setText("Dump Frames Enabled");
            Toast.makeText(this, "Dumping ON", Toast.LENGTH_SHORT).show();
        } else {
            dumpFramesBtn.setText("Dump Frames Disabled");
            Toast.makeText(this, "Dumping OFF", Toast.LENGTH_SHORT).show();
        }


    }

    public boolean onPanAreaTouchEvent(MotionEvent event) {
        int index = event.getActionIndex();
        int x = (int) event.getX();
        int y = (int) event.getY();
        int button = -1;
        int numPointers = (int) event.getPointerCount();
        int pointerID = (int) event.getPointerId(index);

        //Log.i(TAG2, "Entered this section!\n");
        //
        // ACTION_DOWN and ACTION_UP only are generated by the first finger.
        // Therefore button = 0 in these cases.
        // We define button state as:  0 - up   and 1 - down
        //
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            for (int i = 0; i < 10; i++) {
                m_pointerArray[i] = -1;
            }

            button = 0;
            m_pointerArray[0] = pointerID;
            Log.d(TAG, "ACTION_DOWN mouseButtonFunc: btn: " + button + " state: " + BUTTON_STATE_DOWN + " x: " + x + " y: " + y);

            GL2JNILib.mouseButtonFunc(button, BUTTON_STATE_DOWN, x, y);
        }

        if (event.getActionMasked() == MotionEvent.ACTION_UP) {
            // find the index, given the ID. Clear out array,
            // all fingers are off.
            for (int i = 0; i < 10; i++) {
                if (m_pointerArray[i] == pointerID) {
                    button = i;
                    m_pointerArray[i] = -1;
                }
            }

            //Log.i(TAG, "Button LAST UP: " + button);
            Log.d(TAG, "ACTION_UP mouseButtonFunc: btn: " + button + " state: " + BUTTON_STATE_UP + " x: " + x + " y: " + y);

            GL2JNILib.mouseButtonFunc(button, BUTTON_STATE_UP, x, y);
        }

        if (event.getActionMasked() == MotionEvent.ACTION_POINTER_DOWN) {
            // find empty slot in pointerArray, assign ID, retrieve index
            // as button number.
            for (int i = 0; i < 10; i++) {
                if (m_pointerArray[i] == -1) {
                    m_pointerArray[i] = pointerID;
                    button = i;
                    break;
                }
            }

            int ptrIndex = event.findPointerIndex(pointerID);
            x = (int) event.getX(ptrIndex);
            y = (int) event.getY(ptrIndex);

            //Log.i(TAG, "Button  DOWN: " + button + " " + x + " " + y );
            GL2JNILib.mouseButtonFunc(button, 1, x, y);
        }

        if (event.getActionMasked() == MotionEvent.ACTION_POINTER_UP) {
            // find slot in pointerArray with pointerID, clear it out.
            for (int i = 0; i < 10; i++) {
                if (m_pointerArray[i] == pointerID) {
                    m_pointerArray[i] = -1;
                    button = i;
                    break;
                }
            }

            int ptrIndex = event.findPointerIndex(pointerID);
            x = (int) event.getX(ptrIndex);
            y = (int) event.getY(ptrIndex);


            GL2JNILib.mouseButtonFunc(button, 0, x, y);
        }

        //If a touch is moved on the screen
        if (event.getActionMasked() == MotionEvent.ACTION_MOVE) {

            for (int i = 0; i < numPointers; i++) {
                x = (int) event.getX(i);
                y = (int) event.getY(i);
                pointerID = event.getPointerId(i);

                // find button index
                for (int pointerIndex = 0; pointerIndex < 10; pointerIndex++) {
                    if (m_pointerArray[pointerIndex] == pointerID) {
                        button = pointerIndex;
                        break;
                    }
                }

                Log.d(TAG, "ACTION_MOVE mousePosFunc: btn: " + button + " x: " + x + " y: " + y);
                GL2JNILib.mousePosFunc(button, x, y);

            }

        }

        //We handled the event
        return true;
    }


    // ===== private functions =====
    private void startModules() {

        Log.i(TAG, "startModule is called \n");
        // motion sensor
        if (vo_config[VO_Configuration.ConfigName.USE_MOTION_SENSOR]) {
            if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
                mMotionSensor = new MotionSensorManager((SensorManager) getSystemService(SENSOR_SERVICE));
            } else {
                mMotionSensor = new MotionSensorManager();
            }
            //mMotionSensor.start();
        }
        //Query the Device Type
        String str = Build.BOARD.substring(3);
        Log.i(TAG, "TARGET is " + str);

        Log.i(TAG, "set JNI configuration");
        GL2JNILib.setJNIConfiguration(vo_config, str, cannedDataSetType.ordinal());

        mIsInted = true;

        findViewById(R.id.startStopGen).setVisibility(View.VISIBLE);
        GL2JNILib.setDispFlag(false); //Switch to splatting

    }

    @Override
    public void onDeviceOpened(Device aDevice) {
        Log.d(TAG, "Permission granted for device " + aDevice.getDeviceInfo().getUri());

        mDeviceOpenPending = false;

        mDevice = aDevice;

        //Find device ID
        List<DeviceInfo> devices = OpenNI.enumerateDevices();
        for (int i = 0; i < devices.size(); i++) {
            if (devices.get(i).getUri().equals(mDevice.getDeviceInfo().getUri())) {
                mActiveDeviceID = i;
                break;
            }
        }

        //these native cmds are very slow, not sure why. Also, the value seems to persist
        //when we read them back from the Windows console Orbbec app. So not needed to do everytime.
        //the getIrGainFromJNI function only reads back a local variable from the set command.
        //OrbecNative.setIrGainFromJNI(0x30);

        orbecStream = new OrbecStream(mDevice);





    }

    @Override
    public void onDeviceOpenFailed(String uri) {
        Log.e(TAG, "Failed to open device " + uri);
        mDeviceOpenPending = false;
    }

    private void destroyModules(boolean becauseError) {

        if (!modulesDestroyed) {
            modulesDestroyed = true;

            if (!becauseError) {
                findViewById(R.id.waiting).setVisibility(View.VISIBLE);
                TextView waitTitle = (TextView) findViewById(R.id.textViewWaiting);
                waitTitle.setText(R.string.action_deinit_camera);
            }

            glGuidanceRenderer.scanningQuit = true;

            new Thread(new SensorCloseTask()).start();

            clearJava();
        }

    }

    private void checkSensorAvailability() {
        if (vo_config[VO_Configuration.ConfigName.USE_MOTION_SENSOR]) {

            if (mMotionSensor != null) {
                // it is assumed the sensor is already started up and the data is available
                if (!mMotionSensor.checkDataAvailable()) {
                    Log.i(TAG, "Motion Sensor is not available");
                    Toast.makeText(this, "Motion Sensor is not available\n " +
                            "Please restart the device", Toast.LENGTH_LONG).show();
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    DestroyActivityBecauseError();
                } else {
                    Log.i(TAG, "SCVETEST: Motion Sensor is available in Java");
                    if (!mMotionSensor.checkDataAligned()) {
                        Toast.makeText(this, "Motion Sensor Alignment Error, Please Restart Scan", Toast.LENGTH_LONG).show();
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        DestroyActivityBecauseError();
                    }
                }
            }
        }
    }

    class SensorCloseTask implements Runnable {
        @Override
        public void run() {


            GL2JNILib.destroyModules();

            if (mIsInted)
                mIsInted = false;

            GL2JNILib.destroyDrawer();

            if (vo_config[VO_Configuration.ConfigName.USE_MOTION_SENSOR]) {
                if (mMotionSensor != null) {
                    mMotionSensor.stop();
                }
            }

            if (mMotionSensor != null) {
                mMotionSensor = null;
            }

        }

    }

    public void requestStopScanAS() {
        Log.i(TAG, "Stop Scan Requested Sensor TIMEOUT from native");

        //start handler thread for Display from JNI
        new Thread(new ToastDispASTIMEOUT()).start();

        UIHandler.post(new Runnable() {
            @Override
            public void run() {
                toggleButton.performClick(); //this will in turn trigger onButtonStopScanning
            }
        });
    }

    void stopScanAS() {

        Toast.makeText(getApplicationContext(), "Sensor TIMEOUT: Too Many Void Frames! Are your too Close to Object? Please Press Generate or Restart Scan.", Toast.LENGTH_LONG).show();
        Toast.makeText(getApplicationContext(), "Sensor TIMEOUT: Too Many Void Frames! Are your too Close to Object? Please Press Generate or Restart Scan.", Toast.LENGTH_LONG).show();
        //DestroyActivityBecauseError();

    }

    public void DisplayTrackerLost() {


        //start handler thread for Display from JNI
        new Thread(new ToastDispTrackLost()).start();

        //DestroyActivityBecauseError();
        UIHandler.post(new Runnable() {
            @Override
            public void run() {
                toggleButton.performClick(); //this will in turn trigger onButtonStopScanning
            }
        });

    }

    public void clickButton(){
        UIHandler.post(new Runnable() {
            @Override
            public void run() {
                toggleButton.performClick();
            }
        });
    }

    public void scveFrameProcessed() {

        if (vo_config[VO_Configuration.ConfigName.USE_LIVE_TRACKING]) {
            orbecStream.procCnt++;
        } else {
            cannedDataStream.scveSingleFrameProcessedCb();
        }

    }

    void DispToastTrackerLost() {
        Toast.makeText(getApplicationContext(), "3DR: Tracking Lost! Please Restart Scan or Continue to Model Generation", Toast.LENGTH_LONG).show();
        Toast.makeText(getApplicationContext(), "3DR: Tracking Lost! Please Restart Scan or Continue to Model Generation", Toast.LENGTH_LONG).show();
    }

    public void requestMeshReady(String modelID) {
        Log.i(TAG, "Mesh Ready: Launching Viewer");
        MeshReady(modelID);
    }

    public void SSD_processing_new_KF() {

        //Log.i(TAG, "SSD native code reporting new KF ready");

        if (mGuidanceEnabled) {
            glGuidanceRenderer.newKeyFrameFlag = true;
        }

    }

    class ToastDispTrackLost implements Runnable {
        @Override
        public void run() {

            // check for camera
            UIHandler.post(new Runnable() {
                @Override
                public void run() {
                    DispToastTrackerLost();
                }
            });
        }
    }

    class ToastDispASTIMEOUT implements Runnable {
        @Override
        public void run()

        {

            // check for camera
            UIHandler.post(new Runnable() {
                @Override
                public void run() {
                    stopScanAS();

                }
            });
        }

    }

    boolean mContinuousPress = false;
    final int STEP_SIZE = 5;




    public native void setJava();
    public native void clearJava();



}

