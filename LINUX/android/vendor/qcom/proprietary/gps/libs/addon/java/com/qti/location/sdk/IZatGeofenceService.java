/* ======================================================================
*  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.location.Location;
import android.app.PendingIntent;

import com.qti.geofence.GeofenceData;

import java.util.Map;

 /** @addtogroup IZatGeofenceService
     @{ */

/**
 * IZatGeofenceService provides the interface for interacting with the IZat geofence service.
 *
 * <pre>
 * <code>
 *  // Sample Code
 *
 *  import android.app.Activity;
 *  import android.os.Bundle;
 *  import android.util.Log;
 *  import android.view.View;
 *  import android.widget.Button;
 *  import android.widget.TextView;
 *  import com.qti.location.sdk.IZatManager;
 *  import com.qti.location.sdk.IZatGeofenceService;
 *
 *  public class FullscreenActivity extends Activity {
 *      ......
 *      private IZatGeofenceService.IZatGeofenceHandle mHandle = null;
 *      final GeofenceCallback geofenceCb = new GeofenceCallback();
 *      final Button geofenceBtn = (Button)findViewById(R.id.geofenceBtn);
 *
 *      {@literal @}Override
 *      protected void onCreate(Bundle savedInstanceState) {
 *
 *          geofenceBtn.setOnClickListener(new View.OnClickListener() {
 *              {@literal @}Override
 *              public void onClick(View v) {
 *                  // register callback
 *                  gfSevice.registerForGeofenceCallbacks(geofenceCb);
 *
 *                  // check if activity was started with an breach notification intent
 *                  Intent startintent = getIntent();
 *                  if (startintent != null) {
 *                      int transition;
 *                      Location location;
 *                      String strCtx = startintent.getStringExtra("context-string");
 *                      if (strCtx != null) {
 *                          Log.v(TAG, "Context:" + strCtx);
 *                      } else {
 *                          Bundle bundleObj = intent.getBundleExtra("context-data");
 *                          if (bundleObj != null) {
 *                              // where, TestParcelabable class is of Parcelable type
 *                              bundleObj.setClassLoader(TestParcelable.class.getClassLoader());
 *                              TestParcelable testObj = bundleObj.getParcelable("bundle-obj");
 *                          }
 *                      }
 *                      location = startintent.getParcelableExtra("transition-location");
 *                      transition = startintent.getIntExtra("transition-event", 0);
 *
 *                      if (location != null) {
 *                          Log.v(TAG, "transition:" + transition + " location:" +
 *                                  location.toString());
 *                      }
 *                  }
 *
 *                  // register pending intent for this activity
 *                  Intent activitIntent =
 *                          new Intent(getApplicationContext(), FullscreenActivity.class);
 *                  PendingIntent geofenceIntent = PendingIntent.getActivity(
 *                          getApplicationContext(), 0, activitIntent, 0);
 *                  gfService.registerPendingIntent(geofenceIntent);
 *
 *                  // set geofence circle
 *                  IZatGeofenceService.IzatGeofence geofence =
 *                          new IZatGeofenceService.IzatGeofence(37.375507,
 *                                                               -121.983643,
 *                                                               5000.0);
 *
 *                  // set transition type
 *                  geofence.setTransitionTypes(
 *                          IZatGeofenceService.IzatGeofenceTransitionTypes.ENTERED_AND_EXITED);
 *
 *                  // set responsiveness
 *                  geofence.setNotifyResponsiveness(10000);
 *
 *                  // set confidence
 *                  geofence.setConfidence(IZatGeofenceService.IzatGeofenceConfidence.MEDIUM);
 *
 *                  // create dwell time object
 *                  IZatGeofenceService.IzatDwellNotify dwellNotify =
 *                          new IZatGeofenceService.IzatDwellNotify (3,
 *                                  IZatGeofenceService.IzatDwellNotify.DWELL_TYPE_INSIDE_MASK |
 *                                  IZatGeofenceService.IzatDwellNotify.DWELL_TYPE_OUTSIDE_MASK);
 *
 *                  // set dwell time
 *                  geofence.setDwellNotify(dwellNotify);
 *
 *                  // create a geofence name
 *                  String geofenceName = "geofence sample";
 *
 *                  // add a geofence
 *                  mHandle = gfSevice.addGeofence(geofenceName, geofence);
 *              }
 *          });
 *      }
 *  }
 *
 *  public class GeofenceCallback implements IZatGeofenceService.IZatGeofenceCallback {
 *      private static String TAG = "Breach Callback Sample";
 *
 *      public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
 *                                    int event,
 *                                    Location location) {
 *
 *          Log.v(TAG, "got breach event in app, handle.obj is "+ handle.getContext());
 *          if (location != null) {
 *              Log.v(TAG, "The breach location is " +location.getLatitude()+
 *                         "/"+location.getLongitude());
 *          }
 *      }
 *
 *      public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
 *                                  int requestType,
 *                                  int errorCode) {
 *
 *          Log.v(TAG, "got onRequestFailed in app, handle.obj is "+ handle.getContext() +
 *                     "; requestType is " + requestType + "; errorCode is " +errorCode);
 *      }
 *
 *      public void onEngineReportStatus(int status,
 *                                       Location location) {
 *
 *          Log.v(TAG, "got geofence status in app, status is "+status);
 *          if (location != null) {
 *              Log.v(TAG, "The last location is " +location.getLatitude()+
 *                         "/"+location.getLongitude());
 *          }
 *      }
 *  }
 * </code>
 * </pre>
 */
