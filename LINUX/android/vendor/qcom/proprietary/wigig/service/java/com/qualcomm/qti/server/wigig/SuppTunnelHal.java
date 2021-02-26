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

import vendor.qti.hardware.wigig.supptunnel.V1_0.ISuppTunnel;
import vendor.qti.hardware.wigig.supptunnel.V1_0.ISuppTunnelCallback;
import vendor.qti.hardware.wigig.supptunnel.V1_0.ISuppTunnelProvider;
import vendor.qti.hardware.wigig.supptunnel.V1_0.SuppTunnelStatus;
import vendor.qti.hardware.wigig.supptunnel.V1_0.SuppTunnelStatusCode;
import vendor.qti.hardware.wigig.supptunnel.V1_0.TunnelType;

/**
 * Provides access to the supplicant tunnel service via HIDL
 *
 * {@hide}
 */
public class SuppTunnelHal {

    private static final String TAG = "SuppTunnelHal";
    private static boolean DBG = true;

    // lock to serialize HAL operations
    private final Object mLock = new Object();

    // Supplicant tunnel HAL interface objects
    private IServiceManager mIServiceManager = null;
    private ISuppTunnelProvider mISuppTunnelProvider;
    private ISuppTunnel mISuppTunnel;
    private ISuppTunnel mHostapdISuppTunnel;
    private final IServiceNotification mServiceNotificationCallback =
            new IServiceNotification.Stub() {
        public void onRegistration(String fqName, String name, boolean preexisting) {
            synchronized (mLock) {
                Log.i(TAG, "IServiceNotification.onRegistration for: " + fqName
                        + ", " + name + " preexisting=" + preexisting);
                if (!initSupplicantTunnelProvider() || !initWigigSupplicantTunnel() ||
                    !initWigigHostapdTunnel()) {
                    Log.e(TAG, "initalizing supplicant tunnel interfaces failed.");
                    supplicantTunnelServiceDiedHandler();
                } else {
                    Log.i(TAG, "Completed initialization of ISuppTunnel interfaces.");
                }
            }
        }
    };
    private final HwRemoteBinder.DeathRecipient mServiceManagerDeathRecipient =
            cookie -> {
                Log.w(TAG, "IServiceManager died: cookie=" + cookie);
                synchronized (mLock) {
                    supplicantTunnelServiceDiedHandler();
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                }
            };
    private final HwRemoteBinder.DeathRecipient mSupplicantTunnelDeathRecipient =
            cookie -> {
                Log.w(TAG, "ISuppTunnel/ISuppTunnelProvider died: cookie=" + cookie);
                synchronized (mLock) {
                    supplicantTunnelServiceDiedHandler();
                }
            };

    private static SuppTunnelHal sInstance;

    public static SuppTunnelHal getInstance() {
        if (sInstance == null) {
            sInstance = new SuppTunnelHal();
        }
        return sInstance;
    }

    public boolean startSupplicant() {
        synchronized(mLock) {
            return startSupplicantLocked(mISuppTunnel);
        }
    }

    public boolean killSupplicant() {
        synchronized(mLock) {
            return killSupplicantLocked(mISuppTunnel);
        }
    }

    // send a command to the supplicant and return reply
    public String doCommand(String cmd) {
        synchronized(mLock) {
            return doCommandLocked(mISuppTunnel, cmd);
        }
    }

    public boolean registerCallback(ISuppTunnelCallback callback) {
        synchronized(mLock) {
            return registerCallbackLocked(callback);
        }
    }

    public boolean startHostapd() {
        synchronized(mLock) {
            return startSupplicantLocked(mHostapdISuppTunnel);
        }
    }

    public boolean killHostapd() {
        synchronized(mLock) {
            return killSupplicantLocked(mHostapdISuppTunnel);
        }
    }

    // send a command to the hostapd and return reply
    public String doHostapdCommand(String cmd) {
        synchronized(mLock) {
            return doCommandLocked(mHostapdISuppTunnel, cmd);
        }
    }

    public boolean setHostapdConfigOption(String name, String value) {
        synchronized(mLock) {
            return setConfigOptionLocked(mHostapdISuppTunnel, name, value);
        }
    }

