/*
* Copyright (c) 2012-2013, 2016-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

package com.qualcomm.wfd.client.net;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pInfo;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pManager.ActionListener;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.net.wifi.p2p.WifiP2pWfdInfo;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums.NetType;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;
import com.qualcomm.wfd.client.Whisperer;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;


public class WifiP2pNetManager implements NetManagerIF {
    static final String TAG = "WifiP2pNetManager";

    private Whisperer whisperer = Whisperer.getInstance();

    private Handler mEventHandler;
    WifiP2pManager manager;
    Channel channel;
    private HandlerThread processThread;

    private final WifiP2pNetDevice localDevice = new WifiP2pNetDevice(null);
    private volatile WifiP2pInfo localConnectionInfo;
    private volatile boolean mEnabled;
    private volatile boolean mConnected;

    private final Collection<WifiP2pNetDevice> peers = new ArrayList<>() ;
    private final Collection<WifiP2pNetDevice> connectedPeers = new ArrayList<>() ;

    private final IntentFilter intentFilter = new IntentFilter();
    private final BroadcastReceiver mReceiver;

    static WifiP2pNetManager sInstance;

    public WifiP2pNetManager(Handler handler) {
        if (sInstance != null) {
            throw new RuntimeException("Can create only one instance");
        }
        sInstance = this;
        mEventHandler = handler;
		processThread = new HandlerThread("WifiP2pNetManager");
		processThread.start();
		manager = (WifiP2pManager) whisperer.getActivity()
				.getSystemService(Context.WIFI_P2P_SERVICE);
		//channel = manager.initialize(context, processThread.getLooper(), null);

        channel = manager.initialize(whisperer.getActivity(), processThread.getLooper(), new WifiP2pManager.ChannelListener() {
            private boolean retryChannel = false;
            @Override
            public void onChannelDisconnected() {
                Log.d(TAG, "onChannelDisconnected() called");
                // we will try once more
                if (manager != null && !retryChannel) {
                    mEventHandler.obtainMessage(WIFI_UTIL_RETRY_CHANNEL, getNetType().ordinal(), 0).sendToTarget();
                    retryChannel = true;
                    channel = manager.initialize(whisperer.getActivity(), processThread.getLooper(), this);
                } else {
                    peers.clear();
                    connectedPeers.clear();
                    mEventHandler.obtainMessage(WIFI_UTIL_CHANNAL_LOST_PERMANENTLY, getNetType().ordinal(), 0).sendToTarget();
                }
            }
        });

        // add necessary intent values to be matched.
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION);
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION);
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION);

        mReceiver = new WifiDirectBroadcastReceiver(manager);
    }

    public void destroy() {
        //whisperer.activity.unregisterReceiver(mReceiver);
        stopPeerDiscovery(ResultListener.NullListener);
        processThread.quitSafely();
        sInstance = null;
    }

    /**
     * Method to convert a WifiP2PDevice to a WFDDevice
     *
     * @param device
     *            the WifiP2pDevice to be converted to WfdDevice
     * @return WfdDevice the converted WfdDevice
     */
    WfdDevice convertToWfdDevice(WifiP2pNetDevice device) {
        // another parameter: boolean parseLeaseFile
        WifiP2pDevice wifiP2pDevice = device.getDevice();

        if (wifiP2pDevice == null) {
            Log.e(TAG, "convertToWfdDevice Something amiss!! wifiP2pDevice is null");
            return null;
        }
        if (wifiP2pDevice.wfdInfo == null) {
            Log.e(TAG, "convertToWfdDevice Something fishy!! WFDInfo is null for device");
            return null;
        }
        WfdDevice wfdDevice = new WfdDevice();
        wfdDevice.deviceType = wifiP2pDevice.wfdInfo.getDeviceType();
        wfdDevice.netType = getNetType().ordinal();
        wfdDevice.macAddress = wifiP2pDevice.deviceAddress;
        Log.d(TAG, "convertToWfdDevice: device macAddress= "
                + wfdDevice.macAddress);
        wfdDevice.deviceName = wifiP2pDevice.deviceName;
        //P2pWfdDeviceInfo extraWfdInfo = null;

        wfdDevice.ipAddress = device.getIp();
        /*
        if (!parseLeaseFile) {
            if (info != null) {
                wfdDevice.ipAddress = info.groupOwnerAddress.getHostAddress();
                Log.d(TAG, "convertToWfdDevice- IP: " + wfdDevice.ipAddress);
            }
        } else {
            Log.d(TAG, "convertToWfdDevice parsing lease file");
            wfdDevice.ipAddress = getPeerIP(wfdDevice.macAddress);
            Log.d(TAG, "convertToWfdDevice- ipAddress: " + wfdDevice.ipAddress);
        }
        */

        if (wfdDevice.ipAddress == null) {
            Log.e(TAG, "convertToWfdDevice- no ipAddress was found");
        }

        wfdDevice.rtspPort = wifiP2pDevice.wfdInfo.getControlPort();
        wfdDevice.isAvailableForSession = wifiP2pDevice.wfdInfo.isSessionAvailable();
        wfdDevice.addressOfAP = null;
        wfdDevice.coupledSinkStatus = 0;
        wfdDevice.preferredConnectivity = 0;

        return wfdDevice;
    }

    @Override
    public void send_wfd_set(boolean isAvailableForSession, WFDDeviceType type) {

		int wfdDeviceInfo = 0;

		if (type == WFDDeviceType.SOURCE)
			wfdDeviceInfo |= P2pWfdDeviceInfo.DEVICETYPE_SOURCE;
		else if(type == WFDDeviceType.PRIMARY_SINK)
			wfdDeviceInfo |= P2pWfdDeviceInfo.DEVICETYPE_PRIMARYSINK;
		else if(type == WFDDeviceType.SECONDARY_SINK)
			wfdDeviceInfo |= P2pWfdDeviceInfo.DEVICETYPE_SECONDARYSINK;
		else if(type == WFDDeviceType.SOURCE_PRIMARY_SINK)
			wfdDeviceInfo |= P2pWfdDeviceInfo.DEVICETYPE_SOURCE_PRIMARYSINK;

		// Needs to be enabled when service discovery is supported
		//wfdDeviceInfo |= SERVICE_DISCOVERY_SUPPORTED;

		//wfdDeviceInfo |= PREFERRED_CONNECTIVITY_TDLS;

		wfdDeviceInfo |= P2pWfdDeviceInfo.CP_SUPPORTED;

		//wfdDeviceInfo |= TIME_SYNC_SUPPORTED;

		//if(type == WFDDeviceType.PRIMARY_SINK ||
		//type == WFDDeviceType.SECONDARY_SINK ||
		//type == WFDDeviceType.SOURCE_PRIMARY_SINK) {

		//	wfdDeviceInfo |= AUDIO_NOT_SUPPORTED_AT_PSINK;
		//}

		if ( type == WFDDeviceType.SOURCE ||
			type == WFDDeviceType.SOURCE_PRIMARY_SINK ) {
			wfdDeviceInfo |= P2pWfdDeviceInfo.AUDIO_ONLY_SUPPORTED_AT_SOURCE;
		}

		//wfdDeviceInfo |= TDLS_PERSISTENT_GROUP;

		//wfdDeviceInfo |= TDLS_PERSISTENT_GROUP_REINVOKE;

		WifiP2pWfdInfo wfdP2pInfo = new WifiP2pWfdInfo(wfdDeviceInfo,
								P2pWfdDeviceInfo.DEFAULT_SESSION_MGMT_CTRL_PORT,
								P2pWfdDeviceInfo.WIFI_MAX_THROUGHPUT);
		wfdP2pInfo.setWfdEnabled(true);
		wfdP2pInfo.setSessionAvailable(isAvailableForSession);
		if ((type == WFDDeviceType.SOURCE ||
			type == WFDDeviceType.SOURCE_PRIMARY_SINK )) {
				wfdP2pInfo.setCoupledSinkSupportAtSource(false);
		}

		if ( (type == WFDDeviceType.PRIMARY_SINK ||
			type == WFDDeviceType.SECONDARY_SINK ||
			type == WFDDeviceType.SOURCE_PRIMARY_SINK)) {

			wfdP2pInfo.setCoupledSinkSupportAtSink(true);
		}
		manager.setWFDInfo(channel, wfdP2pInfo, new ActionListener() {

			@Override
			public void onSuccess() {
				Log.d(TAG, "Successfully set WFD IE Params");
			}

			@Override
			public void onFailure(int error) {
				Log.d(TAG, "Failed to set WFD IE Params: " + error + ".");
			}

		});

	}

    public static String getPeerIP(String peerMac) {
        Log.d(TAG, "getPeerIP():  peerMac= " + peerMac);

        String ip = null;

        /* Try ARP table lookup */
        BufferedReader br = null;
        try {
            br = new BufferedReader(new FileReader("/proc/net/arp"));
            String line;
            if (br != null) {
                while ((line = br.readLine()) != null) {
                    Log.d(TAG, "line in /proc/net/arp is " + line);
                    String[] splitted = null;
                    if (line != null) {
                        splitted = line.split(" +");
                    }

                    // consider it as a match if 5 out of 6 bytes of the mac
                    // address match
                    // ARP output is in the format
                    // <IP address> <HW type> <Flags> <HW address> <Mask Device>

                    if (splitted != null && splitted.length >= 4) {
                        String[] peerMacBytes = peerMac.split(":");
                        String[] arpMacBytes = splitted[3].split(":");

                        if (arpMacBytes == null) {
                            continue;
                        }

                        int matchCount = 0;
                        for (int i = 0; i < arpMacBytes.length; i++) {
                            if (peerMacBytes[i]
                                    .equalsIgnoreCase(arpMacBytes[i])) {
                                matchCount++;
                            }
                        }

                        if (matchCount >= 5) {
                            ip = splitted[0];
                            // Perfect match!
                            if (matchCount == 6) {
                                // Perfect match!
                                return ip;
                            }
                        }
                    }
                }
            } else {
                Log.e(TAG, "Unable to open /proc/net/arp");
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (br != null)
                    br.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return ip;
    }

    /**
     * @return the local IP if found on p2p interface
     */
    /*
    public static String getLocalIp() {
        Enumeration<NetworkInterface> netIntfList, virtualIntfList;
        Enumeration<InetAddress> phyAddresses, virtualAddresses;
        try {
            netIntfList = NetworkInterface.getNetworkInterfaces();
            while (netIntfList.hasMoreElements()) {
                NetworkInterface netIntf = netIntfList.nextElement();
                if (netIntf.isUp() && netIntf.getName().contains("p2p")) {
                    virtualIntfList = netIntf.getSubInterfaces();
                    while (virtualIntfList.hasMoreElements()) {
                        NetworkInterface virtualNetIntf = virtualIntfList
                                .nextElement();
                        virtualAddresses = virtualNetIntf.getInetAddresses();
                        while (virtualAddresses.hasMoreElements()) {
                            InetAddress virtualIPAddress = virtualAddresses
                                    .nextElement();
                            if (virtualIPAddress instanceof Inet4Address) {
                                Log.e(TAG,
                                        "IP address of device on virtual interface "
                                                + netIntf.getName()
                                                + " is "
                                                + virtualIPAddress
                                                        .getHostAddress());
                                return virtualIPAddress.getHostAddress();
                            }
                        }
                    }
                    phyAddresses = netIntf.getInetAddresses();
                    while (phyAddresses.hasMoreElements()) {
                        InetAddress phyIPAddress = phyAddresses.nextElement();
                        if (phyIPAddress instanceof Inet4Address) {
                            Log.e(TAG,
                                    "IP address of device on physical interface "
                                + netIntf.getName() + " is "
                                            + phyIPAddress.getHostAddress());
                            return phyIPAddress.getHostAddress();
                        }
                    }
                }
            }
        } catch (SocketException e) {
            Log.e(TAG, "SocketException");
        }
        return null;
    }
    */

    @Override
    public void disconnect(final ResultListener rl) {
        Log.d(TAG, "disconnect() called");
        if (!whisperer.isMultiSinkMode()) {
            manager.removeGroup(channel, wrapResultListener(rl));
        } else {
            Log.e(TAG,"Ignoring call to remove group since Auto-GO mode is enabled");
        }
    }

    @Override
    public NetType getNetType() {
        return NetType.WIFI_P2P;
    }

    /* Callback Starts */
    @Override
    public void onMainActivityResume() {
        Log.v(TAG, "onMainActivityResume");
        whisperer.getActivity().registerReceiver(mReceiver, intentFilter);
    }

    @Override
    public void onMainActivityPause() {
        Log.v(TAG, "onMainActivityPause");
        if (!whisperer.isMultiSinkMode()) {// TODO: MITask Fix Me!
            whisperer.getActivity().unregisterReceiver(mReceiver);
        }
    }

    void onDisconnected() {
        Log.v(TAG, "onDisconnected");
        peers.clear();
        connectedPeers.clear();
        mConnected = false;
        mEventHandler.obtainMessage(WIFI_UTIL_DISCONNECTED, getNetType().ordinal(), 0).sendToTarget();
    }

    void onConnectionInfoAvailable(WifiP2pInfo info) {
        Log.v(TAG, "onConnectionInfoAvailable " + info);
        //localDevice.mWifiP2pInfo = info;
        localConnectionInfo = info;
        mConnected = true;
        mEventHandler.obtainMessage(WIFI_UTIL_CONNECTION_INFO, getNetType().ordinal(), 0,
                new WifiWigigP2pConnectionInfo(info)).sendToTarget();
    }

    void onPeerListUpdate(Collection<WifiP2pDevice> devices) {
        Log.v(TAG, "onPeerListUpdate");
        peers.clear();
        for (WifiP2pDevice dev: devices) {
            peers.add(new WifiP2pNetDevice(dev));
        }
        mEventHandler.obtainMessage(WIFI_UTIL_PEERS_CHANGED, getNetType().ordinal(), 0, peers).sendToTarget();
    }

    void onConnectedListUpdate(Collection<WifiP2pDevice> devices) {
        Log.v(TAG, "onConnectedListUpdate");
        connectedPeers.clear();
        for (WifiP2pDevice device: devices) {
            connectedPeers.add(new WifiP2pNetDevice(device));
        }
        mEventHandler.obtainMessage(WIFI_UTIL_CONNECTED_PEERS_CHANGED, getNetType().ordinal(), 0, connectedPeers).sendToTarget();
    }

    void onLocalDeviceChanged(WifiP2pDevice device) {
        Log.v(TAG, "onLocalDeviceChanged");
        localDevice.setDevice(device);
        mEventHandler.obtainMessage(WIFI_UTIL_LOCAL_DEVICE_CHANGED, getNetType().ordinal(), 0, localDevice).sendToTarget();
    }

    void onStateChanged(int state) {
        Log.v(TAG, "onStateChanged " + state);
        mEnabled = (state == WifiP2pManager.WIFI_P2P_STATE_ENABLED);
        mEventHandler.obtainMessage(NetManagerIF.WIFI_UTIL_P2P_STATE_CHANGED, getNetType().ordinal(), state).sendToTarget();
    }
    /* Callback Ends */

    @Override
    public void setMiracastMode(int mode) {
        manager.setMiracastMode(mode);
    }

    @Override
    public void createGroup(final ResultListener rl) {
        manager.createGroup(channel, wrapResultListener(rl));
    }

    @Override
    public void removeGroup(final ResultListener rl) {
        manager.removeGroup(channel, wrapResultListener(rl));
    }

    @Override
    public void discoveryPeers(final ResultListener rl) {
        manager.discoverPeers(channel, wrapResultListener(rl));
    }

    @Override
    public void stopPeerDiscovery(final ResultListener rl) {
        manager.stopPeerDiscovery(channel, wrapResultListener(rl));
    }

    @Override
    public void cancelConnect(final ResultListener rl) {
        if (localDevice.hasDeviceInfo() && localDevice.getDevice().status == WifiP2pDevice.CONNECTED) {
            disconnect(rl);
        } else {
            manager.cancelConnect(channel, wrapResultListener(rl));
        }
    }

    @Override
    public boolean isEnabled() {
        return mEnabled;
    }

    @Override
    public boolean isConnected() {
        return mConnected;
    }

    @Override
    public WifiP2pNetDevice getLocalDevice() {
        return localDevice;
    }

    @Override
    public ConnectionInfoIF getLocalConnectionInfo() {
        return localConnectionInfo != null? new WifiWigigP2pConnectionInfo(localConnectionInfo) : null;
    }

    @Override
    public Collection<NetDeviceIF> getPeers() {
        return Collections.unmodifiableCollection((Collection)peers);
    }

    @Override
    public Collection<NetDeviceIF> getConnectedPeers() {
        return Collections.unmodifiableCollection((Collection)connectedPeers);
    }

    private static ActionListener wrapResultListener(final ResultListener rl) {
        return new ActionListener() {
            @Override
            public void onSuccess() {
                rl.onSuccess(null);
            }

            @Override
            public void onFailure(int i) {
                rl.onFailure(i);
            }
        };
    }
}