public interface IZatGeofenceService {

    public static final int GEOFENCE_RESULT_SUCCESS = 0;
    public static final int GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES = -100;
    public static final int GEOFENCE_RESULT_ERROR_ID_UNKNOWN = -102;
    public static final int GEOFENCE_RESULT_ERROR_INVALID_TRANSITION = -103;
    public static final int GEOFENCE_RESULT_ERROR_GENERIC = -149;

    public static final int GEOFENCE_REQUEST_TYPE_ADD = 1;
    public static final int GEOFENCE_REQUEST_TYPE_REMOVE = 2;
    public static final int GEOFENCE_REQUEST_TYPE_PAUSE = 3;
    public static final int GEOFENCE_REQUEST_TYPE_RESUME = 4;
    public static final int GEOFENCE_REQUEST_TYPE_UPDATE = 5;

    public static final int GEOFENCE_EVENT_ENTERED = 1;
    public static final int GEOFENCE_EVENT_EXITED = 2;
    public static final int GEOFENCE_EVENT_UNCERTAIN = 4;
    public static final int GEOFENCE_EVENT_DWELL_INSIDE = 8;
    public static final int GEOFENCE_EVENT_DWELL_OUTSIDE = 16;

    //GNSS is unavailable and GNSS position fixes cannot be used to monitor Geofence
    public static final int GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE = 1;
    //GNSS is now available and GNSS position fixes can be used to monitor Geofence
    public static final int GEOFENCE_GEN_ALERT_GNSS_AVAILABLE = 2;
    //The engine is out of service and no cell ID coverage information is available
    public static final int GEOFENCE_GEN_ALERT_OOS = 3;
    //The engine has an invalid time
    public static final int GEOFENCE_GEN_ALERT_TIME_INVALID = 4;

    /**
     * Registers a geofence callback.
     *
     * This function enables applications to get geofence callbacks. If there
     * is already a callback registered, the new callback
     * overrides the existing one.
     *
     * @param statusCb Status of the registration to receive geofence callbacks.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException Callback status parameter is NULL.
     * @return
     * None.
     */
    void registerForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                      throws IZatIllegalArgumentException;

    /**
     * Deregisters a geofence callback.
     *
     * This function enables applications to stop getting geofence callbacks.
     *
     * @param statusCb Status of the deregistration to stop receiving geofence callbacks.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException Callback status parameter is NULL.
     * @return
     * None.
     */
    void deregisterForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                        throws IZatIllegalArgumentException;

    /**
     * Adds a geofence.
     *
     * This function enables applications to add a geofence and to start monitoring
     * the transition. When the device enters or exits the geofence
     * circle, the application receives alerts through the callback.
     * If the geofence fails to be added, the application receives
     * the failure error through the registered callback.
     *
     * @param context Object used to identify the geofence, which could
     *         be a string as the name of the geofence, or it can be a
     *         <a href="https://developer.android.com/reference/android/os/Bundle.html">
     *         Bundle</a> type. This parameter can be NULL.
     * @param geofence Geofence request.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException  Geofence parameter is NULL.
     * @return
     * The handle binding with the geofence.
     */
    IZatGeofenceHandle addGeofence(Object context,
                                   IzatGeofence geofence)
                                   throws IZatIllegalArgumentException;

