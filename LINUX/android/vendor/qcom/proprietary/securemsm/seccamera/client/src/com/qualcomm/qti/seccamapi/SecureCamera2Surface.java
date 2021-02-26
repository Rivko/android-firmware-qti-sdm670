// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
package com.qualcomm.qti.seccamapi;

import android.hardware.camera2.CameraDevice;
import android.util.Log;
import android.view.Surface;

import com.qualcomm.qti.seccamapi.SecureSurface;

public class SecureCamera2Surface extends SecureSurface {

    //=========================================================================
    //
    //=========================================================================
    public SecureCamera2Surface(int cameraId, int width, int height, int format, int numOfBuffers) {
        super(cameraId, width, height, format, numOfBuffers);
    }

}
