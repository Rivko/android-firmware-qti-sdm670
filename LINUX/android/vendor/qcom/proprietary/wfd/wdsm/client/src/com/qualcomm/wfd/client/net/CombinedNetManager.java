/*
* Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.NetType;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.EnumMap;
import java.util.Map;
import java.util.Set;

import static com.qualcomm.wfd.client.net.ResultListener.AnySuccessResultListener;

//import com.qualcomm.qti.wigig.p2p.WigigP2pManager;

public class CombinedNetManager implements NetManagerIF {
    static final String TAG = "CombinedNetManager";

    static CombinedNetManager sInstance;

    private final Handler mMyHandler = new MyHandler();
    private final Handler mEventHandler;

    private final int mNetCount;
    private final Map<NetType, NetManagerIF> mNetManagers = new EnumMap<>(NetType.class);
    private final Collection<NetDeviceIF> mPeers = new ArrayList<>();
    private final Collection<NetDeviceIF> mConnectedPeers = new ArrayList<>();

    public CombinedNetManager(Handler handler, NetType ... enabledNets) {
        synchronized (CombinedNetManager.class) {
            if (sInstance != null) {
                throw new RuntimeException("Can create only one instance");
            } else {
                sInstance = this;
            }
        }

        Log.v(TAG, "enabled nets: " + Arrays.toString(enabledNets));
        mEventHandler = handler;
        mNetCount = enabledNets.length;
        for (NetType type: enabledNets) {
            if (mNetManagers.get(type) != null) {
                throw new RuntimeException("Unexpected net type: " + type);
            }
            NetManagerIF netManager;
            switch (type) {
                case WIFI_P2P:
                    netManager = new WifiP2pNetManager(mMyHandler);
                    break;
/*
                case WIGIG_P2P:
                    if (WigigP2pManager.getInstance() != null) {
                        netManager = new WigigP2pNetManager(mMyHandler);
                    } else {
                        Log.w(TAG, "Wigig is not supported");
                        netManager = null;
                    }
                    break;
*/
                case LAN:
                    netManager = new LanManager(mMyHandler);
                    break;
                default:
                    netManager = null;
                    Log.e(TAG, "Unexpected net type: " + type);
                    break;
            }
            if (netManager != null) {
                mNetManagers.put(type, netManager);
            }
        }
    }

    public void destroy() {
        Log.v(TAG, "destroy");
        for (NetManagerIF netManager: getNetManagers()) {
            netManager.destroy();
        }
        mNetManagers.clear();
        mPeers.clear();
        mConnectedPeers.clear();
        synchronized (CombinedNetManager.class) {
            sInstance = null;
        }
    }

    private Collection<NetManagerIF> getNetManagers() {
        return mNetManagers.values();
    }

    public boolean allSubNetPeersReceived() {
        for (NetManagerIF mgr: getNetManagers()) {
            if (mgr.getPeers().size() == 0) {
                return false;
            }
        }
        return true;
    }

    @Override
    public NetType getNetType() {
        return NetType.UNKNOWN_NET;
    }

    public Set<NetType> getNetTypes() {
        return mNetManagers.keySet();
    }

    @Override
    public void onMainActivityResume() {
        Log.v(TAG, "onMainActivityResume");
        for (NetManagerIF netManager: getNetManagers()) {
            netManager.onMainActivityResume();
        }
    }

    @Override
    public void onMainActivityPause() {
        Log.v(TAG, "onMainActivityPause");
        for (NetManagerIF netManager: getNetManagers()) {
            netManager.onMainActivityPause();
        }
    }

    @Override
    public void send_wfd_set(boolean isAvailableForSession, WfdEnums.WFDDeviceType type) {
        Log.v(TAG, "send_wfd_set: " + isAvailableForSession + ", " + type);
        for (NetManagerIF netManager: getNetManagers()) {
            netManager.send_wfd_set(isAvailableForSession, type);
        }
    }

    @Override
    public void disconnect(ResultListener rl) {
        Log.v(TAG, "disconnect");
        final ResultListener listener = new AnySuccessResultListener(rl, mNetCount);
        for (NetManagerIF mgr: getNetManagers()) {
            mgr.disconnect(listener);
        }
    }

    @Override
    public void setMiracastMode(int mode) {
        Log.v(TAG, "setMiracastMode: " + mode);
        for (NetManagerIF netManager: getNetManagers()) {
            netManager.setMiracastMode(mode);
        }
    }

    @Override
    public void createGroup(ResultListener rl) {
        Log.v(TAG, "createGroup");
        final ResultListener listener = new AnySuccessResultListener(rl, mNetCount);
        for (NetManagerIF mgr: getNetManagers()) {
            mgr.createGroup(listener);
        }
    }

    @Override
    public void removeGroup(ResultListener rl) {
        Log.v(TAG, "removeGroup");
        final ResultListener listener = new AnySuccessResultListener(rl, mNetCount);
        for (NetManagerIF mgr: getNetManagers()) {
            mgr.removeGroup(listener);
        }
    }

    @Override
    public void discoveryPeers(ResultListener rl) {
        Log.v(TAG, "discoveryPeers");
        final ResultListener listener = new AnySuccessResultListener(rl, mNetCount);
        for (NetManagerIF mgr: getNetManagers()) {
            mgr.discoveryPeers(listener);
        }
    }

    @Override
    public void stopPeerDiscovery(final ResultListener rl) {
        Log.v(TAG, "stopPeerDiscovery");
        final ResultListener listener = new AnySuccessResultListener(rl, mNetCount);
        for (NetManagerIF mgr: getNetManagers()) {
            mgr.stopPeerDiscovery(listener);
        }
    }

    @Override
    public void cancelConnect(final ResultListener rl) {
        Log.v(TAG, "cancelConnect");
        final ResultListener listener = new AnySuccessResultListener(rl, mNetCount);
        for (NetManagerIF mgr: getNetManagers()) {
            mgr.cancelConnect(listener);
        }
    }

    @Override
    public boolean isEnabled() {
        boolean enabled = false;
        for (NetManagerIF mgr: getNetManagers()) {
            enabled = enabled ||  mgr.isEnabled();
        }
        return enabled;
    }

    @Override
    public boolean isConnected() {
        boolean connected = false;
        for (NetManagerIF mgr: getNetManagers()) {
            connected = connected ||  mgr.isConnected();
        }
        return connected;
    }

    @Override
    public NetDeviceIF getLocalDevice() {
        return null;
    }

    public Map<NetType, NetDeviceIF> getLocalDevices() {
        Map<NetType, NetDeviceIF> localDevices = new EnumMap<>(NetType.class);
        for (NetType type: mNetManagers.keySet()) {
            NetManagerIF mgr = mNetManagers.get(type);
            localDevices.put(type, mgr.getLocalDevice());
        }
        return localDevices;
    }

    @Override
    public Collection<NetDeviceIF> getPeers() {
        return Collections.unmodifiableCollection((Collection)mPeers);
    }

    @Override
    public Collection<NetDeviceIF> getConnectedPeers() {
        return Collections.unmodifiableCollection((Collection)mConnectedPeers);
    }

    @Override
    public ConnectionInfoIF getLocalConnectionInfo() {
        Log.w(TAG, "getLocalConnectionInfo");
        for (NetManagerIF mgr: getNetManagers()) {
            ConnectionInfoIF info = mgr.getLocalConnectionInfo();
            if (info != null) { // return any none null info
                return info;
            }
        }
        return null;
    }

    private void updatePeers() {
        mPeers.clear();
        for (NetManagerIF mgr: getNetManagers()) {
            mPeers.addAll(mgr.getPeers());
        }
    }

    private void updateConnectedPeers() {
        mConnectedPeers.clear();
        for (NetManagerIF mgr: getNetManagers()) {
            mConnectedPeers.addAll(mgr.getConnectedPeers());
        }
    }

    class MyHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Event handler received: " + msg.toString());

            switch (msg.what) {
                case WIFI_UTIL_RETRY_CHANNEL: {
                    updatePeers();
                    mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                }
                break;
                case WIFI_UTIL_CHANNAL_LOST_PERMANENTLY: {
                    updatePeers();
                    updateConnectedPeers();
                    mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                }
                break;
                case WIFI_UTIL_P2P_STATE_CHANGED: { // arg2 = wifi_p2p_state
                    mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                }
                break;
                case WIFI_UTIL_DISCONNECTED: {
                    if (isConnected()) {
                        Log.v(TAG, "Hide DISCONNECTED event from: " + NetType.values()[msg.arg1]);
                    } else {
                        mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                    }
                }
                break;
                case WIFI_UTIL_CONNECTION_INFO: { // obj = WifiP2pInfo
                    mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                }
                break;
                case WIFI_UTIL_PEERS_CHANGED: { // obj = Collection<WifiP2pNetDevice> peers
                    updatePeers();
                    mEventHandler.obtainMessage(WIFI_UTIL_PEERS_CHANGED, getNetType().ordinal(), 0, mPeers).sendToTarget();
                }
                break;
                case WIFI_UTIL_CONNECTED_PEERS_CHANGED: { // obj = Collection<WifiP2pNetDevice> connectedPeers
                    updateConnectedPeers();
                    mEventHandler.obtainMessage(WIFI_UTIL_CONNECTED_PEERS_CHANGED, getNetType().ordinal(), 0, mConnectedPeers).sendToTarget();
                }
                break;
                case WIFI_UTIL_LOCAL_DEVICE_CHANGED: {
                    mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                }
                break;
                case WIFI_UTIL_CONNECTION_REQUEST: {
                    mEventHandler.obtainMessage(msg.what, msg.arg1, msg.arg2, msg.obj).sendToTarget();
                }
                break;
                default:
                    Log.w(TAG, "Unknown message");
                    break;
            }
        }
    }

}
