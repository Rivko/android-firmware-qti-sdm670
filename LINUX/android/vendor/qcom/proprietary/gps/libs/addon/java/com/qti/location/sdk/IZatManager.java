/** @file IZatManager.java
*/

/* ======================================================================
*  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.util.Log;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.os.Bundle;
import android.content.pm.ResolveInfo;
import android.location.Location;
import android.app.PendingIntent;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.qti.izat.IIzatService;
import com.qti.flp.IFlpService;
import com.qti.flp.ILocationCallback;
import com.qti.flp.ISessionStatusCallback;
import com.qti.flp.ITestService;
import com.qti.flp.IMaxPowerAllocatedCallback;
import com.qti.geofence.IGeofenceService;
import com.qti.geofence.IGeofenceCallback;
import com.qti.geofence.GeofenceData;
import com.qti.debugreport.IDebugReportService;
import com.qti.debugreport.IDebugReportCallback;

import com.qti.location.sdk.IZatWiFiDBReceiver.IZatAPInfo;
import com.qti.location.sdk.IZatWiFiDBReceiver.IZatAPInfoExtra;
import com.qti.location.sdk.IZatWiFiDBReceiver.IZatAPLocationData;
import com.qti.location.sdk.IZatWiFiDBReceiver.IZatAPSSIDInfo;
import com.qti.location.sdk.IZatWiFiDBReceiver.IZatAPSpecialInfo;
import com.qti.location.sdk.IZatWiFiDBReceiver.IZatCellInfo;
import com.qti.location.sdk.IZatWiFiDBReceiver.IZatWiFiDBReceiverResponseListener;

import com.qti.wifidbreceiver.APInfo;
import com.qti.wifidbreceiver.APLocationData;
import com.qti.wifidbreceiver.APSpecialInfo;
import com.qti.wifidbreceiver.IWiFiDBReceiver;
import com.qti.wifidbreceiver.IWiFiDBReceiverResponseListener;

import com.qti.location.sdk.IZatWWANDBReceiver.IZatBSInfo;
import com.qti.location.sdk.IZatWWANDBReceiver.IZatBSLocationData;
import com.qti.location.sdk.IZatWWANDBReceiver.IZatBSSpecialInfo;
import com.qti.location.sdk.IZatWWANDBReceiver.IZatBSCellInfo;
import com.qti.location.sdk.IZatWWANDBReceiver.IZatWWANDBReceiverResponseListener;
import com.qti.location.sdk.IZatWWANDBReceiver.IZatBSCellInfo.IZatBSCellTypes;

import com.qti.wwandbreceiver.BSInfo;
import com.qti.wwandbreceiver.BSLocationData;
import com.qti.wwandbreceiver.BSSpecialInfo;
import com.qti.wwandbreceiver.IWWANDBReceiver;
import com.qti.wwandbreceiver.IWWANDBReceiverResponseListener;

/** @addtogroup IZatManager
    @{ */

/**
 * The IZatManager class is the entry point to Qualcomm Location services.
 * It requires the permission of android.permission.ACCESS_FINE_LOCATION.
 *
 * <pre>
 * <code>
 *    // Sample Code
 *
 *    import android.app.Activity;
 *    import android.os.Bundle;
 *    import android.view.View;
 *    import android.widget.Button;
 *    import com.qti.location.sdk.IZatManager;
 *    import com.qti.location.sdk.IZatFlpService;
 *
 *    public class FullscreenActivity extends Activity {
 *
 *       private IZatManager mIzatMgr = null;
 *       private IZatFlpService mFlpSevice = null;
 *
 *       &#64Override
 *       protected void onCreate(Bundle savedInstanceState) {
 *
 *            ...
 *            // get the instance of IZatManager
 *            mIzatMgr = IZatManager.getInstance(getApplicationContext());
 *
 *            ...
 *            final Button connectButton = (Button)findViewById(R.id.connectButton);
 *            connectButton.setOnClickListener(new View.OnClickListener() {
 *                &#64Override
 *                public void onClick(View v) {
 *
 *                    // connect to IZatFlpService through IZatManager
 *                    if (mIzatMgr != null) {
 *                        <b>mFlpSevice = mIzatMgr.connectFlpService();</b>
 *                    }
 *                }
 *            });
 *            ...
 *        }
 *    }
 * </code>
 * </pre>
 */
public class IZatManager {
    private static String TAG = "IZatManager";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private static final String SDK_Version = "6.0.0";

    private static final int UNINITIALIZED_MASK = -1;
    private int mFlpFeaturMasks = UNINITIALIZED_MASK;
    private final int FEATURE_BIT_TIME_BASED_BATCHING_IS_SUPPORTED = 0x1;
    private final int FEATURE_BIT_DISTANCE_BASED_TRACKING_IS_SUPPORTED = 0x2;
    private final int FEATURE_BIT_ADAPTIVE_BATCHING_IS_SUPPORTED = 0x4;
    private final int FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED = 0x8;
    private final int FEATURE_BIT_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED = 0x10;