    /**
     * Removes a geofence.
     *
     * This function enables applications to remove a geofence. If the geofence
     * removal fails, the application gets the failure
     * error through the registered callback.
     *
     * @param handler Geofence handler.
     *         If a registered callback is not found that corresponds
     *         to the passed-in handler,
     *         a {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException Callback does not correspond to the passed-in handler.
     * @return
     * None.
     */
    void removeGeofence(IZatGeofenceHandle handler)
                        throws IZatIllegalArgumentException;

   /**
     * Registers a pending intent.
     * See <a href="https://developer.android.com/reference/android/app/PendingIntent.html">
     * PendingIntent</a>
     *
     * This function enables applications to register an intent and a target action that
     * the service can perform on its behalf. If there
     * is already an intent registered, the new intent
     * overrides the existing one.
     *
     * If the application goes away and there is no registered PendingIntent,
     * it is added geofences are removed from the service.
     * If the application goes away and there is a registered PendingIntent,
     * it is added geofences continue to exist and the supplied PendingIntent
     * is executed on a breach event.
     * Extra data is added to the PendingIntent to include:
     * <ul>
     * <li> The transition event with the ("transition-event", int) key-value pair </li>
     * <li> The transition location with the ("transition-location", Location)
     * key-value pair </li>
     * <li> The context object when the geofence was added with the
     * ("context-data", String) or ("context-data", Bundle) key-value pair </li>
     * </ul>
     *
     * @param geofenceIntent Application intent.
     *         If a NULL intent is passed, a {@link IZatIllegalArgumentException}
     *         is thrown.
     * @throws IZatIllegalArgumentException A NULL intent was passed.
     * @return
     * None.
     */

    void registerPendingIntent(PendingIntent geofenceIntent)
                        throws IZatIllegalArgumentException;

  /**
     * Unregisters a pending intent.
     * See <a href="https://developer.android.com/reference/android/app/PendingIntent.html">
     * PendingIntent</a>
     *
     * This function enables applications to unregister a previously registered PendingIntent.
     *
     * @param geofenceIntent Application intent.
     *         If a NULL intent is passed, a {@link IZatIllegalArgumentException}
     *         is thrown.
     * @throws IZatIllegalArgumentException A NULL intent was passed.
     * @return
     * None.
     */
    void unregisterPendingIntent(PendingIntent geofenceIntent)
                        throws IZatIllegalArgumentException;

     /**
     * Recovers geofences.
     *
     * This function enables an application to query for its added geofences at any time.
     * A typical use case is, if an application goes away after adding geofences and
     * is launched back (manually or via PendingIntent, see {@link registerPendingIntent}),
     * it can query all its previously added geofences.
     * Geofences added without registering for a PendingIntent cannot
     * be recovered.
     *
     * @return A map of the handle binding the geofence (see {@link IZatGeofenceHandle})
     *         and the corresponding {@link IzatGeofence } object.
     */
    Map<IZatGeofenceHandle, IzatGeofence>  recoverGeofences()
                        throws IZatIllegalArgumentException;

 /** @} */ /* end_addtogroup IZatGeofenceService */

 /** @addtogroup IZatGeofenceService
     @{ */

