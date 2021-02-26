/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.server.wigig;

import android.os.HwRemoteBinder;
import android.os.RemoteException;

import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;
import android.util.Log;

import vendor.qti.hardware.wigig.netperftuner.V1_0.INetPerfTuner;
import vendor.qti.hardware.wigig.netperftuner.V1_0.NetPerfTunerStatus;
import vendor.qti.hardware.wigig.netperftuner.V1_0.NetPerfTunerStatusCode;

/**
 * Provides access to the network performance tuner service via
 * HIDL
 *
 * {@hide}
 */
public class NetPerfTunerHal {

    private static final String TAG = "NetPerfTunerHal";
    private static boolean DBG = true;

    // lock to serialize HAL operations
    private final Object mLock = new Object();

    // HAL interface objects
    private IServiceManager mIServiceManager = null;
    private INetPerfTuner mINetPerfTuner;
    private final IServiceNotification mServiceNotificationCallback =
            new IServiceNotification.Stub() {
        public void onRegistration(String fqName, String name, boolean preexisting) {
            synchronized (mLock) {
                Log.i(TAG, "IServiceNotification.onRegistration for: " + fqName
                        + ", " + name + " preexisting=" + preexisting);
                if (!initNetPerfTuner()) {
                    Log.e(TAG, "initalizing network performance tuner interface failed.");
                    netPerfTunerServiceDiedHandler();
                } else {
                    Log.i(TAG, "Completed initialization of INetPerfTuner interface.");
                }
            }
        }
    };
    private final HwRemoteBinder.DeathRecipient mServiceManagerDeathRecipient =
            cookie -> {
                Log.w(TAG, "IServiceManager died: cookie=" + cookie);
                synchronized (mLock) {
                    netPerfTunerServiceDiedHandler();
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                }
            };
    private final HwRemoteBinder.DeathRecipient mNetPerfTunerDeathRecipient =
            cookie -> {
                Log.w(TAG, "INetPerfTuner died: cookie=" + cookie);
                synchronized (mLock) {
                    netPerfTunerServiceDiedHandler();
                }
            };

    private static NetPerfTunerHal sInstance;

    public static NetPerfTunerHal getInstance() {
        if (sInstance == null) {
            sInstance = new NetPerfTunerHal();
        }
        return sInstance;
    }

    public boolean setTuningParameter(String param, String client, String value) {
        synchronized(mLock) {
            return setTuningParameterLocked(param, client, value);
        }
    }

    public boolean unsetTuningParameter(String param, String client) {
        synchronized(mLock) {
            return unsetTuningParameterLocked(param, client);
        }
    }

    private NetPerfTunerHal() {
        if (!initServiceManager()) {
            Log.e(TAG, "failed to initialize HIDL service manager");
        }
        if (!initNetPerfTuner()) {
            Log.e(TAG, "initalizing network performance tuner interface failed.");
            netPerfTunerServiceDiedHandler();
        } else {
            Log.i(TAG, "Completed initialization of INetPerfTuner interface.");
        }
    }

    void enableVerboseLogging(int verbose) {
        if (verbose > 0) {
            DBG = true;
        } else {
            DBG = false;
        }
    }

    private boolean linkToServiceManagerDeath() {
        if (mIServiceManager == null) return false;
        try {
            if (!mIServiceManager.linkToDeath(mServiceManagerDeathRecipient, 0)) {
                Log.wtf(TAG, "Error on linkToDeath on IServiceManager");
                netPerfTunerServiceDiedHandler();
                mIServiceManager = null; // Will need to register a new ServiceNotification
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "IServiceManager.linkToDeath exception", e);
            return false;
        }
        return true;
    }

    /**
     * Registers a service notification for the INetPerfTuner
     * service.
     * This will help in case wigig HAL service is killed and
     * restarted.
     * @return true if the service notification was successfully registered
     */
    private boolean initServiceManager() {
        Log.i(TAG, "Registering INetPerfTuner service ready callback.");
        synchronized (mLock) {
            mINetPerfTuner = null;
            if (mIServiceManager != null) {
                // Already have an IServiceManager and serviceNotification registered,
                // don't register another.
                return true;
            }
            try {
                mIServiceManager = IServiceManager.getService();
                if (mIServiceManager == null) {
                    Log.e(TAG, "Failed to get HIDL Service Manager");
                    return false;
                }
                if (!linkToServiceManagerDeath()) {
                    return false;
                }
                /* TODO(b/33639391) : Use the new INetPerfTuner.registerForNotifications() once it
                   exists */
                if (!mIServiceManager.registerForNotifications(
                        INetPerfTuner.kInterfaceName, "", mServiceNotificationCallback)) {
                    Log.e(TAG, "Failed to register for notifications to "
                            + INetPerfTuner.kInterfaceName);
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception while trying to register a listener for INetPerfTuner service: "
                        + e);
                netPerfTunerServiceDiedHandler();
            }
            return true;
        }
    }

    private boolean linkToNetPerfTunerDeath() {
        if (mINetPerfTuner == null) return false;
        try {
            if (!mINetPerfTuner.linkToDeath(mNetPerfTunerDeathRecipient, 0)) {
                Log.wtf(TAG, "Error on linkToDeath on INetPerfTuner");
                netPerfTunerServiceDiedHandler();
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "INetPerfTuner.linkToDeath exception", e);
            return false;
        }
        return true;
    }

    private boolean initNetPerfTuner() {
        synchronized (mLock) {
            try {
                mINetPerfTuner = INetPerfTuner.getService();
            } catch (RemoteException e) {
                Log.e(TAG, "INetPerfTuner.getService exception: " + e);
                return false;
            }
            if (mINetPerfTuner == null) {
                Log.e(TAG, "Got null INetPerfTuner service. Stopping npt HIDL startup");
                return false;
            }
            if (!linkToNetPerfTunerDeath()) {
                return false;
            }
        }
        return true;
    }

    private static class Mutable<E> {
        public E value;

        Mutable() {
            value = null;
        }

        Mutable(E value) {
            this.value = value;
        }
    }

    private void netPerfTunerServiceDiedHandler() {
        synchronized (mLock) {
            mINetPerfTuner = null;
            // TODO should we notify upper layers??
        }
    }

    private boolean setTuningParameterLocked(String param, String client, String value) {
        if (mINetPerfTuner == null) {
            Log.e(TAG, "network performance tuner service not available");
            return false;
        }
        try {
            NetPerfTunerStatus status = mINetPerfTuner.setTuningParameter(param, client, value);
            if (status.code != NetPerfTunerStatusCode.SUCCESS) {
                Log.e(TAG, "setTuningParameter call failed, code: " + status.code + " " + status.debugMessage);
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "setTuningParameter remote call failed: " + e);
            return false;
        }

        return true;
    }

    private boolean unsetTuningParameterLocked(String param, String client) {
        if (mINetPerfTuner == null) {
            Log.e(TAG, "network performance tuner service not available");
            return false;
        }
        try {
            NetPerfTunerStatus status = mINetPerfTuner.unsetTuningParameter(param, client);
            if (status.code != NetPerfTunerStatusCode.SUCCESS) {
                Log.e(TAG, "unsetTuningParameter call failed, code: " + status.code + " " + status.debugMessage);
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "unsetTuningParameter remote call failed: " + e);
            return false;
        }

        return true;
    }
}