    private Map<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem> mFlpRequestsMap
        = createIdMap();
    private Map<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem> createIdMap() {
        Map<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem> result =
            new HashMap<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatFlpService.IFlpLocationCallback,
                LocationCallbackWrapper> mFlpPassiveCbMap = createPassiveCbMap();
    private Map<IZatFlpService.IFlpLocationCallback,
                LocationCallbackWrapper> createPassiveCbMap() {
        Map<IZatFlpService.IFlpLocationCallback, LocationCallbackWrapper> result =
            new HashMap<IZatFlpService.IFlpLocationCallback, LocationCallbackWrapper>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatTestService.IFlpMaxPowerAllocatedCallback,
                MaxPowerAllocatedCallbackWrapper> mFlpMaxPowerCbMap = createMaxPowerCbMap();
    private Map<IZatTestService.IFlpMaxPowerAllocatedCallback,
                MaxPowerAllocatedCallbackWrapper> createMaxPowerCbMap() {
        Map<IZatTestService.IFlpMaxPowerAllocatedCallback,
            MaxPowerAllocatedCallbackWrapper> result =
            new HashMap<IZatTestService.IFlpMaxPowerAllocatedCallback,
                        MaxPowerAllocatedCallbackWrapper>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatGeofenceServiceImpl,
                IZatGeofenceService.IZatGeofenceCallback> mGeofenceClientCallbackMap
        = createGeofenceClientCallbackMap();
    private Map<IZatGeofenceServiceImpl,
                IZatGeofenceService.IZatGeofenceCallback> createGeofenceClientCallbackMap() {
        Map<IZatGeofenceServiceImpl, IZatGeofenceService.IZatGeofenceCallback> result =
            new HashMap<IZatGeofenceServiceImpl, IZatGeofenceService.IZatGeofenceCallback>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                GeofenceMapItem> mGeofencesMap = createGeofencesMap();
    private Map<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                GeofenceMapItem> createGeofencesMap() {
        Map<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
            GeofenceMapItem> result =
            new HashMap<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                        GeofenceMapItem>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatDebugReportingServiceImpl,
                IZatDebugReportingService.IZatDebugReportCallback> mDebugReportClientCallbackMap
        = createDebugReportClientCallbackMap();
    private Map<IZatDebugReportingServiceImpl,
                IZatDebugReportingService.IZatDebugReportCallback>
        createDebugReportClientCallbackMap() {
            Map<IZatDebugReportingServiceImpl,
                IZatDebugReportingService.IZatDebugReportCallback> result =
                new HashMap<IZatDebugReportingServiceImpl,
                IZatDebugReportingService.IZatDebugReportCallback>();
            return Collections.synchronizedMap(result);
    }
    private IZatWiFiDBReceiverImpl mWiFiDBRecImpl = null;
    private IZatWWANDBReceiverImpl mWWANDBRecImpl = null;

    private static final int FLP_PASSIVE_LISTENER_HW_ID = -1;
    private static final int FLP_RESULT_SUCCESS = 0;
    private static final int FLP_RESULT_FAILURE = -1;

    private static final int FLP_SEESION_BACKGROUND = 1;
    private static final int FLP_SEESION_FOREGROUND = 2;
    private static final int FLP_SEESION_PASSIVE = 4;

    private static final int GEOFENCE_DWELL_TYPE_INSIDE = 1;
    private static final int GEOFENCE_DWELL_TYPE_OUTSIDE = 2;

    private static volatile int sFlpRequestsCnt = 0;
    private static final Object sFlpServiceLock = new Object();
    private static final Object sTestServiceLock = new Object();
    private static final Object sGeofenceServiceLock = new Object();
    private static final Object sGeofencesMapLock = new Object();
    private static final Object sGeofenceClientCallbackMapLock = new Object();
    private static final Object sDebugReportServiceLock = new Object();
    private static final Object sDebugReportCallbackMapLock = new Object();
    private static final Object sWiFiDBReceiverLock = new Object();
    private static final Object sWWANDBReceiverLock = new Object();

    private static final String REMOTE_IZAT_SERVICE_NAME = "com.qualcomm.location.izat.IzatService";
    private Context mContext;
    private static IZatManager sInstance = null;
    private static IIzatService sIzatService = null;
    private GeofenceStatusCallbackWrapper mGeofenceCbWrapper = new GeofenceStatusCallbackWrapper();
    private DebugReportCallbackWrapper mDebugReportCbWrapper = new DebugReportCallbackWrapper();
    private WiFiDBReceiverRespListenerWrapper mWiFiDBReceiverRespListenerWrapper =
            new WiFiDBReceiverRespListenerWrapper();
    private WWANDBReceiverRespListenerWrapper mWWANDBReceiverRespListenerWrapper =
            new WWANDBReceiverRespListenerWrapper();

    /**
     * Returns an instance of IZatManager.
     *
     * @param context Context object.
     *
     * @throws IZatIllegalArgumentException The context is NULL.
     * @return
     * The instance of IZatManager.
     */
    public static synchronized IZatManager getInstance(Context context)
        throws IZatIllegalArgumentException {
        if (null == context) {
            throw new IZatIllegalArgumentException("null argument");
        }

        if (null == sInstance) {
            sInstance = new IZatManager(context);
        }

        return sInstance;
    }

    private IZatManager(Context context) {
        // To avoid collisions with other process, use thread id + 1 byte for counter
        sFlpRequestsCnt = android.os.Process.myTid() << 8;
        if (VERBOSE) {
            Log.v(TAG, "IZatManager ctor sFlpRequestsCnt=" + sFlpRequestsCnt);
        }
        mContext = context;
    }

    /**
     * Gets the Qualcomm Location SDK version and the IZatService version.
     * <p>
     * This API requires that IZatService is present, otherwise
     * an {@link IZatServiceUnavailableException} is thrown.
     *
     * This returns version information in format X.Y.Z:A.B.C,
     * where XYZ are major, minor, and revision IDs for the SDK
     * and ABC are major, minor, and revision IDs for the service.
     * This API may take as long as several seconds to execute.
     * It is recommended not to call this API in the main thread.
     * </p>
     * @throws IZatServiceUnavailableException  The IZatService is not present.
     * @return
     * The versions of the SDK and the service.
     */
    public String getVersion()
        throws IZatServiceUnavailableException {
        String service_version;

        if (null == sIzatService) {
            connectIzatService();
        }

        try {
            service_version = sIzatService.getVersion();
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IzatService version", e);
        }

        if (null == service_version) {
            service_version = "1.0.0";
        }

        String version = SDK_Version + ":" + service_version;

        return version;
    }

    private synchronized void connectIzatService() {
        if (null == sIzatService) {
            if (VERBOSE) {
                Log.d(TAG, "Connecting to Izat service by name [" +
                      REMOTE_IZAT_SERVICE_NAME + "]");
            }

            // check if IZat service installed
            PackageManager pm = mContext.getPackageManager();
            ResolveInfo ri = pm.resolveService(new Intent(REMOTE_IZAT_SERVICE_NAME), 0);
            if (null == ri) {
                Log.e(TAG, "Izat service (" + REMOTE_IZAT_SERVICE_NAME + ") not installed");
                throw new IZatServiceUnavailableException("Izat service unavailable.");
            }

            // check if IZat service started
            if (null == ServiceManager.getService(REMOTE_IZAT_SERVICE_NAME)) {
                Log.e(TAG, "Izat service (" + REMOTE_IZAT_SERVICE_NAME + ") is not started");
                throw new IZatServiceUnavailableException("Izat service not started.");
            }

            sIzatService =
                IIzatService.Stub.asInterface(ServiceManager.getService(REMOTE_IZAT_SERVICE_NAME));
            if(null == sIzatService) {
                Log.e(TAG, "Izat service (" + REMOTE_IZAT_SERVICE_NAME + ") not started");
                throw new IZatServiceUnavailableException("Izat service unavailable.");
            }
        }
    }


    /**
     * Gets an IZatFlpService interface.
     *
     * This API may take several seconds to execute.
     * Qualcomm Technologies, Inc. recommends not to call this API in the main thread.
     *
     * @throws IZatServiceUnavailableException The FLP service is not present.
     *
     * @return
     * An IZatFlpService interface.
     *
     * @dependencies
     * This API requires that the IZat FLP service is present, otherwise
     * an IZatServiceUnavailableException is thrown.
     */
    public IZatFlpService connectFlpService()
        throws IZatServiceUnavailableException {

        if (null == sIzatService) {
            connectIzatService();
        }

        try {
            IFlpService flpService = (IFlpService)(sIzatService.getFlpService());
            if (null == flpService) {
                throw new IZatServiceUnavailableException("FLP Service is not available.");
            }
            synchronized (sFlpServiceLock) {
                if (UNINITIALIZED_MASK == mFlpFeaturMasks) {
                    mFlpFeaturMasks = flpService.getAllSupportedFeatures();
                }
            }
            if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_TRACKING_IS_SUPPORTED)>0) {
                return new IZatFlpServiceImpl(flpService);
            } else {
                Log.e(TAG, "Izat FLP positioning is not supported on this device.");
                return null;
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IFlpService", e);
        }
    }

    /**
     * Disconnects form the specified IZatFlpService interface.
     *
     * @param service Handle object that is returned from
     * the IZatManager#connectFlpService().
     * @throws IZatIllegalArgumentException The input service is NULL.
     * @return
     * None.
     */
    public void disconnectFlpService(IZatFlpService service)
        throws IZatIllegalArgumentException {
        if (null == service || !(service instanceof IZatFlpServiceImpl))
            throw new IZatIllegalArgumentException();

        // stop all the undergoing sessions before disconnecting
        try {
            synchronized (sFlpServiceLock) {
                IFlpService flpService =
                   (IFlpService)(sIzatService.getFlpService());
                if (null == flpService) {
                    throw new IZatServiceUnavailableException("FLP Service is not available.");
                }
                FlpRequestMapItem mapItem = null;
                for (IZatFlpServiceImpl.IZatSessionHandlerImpl key : mFlpRequestsMap.keySet()) {
                    mapItem = mFlpRequestsMap.get(key);
                    // stop session
                    if (flpService.stopFlpSession(mapItem.getHwId()) !=
                            FLP_RESULT_SUCCESS) {
                        Log.e(TAG, "stopFlpSession failed in disconnecting");
                        return;
                    }
                    if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                        // un-register cb
                        if (NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL ==
                                mapItem.getNotifyType()) {
                            flpService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                          mapItem.getCbWrapper());
                        }
                        if (NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX ==
                                mapItem.getNotifyType()) {
                            flpService.unregisterCallback(FLP_SEESION_FOREGROUND,
                                                          mapItem.getCbWrapper());
                        }
                    } else {
                        flpService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                      mapItem.getCbWrapper());
                    }

                    // unregister if any session status handlers are registered
                    if (mapItem.getStatusCallback() != null) {
                        flpService.unregisterForSessionStatus(mapItem.getStatusCallback());
                    }
                }
                mFlpRequestsMap.clear();

                // remove all passive listeners.
                LocationCallbackWrapper cbWrapper = null;
                for (IZatFlpService.IFlpLocationCallback key : mFlpPassiveCbMap.keySet()) {
                    cbWrapper = mFlpPassiveCbMap.get(key);
                    flpService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                  cbWrapper);
                }
                mFlpPassiveCbMap.clear();
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed stop all flp sessions", e);
        }
    }

    /**
     * Gets an {@link IZatTestService} interface.
     * <p>
     * This API requires that the IZatTest service is present, otherwise
     * an {@link IZatServiceUnavailableException} is thrown.<br/>
     * This API may take as long as several seconds to execute.
     * It is recommended not to call this API in main thread.
     * Only a system application is allowed to call this API.
     * </p>
     *
     * @throws IZatServiceUnavailableException The Test service is not present.
     * @throws IZatSecurityException The callling application is not a system app.
     * @return An {@link IZatTestService} interface.
     */
    public IZatTestService connectTestService()
        throws IZatServiceUnavailableException, IZatSecurityException {

        if (!mContext.getApplicationInfo().isSystemApp()) {
            throw new IZatSecurityException("Test Service is only available to system app.");
        }

        if (null == sIzatService) {
            connectIzatService();
        }

        try {
            ITestService testService = (ITestService)(sIzatService.getTestService());
            if (null == testService) {
                throw new IZatServiceUnavailableException("Test Service is not available.");
            }
            return new IZatTestServiceImpl(testService);
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get ITestService", e);
        }
    }

    /**
     * Disconnects from the specified {@link IZatTestService} interface.
     *
     * @param service Handle object that is returned from
     * {@link IZatManager#connectTestService()}.
     * @throws IZatIllegalArgumentException The input service is NULL.
     */
    public void disconnectTestService(IZatTestService service)
        throws IZatIllegalArgumentException {
        if (null == service || !(service instanceof IZatTestServiceImpl))
            throw new IZatIllegalArgumentException();

        synchronized (sTestServiceLock) {
            for (IZatTestService.IFlpMaxPowerAllocatedCallback key :
                     mFlpMaxPowerCbMap.keySet()) {
                service.deregisterForMaxPowerAllocatedChange(key);
            }
            mFlpMaxPowerCbMap.clear();
        }
    }

    /**
     * Gets an {@link IZatGeofenceService} interface.
     * <p>
     * This API requires that the Geofence service is present, otherwise
     * an {@link IZatServiceUnavailableException} is thrown.<br/>
     * This API may take as long as several seconds to execute.
     * It is recommended not to call this API in the main thread.
     * </p>
     *
     * @throws IZatServiceUnavailableException The Geofence service is not present.
     * @return An {@link IZatGeofenceService} interface.
     */
    public IZatGeofenceService connectGeofenceService()
        throws IZatServiceUnavailableException {
        if (null == sIzatService) {
            connectIzatService();
        }

        try {
            IGeofenceService geofenceService =
                (IGeofenceService)(sIzatService.getGeofenceService());
            if (null == geofenceService) {
                throw new IZatServiceUnavailableException("Geofence Service is not available.");
            }
            // register the geofence callback
            synchronized (sGeofenceServiceLock) {
                geofenceService.registerCallback(mGeofenceCbWrapper);
            }
            return new IZatGeofenceServiceImpl(geofenceService);
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IGeofenceService", e);
        }
    }

   /**
     * Disconnects from the specified {@link IZatGeofenceService} interface.
     *
     * @param service Handle object that is returned from
     * {@link IZatManager#connectGeofenceService()}.
     * @throws IZatIllegalArgumentException The input service is NULL.
     */
    public void disconnectGeofenceService(IZatGeofenceService service)
        throws IZatIllegalArgumentException {
        if (null == service || !(service instanceof IZatGeofenceServiceImpl))
            throw new IZatIllegalArgumentException();

        try {
            synchronized (sGeofenceServiceLock) {
                IGeofenceService geofenceService =
                   (IGeofenceService)(sIzatService.getGeofenceService());
                if (null == geofenceService) {
                    throw new IZatServiceUnavailableException("Geofence Service is not available.");
                }
                // remove all geofence added.
                GeofenceMapItem mapItem = null;
                for (IZatGeofenceServiceImpl.IZatGeofenceHandleImpl key :
                         mGeofencesMap.keySet()) {
                    mapItem = mGeofencesMap.get(key);
                    geofenceService.removeGeofence(mapItem.getHWGeofenceId());
                }
                // un-register the geofence callback
                geofenceService.unregisterCallback(mGeofenceCbWrapper);
            }
            synchronized (sGeofencesMapLock) {
                mGeofencesMap.clear();
            }
            // delete the client callback
            synchronized (sGeofenceClientCallbackMapLock) {
                for (IZatGeofenceServiceImpl key : mGeofenceClientCallbackMap.keySet()) {
                    if (service == key) {
                        mGeofenceClientCallbackMap.remove(key);
                        break;
                    }
                }
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to remove all geofence added", e);
        }
    }

   /**
    * Gets an {@link IZatDebugReportingService} interface.
    * <p>
    * This API requires that the IZat debug reporting service is present, otherwise
    * an {@link IZatServiceUnavailableException} is thrown.<br/>
    * This API may take as long as several seconds to execute.
    * It is recommended not to call this API in the main thread.
    * </p>
    *
    * @throws IZatServiceUnavailableException The Debug Reporting Service is not present.
    * @return An {@link IZatDebugReportingService} interface.
    */
   public IZatDebugReportingService connectDebugReportingService()
        throws IZatServiceUnavailableException {
        if (null == sIzatService) {
            connectIzatService();
        }

        try {
            IDebugReportService debugReportService =
                (IDebugReportService)(sIzatService.getDebugReportService());
            if (null == debugReportService) {
                throw new IZatServiceUnavailableException("Debug Reporting Service is not available.");
            }
            synchronized (sDebugReportServiceLock) {
                debugReportService.registerForDebugReporting(mDebugReportCbWrapper);
            }
            return new IZatDebugReportingServiceImpl(debugReportService);
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IDebugReportService", e);
        }
   }

   /**
     * Disconnects from the specified {@link IZatDebugReportingService} interface.
     *
     * @param service Handle object that is returned from
     * {@link IZatManager#connectDebugReportingService()}.
     * @throws IZatIllegalArgumentException The input service is NULL.
     */
   public void disconnectDebugReportingService(IZatDebugReportingService service) {
       if (null == service || !(service instanceof IZatDebugReportingServiceImpl))
            throw new IZatIllegalArgumentException();

       try {
           IDebugReportService debugReportService =
               (IDebugReportService)(sIzatService.getDebugReportService());
            if (null == debugReportService) {
                throw new IZatServiceUnavailableException("Debug Report Service is not available.");
            }
            synchronized (sDebugReportServiceLock) {
               debugReportService.unregisterForDebugReporting(mDebugReportCbWrapper);
            }
            synchronized (sDebugReportCallbackMapLock) {
                for(IZatDebugReportingServiceImpl key :
                        mDebugReportClientCallbackMap.keySet()) {
                    if (service == key) {
                        mDebugReportClientCallbackMap.remove(key);
                        break;
                    }
                }
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to disconnect DebugReportService", e);
        }
   }

    /**
     * Gets an {@link IZatWiFiDBReceiver} interface.
     * <p>
     * This API requires that the Wi-Fi DB receiver is present, otherwise
     * an {@link IZatServiceUnavailableException} is thrown.<br/>
     * This API may take as long as several seconds to execute.
     * It is recommended not to call this API in the main thread.
     * </p>
     *
     * @param listener Handle object that is returned from the {@link IZatWiFiDBReceiver} service.
     * @throws IZatServiceUnavailableException The Wi-Fi DB
     *                                         receiver is not
     *                                         present.
     * @return An {@link IZatWiFiDBReceiver} interface.
     */
    public IZatWiFiDBReceiver connectToWiFiDBReceiver(
        IZatWiFiDBReceiverResponseListener listener)
        throws IZatServiceUnavailableException{

        if (null == sIzatService) {
            connectIzatService();
        }

        try {
            if(null == mWiFiDBRecImpl) {
                IWiFiDBReceiver wifiDBReceiver =
                    (IWiFiDBReceiver)(sIzatService.getWiFiDBReceiver());
                // register the callbacks
                synchronized (sWiFiDBReceiverLock) {
                    wifiDBReceiver.registerResponseListener(mWiFiDBReceiverRespListenerWrapper);
                }
                mWiFiDBRecImpl = new IZatWiFiDBReceiverImpl(wifiDBReceiver, listener);
            }
            return mWiFiDBRecImpl;

        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IWiFiDBReceiver", e);
        }
    }

    /**
     * Disconnects the specified {@link IZatWiFiDBReceiver}
     * interface.
     *
     * @param receiver Handle object that is returned from
     * {@link IZatManager#connectToWiFiDBReceiver()}.
     * @throws IZatIllegalArgumentException The input receiver is NULL.
     *
     */
    public void disconnectFromWiFiDBReceiver(IZatWiFiDBReceiver receiver)
        throws IZatIllegalArgumentException {
        if (null == receiver || !(receiver instanceof IZatWiFiDBReceiverImpl))
            throw new IZatIllegalArgumentException();
        try {
            IWiFiDBReceiver wifiDBReceiver =
                    (IWiFiDBReceiver)(sIzatService.getWiFiDBReceiver());
            synchronized (sWiFiDBReceiverLock) {
                wifiDBReceiver.removeResponseListener(mWiFiDBReceiverRespListenerWrapper);
            }
            mWiFiDBRecImpl = null;
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to disconnect WiFiDBReceiver", e);
        }
    }

    /**
     * Gets an {@link IZatWWANDBReceiver} interface.
     * <p>
     * This API requires that WWAN DB Receiver is present, otherwise
     * an {@link IZatServiceUnavailableException}
     * is thrown.<br/>
     *
     * This API may take as long as several seconds to execute.
     * It is recommended not to call this API in the main thread.
     * </p>
     *
     * @param listener Handle object that is returned from
     *                     the {@link IZatWWANDBReceiver} service.
     *
     * @throws IZatServiceUnavailableException The WWAN DB
     *                                         Receiver is not
     *                                         present.
     * @return An {@link IZatWWANDBReceiver} interface.
     */
    public IZatWWANDBReceiver connectToWWANDBReceiver(
        IZatWWANDBReceiverResponseListener listener)
        throws IZatServiceUnavailableException{

        if (sIzatService == null) {
            connectIzatService();
        }

        try {
            if(null == mWWANDBRecImpl) {
                IWWANDBReceiver wwanDBReceiver =
                        (IWWANDBReceiver)(sIzatService.getWWANDBReceiver());
                // register the callbacks
                synchronized (sWWANDBReceiverLock) {
                    wwanDBReceiver.registerResponseListener(mWWANDBReceiverRespListenerWrapper);
                }
                mWWANDBRecImpl = new IZatWWANDBReceiverImpl(wwanDBReceiver, listener);
            }
            return mWWANDBRecImpl;

        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IWWANDBReceiver", e);
        }
    }

    /**
     * Disconnects the specified {@link IZatWWANDBReceiver}
     * interface.
     *
     * @param receiver Handle object that is returned from
     * {@link IZatManager#connectToWWANDBReceiver()}.
     *
     * @throws IZatIllegalArgumentException The input receiver is NULL.
     */
    public void disconnectFromWWANDBReceiver(IZatWWANDBReceiver receiver)
            throws IZatIllegalArgumentException {
            if (receiver == null || !(receiver instanceof IZatWWANDBReceiverImpl)){
                    throw new IZatIllegalArgumentException();
            }
            try {
                IWWANDBReceiver wwanDBReceiver =
                        (IWWANDBReceiver)(sIzatService.getWWANDBReceiver());
                synchronized (sWWANDBReceiverLock) {
                    wwanDBReceiver.removeResponseListener(mWWANDBReceiverRespListenerWrapper);
                }
                mWWANDBRecImpl = null;
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to disconnect WWANDBReceiver", e);
            }
    }

/** @cond */

    /*
     * Implementation of interface IZatFlpService. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class IZatFlpServiceImpl implements IZatFlpService {

        IFlpService mService;
        public IZatFlpServiceImpl(IFlpService service) {
            mService = service;
        }

        @Override
        public IZatFlpSessionHandle startFlpSession(IFlpLocationCallback callback,
                                                    IzatFlpRequest flpRequest)
                                                    throws IZatIllegalArgumentException {
            if (null == callback || null == flpRequest) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            if ((flpRequest.getTimeInterval() <= 0) &&
                (flpRequest.getDistanceInterval() <= 0) &&
                (flpRequest.getTripDistance() <= 0)) {
                throw new IZatIllegalArgumentException("Atleast one of the parameters " +
                        "in time, distance interval and trip distance must be valid");
            }
            try {
                synchronized (sFlpServiceLock) {
                    // first to check if this request is already started.
                    NotificationType notifType =
                            NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX;
                    if ((flpRequest.mIsRunningInBackground) &&
                        (flpRequest instanceof IzatFlpBgRequest)) {

                        // get the active mode
                        IzatFlpBgRequest.IzatFlpBgRequestMode activeMode =
                                ((IzatFlpBgRequest)flpRequest).getActiveMode();

                        notifType = NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL;
                        if (IzatFlpBgRequest.IzatFlpBgRequestMode.TRIP_MODE == activeMode) {
                            if (0 == (mFlpFeaturMasks &
                                      FEATURE_BIT_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED)) {
                                throw new IZatFeatureNotSupportedException (
                                        "Outdoor trip mode is not supported.");
                            } else if (flpRequest.getTripDistance() > 0) {
                                notifType = NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED;
                            }
                        }
                    }

                    for (FlpRequestMapItem mapItem: mFlpRequestsMap.values()) {
                        if ((mapItem.getCallback() == callback) &&
                            (mapItem.getNotifyType() == notifType) &&
                            (mapItem.getTimeInterval() == flpRequest.getTimeInterval()) &&
                            (mapItem.getDistanceInterval() == flpRequest.getDistanceInterval()) &&
                                    (mapItem.getTripDistance() == flpRequest.getTripDistance())){
                            throw new IZatIllegalArgumentException("this session" +
                                                                   " started already.");
                        }
                    }
                    int hwId = sFlpRequestsCnt++;
                    long sessionStartTime = System.currentTimeMillis();
                    // register the cb
                    LocationCallbackWrapper cbWrapper = new LocationCallbackWrapper(callback);

                    if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                        /* If the modem support LB2.0, then the batched fixes and live fix will be
                          returned independently to clients based on the request issued by
                          each client. */
                        if (flpRequest.mIsRunningInBackground) {
                            mService.registerCallback(FLP_SEESION_BACKGROUND,
                                                      hwId,
                                                      cbWrapper,
                                                      sessionStartTime);
                        } else {
                            mService.registerCallback(FLP_SEESION_FOREGROUND,
                                                      hwId,
                                                      cbWrapper,
                                                      sessionStartTime);
                        }
                    } else {
                            /* If the modem does not support LB2.0, then the batching client
                               will also receive the live fix, which is the legacy behaviour.*/
                            mService.registerCallback(FLP_SEESION_PASSIVE,
                                                      hwId,
                                                      cbWrapper,
                                                      sessionStartTime);
                    }
                    // start flp session
                    int result = mService.startFlpSession(hwId,
                                    notifType.getCode(),
                                    flpRequest.getTimeInterval(),
                                    flpRequest.getDistanceInterval(),
                                    flpRequest.getTripDistance());
                    if (VERBOSE) {
                        Log.v(TAG, "startFlpSession() returning : " + result);
                    }
                    if (FLP_RESULT_SUCCESS == result) {
                        IZatSessionHandlerImpl handle = new IZatSessionHandlerImpl();
                        mFlpRequestsMap.put(handle,
                                new FlpRequestMapItem(callback,
                                                      notifType,
                                                      flpRequest.getTimeInterval(),
                                                      flpRequest.getDistanceInterval(),
                                                      flpRequest.getTripDistance(),
                                                      cbWrapper,
                                                      hwId,
                                                      sessionStartTime));
                        return handle;
                    } else {
                        if (0 < (mFlpFeaturMasks &
                                 FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)) {
                            if (flpRequest.mIsRunningInBackground) {
                                mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                            cbWrapper);
                            } else {
                                mService.unregisterCallback(FLP_SEESION_FOREGROUND,
                                                            cbWrapper);
                            }
                        } else {
                                mService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                            cbWrapper);
                        }
                        sFlpRequestsCnt--;
                        return null;
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed startFlpSession", e);
            }
        }

        @Override
        public void stopFlpSession(IZatFlpSessionHandle handler)
            throws IZatIllegalArgumentException {

            if (null == handler || !(handler instanceof IZatFlpServiceImpl.IZatSessionHandlerImpl)) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sFlpServiceLock) {
                    // first to check if this request is already started.
                    FlpRequestMapItem mapItem = mFlpRequestsMap.get(handler);
                    if (null == mapItem) {
                        Log.e(TAG, "this request ID is unknown.");
                        return;
                    }
                    if (mService.stopFlpSession(mapItem.getHwId()) != FLP_RESULT_SUCCESS){
                        Log.e(TAG, "stopFlpSession() failed. ");
                        return;
                    }
                    if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                        if ((NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL ==
                                mapItem.getNotifyType()) ||
                            (NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED ==
                                    mapItem.getNotifyType())) {
                            mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                        mapItem.getCbWrapper());
                        } else if (NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX == mapItem.getNotifyType()) {
                            mService.unregisterCallback(FLP_SEESION_FOREGROUND,
                                                        mapItem.getCbWrapper());
                        }
                    } else {
                        mService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                    mapItem.getCbWrapper());
                    }
                    mFlpRequestsMap.remove(handler);
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed stopFlpSession", e);
            }
        }

        @Override
        public void registerForPassiveLocations(IZatFlpService.IFlpLocationCallback callback)
            throws IZatIllegalArgumentException {
            if (null == callback) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sFlpServiceLock) {
                    if (null == mFlpPassiveCbMap.get(callback)) {
                        LocationCallbackWrapper cbWrapper = new LocationCallbackWrapper(callback);
                        mFlpPassiveCbMap.put(callback, cbWrapper);
                        mService.registerCallback(FLP_SEESION_PASSIVE,
                                                  FLP_PASSIVE_LISTENER_HW_ID,
                                                  cbWrapper,
                                                  System.currentTimeMillis());
                    } else {
                        Log.w(TAG, "this passive callback is already registered.");
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed registerForPassiveLocations", e);
            }
        }

        @Override
        public void deregisterForPassiveLocations(IZatFlpService.IFlpLocationCallback callback)
            throws IZatIllegalArgumentException {
            if (null == callback) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sFlpServiceLock) {
                    LocationCallbackWrapper cbWrapper = mFlpPassiveCbMap.get(callback);
                    if (null == cbWrapper) {
                        Log.w(TAG, "this passive callback is not registered.");
                    } else {
                        mService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                    cbWrapper);
                        mFlpPassiveCbMap.remove(callback);
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed deregisterForPassiveLocations", e);
            }
        }

        private class IZatSessionHandlerImpl implements IZatFlpService.IZatFlpSessionHandle {
            @Override
            public void pullLocations() {

                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (null == mapItem) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (null == mapItem.getCbWrapper()) {
                            Log.w(TAG, "no available callback");
                            return;
                        }
                        int result = mService.pullLocations(mapItem.getCbWrapper(),
                                                            mapItem.getSessionStartTime(),
                                                            mapItem.getHwId());
                        if (FLP_RESULT_SUCCESS == result) {
                            mapItem.setSessionStartTime(System.currentTimeMillis());
                        }
                        if (VERBOSE) {
                            Log.v(TAG, "pullLocations() returning : " + result);
                        }
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed pullLocations", e);
                }
            }

            @Override
            public void setToForeground() {
                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (null == mapItem) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (null == mapItem.getCbWrapper()) {
                            Log.w(TAG, "no available callback");
                            return;
                        }
                        int result = FLP_RESULT_SUCCESS;
                        if (NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX !=
                                mapItem.getNotifyType()) {
                            result = mService.updateFlpSession(mapItem.getHwId(),
                                NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX.getCode(),
                                mapItem.getTimeInterval(),
                                mapItem.getDistanceInterval(), 0);
                            if (FLP_RESULT_SUCCESS == result) {
                                // update the item flag in map
                                mapItem.updateNotifyType(
                                    NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX);
                                mFlpRequestsMap.put(this, mapItem);
                                // move the callback form bg to fg
                                if ((mFlpFeaturMasks &
                                    FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                                    mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                                mapItem.getCbWrapper());
                                    mService.registerCallback(FLP_SEESION_FOREGROUND,
                                                              mapItem.getHwId(),
                                                              mapItem.getCbWrapper(),
                                                              mapItem.getSessionStartTime());
                                }
                            } else {
                                Log.v(TAG, "mService.updateFlpSession failed.");
                            }
                        }
                        if (VERBOSE)  {
                            Log.v(TAG, "setToForeground() returning : " + result);
                        }
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed setToForeground", e);
                }
            }

            @Override
            public void setToBackground() {
                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (null == mapItem) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (null == mapItem.getCbWrapper()) {
                            Log.w(TAG, "no available callback");
                            return;
                        }
                        int result = FLP_RESULT_SUCCESS;
                        if (NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL !=
                                mapItem.getNotifyType()) {
                            result = mService.updateFlpSession(mapItem.getHwId(),
                                NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL.getCode(),
                                mapItem.getTimeInterval(),
                                mapItem.getDistanceInterval(), 0);
                            if (FLP_RESULT_SUCCESS == result) {
                                // update the item flag in map
                                mapItem.updateNotifyType(
                                    NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL);
                                mFlpRequestsMap.put(this, mapItem);
                                if ((mFlpFeaturMasks &
                                    FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                                    // move the callback form fg to bg
                                    mService.unregisterCallback(FLP_SEESION_FOREGROUND,
                                                                mapItem.getCbWrapper());
                                    mService.registerCallback(FLP_SEESION_BACKGROUND,
                                                              mapItem.getHwId(),
                                                              mapItem.getCbWrapper(),
                                                              mapItem.getSessionStartTime());
                                }
                            } else {
                                Log.v(TAG, "mService.updateFlpSession failed.");
                            }
                        }
                        if (VERBOSE) {
                            Log.v(TAG, "setToBackground() returning : " + result);
                        }
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed setToBackground", e);
                }
            }

            @Override
            public void setToTripMode() throws
                    IZatFeatureNotSupportedException {
                if (0 == (mFlpFeaturMasks & FEATURE_BIT_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED)) {
                    throw new IZatFeatureNotSupportedException (
                            "Outdoor trip mode is not supported.");
                }

                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (null == mapItem) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (null == mapItem.getCbWrapper()) {
                            Log.w(TAG, "no available callback");
                            return;
                        }

                        if (0 == (mFlpFeaturMasks &
                                  FEATURE_BIT_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED)) {
                            Log.w(TAG, "Outdoor Trip mode not supported");
                            return;
                        }

                        int result = FLP_RESULT_SUCCESS;
                        if (NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED !=
                                mapItem.getNotifyType()) {
                            result = mService.updateFlpSession(mapItem.getHwId(),
                                    NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED.getCode(),
                                    mapItem.getTimeInterval(),
                                    mapItem.getDistanceInterval(), mapItem.getTripDistance());
                            if (FLP_RESULT_SUCCESS == result) {
                                // update the item flag in map
                                mapItem.updateNotifyType(
                                        NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED);

                                // remember to switch back to previous state once
                                // trip is completed.
                                mapItem.setRestartOnTripCompleted(true);
                                mFlpRequestsMap.put(this, mapItem);
                                if ((mFlpFeaturMasks &
                                    FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                                    if (NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX ==
                                            mapItem.getPreviousNotifyType()) {
                                        // switching from foreground to trip mode
                                        mService.unregisterCallback(FLP_SEESION_FOREGROUND,
                                                                    mapItem.getCbWrapper());
                                        mService.registerCallback(FLP_SEESION_BACKGROUND,
                                                                  mapItem.getHwId(),
                                                                  mapItem.getCbWrapper(),
                                                                  mapItem.getSessionStartTime());
                                    }
                                }
                            } else {
                                Log.v(TAG, "mService.updateFlpSession failed.");
                            }
                        }
                        if (VERBOSE)  {
                            Log.v(TAG, "setToTripMode() returning : " + result);
                        }
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed setToTripMode", e);
                }
            }

            @Override
            public void registerForSessionStatus(IFlpStatusCallback callback)
                    throws IZatIllegalArgumentException, IZatFeatureNotSupportedException {
                if (0 == (mFlpFeaturMasks & FEATURE_BIT_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED)) {
                    throw new IZatFeatureNotSupportedException (
                            "Session status not supported.");
                }

                if (null == callback) {
                    throw new IZatIllegalArgumentException("invalid input parameter");
                }

                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (null == mapItem) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }

                        FlpStatusCallbackWrapper cbWrapper =
                            new FlpStatusCallbackWrapper(callback, mService);
                        mapItem.setStatusCallback(cbWrapper);
                        mService.registerForSessionStatus(mapItem.getHwId(), cbWrapper);

                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed registerForSessionStatus", e);
                }
            }

            @Override
            public void unregisterForSessionStatus()
                    throws  IZatIllegalArgumentException, IZatFeatureNotSupportedException {

                    if (0 == (mFlpFeaturMasks & FEATURE_BIT_OUTDOOR_TRIP_BATCHING_IS_SUPPORTED)) {
                    throw new IZatFeatureNotSupportedException (
                            "Session status not supported.");
                }

                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (null == mapItem) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }

                        FlpStatusCallbackWrapper cbWrapper = mapItem.getStatusCallback();
                        if (null == cbWrapper) {
                            Log.w(TAG, "no status callback wrapper is registered.");
                        } else {
                            mService.unregisterForSessionStatus(cbWrapper);
                            mapItem.setStatusCallback(null);
                        }
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed unregisterForSessionStatus", e);
                }
        }
    }
  }

    /**
     * Implementation of interface IZatTestService. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class IZatTestServiceImpl implements IZatTestService {

        ITestService mService;
        public IZatTestServiceImpl(ITestService service) {
            mService = service;
        }

        @Override
        public void deleteAidingData(long flags)
            throws IZatIllegalArgumentException {
            if (0 == flags) {
                throw new IZatIllegalArgumentException("invalid input parameter." +
                                                       " flags must be filled");
            }
            try {
                mService.deleteAidingData(flags);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed deregisterForPassiveLocations", e);
            }
        }

        @Override
        public void updateXtraThrottle(boolean enabled) {
            try {
                mService.updateXtraThrottle(enabled);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed updateXtraThrottle", e);
            }
        }

        @Override
        public void registerForMaxPowerAllocatedChange(
            IZatTestService.IFlpMaxPowerAllocatedCallback callback)
                throws IZatIllegalArgumentException {

            if (null == callback) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sTestServiceLock) {
                    if (null == mFlpMaxPowerCbMap.get(callback)) {
                        MaxPowerAllocatedCallbackWrapper cbWrapper =
                                    new MaxPowerAllocatedCallbackWrapper(callback);
                        mFlpMaxPowerCbMap.put(callback, cbWrapper);
                        mService.registerMaxPowerChangeCallback(cbWrapper);
                    } else {
                        Log.w(TAG, "this max power callback is already registered.");
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed registerForMaxPowerAllocatedChange", e);
            }
        }

        @Override
        public void deregisterForMaxPowerAllocatedChange(
            IZatTestService.IFlpMaxPowerAllocatedCallback callback)
                throws IZatIllegalArgumentException {

            if (null == callback) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sTestServiceLock) {
                    MaxPowerAllocatedCallbackWrapper cbWrapper = mFlpMaxPowerCbMap.get(callback);
                    if (null == cbWrapper) {
                        Log.w(TAG, "this passive callback is not registered.");
                    } else {
                        mService.unregisterMaxPowerChangeCallback(cbWrapper);
                        mFlpMaxPowerCbMap.remove(callback);
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed deregisterForMaxPowerAllocatedChange", e);
            }
        }
    }

    private class IZatWiFiDBReceiverImpl extends IZatWiFiDBReceiver {
        IWiFiDBReceiver mReceiver;
        public IZatWiFiDBReceiverImpl(IWiFiDBReceiver receiver,
                                      IZatWiFiDBReceiverResponseListener listener)
            throws IZatIllegalArgumentException {
            super(listener);
            if(null == receiver || null == listener) {
                throw new IZatIllegalArgumentException("IZatWiFiDBReceiverImpl:" +
                                                       " null receiver / listener");
            }
            mReceiver = receiver;
        }

        @Override
        public void requestAPList(int expire_in_days) {
            try {
                mReceiver.requestAPList(expire_in_days);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to request AP LIst", e);
            }
        }

        @Override
        public void pushWiFiDB(List<IZatAPLocationData> location_data,
                               List<IZatAPSpecialInfo> special_info,
                               int days_valid) {
            List<APLocationData> locationData = new ArrayList<APLocationData>();
            List<APSpecialInfo> specialInfo = new ArrayList<APSpecialInfo>();

            for(IZatAPLocationData loc_data: location_data) {
                if(null != loc_data) {
                    APLocationData locData = new APLocationData();
                    locData.mMacAddress = loc_data.getMacAddress();
                    locData.mLatitude = loc_data.getLatitude();
                    locData.mLongitude = loc_data.getLongitude();
                    locData.mValidBits = locData.AP_LOC_WITH_LAT_LON;
                    try{
                        locData.mMaxAntenaRange = loc_data.getMaxAntenaRange();
                        locData.mValidBits |= locData.AP_LOC_MAR_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "MAR exception ");
                        //do nothing
                    }

                    try{
                        locData.mHorizontalError = loc_data.getHorizontalError();
                        locData.mValidBits |= locData.AP_LOC_HORIZONTAL_ERR_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "HE exception ");
                        //do nothing
                    }

                    try{
                        locData.mReliability = loc_data.getReliability().ordinal();
                        locData.mValidBits |= locData.AP_LOC_RELIABILITY_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "REL exception ");
                        //do nothing
                    }
                    locationData.add(locData);
                }
            }

            for(IZatAPSpecialInfo spl_info: special_info) {
                if(null != spl_info) {
                    APSpecialInfo info = new APSpecialInfo();
                    info.mMacAddress = spl_info.mMacAddress;
                    info.mInfo = spl_info.mInfo.ordinal();
                    specialInfo.add(info);
                }
            }

            try {
                mReceiver.pushWiFiDB(locationData, specialInfo, days_valid);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to push WiFi DB", e);
            }
        }
    }

    private class IZatWWANDBReceiverImpl extends IZatWWANDBReceiver {
        IWWANDBReceiver mReceiver;
        public IZatWWANDBReceiverImpl(IWWANDBReceiver receiver,
                                      IZatWWANDBReceiverResponseListener listener)
            throws IZatIllegalArgumentException {
            super(listener);
            if(null == receiver || null == listener) {
                throw new IZatIllegalArgumentException("IZatWWANDBReceiverImpl:" +
                                                       " null receiver / listener");
            }
            mReceiver = receiver;
        }

        @Override
        public void requestBSList(int expire_in_days) {
            try {
                mReceiver.requestBSList(expire_in_days);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to request BS LIst", e);
            }
        }

        @Override
        public void pushWWANDB(List<IZatBSLocationData> location_data,
                               List<IZatBSSpecialInfo> special_info,
                               int days_valid) {
            List<BSLocationData> locationData = new ArrayList<BSLocationData>();
            List<BSSpecialInfo> specialInfo = new ArrayList<BSSpecialInfo>();

            for(IZatBSLocationData loc_data: location_data) {
                if(null != loc_data) {
                    BSLocationData locData = new BSLocationData();
                    locData.mCellType = loc_data.getCellType();
                    locData.mCellRegionID1 = loc_data.getCellRegionID1();
                    locData.mCellRegionID2 = loc_data.getCellRegionID2();
                    locData.mCellRegionID3 = loc_data.getCellRegionID3();
                    locData.mCellRegionID4 = loc_data.getCellRegionID4();
                    locData.mLatitude = loc_data.getLatitude();
                    locData.mLongitude = loc_data.getLongitude();
                    locData.mValidBits = locData.BS_LOC_WITH_LAT_LON;

                    try{
                        locData.mHorizontalCoverageRadius = loc_data.getHorizontalCoverageRadius();
                        locData.mValidBits |= locData.BS_LOC_HORIZONTAL_COV_RADIUS_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "HCR exception ");
                        //do nothing
                    }

                    try{
                        locData.mHorizontalConfidence = loc_data.getHorizontalConfidence();
                        locData.mValidBits |= locData.BS_LOC_HORIZONTAL_CONFIDENCE_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "HC exception ");
                        //do nothing
                    }

                    try{
                        locData.mHorizontalReliability =
                                loc_data.getHorizontalReliability().ordinal();
                        locData.mValidBits |= locData.BS_LOC_HORIZONTAL_RELIABILITY_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "HR exception ");
                        //do nothing
                    }

                    try{
                        locData.mAltitude = loc_data.getAltitude();
                        locData.mValidBits |= locData.BS_LOC_ALTITUDE_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "ALT exception ");
                        //do nothing
                    }

                    try{
                        locData.mAltitudeUncertainty = loc_data.getAltitudeUncertainty();
                        locData.mValidBits |= locData.BS_LOC_ALTITUDE_UNCERTAINTY_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "ALU exception ");
                        //do nothing
                    }

                    try{
                        locData.mAltitudeConfidence = loc_data.getAltitudeConfidence();
                        locData.mValidBits |= locData.BS_LOC_ALTITUDE_CONFIDENCE_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "ALC exception ");
                        //do nothing
                    }

                    try{
                        locData.mAltitudeReliability = loc_data.getAltitudeReliability().ordinal();
                        locData.mValidBits |= locData.BS_LOC_ALTITUDE_RELIABILITY_VALID;
                    }catch (IZatStaleDataException e){
                        Log.e(TAG, "ALR exception ");
                        //do nothing
                    }

                    locationData.add(locData);
                }
            }

            for(IZatBSSpecialInfo spl_info: special_info) {
                if(null != spl_info) {
                    BSSpecialInfo info = new BSSpecialInfo();
                    info.mCellType = spl_info.getCellType();
                    info.mCellRegionID1 = spl_info.getCellRegionID1();
                    info.mCellRegionID2 = spl_info.getCellRegionID2();
                    info.mCellRegionID3 = spl_info.getCellRegionID3();
                    info.mCellRegionID4 = spl_info.getCellRegionID4();
                    info.mInfo = spl_info.mInfo.ordinal();
                    specialInfo.add(info);
                }
            }

            try {
                mReceiver.pushWWANDB(locationData, specialInfo, days_valid);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to push WWAN DB", e);
            }
        }
    }

    private class IZatGeofenceServiceImpl implements IZatGeofenceService {
        IGeofenceService mService;
        public IZatGeofenceServiceImpl(IGeofenceService service) {
            mService = service;
        }

        @Override
        public void registerForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                                 throws IZatIllegalArgumentException {
            if (null == statusCb) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            /* save the callbacks bonding with the caller. If there is already a
               callback associated with the caller, then the new cb will override
               the existing cb. */
            synchronized (sGeofenceClientCallbackMapLock) {
                mGeofenceClientCallbackMap.put(this, statusCb);
            }
        }

        @Override
        public void deregisterForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                                   throws IZatIllegalArgumentException {
            if (null == statusCb) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            synchronized (sGeofenceClientCallbackMapLock) {
                mGeofenceClientCallbackMap.remove(this);
            }
        }

        @Override
        public void registerPendingIntent(PendingIntent geofenceIntent)
                                throws IZatIllegalArgumentException {
            if (null == geofenceIntent) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }

            synchronized (sGeofenceServiceLock) {
                try {
                    mService.registerPendingIntent(geofenceIntent);
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed registerPendingIntent");
                }
            }
        }

        @Override
        public void unregisterPendingIntent(PendingIntent geofenceIntent)
                                throws IZatIllegalArgumentException {
            if (null == geofenceIntent) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }

            synchronized (sGeofenceServiceLock) {
                try {
                    mService.unregisterPendingIntent(geofenceIntent);
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed unregisterPendingIntent");
                }
            }
        }

        @Override
        public Map<IZatGeofenceHandle, IzatGeofence> recoverGeofences() {
            IZatGeofenceCallback cb = mGeofenceClientCallbackMap.get(this);
            if (null == cb) {
                Log.e(TAG, "callback is not registered");
                return null;
            }

            Map<IZatGeofenceHandle, IzatGeofence> gfHandleDataMap =
                new HashMap<IZatGeofenceHandle, IzatGeofence> ();
            ArrayList<GeofenceData> gfList = new ArrayList<GeofenceData> ();

            try {
                synchronized(sGeofenceServiceLock) {
                    mService.recoverGeofences(gfList);
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to recover geofences", e);
            }
            synchronized (sGeofencesMapLock) {
                for (GeofenceData geofence : gfList) {
                    // create a new IzatGeofence object to be returned
                    IZatGeofenceService.IzatGeofence gfObj =
                            getIzatGeofenceFromGeofenceData(geofence);

                    // check if IZatGeofenceHandle for this geofence id exists
                    // Update gfHandleDataMap
                    boolean handleExists = false;
                    for (Map.Entry<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                            GeofenceMapItem> entry :
                        mGeofencesMap.entrySet()) {
                        if (entry.getValue().getHWGeofenceId() == geofence.getGeofenceId()) {
                            handleExists = true;
                            gfHandleDataMap.put(entry.getKey(), gfObj);
                            break;
                        }
                    }

                    if (false == handleExists) {
                        IZatGeofenceHandleImpl handle = new IZatGeofenceHandleImpl();
                        String appTextData = geofence.getAppTextData();
                        Object appData;
                        if (appTextData != null) {
                            appData = appTextData;
                        } else {
                            appData = geofence.getAppBundleData();
                        }
                        mGeofencesMap.put(handle,
                            new GeofenceMapItem(appData,
                            geofence.getGeofenceId(), cb));
                        gfHandleDataMap.put(handle, gfObj);
                    }
                }
            }
            return gfHandleDataMap;
        }

        @Override
        public IZatGeofenceHandle addGeofence(Object context, IzatGeofence geofence)
                                              throws IZatIllegalArgumentException {
            if (null == geofence) {
                throw new IZatIllegalArgumentException("invalid null geofence");
            }
            if (geofence.getLatitude() < -90 || geofence.getLatitude() > 90) {
                throw new IZatIllegalArgumentException("invalid geofence latitude." +
                                                       " Expected in range -90..90.");
            }
            if (geofence.getLongitude() < -180 || geofence.getLongitude() > 180) {
                throw new IZatIllegalArgumentException("invalid geofence longitude." +
                                                       " Expected in range -180..180.");
            }
            IZatGeofenceCallback cb = mGeofenceClientCallbackMap.get(this);
            int geofenceId;

            if (null == cb) {
                Log.e(TAG, "callback is not registered.");
                return null;
            }
            try {
                synchronized (sGeofenceServiceLock) {
                    double latitude = geofence.getLatitude();
                    double longitude = geofence.getLongitude();
                    double radius = geofence.getRadius();
                    int transitionType = geofence.getTransitionTypes().getValue();
                    int responsiveness = geofence.getNotifyResponsiveness();
                    int confidence = geofence.getConfidence().getValue();

                    int dwellTime = 0;
                    int dwellType = 0;
                    if (geofence.getDwellNotify() != null) {
                        dwellTime = geofence.getDwellNotify().getDwellTime();
                        dwellType = geofence.getDwellNotify().getDwellType();
                    }

                    if ((context != null) &&
                            ((context instanceof String) || (context instanceof Bundle))) {

                        String appTextData =
                                ((context instanceof String) ? context.toString() : null);
                        Bundle bundleObj =
                                ((context instanceof Bundle) ? (Bundle) context : null);

                        geofenceId = mService.addGeofenceObj(new GeofenceData(responsiveness,
                                                                              latitude,
                                                                              longitude,
                                                                              radius,
                                                                              transitionType,
                                                                              confidence,
                                                                              dwellType,
                                                                              dwellTime,
                                                                              appTextData,
                                                                              bundleObj,
                                                                              -1));
                    } else {
                        geofenceId = mService.addGeofence(latitude,
                                                          longitude,
                                                          radius,
                                                          transitionType,
                                                          responsiveness,
                                                          confidence,
                                                          dwellTime,
                                                          dwellType);
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed addGeofence", e);
            }
            IZatGeofenceHandleImpl handle = new IZatGeofenceHandleImpl();
            synchronized (sGeofencesMapLock) {
                mGeofencesMap.put(handle,
                                  new GeofenceMapItem(context, geofenceId, cb));
            }
            return handle;
        }

        @Override
        public void removeGeofence(IZatGeofenceHandle handler)
                                   throws IZatIllegalArgumentException {
            if (null == handler ||
                !(handler instanceof IZatGeofenceServiceImpl.IZatGeofenceHandleImpl)) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sGeofenceServiceLock) {
                    GeofenceMapItem mapItem = mGeofencesMap.get(handler);
                    if (null == mapItem) {
                        Log.e(TAG, "this request ID is unknown.");
                        IZatGeofenceCallback cb =
                                mGeofenceClientCallbackMap.get(handler);
                        if (cb != null) {
                            cb.onRequestFailed(handler,
                                    GEOFENCE_REQUEST_TYPE_REMOVE,
                                    GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                        } else {
                            throw new IZatIllegalArgumentException(
                                    "Invalid Geofence handle");
                        }
                        return;
                    }
                    mService.removeGeofence(mapItem.getHWGeofenceId());
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed removeGeofence", e);
            }
            synchronized (sGeofencesMapLock) {
                mGeofencesMap.remove(handler);
            }
        }

        private GeofenceData recoverGeofenceDataById(int geofenceId)
                throws IZatIllegalArgumentException {
            ArrayList<GeofenceData> gfList = new ArrayList<GeofenceData> ();
            try {
                synchronized(sGeofenceServiceLock) {
                    mService.recoverGeofences(gfList);

                    for (GeofenceData geofence : gfList) {
                        if (geofenceId == geofence.getGeofenceId()) {
                            return geofence;
                        }
                    }
                    throw new IZatIllegalArgumentException("Invalid geofence id");
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed to recover geofence", e);
            }
        }

        private IzatGeofence getIzatGeofenceFromGeofenceData(GeofenceData geofence) {
            IZatGeofenceService.IzatGeofence gfObj =
                    new IZatGeofenceService.IzatGeofence(geofence.getLatitude(),
                    geofence.getLongitude(), geofence.getRadius());
                    gfObj.setNotifyResponsiveness(geofence.getNotifyResponsiveness());
                    gfObj.setTransitionTypes(
                            IZatGeofenceService.IzatGeofenceTransitionTypes.values()[
                            geofence.getTransitionType().getValue()]);
                    gfObj.setConfidence(
                            IZatGeofenceService.IzatGeofenceConfidence.values()[
                            geofence.getConfidence().getValue() - 1]);

                    IZatGeofenceService.IzatDwellNotify dwellNotify =
                            new IZatGeofenceService.IzatDwellNotify(
                                    geofence.getDwellTime(),
                                    geofence.getDwellType().getValue());
                    gfObj.setDwellNotify(dwellNotify);

            return gfObj;
        }

        private class IZatGeofenceHandleImpl implements IZatGeofenceService.IZatGeofenceHandle {

            @Override
            public Object getContext() {
                GeofenceMapItem mapItem = mGeofencesMap.get(this);
                if (mapItem != null) {
                    return mapItem.getContext();
                }
                return null;
            }

            @Override
            @Deprecated
            public void update(IzatGeofenceTransitionTypes transitionTypes,
                               int notifyResponsiveness)
                               throws IZatIllegalArgumentException {
                if (null == transitionTypes || 0 >= notifyResponsiveness) {
                    throw new IZatIllegalArgumentException("invalid input parameter");
                }

                IzatGeofence gf = new IZatGeofenceService.IzatGeofence(null, null, null);
                gf.setTransitionTypes(transitionTypes);
                gf.setNotifyResponsiveness(notifyResponsiveness);

                update(gf);
            }

            @Override
            public void update(IzatGeofence geofence) throws IZatIllegalArgumentException {
                try {
                    synchronized (sGeofenceServiceLock) {
                        GeofenceMapItem mapItem = mGeofencesMap.get(this);
                        if (null == mapItem) {
                            Log.e(TAG, "this request ID is unknown.");
                            IZatGeofenceCallback cb =
                                    mGeofenceClientCallbackMap.get(this);
                            if (cb != null) {
                                cb.onRequestFailed(this,
                                        GEOFENCE_REQUEST_TYPE_UPDATE,
                                        GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                            } else {
                                throw new IZatIllegalArgumentException(
                                        "Invalid Geofence handle");
                            }
                            return;
                        }
                        mService.updateGeofenceData(mapItem.getHWGeofenceId(),
                                                    geofence.getLatitude(),
                                                    geofence.getLongitude(),
                                                    geofence.getRadius(),
                                                    geofence.getTransitionTypes().getValue(),
                                                    geofence.getNotifyResponsiveness(),
                                                    geofence.getConfidence().getValue(),
                                                    geofence.getDwellNotify().getDwellTime(),
                                                    geofence.getDwellNotify().getDwellType(),
                                                    geofence.getUpdatedFieldsMask());
                        geofence.resetUpdatedFieldsMask();
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed update", e);
                }
            }

            @Override
            public void pause() throws IZatIllegalArgumentException {
                try {
                    synchronized (sGeofenceServiceLock) {
                        GeofenceMapItem mapItem = mGeofencesMap.get(this);
                        if (null == mapItem) {
                            Log.e(TAG, "this request ID is unknown.");
                            IZatGeofenceCallback cb =
                                    mGeofenceClientCallbackMap.get(this);
                            if (cb != null) {
                                cb.onRequestFailed(this,
                                        GEOFENCE_REQUEST_TYPE_PAUSE,
                                        GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                            } else {
                                throw new IZatIllegalArgumentException(
                                        "Invalid Geofence handle");
                            }
                            return;
                        }
                        mService.pauseGeofence(mapItem.getHWGeofenceId());
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed pause", e);
                }
            }

            @Override
            public void resume() throws IZatIllegalArgumentException {
                try {
                    synchronized (sGeofenceServiceLock) {
                        GeofenceMapItem mapItem = mGeofencesMap.get(this);
                        if (null == mapItem) {
                            Log.e(TAG, "this request ID is unknown.");
                            IZatGeofenceCallback cb =
                                    mGeofenceClientCallbackMap.get(this);
                            if (cb != null) {
                                cb.onRequestFailed(this,
                                        GEOFENCE_REQUEST_TYPE_RESUME,
                                        GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                            } else {
                                throw new IZatIllegalArgumentException(
                                        "Invalid Geofence handle");
                            }
                            return;
                        }
                        mService.resumeGeofence(mapItem.getHWGeofenceId());
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed resume", e);
                }
            }

            @Override
            public boolean isPaused() throws IZatIllegalArgumentException {
                synchronized (sGeofenceServiceLock) {
                    GeofenceMapItem mapItem = mGeofencesMap.get(this);
                    if (null == mapItem) {
                        Log.e(TAG, "this request ID is unknown.");
                        throw new IZatIllegalArgumentException(
                                "Invalid Geofence handle");
                    }
                    return recoverGeofenceDataById(mapItem.mHWGeofenceId).isPaused();
                }
            }
       }
    }

    public class IZatDebugReportingServiceImpl implements IZatDebugReportingService {
        IDebugReportService mService;

        public IZatDebugReportingServiceImpl(IDebugReportService service) {
            mService = service;
        }

        @Override
        public void registerForDebugReports(IZatDebugReportCallback reportCb)
                                                 throws IZatIllegalArgumentException {
            if (null == reportCb) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }

            synchronized (sDebugReportCallbackMapLock) {
                mDebugReportClientCallbackMap.put(this, reportCb);
            }

            synchronized(sDebugReportServiceLock) {
                if (!mDebugReportClientCallbackMap.isEmpty()) {
                    try {
                        mService.startReporting();
                    } catch (RemoteException e) {
                        throw new RuntimeException("Failed to register for debug reports");
                    }
                }
            }
        }

        @Override
        public void deregisterForDebugReports(IZatDebugReportCallback reportCb)
                                                   throws IZatIllegalArgumentException {
            if (null == reportCb) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }

            synchronized (sDebugReportCallbackMapLock) {
                IZatDebugReportCallback cb = mDebugReportClientCallbackMap.get(this);
                if (cb != null) {
                    mDebugReportClientCallbackMap.remove(this);
                }
            }

            synchronized(sDebugReportServiceLock) {
                if (mDebugReportClientCallbackMap.isEmpty()) {
                    try {
                        mService.stopReporting();
                    } catch (RemoteException e) {
                        throw new RuntimeException("Failed to deregister for debug reports");
                    }
                }
            }
        }

       @Override
       public Bundle getDebugReport()
           throws IZatIllegalArgumentException {
           synchronized(sDebugReportServiceLock) {
                Bundle bdlReportObj = null;

                try {
                    bdlReportObj = mService.getDebugReport();
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed to get debug report");
                }

                return bdlReportObj;
           }
       }
    }

    /**
     * Implementation of callback object. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class LocationCallbackWrapper extends ILocationCallback.Stub {
        IZatFlpService.IFlpLocationCallback mCallback;
        public LocationCallbackWrapper(IZatFlpService.IFlpLocationCallback callback) {
            mCallback = callback;
        }
        public void onLocationAvailable(Location[] locations) {
            if (null == mCallback) {
                Log.w(TAG, "mCallback is NULL in LocationCallbackWrapper");
                return;
            }
            mCallback.onLocationAvailable(locations);
        }
    }

    /* Implementation of callback object. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class FlpStatusCallbackWrapper extends ISessionStatusCallback.Stub {
        IZatFlpService.IFlpStatusCallback mCallback;
        IFlpService mService;

        public FlpStatusCallbackWrapper(IZatFlpService.IFlpStatusCallback callback,
            IFlpService flpService) {
            mCallback = callback;
            mService = flpService;
        }

        public void onBatchingStatusCb(int status) {
            if (null == mCallback) {
                Log.w(TAG, "mCallback is NULL in FlpStatusCallbackWrapper");
            }

            try {
                synchronized(sFlpServiceLock) {
                    IZatFlpService.IzatFlpStatus batchStatus =
                            IZatFlpService.IzatFlpStatus.values()[status];

                    if (batchStatus != IZatFlpService.IzatFlpStatus.OUTDOOR_TRIP_COMPLETED) {
                        mCallback.onBatchingStatusCb(batchStatus);
                        return;
                    }

                    FlpRequestMapItem mapItem = null;
                    IZatFlpServiceImpl.IZatSessionHandlerImpl sessionHandler = null;
                    for (IZatFlpServiceImpl.IZatSessionHandlerImpl key : mFlpRequestsMap.keySet()) {
                        mapItem = mFlpRequestsMap.get(key);
                        if (mapItem.getStatusCallback() == this) {
                            sessionHandler = key;
                            break;
                        }
                    }

                    if (null == mapItem) {
                        Log.w(TAG, "no flp session undergoing");
                        return;
                    }

                    NotificationType notifType = mapItem.getNotifyType();
                    if ((IZatFlpService.IzatFlpStatus.OUTDOOR_TRIP_COMPLETED == batchStatus) &&
                        (NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED == notifType)) {
                        mCallback.onBatchingStatusCb(batchStatus);

                        // update the FLP session previous notification mode
                        if (mapItem.getRestartOnTripCompleted()) {
                            int result = FLP_RESULT_SUCCESS;
                            result = mService.startFlpSession(mapItem.getHwId(),
                                    mapItem.getPreviousNotifyType().getCode(),
                                    mapItem.getTimeInterval(),
                                    mapItem.getDistanceInterval(), mapItem.getTripDistance());

                            if (FLP_RESULT_SUCCESS == result) {
                                // update the item flag in map
                                mapItem.updateNotifyType(mapItem.getPreviousNotifyType());
                                mapItem.setRestartOnTripCompleted(false);

                                if ((mFlpFeaturMasks &
                                    FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                                    if (NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX ==
                                            mapItem.getNotifyType()) {
                                        // switching from trip mode to foreground mode
                                        mService.unregisterCallback(FLP_SEESION_FOREGROUND,
                                                                    mapItem.getCbWrapper());
                                        mService.registerCallback(FLP_SEESION_BACKGROUND,
                                                                  mapItem.getHwId(),
                                                                  mapItem.getCbWrapper(),
                                                                  mapItem.getSessionStartTime());
                                    }
                                }
                            } else {
                                Log.v(TAG, "mService.updateFlpSession on trip completed failed.");
                            }
                        } else {
                            // remove the session handler from the cache as the trip ahs completed
                            mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                        mapItem.getCbWrapper());
                            sessionHandler.unregisterForSessionStatus();
                            mFlpRequestsMap.remove(sessionHandler);
                        }
                    }
                }
            } catch (RemoteException e) {
                   throw new RuntimeException("Failed to handle onBatchingStatusCb for status:" +
                           status);
            }
        }
    }

    private enum NotificationType {
        NOTIFICATION_WHEN_BUFFER_IS_FULL(1),
        NOTIFICATION_ON_EVERY_LOCATION_FIX(2),
        NOTIFICATION_WHEN_TRIP_IS_COMPLETED(3);
        private final int mCode;
        private NotificationType(int c) {
            mCode = c;
        }

        public int getCode() {
            return mCode;
        }
    }

    private class FlpRequestMapItem {
        public IZatFlpService.IFlpLocationCallback mCallback = null;
        public NotificationType mNotiType = null;
        public LocationCallbackWrapper mCbWrapper = null;
        public FlpStatusCallbackWrapper mStatusCbWrapper = null;
        public long mMaxTime = -1;
        public int mMaxDistance = -1;
        public long mTripDistance = -1;
        public int mHwId = -1;
        private long mSessionStartTime = -1;
        private boolean mRestartOnTripCompleted = false;
        private NotificationType mPreviousNotifType = null;
        public FlpRequestMapItem(IZatFlpService.IFlpLocationCallback callback,
                                 NotificationType notiType,
                                 long maxTime,
                                 int maxDistance,
                                 long tripDistance,
                                 LocationCallbackWrapper cbWrapper,
                                 int hwId,
                                 long sessionStartTime) {
            mCallback = callback;
            mNotiType = notiType;
            mPreviousNotifType = notiType;
            mMaxTime = maxTime;
            mMaxDistance = maxDistance;
            mTripDistance = tripDistance;
            mCbWrapper = cbWrapper;
            mHwId = hwId;
            mSessionStartTime = sessionStartTime;
            mRestartOnTripCompleted = false;
            mStatusCbWrapper = null;
        }
        public IZatFlpService.IFlpLocationCallback getCallback() {
            return mCallback;
        }
        public void updateNotifyType(NotificationType type) {
            mNotiType = type;

            if (NotificationType.NOTIFICATION_WHEN_TRIP_IS_COMPLETED != type) {
                mPreviousNotifType = type;
            }
        }
        public NotificationType getNotifyType() {
            return mNotiType;
        }

        public NotificationType getPreviousNotifyType() {
            return mPreviousNotifType;
        }

        public long getTimeInterval() {
            return mMaxTime;
        }
        public int getDistanceInterval() {
            return mMaxDistance;
        }

        public long getTripDistance() {
            return mTripDistance;
        }

        public LocationCallbackWrapper getCbWrapper() {
            return mCbWrapper;
        }
        public int getHwId() {
            return mHwId;
        }
        public long getSessionStartTime() {
            return mSessionStartTime;
        }
        public void setSessionStartTime(long sessionStartTime) {
            mSessionStartTime = sessionStartTime;
        }

        public void setRestartOnTripCompleted(boolean restart) {
            mRestartOnTripCompleted = restart;
        }

        public boolean getRestartOnTripCompleted() {
            return mRestartOnTripCompleted;
        }

        public void setStatusCallback(FlpStatusCallbackWrapper cbWrapper) {
            mStatusCbWrapper = cbWrapper;
        }

        public FlpStatusCallbackWrapper getStatusCallback() {
            return mStatusCbWrapper;
        }
    }

    private class GeofenceMapItem {
        Object mContext;
        int mHWGeofenceId;
        IZatGeofenceService.IZatGeofenceCallback mCallback;

        public GeofenceMapItem(Object context,
                               int geofenceId,
                               IZatGeofenceService.IZatGeofenceCallback callback)
        {
            mContext = context;
            mHWGeofenceId = geofenceId;
            mCallback = callback;
        }

        public Object getContext() {
            return mContext;
        }
        public int getHWGeofenceId() {
            return mHWGeofenceId;
        }
        public IZatGeofenceService.IZatGeofenceCallback getCallback() {
            return mCallback;
        }
    }

    private class MaxPowerAllocatedCallbackWrapper
        extends IMaxPowerAllocatedCallback.Stub {

        IZatTestService.IFlpMaxPowerAllocatedCallback mCallback;
        public MaxPowerAllocatedCallbackWrapper(
            IZatTestService.IFlpMaxPowerAllocatedCallback callback) {
            mCallback = callback;
        }
        public void onMaxPowerAllocatedChanged(double power_mW) {
            if (null == mCallback) {
                Log.w(TAG, "mCallback is NULL in MaxPowerAllocatedCallbackWrapper");
                return;
            }
            mCallback.onMaxPowerAllocatedChanged(power_mW);
        }
    }

    private class GeofenceStatusCallbackWrapper
        extends IGeofenceCallback.Stub {

        public void onTransitionEvent(int geofenceHwId,
                                      int event,
                                      Location location) {
            if (VERBOSE) {
                Log.d(TAG, "onTransitionEvent - geofenceHwId is " +
                      geofenceHwId + "; event is " + event);
            }
            // find the callback through geofence hw id
            synchronized (sGeofencesMapLock) {
                GeofenceMapItem mapItem = null;
                for (IZatGeofenceServiceImpl.IZatGeofenceHandleImpl key :
                        mGeofencesMap.keySet()) {
                    mapItem = mGeofencesMap.get(key);
                    if (mapItem.getHWGeofenceId() == geofenceHwId) {
                        mapItem.getCallback().onTransitionEvent(key, event, location);
                        return;
                    }
                }
            }
        }

        public void onRequestResultReturned(int geofenceHwId,
                                            int requestType,
                                            int result) {
            if (VERBOSE) {
                Log.d(TAG, "onRequestResultReturned - geofenceHwId is " +
                      geofenceHwId + "; requestType is " + requestType +
                      "; result is " + result);
            }

            if (result == IZatGeofenceService.GEOFENCE_RESULT_SUCCESS) {
                return;
            }
            // find the callback through geofence hw id
            synchronized (sGeofencesMapLock) {
                GeofenceMapItem mapItem = null;
                for (IZatGeofenceServiceImpl.IZatGeofenceHandleImpl key :
                        mGeofencesMap.keySet()) {
                    mapItem = mGeofencesMap.get(key);
                    if (mapItem.getHWGeofenceId() == geofenceHwId) {
                        if (IZatGeofenceService.GEOFENCE_REQUEST_TYPE_ADD == requestType) {
                            mGeofencesMap.remove(key);
                        }
                        mapItem.getCallback().onRequestFailed(key, requestType, result);
                        return;
                    }
                }
            }
        }

        public void onEngineReportStatus(int status,
                                         Location location) {
            if (VERBOSE) {
                Log.d(TAG, "onEngineReportStatus - status is " + status);
            }
            // broadcast to all clients
            synchronized (sGeofenceClientCallbackMapLock) {
                for (IZatGeofenceServiceImpl key : mGeofenceClientCallbackMap.keySet()) {
                    mGeofenceClientCallbackMap.get(key).onEngineReportStatus(status,
                                                                             location);
                }
            }
        }
    }

    private class DebugReportCallbackWrapper
        extends IDebugReportCallback.Stub {

        public void onDebugDataAvailable(Bundle debugReport) {
            if (VERBOSE) {
                Log.v(TAG, "onDebugDataAvailable");
            }
            synchronized(sDebugReportCallbackMapLock) {
                // broadcast to all clients
                for (IZatDebugReportingServiceImpl key : mDebugReportClientCallbackMap.keySet()) {
                    mDebugReportClientCallbackMap.get(key).onDebugReportAvailable(debugReport);
                }
            }
        }
    }

    private class WiFiDBReceiverRespListenerWrapper
        extends IWiFiDBReceiverResponseListener.Stub {

        public void onAPListAvailable(List<APInfo> ap_info) {
            if (VERBOSE) {
                Log.d(TAG, "onAPListAvailable");
            }
            if(null != mWiFiDBRecImpl) {
                List<IZatAPInfo> apInfo = new ArrayList<IZatAPInfo>();
                for(APInfo ap: ap_info) {
                    IZatCellInfo cellInfo = null;
                    switch(ap.mCellType) {
                    case 1:
                        cellInfo = new IZatCellInfo(ap.mCellRegionID1,
                                                    ap.mCellRegionID2,
                                                    ap.mCellRegionID3,
                                                    ap.mCellRegionID4,
                                                    IZatCellInfo.IZatCellTypes.GSM);
                        break;
                    case 2:
                        cellInfo = new IZatCellInfo(ap.mCellRegionID1,
                                                    ap.mCellRegionID2,
                                                    ap.mCellRegionID3,
                                                    ap.mCellRegionID4,
                                                    IZatCellInfo.IZatCellTypes.WCDMA);
                        break;
                    case 3:
                        cellInfo = new IZatCellInfo(ap.mCellRegionID1,
                                                    ap.mCellRegionID2,
                                                    ap.mCellRegionID3,
                                                    ap.mCellRegionID4,
                                                    IZatCellInfo.IZatCellTypes.CDMA);
                        break;
                    case 4:
                        cellInfo = new IZatCellInfo(ap.mCellRegionID1,
                                                    ap.mCellRegionID2,
                                                    ap.mCellRegionID3,
                                                    ap.mCellRegionID4,
                                                    IZatCellInfo.IZatCellTypes.LTE);
                        break;
                    default:
                        //do nothing
                    }
                    IZatAPSSIDInfo ssidInfo = null;
                    if(null != ap.mSSID && ap.mSSID.length > 0) {
                        ssidInfo = new IZatAPSSIDInfo(ap.mSSID, (short)ap.mSSID.length);
                    }
                    apInfo.add(new IZatAPInfo(ap.mMacAddress,
                                              new IZatAPInfoExtra(cellInfo, ssidInfo)));

                }
                mWiFiDBRecImpl.mResponseListener.onAPListAvailable(apInfo);
            }
        }
        public void onStatusUpdate(boolean is_success, String error) {
            if (VERBOSE) {
                Log.d(TAG, "onStatusUpdate");
            }
            if(null != mWiFiDBRecImpl) {
                mWiFiDBRecImpl.mResponseListener.onStatusUpdate(is_success, error);
            }
        }
        public void onServiceRequest() {
            if (VERBOSE) {
                Log.d(TAG, "onServiceRequest");
            }
            if(null != mWiFiDBRecImpl) {
                mWiFiDBRecImpl.mResponseListener.onServiceRequest();
            }
        }
    }

    private class WWANDBReceiverRespListenerWrapper
        extends IWWANDBReceiverResponseListener.Stub {

        public void onBSListAvailable(List<BSInfo> bs_info) {
            if (VERBOSE) {
                Log.d(TAG, "onBSListAvailable");
            }
            if(null != mWWANDBRecImpl) {
                List<IZatBSInfo> bsInfo = new ArrayList<IZatBSInfo>();
                for(BSInfo bs: bs_info) {
                    IZatBSCellInfo cellInfo = null;
                    switch(bs.mCellType) {
                    case 1:
                        cellInfo = new IZatBSCellInfo(bs.mCellRegionID1,
                                                      bs.mCellRegionID2,
                                                      bs.mCellRegionID3,
                                                      bs.mCellRegionID4,
                                                      IZatBSCellInfo.IZatBSCellTypes.GSM);
                        break;
                    case 2:
                        cellInfo = new IZatBSCellInfo(bs.mCellRegionID1,
                                                      bs.mCellRegionID2,
                                                      bs.mCellRegionID3,
                                                      bs.mCellRegionID4,
                                                      IZatBSCellInfo.IZatBSCellTypes.WCDMA);
                        break;
                    case 3:
                        cellInfo = new IZatBSCellInfo(bs.mCellRegionID1,
                                                      bs.mCellRegionID2,
                                                      bs.mCellRegionID3,
                                                      bs.mCellRegionID4,
                                                      IZatBSCellInfo.IZatBSCellTypes.CDMA);
                        break;
                    case 4:
                        cellInfo = new IZatBSCellInfo(bs.mCellRegionID1,
                                                      bs.mCellRegionID2,
                                                      bs.mCellRegionID3,
                                                      bs.mCellRegionID4,
                                                      IZatBSCellInfo.IZatBSCellTypes.LTE);
                        break;
                    default:
                        //do nothing
                    }
                    bsInfo.add(new IZatBSInfo(cellInfo));
                }
                mWWANDBRecImpl.mResponseListener.onBSListAvailable(bsInfo);
            }
        }
        public void onStatusUpdate(boolean is_success, String error) {
            if (VERBOSE) {
                Log.d(TAG, "onStatusUpdate");
            }
            if (null != mWWANDBRecImpl) {
                mWWANDBRecImpl.mResponseListener.onStatusUpdate(is_success, error);
            }
        }
        public void onServiceRequest() {
            if (VERBOSE) {
                Log.d(TAG, "onServiceRequest");
            }
            if (null != mWWANDBRecImpl) {
                mWWANDBRecImpl.mResponseListener.onServiceRequest();
            }
        }
    }
/** @endcond */
}
/** @} */  /* end_addtogroup IZatManager */