    /**
     * Interface class IZatGeofenceHandle.
     *
     * IZatGeofenceHandle is the interface used to manipulate an added geofence.
     *
     * <pre>
     * <code>
     *  // Sample Code
     *
     *  import android.app.Activity;
     *  import android.os.Bundle;
     *  import android.util.Log;
     *  import android.view.View;
     *  import android.widget.Button;
     *  import android.widget.TextView;
     *  import com.qti.location.sdk.IZatManager;
     *  import com.qti.location.sdk.IZatGeofenceService;
     *
     *  public class FullscreenActivity extends Activity {
     *      ......
     *      private IZatGeofenceService.IZatGeofenceHandle mHandle = null;
     *      final GeofenceCallback geofenceCb = new GeofenceCallback();
     *      final Button geofenceBtn = (Button)findViewById(R.id.geofenceBtn);
     *
     *     {@literal @}Override
     *      protected void onCreate(Bundle savedInstanceState) {
     *          ......
     *          geofenceBtn.setOnClickListener(new View.OnClickListener() {
     *             {@literal @}Override
     *              public void onClick(View v) {
     *                      // register callback
     *                      gfSevice.registerForGeofenceCallbacks(geofenceCb);
     *
     *                      // set geofence circle
     *                      IZatGeofenceService.IzatGeofence geofence =
     *                              new IZatGeofenceService.IzatGeofence(37.375507,
     *                                                                   -121.983643,
     *                                                                   5000.0);
     *                      // add a geofence
     *                      mHandle = gfSevice.addGeofence(geofenceName, geofence);
     *
     *                      // update the geofence through handle
     *                      geofence.setTransitionTypes(
     *                              IZatGeofenceService.IzatGeofenceTransitionTypes.EXITED_ONLY);
     *                      geofence.setNotifyResponsiveness(897600);
     *                      mHandle.update(geofence);
     *
     *                      // pause the geofence through handle
     *                      mHandle.pause();
     *              }
     *         });
     *      }
     *  }
     *
     *  public class GeofenceCallback implements IZatGeofenceService.IZatGeofenceCallback {
     *
     *      public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                    int event,
     *                                    Location location) {
     *          ......
     *      }
     *
     *      public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                  int requestType,
     *                                  int errorCode) {
     *          ......
     *      }
     *
     *      public void onEngineReportStatus(int status,
     *                                       Location location) {
     *          ......
     *      }
     *  }
     * </code>
     * </pre>
     */
    interface IZatGeofenceHandle {
        /**
         * Retrieves the context associated with a geofence.
         *
         * This function enables applications to get the context object
         * associated with a geofence.
         *
         * @return Context associated with the geofence.
         */
        Object getContext();

        /**
         * Updates the geofence (deprecated).
         *
         * This function enables applications to update the transition types and
         * responsiveness of an existing geofence.
         *
         * If the geofence associated with this handle has been removed,
         * the handle is no longer valid, so no geofence
         * is added or updated. If the geofence update fails,
         * the application gets the failure error through the registered
         * callback.
         *
         * If a registered callback corresponding to this
         * handle is not found, a {@link IZatIllegalArgumentException} is thrown.
         *
         * @param transitionTypes New geofence transition types.
         *         This parameter cannot be NULL, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException  The transitionTypes parameter is NULL.
         *
         * @param notifyResponsiveness New geofence responsiveness in seconds.
         *         This parameter cannot be less than or equal to zero, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException  Responsiveness is less than or equal to zero.
         * @return
         * None.
         */
        void update(IzatGeofenceTransitionTypes transitionTypes,
                    int notifyResponsiveness)
                    throws IZatIllegalArgumentException;

        /**
         * Updates the geofence.
         *
         * This function enables applications to update the geofence data.
         *
         * If the geofence associated with this handle has been removed,
         * the handle is no longer valid, so no geofence
         * is added or updated. If the geofence update fails,
         * the application gets the failure error through the registered
         * callback.
         *
         * If a registered callback corresponding to this handle is not
         * found, a {@link IZatIllegalArgumentException} is thrown.
         *
         * @param geofence Updated geofence object.
         *         This parameter cannot be NULL, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException  The geofence parameter is NULL.
         * @return
         * None.
         */
        void update(IzatGeofence geofence) throws IZatIllegalArgumentException;

        /**
         * Pauses the geofence.
         *
         * This function enables applications to pause an existing geofence.
         *
         * If the geofence associated with this handle has been removed,
         * the handle is no longer valid, so no geofence
         * is added or paused. Once a geofence is paused successfully, when
         * the device enters or exits the geofence circle, the application does
         * not get an alert. If the geofence pause fails, the application
         * gets the failure error through the registered callback.
         *
         * If a registered callback corresponding to this handle is not
         * found, a {@link IZatIllegalArgumentException} is thrown.
         *
         * @return
         * None.
         */
        void pause() throws IZatIllegalArgumentException;

        /**
         * Resumes a paused geofence.
         *
         * This function enables applications to resume a paused geofence.
         *
         * If the geofence associated with this handle has been removed,
         * the handle is no longer valid, so no geofence is
         * added or resumed. Once a geofence is resumed successfully,
         * the application continues to get alerts when the device enters
         * or exits the geofence circle. If the geofence resume fails,
         * the application gets the failure error through the
         * registered callback.
         *
         * If a registered callback corresponding to this handle is not
         * foumd, a {@link IZatIllegalArgumentException} is thrown.
         *
         * @throws IZatIllegalArgumentException A registered callback corresponding
         *                                      to this handle was not found.
         *
         * @return
         * None.
         */
        void resume() throws IZatIllegalArgumentException;

