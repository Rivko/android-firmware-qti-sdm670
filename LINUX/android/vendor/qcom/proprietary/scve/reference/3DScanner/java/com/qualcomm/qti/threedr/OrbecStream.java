/*****************************************************************************
 * *
 * OpenNI 2.x Alpha                                                          *
 * Copyright (C) 2012 PrimeSense Ltd.                                        *
 * *
 * This file is part of OpenNI.                                              *
 * *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 * *
 * http://www.apache.org/licenses/LICENSE-2.0                            *
 * *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * *
 *****************************************************************************/
package com.qualcomm.qti.threedr;

import android.content.Context;
import android.util.Log;

import com.qualcomm.qti.threedr.utils.Constants;

import org.openni.Device;
import org.openni.OpenNI;
import org.openni.PixelFormat;
import org.openni.SensorType;
import org.openni.VideoFrameRef;
import org.openni.VideoMode;
import org.openni.VideoStream;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeoutException;

public class OrbecStream {

    private static final String TAG = "Scan3D";
    private static SensorType[] SENSORS = {SensorType.DEPTH, SensorType.COLOR, SensorType.IR};
    private static CharSequence[] SENSOR_NAMES = {"Depth", "Color", "IR"};

    private Thread mOrbecGrabThread;

    private boolean mShouldRun = true;
    private Device mDevice;
    private VideoStream mStreamColor;
    private VideoStream mStreamDepth;
    public boolean mGrab = true;

    public int queueCnt = 0;
    public int procCnt = 0;

    public boolean dumpFrames = false;
    private boolean isScan;
    private int dumpCnt = 0;


    public OrbecStream(Device mDevice) {
        this.mDevice = mDevice;

        initialize();
    }

    public void setAutoExposureEnabled(boolean enable) {
        mStreamColor.getCameraSettings().setAutoExposureEnabled(enable);

    }

    public void setScan(){
        isScan = true;
        Log.i(TAG,"IS SCAN IS SET "+isScan);
    }

    public void setNoScan(){
        isScan = false;
    }


    private void initialize() {

        try {

            if (mStreamColor != null) {
                mStreamColor.destroy();
                mStreamColor = null;
            }
            if (mStreamDepth != null) {
                mStreamDepth.destroy();
                mStreamDepth = null;
            }

            VideoMode vmode = null;

            mStreamColor = VideoStream.create(mDevice, SensorType.COLOR);
            vmode = getVideoMode(640,480,mStreamColor,PixelFormat.RGB888);
            mStreamColor.setVideoMode(vmode);
            mStreamColor.setMirroringEnabled(false);

            mStreamColor.start();

            mStreamDepth = VideoStream.create(mDevice, SensorType.DEPTH);
            vmode = getVideoMode(640,480,mStreamDepth,PixelFormat.DEPTH_100_UM);
            mStreamDepth.setVideoMode(vmode);
            mStreamDepth.setMirroringEnabled(false);
            mStreamDepth.start();


            mDevice.setDepthColorSyncEnabled(true);

            startStream();

            VideoStream.CameraSettings camSettings = mStreamColor.getCameraSettings();
            int camGain = camSettings.getGain();
            Log.i(TAG,"rgb default gain = " + camGain);


        } catch (RuntimeException e) {
            Log.d(TAG, "Failed to switch to stream: " + e.getMessage());
        }

    }

    private VideoMode getVideoMode(int width, int height, VideoStream stream, PixelFormat pixFormat) {

        List<VideoMode> mStreamVideoModes = stream.getSensorInfo().getSupportedVideoModes();

        int vidModeIndex;
        for (vidModeIndex = 0; vidModeIndex < mStreamVideoModes.size(); ++vidModeIndex) {
            VideoMode mode = mStreamVideoModes.get(vidModeIndex);

            if (mode.getResolutionX() == width
                    && mode.getResolutionY() == height
                    && mode.getFps() == 30) {
                if (mode.getPixelFormat() == pixFormat) {
                    return mode;
                }
            }
        }
        Log.d(TAG, "Error: no matching video mode found");
        return null;

    }

    public void stop() {
        mShouldRun = false;
        isScan = false;

        while (mOrbecGrabThread != null) {
            try {
                mOrbecGrabThread.join();
                mOrbecGrabThread = null;
                break;
            } catch (InterruptedException e) {
            }
        }

        if (mStreamColor != null) {
            mStreamColor.stop();
        }
        if (mStreamDepth != null) {
            mStreamDepth.stop();
        }

    }

    byte[] depthSensorByteArray = new byte[640 * 480 * 2];
    short[] depthSensorShortArray = new short[640 * 480];

