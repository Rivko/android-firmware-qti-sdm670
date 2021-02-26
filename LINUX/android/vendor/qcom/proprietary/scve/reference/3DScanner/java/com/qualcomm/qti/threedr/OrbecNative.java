/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr;

import com.qualcomm.qti.threedr.orbbec.OBCameraParams;

public class OrbecNative {
    static {
        System.loadLibrary("3drNative-lib");
    }

    public static native void enableLaserFromJNI(boolean isEnable);

    public static native String getSerialNumberFromJNI();

    public static native void getCmosParamFromJNI();

    public static native void enableLdpFromJNI(boolean isEnable);

    public static native void enableIrFloodLedFromJNI(boolean isEnable);

    public static native void irGainAddFromJNI(int changeValue);

    public static native void irGainSubFromJNI(int changeValue);

    public static native int getIrGainFromJNI();

    public static native void setIrGainFromJNI(int gain);

    public static native void irExposureAddFromJNI(int changeValue);

    public static native void irExposureSubFromJNI(int changeValue);

    public static native int getIrExposureFromJNI();

    public static native void setIrExposureFromJNI(int exposure);

    public static native OBCameraParams getOBCmosParams();
}