        /**
         * Retrieves the geofence pause status.
         *
         * This function enables applications to get the pause status of a geofence.
         *
         * If the geofence associated with this handle has been removed,
         * the handle is no longer valid.
         *
         * If a registered callback corresponding to this handle is not
         * found, a {@link IZatIllegalArgumentException} is thrown.
         *
         * @throws IZatIllegalArgumentException A registered callback corresponding
         *                                      to this handle was not found.
         * @return TRUE if the geofence is paused.
         */
        boolean isPaused() throws IZatIllegalArgumentException;
    }

/** @} */  /* end_addtogroup IZatGeofenceService */

/** @addtogroup IZatGeofenceService
    @{ */

    /**
     * Interface class IZatGeofenceCallback.
     *
     * IZatGeofenceCallback is the geofence callback interface.
     *
     * <pre>
     * <code>
     *
     *  // Sample Code
     *
     *  public class GeofenceCallback implements IZatGeofenceService.IZatGeofenceCallback {
     *
     *      private static String TAG = "geofence callback sample";
     *      public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                    int event,
     *                                    Location location) {
     *
     *          Log.v(TAG, "got breach event in app, handle.obj is "+ handle.getContext());
     *
     *          if (location != null) {
     *             Log.v(TAG, "The breach location is " +location.getLatitude()+
     *                        "/"+location.getLongitude());
     *          } else {
     *             Log.v(TAG, "The breach location is Null");
     *          }
     *      }
     *
     *      public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                  int requestType,
     *                                  int errorCode) {
     *
     *          Log.v(TAG, "got onRequestFailed in app, handle.obj is "+
     *                     handle.getContext() + "; requestType is " +
     *                     requestType + "; errorCode is " +errorCode);
     *      }
     *
     *      public void onEngineReportStatus(int status,
     *                                       Location location) {
     *
     *          Log.v(TAG, "got geofence status in app, status is "+status);
     *
     *          if (location != null) {
     *             Log.v(TAG, "The last location is " +location.getLatitude()+
     *                        "/"+location.getLongitude());
     *          } else {
     *             Log.v(TAG, "The last location is Null");
     *          }
     *      }
     *  }
     * </code>
     * </pre>
     */
    interface IZatGeofenceCallback {
        /**
         * Transitions an event callback.
         *
         * This API is called by the underlying service back
         * to applications when the device enters or exits the geofence
         * circle, or when the dwell time expires. Applications should implement
         * this interface.
         *
         * @param handle Geofence handle.
         * @param event Indicates when the device enters or exits the geofence. \n
         *         Possible values: \n
         *         {@link GEOFENCE_EVENT_ENTERED}, \n
         *         {@link GEOFENCE_EVENT_EXITED}, \n
         *         {@link GEOFENCE_EVENT_UNCERTAIN}, \n
         *         {@link GEOFENCE_EVENT_DWELL_INSIDE}, \n
         *         {@link GEOFENCE_EVENT_DWELL_OUTSIDE}
         * @param location Location where the device enters or
         *         exits the geofence circle, or the location when
         *         the dwell time expires.
         * @return
         * None.
         */
        void onTransitionEvent(IZatGeofenceHandle handle,
                               int event,
                               Location location);
        /**
         * Requests a failure callback.
         *
         * This API is called by the underlying service back
         * to applications when an add/remove/update/pause/resume geofence
         * request fails. Applications should implement this interface.
         *
         * @param handle Geofence handle.
         * @param requestType Indicates which request has failed. \n
         *         Possible values: \n
         *         {@link GEOFENCE_REQUEST_TYPE_ADD}, \n
         *         {@link GEOFENCE_REQUEST_TYPE_REMOVE}, \n
         *         {@link GEOFENCE_REQUEST_TYPE_UPDATE}, \n
         *         {@link GEOFENCE_REQUEST_TYPE_RESUME}, \n
         *         {@link GEOFENCE_REQUEST_TYPE_PAUSE}
         * @param errorCode Failure reason. \n
         *         Possible values: \n
         *         {@link GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES}, \n
         *         {@link GEOFENCE_RESULT_ERROR_ID_UNKNOWN}, \n
         *         {@link GEOFENCE_RESULT_ERROR_INVALID_TRANSITION},\n
         *         {@link GEOFENCE_RESULT_ERROR_GENERIC},
         * @return
         * None.
         */
        void onRequestFailed(IZatGeofenceHandle handle,
                             int requestType,
                             int errorCode);
        /**
         * Engine status callback.
         *
         * This API is called by the underlying service back
         * to applications when the geofence engine reports its status.
         *
         * @param status Indicates the geofence engine status. \n
         *         Possible values: \n
         *         {@link GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE}, \n
         *         {@link GEOFENCE_GEN_ALERT_GNSS_AVAILABLE}, \n
         *         {@link GEOFENCE_GEN_ALERT_OOS}, \n
         *         {@link GEOFENCE_GEN_ALERT_TIME_INVALID}
         * @param location Last location that the engine generated.
         *                     This parameter can be NULL.
         * @return
      * None.
         */
        void onEngineReportStatus(int status,
                                  Location location);
    }

