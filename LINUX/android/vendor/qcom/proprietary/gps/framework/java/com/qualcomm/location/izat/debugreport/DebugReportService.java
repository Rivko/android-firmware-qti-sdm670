/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.debugreport;

import android.content.Context;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.Bundle;

import android.util.Log;
import java.lang.IndexOutOfBoundsException;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import com.qti.debugreport.*;
import com.qualcomm.location.izat.CallbackData;
import com.qualcomm.location.izat.DataPerPackageAndUser;

public class DebugReportService {
    private static final String TAG = "DebugReportService";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);

    private static final Object sCallBacksLock = new Object();
    private RemoteCallbackList<IDebugReportCallback> mDebugReportCallbacks
        = new RemoteCallbackList<IDebugReportCallback>();
    private final Context mContext;
    Timer mDebugReportTimer;

    private class ClientDebugReportData extends CallbackData {
        private IDebugReportCallback mCallback;
        private boolean mReportPeriodic;

        public ClientDebugReportData(IDebugReportCallback callback) {
            mCallback = callback;
            super.mCallback = callback;
            mReportPeriodic = false;
        }
    }

    private DataPerPackageAndUser<ClientDebugReportData> mDataPerPackageAndUser;

    // DebugReport Data classes
    ArrayList<IZatEphmerisDebugReport> mListOfEphmerisReports =
            new ArrayList<IZatEphmerisDebugReport>();
    ArrayList<IZatFixStatusDebugReport> mListOfFixStatusReports =
            new ArrayList<IZatFixStatusDebugReport>();
    ArrayList<IZatLocationReport> mListOfBestLocationReports = new ArrayList<IZatLocationReport>();
    ArrayList<IZatLocationReport> mListOfEPIReports = new ArrayList<IZatLocationReport>();
    ArrayList<IZatGpsTimeDebugReport> mListGpsTimeOfReports =
            new ArrayList<IZatGpsTimeDebugReport>();
    ArrayList<IZatXoStateDebugReport> mListXoStateOfReports =
            new ArrayList<IZatXoStateDebugReport>();
    ArrayList<IZatRfStateDebugReport> mListRfStateOfReports =
            new ArrayList<IZatRfStateDebugReport>();
    ArrayList<IZatErrorRecoveryReport> mListOfErrorRecoveries =
            new ArrayList<IZatErrorRecoveryReport>();
    ArrayList<IZatPDRDebugReport> mListOfPDRReports = new ArrayList<IZatPDRDebugReport>();
    ArrayList<IZatSVHealthDebugReport> mListOfSVHealthReports =
            new ArrayList<IZatSVHealthDebugReport>();
    ArrayList<IZatXTRADebugReport> mListOfXTRAReports = new ArrayList<IZatXTRADebugReport>();

    public static DebugReportService sInstance = null;
    public static DebugReportService getInstance(Context ctx) {
        if (sInstance == null) {
            sInstance = new DebugReportService(ctx);
        }
        return sInstance;
    }

    public DebugReportService(Context ctx) {
        if (VERBOSE) {
            Log.d(TAG, "DebugReportService construction");
        }

        mContext = ctx;
        mDataPerPackageAndUser = new DataPerPackageAndUser<ClientDebugReportData>(mContext,
                new UserChanged());

        native_debugreport_init();
    }

    /* Remote binder */
    private final IDebugReportService.Stub mBinder = new IDebugReportService.Stub() {

        public void registerForDebugReporting(final IDebugReportCallback callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }

            synchronized (sCallBacksLock) {
                if (VERBOSE) {
                    Log.d(TAG, "registerForDebugReporting: " +
                            mDataPerPackageAndUser.getPackageName(null));
                }


                ClientDebugReportData clData = mDataPerPackageAndUser.getData();
                if (null == clData) {
                    clData = new ClientDebugReportData(callback);
                    mDataPerPackageAndUser.setData(clData);
                } else {
                    if (null != clData.mCallback) {
                        mDebugReportCallbacks.unregister(clData.mCallback);
                    }
                    clData.mCallback = callback;
                }

                mDebugReportCallbacks.register(callback);
            }

            try {
                callback.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                    @Override
                    public void binderDied() {
                        synchronized (sCallBacksLock) {
                            ClientDebugReportData clData =
                                    mDataPerPackageAndUser.getDataByCallback(callback);

                            if (null != clData) {
                                if (VERBOSE) {
                                    Log.d(TAG, "Package died: " +
                                            clData.mPackageName);
                                }
                            }

                            mDebugReportCallbacks.unregister(callback);
                            checkOnPeriodicReporting();
                        }
                    }
                }, 0);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed unregister debug report cb", e);
            }
        }

        public void unregisterForDebugReporting(IDebugReportCallback callback) {
            if (callback == null) {
                Log.e(TAG, "callback is null");
                return;
            }

            synchronized (sCallBacksLock) {
                 if (VERBOSE) {
                    Log.d(TAG, "unregisterForDebugReporting: " +
                            mDataPerPackageAndUser.getPackageName(null));
                }

                mDataPerPackageAndUser.removeData(mDataPerPackageAndUser.getData());
                mDebugReportCallbacks.unregister(callback);
                checkOnPeriodicReporting();
            }
        }

        public Bundle getDebugReport() {
            if (VERBOSE) {
                Log.d(TAG, "getDebugReport: " + mDataPerPackageAndUser.getPackageName(null));
            }

            synchronized(sCallBacksLock) {
                mListOfEphmerisReports.clear();
                mListOfFixStatusReports.clear();
                mListOfEPIReports.clear();
                mListOfBestLocationReports.clear();
                mListGpsTimeOfReports.clear();
                mListXoStateOfReports.clear();
                mListRfStateOfReports.clear();
                mListOfErrorRecoveries.clear();
                mListOfPDRReports.clear();
                mListOfSVHealthReports.clear();
                mListOfXTRAReports.clear();

                native_debugreport_getReport(30, mListOfEphmerisReports,
                                                 mListOfFixStatusReports,
                                                 mListOfEPIReports,
                                                 mListOfBestLocationReports,
                                                 mListGpsTimeOfReports,
                                                 mListXoStateOfReports,
                                                 mListRfStateOfReports,
                                                 mListOfErrorRecoveries,
                                                 mListOfPDRReports,
                                                 mListOfSVHealthReports,
                                                 mListOfXTRAReports);

                Bundle bundleDebugReportObj = new Bundle();
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_EPH_STATUS_KEY,
                        mListOfEphmerisReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_FIX_STATUS_KEY,
                        mListOfFixStatusReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_EXTERNAL_POSITION_INJECTION_KEY,
                        mListOfEPIReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_BEST_POSITION_KEY,
                        mListOfBestLocationReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_GPS_TIME_KEY,
                        mListGpsTimeOfReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_RF_STATE_KEY,
                        mListRfStateOfReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_XO_STATE_KEY,
                        mListXoStateOfReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_LAST_ERROR_RECOVERIES_KEY,
                        mListOfErrorRecoveries);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_PDR_INFO_KEY,
                        mListOfPDRReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_SV_HEALTH_KEY,
                        mListOfSVHealthReports);
                bundleDebugReportObj.putParcelableArrayList(
                        IZatDebugConstants.IZAT_DEBUG_XTRA_STATUS_KEY,
                        mListOfXTRAReports);
                return bundleDebugReportObj;
            }
        }

        public void startReporting() {
            if (VERBOSE) {
                Log.d(TAG, "Request to start periodic reporting by package:"
                           + mDataPerPackageAndUser.getPackageName(null) );
            }

            // update ClientGeofenceData for this package
            synchronized(sCallBacksLock) {
                ClientDebugReportData clData = mDataPerPackageAndUser.getData();
                if (null != clData) {
                    clData.mReportPeriodic = true;
                }
            }

            if (mDebugReportTimer != null) {
                if (VERBOSE) {
                    Log.d(TAG, "Periodic reporting already in progress");
                }
                return;
            }

            mDebugReportTimer = new Timer();
            mDebugReportTimer.scheduleAtFixedRate(new TimerTask() {
                @Override
                public void run() {
                    synchronized (sCallBacksLock) {
                        mListOfEphmerisReports.clear();
                        mListOfFixStatusReports.clear();
                        mListOfEPIReports.clear();
                        mListOfBestLocationReports.clear();
                        mListGpsTimeOfReports.clear();
                        mListXoStateOfReports.clear();
                        mListRfStateOfReports.clear();
                        mListOfErrorRecoveries.clear();
                        mListOfPDRReports.clear();
                        mListOfSVHealthReports.clear();
                        mListOfXTRAReports.clear();

                        native_debugreport_getReport(1, mListOfEphmerisReports,
                                mListOfFixStatusReports,
                                mListOfEPIReports,
                                mListOfBestLocationReports,
                                mListGpsTimeOfReports,
                                mListXoStateOfReports,
                                mListRfStateOfReports,
                                mListOfErrorRecoveries,
                                mListOfPDRReports,
                                mListOfSVHealthReports,
                                mListOfXTRAReports);

                        if (mListOfEphmerisReports.isEmpty() &&
                                mListOfFixStatusReports.isEmpty() &&
                                mListOfEPIReports.isEmpty() &&
                                mListOfBestLocationReports.isEmpty() &&
                                mListGpsTimeOfReports.isEmpty() &&
                                mListXoStateOfReports.isEmpty() &&
                                mListRfStateOfReports.isEmpty() &&
                                mListOfErrorRecoveries.isEmpty() &&
                                mListOfPDRReports.isEmpty() &&
                                mListOfSVHealthReports.isEmpty() &&
                                mListOfXTRAReports.isEmpty()) {
                            if (VERBOSE) {
                                Log.d(TAG, "Empty debug report");
                            }
                            return;
                        }

                        Bundle bundleDebugReportObj = new Bundle();

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_EPH_STATUS_KEY,
                                    mListOfEphmerisReports.get(0) );
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_FIX_STATUS_KEY,
                                    mListOfFixStatusReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_EXTERNAL_POSITION_INJECTION_KEY,
                                    mListOfEPIReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_BEST_POSITION_KEY,
                                    mListOfBestLocationReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_GPS_TIME_KEY,
                                    mListGpsTimeOfReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_RF_STATE_KEY,
                                    mListRfStateOfReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_XO_STATE_KEY,
                                    mListXoStateOfReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_LAST_ERROR_RECOVERIES_KEY,
                                    mListOfErrorRecoveries.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_PDR_INFO_KEY,
                                    mListOfPDRReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_SV_HEALTH_KEY,
                                    mListOfSVHealthReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        try {
                            bundleDebugReportObj.putParcelable(
                                    IZatDebugConstants.IZAT_DEBUG_XTRA_STATUS_KEY,
                                    mListOfXTRAReports.get(0));
                        } catch (IndexOutOfBoundsException ioobe) {}

                        String ownerPackage = null;
                        for (ClientDebugReportData clData : mDataPerPackageAndUser.getAllData()) {
                            if (true == clData.mReportPeriodic) {
                                if (VERBOSE) {
                                    Log.d(TAG, "Sending report to " + clData.mPackageName);
                                }

                                try {
                                    clData.mCallback.onDebugDataAvailable(
                                            bundleDebugReportObj);
                                } catch (RemoteException e) {
                                             // do nothing
                                }
                            }
                        }
                    }
                }}, 0, 1000);
            }

        public void stopReporting() {
            if (VERBOSE) {
                Log.d(TAG, "Request to stop periodic reporting by package:"
                           + mDataPerPackageAndUser.getPackageName(null));
            }

            // update ClientGeofenceData for this package
            synchronized (sCallBacksLock) {
                ClientDebugReportData clData = mDataPerPackageAndUser.getData();
                if (null != clData) {
                    clData.mReportPeriodic = false;
                }

                checkOnPeriodicReporting();
            }
        }
    };

    private void checkOnPeriodicReporting() {
        boolean continuePeriodicReporting = false;

        if (mDebugReportTimer == null) {
            if (VERBOSE) {
                Log.d(TAG, "No peridoc reporting in progress !!");
            }
            return;
        }

        for (ClientDebugReportData clData : mDataPerPackageAndUser.getAllData()) {
            if (clData.mReportPeriodic == true) {
                continuePeriodicReporting = true;
                break;
            }
        }

        if (continuePeriodicReporting == false) {
            if (VERBOSE) {
                Log.d(TAG, "Service is stopping periodic debug reports");
            }

            mDebugReportTimer.cancel();
            mDebugReportTimer = null;
        }
    }

    class UserChanged implements DataPerPackageAndUser.UserChangeListener<ClientDebugReportData> {
        @Override
        public void onUserChange(Map<String, ClientDebugReportData> prevUserData,
                                 Map<String, ClientDebugReportData> currentUserData) {
            if (VERBOSE) {
                Log.d(TAG, "Active user has changed, updating debugReport callbacks...");
            }

            synchronized (sCallBacksLock) {
                // Remove prevUser callbacks
                for (ClientDebugReportData debugReportDataData: prevUserData.values()) {
                    mDebugReportCallbacks.unregister(debugReportDataData.mCallback);
                }

                // Add back current user callbacks
                for (ClientDebugReportData debugReportDataData: currentUserData.values()) {
                    mDebugReportCallbacks.register(debugReportDataData.mCallback);
                }

                checkOnPeriodicReporting();
            }
        }
    }

    public IDebugReportService getDebugReportBinder() {
        return mBinder;
    }

    static {
        native_debugreport_class_init();
    }

    private static native void native_debugreport_class_init();
    private native void native_debugreport_init();
    private native void native_debugreport_deinit();
    private native void native_debugreport_getReport(
            int depth,
            List<IZatEphmerisDebugReport> ephmerisReports,
            List<IZatFixStatusDebugReport> fixStatusReports,
            List<IZatLocationReport> epiReports,
            List<IZatLocationReport> bestLocationReports,
            List<IZatGpsTimeDebugReport> gpsTimeReports,
            List<IZatXoStateDebugReport> xoStateReports,
            List<IZatRfStateDebugReport> rfStateReports,
            List<IZatErrorRecoveryReport> errorRecoveries,
            List<IZatPDRDebugReport> pdrReports,
            List<IZatSVHealthDebugReport> svHealthReports,
            List<IZatXTRADebugReport> xtraReports);
}
