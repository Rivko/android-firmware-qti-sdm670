/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.preferences.camera2;

import android.hardware.camera2.CaptureRequest;

/**
 * A common interface for camera preferences that apply to the Camera2 camera API
 */
public interface Camera2CameraPreference {
    /**
     * Apply this preference setting to the specified {@link CaptureRequest.Builder}
     */
    void apply(CaptureRequest.Builder builder);
}
