/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.annotation.TargetApi;
import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Handler;
import android.text.format.Formatter;
import android.util.Log;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;
import com.qualcomm.wfd.client.Whisperer;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;

import static com.qualcomm.wfd.client.net.ResultListener.AnySuccessResultListener;
import static java.lang.annotation.RetentionPolicy.SOURCE;

public class LanManager implements NetManagerIF, TcpCommandServer.CommandListener {
    static final String TAG = "LanManager";

    static LanManager sInstance;

    private Whisperer whisperer = Whisperer.getInstance();

    private Context mContext;
    private Inet4Address mMyAddr;
    private ConnectionInfoIF mConnectoinInfo = new ConnectionInfoIF() {
        @Override
        public boolean isGroupOwner() {
            return true;
        }

        @Override
        public String getGroupOwnerIp() {
            return mMyAddr == null? null : mMyAddr.getHostAddress();
        }
    };

    private Handler mEventHandler;
    private NsdManager mNsdManager;

    private boolean mAvailableForSession;
    private WfdEnums.WFDDeviceType mDeviceType;

    private final LanDevice mLocalDevice = new LanDevice();
    private final Collection<LanDevice> mAllPeers = new ArrayList<>() ;
    //private final Collection<LanDevice> mPeers = new ArrayList<>() ;
    private final Collection<LanDevice> mConnectedPeers = new ArrayList<>() ;

    private MyRegListener mRegListener;
    private final TcpCommandServer mTcpCommandServer = new TcpCommandServer();

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    final class MyRegListener implements NsdManager.RegistrationListener {
        private final NsdServiceInfo mServiceInfo;
        private final int mProtocolType;
        private ResultListener<NsdServiceInfo, String> mRegResultListener = ResultListener.NullListener;
        private ResultListener<NsdServiceInfo, String> mUnRegResultListener = ResultListener.NullListener;

        MyRegListener(NsdServiceInfo serviceInfo, int protocolType) {
            mServiceInfo = serviceInfo;
            mProtocolType = protocolType;
        }

        NsdServiceInfo getServiceInfo() {
            return mServiceInfo;
        }

        int getProtocolType() {
            return mProtocolType;
        }

        @Override
        public void onRegistrationFailed(NsdServiceInfo nsdServiceInfo, int i) {
            Log.e(TAG, "registration failed: " + nsdServiceInfo);
            mRegResultListener.onFailure("registration failed");
        }

        @Override
        public void onUnregistrationFailed(NsdServiceInfo nsdServiceInfo, int i) {
            Log.e(TAG, "unregistration failed: " + nsdServiceInfo);
            mUnRegResultListener.onFailure("unregistration failed");
        }

        @Override
        public void onServiceRegistered(NsdServiceInfo nsdServiceInfo) {
            Log.v(TAG, "service registered: " + nsdServiceInfo);
            mRegResultListener.onSuccess(nsdServiceInfo);
        }

        @Override
        public void onServiceUnregistered(NsdServiceInfo nsdServiceInfo) {
            Log.v(TAG, "service unregistered: " + nsdServiceInfo);
            mUnRegResultListener.onSuccess(nsdServiceInfo);
        }
    }

