/* ====================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file IColorServiceImpl.java
 *
 */

package com.qti.service.colorservice;

import java.util.Hashtable;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.RemoteException;
import android.util.Log;
import com.qti.snapdragon.sdk.display.IColorService;
import com.qti.snapdragon.sdk.display.ModeInfo;
import com.qti.snapdragon.sdk.display.ColorManager;

/**
 *
 * This class implements the actual system service for the display SDK.
 * Internally, it mostly calls native display APIs and passes the return values
 * to the app. This class loads the required native libraries.
 *
 */
class IColorServiceImpl extends IColorService.Stub {

    private static final String TAG = "IColorServiceImpl";
    private final Context context;
    public static final int RET_SUCCESS = 0;
    public static final int RET_FAILURE = ColorManager.RET_FAILURE;
    public static final int RET_NOT_SUPPORTED = ColorManager.RET_NOT_SUPPORTED;
    public static final int RET_VALUE_OUT_OF_RANGE = ColorManager.RET_VALUE_OUT_OF_RANGE;
    public static boolean VERBOSE_ENABLED = false;

    //make sure below mapping is always maintained
    //feature mapping
    private static final Hashtable<Integer,Integer> DCM_FEATURE_MAP = new Hashtable<Integer, Integer>()
            {{ //SDK id, Native id
                put(0, 0); //COLOR_BALANCE
                put(1, 1); //MODE_SELECTION
                put(2, 2); //MODE_MANAGEMENT
                put(3, 3); //ADAPTIVE_BACKLIGHT
                put(4, 4); //GLOBAL_PA
                put(5, 5); //MEM_COLOR
                put(6, 6); //SVI
            }};
    //end feature mapping

    //display mapping
    private static final Hashtable<Integer, Integer> DCM_DISPLAY_MAP = new Hashtable<Integer, Integer>()
            {{ //SDK id, native id
                put(0, 0); //primary
                put(1, 1); //external
                put(2, 2); //wifi
            }};
    //end display mapping


    /**
     * This constructor runs the service initialisation.
     *
     * @param context
     *            The current context
     * @throws InstantiationException
     *             if native initialisation fails
     */
    IColorServiceImpl(Context context) throws InstantiationException {
        this.context = context;
        int retVal = native_init();
        if(VERBOSE_ENABLED)Log.v(TAG, "Impl init returned " + retVal);
        if (retVal != RET_SUCCESS) {
            throw new InstantiationException("Init failed");
        }
    }


    /**
     * This method checks for the given feature to be supported on the given
     * display type
     *
     * @param displayId
     *            Id of the display to be checked
     * @param featureId
     *            Id of the feature to be checked
     * @return true if supported false if not supported
     */
    public boolean isFeatureSupported(int displayId, int featureId) {
        try {
            System.loadLibrary("sdm-disp-apis");
        } catch (Exception e) {
            Log.e(TAG, "Required library3 doesnt exist");
            return false;
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Required library3 doesnt exist");
            return false;
        }
        try {
            System.loadLibrary("sd_sdk_display");
        } catch (Exception e) {
            Log.e(TAG, "sd_sdk_display.so not included");
            return false;
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "sd-sdk-display.so not included");
            return false;
        }

