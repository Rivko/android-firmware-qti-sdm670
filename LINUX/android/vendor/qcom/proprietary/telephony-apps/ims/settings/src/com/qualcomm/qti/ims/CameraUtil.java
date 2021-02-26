/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims;

import android.content.Context;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.util.Log;
import android.util.Size;

import java.util.HashMap;

/**
 * This class contains Camera utility functions.
 */
public class CameraUtil {

    private static String TAG = "CameraUtil";
    private static CameraUtil sCameraUtil;
    /**
     * The ID for the front facing camera.
     */
    private int mFrontFacingCameraId = -1;

    /**
     * Indicates whether the list of cameras has been initialized yet.
     */
    private boolean mIsInitialized = false;

    private CameraCharacteristics mCharacteristics;
    private static final HashMap<Size, Integer>
            VIDEO_QUALITY_TABLE = new HashMap<Size, Integer>();

    private static final int VIDEO_QUALITY_UNKNOWN = -1;

    static {
        VIDEO_QUALITY_TABLE.put(new Size(4096,2160), CamcorderProfile.QUALITY_4KDCI);
        VIDEO_QUALITY_TABLE.put(new Size(3840,2160), CamcorderProfile.QUALITY_2160P);
        VIDEO_QUALITY_TABLE.put(new Size(2560,1440), CamcorderProfile.QUALITY_QHD);
        VIDEO_QUALITY_TABLE.put(new Size(2048,1080), CamcorderProfile.QUALITY_2k);
        VIDEO_QUALITY_TABLE.put(new Size(1920,1080), CamcorderProfile.QUALITY_1080P);
        VIDEO_QUALITY_TABLE.put(new Size(1280,720), CamcorderProfile.QUALITY_720P);
        VIDEO_QUALITY_TABLE.put(new Size(720,480), CamcorderProfile.QUALITY_480P);
        VIDEO_QUALITY_TABLE.put(new Size(640,480), CamcorderProfile.QUALITY_VGA);
        VIDEO_QUALITY_TABLE.put(new Size(352,288), CamcorderProfile.QUALITY_CIF);
        VIDEO_QUALITY_TABLE.put(new Size(320,240), CamcorderProfile.QUALITY_QVGA);
        VIDEO_QUALITY_TABLE.put(new Size(176,144), CamcorderProfile.QUALITY_QCIF);
    }

    /**
     * Private constructor. Must use getInstance() to get this singleton.
     */
    private CameraUtil() {
    }

    /**
     * This method returns a singleton instance of {@class CameraUtil}
     */
    public static synchronized CameraUtil getInstance() {
        if (sCameraUtil == null) {
            sCameraUtil = new CameraUtil();
        }
        return sCameraUtil;
    }

    /* This API returns whether front camera supports High video quality */
    public boolean isHighVideoQualitySupported(Context context) {
        maybeInitializeCameraList(context);
        if (mFrontFacingCameraId != -1) {
            final int quality = getHighestSupportedVideoQuality(mFrontFacingCameraId);
            Log.d(TAG, "Highest supported video quality: " + quality);
            return (quality == CamcorderProfile.QUALITY_4KDCI ||
                    quality == CamcorderProfile.QUALITY_2160P ||
                    quality == CamcorderProfile.QUALITY_QHD ||
                    quality == CamcorderProfile.QUALITY_2k ||
                    quality == CamcorderProfile.QUALITY_1080P ||
                    quality == CamcorderProfile.QUALITY_720P);
        } else {
            Log.e(TAG, "Front camera is not supported or some error occurred");
            return false;
        }
    }

    private int getHighestSupportedVideoQuality(int cameraId) {
        final StreamConfigurationMap map = mCharacteristics.get(
                CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        final Size[] sizes = map.getOutputSizes(MediaRecorder.class);
        for (int i = 0; i < sizes.length; i++) {
            if (VIDEO_QUALITY_TABLE.containsKey(sizes[i])) {
                final int videoQuality = VIDEO_QUALITY_TABLE.get(sizes[i]);
                if (CamcorderProfile.hasProfile(cameraId, videoQuality)) {
                    //sizes array is sorted, so this is the highest supported video quality
                    return videoQuality;
                }
            }
        }
        // if no match found
        return VIDEO_QUALITY_UNKNOWN;
    }

    /**
     * Get the list of cameras available for use.
     *
     * @param context The context.
     */
    private void maybeInitializeCameraList(Context context) {
        if (mIsInitialized || context == null) {
            return;
        }
        final CameraManager cameraManager;
        try {
            cameraManager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        } catch (Exception e) {
            Log.e(TAG, "Could not get camera service.");
            return;
        }
        if (cameraManager == null) {
            return;
        }

        try {
            String[] cameraIdList = cameraManager.getCameraIdList();
            Log.d(TAG, "cameraIdList is: " + cameraIdList);
            for (int i = 0; i < cameraIdList.length; i++) {
                final CameraCharacteristics characteristics
                        = cameraManager.getCameraCharacteristics(cameraIdList[i]);
                if (characteristics != null) {
                    if (characteristics.get(CameraCharacteristics.LENS_FACING) ==
                            CameraCharacteristics.LENS_FACING_FRONT) {
                        mFrontFacingCameraId = i;
                        mCharacteristics = characteristics;
                        break;
                    }
                }
            }
            mIsInitialized = true;
        } catch (IllegalArgumentException e) {
            Log.e(TAG, "Device id is unknown: " + e);
            // Device Id is unknown.
        } catch (CameraAccessException e) {
            Log.e(TAG, "Could not access camera: " + e);
            // Camera disabled by device policy.
        }
    }
}