 /** @} */ /* end_addtogroup IZatGeofenceService */

 /** @addtogroup IZatGeofenceService
     @{ */

    /**
     * class IzatGeofence.
     */
    public class IzatGeofence {
        private int mNotifyResponsiveness;
        private double mLatitude;
        private double mLongitude;
        private double mRadius;
        private IzatGeofenceTransitionTypes mTransitionTypes;
        private IzatGeofenceConfidence mConfidence;
        private IzatDwellNotify mDwellNotify;
        private int mUpdatedFieldsMask = 0x0;

        /**
         * Creates a geofence area.
         *
         * This function creates a geofence of a circular region on a horizontal plane.
         *
         * @param latitude Latitude in degrees; between -90 and +90, inclusive.
         * @param longitude Longitude in degrees; between -180 and +180, inclusive.
         * @param radius Radius in meters.
         *
         * @return
         * None.
         */
        public IzatGeofence(double latitude,
                            double longitude,
                            double radius) {
            this((Double) latitude, (Double) longitude, (Double) radius);
        }

        /**
         * Creates a geofence area with optional parameters.
         *
         * This function creates a geofence of a circular region on a horizontal plane.
         * This constructor is usefull for geofence updates. Using NULL in any of these
         * parameters keeps them unchanged after the update operation.
         * However, the parameters are mandatory for a new geofence.
         *
         * @param latitude Latitude in degrees; between -90 and +90, inclusive.
         * @param longitude Longitude in degrees; between -180 and +180, inclusive.
         *
         * @param radius Radius in meters.
         * @return
         * None.
         */
        public IzatGeofence(Double latitude,
                            Double longitude,
                            Double radius) {
            if (null != latitude) {
                mLatitude = latitude;
                mUpdatedFieldsMask |= GeofenceData.LATITUDE_IS_SET;
            }
            if (null != longitude) {
                mLongitude = longitude;
                mUpdatedFieldsMask |= GeofenceData.LONGITUDE_IS_SET;
            }
            if (null != radius) {
                mRadius = radius;
                mUpdatedFieldsMask |= GeofenceData.RADIUS_IS_SET;
            }

            mNotifyResponsiveness = 0;
            mTransitionTypes = IzatGeofenceTransitionTypes.UNKNOWN;
            mConfidence = IzatGeofenceConfidence.LOW;
        }
        public void resetUpdatedFieldsMask() {
            mUpdatedFieldsMask = 0x0;
        }
        public int getUpdatedFieldsMask() {
            return mUpdatedFieldsMask;
        }
        public double getLatitude() {
            return mLatitude;
        }
        public double getLongitude() {
            return mLongitude;
        }
        public double getRadius() {
            return mRadius;
        }
        /**
         * Sets the transition type.
         *
         * This function sets the geofence transition type that is being monitored,
         * based on which the alert is generated.
         *
         * @param transitionTypes Indicates which transition type generates
         *         the alert.
         * @return
         * None.
         */
        public void setTransitionTypes(IzatGeofenceTransitionTypes transitionTypes) {
            mTransitionTypes = transitionTypes;
            mUpdatedFieldsMask |=  GeofenceData.TRANSITION_TYPES_IS_SET;
        }

