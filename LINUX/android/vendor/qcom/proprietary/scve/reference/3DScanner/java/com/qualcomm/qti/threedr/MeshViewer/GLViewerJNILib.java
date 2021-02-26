/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.MeshViewer;

// Wrapper for native library
public class GLViewerJNILib {

     static {
         System.loadLibrary("glviewer");
     }

    /**
     * @param width the current view width
     * @param height the current view height
     */
     public static native void init(int width, int height, String directory, String modelName, int numTextures);
     public static native void step();
     public static native void shutdown();

     public static native void mouseButtonFunc(int button, 
    		 								   int state,
    		 								   int x,
    		 								   int y);
     public static native void mousePosFunc(int button, int x, int y);
     public static native void setMeshMode(int meshMode);

     public static native void setRotationAndDraw(float rotX, float rotY);
     public static native void setViewer(int X, int Y, int width, int height);

     public static native void lockRotationAngle();
}
