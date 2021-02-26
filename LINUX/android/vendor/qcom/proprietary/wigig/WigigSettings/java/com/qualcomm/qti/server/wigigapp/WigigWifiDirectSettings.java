/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigigapp;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.NetworkInfo;
import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pGroup;
import android.net.wifi.p2p.WifiP2pInfo;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.ContextMenu;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.Toast;

import com.qualcomm.qti.wigig.p2p.WigigP2pManager;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

public class WigigWifiDirectSettings extends WigigActivity implements WigigP2pManager.PeerListListener {
    private static final String TAG = "WigigWifiDirectSettings";
    private List<P2pPeer> peers;
    private WigigP2pManager.Channel mChannel;
    private String mP2PDeviceName;
    private boolean mP2PDiscoveryState;
    private PreferenceCategory mPeersCategory;
    private PreferenceCategory mRememberedGroupsCategory;
    private boolean mIsConnecting;
    private boolean mP2PGroupFormed;
    private boolean mP2PGroupOwner;
    private InetAddress mP2PGroupAddress;
    private Object myOnPersistentGroupInfoAvailableListener = null;
    private int mModeChange = MODE_NOT_CHANGED;

    private class P2pGroupPreference extends Preference {
        private String mName;
        private int mNetId;
        public String getName() { return mName; }
        public int getNetId() { return mNetId; }
        public P2pGroupPreference(Context context, String name, int netid) {
            super(context);
            mName = name;
            mNetId = netid;
            setTitle(mName);
            setSummary(null);
        }
    }
    private List<P2pGroupPreference> groups;

    public WigigWifiDirectSettings() {
        super(R.xml.pref_wfd, new String[] {WigigP2pManager.WIGIG_P2P_STATE_CHANGED_ACTION,
                WigigP2pManager.WIGIG_P2P_PEERS_CHANGED_ACTION,
                WigigP2pManager.WIGIG_P2P_CONNECTION_CHANGED_ACTION,
                WigigP2pManager.WIGIG_P2P_THIS_DEVICE_CHANGED_ACTION,
                WigigP2pManager.WIGIG_P2P_DISCOVERY_CHANGED_ACTION,
                WigigP2pManager.WIGIG_P2P_PERSISTENT_GROUPS_CHANGED_ACTION} );
        peers = new ArrayList<>();
        groups = new ArrayList<>();
        mP2PDeviceName = "";
        mP2PDiscoveryState = false;
        mIsConnecting = false;

    }

    /*
    This implements PeersListListener interface for P2P. See requestPeers().
     */
    @Override
    public void onPeersAvailable(WifiP2pDeviceList list)
    {
        Log.d(TAG, "P2P onPeersAvailable called");
        boolean isConnected = false;
        mIsConnecting = false;
        peers.clear();
        for (WifiP2pDevice d : list.getDeviceList()) {
            Log.d(TAG, "P2P peer added " + d.deviceName + " ,device address " + d.deviceAddress);
            P2pPeer.PeerStatus status = P2pPeer.PeerStatus.PeerUnavailable;
            switch (d.status) {
                case WifiP2pDevice.AVAILABLE:
                    status = P2pPeer.PeerStatus.PeerAvailable;
                    break;
                case WifiP2pDevice.CONNECTED:
                    status = P2pPeer.PeerStatus.PeerConnected;
                    isConnected = true;
                    break;
                case WifiP2pDevice.INVITED:
                    status = P2pPeer.PeerStatus.PeerInvited;
                    mIsConnecting = true;
                    break;
                case WifiP2pDevice.UNAVAILABLE:
                    status = P2pPeer.PeerStatus.PeerUnavailable;
                    break;
                case WifiP2pDevice.FAILED:
                    status = P2pPeer.PeerStatus.PeerFailed;
                    break;
            }
            addPeer(d.deviceName, d.deviceAddress, status);
        }
        setConnected(isConnected);
        refreshP2PScreen();
    }