    private final Collection<MyDiscoveryListener> mDiscoveryListeners = new ArrayList<>(2);

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN)
    final class MyDiscoveryListener implements NsdManager.DiscoveryListener {
        final String mServiceType;
        final int mProcotolType;
        private ResultListener mStartListener = ResultListener.NullListener;
        private ResultListener mStopListener = ResultListener.NullListener;

        MyDiscoveryListener(String serviceType, int protocolType, ResultListener startListener) {
            mServiceType = serviceType;
            mProcotolType = protocolType;
            mStartListener = startListener;
        }

        String getServiceType() {
            return mServiceType;
        }

        int getProtocolType() {
            return mProcotolType;
        }

        void setStopListener(ResultListener stopListener) {
            mStopListener = stopListener;
        }

        @Override
        public void onStartDiscoveryFailed(String s, int i) {
            mStartListener.onFailure(s);
        }

        @Override
        public void onStopDiscoveryFailed(String s, int i) {
            mStopListener.onFailure(s);
        }

        @Override
        public void onDiscoveryStarted(String s) {
            mStartListener.onSuccess(s);
        }

        @Override
        public void onDiscoveryStopped(String s) {
            mStopListener.onSuccess(s);
        }

        @Override
        public void onServiceFound(NsdServiceInfo nsdServiceInfo) {
            initLocalDevice();
            addAPeer(nsdServiceInfo);
        }

        @Override
        public void onServiceLost(NsdServiceInfo nsdServiceInfo) {
            removeAPeer(nsdServiceInfo);
        }
    }

    private NsdManager.ResolveListener mResolveListener;

    final class MyResolveListener implements NsdManager.ResolveListener {
        private ResultListener mResultListener = ResultListener.NullListener;
        private final LanDevice mDevice;

        MyResolveListener(LanDevice dev, ResultListener rl) {
            mDevice = dev;
            mResultListener = rl;
        }

        @Override
        public void onResolveFailed(NsdServiceInfo nsdServiceInfo, int i) {
            mResultListener.onFailure(i);
        }

        @Override
        public void onServiceResolved(NsdServiceInfo nsdServiceInfo) {
            Log.v(TAG, "onServiceResolved: " + nsdServiceInfo);
            if (nsdServiceInfo.getHost() instanceof Inet6Address) {
                Log.v(TAG, "Ignore IPv6 address " + nsdServiceInfo.getHost());
                return;
            }
            LanDevice dev = new LanDevice(nsdServiceInfo);
            if (!mDevice.equals(dev)) {
                Log.e(TAG, "resolved device is not the same one");
                Log.e(TAG, "Original dev: " + mDevice);
                Log.e(TAG, "Resolved dev: " + dev);
                //throw new RuntimeException("resolved device is not the same one");
                return;
            }

            mDevice.updateNsdInfo(nsdServiceInfo);
            mResultListener.onSuccess(nsdServiceInfo);

            if (!filterPeer(dev)) {
                initLocalDevice();
                if (mDevice.isStatusConnected()) {
                    mEventHandler.obtainMessage(WIFI_UTIL_CONNECTION_INFO, getNetType().ordinal(), 0, mConnectoinInfo).sendToTarget();
                }
            }
        }
    }

    public LanManager(Handler handler) {
        if (sInstance != null) {
            throw new RuntimeException("Can create only one instance");
        }
        sInstance = this;
        mContext = whisperer.getActivity();
        mEventHandler = handler;
        mNsdManager = (NsdManager)mContext.getSystemService(Context.NSD_SERVICE);
        mTcpCommandServer.setListener(this);
    }

    @Override
    public void onConnectionRequestCommand(InetAddress addr, int port) {
        Log.v(TAG, "onConnectionRequestCommand");
        initLocalDevice();

        NsdServiceInfo info = new NsdServiceInfo();
        info.setHost(addr);
        info.setPort(port);
        info.setServiceName(getServiceName(WfdEnums.WFDDeviceType.SOURCE));
        info.setServiceType(getServiceType(WfdEnums.WFDDeviceType.SOURCE));
        LanDevice dev = new LanDevice(info);
        mEventHandler.obtainMessage(WIFI_UTIL_CONNECTION_REQUEST, getNetType().ordinal(), 0, dev).sendToTarget();
    }

    @Override
    public void destroy() {
        stopPeerDiscovery(ResultListener.NullListener);
        if (mTcpCommandServer.isStarted()) {
            mTcpCommandServer.stop();
        }
        mNsdManager = null;
        sInstance = null;
    }

    @Override
    public NetType getNetType() {
        return NetType.LAN;
    }

    @Override
    public void onMainActivityResume() {
        // nothing
    }

    @Override
    public void onMainActivityPause() {
        // nothing
    }

    @Override
    public void send_wfd_set(boolean isAvailableForSession, WfdEnums.WFDDeviceType type) {
        Log.v(TAG, "send_wfd_set " + isAvailableForSession + ", " + type);
        mAvailableForSession = isAvailableForSession;
        if (mDeviceType == type) {
            return;
        }

        mDeviceType = type;
        if (type == WfdEnums.WFDDeviceType.PRIMARY_SINK
                || type == WfdEnums.WFDDeviceType.SECONDARY_SINK){
            if (!mTcpCommandServer.isStarted()) {
                mTcpCommandServer.start();
            }
        } else {
            if (mTcpCommandServer.isStarted()) {
                mTcpCommandServer.stop();
            }
        }

        initLocalDevice();
        if (mRegListener != null) {
            unregisterService();
            registerService();
        }
    }

    @Override
    public void disconnect(ResultListener rl) {
        boolean connected = false;
        for (LanDevice dev: mAllPeers) {
            if (dev.isStatusConnected()) {
                connected = true;
            }
            dev.disconnect(ResultListener.NullListener);
        }
        if (connected) {
            Log.v(TAG, "send WIFI_UTIL_DISCONNECTED");
            mEventHandler.obtainMessage(WIFI_UTIL_DISCONNECTED, getNetType().ordinal(), 0).sendToTarget();
        }
        rl.onSuccess(null);
    }

    @Override
    public void setMiracastMode(int mode) {
        // nothing
    }

    @Override
    public void createGroup(ResultListener rl) {
        rl.onSuccess(null);
    }

    @Override
    public void removeGroup(ResultListener rl) {
        rl.onSuccess(null);
    }

    @Override
    public void discoveryPeers(ResultListener rl) {
        registerService();
        if (!mDiscoveryListeners.isEmpty()) {
            rl.onFailure("Already in discovery state");
            return;
        }
        mAllPeers.clear();
        mConnectedPeers.clear();

        ResultListener resultListener = new AnySuccessResultListener(rl, 3);
        discoverDeviceType(WfdEnums.WFDDeviceType.PRIMARY_SINK, resultListener);
        discoverDeviceType(WfdEnums.WFDDeviceType.SECONDARY_SINK, resultListener);
        discoverDeviceType(WfdEnums.WFDDeviceType.SOURCE, resultListener);
    }

    private void registerService() {
        if (mRegListener != null) {
            Log.e(TAG, "already registered");
            return;
        }

        int port;
        if (mDeviceType == WfdEnums.WFDDeviceType.PRIMARY_SINK
                || mDeviceType == WfdEnums.WFDDeviceType.SECONDARY_SINK) {
            port = mTcpCommandServer.getServerPort();
        } else {
            port = P2pWfdDeviceInfo.DEFAULT_SESSION_MGMT_CTRL_PORT;
        }

        NsdServiceInfo serviceInfo = new NsdServiceInfo();
        serviceInfo.setPort(port);
        serviceInfo.setServiceName(getServiceName(mDeviceType));
        serviceInfo.setServiceType(getServiceType(mDeviceType));

        mRegListener = new MyRegListener(serviceInfo, NsdManager.PROTOCOL_DNS_SD);
        Log.v(TAG, "register service " + mRegListener.getServiceInfo());
        mNsdManager.registerService(mRegListener.getServiceInfo(),
                mRegListener.getProtocolType(), mRegListener);
        initLocalDevice();
    }

    private void unregisterService() {
        if (mRegListener == null) {
            Log.e(TAG, "not registered");
            return;
        }

        mNsdManager.unregisterService(mRegListener);
        mRegListener = null;
    }

    private void discoverDeviceType(WfdEnums.WFDDeviceType devType, ResultListener rl) {
        MyDiscoveryListener listener = new MyDiscoveryListener(getServiceType(devType),
                NsdManager.PROTOCOL_DNS_SD, rl);
        mNsdManager.discoverServices(listener.getServiceType(), listener.getProtocolType(), listener);
        mDiscoveryListeners.add(listener);
    }

    @Override
    public void stopPeerDiscovery(ResultListener rl) {
        if (mDiscoveryListeners.isEmpty()) {
            rl.onFailure("Not in discovery state");
            return;
        }
        ResultListener resultListener =
                new AnySuccessResultListener(rl, mDiscoveryListeners.size());
        for (MyDiscoveryListener listener : mDiscoveryListeners) {
            listener.setStopListener(resultListener);
            mNsdManager.stopServiceDiscovery(listener);
        }
        mDiscoveryListeners.clear();
        // don't clear mDiscoveryOpResultListener here
        mAllPeers.clear();
        mConnectedPeers.clear();

        unregisterService();
    }

    void resolveService(LanDevice dev, ResultListener rl) {
        mResolveListener = new MyResolveListener(dev, rl);
        mNsdManager.resolveService(dev.getNsdInfo(), mResolveListener);
    }

    void connect(LanDevice dev, ResultListener rl) {
        rl.onSuccess(0);
        if (dev.isResolved()) {
            mEventHandler.obtainMessage(WIFI_UTIL_CONNECTION_INFO, getNetType().ordinal(), 0, mConnectoinInfo).sendToTarget();
        } else {
            resolveService(dev, ResultListener.NullListener);
        }
    }

    @Override
    public void cancelConnect(ResultListener rl) {
        rl.onSuccess(null);
    }

    @Override
    public boolean isEnabled() {
        return true;
    }

    @Override
    public boolean isConnected() {
        boolean connected = false;
        for (LanDevice dev: mAllPeers) {
            if (dev.isStatusConnected()) {
                connected = true;
            }
        }
        return connected;
    }

    @Override
    public NetDeviceIF getLocalDevice() {
        return mLocalDevice;
    }

    @Override
    public Collection<NetDeviceIF> getPeers() {
        //return Collections.unmodifiableCollection(mPeers);
        ArrayList<NetDeviceIF> peers = new ArrayList<>(mAllPeers.size());
        for (LanDevice dev: mAllPeers) {
            if (!filterPeer(dev)) {
                peers.add(dev);
            }
        }
        return peers;
    }

    private boolean filterPeer(LanDevice dev) {
        WfdEnums.WFDDeviceType thisType = mDeviceType;
        WfdEnums.WFDDeviceType devType = dev.getWfdType();
        if (devType == WfdEnums.WFDDeviceType.UNKNOWN) {
            Log.w(TAG, "filter unknown device type");
            return false;
        }

        switch (thisType) {
            case SOURCE:
                switch (devType) {
                    case PRIMARY_SINK:
                    case SECONDARY_SINK:
                        return false;
                    default:
                        return true;
                }
            case PRIMARY_SINK:
            case SECONDARY_SINK:
                switch (devType) {
                    case SOURCE:
                        return false;
                    default:
                        return true;
                }
            default:
                Log.w(TAG, "this device type unknown");
                return false;
        }
    }

    @Override
    public Collection<NetDeviceIF> getConnectedPeers() {
        return Collections.unmodifiableCollection(mConnectedPeers);
    }

    @Override
    public ConnectionInfoIF getLocalConnectionInfo() {
        return mMyAddr == null? null : mConnectoinInfo;
    }

    private void addAPeer(NsdServiceInfo nsdInfo) {
        LanDevice dev = new LanDevice(nsdInfo);

        // resolve immediately
        resolveService(dev, ResultListener.NullListener);

        for (LanDevice d: mAllPeers) {
            if (dev.equals(d)) {
                Log.w(TAG, "LanDevice already discovered");
                return;
            }
        }
        mAllPeers.add(dev);
        Log.v(TAG, "Add a peer: " + dev);
        if (!filterPeer(dev)) {
            mEventHandler.obtainMessage(WIFI_UTIL_PEERS_CHANGED, getNetType().ordinal(), 0, getPeers()).sendToTarget();
        }
    }

    private void removeAPeer(NsdServiceInfo nsdInfo) {
        LanDevice dev = new LanDevice(nsdInfo);
        for (Iterator<LanDevice> iter = mAllPeers.iterator(); iter.hasNext(); ) {
            LanDevice d = iter.next();
            if (d.equals(dev)) {
                iter.remove();
                Log.v(TAG, "Remove a peer: " + d);
                if (!filterPeer(dev)) {
                    mEventHandler.obtainMessage(WIFI_UTIL_PEERS_CHANGED, getNetType().ordinal(), 0, getPeers()).sendToTarget();
                }
                return;
            }
        }
        Log.w(TAG, "Cannot remove peer: " + dev);
    }

    private void initLocalDevice() {
        WifiManager wifiManager = (WifiManager)mContext.getSystemService(Context.WIFI_SERVICE);
        if (wifiManager == null) {
            Log.e(TAG, "wifi manager is null");
            return;
        }

        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        if (wifiInfo == null) {
            Log.e(TAG, "wifi info is null");
            return;
        }
        int ip = wifiInfo.getIpAddress();
        try {
            mMyAddr =  (Inet4Address)InetAddress.getByName(Formatter.formatIpAddress(ip));
        } catch (UnknownHostException e) {
            e.printStackTrace();
            return;
        }
        Log.v(TAG, "Local device: " + mMyAddr + ", type: " + mDeviceType);

        NsdServiceInfo info = new NsdServiceInfo();
        info.setHost(mMyAddr);
        info.setPort(P2pWfdDeviceInfo.DEFAULT_SESSION_MGMT_CTRL_PORT);
        info.setServiceName(getServiceName(mDeviceType));
        info.setServiceType(getServiceType(mDeviceType));
        mLocalDevice.updateNsdInfo(info);
    }

    static String getServiceName(WfdEnums.WFDDeviceType deviceType) {
        switch (deviceType) {
            case SOURCE:
                return "displaysrc";
            case PRIMARY_SINK:
                return "display";
            case SECONDARY_SINK:
                return "displaysink";
            default:
                return null;
        }
    }

    static String getServiceType(WfdEnums.WFDDeviceType deviceType) {
        switch (deviceType) {
            case SOURCE:
                return "_displaysrc._tcp";
            case PRIMARY_SINK:
                return "_display._tcp";
            case SECONDARY_SINK:
                return "_displaysink._tcp";
            default:
                return null;
        }
    }

    static WfdEnums.WFDDeviceType getDeviceTypeFromServiceType(String serviceType) {
        if (serviceType.indexOf("_displaysrc._tcp") >= 0) {
            return WfdEnums.WFDDeviceType.SOURCE;
        } else if (serviceType.indexOf("_display._tcp") >= 0) {
            return WfdEnums.WFDDeviceType.PRIMARY_SINK;
        } else if (serviceType.indexOf("_displaysink._tcp") >= 0) {
            return WfdEnums.WFDDeviceType.SECONDARY_SINK;
        } else {
            return WfdEnums.WFDDeviceType.UNKNOWN;
        }
    }

}