    public boolean setConfigOption(String name, String value) {
        synchronized(mLock) {
            return setConfigOptionLocked(mISuppTunnel, name, value);
        }
    }

    private SuppTunnelHal() {
        if (!initServiceManager()) {
            Log.e(TAG, "failed to initialize HIDL service manager");
        }
        if (!initSupplicantTunnelProvider() || !initWigigSupplicantTunnel() ||
            !initWigigHostapdTunnel()) {
            Log.e(TAG, "initalizing supplicant tunnel interfaces failed.");
            supplicantTunnelServiceDiedHandler();
        } else {
            Log.i(TAG, "Completed initialization of ISuppTunnel interfaces.");
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
                supplicantTunnelServiceDiedHandler();
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
     * Registers a service notification for the ISuppTunnelProvider
     * service, which also triggers intialization of the ISuppTunnel
     * interface.
     * This will help in case wigig HAL service is killed and
     * restarted.
     * @return true if the service notification was successfully registered
     */
    private boolean initServiceManager() {
        Log.i(TAG, "Registering ISuppTunnelProvider service ready callback.");
        synchronized (mLock) {
            mISuppTunnelProvider = null;
            mISuppTunnel = null;
            mHostapdISuppTunnel = null;
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
                /* TODO(b/33639391) : Use the new ISuppTunnelProvider.registerForNotifications() once it
                   exists */
                if (!mIServiceManager.registerForNotifications(
                        ISuppTunnelProvider.kInterfaceName, "", mServiceNotificationCallback)) {
                    Log.e(TAG, "Failed to register for notifications to "
                            + ISuppTunnelProvider.kInterfaceName);
                    mIServiceManager = null; // Will need to register a new ServiceNotification
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Exception while trying to register a listener for ISuppTunnelProvider service: "
                        + e);
                supplicantTunnelServiceDiedHandler();
            }
            return true;
        }
    }

    private boolean linkToSupplicantTunnelProviderDeath() {
        if (mISuppTunnelProvider == null) return false;
        try {
            if (!mISuppTunnelProvider.linkToDeath(mSupplicantTunnelDeathRecipient, 0)) {
                Log.wtf(TAG, "Error on linkToDeath on ISuppTunnelProvider");
                supplicantTunnelServiceDiedHandler();
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "ISuppTunnelProvider.linkToDeath exception", e);
            return false;
        }
        return true;
    }

    private boolean initSupplicantTunnelProvider() {
        synchronized (mLock) {
            try {
                mISuppTunnelProvider = ISuppTunnelProvider.getService();
            } catch (RemoteException e) {
                Log.e(TAG, "ISuppTunnelProvider.getService exception: " + e);
                return false;
            }
            if (mISuppTunnelProvider == null) {
                Log.e(TAG, "Got null ISuppTunnelProvider service. Stopping supplicant HIDL startup");
                return false;
            }
            if (!linkToSupplicantTunnelProviderDeath()) {
                return false;
            }
        }
        return true;
    }

    private boolean linkToSupplicantTunnelDeath(ISuppTunnel tunnel) {
        if (tunnel == null) return false;
        try {
            if (!tunnel.linkToDeath(mSupplicantTunnelDeathRecipient, 0)) {
                Log.wtf(TAG, "Error on linkToDeath on ISuppTunnel");
                supplicantTunnelServiceDiedHandler();
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "ISupplicantStaIface.linkToDeath exception", e);
            return false;
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

    private ISuppTunnel initSupplicantTunnelLocked(int type) {
        ISuppTunnel tunnelOut = null;
        Mutable<ISuppTunnel> tunnel = new Mutable<>();
        try {
            mISuppTunnelProvider.getTunnel(type,
                    (SuppTunnelStatus status,
                    ISuppTunnel tunnelRet) -> {
                if (status.code != SuppTunnelStatusCode.SUCCESS) {
                    Log.e(TAG, "Getting Supplicant Tunnel failed: " + status.code);
                    return;
                }
                tunnel.value = tunnelRet;
            });
        } catch (RemoteException e) {
            Log.e(TAG, "ISupplicant.listInterfaces exception: " + e);
            return null;
        }
        if (tunnel.value == null) {
            Log.e(TAG, "Failed to get supplicant tunnel");
            return null;
        }
        tunnelOut = ISuppTunnel.asInterface(tunnel.value.asBinder());
        if (!linkToSupplicantTunnelDeath(tunnelOut)) {
            return null;
        }
        return tunnelOut;
    }

    private boolean initWigigSupplicantTunnel() {
        synchronized(mLock) {
            mISuppTunnel = initSupplicantTunnelLocked(TunnelType.WIGIG_SUPPLICANT);
            return (mISuppTunnel != null);
        }
    }

    private boolean initWigigHostapdTunnel() {
        synchronized(mLock) {
            mHostapdISuppTunnel = initSupplicantTunnelLocked(TunnelType.WIGIG_HOSTAPD);
            return (mHostapdISuppTunnel != null);
        }
    }

    private void supplicantTunnelServiceDiedHandler() {
        synchronized (mLock) {
            mISuppTunnelProvider = null;
            mISuppTunnel = null;
            mHostapdISuppTunnel = null;
            // TODO should we notify upper layers??
        }
    }

    private boolean startSupplicantLocked(ISuppTunnel tunnel) {
        if (tunnel == null) {
            Log.e(TAG, "Supplicant tunnel service not available");
            return false;
        }
        try {
            SuppTunnelStatus status = tunnel.startSupplicant();
            if (status.code != SuppTunnelStatusCode.SUCCESS) {
                Log.e(TAG, "startSupplicant call failed, code: " + status.code + " " + status.debugMessage);
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "startSupplicant remote call failed: " + e);
            return false;
        }

        return true;
    }

    private boolean killSupplicantLocked(ISuppTunnel tunnel) {
        if (tunnel == null) {
            Log.e(TAG, "Supplicant tunnel service not available");
            return false;
        }
        try {
            SuppTunnelStatus status = tunnel.killSupplicant();
            if (status.code != SuppTunnelStatusCode.SUCCESS) {
                Log.e(TAG, "killSupplicant call failed, code: " + status.code + " " + status.debugMessage);
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "killSupplicant remote call failed: " + e);
            return false;
        }

        return true;
    }

    private String doCommandLocked(ISuppTunnel tunnel, String cmd) {
        if (tunnel == null) {
            Log.e(TAG, "Supplicant tunnel service not available");
            return "FAIL";
        }
        Mutable<String> replyOut = new Mutable<>();
        try {
            tunnel.doCommand(cmd,
                    (SuppTunnelStatus status,
                    String reply) -> {
                if (status.code != SuppTunnelStatusCode.SUCCESS) {
                    Log.e(TAG, "doCommand failed: " + status.code + " " + status.debugMessage);
                    replyOut.value = "FAIL";
                    return;
                }
                replyOut.value = reply;
            });
        } catch (RemoteException e) {
            Log.e(TAG, "doCommand remote call failed: " + e);
            return "FAIL";
        }

        return replyOut.value;
    }

    private boolean setConfigOptionLocked(ISuppTunnel tunnel, String name, String value) {
        if (tunnel == null) {
            Log.e(TAG, "Supplicant tunnel service not available");
            return false;
        }
        try {
            SuppTunnelStatus status = tunnel.setConfigOption(name, value);
            if (status.code != SuppTunnelStatusCode.SUCCESS) {
                Log.e(TAG, "setConfigOption call failed, code: " + status.code + " " + status.debugMessage);
                return false;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "setConfigOption remote call failed: " + e);
            return false;
        }

        return true;
    }

    private boolean registerCallbackLocked(ISuppTunnelCallback callback) {
        if (mISuppTunnel == null) {
            Log.e(TAG, "Supplicant tunnel service not available");
            return false;
        }
        try {
            SuppTunnelStatus status = mISuppTunnel.registerCallback(callback);
            if (status.code != SuppTunnelStatusCode.SUCCESS) {
                Log.e(TAG, "registerCallback call failed, code: " + status.code + " " + status.debugMessage);
                    return false;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "registerCallback remote call failed: " + e);
                return false;
            }

            return true;
        }
}