    byte[] rgbBuffer = new byte[640 * 480 * 3]; //3 bytes per pixel
    //byte[] yuv420Buffer = new byte[640 * 480 * 3 / 2];

    private void startStream() {


        /*
        //for fake depth test
        for (int i =0; i<depthSensorShortArray.length-1; i++) {
            depthSensorShortArray[i] = (short) 6000;//(short) 6000;
        }
        */


        mShouldRun = true;

        mOrbecGrabThread = new Thread() {
            @Override
            public void run() {

                List<VideoStream> streamsColor = new ArrayList<VideoStream>();
                List<VideoStream> streamsDepth = new ArrayList<VideoStream>();

                streamsColor.add(mStreamColor);
                streamsDepth.add(mStreamDepth);

                long lastTime = System.nanoTime();
                long frameCount = 0;
                long skippedFrames = 0;
                int fps = 0;

                VideoFrameRef frameColor = null;
                ByteBuffer frameDataColor = null;
                VideoFrameRef frameDepth = null;
                ByteBuffer frameDataDepth = null;

                long timeStampColor = 0;
                long timeStampDepth = 0;

                int skipCount = 0;
                int skipFrames = 0; //0 for no skip frames, 1 for 50%, etc

                boolean first = true;

                while (mShouldRun) {

                    try {

                        OpenNI.waitForAnyStream(streamsColor, 100);
                        frameColor = mStreamColor.readFrame();

                        OpenNI.waitForAnyStream(streamsDepth, 100);
                        frameDepth = mStreamDepth.readFrame();

                        if (mGrab && (queueCnt == procCnt)) {
                            frameDataColor = frameColor.getData();
                            //timeStampColor = frameColor.getTimestamp();
                            frameDataDepth = frameDepth.getData();
                            timeStampDepth = frameDepth.getTimestamp();

                            //long diff = Math.abs(timeStampColor - timeStampDepth);

                            frameDataColor.get(rgbBuffer);

                            if (dumpFrames) {

                                frameDataDepth.get(depthSensorByteArray);

                                BufferedOutputStream bos1 = new BufferedOutputStream(new FileOutputStream(Constants.DUMP_FOLDER + "/rgb888_640x480_" + dumpCnt + ".bin"));
                                bos1.write(rgbBuffer);
                                bos1.flush();
                                bos1.close();

                                BufferedOutputStream bos2 = new BufferedOutputStream(new FileOutputStream(Constants.DUMP_FOLDER + "/depth_short_640x480_" + dumpCnt + ".bin"));
                                frameDataDepth.rewind();
                                bos2.write(depthSensorByteArray);
                                bos2.flush();
                                bos2.close();
                                Log.i(TAG," orbbec dump saved idx = "+dumpCnt);
                                dumpCnt++;
                            }


                            ShortBuffer shortBufDepth = ((ByteBuffer) frameDataDepth.rewind()).asShortBuffer();
                            shortBufDepth.get(depthSensorShortArray);

                            boolean status = GL2JNILib.submitColorAndDepth(rgbBuffer, MainActivity.ColorImageWidth, MainActivity.ColorImageHeight,
                                    depthSensorShortArray, MainActivity.DepthImageWidth, MainActivity.DepthImageHeight,
                                    timeStampDepth, 0/*sync timestamp or unix timestamp will be assigned in JNI*/, MainActivity.isOrbbecSensor);

                            if(status) {
                                skippedFrames = 0;
                                queueCnt++;

                                ++frameCount;

                                if (frameCount == 30) {
                                    long now = System.nanoTime();
                                    long diffr = now - lastTime;
                                    fps = (int) (1e9 * 30 / diffr);
                                    frameCount = 0;
                                    lastTime = now;
                                }

                                //Log.i(TAG,"RGBD frame submit fps = " + fps);
                            }else{
                                //Count Skipped Frames
                                ++skippedFrames;
                                //Too Many Skipped Frames, Display Message and Exit
                                if(skippedFrames>20 && isScan) {
                                    GL2JNILib.AbortScanTimeOut();
                                }

                            }


                        }
                        else {

                            frameColor.release();
                            frameDepth.release();

                        }

                        //Log.i(TAG,"mGrab = " + mGrab + " queueCnt = " + queueCnt + " procCnt = " + procCnt);


                    } catch (TimeoutException e) {

                    } catch (Exception e) {
                        Log.e(TAG, "Failed reading frame: " + e);
                    }
                }
            }

        };

        mOrbecGrabThread.setName("OrbecStream grab thread");
        mOrbecGrabThread.start();
    }


    public void dumpCurrentFrame() {
        dumpFrames = true;
    }
}
