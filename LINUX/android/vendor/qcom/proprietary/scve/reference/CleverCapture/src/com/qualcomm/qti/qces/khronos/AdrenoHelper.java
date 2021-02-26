/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.qces.khronos;

import android.opengl.EGL14;
import android.opengl.GLES30;
import android.util.Log;

/**
 * Helper methods for working with the Adreno (TM) GPU
 */
public class AdrenoHelper {

    private static String TAG = "AdrenoHelper";
    private static String ADRENO_GPU_MODEL_NUMBER_PREFIX = "Adreno (TM) ";

    private static boolean mConfigRetrieved;
    private static int mAdrenoVersion = -1;

    /**
     * Not constructable
     */
    private AdrenoHelper() {
    }

    /**
     * Loads (and caches) the Adreno configuration. This is a NOP if the Areno config has already
     * been loaded.
     * <p><i>Note: must be called from a thread with an active GL context</i>
     * <p><i>Note: must be called within a <code>synchronized (AdrenoHelper.class)</code> scope</i>
     */
    private static void loadAdrenoConfig() {
        if (mConfigRetrieved) {
            return;
        }

        if (EGL14.eglGetCurrentContext() == EGL14.EGL_NO_CONTEXT) {
            throw new RuntimeException("AdrenoHelper can only be used on a thread with an active EGL context");
        }

        String model = GLES30.glGetString(GLES30.GL_RENDERER);
        if (model.startsWith(ADRENO_GPU_MODEL_NUMBER_PREFIX)) {
            String ver = model.substring(ADRENO_GPU_MODEL_NUMBER_PREFIX.length());
            try {
                mAdrenoVersion = Integer.parseInt(ver);
                Log.i(TAG, "Found an Adreno " + mAdrenoVersion);
            } catch (NumberFormatException e) {
                Log.e(TAG, "Failed parsing Adreno model number: " + ver);
            }
        } else {
            Log.e(TAG, "Unexpected GPU model: " + model);
        }

        mConfigRetrieved = true;
    }

    /**
     * Returns the Adreno GPU model number.
     * <p><i>Note: must be called from a thread with an active GL context</i>
     * @return the Adreno GPU model number, or -1 if the GPU is not an Adreno
     */
    public static int getAdrenoVersion() {
        synchronized (AdrenoHelper.class) {
            loadAdrenoConfig();
            return mAdrenoVersion;
        }
    }

    /**
     * Tests if the Adreno GPU is a 4xx series GPU.
     * <p><i>Note: must be called from a thread with an active GL context</i>
     * @return true if GPU is an Adreno 4xx series, else false
     */
    public static boolean isAdreno4xx() {
        int ver = getAdrenoVersion();
        return (ver >= 400 && ver < 500);
    }
}