    /*
     This implements Persistent Groups change listener.
     Normally it would be implemented as
        @Override
        public void onPersistentGroupInfoAvailable(WifiP2pGroupList list) {}
     but WifiP2pGroupList is not a public API. We have to implement this
     via proxy/reflection.
     */
    private class ProxyOnPersistentGroupInfoAvailable implements InvocationHandler {

        // implements the method onAudioFocusChange from the OnAudioFocusChangeListener
        public void onPersistentGroupInfoAvailable(Object wifiGroupList) {
            Collection<WifiP2pGroup> result = null;
            int netid;
            groups.clear();
            try {
                Method method = wifiGroupList.getClass().getDeclaredMethod("getGroupList");
                result = (Collection<WifiP2pGroup>) method.invoke(wifiGroupList);
            } catch (Throwable e) {
                Log.e(TAG, "ProxyOnPersistenGroupInfoAvaiabled exception " + e.getMessage());
            }
            if (result != null) {
                for (WifiP2pGroup g : result) {
                    Log.d(TAG, "Group available: " + g.getNetworkName());
                    netid = -1;
                    try {
                        Method method = g.getClass().getDeclaredMethod("getNetworkId");
                        Object value;
                        value = method.invoke(g);
                        netid = (int)value;
                    }
                    catch (Throwable e) {
                        Log.e(TAG, "Can not get network id for group " + g.getNetworkName() + " exception: " + e.getMessage());
                    }
                    Log.d(TAG, "Group available: " + g.getNetworkName() + " netid: " + netid);
                    P2pGroupPreference group = new P2pGroupPreference(WigigWifiDirectSettings.this, g.getNetworkName(), netid);
                    groups.add(group);
                    /*
                    Collection<WifiP2pDevice> devices = g.getClientList();
                    for (WifiP2pDevice d : devices) {
                        Log.d(TAG, "Device in the group " + d.deviceName);
                    }
                    */
                }
                refreshP2PScreen();
            }
        }

