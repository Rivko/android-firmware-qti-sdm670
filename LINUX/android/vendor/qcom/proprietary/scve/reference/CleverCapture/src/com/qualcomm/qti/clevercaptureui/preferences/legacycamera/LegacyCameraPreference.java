/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.preferences.legacycamera;

import android.hardware.Camera;

/**
 * A common interface for camera preferences that apply to the legacy {@link Camera} API
 */
@SuppressWarnings("deprecation")
public interface LegacyCameraPreference {
    /**
     * Apply this preference setting to the appropriate {@link Camera.Parameters}
     */
    void apply();
}
