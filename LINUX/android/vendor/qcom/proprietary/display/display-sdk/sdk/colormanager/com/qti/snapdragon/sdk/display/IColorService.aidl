/* ====================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file IColorService.aidl
 *
 */

package com.qti.snapdragon.sdk.display;

import com.qti.snapdragon.sdk.display.ModeInfo;

interface IColorService {
    boolean isFeatureSupported(int displayId, int featureId);
    //color balance
    int setColorBalance(int displayId, int warmth);
    int getColorBalance(int displayId);
    //end color balance

    //mode
    int getNumModes(int displayId, int modeType);
    long[] getActiveMode(int displayId);
    int setActiveMode(int displayId, int modeId);
    int deleteMode(int displayId, int modeId);
    ModeInfo[] getModes(int displayId, int modeType);
    int createNewMode(int displayId, String name, long flag, int cbWarmth);
    int modifyMode(int displayId, int modeId, String name, long flag, int cbWarmth);
    int getDefaultMode(int displayId);
    int setDefaultMode(int displayId, int modeId);
    int createNewModeAllFeatures(int displayId, String name);
    int modifyModeAllFeatures(int displayId, int modeId, String name);
    //end mode

    //svi
    int getRangeSunlightVisibilityStrength(int displayId, int minMax);
    int setSunlightVisibilityStrength(int displayId, int strengthVal);
    int getSunlightVisibilityStrength(int displayId);
    //end svi

    //backlight
    int getBacklightQualityLevel(int displayId);
    int setBacklightQualityLevel(int displayId, int level);
    int getAdaptiveBacklightScale(int displayId);
    //end backlight

    //active feature
    int isActiveFeatureOn(int displayId, int feature);
    int setActiveFeatureControl(int displayId, int feature, int request);
    //end active feature

    //mem color
    int[] getRangeMemoryColorParameter(int displayId, int type);
    int setMemoryColorParameters(int displayId, int type, int hue, int saturation, int intensity);
    int[] getMemoryColorParameters(int displayId, int type);
    int disableMemoryColorConfiguration(int displayId, int type);
    //end mem color

    //pa
    int[] getRangePAParameter(int displayId);
    int setPAParameters(int displayId, int flag, int hue, int saturation, int intensity, int contrast, int satThreshold);
    int[] getPAParameters(int displayId);
    //end pa

    void release();
}