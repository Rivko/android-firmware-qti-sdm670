/* ======================================================================
*  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.location.Location;

/** @addtogroup IZatFlpService
    @{ */

/**
 * The IZatFlpService provides the interface for interacting with the Qualcomm Location FLP service.
 *
 * <pre>
 * <code>
 *
 *  // Sample Code
 *
 *  import android.app.Activity;
 *  import android.os.Bundle;
 *  import android.view.View;
 *  import android.widget.Button;
 *  import com.qti.location.sdk.IZatManager;
 *  import com.qti.location.sdk.IZatFlpService;
 *
 *  public class FullscreenActivity extends Activity {
 *
 *     private IZatManager mIzatMgr = null;
 *     private IZatFlpService mFlpSevice = null;
 *     // session handle that will be assigned after the session starts.
 *     private IZatFlpService.IZatFlpSessionHandle mHandle = null;
 *
 *     @Override
 *     protected void onCreate(Bundle savedInstanceState) {
 *
 *          ...
 *          // get the instance of IZatManager
 *          mIzatMgr = IZatManager.getInstance(getApplicationContext());
 *
 *          ...
 *          // create a callback object used to receive the location objects.
 *          final LocationCallback locationCb = new LocationCallback();
 *
 *          ...
 *          final Button connectButton = (Button)findViewById(R.id.connectButton);
 *          connectButton.setOnClickListener(new View.OnClickListener() {
 *              @Override
 *              public void onClick(View v) {
 *
 *                  // connect to IZatFlpService through IZatManager
 *                  if (mIzatMgr != null) {
 *                      mFlpSevice = mIzatMgr.connectFlpService();
 *                  }
 *              }
 *          });
 *
 *          ...
 *          final Button startButton = (Button)findViewById(R.id.startButton);
 *          startButton.setOnClickListener(new View.OnClickListener() {
 *              @Override
 *              public void onClick(View v) {
 *
 *                  // start an FLP position session with time interval and
 *                  // distance interval
 *                  if (mFlpSevice != null) {
 *
 *                      // create a batching request object
 *                      IZatFlpService.IzatFlpRequest request =
 *                          IZatFlpService.IzatFlpRequest.getBackgroundFlprequest();
 *                      request.setTimeInterval(3000); // in milliseconds
 *                      request.setDistanceInterval(1); // in meters
 *
 *                      // start the session.
 *                      mHandle = mFlpSevice.startFlpSession(locationCb, request);
 *                  }
 *              }
 *          });
 *      }
 *  }
 *
 *  public class LocationCallback implements IZatFlpService.IFlpLocationCallback {
 *      public void onLocationAvailable(Location[] locations) {
 *          ...
 *      }
 *  }
 *
 * </code>
 * </pre>
 */

public interface IZatFlpService {
    /**
     * Starts a fused location positioning session.
     *
     * This function enables applications to start a fused location positioning session,
     * which can be configured as a location background session or a location
     * foreground session. A location background session is able to batch
     * locations in the hardware when the device is asleep. A location
     * foreground session navigates with fused locations.
     *
     * @param callback Callback to receive locations.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @param flpRequest Location request for the updates.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException One or more parameters are NULL.
     * @return
     * The handle that binds with the session.
     */
    IZatFlpSessionHandle startFlpSession(IFlpLocationCallback callback,
                                         IzatFlpRequest flpRequest)
                                         throws IZatIllegalArgumentException;

    /**
     * Stops a fused location positioning session.
     *
     * This function enables applications to stop a running fused location positioning
     * session.
     *
     * @param handler Handle returned by {@link #startFlpSession}.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException The parameter is NULL
     * @return
     * None.
     */
    void stopFlpSession(IZatFlpSessionHandle handler)
                        throws IZatIllegalArgumentException;

    /**
     * Starts listening for passive locations.
     *
     * This function enables applications to passively get locations when the user did not
     * start an FLP session, saving power. A passive client gets location updates
     * via callback only when there are updates for other active clients, either
     * for a foreground or a background session.
     *
     * @param callback Callback to receive locations.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException The parameter is NULL.
     * @return
     * None.
     */
    void registerForPassiveLocations(IFlpLocationCallback callback)
                                     throws IZatIllegalArgumentException;

