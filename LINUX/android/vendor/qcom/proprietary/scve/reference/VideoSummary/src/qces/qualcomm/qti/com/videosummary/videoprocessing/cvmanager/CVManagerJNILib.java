/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.videoprocessing.cvmanager;

import java.nio.ByteBuffer;

public class CVManagerJNILib {

    public static final String[] SCVE_MODES = {
            "$#MD0",//MODE_DEFAULT
            "$#MD1", //MODE_PERFORMANCE
            "$#MD2", //MODE_OFFLOAD
            "$#MD3"};//MODE_LOW_POWER

    static {
        try {
            System.loadLibrary("feature_cv_jni");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    /**
     * Not constructable, static helper utils only
     */
    private CVManagerJNILib() {
    }

    public static native int CVNativeInit();

    public static native int CVNativeDeInit();

    public static native int[] CVNativeProcess(ByteBuffer buf, int width, int height, int offsetY, int strideY, int offsetVU, int strideVU);

    public static native CVSummaryOutput CVNativeSummary(int[][] videosScores, byte[][] videosFRTables, int summaryMode, int targetLength, byte compressRatio, int inputFPS);

    public static native byte[] CVNativeGetFaceTable();

    public static native int CVNativeSet(byte[] bArray);
}
