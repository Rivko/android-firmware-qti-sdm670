/* ====================================================================
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file ColorManager.java
 *
 */

package com.qti.snapdragon.sdk.display;

import java.io.IOException;
import java.io.InputStream;
import java.util.EnumSet;
import java.util.HashMap;

import com.qti.snapdragon.sdk.display.PictureAdjustmentConfig;
import com.qti.snapdragon.sdk.display.PictureAdjustmentConfig.PICTURE_ADJUSTMENT_PARAMS;
import com.qti.snapdragon.sdk.display.MemoryColorConfig.MEMORY_COLOR_PARAMS;
import com.qti.snapdragon.sdk.display.MemoryColorConfig.MEMORY_COLOR_TYPE;

import android.app.Activity;
import android.app.Application;
import android.app.Application.ActivityLifecycleCallbacks;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.ServiceManager;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

/*
 * Logic for saving system values and using them:
 *
 * We will not handle the restore facility
 */
/**
 * This class provides APIs for changing the device display. The color manager
 * will maintain separate instances for each display type. A static method,
 * getInstance is provided for requesting an instance of the class for each
 * display type. <br>
 * The sequence of APIs calls are as follows: <br>
 * 1) connect(), then wait for the callback to be received. Not required if this
 * is already called once, but a call is recommended. <br>
 * 2) once callback is received, then getInstance() <br>
 * 3) isFeatureSupported() <br>
 * 4) setColorBalance() <br>
 * 5) When everything is done, call release(). This is optional. <br>
 * If an instance is already in use, a getInstance() call will return a handle
 * to the same instance.
 * <br>
 *Because these APIs effect the display, the application will need to restore the
 * display to the settings before any changes were made when the application leaves
 * the foreground. Failure to do so will cause the device display to be persisting
 *  with the previous app's display settings.
 */
public class ColorManager {

    private static String TAG = "ColorManager";
    private static boolean VERBOSE_ENABLED = true;
    /** Bitmask value for Color Balance feature */
    public static final long BITFLAG_COLOR_BALANCE = Long.parseLong("1", 2);
    public static final long BITFLAG_GLOBAL_PICTURE_ADJUSTMENT = Long.parseLong("10", 2);
    public static final long BITFLAG_MEMORY_COLOR_SKIN = Long.parseLong("100", 2);
    public static final long BITFLAG_MEMORY_COLOR_SKY = Long.parseLong("1000", 2);
    public static final long BITFLAG_MEMORY_COLOR_FOLIAGE = Long.parseLong("10000", 2);


    private static int PA_GLOBAL_HUE = 0x001;
    private static int PA_GLOBAL_SAT = 0x002;
    private static int PA_GLOBAL_VAL = 0x004;
    private static int PA_GLOBAL_CON = 0x008;
    private static int PA_GLOBAL_SAT_THRESH = 0x010;
    private static int PA_GLOBAL_DESAT = 0x020;
    private static int PA_GLOBAL_DISABLE = 0x040;


    /**
     * This enumeration provides a list of features exposed by the ColorManager
     * class.
     */
    public enum DCM_FEATURE {
        /**
         * Color Balance feature allows for the adjustment of the display warmth
         */
        FEATURE_COLOR_BALANCE(0),
        /**
         * Mode Selection allows for the setting of the active display mode and
         * the device default display mode. The later requires that the
         * application have system privileges
         */
        FEATURE_COLOR_MODE_SELECTION(1),
        /**
         * Mode Management allows for the creation and modification of USER
         * modes
         */
        FEATURE_COLOR_MODE_MANAGEMENT(2),
        /**
         * Allows to start/stop/alter powersaving levels/read backlight scale
         * for adaptive backlight feature
         */
        FEATURE_ADAPTIVE_BACKLIGHT(3),
        /**
         * Allows to set/get the global hue, saturation, intensity and contrast
         * adjustment values
         */
        FEATURE_GLOBAL_PICTURE_ADJUSTMENT(4),
        /**
         * Allows to set/get the memory color adjustment values of SKIN, SKY,
         * FOLIAGE regions
         */
        FEATURE_MEMORY_COLOR_ADJUSTMENT(5),
        /**
         * Alows to start/stop/alter the strength of the sunlight visibility
         * feature
         */
        FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT(6);

        private int value;


        private DCM_FEATURE(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    }


    /**
     * This enumeration provides a list of display types that are supported
     * by these APIs
     */
    public enum DCM_DISPLAY_TYPE {
        DISP_PRIMARY(0),
        DISP_EXTERNAL(1),
        DISP_WIFI(2);

        private int value;


        private DCM_DISPLAY_TYPE(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    }

    private static final int NUM_DISPLAY_TYPES = 3;// always change this if
                                                   // display types increase or
                                                   // decrease


    /**
     * This enumeration provides a list of active features
     */
    private enum ACTIVE_FEATURE_TYPE {
        FEATURE_ADAPTIVE_BACKLIGHT(0),
        FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT(1);
        private int value;


        private ACTIVE_FEATURE_TYPE(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    }


    /**
     * This enumeration provides a list of control requests like on and off
     */
    private enum CONTROL_REQUEST_TYPE {
        ON(0), OFF(1);
        private int value;


        private CONTROL_REQUEST_TYPE(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    }


    /**
     * This enumeration provides a list of backlight quality levels
     */
    public enum ADAPTIVE_BACKLIGHT_QUALITY_LEVEL {
        LOW(0),
        MEDIUM(1),
        HIGH(2),
        AUTO(3);
        private int value;

        private ADAPTIVE_BACKLIGHT_QUALITY_LEVEL(int value) {
            this.value = value;
        }

        protected int getValue() {
            return value;
        }
    }


    // //////mode
    /**
     * This enum provides a list of Mode types.
     */
    public enum MODE_TYPE {
        /**
         * Querying mode info for this mode type will return only system modes.
         * System display modes are display modes defined by the device
         * manufacturer and are immutable and cannot be deleted
         */
        MODE_SYSTEM(0),
        /**
         * Querying mode info for this mode type will return only system modes.
         * User display modes are defined by an application and/or by the end
         * user via an application with user level privileges. User display
         * modes are mutable, and can be deleted
         */
        MODE_USER(1),
        /**
         * Querying mode info for this mode type will return user and system
         * modes
         */
        MODE_ALL(2);

        private int value;


        private MODE_TYPE(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    }

    // /end mode

    private static final String SERVICE_PKG_NAME = "com.qti.service.colorservice";

    /** API returns success */
    public static final int RET_SUCCESS = 0;
    /** API returns general failure */
    public static final int RET_FAILURE = -999;
    /** Feature not supported */
    public static final int RET_NOT_SUPPORTED = -901;
    /** Value passed is out of range */
    public static final int RET_VALUE_OUT_OF_RANGE = -902;
    /** Required permission missing */
    public static final int RET_PERMISSION_DENIED = -903;
    /** Illegal argument passed */
    public static final int RET_ILLEGAL_ARGUMENT = -904;
    /** The feature is disabled for use*/
    public static final int RET_FEATURE_DISABLED = -905;

    private static ColorManager[] myInstance = new ColorManager[NUM_DISPLAY_TYPES];
    private int displayId;

    private boolean isSystemApp = false;
    private Application myApplication;

    private static final String REMOTE_SERVICE_NAME = IColorService.class.getName();
    private static IColorService service;
    private static final String permission = "com.qti.snapdragon.sdk.permission.DISPLAY_SETTINGS";
    private static final int INIT_VALUE = -999;

    // /////Color Balance
    /** Lowest value allowed for Color balance */
    public static final int COLOR_BALANCE_WARMTH_LOWER_BOUND = -100;
    /** Highest value allowed for Color balance */
    public static final int COLOR_BALANCE_WARMTH_UPPER_BOUND = 100;
    // end color balance

    // mode
    /** Maximum length of a name that can be given to a mode */
    public static final int MODE_NAME_MAX_LENGTH = 32;
    // end mode

    // new service structure
    private static Context serviceContext = null;
    private static ColorManagerListener colorMgrListener;
    private static DisplayConn conn = new DisplayConn();
    private static final String SERVICE_INTENT_NAME = "com.qti.service.colorservice.ColorServiceApp";
    private static final String SERVICE_INTENT_PACKAGE = "com.qti.service.colorservice";
    private static boolean isConnecting = false;

    //ranges
    HashMap<String, Integer> memColorRanges = new HashMap<String, Integer>();
    HashMap<String, Integer> paRanges = new HashMap<String, Integer>();

    private static class DisplayConn implements ServiceConnection {

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            if (colorMgrListener == null) {
                Log.e(TAG, "Listener is null");
                return;
            }
            ColorManager.service = IColorService.Stub.asInterface(ServiceManager
                    .getService(REMOTE_SERVICE_NAME));
            if(isConnecting == true){
                Log.v(TAG, "Callback called");
                colorMgrListener.onConnected();
            }
            else{
                Log.v(TAG, "Callback not called");
            }
            isConnecting=false;
        }


        @Override
        public void onServiceDisconnected(ComponentName name) {
        }

    }


