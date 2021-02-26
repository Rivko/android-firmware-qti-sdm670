/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.facerecognition;

import java.nio.charset.Charset;

public class FaceRecognitionResult {
    public static final int SCVE_FACE_RECOGNITION_FEATURE_ID_LEN = 20;

    public float  matchScore;
    public byte[] pid;

    public FaceRecognitionResult()
    {
        pid = new byte[SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];
    }

    public FaceRecognitionResult(FaceRecognitionResult instance)
    {
        pid = new byte[SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];
        setResult(instance);
    }

    public void setResult(FaceRecognitionResult instance)
    {
        int length = (pid.length < instance.pid.length) ? pid.length : instance.pid.length;
        for (int i=0; i<length; i++)
        {
            pid[i] = instance.pid[i];
        }
        matchScore = instance.matchScore;
    }

    public String toString()
    {
        Charset charset = Charset.forName("UTF-8");
        int length = 0;
        while (length < SCVE_FACE_RECOGNITION_FEATURE_ID_LEN && pid[length] != 0)
        {
            length++;
        }
        return new String(pid, 0, length, charset);
    }
}
