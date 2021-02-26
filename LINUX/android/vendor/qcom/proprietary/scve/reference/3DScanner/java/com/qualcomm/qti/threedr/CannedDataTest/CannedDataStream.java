/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.CannedDataTest;

import android.content.Context;
import android.util.Log;

import com.qualcomm.qti.threedr.GL2JNILib;
import com.qualcomm.qti.threedr.MainActivity;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class CannedDataStream {

    private static final String TAG = "Scan3D";
    Context mContext;
    //byte[] depthSensorByteArray = new byte[640 * 480 * 2];
    short[] depthSensorShortArray = new short[MainActivity.DepthImageWidth * MainActivity.DepthImageHeight];
    byte[] rgbBuffer = new byte[MainActivity.ColorImageWidth * MainActivity.ColorImageHeight * 3]; //3 bytes per pixel
    long frameSubmitTime = 0;
    long startTime = 0;
    long endTime = 0;
    private Thread mCannedDataGrabThread;
    private boolean mShouldRun = true;

    /*this flag is used to synchronize between java frame grab and scve processing,
    * after a frame is submitted to SCVE lib this flag will be false, and once SCVE lib
    * completes processing it will call the callback method which will set this flag to true*/
    private boolean submitNextFrame = true;

    public CannedDataStream(Context context) {
        mContext = context;
        initialize();
    }

    private void initialize() {
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        startStream();
    }

    public void stop() {
        mShouldRun = false;

        while (mCannedDataGrabThread != null) {
            try {
                mCannedDataGrabThread.join();
                mCannedDataGrabThread = null;
                break;
            } catch (InterruptedException e) {
            }
        }
    }

    int lastProcessFramIdx = -1;
    private void startStream() {
        Log.i(TAG, "canneddata startStream");
        mShouldRun = true;
        mCannedDataGrabThread = new Thread() {
            @Override
            public void run() {
                startTime = System.currentTimeMillis();
                while (mShouldRun) {
                    //submit RGBD data to SCVE lib

                    CannedDataIO.GrabResult grabResult = CannedDataIO.getInstance().grab();
                    if(lastProcessFramIdx==15){
                        ((MainActivity)mContext).clickButton();//this will start scan just before we passing first frame
                    }
                    if (grabResult != null) {
                        //grabResult.yuvImage.getYuvData();
                        //yuv420Buffer = Arrays.copyOf(grabResult.yuvImage.getYuvData(), grabResult.yuvImage.getYuvData().length);
                        rgbBuffer = grabResult.rgbImage;
                        //convert the depth image byte array to short array
                        ByteBuffer.wrap(grabResult.depthImage).order(ByteOrder.LITTLE_ENDIAN).asShortBuffer().get(depthSensorShortArray);

                        //Wait for the scve call back to be called i.e after processing the previous frame submitted
                        while (!submitNextFrame) {
                            try {
                                Thread.sleep(50);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        submitNextFrame = false;

                        GL2JNILib.submitColorAndDepth(rgbBuffer, MainActivity.ColorImageWidth, MainActivity.ColorImageHeight,
                                depthSensorShortArray, MainActivity.DepthImageWidth, MainActivity.DepthImageHeight,
                                grabResult.timestamp, grabResult.syncTimestamp, MainActivity.isOrbbecSensor);
                        long temp = System.currentTimeMillis();
                        lastProcessFramIdx = grabResult.frameIdx;
                        Log.i(TAG, "canneddata frame idx = "+lastProcessFramIdx+" fps = " + 1000 / (float) (temp - frameSubmitTime));
                        frameSubmitTime = temp;
                        grabResult.rgbImage = null;
                        grabResult.depthImage = null;
                        grabResult = null;
                    } else {
                        ((MainActivity)mContext).clickButton();//this will stop scanning
                        endTime = System.currentTimeMillis();
                        Log.i(TAG, "Canneddata Avg frame rate = " + (1000 * (lastProcessFramIdx+1) ) / (float) (endTime - startTime) + " fps for " + (lastProcessFramIdx+1) + " frames");
                        break;
                    }
                }

            }
        };

        mCannedDataGrabThread.setName("CannedData MainLoop Thread");
        mCannedDataGrabThread.start();
    }

    public void scveSingleFrameProcessedCb() {
        submitNextFrame = true;
    }

}