    /**
     * This method will start the service required for the Display SDK. This
     * method should be called atleast once before using any other API. A
     * callback is received once the service is connected. It is recommended to
     * first make a call to connect() and then other API calls only after the
     * callback is received.
     * @param context
     * @param colorListener
     *            the listener to get the callback once the connection is
     *            successful
     * @return RET_SUCCESS if the request to connect succeeds and then wait for
     *         the callback. <br>
     *         RET_ILLEGAL_ARGUMENT if any of the argument is passed null <br>
     *         RET_FAILURE if the connection request fails
     */
    public static int connect(Context context, ColorManagerListener colorListener) {
        if (context == null || colorListener == null) {
            Log.e(TAG, "One of the parmeter passed is null");
            return RET_ILLEGAL_ARGUMENT;
        }
        int res = context.getApplicationContext().checkCallingOrSelfPermission(permission);
        if (res != PackageManager.PERMISSION_GRANTED) {
            Log.e(TAG,
                    "Required permission 'com.qti.snapdragon.sdk.permission.DISPLAY_SETTINGS' is"
                            + " missing");
            return RET_FAILURE;
        }
        ColorManager.colorMgrListener = colorListener;
        serviceContext = context;
        if(isConnecting){
            Log.v(TAG, "Connection already in progress");
            return RET_FAILURE;
        }

        if(isServiceRunning()){
            Log.v(TAG, "Service running");
            Intent serviceIntent = new Intent();
            serviceIntent.setClassName(SERVICE_INTENT_PACKAGE, SERVICE_INTENT_NAME);
            boolean serviceBound = context.getApplicationContext().bindService(
                    serviceIntent, conn, Context.BIND_AUTO_CREATE);
            if(serviceBound == true){
                Log.v(TAG, "Running service bound");
                ColorManager.colorMgrListener.onConnected();
                return RET_SUCCESS;
            }
            else{
                Log.e(TAG, "Bind failed even when service is running");
                return RET_FAILURE;
            }
        }
        else{
            Log.v(TAG, "Service is not running");
        }
        try {
            isConnecting= true;
            //added after unbind removed from connectio callback
            try{
                context.getApplicationContext().unbindService(conn);
            }
            catch(Exception e){
                e.printStackTrace();
            }
            //end
            Intent serviceIntent = new Intent();
            serviceIntent.setClassName(SERVICE_INTENT_PACKAGE, SERVICE_INTENT_NAME);
            boolean serviceBound = context.getApplicationContext().bindService(
                    serviceIntent, conn, Context.BIND_AUTO_CREATE);
            if (serviceBound == true) {
                return RET_SUCCESS;
            } else {
                Log.e(TAG, "Failed to connect to remote service");
                isConnecting= false;
                return RET_FAILURE;
            }
        } catch (SecurityException e) {
            isConnecting= false;
            e.printStackTrace();
            return RET_FAILURE;
        }
    }

    /**
     * This listener will inform the calling app when the connection is made to the display service.
     */
    /*NEW*/
    public interface ColorManagerListener {
        /**
         * This callback will be invoked when the display service is started.
         * User app is required to override this callback.
         * All other display APIs must be used once this callback is received.
         */
        public void onConnected();
    }


    // end

    /**
     * This method will avoid cloning and throw CloneNotSupportedException
     */
    public Object clone() throws CloneNotSupportedException {
        throw new CloneNotSupportedException();
    }


    /*
     * Private constructor to prevent instance creation from user apps. Instance
     * can only be requested through getInstance() method
     */
    private ColorManager(Application application, int displayId) {
        this.service = IColorService.Stub.asInterface(ServiceManager.getService(REMOTE_SERVICE_NAME));
        if (this.service == null) {
            throw new IllegalStateException("Failed to find IService by name [" + REMOTE_SERVICE_NAME + "]");
        }

        myApplication = application;

        // check if system app
        isSystemApp = (application.getApplicationInfo().flags & (ApplicationInfo.FLAG_SYSTEM |
                ApplicationInfo.FLAG_UPDATED_SYSTEM_APP)) != 0;
        Log.v(TAG, "System app? " + isSystemApp);

        this.displayId = displayId;
    }


    /*
     * This method will ensure only one instance per display type at a time. If
     * an instance exists, the method will return the existing one and not
     * create another.
     */
    /**
     * This method will return an instance of the ColorManager class for the
     * input display id. The app will need to have the permission
     * com.qti.snapdragon.sdk.permission.DISPLAY_SETTINGS in order to use this
     * feature.
     * @param application
     * @param context
     *            calling activity context
     * @param displayId
     *            id of the display for which this feature is to be used
     * @return ColorManager instance
     * @throws IllegalArgumentException
     *             if any parameter is passed null or the context passed is not
     *             an activity context
     */
    public static ColorManager getInstance(Application application, Context context, DCM_DISPLAY_TYPE displayId)
            throws IllegalArgumentException {

        if (application == null || context == null) {
            Log.e(TAG, "Application or context passed is null");
            throw new IllegalArgumentException("Null passed for Application or context");
        }
        if (displayId == null) {
            Log.e(TAG, "Display Id passed is null");
            throw new IllegalArgumentException("Display ID passed is null");
        }

        int res = application.checkCallingOrSelfPermission(permission);
        if (res != PackageManager.PERMISSION_GRANTED) {
            Log.e(TAG,
                    "Required permission 'com.qti.snapdragon.sdk.permission.DISPLAY_SETTINGS' is missing");
            return null;
        }

        if (null == myInstance[displayId.getValue()]) {
            try {
                myInstance[displayId.getValue()] = new ColorManager(application,
                        displayId.getValue());
                Log.v(TAG, "Instance created for display type " + displayId);
            } catch (Exception e) {
                e.printStackTrace();
                myInstance[displayId.getValue()] = null;
                return null;
            }

            return myInstance[displayId.getValue()];
        } else {
            Log.v(TAG, "Returning existing instance");
            return myInstance[displayId.getValue()];
        }
    }


    /**
     * This method returns TRUE if the input feature is supported, FALSE
     * otherwise.
     * @param feature
     *            should be a value from the enum DCM_FEATURE
     * @return true if the given feature is supported, false otherwise.
     * @throws IllegalArgumentException
     *             if feature id is passed null
     */
    public boolean isFeatureSupported(DCM_FEATURE feature) throws IllegalArgumentException {
        if (myApplication == null) {
            return false;
        }
        if (feature == null) {
            Log.e(TAG, "Feature id passed is null");
            throw new IllegalArgumentException("Feature ID passed is null");
        }
        PackageManager pm = myApplication.getPackageManager();
        try {
            PackageInfo info = pm.getPackageInfo(SERVICE_PKG_NAME, PackageManager.GET_META_DATA);
        } catch (NameNotFoundException e) {
            e.printStackTrace();
            return false;
        }
        try {
            return this.service.isFeatureSupported(this.displayId, feature.getValue());
        } catch (RemoteException e) {
            e.printStackTrace();
            Log.e(TAG, "Service isFeatureSupported crashed");
            return false;
        }

    }