    /**
     * Stops listening for passive locations.
     *
     * This function enables applications to stop listening for passive locations.
     * If the callback is not registered, this function
     * just returns.
     *
     * @param callback Callback to receive locations.
     *         This parameter cannot be NULL, otherwise a
     *         {@link IZatIllegalArgumentException} is thrown.
     * @throws IZatIllegalArgumentException The parameter is NULL.
     * @return
     * None.
     */
    void deregisterForPassiveLocations(IFlpLocationCallback callback)
                                       throws IZatIllegalArgumentException;

/** @} */  /* end_addtogroup IZatFlpService */

/** @addtogroup IZatFlpService
    @{ */

    /**
     * Interface class IZatFlpSessionHandle.
     *
     * IZatFlpSessionHandle is the interface used to manipulate the ongoing FLP session.
     *
     * <pre>
     * <code>
     *
     *  // Sample Code
     *
     *  import android.app.Activity;
     *  import android.os.Bundle;
     *  import android.view.View;
     *  import android.widget.Button;
     *  import com.qti.location.sdk.IZatManager;
     *  import com.qti.location.sdk.IZatFlpService;
     *
     *  public class FullscreenActivity extends Activity {
     *
     *     ...
     *     // session handle that will be assigned after the session starts.
     *     private IZatFlpService.IZatFlpSessionHandle mHandle = null;
     *
     *     @Override
     *     protected void onCreate(Bundle savedInstanceState) {
     *
     *          ...
     *          final Button startButton = (Button)findViewById(R.id.startButton);
     *          startButton.setOnClickListener(new View.OnClickListener() {
     *              @Override
     *              public void onClick(View v) {
     *                 ...
     *                 // start the session.
     *                 mHandle = mFlpSevice.startFlpSession(locationCb, request);
     *              }
     *          });
     *
     *          ...
     *          final Button queryButton = (Button)findViewById(R.id.queryButton);
     *          queryButton.setOnClickListener(new View.OnClickListener() {
     *              @Override
     *              public void onClick(View v) {
     *                  ...
     *                  if (mHandle != null) {
     *                      // pull out all available locations, and the result will be
     *                      // returned through the session callback.
     *                      mHandle.pullLocations();
     *                  }
     *              }
     *          });
     *
     *          ...
     *          final Button setToBackgroundButton =
     *              (Button)findViewById(R.id.setToBackgroundButton);
     *          setToBackgroundButton.setOnClickListener(new View.OnClickListener() {
     *              @Override
     *              public void onClick(View v) {
     *                  ...
     *                  if (mHandle != null) {
     *                      // set the current FLP session to background
     *                      mHandle.setToBackground();
     *                  }
     *              }
     *          });
     *      }
     *  }
     *
     *  public class LocationCallback implements IZatFlpService.IFlpLocationCallback {
     *      public void onLocationAvailable(Location[] locations) {
     *          ...
     *      }
     *  }
     *
     * </code>
     * </pre>
     */

    interface IZatFlpSessionHandle {
        /**
         * Requests recent locations.
         *
         * This function enables applications to pull out all the available locations saved
         * in the device buffer. The locations returned are subsequently
         * deleted from the buffer.
         *
         * If the FLP session associated with this handle has been stopped,
         * the handle is no longer valid and no fix is returned
         * by calling this method with the invalidated handle.
         *
         */
        void pullLocations();

        /**
         * Makes the current FLP session run in the foreground.
         *
         * This function enables applications to make the current FLP session run in
         * the foreground, which performs navigation only. In this case, applications
         * get a callback on every fix. This consumes more power than running in the background.
         */
        void setToForeground();

        /**
         * Makes the current FLP session run in the background.
         *
         * This function enables applications to make the current FLP session run in
         * the background, which performs location batching only. In this case,
         * applications get a callback when the batching buffer is full.
         * This consumes less power than running in the foreground.
         */
        void setToBackground();

