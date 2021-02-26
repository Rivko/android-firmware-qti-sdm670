/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.facerecognition;

import java.nio.ByteBuffer;

public class FaceRecognition {
    public static final int SCVE_SUCCESS = 0;

    public static final int SCVE_MODE_DEFAULT = 0;
    public static final int SCVE_MODE_CPU_OFFLOAD = 1;
    public static final int SCVE_MODE_PERFORMANCE = 2;
    public static final int SCVE_MODE_LOW_POWER = 3;

    public static final int SCVE_COLORFORMAT_GREY_8BIT = 0;
    public static final int SCVE_COLORFORMAT_NV21      = 1;
    public static final int SCVE_COLORFORMAT_NV12      = 2;

    public static final int SCVE_FACERECOGNITION_STILL_IMAGE = 0;
    public static final int SCVE_FACERECOGNITION_VIDEO       = 1;

    static {
        System.loadLibrary("FaceRecognitionJNI");

        nClassInit();
    }

    public FaceRecognition()
    {}

    private long nativeContext; // N.B. Do not modify from Java (only from native code)
    private static native void nClassInit();

    public native int nCreateScveContext(int operationMode);
    public native int nFreeScveContext();

    // frMode option: SCVE_FACERECOGNITION_STILL_IMAGE or SCVE_FACERECOGNITION_VIDEO
    public native int nCreateFaceRecognition(String databaseName, int frMode);
    public native int nFreeFaceRecognition();

    public native int nInit(int colorFormat, int width, int height, int lumaStride, int chromaStride, int maxFacesPerImage, int maxNumUsers);

    //Detect Faces and stored detected face info into faceInfo
    // return number of results or negative number if error
    public native int nDetectFaces(byte[] imageData, FaceInfo[] faceInfo);

    //Variant of nDetectFaces which accepts a direct ByteBuffer
    public native int nDetectFaces(ByteBuffer imageData, FaceInfo[] faceInfo);

    //Recognize Face
    // return number of results or negative number if error
    public native int nRecognizeFace(byte[] imageData, FaceInfo faceInfo, int nMaxResults, FaceRecognitionResult []results);

    //Variant of nRecognizeFace which accepts a direct ByteBuffer
    public native int nRecognizeFace(ByteBuffer imageData, FaceInfo faceInfo, int nMaxResults, FaceRecognitionResult []results);

    //Enroll Face to the database
    public native int nEnrollFace(byte[] imageData, FaceInfo faceInfo, byte[] featureID);

    //Variant of nEnrollFace which accepts a direct ByteBuffer
    public native int nEnrollFace(ByteBuffer imageData, FaceInfo faceInfo, byte[] featureID);

    //Get the IDs of enrolled samples from the database, retun the number of count
    // public native int nGetEnrolledDatabseIDS(FaceRecognitionResult []result);

    //Delete from Database
    public native int nRemoveFromDatabase(byte[] featureID);

    //Save Database
    public native int nSaveDatabase(String databaseName);

    //Get DB - num users, return number of users or negative number if error
    public native int nGetNumDatabaseUsers();

}