    /**
     * This method will change the color balance warmth to the input value.
     * @param warmth
     *            within the range (-100,100)
     * @return RET_SUCCESS if api succeeds, <br>
     *         RET_VALUE_OUT_OF_RANGE if provided value is outside the range of
     *         (-100,100), <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_BALANCE is not supported, <br>
     *         RET_FAILURE for all other failures
     */
    public int setColorBalance(int warmth) {

        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_BALANCE);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            //
            if (warmth < COLOR_BALANCE_WARMTH_LOWER_BOUND || warmth > COLOR_BALANCE_WARMTH_UPPER_BOUND) {
                Log.e(TAG, "Warmth given is outside the range (-100, 100)");
                return RET_VALUE_OUT_OF_RANGE;
            } else {
                try {
                    Log.v(TAG, "Calling setColorBalance for display " + this.displayId);
                    int retVal = this.service.setColorBalance(this.displayId, warmth);
                    if (retVal == 0) {// passed
                        Log.v(TAG, "SetColorBalance() worked");
                    } else {
                        Log.e(TAG, "Service setColorBalance failed with return value " + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;

                } catch (RemoteException e) {
                    Log.e(TAG, "Service set color balance failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method returns the current color balance warmth value
     *
     * @return The color balance value between (-100, 100) <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_BALANCE is not supported, <br>
     *         RET_FAILURE for all other failures
     */
    public int getColorBalance() {
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_BALANCE);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            //
            try {
                Log.v(TAG, "Calling getColorBalance for display " + this.displayId);
                int retVal = this.service.getColorBalance(this.displayId);
                if (retVal < COLOR_BALANCE_WARMTH_LOWER_BOUND) {// failed
                    Log.e(TAG, "Service getColorBalance failed with return value "
                            + (retVal - COLOR_BALANCE_WARMTH_LOWER_BOUND));
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service get color balance failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    // mode
    /**
     * This method returns the number of display modes available on device.
     * @param type
     *            type of modes for which count is required
     * @return number of modes if api succeeds, <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_SELECTION is not
     *         supported, <br>
     *         RET_FAILURE for all other failures
     * @throws IllegalArgumentException
     *             if type is passed null
     */
    public int getNumOfModes(MODE_TYPE type) throws IllegalArgumentException {
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_SELECTION);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }

            if (type == null) {
                Log.e(TAG, "Feature id passed is null");
                throw new IllegalArgumentException("Type passed is null");
            }

            try {
                Log.v(TAG, "Calling getNumOfModes for display " + this.displayId);
                int retVal = this.service.getNumModes(displayId, type.getValue());
                if (retVal < 0) {// failed
                    Log.e(TAG, "Service getNumModes failed with return value " + (retVal));
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service get num modes failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method returns an array which includes the id of the currently
     * active display mode and a bitmask indicating there are active
     * customizations on top of the settings encapsulated by the display mode.
     * Refer to the field summary for bitmask values (BITFLAG_*) indicating bitmask for
     * color balance, picture adjustment, and memory color (skin, sky, foliage).
     * @return integer array if api succeeds with mode-id as parameter 0 and
     *         flag as parameter 1, <br>
     *         RET_NOT_SUPPORTED as parameter 0 if FEATURE_COLOR_MODE_SELECTION
     *         is not supported, <br>
     *         RET_FAILURE as parameter 0 for all other failures
     */
    public int[] getActiveMode() {
        int[] retArray = { RET_FAILURE, 0 };
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_SELECTION);
                if (isSupported == false) {
                    retArray[0] = RET_NOT_SUPPORTED;
                    return retArray;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                retArray[0] = RET_NOT_SUPPORTED;
                return retArray;
            }
            //
            try {
                Log.v(TAG, "Calling getActiveMode() for display " + this.displayId);
                long[] retVal = { RET_FAILURE, 0 };
                retVal = this.service.getActiveMode(this.displayId);
                if (retVal == null || retVal.length <= 0) {
                    Log.e(TAG, "getActive service returned null ");
                    retArray[0] = RET_FAILURE;
                    return retArray;
                } else if (retVal[0] < 0) {// failed
                    Log.e(TAG, "Service getActiveMode failed with return value " + (retVal[0]));
                    retArray[0] = RET_FAILURE;
                    return retArray;
                }
                retArray[0] = (int) retVal[0];
                retArray[1] = (int) retVal[1];
                return retArray;

            } catch (RemoteException e) {
                Log.e(TAG, "Service get active mode failed");
                retArray[0] = RET_FAILURE;
                return retArray;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            retArray[0] = RET_FAILURE;
            return retArray;
        }
    }


    /**
     * This method will activate a display mode, causing the device display to
     * change accordingly. This will override the default mode if different
     * until the device is rebooted or the default mode is set to active again.
     * @param modeId
     *            the id of the mode which needs to be set
     * @return RET_SUCCESS if the API succeeds <br>
     *         RET_VALUE_OUT_OF_RANGE if mode id provided is negative <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_SELECTION is not
     *         supported, <br>
     *         RET_FAILURE for all other failures
     */
    public int setActiveMode(int modeId) {
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_SELECTION);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            //
            // TODO: mode none needs to be handled
            if (modeId < 0) {
                Log.e(TAG, "Mode ID provided is less than 0");
                return RET_VALUE_OUT_OF_RANGE;
            } else {
                try {
                    Log.v(TAG, "Calling setActiveMode for display " + this.displayId);
                    int retVal = this.service.setActiveMode(this.displayId, modeId);
                    if (retVal == 0) {// passed
                        Log.v(TAG, "SetActiveMode() worked");
                    } else {
                        Log.e(TAG, "Service setActiveMode failed with return value " + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;

                } catch (RemoteException e) {
                    Log.e(TAG, "Service set active mode failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method returns an array of ModeInfo objects
     * @param type
     *            type of modes for which the list is required
     * @return Mode[] containing mode info. <br>
     *         null if FEATURE_COLOR_MODE_SELECTION is not supported, if mode
     *         count is 0 and for all other reasons
     * @throws IllegalArgumentException
     *             if type is passed null
     */
    public ModeInfo[] getModes(MODE_TYPE type) throws IllegalArgumentException {
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_SELECTION);
                if (isSupported == false) {
                    Log.e(TAG, "FEATURE_COLOR_MODE_SELECTION is not supported for display " + this.displayId);
                    return null;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return null;
            }
            if (type == null) {
                Log.e(TAG, "Mode Type passed is null");
                throw new IllegalArgumentException("Type passed is null");
            }
            try {
                ModeInfo[] mode = this.service.getModes(displayId, type.getValue());
                return mode;
            } catch (RemoteException e) {
                Log.e(TAG, "Service get modes failed");
                e.printStackTrace();
                return null;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return null;
        }
    }


    /**
     * This method will delete a USER type Mode. Note that a mode of type SYSTEM
     * cannot be deleted. Also note that a non-system app cannot use this
     * method. A mode that is active or default cannot be deleted.
     * @param modeId
     *            id of the mode to be deleted.
     * @return RET_SUCCESS if the API succeeds <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_MANAGEMENT is not
     *         supported, <br>
     *         RET_PERMISSION_DENIED if a non system app tries to use this API <br>
     *         RET_FAILURE for all other failures <br>
     *         RET_VALUE_OUT_OF_RANGE if mode id is less than 0
     */
    public int deleteMode(int modeId) {
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_MANAGEMENT);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            //
            if (!isSystemApp) {
                Log.e(TAG, "You do not have permission to perform this operation");
                return RET_PERMISSION_DENIED;
            } else {
                if (modeId < 0) {
                    Log.e(TAG, "Mode ID provided is less than 0");
                    return RET_VALUE_OUT_OF_RANGE;
                } else {
                    try {
                        int retVal = this.service.deleteMode(displayId, modeId);
                        if (retVal == 0) {// passed
                            Log.v(TAG, "deleteMode() worked");
                        } else {
                            Log.e(TAG, "Service deleteMode failed with return value " + retVal);
                            return RET_FAILURE;
                        }
                        return retVal;

                    } catch (RemoteException e) {
                        Log.e(TAG, "Service set active mode failed");
                        return RET_FAILURE;
                    }
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method will create a new mode with the input mode name. The new mode
     * will be of the type USER. The method will return the ID of the new mode
     * if successful, or an error code if not successful. The mode name must be
     * unique.
     * @param modeName
     *            name for the new mode
     * @param featureList
     *            EnumSet of features that need to be set for the new mode,
     *            excluding FEATURE_COLOR_MODE_MANAGEMENT and
     *            FEATURE_COLOR_MODE_SELECTION. These features will be ignored
     *            if passed in the enumset.
     * @param colorBalance
     *            Color balance value for the mode
     * @return New Mode Id (positive number) if API succeeds <br>
     *         RET_PERMISSION_DENIED if a non system app tries to use this API <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_MANAGEMENT or any of the
     *         features in the featurelist are not supported, <br>
     *         RET_VALUE_OUT_OF_RANGE if any of the provided values is outside
     *         the range, <br>
     *         RET_FAILURE for all other failures
     */
    public int createNewMode(String modeName, EnumSet<DCM_FEATURE> featureList, int colorBalance) {
        if (myInstance[displayId] != null) {
            long FLAG = 0;

            // checK if feature is supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_MANAGEMENT);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            // check if system app
            if (!isSystemApp) {
                Log.e(TAG, "You do not have permission to perform this operation");
                return RET_PERMISSION_DENIED;
            }

            // check if name exists
            if (modeName == null || modeName.equals("") || modeName.length() > MODE_NAME_MAX_LENGTH) {
                Log.e(TAG, "Mode name is missing or crossing max length");
                return RET_FAILURE;
            }

            // now play with arguments
            // Step 1: if featurelist null return RET_FAILURE
            if (featureList == null || featureList.isEmpty()) {
                Log.e(TAG, "FeatureList missing");
                return RET_FAILURE;
            }

            // Step 2: check each feature n create flag, correspondingly check
            // for parameters
            // Step 2.a: start with color balance
            if (featureList.contains(DCM_FEATURE.FEATURE_COLOR_BALANCE)) {
                // check if that feature is supported
                if (isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_BALANCE)) {
                    // check if Color Balance value is passed correct
                    if (colorBalance >= COLOR_BALANCE_WARMTH_LOWER_BOUND
                            && colorBalance <= COLOR_BALANCE_WARMTH_UPPER_BOUND) {
                        // set flags
                        FLAG = FLAG | BITFLAG_COLOR_BALANCE;
                    } else {
                        Log.e(TAG, "Color balance value passed is out of range");
                        return RET_VALUE_OUT_OF_RANGE;
                    }
                } else {
                    Log.e(TAG, "FEATURE_COLOR_BALANCE is not supported");
                    return RET_NOT_SUPPORTED;
                }
            }

            // step 2.b: add other features as and wen they r exposed

            // when all features checked, then call service method
            // here, flag shouldnt be zero
            if (FLAG == 0) {
                Log.e(TAG, "FeatureList is incomplete. Colorbalance missing");
                return RET_FAILURE;
            }
            try {
                Log.v(TAG, "For createNewMode- " + modeName + " " + FLAG + " " + colorBalance);
                int retVal = this.service.createNewMode(this.displayId, modeName, FLAG, colorBalance);
                if (retVal >= 0) {// passed
                    Log.v(TAG, "createNewMode() worked");
                } else {
                    Log.e(TAG, "Service createNewMode failed with return value " + retVal);
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service create new mode failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method will modify the settings encapsulated by an existing USER
     * mode, including its name. If the name needs to be unchanged, pass in the
     * existing name as newName parameter.
     * @param modeId
     *            id of the mode to be modified
     * @param newName
     *            new name for the mode. Pass same name if name need not be
     *            modified
     * @param featureList
     *            EnumSet of features that need to be set for the new mode,
     *            excluding FEATURE_COLOR_MODE_MANAGEMENT and
     *            FEATURE_COLOR_MODE_SELECTION. These feature will be ignored if
     *            passed in the enumset.
     * @param colorBalance
     *            Color balance value for the mode
     * @return RET_SUCCESS if API succeeds <br>
     *         RET_PERMISSION_DENIED if a non system app tries to use this API
     *         or attempts are made to modify a SYSTEM mode <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_MANAGEMENT or any of the
     *         features in the featurelist are not supported, <br>
     *         RET_VALUE_OUT_OF_RANGE if any of the provided values is outside
     *         the range, <br>
     *         RET_FAILURE for all other failures
     */
    public int modifyMode(int modeId, String newName, EnumSet<DCM_FEATURE> featureList, int colorBalance) {
        if (myInstance[displayId] != null) {
            long FLAG = 0;
            // checK if feature is supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_MANAGEMENT);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            // check if system app
            if (!isSystemApp) {
                Log.e(TAG, "You do not have permission to perform this operation");
                return RET_PERMISSION_DENIED;
            }

            // check if id is non negative
            if (modeId < 0) {
                Log.e(TAG, "Mode id passed is negative");
                return RET_FAILURE;
            }
            // check if name exists
            if (newName == null || newName.equals("") || newName.length() > MODE_NAME_MAX_LENGTH) {
                Log.e(TAG, "Mode name is missing or crossing max length");
                return RET_FAILURE;
            }

            // now play with arguments
            // Step 1: if featurelist null return RET_FAILURE
            if (featureList == null || featureList.isEmpty()) {
                Log.e(TAG, "FeatureList missing");
                return RET_FAILURE;
            }

            // Step 2: check each feature n create flag, correspondingly check
            // for parameters
            // Step 2.a: start with color balance
            if (featureList.contains(DCM_FEATURE.FEATURE_COLOR_BALANCE)) {
                // check if that feature is supported
                if (isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_BALANCE)) {
                    // check if Color Balance value is passed correct
                    if (colorBalance >= COLOR_BALANCE_WARMTH_LOWER_BOUND
                            && colorBalance <= COLOR_BALANCE_WARMTH_UPPER_BOUND) {
                        // set flags
                        FLAG = FLAG | BITFLAG_COLOR_BALANCE;
                    } else {
                        Log.e(TAG, "Color balance value passed is out of range");
                        return RET_VALUE_OUT_OF_RANGE;
                    }
                } else {
                    Log.e(TAG, "FEATURE_COLOR_BALANCE is not supported");
                    return RET_NOT_SUPPORTED;
                }
            }

            // step 2.b: add other features as and wen they r exposed

            // when all features checked, then call service method
            // here, flag shouldnt be zero
            if (FLAG == 0) {
                Log.e(TAG, "FeatureList is incomplete. Colorbalance missing");
                return RET_FAILURE;
            }
            try {
                Log.v(TAG, "For modifyMode- " + modeId + " " + newName + " " + FLAG + " " + colorBalance);
                int retVal = this.service.modifyModeAllFeatures(this.displayId, modeId, newName);
                if (retVal == 0) {// passed
                    Log.v(TAG, "modifyMode() worked");
                } else {
                    Log.e(TAG, "Service modifyMode failed with return value " + retVal);
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service modify mode failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method returns the id of the default display mode.
     * @return id of the mode which is currently default for the display, <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_SELECTION is not
     *         supported, <br>
     *         RET_FAILURE for all other failures
     */
    public int getDefaultMode() {
        int retVal = RET_FAILURE;
        ;
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_SELECTION);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            //
            try {
                Log.v(TAG, "Calling getDefaultMode() for display " + this.displayId);
                retVal = this.service.getDefaultMode(this.displayId);
                if (retVal < 0) {// failed
                    Log.e(TAG, "Service getDefaultMode failed with return value " + retVal);
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service get default mode failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }


    /**
     * This method will set a mode as the default display mode for the device.
     * This requires that the application have system privileges as a default
     * mode will persist a device reboot.
     * @param modeId
     *            the id of the mode which needs to be set as default
     * @return RET_SUCCESS if the API succeeds <br>
     *         RET_PERMISSION_DENIED if a non system app tries to use this API <br>
     *         RET_VALUE_OUT_OF_RANGE if mode id provided is negative <br>
     *         RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_SELECTION is not
     *         supported, <br>
     *         RET_FAILURE for all other failures
     */
    public int setDefaultMode(int modeId) {
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_SELECTION);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            //
            // check if system app
            if (!isSystemApp) {
                Log.e(TAG, "You do not have permission to perform this operation");
                return RET_PERMISSION_DENIED;
            }
            if (modeId < 0) {
                Log.e(TAG, "Mode ID provided is less than 0");
                return RET_VALUE_OUT_OF_RANGE;
            } else {
                try {
                    Log.v(TAG, "Calling setDefaultMode for display " + this.displayId);
                    int retVal = this.service.setDefaultMode(this.displayId, modeId);
                    if (retVal == 0) {// passed
                        Log.v(TAG, "SetDefaultMode() worked");
                    } else {
                        Log.e(TAG, "Service setDefaultMode failed with return value " + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;

                } catch (RemoteException e) {
                    Log.e(TAG, "Service set default mode failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method will create a new mode with the input mode name. The new mode
     *  will be of the type USER.
     * The method will return the ID of the new mode if successful, or an error code
     *  if not successful. The mode name must be unique.
     * When a new mode is created, all features, except active control features like
     * Sunlight Visibility and Adaptive Backlight parameters, are saved.
     * @param modeName name for the new mode
     * @return New Mode Id (positive number) if API succeeds
     *         <br>RET_PERMISSION_DENIED if a non system app tries to use this API
     *         <br>RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_MANAGEMENT is not supported,
     *         <br>RET_FAILURE for all other failures
     */
    /*New*/
    public int createNewMode(String modeName){
        if (myInstance[displayId] != null) {
            long FLAG = 0;

            // checK if feature is supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_MANAGEMENT);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            // check if system app
            if (!isSystemApp) {
                Log.e(TAG, "You do not have permission to perform this operation");
                return RET_PERMISSION_DENIED;
            }

            // check if name exists
            if (modeName == null || modeName.equals("") || modeName.length() > MODE_NAME_MAX_LENGTH) {
                Log.e(TAG, "Mode name is missing or crossing max length");
                return RET_FAILURE;
            }

            try {
                Log.v(TAG, "For createNewMode- " + modeName);
                int retVal = this.service.createNewModeAllFeatures(this.displayId, modeName);
                if (retVal >= 0) {// passed
                    Log.v(TAG, "createNewMode() worked");
                } else {
                    Log.e(TAG, "Service createNewMode failed with return value " + retVal);
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service create new mode failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method will modify the settings encapsulated by an existing USER mode,
     *  including its name.
     * If the name needs to be unchanged, pass in the existing name as newName parameter.
     * @param modeId id of the mode to be modified
     * @param newName new name for the mode. Pass same name if name need not be modified
     * @return RET_SUCCESS if API succeeds
     *         <br>RET_PERMISSION_DENIED if a non system app tries to use this API
     *         <br>RET_NOT_SUPPORTED if FEATURE_COLOR_MODE_MANAGEMENT is not supported,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int modifyMode(int modeId, String newName){
        if (myInstance[displayId] != null) {
            long FLAG = 0;
            // checK if feature is supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_COLOR_MODE_MANAGEMENT);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            // check if system app
            if (!isSystemApp) {
                Log.e(TAG, "You do not have permission to perform this operation");
                return RET_PERMISSION_DENIED;
            }

            // check if id is non negative
            if (modeId < 0) {
                Log.e(TAG, "Mode id passed is negative");
                return RET_FAILURE;
            }
            // check if name exists
            if (newName == null || newName.equals("") || newName.length() > MODE_NAME_MAX_LENGTH) {
                Log.e(TAG, "Mode name is missing or crossing max length");
                return RET_FAILURE;
            }
            try {
                Log.v(TAG, "For modifyMode- " + modeId + " " + newName);
                int retVal = this.service.modifyModeAllFeatures(this.displayId, modeId, newName);
                if (retVal == 0) {// passed
                    Log.v(TAG, "modifyMode() worked");
                } else {
                    Log.e(TAG, "Service modifyMode failed with return value " + retVal);
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service modify mode failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }
    // end mode

    // SVI
    /**
     * This API gets the maximum limit of sunlight visibility strength
     * @return max sunlight visibility strength value
     *      <br> RET_NOT_SUPPORTED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is
     *         not supported
     *      <br> RET_FEATURE_DISABLED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled
     *      <br> RET_FAILURE for all other failures
     */
    public int getMaxSunlightVisibilityStrength() {
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(isSunlightVisibilityEnabled() == false){
                Log.e(TAG, "FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled");
                return RET_FEATURE_DISABLED;
            }
            //
            try {
                Log.v(TAG, "Calling getMaxSVI for display " + this.displayId);
                int retVal = this.service.getRangeSunlightVisibilityStrength(this.displayId, 1);
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMaxSVI failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API gets the minimum limit of sunlight visibility strength
     * @return minimum sunlight visibility strength value
     *          <br>RET_NOT_SUPPORTED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is
     *          not supported
     *          <br> RET_FEATURE_DISABLED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled
     *          <br>RET_FAILURE for all other failures
     */
    public int getMinSunlightVisibilityStrength(){
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(isSunlightVisibilityEnabled() == false){
                Log.e(TAG, "FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled");
                return RET_FEATURE_DISABLED;
            }
            //
            try {
                Log.v(TAG, "Calling getMinSVI for display " + this.displayId);
                int retVal = this.service.getRangeSunlightVisibilityStrength(this.displayId, 0);
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMinSVI failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API sets the sunlight visibility strength. This API would fail if the
     *  sunlight visibility feature is not running.
     * @param strengthVal
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is
     *          not supported,
     *         <br> RET_FEATURE_DISABLED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled
     *         <br>RET_VALUE_OUT_OF_RANGE if provided value is outside the range
     *         <br>RET_FAILURE for all other failures. Make sure this feature is enabled
     */
    public int setSunlightVisibilityStrength(int strengthVal){
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT);
                if (isSupported != true) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(isSunlightVisibilityEnabled() == false){
                Log.e(TAG, "FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled");
                return RET_FEATURE_DISABLED;
            }
            //
            if (strengthVal < getMinSunlightVisibilityStrength()
                    || strengthVal > getMaxSunlightVisibilityStrength()) {
                Log.e(TAG, "strengthVal given is outside the range ("
                        + getMinSunlightVisibilityStrength() + ","
                        + getMaxSunlightVisibilityStrength() + ")");
                return RET_VALUE_OUT_OF_RANGE;
            } else {
                try {
                    Log.v(TAG, "Calling setSVI for display " + this.displayId);
                    int retVal = this.service.setSunlightVisibilityStrength(this.displayId,
                            strengthVal);
                    if (retVal == 0) {// passed
                        Log.v(TAG, "setSunlightVisibilityStrength() worked");
                    } else {
                        Log.e(TAG,
                                "Service setSunlightVisibilityStrength failed with return value "
                                        + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;

                } catch (RemoteException e) {
                    Log.e(TAG, "Service setSunlightVisibilityStrength failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API gets the current value set for sunlight visibility strength.
     * This API would fail if the sunlight visibility feature is not running.
     * @return Strength value if API succeeds
     *          <br>RET_NOT_SUPPORTED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is
     *          not supported,
     *          <br> RET_FEATURE_DISABLED if FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled
     *          <br>RET_FAILURE for all other failures, make sure the feature is enabled
     */
    public int getSunlightVisibilityStrength(){
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(isSunlightVisibilityEnabled() == false){
                Log.e(TAG, "FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is disabled");
                return RET_FEATURE_DISABLED;
            }
            //
            try {
                Log.v(TAG, "Calling getSunlightVisibilityStrength for display " + this.displayId);
                int retVal = this.service.getSunlightVisibilityStrength(this.displayId);
                if (retVal < getMinSunlightVisibilityStrength() ) {// failed
                    Log.e(TAG, "Service getSunlightVisibilityStrength failed with return value " + retVal);
                    return RET_FAILURE;
                }
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service getSunlightVisibilityStrength failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }
    // end SVI

    //Backlight
    /**
     * This API gets the quality level of adaptive backlight. Successful return of
     * this function does not imply that adaptive backlight is running. This function
     * may return the quality level with which adaptive backlight can be later started.
     * @return quality level if successful which will be an enum value from
     *  ADAPTIVE_BACKLIGHT_QUALITY_LEVEL, null if feature is disabled and for other failures
     */
    public ADAPTIVE_BACKLIGHT_QUALITY_LEVEL getBacklightQualityLevel(){
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_ADAPTIVE_BACKLIGHT);
                if (isSupported == false) {
                    Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is not supported" );
                    return null;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is not supported" );
                return null;
            }
            if(isAdaptiveBacklightEnabled() == false){
                Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is disabled");
                return null;
            }
            //
            try {
                Log.v(TAG, "Calling getBacklightQualityLevel for display " + this.displayId);
                int retVal = this.service.getBacklightQualityLevel(this.displayId);
                if(retVal < 0){
                    Log.v(TAG, "getBacklightQualityLevel returned error value " + retVal);
                    return null;
                }
                try{
                    return ADAPTIVE_BACKLIGHT_QUALITY_LEVEL.values()[retVal];
                }
                catch(Exception e){
                    Log.e(TAG, "Level type mismatch");
                    return null;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getBacklightQualityLevel failed");
                return null;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return null;
        }
    }

    /**
     * This API sets the quality level of adaptive backlight. Successful return of
     * this function does not imply that adaptive backlight is running. If adaptive
     * backlight is not running then this quality level shall be applied when the
     * feature has been started next
     * @param level enum value from ADAPTIVE_BACKLIGHT_QUALITY_LEVEL
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_ADAPTIVE_BACKLIGHT is not supported
     *         <br> RET_FEATURE_DISABLED if FEATURE_ADAPTIVE_BACKLIGHT is disabled
     *         <br>RET_ILLEGAL_ARGUMENT if level is null
     *         <br>RET_FAILURE for all other failures
     */
    public int setBacklightQualityLevel(ADAPTIVE_BACKLIGHT_QUALITY_LEVEL level){
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_ADAPTIVE_BACKLIGHT);
                if (isSupported == false) {
                    Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is not supported" );
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is not supported" );
                return RET_NOT_SUPPORTED;
            }
            if(isAdaptiveBacklightEnabled() == false){
                Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is disabled");
                return RET_FEATURE_DISABLED;
            }
            if(level == null){
                Log.e(TAG, "level passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            else{
                try{
                    Log.v(TAG, "Calling setBacklightQualityLevel for display " + this.displayId);
                    int retVal = this.service.setBacklightQualityLevel(this.displayId, level.getValue());
                    if (retVal == 0) {// passed
                        Log.v(TAG, "setBacklightQualityLevel() worked");
                    } else {
                        Log.e(TAG, "Service setBacklightQualityLevel failed with return value " + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;
                } catch (RemoteException e) {
                    Log.e(TAG, "Service setBacklightQualityLevel failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API gets the Backlight scale as percentage. This function will fail if
     * adaptive backlight is not running.
     * @return scale value as percentrage if API succeeds
     *          <br>RET_NOT_SUPPORTED if FEATURE_ADAPTIVE_BACKLIGHT is not supported
     *          <br> RET_FEATURE_DISABLED if FEATURE_ADAPTIVE_BACKLIGHT is disabled
     *          <br>RET_FAILURE for all other failures
     */
    public int getAdaptiveBacklightScale(){
        if (myInstance[displayId] != null) {
            // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_ADAPTIVE_BACKLIGHT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(isAdaptiveBacklightEnabled() == false){
                Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is disabled");
                return RET_FEATURE_DISABLED;
            }
            //
            try {
                Log.v(TAG, "Calling getAdaptiveBacklightScale for display " + this.displayId);
                int retVal = this.service.getAdaptiveBacklightScale(this.displayId);
                return retVal;

            } catch (RemoteException e) {
                Log.e(TAG, "Service getAdaptiveBacklightScale failed");
                return RET_FAILURE;
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }
    //end Backlight

    //active feature
    /**
     * This API will return true if Sunlight Visibility Improvement is enabled.
     * @return true if enabled, false if disabled
     */
    public boolean isSunlightVisibilityEnabled(){
        if (myInstance[displayId] != null) {
         // check if feature supported
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT);
                if (isSupported == false) {
                    Log.e(TAG, "FEATURE_SUNLIGHT_VISIBILITY_IMPROVEMENT is not supported");
                    return false;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return false;
            }
            try {
                Log.v(TAG, "Calling isSunlightVisibilityEnabled for display " + this.displayId);
                int retVal = this.service.isActiveFeatureOn(this.displayId,
                        ACTIVE_FEATURE_TYPE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT.getValue());
                if (retVal == 1) {// passed //nothing to do with on/off value,
                                  // native returns 1 for on
                    Log.v(TAG, "isActiveFeatureOn returned retVal");
                    return true;
                } else {
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Service isActiveFeatureOn failed");
                return false;
            }

        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return false;
        }
    }

    /**
     * This API will return true if Adaptive Backlight is enabled.
     * @return true if enabled, false if disabled
     */
    public boolean isAdaptiveBacklightEnabled(){
        if (myInstance[displayId] != null) {
            // check if feature supported
               try {
                   boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_ADAPTIVE_BACKLIGHT);
                   if (isSupported == false) {
                       Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is not supported");
                       return false;
                   }
               } catch (IllegalArgumentException e) {
                   e.printStackTrace();
                   return false;
               }
            try {
                Log.v(TAG, "Calling isAdaptiveBacklightEnabled for display " + this.displayId);
                int retVal = this.service.isActiveFeatureOn(this.displayId,
                        ACTIVE_FEATURE_TYPE.FEATURE_ADAPTIVE_BACKLIGHT.getValue());
                if (retVal == 1) {// passed
                    Log.v(TAG, "isActiveFeatureOn returned retVal");
                    return true;
                } else {
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Service isActiveFeatureOn failed");
                return false;
            }

           } else {
               Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
               return false;
           }
    }

    /**
     * This API will either start/stop or enable/disable sunlight visibility improvement
     * feature.
     * @param enable pass true if need to enable or false to disable
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_SUNLIGHT_VISIBILITY_IMPROVEMENT is not supported
     *         <br>RET_FAILURE for all other failures
     */
    public int setSunlightVisibilityEnabled(boolean enable){
        if (myInstance[displayId] != null) {
            if (!isFeatureSupported(DCM_FEATURE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT)){
                Log.e(TAG, "FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT is not supported");
                return RET_NOT_SUPPORTED;
            }
 else {
                try {
                    Log.v(TAG, "Calling setActiveFeatureControl for display " + this.displayId);
                    CONTROL_REQUEST_TYPE request = (enable == true) ? CONTROL_REQUEST_TYPE.ON
                            : CONTROL_REQUEST_TYPE.OFF;
                    int retVal = this.service.setActiveFeatureControl(this.displayId,
                            ACTIVE_FEATURE_TYPE.FEATURE_SUNLIGHT_VISBILITY_IMPROVEMENT.getValue(),
                            request.getValue());
                    if (retVal == 0) {// passed
                        Log.v(TAG, "setActiveFeatureControl() worked");
                    } else {
                        Log.e(TAG, "Service setActiveFeatureControl failed with return value "
                                + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;

                } catch (RemoteException e) {
                    Log.e(TAG, "Service setActiveFeatureControl failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API will either start/stop or enable/disable Adaptive backlight feature.
     * @param enable pass true if need to enable or false to disable
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_ADAPTIVE_BACKLIGHT is not supported
     *         <br>RET_FAILURE for all other failures
     */
    public int setAdaptiveBacklightEnabled(boolean enable){
        if (myInstance[displayId] != null) {
            if (!isFeatureSupported(DCM_FEATURE.FEATURE_ADAPTIVE_BACKLIGHT)){
                Log.e(TAG, "FEATURE_ADAPTIVE_BACKLIGHT is not supported");
                return RET_NOT_SUPPORTED;
            }
            else{
                try {
                    Log.v(TAG, "Calling setActiveFeatureControl for display " + this.displayId);
                    CONTROL_REQUEST_TYPE request = (enable == true) ? CONTROL_REQUEST_TYPE.ON
                            : CONTROL_REQUEST_TYPE.OFF;
                    int retVal = this.service.setActiveFeatureControl(this.displayId,
                            ACTIVE_FEATURE_TYPE.FEATURE_ADAPTIVE_BACKLIGHT.getValue(),
                            request.getValue());
                    if (retVal == 0) {// passed
                        Log.v(TAG, "setActiveFeatureControl() worked");
                    } else {
                        Log.e(TAG, "Service setActiveFeatureControl failed with return value "
                                + retVal);
                        return RET_FAILURE;
                    }
                    return retVal;

                } catch (RemoteException e) {
                    Log.e(TAG, "Service setActiveFeatureControl failed");
                    return RET_FAILURE;
                }
            }
        } else {
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }
    //end active feature

    //Mem color config
    /**
     * This API will return the maximum value allowed by the framework for the input
     *  parameter passed in the API. This API only applies to Memory Color parameters.
     * @param type enum value of type MEMORY_COLOR_TYPE
     * @param parameter enum value from MEMORY_COLOR_DATA, denoting the required data
     * @return the maximum value for the requested data
     *          <br>RET_NOT_SUPPORTED if FEATURE_MEMORY_COLOR_ADJUSTMENT is not supported,
     *         <br>RET_ILLEGAL_ARGUMENT if type or parameter is null,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int getMaxLimitMemoryColor(MEMORY_COLOR_TYPE type,
            MEMORY_COLOR_PARAMS parameter){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_MEMORY_COLOR_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(type == null || parameter == null){
                Log.e(TAG, "Feature type or request passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            try {
              //try to retrieve local value
                String key = type+""+parameter+"max";
                Integer maxVal = memColorRanges.get(key);
                if(maxVal != null){
                    return maxVal.intValue();
                }
                Log.v(TAG, "Calling getMaxLimitMemoryColor for display " + this.displayId);
                int[] retVal = this.service.getRangeMemoryColorParameter(this.displayId, type.getValue());
                if(retVal == null || retVal.length < 6){
                    Log.e(TAG, "Service did not return proper parameter");
                    return RET_FAILURE;
                }
                else{
                    int index = parameter.getValue()*2+1;
                    memColorRanges.put(key, retVal[index]);
                    return retVal[index];
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMaxLimitMemoryColor failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API will return the minimum value allowed by the framework for the input
     *  parameter passed in the API. This API only applies to Memory Color Parameters.
     * @param type enum value of type MEMORY_COLOR_TYPE
     * @param parameter enum value from MEMORY_COLOR_DATA, denoting the required data
     * @return the minimum value for the requested data
     *          <br>RET_NOT_SUPPORTED if FEATURE_MEMORY_COLOR_ADJUSTMENT is not supported,
     *         <br>RET_ILLEGAL_ARGUMENT if type or parameter is null,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int getMinLimitMemoryColor(MEMORY_COLOR_TYPE type,
            MEMORY_COLOR_PARAMS parameter){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_MEMORY_COLOR_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(type == null || parameter == null){
                Log.e(TAG, "Feature type or request passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            try {
                //try to retrieve local value
                String key = type+""+parameter+"min";
                Integer minVal = memColorRanges.get(key);
                if(minVal != null){
                    return minVal.intValue();
                }
                Log.v(TAG, "Calling getMinLimitMemoryColor for display " + this.displayId);
                int[] retVal = this.service.getRangeMemoryColorParameter(this.displayId, type.getValue());
                if(retVal == null || retVal.length < 6){
                    Log.e(TAG, "Service did not return proper parameter");
                    return RET_FAILURE;
                }
                else{
                    int index = parameter.getValue()*2;
                    memColorRanges.put(key, retVal[index]);
                    return retVal[index];
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMinLimitMemoryColor failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method will change the memory color parameters to the input value.
     * @param memColorConfig
     *            An instance of MemoryColorConfig, enclosing all memory color
     *             config values like hue, saturation and intensity as well as the
     *              type of mem color to be set
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_MEMORY_COLOR_ADJUSTMENT is not supported,
     *         <br>RET_ILLEGAL_ARGUMENT if memConfig or memory color type is null,
     *         <br>RET_VALUE_OUT_OF_RANGE if any parameter is outside the specified range,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int setMemoryColorParams(MemoryColorConfig memConfig){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_MEMORY_COLOR_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(memConfig == null || memConfig.getMemoryColorType() == null){
                Log.e(TAG, "MemoryColorConfig instance or the Memory color type passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            if (memConfig.getHue() < getMinLimitMemoryColor(memConfig.getMemoryColorType(),
                    MEMORY_COLOR_PARAMS.HUE)
                    || memConfig.getHue() > getMaxLimitMemoryColor(memConfig.getMemoryColorType(),
                            MEMORY_COLOR_PARAMS.HUE)) {
                Log.e(TAG, "Hue is out of range");
                return RET_VALUE_OUT_OF_RANGE;
            } else if (memConfig.getSaturation() < getMinLimitMemoryColor(
                    memConfig.getMemoryColorType(), MEMORY_COLOR_PARAMS.SATURATION)
                    || memConfig.getSaturation() > getMaxLimitMemoryColor(
                            memConfig.getMemoryColorType(), MEMORY_COLOR_PARAMS.SATURATION)) {
                Log.e(TAG, "Saturation is out of range");
                return RET_VALUE_OUT_OF_RANGE;
            } else if (memConfig.getIntensity() < getMinLimitMemoryColor(
                    memConfig.getMemoryColorType(), MEMORY_COLOR_PARAMS.INTENSITY)
                    || memConfig.getIntensity() > getMaxLimitMemoryColor(
                            memConfig.getMemoryColorType(), MEMORY_COLOR_PARAMS.INTENSITY)) {
                Log.e(TAG, "Intensity value is out of range");
                return RET_VALUE_OUT_OF_RANGE;
            }
            try {
                Log.v(TAG, "Calling setMemoryColorParams for display " + this.displayId);
                int retVal = this.service.setMemoryColorParameters(this.displayId, memConfig
                        .getMemoryColorType().getValue(), memConfig.getHue(), memConfig
                        .getSaturation(), memConfig.getIntensity());
                if (retVal != RET_SUCCESS) {
                    Log.e(TAG, "setMemorycolorParams failed with return value " + retVal);
                    return RET_FAILURE;
                } else {
                    return retVal;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service setMemoryColorParams failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method returns the current Memory color value based on the type requested
     * @param type enum value of type MEMORY_COLOR_TYPE
     * @return MemColorConfig instance enclosing all the current Memory Color values
     *  for the requested type
     *         <br>null for failures
     */
    /*NEW*/
    public MemoryColorConfig getMemoryColorParams(MEMORY_COLOR_TYPE type){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_MEMORY_COLOR_ADJUSTMENT);
                if (isSupported == false) {
                    Log.e(TAG, "FEATURE_MEMORY_COLOR_ADJUSTMENT is not supported");
                    return null;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                Log.e(TAG, "FEATURE_MEMORY_COLOR_ADJUSTMENT is not supported");
                return null;
            }
            if(type == null){
                Log.e(TAG, "Mem color type passed is null" );
                return null;
            }
            try {
                Log.v(TAG, "Calling getMemoryColorParams for display " + this.displayId);
                int[] retVal = this.service.getMemoryColorParameters(this.displayId, type.getValue());
                if(retVal == null || retVal.length < 4){
                    Log.e(TAG, "getMemorycolorParams failed ");
                    return null;
                }
                else{
                    MemoryColorConfig memConfig = new MemoryColorConfig(type, retVal[0], retVal[1], retVal[2]);
                    memConfig.isEnabled =  (retVal[3]==0)?false:true;
                    return memConfig;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMemoryColorParams failed");
                return null;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return null;
        }
    }

    /**
     * This API will disable the memory color configuration based on the type passed.
     * Once disabled, all memory color parameters which were set for the type will be lost.
     * To enable the feature, call setMemoryColorParams().
     * @param type MEMORY_COLOR_TYPE which needs to be disabled.
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_MEMORY_COLOR_ADJUSTMENT is not supported,
     *         <br>RET_FAILURE for all other failures
     */
    public int disableMemoryColorConfig(MEMORY_COLOR_TYPE type){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_MEMORY_COLOR_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(type == null){
                Log.e(TAG, "Memory color type passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            try {
                Log.v(TAG, "Calling disableMemoryColorConfig for display " + this.displayId);
                int retVal = this.service.disableMemoryColorConfiguration(this.displayId, type.getValue());
                if(retVal != RET_SUCCESS){
                    Log.e(TAG, "disableMemoryColorConfig failed with return value "+retVal);
                    return RET_FAILURE;
                }
                else{
                    return retVal;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service disableMemoryColorConfig failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }
    //end mem color config

    // pa
    /**
     * This API will return the maximum value allowed by the framework for the input
     *  parameter passed in the API. This API only applies to Global Picture Adjustment.
     * @param parameter enum value from PICTURE_ADJUSTMENT_PARAMS, denoting
     *  the required data
     * @return the maximum value for the requested data
     *         <br>RET_NOT_SUPPORTED if FEATURE_GLOBAL_PICTURE_ADJUSTMENT is not
     *          supported,
     *         <br>RET_ILLEGAL_ARGUMENT if parameter is null,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS parameter){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(parameter == null){
                Log.e(TAG, "Parameter passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            try {
              //try to retrieve local value
                String key = parameter+"max";
                Integer maxVal = paRanges.get(key);
                if(maxVal != null){
                    return maxVal.intValue();
                }
                Log.v(TAG, "Calling getMaxLimitPictureAdjustment for display " + this.displayId);
                int[] retVal = this.service.getRangePAParameter(this.displayId);
                if(retVal == null || retVal.length < 10){
                    Log.e(TAG, "Service did not return proper parameter");
                    return RET_FAILURE;
                }
                else{
                    int index = parameter.getValue()*2+1;
                    paRanges.put(key, retVal[index]);
                    return retVal[index];
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMaxLimitPictureAdjustment failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API will return the minimum value allowed by the framework for the input
     *  parameter passed in the API. This API only applies to Global Picture Adjustment.
     * @param parameter enum value from PICTURE_ADJUSTMENT_PARAMS, denoting the
     *  required parameter
     * @return the minimum value for the requested parameter
     *          <br>RET_NOT_SUPPORTED if FEATURE_GLOBAL_PICTURE_ADJUSTMENT is not
     *           supported,
     *         <br>RET_ILLEGAL_ARGUMENT if parameter is null,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS parameter){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(parameter == null){
                Log.e(TAG, "Parameter passed is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            try {
                //try to retrieve local value
                String key = parameter+"min";
                Integer minVal = paRanges.get(key);
                if(minVal != null){
                    return minVal.intValue();
                }
                Log.v(TAG, "Calling getMinLimitPictureAdjustment for display " + this.displayId);
                int[] retVal = this.service.getRangePAParameter(this.displayId);
                if(retVal == null || retVal.length < 10){
                    Log.e(TAG, "Service did not return proper parameter");
                    return RET_FAILURE;
                }
                else{
                    int index = parameter.getValue()*2;
                    paRanges.put(key, retVal[index]);
                    return retVal[index];
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getMaxLimitPictureAdjustment failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method will change the picture adjustment parameters to the input value.
     * @param PictureAdjustmentConfig
     *            An instance of PictureAdjustmentConfig, enclosing all picture
     *             adjustment values like hue, saturation, intensity and contrast
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_GLOBAL_PICTURE_ADJUSTMENT is not supported,
     *         <br>RET_ILLEGAL_ARGUMENT if paConfig is null,
     *         <br>RET_VALUE_OUT_OF_RANGE if any parameter is outside the specified range,
     *         <br>RET_FAILURE for all other failures
     */
    /*
     * here, will always send desat and disable bits as 0.
     */
    /*NEW*/
    public int setPictureAdjustmentParams(PictureAdjustmentConfig paConfig){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(paConfig == null || paConfig.getParamFlags() == null){
                Log.e(TAG, "Parameter passed is null or the enumset of parameters is null" );
                return RET_ILLEGAL_ARGUMENT;
            }
            int flag = 0;
            EnumSet<PICTURE_ADJUSTMENT_PARAMS> dataSet = paConfig.getParamFlags();
            //hue
            if( dataSet.contains(PICTURE_ADJUSTMENT_PARAMS.HUE)){
                flag = flag | PA_GLOBAL_HUE;
                if(paConfig.getHue() < getMinLimitPictureAdjustment( PICTURE_ADJUSTMENT_PARAMS.HUE)
                        || paConfig.getHue() > getMaxLimitPictureAdjustment( PICTURE_ADJUSTMENT_PARAMS.HUE)){
                    Log.e(TAG, "Hue is out of range");
                    return RET_VALUE_OUT_OF_RANGE;
                }
            }
            // saturation
            if (dataSet.contains(PICTURE_ADJUSTMENT_PARAMS.SATURATION)) {
                flag = flag | PA_GLOBAL_SAT;
                if (paConfig.getSaturation() < getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.SATURATION)
                        || paConfig.getSaturation() > getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.SATURATION)) {
                    Log.e(TAG, "Saturation is out of range");
                    return RET_VALUE_OUT_OF_RANGE;
                }
            }
            // intensity
            if (dataSet.contains(PICTURE_ADJUSTMENT_PARAMS.INTENSITY)) {
                flag = flag | PA_GLOBAL_VAL;
                if (paConfig.getIntensity() < getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.INTENSITY)
                        || paConfig.getIntensity() > getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.INTENSITY)) {
                    Log.e(TAG, "Intensity is out of range");
                    return RET_VALUE_OUT_OF_RANGE;
                }
            }
            // contrast
            if (dataSet.contains(PICTURE_ADJUSTMENT_PARAMS.CONTRAST)) {
                flag = flag | PA_GLOBAL_CON;
                if (paConfig.getContrast() < getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.CONTRAST)
                        || paConfig.getContrast() > getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.CONTRAST)) {
                    Log.e(TAG, "Contrast is out of range");
                    return RET_VALUE_OUT_OF_RANGE;
                }
            }
            // threshold
            if (dataSet.contains(PICTURE_ADJUSTMENT_PARAMS.SATURATION_THRESHOLD)) {
                flag = flag | PA_GLOBAL_SAT_THRESH;
                if (paConfig.getSaturationThreshold() < getMinLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.SATURATION_THRESHOLD)
                        || paConfig.getSaturationThreshold() >
                getMaxLimitPictureAdjustment(PICTURE_ADJUSTMENT_PARAMS.SATURATION_THRESHOLD)) {
                    Log.e(TAG, "Saturation threshold is out of range");
                    return RET_VALUE_OUT_OF_RANGE;
                }
            }
            if(flag  == 0){
                Log.e(TAG, "The enumset passed is empty");
                return RET_ILLEGAL_ARGUMENT;
            }
            try {
                Log.v(TAG, "Calling setPAParameters for display " + this.displayId);
                int retVal = this.service.setPAParameters(this.displayId, flag, paConfig.getHue(), paConfig.getSaturation(),
                        paConfig.getIntensity(), paConfig.getContrast(), paConfig.getSaturationThreshold());
                if(retVal != RET_SUCCESS){
                    Log.e(TAG, "setPAParameters failed with return value "+retVal);
                    return RET_FAILURE;
                }
                else{
                    return retVal;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service setPAParameters failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API will set the global saturation threshold below which no adjustment happens.
     * @param thresholdVal
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_GLOBAL_PICTURE_ADJUSTMENT is not supported,
     *         <br>RET_VALUE_OUT_OF_RANGE if thresholdVal is outside the specified range,
     *         <br>RET_FAILURE for all other failures
     */
    public int setPictureAdjustmentSaturationThreshold(int thresholdVal){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            if(thresholdVal < getMinLimitPictureAdjustment( PICTURE_ADJUSTMENT_PARAMS.SATURATION_THRESHOLD)
                    || thresholdVal > getMaxLimitPictureAdjustment( PICTURE_ADJUSTMENT_PARAMS.SATURATION_THRESHOLD)){
                Log.e(TAG, "Saturation threshold is out of range");
                return RET_VALUE_OUT_OF_RANGE;
            }
            try {
                Log.v(TAG, "Calling setPictureAdjustmentSaturationThreshold for display " + this.displayId);
                int retVal = this.service.setPAParameters(this.displayId, PA_GLOBAL_SAT_THRESH, 0, 0,
                        0, 0, thresholdVal);
                if(retVal != RET_SUCCESS){
                    Log.e(TAG, "setPictureAdjustmentSaturationThreshold failed with return value "+retVal);
                    return RET_FAILURE;
                }
                else{
                    return retVal;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service setPictureAdjustmentSaturationThreshold failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method will apply complete desaturation. However, any pixel
     *  with memory color adjustment would still show effect of saturation
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_GLOBAL_PICTURE_ADJUSTMENT is not supported,
     *         <br>RET_FAILURE for all other failures
     */
    public int enablePictureAdjustmentDesaturation(){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            try {
                Log.v(TAG, "Calling enablePictureAdjustmentDesaturation for display " + this.displayId);
                int retVal = this.service.setPAParameters(this.displayId, PA_GLOBAL_DESAT, 0, 0,
                        0, 0, 0);
                if(retVal != RET_SUCCESS){
                    Log.e(TAG, "enablePictureAdjustmentDesaturation failed with return value "+retVal);
                    return RET_FAILURE;
                }
                else{
                    return retVal;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service enablePictureAdjustmentDesaturation failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This API will disable the picture adjustment configuration.
     * Once disabled, all picture adjustment parameters which were set will be lost.
     * To enable the feature, call setPictureAdjustmentParams().
     * @return RET_SUCCESS if api succeeds,
     *         <br>RET_NOT_SUPPORTED if FEATURE_GLOBAL_PICTURE_ADJUSTMENT is not supported,
     *         <br>RET_FAILURE for all other failures
     */
    /*NEW*/
    public int disablePictureAdjustmentConfig(){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return RET_NOT_SUPPORTED;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return RET_NOT_SUPPORTED;
            }
            try {
                Log.v(TAG, "Calling disablePictureAdjustmentConfig for display " + this.displayId);
                int retVal = this.service.setPAParameters(this.displayId, PA_GLOBAL_DISABLE, 0, 0,
                        0, 0, 0);
                if(retVal != RET_SUCCESS){
                    Log.e(TAG, "disablePictureAdjustmentConfig failed with return value "+retVal);
                    return RET_FAILURE;
                }
                else{
                    return retVal;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service disablePictureAdjustmentConfig failed");
                return RET_FAILURE;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return RET_FAILURE;
        }
    }

    /**
     * This method returns the current Picture Adjustment values
     * @return PAConfig instance enclosing all the current Picture Adjustment values
     *         <br>null for failures
     */
    /*NEW*/
    public PictureAdjustmentConfig getPictureAdjustmentParams(){
        if (myInstance[displayId] != null) {
            try {
                boolean isSupported = isFeatureSupported(DCM_FEATURE.FEATURE_GLOBAL_PICTURE_ADJUSTMENT);
                if (isSupported == false) {
                    return null;
                }
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
                return null;
            }
            try {
                Log.v(TAG, "Calling getPictureAdjustmentParams for display " + this.displayId);
                int[] retVal2 = this.service.getRangePAParameter(this.displayId);
                if(retVal2 == null || retVal2.length < 10){
                    Log.e(TAG, "getRanagePAParameter failed");
                    return null;
                }
                int[] retVal = this.service.getPAParameters(this.displayId);
                if(retVal == null || retVal.length < 6){
                    Log.e(TAG, "getPictureAdjustmentParams failed");
                    return null;
                }
                else{
                    boolean deSatEnabled = false;
                    boolean paDisabled = true;
                    EnumSet<PICTURE_ADJUSTMENT_PARAMS> params = EnumSet.allOf(PICTURE_ADJUSTMENT_PARAMS.class);
                    /*if((retVal[0] & PA_GLOBAL_HUE) == 0){
                       params.remove(PICTURE_ADJUSTMENT_PARAMS.HUE);
                    }
                    if((retVal[0] & PA_GLOBAL_SAT) == 0){
                        params.remove(PICTURE_ADJUSTMENT_PARAMS.SATURATION);
                    }
                    if((retVal[0] & PA_GLOBAL_VAL) == 0){
                        params.remove(PICTURE_ADJUSTMENT_PARAMS.INTENSITY);
                    }
                    if((retVal[0] & PA_GLOBAL_CON) == 0){
                        params.remove(PICTURE_ADJUSTMENT_PARAMS.CONTRAST);
                    }
                    if((retVal[0] & PA_GLOBAL_SAT_THRESH) == 0){
                        params.remove(PICTURE_ADJUSTMENT_PARAMS.SATURATION_THRESHOLD);
                    }*///while getting, all values are retrieved, so commented
                    if((retVal[0] & PA_GLOBAL_DESAT) != 0){
                        deSatEnabled = true;
                    }
                    else{
                        deSatEnabled = false;
                    }
                    if((retVal[0] & PA_GLOBAL_DISABLE) != 0){
                        paDisabled = true;
                    }
                    else{
                        paDisabled = false;
                    }
                    PictureAdjustmentConfig paConfig = new PictureAdjustmentConfig(params,
                            retVal[1], retVal[2], retVal[3], retVal[4], retVal[5]);
                    paConfig.isDesaturation = deSatEnabled;
                    paConfig.isGlobalPADisabled = paDisabled;
                    return paConfig;
                }

            } catch (RemoteException e) {
                Log.e(TAG, "Service getPictureAdjustmentParams failed");
                return null;
            }
        }
        else{
            Log.e(TAG, "Instance for the display type " + displayId + " doesnt exist");
            return null;
        }
    }
    // end pa


    /*
     * This will reset all values to system values, release self instance and
     * unregister the callbacks If user calls this method frequently, might
     * result in flickering since system values are set.
     */
    /**
     * This method will release the ColorManager object instance
     */
    public void release() {
        if (myInstance[this.displayId] != null) {
            myInstance[this.displayId] = null;
            myApplication = null;
        }
    }


    /*
     * Check mode data validity
     */
    private int checkModeValidity(ModeInfo pf) {
        if (pf == null) {
            Log.e(TAG, "Mode data missing");
            return RET_FAILURE;
        }
        if (pf.getId() < 0) {
            Log.e(TAG, "ID out of range");
            return RET_VALUE_OUT_OF_RANGE;
        }
        return RET_SUCCESS;
    }


    protected static class Log {

        // static final boolean VERBOSE_ENABLED = false; // set this to false to
        // stop
        // logging

        protected static void i(String tag, String text) {
            if (VERBOSE_ENABLED) {
                android.util.Log.i(tag, text);
            }
        }


        protected static void d(String tag, String text) {
            if (VERBOSE_ENABLED) {
                android.util.Log.d(tag, text);
            }
        }


        protected static void e(String tag, String text) {
            android.util.Log.e(tag, text);
        }


        protected static void v(String tag, String text) {
            if (VERBOSE_ENABLED) {
                android.util.Log.v(tag, text);
            }
        }
    }

    private static boolean isServiceRunning() {
        boolean serviceFound = false;
        InputStream in = null;
        ProcessBuilder cmd;
        try{
             String[] args = {"/system/bin/sh", "-c", "ps"};
             cmd = new ProcessBuilder(args);

             Process process = cmd.start();
             in = process.getInputStream();
             byte[] re = new byte[1024];
             while(in.read(re) != -1){
                  if(new String(re).contains("colorservice")){
                        serviceFound = true;
                        break;
                  }
             }
        } catch(IOException ex){
             ex.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch(IOException ex){
                    android.util.Log.d(TAG, "Harmless exception on close!");
                    ex.printStackTrace();
                }
            }
        }
        return serviceFound;

  }
}