        /**
         * Makes the current FLP session batch fixes for a trip distance.
         *
         * This function enables applications to make the current FLP session run in
         * the background for the entire distance of the specified trip.
         * Locations are batched at 1 sec TBF by default, unless the time interval
         * is also specified. Applications get a callback for batched locations
         * only after the trip distance is covered or the batch buffer is full,
         * whichever occurs first. On completion of the trip, applications receive
         * a {@link com.qti.location.sdk.IZatFlpService.IFlpStatusCallback#onBatchingStatusCb(
         * com.qti.location.sdk.IZatFlpService.IzatFlpStatus)}
         * callback with a
         * {@link com.qti.location.sdk.IZatFlpService.IzatFlpStatus#OUTDOOR_TRIP_COMPLETED}
         * status. Compared to background mode, this gives better accuracy.
         *
        */
        void setToTripMode()
                throws IZatFeatureNotSupportedException;

        /**
         * Starts listening for batch status notifications.
         *
         * This function enables applications to get status notifications on their batching requests.
         *
         * @param callback Callback to receive status notifications.
         *         This parameter cannot be NULL, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException The callback parameter is NULL.
         * @return
         * None.
         */

        void registerForSessionStatus(IFlpStatusCallback callback)
                throws IZatIllegalArgumentException, IZatFeatureNotSupportedException;

        /**
         * Stops listening for batching status notifications.
         *
         * This function enables applications to stop listening for status notifications.
         * If a callback was not registered, this function
         * just returns.
         * @throws IZatFeatureNotSupportedException The feature is not supported.
         * @return
         * None.
         */
        void unregisterForSessionStatus()
                throws IZatFeatureNotSupportedException;
    }

/** @} */  /* end_addtogroup IZatFlpService */

/** @addtogroup IZatFlpService
    @{ */

    /**
     * Interface class IFlpLocationCallback.
     *
     * IFlpLocationCallback is the interface for receiving locations.
     *
     * <pre>
     * <code>
     *
     *  // Sample Code
     *
     *  import android.app.Activity;
     *  import android.os.Bundle;
     *  import android.view.View;
     *  import android.widget.Button;
     *  import android.util.Log;
     *  import com.qti.location.sdk.IZatManager;
     *  import com.qti.location.sdk.IZatFlpService;
     *
     *  public class FullscreenActivity extends Activity {
     *
     *      ...
     *      // create a callback object used to receive the location objects.
     *      final LocationCallback locationCb = new LocationCallback();
     *
     *      ...
     *      final Button startButton = (Button)findViewById(R.id.startButton);
     *      startButton.setOnClickListener(new View.OnClickListener() {
     *          @Override
     *          public void onClick(View v) {
     *             ...
     *             // start the session.
     *             mHandle = mFlpSevice.startFlpSession(locationCb, request);
     *          }
     *      });
     *  }
     *
     *  public class LocationCallback implements IZatFlpService.IFlpLocationCallback {
     *      public void onLocationAvailable(Location[] locations) {
     *          for (int i =0; i < locations.length; i++) {
     *              if (location[i] != null) {
     *                 Log.d("IzatSDK",
     *                       "Position measurement: " + location[i].toString());
     *              }
     *          }
     *      }
     *  }
     *
     * </code>
     * </pre>
     */
    interface IFlpLocationCallback {
        /**
         * Location callback.
         *
         * This API is called by the underlying service back
         * to applications when locations are available.
         * Applications should implement this interface.
         *
         * @param locations Array of available locations.
         * @return
         * None.
         */
        void onLocationAvailable(Location[] locations);
    }

/** @} */  /* end_addtogroup IZatFlpService */

/** @addtogroup IZatFlpService
    @{ */