        /**
         * Gets the transition type setting.
         *
         * @return The current transition type of the geofence.
         */
        public IzatGeofenceTransitionTypes getTransitionTypes() {
            return mTransitionTypes;
        }
        /**
         * Sets the responsiveness.
         *
         * This function sets the geofence responsiveness; it defaults to 0.
         * Smaller responsiveness values means the device could get the alert
         * more quickly after the user enters or exits the geofence
         * area. A larger responsiveness value means there could be a delay
         * for the alert, but it can help save power.
         *
         * @param notifyResponsiveness Responsiveness in milliseconds.
         * @return
         * None.
         */
        public void setNotifyResponsiveness(int notifyResponsiveness) {
            mNotifyResponsiveness = notifyResponsiveness;
            mUpdatedFieldsMask |= GeofenceData.RESPONSIVENESS_IS_SET;
        }
        /**
         * Gets the responsiveness setting.
         *
         * @return The current responsiveness of the geofence.
         */
        public int getNotifyResponsiveness() {
            return mNotifyResponsiveness;
        }
        /**
         * Sets the confidence.
         *
         * This function sets the geofence confidence level, which determines the probability
         * that the alert happened at the geofence boundary. A higher confidence
         * level can cost more power consumption.
         *
         * @param confidence Geofence confidence.
         * @return
         * None.
         */
        public void setConfidence(IzatGeofenceConfidence confidence) {
            mConfidence = confidence;
            mUpdatedFieldsMask |= GeofenceData.CONFIDENCE_IS_SET;
        }

        /**
         * Gets the confidence setting.
         *
         * @return The current confidence of the geofence.
         */
        public IzatGeofenceConfidence getConfidence() {
            return mConfidence;
        }
        /**
         * Sets the dwell notification.
         *
         * @param dwellNotify Geofence dwell notification.
         *
         * @return
         * None.
         */
        public void setDwellNotify(IzatDwellNotify dwellNotify) {
            mDwellNotify = dwellNotify;
            mUpdatedFieldsMask |= GeofenceData.DWELL_NOTIFY_IS_SET;
        }
        /**
         * Gets the dwell notification setting.
         *
         * @return The current dwell notification of the geofence.
         */
        public IzatDwellNotify getDwellNotify() {
            return mDwellNotify;
        }
    }

    public enum IzatGeofenceTransitionTypes {
        UNKNOWN(0), ENTERED_ONLY(1), EXITED_ONLY(2), ENTERED_AND_EXITED(3);

        private final int value;
        private IzatGeofenceTransitionTypes(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }

    /** Geofence confidence levels. */
    public enum IzatGeofenceConfidence {
        /**
         * Low confidence: Lower power usage, but it can impact the yield
         * because incorrect transition events may be sent.
         */
        LOW(1),
        /**
         * Medium confidence.
         */
        MEDIUM(2),
        /**
         * High confidence: This setting results in higher power usage but the
         * best transition events accuracy.
         */
        HIGH(3);

        private final int value;
        private IzatGeofenceConfidence(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }

 /** @} */ /* end_addtogroup IZatGeofenceService */

 /** @addtogroup IZatGeofenceService
     @{ */

    /**
     * class IzatDwellNotify.
     */
    public class IzatDwellNotify {
        public static final int DWELL_TYPE_INSIDE_MASK = 1;
        public static final int DWELL_TYPE_OUTSIDE_MASK = 2;
        int mDwellTime;
        int mDwellType;

        /**
         * Creates a geofence dwell notification.
         *
         * @param dwellTime Indicates the time in seconds that a
         *         user spends in the geofence area before a dwell
         *         event is sent.
         * @param dwellType Indicates in which area the application is
         *         interested. \n
         *         Possible values (one or both): \n
         *         {@link #DWELL_TYPE_INSIDE_MASK}, \n
         *         {@link #DWELL_TYPE_OUTSIDE_MASK}
         * @return
         * None.
         */
        public IzatDwellNotify(int dwellTime, int dwellType) {
            mDwellTime = dwellTime;
            mDwellType = dwellType;
        }
        /**
         * Gets the dwell time.
         *
         * @return The current dwell time of the dwell notification.
         */
        public int getDwellTime() {
            return mDwellTime;
        }
        /**
         * Gets the dwell type.
         *
         * @return The current dwell type of the dwell notification.
         */
        public int getDwellType() {
            return mDwellType;
        }
    }
}
    /** @} */  /* end_addtogroup IZatGeofenceService */