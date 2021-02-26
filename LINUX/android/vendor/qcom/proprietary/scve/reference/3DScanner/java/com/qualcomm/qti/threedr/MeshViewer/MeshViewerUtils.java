/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.MeshViewer;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;
import com.qualcomm.qti.threedr.modelbrowser.ModelInfo;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class MeshViewerUtils {

    final static String TAG = "MeshViewerUtils";

    final static String _ModelViewerPackage = "com.qualcomm.qti.threedr";
    final static String _ModelViewerActivity = "com.qualcomm.qti.threedr.MeshViewer.MeshViewerActivity";

    public static final String MP4_EXTENSION = "_3DR.mp4";


    public static void LauncherModelViewer(Context context, String modelId) {

        String directory = BrowserUtils.ParentFolderPath + "/" + modelId;

        File f = new File(directory);

        ModelInfo modelInfo = new ModelInfo(f.getParent(), f.getName());
        Intent intent = new Intent()
                .setClassName(_ModelViewerPackage, _ModelViewerActivity)
                .putExtra(BrowserUtils.EXTRA_MODELINFO, modelInfo);

        context.startActivity(intent);

    }

    public static String ReadAssetFile(Context context, String fileName) {
        String result = null;
        StringBuilder stringBuilder = null;
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(context.getAssets().open(fileName)));

            stringBuilder = new StringBuilder();

            while (true) {
                String cLine = reader.readLine();
                if (cLine == null) break;
                stringBuilder.append(cLine);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (stringBuilder != null) {
            result = stringBuilder.toString();
        }

        return result;
    }

    public static String ReadFile(String fileName) {
        String result = null;
        StringBuilder stringBuilder = null;
        try {
            BufferedReader reader = new BufferedReader(new FileReader(fileName));

            stringBuilder = new StringBuilder();

            while (true) {
                String cLine = reader.readLine();
                if (cLine == null) break;
                stringBuilder.append(cLine);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (stringBuilder != null) {
            result = stringBuilder.toString();
        }

        return result;
    }

    public static void ShareFile(Context context, File file) {
        ArrayList<Uri> uris = new ArrayList<Uri>();
        uris.add(Uri.fromFile(file));
        Log.e("X3", "zipFile = " + file.getAbsolutePath());
        String screenshotFilename = file.getAbsolutePath().replace(".roa", ".jpg");
        screenshotFilename = screenshotFilename.replace(".zip", ".jpg");
        screenshotFilename = screenshotFilename.replace(".mp4", ".jpg");
        Log.e("X3", "screenshotFile = " + screenshotFilename);
        File screenshotFile = new File(screenshotFilename);
        if (screenshotFile.exists()) {
            uris.add(Uri.fromFile(screenshotFile));
        } else {
            Log.e("X3", "screenshotFile doesn't exist - " + screenshotFile.getAbsolutePath());
        }
        EmailComposer.Send(context, file.getName(), uris);
    }

    public static void ShareFile(Context context, String fileStr) {
        ShareFile(context, new File(fileStr));
    }

    public static boolean AllGranted(int[] grantResults) {
        for (int grant : grantResults) {
            if (grant == PackageManager.PERMISSION_DENIED)
                return false;
        }

        return true;
    }

    public static void Notify(final Context context, final String msg) {
        Log.e("X3", " Notify " + msg);
        Activity activity = (Activity) (context);
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, msg, Toast.LENGTH_LONG).show();
            }
        });

        Log.e("X3", " Notify " + msg);
    }


    /**
     * MP4 file parameters
     */

    private static int BIT_RATE = 8000000;   // 8Mbps
    private static int VIDEO_WIDTH = 1280;
    private static int VIDEO_HEIGHT = 720;
    private static int FRAME_RATE = 60;
    private static int TOTAL_VIDEO_TIME_SECONDS = 9;//6;
    private static float TOTAL_ROTATION_DEGREES = 720f;

    public static final int TOTAL_FRAMES = TOTAL_VIDEO_TIME_SECONDS * FRAME_RATE;
    private static float DEGREES_PER_FRAME = (TOTAL_ROTATION_DEGREES / (float) TOTAL_VIDEO_TIME_SECONDS) / (float) FRAME_RATE;

    private static TextureMovieEncoder2 mVideoEncoder;
    private static WindowSurface mInputWindowSurface;
    private static EglCore mEglCore;
    private static ProgressDialog progressDialog;

    /**
     * Generates the presentation time for frame N, in nanoseconds.
     */
    private static long computePresentationTimeNsec(int frameIndex, int frame_rate) {
        final long ONE_BILLION = 1000000000;
        return frameIndex * ONE_BILLION / frame_rate;
    }

    /***
     * Processing code below for MP4 creation without any display. This uses OpenGL
     * and gets a Surface object from the MediaCodec Encoder and manually creates an EGL Context
     * for recording if one doesn't already exist.
     */
    public static void setupGLandMP4forMeshModel(ModelInfo mModelInfo, boolean alreadyInited, EglCore eglCore) {
        Log.d(TAG, "starting to record");

        File outputFile = new File(mModelInfo.getFile().getAbsolutePath() + "/" +
                mModelInfo.getName() + MP4_EXTENSION);

        VideoEncoderCore encoderCore;
        try {
            encoderCore = new VideoEncoderCore(VIDEO_WIDTH, VIDEO_HEIGHT,
                    BIT_RATE, outputFile);
        } catch (IOException ioe) {
            throw new RuntimeException(ioe);
        }

        if (eglCore == null)
            mEglCore = new EglCore(null, EglCore.FLAG_RECORDABLE);
        else
            mEglCore = eglCore;

        mInputWindowSurface = new WindowSurface(mEglCore, encoderCore.getInputSurface(), true);
        mVideoEncoder = new TextureMovieEncoder2(encoderCore);

        mInputWindowSurface.makeCurrent();

        if (alreadyInited)
            GLViewerJNILib.setViewer(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
        else
            GLViewerJNILib.init(VIDEO_WIDTH, VIDEO_HEIGHT, mModelInfo.getFile().getAbsolutePath(),
                    mModelInfo.getModelID(), mModelInfo.getNumTextures());


    }


    public static void renderAndEncodeSingleFrame(int frameIndex) {

        long presTimeNs = computePresentationTimeNsec(frameIndex, FRAME_RATE);
        //Log.d(TAG, "frame: " + frameIndex + " presTimeNs: " + presTimeNs);
        frameIndex++;
        mVideoEncoder.frameAvailableSoon();

        if (frameIndex == TOTAL_FRAMES / 2)
            GLViewerJNILib.setMeshMode(1);

        GLViewerJNILib.setRotationAndDraw(DEGREES_PER_FRAME, 0);
        mInputWindowSurface.setPresentationTime(presTimeNs); //calc here
        boolean swapResult = mInputWindowSurface.swapBuffers(); //publish
        if (!swapResult)
            Log.d(TAG, "problem on inputsurface swapbuf");

    }

    public static void cleanupAfterEncoding() {
        mVideoEncoder.stopRecording();
        GLViewerJNILib.setMeshMode(0);
        mInputWindowSurface.release();
    }
}