      //feature mapping
        Integer nativeFeatureId = DCM_FEATURE_MAP.get(new Integer(featureId));
        if(nativeFeatureId == null){
            Log.e(TAG, "Native feature "+featureId+ " doesnt exist");
            return false;
        }
        //end feature mapping
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return false;
        }
        //end display mapping
        if(VERBOSE_ENABLED)Log.v(TAG, "Calling service isFeatureSupported");
        int retVal = native_isFeatureSupported(nativeDisplayId.intValue(), nativeFeatureId.intValue());

        if(VERBOSE_ENABLED)Log.v(TAG, "Native isFeatureSupported returns " + retVal);
        if (retVal <= 0) {
            Log.e(TAG, "Native returned not supported");
            return false;
        } else {
            Log.e(TAG, "Native returned is supported");
            return true;
        }
    }

    //////color balance
    /**
     * This methed calls the native API to set the Color balance or warmth
     *
     * @param displayId
     *            display Id for which SVI is to be set
     * @param warmth
     *            value of warmth
     * @return return value from native api as is.
     */
    public int setColorBalance(int displayId, int warmth) {
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_setColorBalance(nativeDisplayId.intValue(), warmth);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setColorBalance returned " + retVal);
        return retVal;
    }

    /**
     * This method gets the current warmth for the display
     *
     * @param displayId
     *            id of the display for which SVI is required
     * @return the current warmth of the display or error.
     *          To get error value, deduct -100 from returned value
     */
    public int getColorBalance(int displayId) {
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.COLOR_BALANCE_WARMTH_LOWER_BOUND-1);
        }
        //end display mapping
        int retVal = native_getColorBalance(nativeDisplayId.intValue());
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getColorBalance returned " + retVal);
        return retVal;
    }
    //////end color balance

    ///mode
    public int getNumModes(int displayId, int modeType){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_getNumberOfModes(nativeDisplayId.intValue(), modeType);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getNumModes returned " + retVal);
        return retVal;
    }

    public long[] getActiveMode(int displayId){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return null;
        }
        //end display mapping
        long[] retVal = native_getActiveMode(nativeDisplayId.intValue());
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getActiveMode returned " + retVal[0] + " "+ retVal[1]);
        return retVal;
    }

    public int setActiveMode(int displayId, int modeId) {
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_setActiveMode(nativeDisplayId.intValue(), modeId);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setActiveMode returned " + retVal);
        return retVal;
    }

    public int deleteMode(int displayId, int modeId) {
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_deleteMode(nativeDisplayId.intValue(), modeId);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native deleteMode returned " + retVal);
        return retVal;
    }

    public ModeInfo[] getModes(int displayId, int modeType){
        //Display mapping
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getModes calling in  service");
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return null;
        }
        //end display mapping
        ModeInfo[] pf = native_getModes(nativeDisplayId.intValue(), modeType);
        return pf;
    }

    public int createNewMode(int displayId, String name, long flag, int cbWarmth){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        return native_createNewMode(nativeDisplayId.intValue(), name, name.length(), flag, cbWarmth);
    }

    public int modifyMode(int displayId, int modeId, String name, long flag, int cbWarmth){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        return native_modifyMode(nativeDisplayId.intValue(), modeId, name, name.length(), flag, cbWarmth);
    }

    public int getDefaultMode(int displayId){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_getDefaultMode(nativeDisplayId.intValue());
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getDefaultMode returned " + retVal);
        return retVal;
    }

    public int setDefaultMode(int displayId, int modeId) {
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_setDefaultMode(nativeDisplayId.intValue(), modeId);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setDefaultMode returned " + retVal);
        return retVal;
    }

    public int createNewModeAllFeatures(int displayId, String name){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        return native_createNewModeAllFeatures(nativeDisplayId.intValue(), name, name.length());
    }

    public int modifyModeAllFeatures(int displayId, int modeId, String name){
      //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        return native_modifyModeAllFeatures(nativeDisplayId.intValue(), modeId, name, name.length());
    }
    ///end mode

    //svi
    public int getRangeSunlightVisibilityStrength(int displayId, int minMax){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_getRangeSVI(nativeDisplayId.intValue(), minMax);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getSVI returned " + retVal);
        return retVal;
    }

    public int setSunlightVisibilityStrength(int displayId, int strengthVal){
      //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return -1;
        }
        //end display mapping
        int retVal = native_setSVI(nativeDisplayId.intValue(), strengthVal);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setSVI returned " + retVal);
        return retVal;
    }

    public int getSunlightVisibilityStrength(int displayId){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_getSVI(nativeDisplayId.intValue());
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getSVI returned " + retVal);
        return retVal;
    }
    //end svi

    //backlight
    public int getBacklightQualityLevel(int displayId){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_getBacklightQualityLevel(nativeDisplayId.intValue());
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getBacklightQualityLevel returned " + retVal);
        return retVal;
    }

    public int setBacklightQualityLevel(int displayId, int level){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_setBacklightQualityLevel(nativeDisplayId.intValue(), level);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setBacklightQualityLevel returned " + retVal);
        return retVal;
    }

    public int getAdaptiveBacklightScale(int displayId){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_getAdaptiveBacklightScale(nativeDisplayId.intValue());
        if(VERBOSE_ENABLED)Log.v(TAG, "Native getAdaptiveBacklightScale returned " + retVal);
        return retVal;
    }
    //end backlight

    //active feature
    public int isActiveFeatureOn(int displayId, int feature){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_isActiveFeatureOn(nativeDisplayId.intValue(), feature);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native isActiveFeatureOn returned " + retVal);
        return retVal;
    }

    public int setActiveFeatureControl(int displayId, int feature, int request){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_setActiveFeatureControl(nativeDisplayId.intValue(), feature, request);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setActiveFeatureControl returned " + retVal);
        return retVal;
    }
    //end active feature

    //mem color
    public int[] getRangeMemoryColorParameter(int displayId, int type){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return null;
        }
        //end display mapping
        int[] retVal = native_getRangeMemoryColorParameter(nativeDisplayId.intValue(), type);
        return retVal;
    }

    public int setMemoryColorParameters(int displayId, int type, int hue, int saturation, int intensity){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_setMemoryColorParameters(nativeDisplayId.intValue(), type, hue, saturation, intensity);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setMemoryColorParameters returned " + retVal);
        return retVal;
    }

    public int[] getMemoryColorParameters(int displayId, int type){
      //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return null;
        }
        //end display mapping
        int[] retVal = native_getMemoryColorParameters(nativeDisplayId.intValue(), type);
        return retVal;
    }

    public int disableMemoryColorConfiguration(int displayId, int type){
      //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return (ColorManager.RET_FAILURE);
        }
        //end display mapping
        int retVal = native_disableMemoryColorConfiguration(nativeDisplayId.intValue(), type);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native disableMemoryColorConfiguration returned " + retVal);
        return retVal;
    }
    //end mem color


    //pa
    public int[] getRangePAParameter(int displayId){
        //Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
        if(nativeDisplayId == null){
            Log.e(TAG, "Native display type "+ displayId + " is not supported");
            return null;
        }
        //end display mapping
        int[] retVal = native_getRangePAParameter(nativeDisplayId.intValue());
        return retVal;
    }

    public int setPAParameters(int displayId, int flag, int hue, int saturation, int intensity, int contrast, int satThreshold) {
        // Display mapping
        Integer nativeDisplayId = DCM_DISPLAY_MAP.get(new Integer(displayId));
        if (nativeDisplayId == null) {
            Log.e(TAG, "Native display type " + displayId + " is not supported");
            return -1;
        }
        // end display mapping
        int retVal = native_setPAParameters(nativeDisplayId.intValue(), flag, hue, saturation, intensity, contrast, satThreshold);
        if(VERBOSE_ENABLED)Log.v(TAG, "Native setPAParameters returned " + retVal);
        return retVal;
    }

    public int[] getPAParameters(int displayId){
        //Display mapping
          Integer nativeDisplayId = DCM_DISPLAY_MAP.get(Integer.valueOf(displayId));
          if(nativeDisplayId == null){
              Log.e(TAG, "Native display type "+ displayId + " is not supported");
              return null;
          }
          //end display mapping
          int[] retVal = native_getPAParameters(nativeDisplayId.intValue());
          return retVal;
      }
    //end pa


    /**
     * This method will call the native release.
     */
    public void release() {
        native_deinit();
    }

    /**
     * Loading the required native libraries
     */
    static {
        System.loadLibrary("sdm-disp-apis");
        System.loadLibrary("sd_sdk_display");
    }


    private native int native_isFeatureSupported(int displayId, int featureVal);


    private native int native_init();

    private native void native_deinit();

    ///color balance
    private native int native_setColorBalance(int displayId, int value);

    private native int native_getColorBalance(int displayId);
    ///end color balance

    //mode
    private native int native_getNumberOfModes(int displayId,int modeType);
    private native ModeInfo[] native_getModes(int displayId,int modeType);
    private native long[] native_getActiveMode(int displayId);
    private native int native_setActiveMode(int displayId, int modeId);
    private native int native_deleteMode(int displayId, int modeId);
    private native int native_createNewMode(int displayId, String name, int nameLen, long flag, int cbWarmth);
    private native int native_modifyMode(int displayId, int modeId, String name, int nameLen, long flag, int cbWarmth);
    private native int native_getDefaultMode(int displayId);
    private native int native_setDefaultMode(int displayId, int modeId);
    private native int native_createNewModeAllFeatures(int displayId, String name, int nameLen);
    private native int native_modifyModeAllFeatures(int displayId, int modeId, String name, int nameLen);
    //end mode

    //svi
    private native int native_getRangeSVI(int displayId, int minMax);
    private native int native_setSVI(int displayId, int strengthVal);
    private native int native_getSVI(int displayId);
    //end svi

    //backlight
    private native int native_getBacklightQualityLevel(int displayId);
    private native int native_setBacklightQualityLevel(int displayId, int level);
    private native int native_getAdaptiveBacklightScale(int displayId);
    //end backlight

    //active feature
    private native int native_isActiveFeatureOn(int displayId, int feature);
    private native int native_setActiveFeatureControl(int displayId, int feature, int request);
    //end active feature

    //mem color
    private native int[] native_getRangeMemoryColorParameter(int displayId, int type);
    private native int native_setMemoryColorParameters(int displayId, int type, int hue, int saturation, int intensity);
    private native int[] native_getMemoryColorParameters(int displayId, int type);
    private native int native_disableMemoryColorConfiguration(int displayId, int type);
    //end mem color

    //pa
    private native int[] native_getRangePAParameter(int displayId);
    private native int native_setPAParameters(int displayId, int ops, int hue, int saturation, int intensity, int contrast, int satThreshold);
    private native int[] native_getPAParameters(int displayId);
    //end pa
}
