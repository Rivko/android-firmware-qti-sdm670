/*
 * Copyright (c) 2017,2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigig;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;

import com.android.server.net.BaseNetworkObserver;
import android.content.Context;
import android.content.Intent;
import android.net.InterfaceConfiguration;
import android.net.LinkAddress;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.IBinder;
import android.os.INetworkManagementService;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.text.TextUtils;
import android.util.Log;

import com.qualcomm.qti.wigig.WigigManager;


public class WigigNetworkMonitor extends BaseNetworkObserver {
    private static final String TAG = "WigigNetworkMonitor";
    private static final String WIGIG_NPT_CLIENT = "wigig";
    private static final String DEFAULT_FST_DATA_INTERFACE = "bond0";

    private static final String SYSFS_NET_PREFIX = "sys/class/net/%s/";
    private static final String SYSFS_WIL6210_PREFIX = SYSFS_NET_PREFIX + "device/wil6210/";

    private static final int VR_PROFILE_RETRY_MAX = 5;
    private static final int VR_PROFILE_RETRY_WAIT_INTERVAL = 50;
    private static final int VR_PROFILE_COMMON_STA = 2;

    private Context mContext;
    private INetworkManagementService mNwService;

    // wigig network interface (wigig0)
    private String mWigigInterfaceName;
    // bonding interface in rate upgrade mode
    private String mBondInterfaceName;

    private boolean mHasNpt;
    private NetPerfTunerHal mNetPerfTunerHal;

    // network interface monitoring
    private boolean mWigigUp;
    private boolean mBondUp;
    private boolean mWigigHasIp;
    private boolean mBondHasIp;
    private boolean mWigigFstWasUp;

    // true if wigig is active and needs tuning
    private boolean mWigigActive;

    private boolean mFstEnabled;

    // describes a single tuning parameter we change
    private static class TuningParameter {
        // name of the parameter
        public String mName;
        // system property containing tuning value
        public String mSysProp;
        // default value in case system property not defined
        public String mDefValue;
        // true if this is a per-interface parameter
        public boolean mPerInterface;
        // true if we need to update bond interface when FST enabled
        // (relevant when perInterface is true)
        public boolean mUpdateBond;

        private TuningParameter(String name, String sysProp, String defValue,
                                boolean perInterface, boolean updateBond) {
            mName = name;
            mSysProp = sysProp;
            mDefValue = defValue;
            mPerInterface = perInterface;
            mUpdateBond = updateBond;
        }

        public static TuningParameter global(String name, String sysProp, String defValue) {
            return new TuningParameter(name, sysProp, defValue, false, false);
        }

        public static TuningParameter perInterface(String name, String sysProp, String defValue) {
            return new TuningParameter(name, sysProp, defValue, true, false);
        }

        public static TuningParameter perInterfaceFst(String name, String sysProp, String defValue) {
            return new TuningParameter(name, sysProp, defValue, true, true);
        }
    }

    private static TuningParameter sVRTuningParameters[] = {
        TuningParameter.perInterface("mtu", "persist.vendor.wigig.npt.mtu", "7912"),
        TuningParameter.perInterface("ipv4_arp_timeout", "persist.vendor.wigig.npt.ipv4_arp", "300000"),
        TuningParameter.perInterface("ipv6_arp_timeout", "persist.vendor.wigig.npt.ipv6_arp", "300000"),
        TuningParameter.perInterface("tx_queue_len", "persist.vendor.wigig.npt.tx_queue_len", "10")
    };

    private static TuningParameter sTuningParameters[] = {
        // comment out some parameters for which we still don't have clear measurement that
        // they help and don't have bad effects on other networks like wifi.
        TuningParameter.perInterfaceFst("rps", "persist.vendor.wigig.npt.rps", "238"),
        TuningParameter.perInterface("gro_flush_timeout", "persist.vendor.wigig.npt.gro_ft", "2000"),
        //TuningParameter.global("rmem_max", "persist.vendor.wigig.npt.rmem_max", "8388608"),
        //TuningParameter.global("rmem_default", "persist.vendor.wigig.npt.rmem_default", "1048576"),
        //TuningParameter.global("wmem_max", "persist.vendor.wigig.npt.wmem_max", "8388608"),
        //TuningParameter.global("wmem_default", "persist.vendor.wigig.npt.wmem_default", "1048576"),
        //TuningParameter.global("udp_rmem_min", "persist.vendor.wigig.npt.udp_rmem_min", "1048576"),
        //TuningParameter.global("udp_wmem_min", "persist.vendor.wigig.npt.udp_wmem_min", "1048576"),
        //TuningParameter.global("tcp_mem", "persist.vendor.wigig.npt.tcp_mem", "8388608,8388608,8388608"),
        // tcp_rmem and tcp_wmem help but currently conflict with tcpBufferSizes in android framework,
        // comment out until we have a solution
        //TuningParameter.global("tcp_rmem", "persist.vendor.wigig.npt.tcp_rmem", "1048576,6291456,8388608"),
        //TuningParameter.global("tcp_wmem", "persist.vendor.wigig.npt.tcp_wmem", "1048576,6291456,8388608"),
        //TuningParameter.global("udp_mem", "persist.vendor.wigig.npt.udp_mem", "8388608,8388608,8388608"),
        TuningParameter.global("tcp_limit_output_bytes", "persist.vendor.wigig.npt.tcp_lob", "8800000"),
        //TuningParameter.global("tcp_use_userconfig", "persist.vendor.wigig.npt.tcp_use_uc", "1"),
        //TuningParameter.global("tcp_delack_seg", "persist.vendor.wigig.npt.tcp_delack_s", "20"),
        //TuningParameter.global("tcp_window_scaling", "persist.vendor.wigig.npt.tcp_window_s", "1"),
        //TuningParameter.global("tcp_timestamps", "persist.vendor.wigig.npt.tcp_ts", "1"),
        //TuningParameter.global("tcp_sack", "persist.vendor.wigig.npt.tcp_sack", "1")
    };

    public WigigNetworkMonitor(Context context, String wigigInterfaceName) {
        Log.i(TAG, "creating monitor");
        mContext = context;
        IBinder b = ServiceManager.getService(Context.NETWORKMANAGEMENT_SERVICE);
        mNwService = INetworkManagementService.Stub.asInterface(b);

        mWigigInterfaceName = wigigInterfaceName;
        mBondInterfaceName = SystemProperties.get("persist.vendor.fst.data.interface",
                                                  DEFAULT_FST_DATA_INTERFACE);

        mHasNpt = SystemProperties.getBoolean("persist.vendor.wigig.npt.enable", false);
        Log.i(TAG, "Network performance tuner is " + (mHasNpt ? "enabled" : "disabled"));
        if (mHasNpt) {
            mNetPerfTunerHal = NetPerfTunerHal.getInstance();
        }

        try {
            mNwService.registerObserver(this);
        } catch (RemoteException e) {
            Log.e(TAG, "Couldn't register network observer: " + e.toString());
        }
    }

    private void sendRateUpgradeStateChangeBroadcast(final int state) {
        Intent intent = new Intent(WigigManager.WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(WigigManager.EXTRA_WIGIG_RATE_UPGRADE_STATE, state);
        Log.i(TAG, "sending WIGIG_RATE_UPGRADE_STATE_CHANGED_ACTION, state=" + state);
        mContext.sendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void writeSysfsNetFile(String template, String ifname, String value) {
        String fname = String.format(template, ifname);
        File f = new File(fname);

        try {
            PrintWriter pw = new PrintWriter(f);
            pw.println(value);
            pw.flush();
            pw.close();
            Log.i(TAG, "write " + fname + " value: " + value);
        } catch (FileNotFoundException e) {
            // sysfs files can return true from File.exists but throw
            // this exception, do not log anything here.
        } catch (IOException e) {
            Log.e(TAG, "fail to write " + fname + " value: " + value + " ex: " + e);
        }
    }

    private void updateThermalThrottling() {
        String tt = SystemProperties.get("persist.vendor.wigig.tt.config");
        if (TextUtils.isEmpty(tt)) {
            return;
        }

        String template = SYSFS_WIL6210_PREFIX + "thermal_throttling";

        writeSysfsNetFile(template, mWigigInterfaceName, tt);
    }

    private boolean isVREnabled() {
        return SystemProperties.getBoolean("persist.vendor.wigig.vr.enable", false);
    }

    public void updateVRProfile() {
        int i, profile;
        boolean isSet = false, vrEnable = isVREnabled();

        Log.i(TAG, "Wigig VR is " + (vrEnable ? "enabled" : "disabled"));
        if (!vrEnable)
            // the default configuration for VR is disabled.
            // When VR is disabled - no need to do any actions.
            return;

        if (mNetPerfTunerHal == null) {
            Log.i(TAG, "mNetPerfTunerHal is null");
            return;
        }

        profile = SystemProperties.getInt("persist.vendor.wigig.vr.profile", VR_PROFILE_COMMON_STA);
        for (i = 0; i < VR_PROFILE_RETRY_MAX; i++) {
            // This function is called immediately after wigig state machine loads the driver.
            // The sysfs is created at this time, and its file permissions still not finalized
            // we make several tries to write to sysfs vr_profile to avoid fails as a result
            // of file not ready (permissions).
            isSet = mNetPerfTunerHal.setTuningParameter("vr_profile", mWigigInterfaceName,
                                                        Integer.toString(profile));
            if (isSet) {
                Log.i(TAG, "Successfully set VR profile (" + profile + ")");
                break;
            }
            try {
                Thread.sleep(VR_PROFILE_RETRY_WAIT_INTERVAL);
            } catch (InterruptedException e) {
                e.printStackTrace();
                // continue
            }
        }
        if (!isSet) {
            Log.e(TAG, "fail to set VR profile (" + profile +")");
        }
    }

    @Override
    public void interfaceLinkStateChanged(String iface, boolean up) {
        boolean relevant = false;

        if (mWigigInterfaceName.equals(iface)) {
            mWigigUp = up;
            relevant = true;
            if (mBondUp) {
                if (up) {
                    mWigigFstWasUp = true;
                }
                // send FST connected indication in case associated ("running")
                InterfaceConfiguration ifcg = null;
                try {
                    ifcg = mNwService.getInterfaceConfig(iface);
                    Log.i(TAG, "interface config: " + ifcg.getFlags());
                } catch (RemoteException e) {
                    Log.e(TAG, "Couldn't get interface config: " + e.toString());
                }
                if (ifcg != null && ifcg.hasFlag("running")) {
                    sendRateUpgradeStateChangeBroadcast(WigigManager.WIGIG_RATE_UPGRADE_STATE_CONNECTED);
                } else {
                    sendRateUpgradeStateChangeBroadcast(WigigManager.WIGIG_RATE_UPGRADE_STATE_DISCONNECTED);
                }
            }
        } else if (mBondInterfaceName.equals(iface)) {
            mBondUp = up;
            relevant = true;
            if (!up) {
                mWigigFstWasUp = false;
            }
        }

        if (relevant) {
            Log.i(TAG, "interface " + iface + " is " + (up ? "up" : "down"));
            // "up" depends on IFF_LOWER_UP which for wlan interface is set only when
            // associated. For thermal throttling, ignore "up" as we need to configure wigig also when idle.
            updateThermalThrottling();
            refreshWigigTuning();
        }
    }

    @Override
    public void addressUpdated(String iface, LinkAddress address) {
        boolean relevant = false;

        if (mWigigInterfaceName.equals(iface)) {
            mWigigHasIp = true;
            relevant = true;
        } else if (mBondInterfaceName.equals(iface)) {
            mBondHasIp = true;
            relevant = true;
        }

        if (relevant) {
            Log.i(TAG, "interface " + iface + " got IP address");
            refreshWigigTuning();
        }
    }

    @Override
    public void interfaceRemoved(String iface) {
        boolean relevant = false;

        if (mWigigInterfaceName.equals(iface)) {
            mWigigUp = false;
            mWigigHasIp = false;
            relevant = true;
        } else if (mBondInterfaceName.equals(iface)) {
            mBondUp = false;
            mBondHasIp = false;
            relevant = true;
        }

        if (relevant) {
            Log.i(TAG, "interface " + iface + " was removed");
            refreshWigigTuning();
        }
    }

    @Override
    public void addressRemoved(String iface, LinkAddress address) {
        boolean relevant = false;

        if (mWigigInterfaceName.equals(iface)) {
            mWigigHasIp = false;
            relevant = true;
        } else if (mBondInterfaceName.equals(iface)) {
            mBondHasIp = false;
            relevant = true;
        }

        if (relevant) {
            Log.i(TAG, "interface " + iface + " lost IP address");
            refreshWigigTuning();
        }
    }

    private boolean isFstEnabled() {
        return (SystemProperties.getInt("persist.vendor.fst.rate.upgrade.en", 0) == 1);
    }

    private boolean isWigigActive() {
        if (!isFstEnabled()) {
            // wigig is active if wigig0 is up and has IP address
            return mWigigUp && mWigigHasIp;
        } else {
            // in FST (rate upgrade) mode, consider wigig active if:
            // - bond0 is up and has IP address
            // - wigig interface was up at least once
            return mBondUp && mBondHasIp && mWigigFstWasUp;
        }
    }

    private void refreshWigigTuning() {
        if (!mHasNpt) {
            return;
        }

        boolean isActive = isWigigActive();
        if (isActive != mWigigActive) {
            mWigigActive = isActive;
            if (mWigigActive)
                activateWigigTuning();
            else
                deactivateWigigTuning();
        }
    }

    private void setTuningList(TuningParameter list[]) {
        mFstEnabled = isFstEnabled();

        for (TuningParameter param : list) {
            String value = SystemProperties.get(param.mSysProp, param.mDefValue);

            if (param.mPerInterface) {
                setTuningParameter(param.mName, mWigigInterfaceName, value);
                if (mFstEnabled && param.mUpdateBond) {
                    setTuningParameter(param.mName, mBondInterfaceName, value);
                }
            } else {
                setTuningParameter(param.mName, WIGIG_NPT_CLIENT, value);
            }
        }
    }

    private void unsetTuningList(TuningParameter list[]) {
        for (TuningParameter param : list) {
            if (param.mPerInterface) {
                unsetTuningParameter(param.mName, mWigigInterfaceName);
                if (mFstEnabled && param.mUpdateBond) {
                    unsetTuningParameter(param.mName, mBondInterfaceName);
                }
            } else {
                unsetTuningParameter(param.mName, WIGIG_NPT_CLIENT);
            }
        }
    }

    private void activateWigigTuning() {
        Log.i(TAG, "Activating WIGIG performance tuning");
        setTuningList(sTuningParameters);

        if (isVREnabled()) {
            setTuningList(sVRTuningParameters);
        }
    }

    private void deactivateWigigTuning() {
        Log.i(TAG, "Deactivating WIGIG performance tuning");
        unsetTuningList(sTuningParameters);

        if (isVREnabled()) {
            unsetTuningList(sVRTuningParameters);
        }
    }

    private void setTuningParameter(String paramName, String clientName, String value)
    {
        if (mNetPerfTunerHal != null) {
            mNetPerfTunerHal.setTuningParameter(paramName, clientName, value);
        } else {
            Log.i(TAG, "mNetPerfTunerHal is null");
        }
    }

    private void unsetTuningParameter(String paramName, String clientName)
    {
        if (mNetPerfTunerHal != null) {
            mNetPerfTunerHal.unsetTuningParameter(paramName, clientName);
        } else {
            Log.i(TAG, "mNetPerfTunerHal is null");
        }
    }
}