    /**
     * Class IzatFlpStatus.
     *
     * IzatFlpStatus informs the application of the status of ongoing
     * background FLP sessions. Applications must register to receive
     * the status using
     * {@link com.qti.location.sdk.IZatFlpService.IZatFlpSessionHandle#registerForSessionStatus(
     * com.qti.location.sdk.IZatFlpService.IFlpStatusCallback)}
     */
    public enum IzatFlpStatus {
        /**
         * Applications running an FLP session in
         * {@link com.qti.location.sdk.IZatFlpService.IzatFlpBgRequest.IzatFlpBgRequestMode#TRIP_MODE TRIP_MODE}
         * will receive an OUTDOOR_TRIP_COMPLETED status once the trip distance is covered.
         */
        OUTDOOR_TRIP_COMPLETED(0),

        /**
         * Applications will receive a POSITION_AVAILABLE status if the underlying
         * engine is able to generate any new position fixes after being stuck in a
         * {@link com.qti.location.sdk.IZatFlpService.IzatFlpStatus#POSITION_NOT_AVAILABLE}
         * state for some time.
         */
        POSITION_AVAILABLE(1),

        /**
         * Applications will receive a POSITION_NOT_AVAILABLE status once if the underlying engine
         * is not able to generate any new position fixes. Once new positions are available,
         * the underlying engine will send a
         * {@link com.qti.location.sdk.IZatFlpService.IzatFlpStatus#POSITION_AVAILABLE}
         * status. Applications may choose to stop the FLP session at this time if power
         * is a concern, since the underlying engine will continue its attempt to make a position fix.
        */
        POSITION_NOT_AVAILABLE(2);

        private final int value;
        private IzatFlpStatus(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }

/** @} */  /* end_addtogroup IZatFlpService */

/** @addtogroup IZatFlpService
    @{ */

        /**
         * Class IFlpStatusCallback.
         */
    interface IFlpStatusCallback {

        /**
         * Engine status callback.
         *
         * This function is called by the underlying service back
         * to applications when the batching engine reports its status.
         *
         * @param status Batching status sent back from the engine,
         * which must be one these types
         * {@link com.qti.location.sdk.IZatFlpService.IzatFlpStatus IZatFlpStatus}.
         * @return
         * None.
         */
        void onBatchingStatusCb(IzatFlpStatus status);
    }

/** @} */  /* end_addtogroup IZatFlpService */

/** @addtogroup IZatFlpService
    @{ */

    /**
     * Class IzatFlpRequest.
     */
    public abstract class IzatFlpRequest {
        long mMaxTimeInterval = 0;
        int mMaxDistanceInterval = 0;
        long mTripDistance = 0;
        protected boolean mIsRunningInBackground = false;

        /**
         * Creates an FLP foreground request.
         *
         * This function creates an FLP request running in the foreground, which
         * performs navigation only. With this request, applications
         * get a callback on every fix.
         *
         * @return
         * A new IZatFlpFgRequest.
         */
        public static IzatFlpFgRequest getForegroundFlprequest() {
            return new IzatFlpFgRequest();
        }

        /**
         * Creates an FLP background request.
         *
         * This function creates an FLP request running in the background, which
         * performs location batching only. With this request, applications
         * may get a callback when the batching buffer is full.
         *
         * @return
         * A new IZatFlpBgRequest.
         */
        public static IzatFlpBgRequest getBackgroundFlprequest() {
            return new IzatFlpBgRequest();
        }

        /**
         * Sets the time interval, in milliseconds.
         *
         * This function enables applications to set the time interval in the FLP
         * request. The time interval is optional when distance of displacement is set.
         * The time interval set through this API is the maximum time interval, which means
         * locations could be reported with a smaller time interval if there is another
         * session set to a smaller time interval.
         *
         * @param maxTimeInterval Time interval in milliseconds,
         *         which must be greater than zero, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException Time interval is less than or equal to zero.
         * @return
         * None.
         */
        public void setTimeInterval(long maxTimeInterval)
            throws IZatIllegalArgumentException {
            if (maxTimeInterval <= 0)
                throw new IZatIllegalArgumentException("invalid time interval");

            mMaxTimeInterval = maxTimeInterval;
        }

        /**
         * Gets the time interval that has been set.
         *
         * This function enables applications to get the time interval that has
         * been set in the FLP request. It returns 0 if the time interval
         * is not set.
         *
         * @return The time interval in milliseconds.
         */
        public long getTimeInterval() {
            return mMaxTimeInterval;
        }