        public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
            try {
                if (args != null) {
                    if (method.getName().equals("onPersistentGroupInfoAvailable")) {
                        onPersistentGroupInfoAvailable(args[0]);
                    }
                }
            } catch (Throwable e) {
                Log.e(TAG, "ProxyOnPersistenGroupInfoAvailable invoke exception " + e.getMessage());
            }
            return null;
        }
    }

    @SuppressWarnings("deprecation")
    private void refreshP2PScreen() {
        Preference pref = findPreference("p2p_name");
        if (pref != null) {
            if (mP2PDeviceName.isEmpty())
                pref.setTitle("Obtaining name...");
            else
                pref.setTitle(mP2PDeviceName);
        }

        if (pref != null) {
            if (mP2PDiscoveryState)
                pref.setSummary(R.string.wfd_searching);
            else
                pref.setSummary(R.string.wfd_start_search);
        }

        PreferenceScreen p = getPreferenceScreen();
        mRememberedGroupsCategory.removeAll();
        p.removePreference(mRememberedGroupsCategory);
        mPeersCategory.removeAll();
        p.removePreference(mPeersCategory);

        p.addPreference(mPeersCategory);
        for (P2pPeer peer : peers) {
            mPeersCategory.addPreference(peer);
        }

        p.addPreference(mRememberedGroupsCategory);
        for (P2pGroupPreference g : groups) {
            mRememberedGroupsCategory.addPreference(g);
        }
    }

    private void requestPersistentGroup() {
        if (mP2PManager != null)  {
            if (myOnPersistentGroupInfoAvailableListener == null) {
                Class<?>[] classArray = new Class<?>[1];
                classArray[0] = WigigP2pManager.PersistentGroupInfoListener.class;
                myOnPersistentGroupInfoAvailableListener = Proxy.newProxyInstance(classArray[0].getClassLoader(), classArray, new ProxyOnPersistentGroupInfoAvailable());
            }
            mP2PManager.requestPersistentGroupInfo(mChannel, (WigigP2pManager.PersistentGroupInfoListener)myOnPersistentGroupInfoAvailableListener);
        }
    }

    @Override
    protected void handleEvent(Intent intent) {
        super.handleEvent(intent);
        final String action = intent.getAction();
        if (action.equals(WigigP2pManager.WIGIG_P2P_STATE_CHANGED_ACTION)) {
            // UI update to indicate wifi p2p status.
            int state = intent.getIntExtra(WigigP2pManager.EXTRA_WIGIG_STATE, -1);
            if (state == WigigP2pManager.WIGIG_P2P_STATE_ENABLED) {
                Log.d(TAG, "Wigig P2P State enabled");
            } else {
                Log.d(TAG, "Wigig P2P State disabled");
                peers.clear();
                groups.clear();
                refreshP2PScreen();
            }
        }
        else if (action.equals(WigigP2pManager.WIGIG_P2P_PEERS_CHANGED_ACTION)) {
            /* Discovery peers succeeded and some peers are detected */
            Log.d(TAG, "Request peers....");
            if (mP2PManager != null) {
                mP2PManager.requestPeers(mChannel, this);
            }

        }
        else if (action.equals(WigigP2pManager.WIGIG_P2P_THIS_DEVICE_CHANGED_ACTION)) {
            Log.d(TAG, "P2P device changed action");
            WifiP2pDevice device = intent.getParcelableExtra(WigigP2pManager.EXTRA_WIGIG_P2P_DEVICE);
            mP2PDeviceName = device.deviceName;
            Log.d(TAG, "P2P device own name" + mP2PDeviceName);
            refreshP2PScreen();

        }
        else if (action.equals(WigigP2pManager.WIGIG_P2P_DISCOVERY_CHANGED_ACTION)) {
            int discoveryState = intent.getIntExtra(WigigP2pManager.EXTRA_DISCOVERY_STATE, -1);
            Log.d(TAG, "P2P discovery state int " + discoveryState);
            if (discoveryState == WigigP2pManager.WIGIG_P2P_DISCOVERY_STARTED)
                mP2PDiscoveryState = true;
            else if(discoveryState == WigigP2pManager.WIGIG_P2P_DISCOVERY_STOPPED)
                mP2PDiscoveryState = false;
            refreshP2PScreen();
        }
        else if (action.equals(WigigP2pManager.WIGIG_P2P_PERSISTENT_GROUPS_CHANGED_ACTION)) {
            Log.d(TAG, "P2P persistent groups changed action");
            requestPersistentGroup();
        }
    }

    @Override
    protected void onWigigSwitchedOn() {
        super.onWigigSwitchedOn();
        setWigigEnabled(true);
    }

    @Override
    protected void onWigigSwitchedOff() {
        super.onWigigSwitchedOff();
        setWigigEnabled(false);
        peers.clear();
    }

    @Override
    protected boolean onModeChange(String mode) {
        Log.d(TAG, "Mode preference changed to " + mode);
        if (mode.equals(getResources().getString(R.string.network_mode)))  {
            if (isConnected()) {
                Log.d(TAG, "Should start Network Activity after disconnect");
                modeSwitchDelayed = mode;
                disconnectAll();
            }
            else {
                Log.d(TAG, "Starting Network Activity");
                modeSwitchDelayed = "";
                stopOngoingActivity();
                switchWigigMode(WigigWifiDirectSettings.this, WigigSettingsActivity.class, false);
            }
            return false;   // Don't update the mode menu, the new activity will do that if switched
        }
        else if (mode.equals(getResources().getString(R.string.hotspot_mode))) {
            if ((mModeChange & MODE_TO_CHANGE_HOTSPOT) == MODE_TO_CHANGE_HOTSPOT) {
                switchWigigMode(WigigWifiDirectSettings.this, SoftAP.class,
                        ((mModeChange & MODE_ALREADY_ENABLED) == MODE_ALREADY_ENABLED) ? false : true);
                mModeChange = MODE_NOT_CHANGED;
            } else {
                mModeChange = mModeChange | MODE_TO_CHANGE_HOTSPOT;
                setWigigEnabled(false);
            }
        }
        else if (mode.equals(getResources().getString(R.string.ru_mode))) {
            if ((mModeChange & MODE_TO_CHANGE_RU) == MODE_TO_CHANGE_RU) {
                switchWigigMode(WigigWifiDirectSettings.this, WigigRateUpgradeSettings.class,
                        ((mModeChange & MODE_ALREADY_ENABLED) == MODE_ALREADY_ENABLED) ? false : true);
                mModeChange = MODE_NOT_CHANGED;
            } else {
                mModeChange = mModeChange | MODE_TO_CHANGE_RU;
                setWigigEnabled(false);
            }
        }
        return true;
    }

    @Override
    protected void onWigigFrameworkEnabled() {
        super.onWigigFrameworkEnabled();
        Log.d(TAG, "onWigigFrameworkEnabled()");

    }

    @Override
    protected void onWigigFrameworkDisabled() {
        super.onWigigFrameworkDisabled();
        Log.d(TAG, "onWigigFrameworkDisabled()");
        if ((mModeChange & MODE_TO_CHANGE_HOTSPOT) != 0)
            onModeChange(getString(R.string.hotspot_mode));
        else if ((mModeChange & MODE_TO_CHANGE_RU) != 0)
            onModeChange(getString(R.string.ru_mode));
    }

    @Override
    protected void onInitialStatesObtained(boolean[] states) {
        super.onInitialStatesObtained(states);
        Log.d(TAG, "onInitialStatesObtained() " + states.toString());
        if (states[INITIAL_WIGIG_ENABLED_STATE]) {
            if (states[INITIAL_NETWORK_CONNECTED_STATE]) {
                // WiGig enabled and Network connected, switch to Network mode
                switchWigigModeWithAlert(this, WigigSettingsActivity.class);
            }
            else {
                if (!states[INITIAL_P2P_CONNECTED_STATE]) {
                    // WiGig enabled and P2P not connected, start peers discovery
                    discoverP2P();
                }
            }
        }
    }

    @Override
    protected void onNetworkStateChanged(NetworkInfo.DetailedState ds, int networkId) {
        Log.d(TAG, "onNetworkStateChanged()");
        if (!isInitialStateObtaining()  && ds == NetworkInfo.DetailedState.CONNECTED) {
            switchWigigModeWithAlert(this, WigigSettingsActivity.class);
            return;
        }
        else if (ds != NetworkInfo.DetailedState.IDLE) {
            Log.d(TAG, "NETWORK_STATE_CHANGED state " + ds.toString());
        }
        super.onNetworkStateChanged(ds, networkId);
    }

    @Override
    protected void onP2PStateChanged(NetworkInfo networkInfo) {
        super.onP2PStateChanged(networkInfo);
        Log.d(TAG, "onP2PStateChanged()");
        if (mP2PManager != null) {
            Log.d(TAG, "P2P_CONNECTION_CHANGED_ACTION " + networkInfo.toString());

            if (networkInfo.isConnected()) {
                setConnected(true);
                // We can get connected by another peer. Request updated peers info
                mP2PManager.requestPeers(mChannel, this);
                mP2PManager.requestConnectionInfo(mChannel, new WigigP2pManager.ConnectionInfoListener() {

                    @Override
                    public void onConnectionInfoAvailable(WifiP2pInfo p) {
                        Log.d(TAG, "Connection ifno available " + p.toString());
                        mP2PGroupFormed = p.groupFormed;
                        mP2PGroupOwner = p.isGroupOwner;
                        mP2PGroupAddress = p.groupOwnerAddress;
                    }
                });
                requestPersistentGroup();
            }
            else {
                setConnected(false);
                if (!modeSwitchDelayed.isEmpty()) {
                    onModeChange(modeSwitchDelayed);
                    return;
                }
                if (!isInitialStateObtaining() && !networkInfo.isConnectedOrConnecting()) {
                    Log.d(TAG, "We've been disconnected. Initiate peers discovery");
                    discoverP2P();
                }
            }
            refreshP2PScreen();
        }
    }

    private Preference.OnPreferenceClickListener sClickPreferenceListener = new Preference.OnPreferenceClickListener() {
        @Override
        public boolean onPreferenceClick(Preference preference) {
            if (preference instanceof P2pPeer) {
                P2pPeer pref = (P2pPeer) preference;
                String name = pref.getName();
                if (isPeerConnected(name))
                    displayP2PInfo(name);
                else if (isPeerConnecting(name))
                    cancelP2PConnect();
                else
                    connectP2P(name);
            }
            return true;
        }
    };

    private P2pPeer addPeer(String ssid, String addr, P2pPeer.PeerStatus status) {
        P2pPeer a = new P2pPeer(this, ssid, addr, status);
        a.setOnPreferenceClickListener(sClickPreferenceListener);
        peers.add(a);
        return a;
    }

    private String peerGetAddressByName(String name) {
        for(P2pPeer a : peers) {
            if (a.getName().equalsIgnoreCase(name))
                return a.getAddress();
        }
        return "";
    }

    private boolean isPeerConnected(String name) {
        for(P2pPeer a : peers) {
            if (a.getName().equalsIgnoreCase(name))
                return a.getStatus() == P2pPeer.PeerStatus.PeerConnected;
        }
        return false;
    }

    private boolean isPeerInvited(String name) {
        for(P2pPeer a : peers) {
            if (a.getName().equalsIgnoreCase(name))
                return a.getStatus() == P2pPeer.PeerStatus.PeerInvited;
        }
        return false;
    }

    private boolean isPeerConnecting(String name) {
        for(P2pPeer a : peers) {
            if (a.getName().equalsIgnoreCase(name))
                return a.getStatus() == P2pPeer.PeerStatus.PeerInvited;
        }
        return false;
    }

    private void discoverP2P() {
        if (mP2PManager != null && !mP2PDiscoveryState) {
            Log.d(TAG, "Starting Peers discovery");
            mP2PManager.discoverPeers(mChannel, new WigigP2pManager.ActionListener() {

                @Override
                public void onSuccess() {
                    Log.d(TAG, "P2P Peers discovery OK");
                }

                @Override
                public void onFailure(int reasonCode) {
                    Log.e(TAG, "P2P Peers discovery FAILED");
                    String errorMsg = getResources().getString(R.string.wfd_discovery_failed) + String.valueOf(reasonCode);
                    Toast.makeText(WigigWifiDirectSettings.this, errorMsg, Toast.LENGTH_SHORT).show();
                }
            });
        }
    }

    private void connectP2P(String name) {
        Log.d(TAG, "connectP2P()" + name);
        if (mP2PManager != null) {
            WifiP2pConfig config = new WifiP2pConfig();
            config.deviceAddress = peerGetAddressByName(name);
            if (config.deviceAddress.isEmpty())  {
                Toast.makeText(this, R.string.wfd_unable_to_connect, Toast.LENGTH_SHORT).show();
                return;
            }
            config.wps.setup = WpsInfo.PBC;
            mP2PManager.connect(mChannel, config, new WigigP2pManager.ActionListener() {

                @Override
                public void onSuccess() {
                    Log.d(TAG, "P2P connect OK");
                }

                @Override
                public void onFailure(int reasonCode) {
                    Log.e(TAG, "P2P connect FAILED");
                    String errorMsg = getResources().getString(R.string.wfd_connect_failed) + String.valueOf(reasonCode);
                    Toast.makeText(WigigWifiDirectSettings.this, errorMsg, Toast.LENGTH_SHORT).show();
                }
            });
            refreshP2PScreen();

        }
    }

    private void disconnectP2P() {
        Log.d(TAG, "disconnectP2P()");
        if (mP2PManager != null)  {
            mP2PManager.removeGroup(mChannel, null);
        }
    }

    private void cancelP2PConnect() {
        Log.d(TAG, "cancelP2PConnect()");
        if (mP2PManager != null)  {
            mP2PManager.cancelConnect(mChannel, null);
        }
    }

    private void disconnectAll() {
        Log.d(TAG, "disconnectAll()");
        if (mP2PManager != null) {
            for(P2pPeer a : peers) {
                if (a.getStatus() == P2pPeer.PeerStatus.PeerConnected) {
                    disconnectP2P();
                    break;
                }
            }
            for(P2pPeer a : peers) {
                if (a.getStatus() == P2pPeer.PeerStatus.PeerInvited) {
                    cancelP2PConnect();
                    break;
                }
            }
        }
    }

    private void displayP2PInfo(String name) {
        Log.d(TAG, "displayP2PInfo() " + name);
        String info = "Owner address: " + mP2PGroupAddress.toString() +
                " Group formed: " + mP2PGroupFormed +
                " Group owner: " + mP2PGroupOwner;
        Toast.makeText(this, info, Toast.LENGTH_SHORT).show();
    }

    private void stopOngoingActivity() {
        if (!isConnected() && mIsConnecting) {
            cancelP2PConnect();
        }
        if (mP2PDiscoveryState && mP2PManager != null)
            mP2PManager.stopPeerDiscovery(mChannel, null);
    }

    @SuppressWarnings("deprecation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "WigigWifiDirectSettings onCreate()");
        super.onCreate(savedInstanceState);

        if (!mLastMode.isEmpty()) {
            if (!mLastMode.equals(getResources().getString(R.string.p2p_mode))) {
                Log.d(TAG, "Switching to previous persistent mode " + mLastMode);
                if (mLastMode.equals(getResources().getString(R.string.hotspot_mode)))
                    mModeChange = MODE_TO_CHANGE_HOTSPOT | MODE_ALREADY_ENABLED;
                else if (mLastMode.equals(getResources().getString(R.string.ru_mode)))
                    mModeChange = MODE_TO_CHANGE_RU | MODE_ALREADY_ENABLED;
                onModeChange(mLastMode);
            }
        }

        Bundle b = getIntent().getExtras();
        if(b != null) {
            if (b.getBoolean(ENABLE_WIGIG_STR, false)) {
                if (!isWigigEnabled()) {
                    setWigigEnabled(true);
                }
            }
        }

        mPeersCategory = new PreferenceCategory(this);
        mRememberedGroupsCategory = new PreferenceCategory(this);
        mPeersCategory.setTitle(R.string.dwf_peers_category);
        mRememberedGroupsCategory.setTitle(R.string.dwf_groups_category);

        if (mP2PManager != null) {
            refreshP2PScreen();
            Log.d(TAG, "Initializing P2P");
            mChannel = mP2PManager.initialize(this, getMainLooper(), null);
        }
    }

    @SuppressWarnings("deprecation")
    @Override
    protected void onResume() {
        Log.d(TAG, "Enter WifiDirect onResume");
        super.onResume();
        if (mWigigManager != null) {
            ListPreference modePreference = (ListPreference) findPreference("wigig_mode");
            modePreference.setValue(getResources().getString(R.string.p2p_mode));
            modePreference.setSummary("%s");
            writeLastMode(getResources().getString(R.string.p2p_mode));
        }
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "Enter WifiDirect onPause");
        super.onPause();
        if (mWigigManager != null) {
            stopOngoingActivity();
            peers.clear();
        }
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.options_menu_wfd, menu);
        // Disable the search if already in discovery
        MenuItem searchItem = menu.findItem(R.id.menu_p2p_search);
        if (searchItem != null) {
            if (mP2PDiscoveryState) {
                searchItem.setEnabled(false);
            }
            else {
                searchItem.setEnabled(true);
            }
        }
        return true;
    }

    /*
     Create the context menu
    */
    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo info) {
        AdapterView.AdapterContextMenuInfo adinfo = (AdapterView.AdapterContextMenuInfo)info;
        Object o = getListView().getItemAtPosition(adinfo.position);
        Log.d(TAG, "OnCreateContextMenu() invoked object" + o);
        if (o != null) {
            if (o instanceof P2pPeer) {
                P2pPeer p = (P2pPeer)o;
                super.onCreateContextMenu(menu, v, info);
                MenuInflater inflater = getMenuInflater();
                if (isPeerConnected(p.getName())) {
                    inflater.inflate(R.menu.context_menu_conn_p2p, menu);
                }
                else if (isPeerInvited(p.getName())) {
                    inflater.inflate(R.menu.context_menu_invite_p2p, menu);
                }
            }
            else if (o instanceof P2pGroupPreference) {
                super.onCreateContextMenu(menu, v, info);
                MenuInflater inflater = getMenuInflater();
                inflater.inflate(R.menu.context_menu_group_p2p, menu);
            }
            else {
                Log.d(TAG, "No context menu for other preferences");
            }
        }
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        menu.findItem(R.id.menu_p2p_search).setEnabled(isWigigEnabled());
        menu.findItem(R.id.menu_p2p_changename).setEnabled(isWigigEnabled());

        return super.onPrepareOptionsMenu(menu);
    }

    /*
     Called upon the option menu selection
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        boolean r = true;
        switch (id) {
            case R.id.menu_p2p_search:
                discoverP2P();
                break;
            case R.id.menu_p2p_changename:
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                final EditText newname = new EditText(this);
                builder.setTitle(R.string.title_p2p_changename)
                        .setMessage(R.string.message_p2p_name_change);
                newname.setText(mP2PDeviceName);
                builder.setView(newname);
                builder.setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                String name = newname.getText().toString();
                                if (!(name.isEmpty() || name.equalsIgnoreCase(mP2PDeviceName))) {
                                    mP2PManager.setDeviceName(mChannel, name, new WigigP2pManager.ActionListener() {
                                        @Override
                                        public void onSuccess() {
                                            Toast.makeText(WigigWifiDirectSettings.this, R.string.wfd_name_changed, Toast.LENGTH_SHORT).show();
                                        }

                                        @Override
                                        public void onFailure(int reasonCode) {
                                            Log.e(TAG, "Device name change FAILED");
                                            String errorMsg = getResources().getString(R.string.wfd_name_change_failed) + String.valueOf(reasonCode);
                                            Toast.makeText(WigigWifiDirectSettings.this, errorMsg, Toast.LENGTH_SHORT).show();
                                        }
                                    });
                                }
                            }
                        });
                builder.setCancelable(true);
                builder.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
                AlertDialog alert = builder.create();
                alert.show();
                break;
            default:
                r = super.onOptionsItemSelected(item);
        }
        return r;
    }

    /*
    Called upon contxt menu selection
    */
    @Override
    public boolean onContextItemSelected(MenuItem item)
    {
        Log.d(TAG, "OnContextItemSelected()");
        AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
        Object o = getListView().getItemAtPosition(info.position);
        if (o != null) {
            switch (item.getItemId()) {
                case R.id.menu_context_p2p_disconnect:
                    if (o instanceof P2pPeer) {
                        P2pPeer p = (P2pPeer)o;
                        String name = p.getName();
                        Log.d(TAG, "Disconnecting from " + name);
                        disconnectP2P();
                    }
                    break;
                case R.id.menu_context_p2p_cancel_invite:
                    if (o instanceof P2pPeer) {
                        P2pPeer p = (P2pPeer)o;
                        String name = p.getName();
                        Log.d(TAG, "Cancelling invite from " + name);
                        cancelP2PConnect();
                    }
                    break;
                case R.id.menu_context_p2p_forget_group:
                    if (o instanceof P2pGroupPreference) {
                        P2pGroupPreference g = (P2pGroupPreference)o;
                        String name = g.getName();
                        int netid = g.getNetId();
                        Log.d(TAG, "Forgetting group " + name + " , Netid=" + netid);
                        if (mP2PManager != null && netid != -1) {
                            mP2PManager.deletePersistentGroup(mChannel, g.getNetId(), null);
                        }
                    }
                    break;
            }
        } else {
            Log.d(TAG, "Context menu object is  null");
        }

        return true;
    }
}
