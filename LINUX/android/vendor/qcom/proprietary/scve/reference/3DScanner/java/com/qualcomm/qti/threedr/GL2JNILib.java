/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

import android.content.res.AssetManager;

// Wrapper for native library

public class GL2JNILib {

     static {
       System.loadLibrary("3DR");
         System.loadLibrary("3DRgl2jni");
     }

    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init(String filesdir, String externalFilesDir, AssetManager am);
     public static native void initPreview(String filesdir,AssetManager am);
     public static native void initDrawer(int width, int height,
                                          int hd_camWidth,int hd_camHeight,
                                          int depthImgWidth, int depthImgHeight,
                                   AssetManager am);
     public static native void destroyPreview();
     public static native void destroyModules();
     public static native void destroyDrawer();

     public static native void step(boolean stopScan,
                           boolean update);
     public static native void stepPreview();
     public static native void stopScan(float initPitch, float initRoll);
     public static native void saveModel();

     public static native void tsdf();
     public static native void toggleAdjustButton();
     public static native void mouseButtonFunc(int button,
                                    int state,
                                    int x,
                                    int y);
     public static native void mousePosFunc(int button,
                                 int x,
                                 int y);
     public static native void captureHRImage(byte[] hrImg,
                                              int    hrImgWidth,
                                              int    hrImgHeight);
     public static native void setJNIConfiguration(boolean vo_config[], String TARGET, int cannedDatasetType);
     public static native void sendHighResKeyFrame();

     // sensor
     public static native void getSensorReading(float gyro[],
                                         float rotMat[],
                                         long currTimeStamp[],
                                         int size);

     public static native void captureHRPreviewImage(byte[] hrImg,
             int    hrImgWidth,
             int    hrImgHeight);

     public static native void setDispFlag(boolean showImage);

     public static native int getSelectedKeyFrameID();
     public static native void toggleRenderMode(boolean is_splatting);

     public static native void setViewerTargetAngles(float x, float y);

     public static native float getCameraData(float[] camera_matrix);

     public static native boolean getBoundingBox(float[] bb_info);

     public static native void setBBSize(float size);

     public static native void setBBCenter(float x, float y, float z);

     public static native boolean submitColorAndDepth(byte[] color, int colorImgWidth, int colorImgHeight,
                                                   short[] depth, int depthImgWidth, int depthImgHeight,
                                                   long timestamp, long syncTimestamp, boolean isOrbbecSensor);
     public static native void AbortScanTimeOut();



}