        /**
         * Sets the distance of displacement, in meters.
         *
         * This function enables applications to set the distance of displacement in the FLP
         * request.
         * The distance interval is optional when the time interval is set. The distance interval
         * set through this API is the maximum distance interval, which means locations
         * could be reported with a smaller distance interval if there is another
         * session set to a smaller distance interval.
         *
         * @param maxDistanceInterval Distance of displacement in meters,
         *         which must be greater than zero, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException Distance of displacement is less than or equal to
         *                                      zero.
         * @return
         * None.
         */
        public void setDistanceInterval(int maxDistanceInterval)
            throws IZatIllegalArgumentException {
            if (maxDistanceInterval <= 0)
                throw new IZatIllegalArgumentException("invalid distance of displacement");
            mMaxDistanceInterval = maxDistanceInterval;
        }

        /**
         * Gets the distance of displacement that has been set.
         *
         * This function enables applications to get the distance of displacement that has
         * been set in the FLP request. It returns 0 if the distance of displacement
         * is not set.
         *
         * @return The distance of displacement in meters.
         */
        public int getDistanceInterval() {
            return mMaxDistanceInterval;
        }

        /**
         * Sets the trip distance in meters.
         *
         * This function enables applications to set the trip distance.
         * Trip distance is applicable only if the request mode is
         * {@link com.qti.location.sdk.IZatFlpService.IzatFlpBgRequest.IzatFlpBgRequestMode
         * #TRIP_MODE TRIP_MODE}.
         *
         * @param tripDistance Trip distance in meters,
         *         which must be greater than zero, otherwise a
         *         {@link IZatIllegalArgumentException} is thrown.
         * @throws IZatIllegalArgumentException The trip distance is not greater than zero.
         * @return
         * None.
         */
        public void setTripDistance(long tripDistance)
                throws IZatIllegalArgumentException {
            if (tripDistance <= 0)
                throw new IZatIllegalArgumentException("invalid trip distance");
            mTripDistance = tripDistance;
        }

        /**
         * Gets the trip distance that has been set.
         *
         * This function enables applications to get the trip distance that
         * was set in the FLP request. It returns 0 if the distance is not set.
         *
         * @return The distance of the trip.
         */
        public long getTripDistance() {
            return mTripDistance;
        }
    }

    class IzatFlpFgRequest extends IzatFlpRequest {
        IzatFlpFgRequest() {
            super.mIsRunningInBackground = false;
        }
    }

/** @} */  /* end_addtogroup IZatFlpService */

/** @addtogroup IZatFlpService
    @{ */

    class IzatFlpBgRequest extends IzatFlpRequest {

        /**
         * Class IzatFlpBgRequestMode, which
         * defines the background batching modes.
         */
        public enum IzatFlpBgRequestMode {
            /**
            * Routine mode batches positions based on Time Interval only, Distance Only,
            * or Time Interval and Distance combined, and it is more power conservative,
            * with the tradeoff of potentially giving lower accuracy.
            * By default, a background request is created in this mode.
            */
            ROUTINE_MODE(0),

            /**
            * Trip mode batches positions for a trip distance at 1 Hz (by default, unless
            * a time interval is specified) and it prioritizes accuracy over power.
            */
            TRIP_MODE(1);

            private final int value;
            private IzatFlpBgRequestMode(int value) {
                this.value = value;
            }
            public int getValue() {
                return value;
            }
        }

        private IzatFlpBgRequestMode mActiveMode;

        IzatFlpBgRequest() {
            super.mIsRunningInBackground = true;
            mActiveMode = IzatFlpBgRequestMode.ROUTINE_MODE;
        }

        public void setActiveMode(IzatFlpBgRequestMode mode) {
            mActiveMode = mode;
        }

        public IzatFlpBgRequestMode getActiveMode() {
            return mActiveMode;
        }
    }
}
/** @} */  /* end_addtogroup IZatFlpService */