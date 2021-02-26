/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils;

import android.content.Context;
import android.util.Log;

import java.util.Calendar;

import qces.qualcomm.qti.com.videosummary.face.FaceRecognitionResult;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;

/**
 * Created by mithun on 12/15/15.
 */
public class FRUtils {
    private static final String TAG = FRUtils.class.getSimpleName();

    public static byte[] generateFRDatasetID(Context context) {
        byte[] key = null;
        do {
            key = getBytesForFRDatasetID(Long.toString(Calendar.getInstance().getTimeInMillis()));
            Log.e(TAG, "FrDatasetId collision, so generation new one =" + key);
        } while (FRQueryHelper.queryEnrolledFaceName(context, key) != null);
        return key;
    }

    static byte[] getBytesForFRDatasetID(String randomString) {
        byte[] dsId = new byte[FaceRecognitionResult.SCVE_FACE_RECOGNITION_FEATURE_ID_LEN];
        int length = randomString.length();

        int index = 0;
        while (index < FaceRecognitionResult.SCVE_FACE_RECOGNITION_FEATURE_ID_LEN && index < length) {
            dsId[index] = (byte) randomString.charAt((length - 1) - index);
            index++;
        }

        while (index < FaceRecognitionResult.SCVE_FACE_RECOGNITION_FEATURE_ID_LEN) {
            dsId[index] = (byte) '*';
            index++;
        }
        return dsId;
    }
}
