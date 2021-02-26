/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package my.tests.snapdragonsdktest;

import android.util.Log;
import android.location.Location;
import com.qti.location.sdk.IZatFlpService;

public class StatusCallback implements IZatFlpService.IFlpStatusCallback {
    private static String TAG = "StatusCallbackInApp";
    private String mSessionName;
    private static FlpActivity mFlpActivityObj;
    private static final Object sCallBacksLock = new Object();

    public StatusCallback(String sessionName, FlpActivity flpActivityObj) {
        mSessionName = sessionName;
        mFlpActivityObj = flpActivityObj;
    }

    public void onBatchingStatusCb(IZatFlpService.IzatFlpStatus status) {
        synchronized(sCallBacksLock) {
            Log.v(TAG, "Session Name = " + mSessionName +
                " Batch Status received:" + status );

            if (status == IZatFlpService.IzatFlpStatus.OUTDOOR_TRIP_COMPLETED) {
                // remove the session from the list
                if (mFlpActivityObj != null) {
                    mFlpActivityObj.clearCompletedTrip(mSessionName);
                }
            }
        }
    }
}