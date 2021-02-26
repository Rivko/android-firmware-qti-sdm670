/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.face;

import android.graphics.Bitmap;
import android.util.Log;

/**
 * Created by matthewf on 12/9/15.
 */
public class FaceLibIntf {
    public FaceLibIntf() {
        try
        {
            System.loadLibrary("fastcvopt");
            System.loadLibrary("scveCommon");
            System.loadLibrary("scveFaceRecognition");
            System.loadLibrary("face");
            nClassInit();
        }
        catch( UnsatisfiedLinkError e )
        {
            Log.d("FaceLibIntf", "Native code library failed to load.\n" + e);
        }
    }

    private long nativeContext; // Do not modify from Java (only from native code)

    private static native void nClassInit();

    public native int nCreateScveContext(int operationMode);

    public native int nFreeScveContext();

    public native int nInit(int maxSize, int maxFacesPerImage, int maxNumUsers, String databaseName);

    public native int nFreeFaceRecognition();

    public native int nDetectFaces(Bitmap bitmap, FaceInfo[] faceInfo);

    public native int nRecognizeFace(Bitmap bitmap, FaceInfo faceInfo, int nMaxResults,
                                            FaceRecognitionResult[] results);

    public native int nEnrollFace(Bitmap bitmap, FaceInfo faceInfo, byte[] featureID);

    public native int nRemoveFromDatabase(byte[] featureID);

    public native int nSaveDatabase(String databaseName);

    public native int nGetNumDatabaseUsers();
}
