/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2008 The Android Open Source Project
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

import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pGroup;
import android.net.wifi.p2p.nsd.WifiP2pServiceInfo;
import android.net.wifi.WpsInfo;
import android.text.TextUtils;
import android.util.LocalLog;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import vendor.qti.hardware.wigig.supptunnel.V1_0.ISuppTunnelCallback;

/**
 * Native calls for bring up/shut down of the supplicant daemon and for
 * sending requests to the supplicant daemon
 *
 * waitForEvent() is called on the monitor thread for events. All other methods
 * must be serialized from the framework.
 *
 * {@hide}
 */
public class WigigNative {

    private static final String TAG = "WigigNative";
    private static boolean DBG = true;
    private final String mTAG;
    private static final int DEFAULT_GROUP_OWNER_INTENT     = 6;

    // Hold this lock before calling supplicant - it is required to
    // mutually exclude access from Wifi and P2p state machines
    static final Object mLock = new Object();

    public final String mInterfaceName;
    public final String mInterfacePrefix;

    private SuppTunnelHal mSuppTunnelHal;

    public WigigNative(String interfaceName) {
        mInterfaceName = interfaceName;
        mTAG = "WigigNative-" + interfaceName;
        mInterfacePrefix = "IFNAME=" + interfaceName + " ";
        mSuppTunnelHal = SuppTunnelHal.getInstance();
    }

    void enableVerboseLogging(int verbose) {
        if (verbose > 0) {
            DBG = true;
        } else {
            DBG = false;
        }
    }

    private static final LocalLog mLocalLog = new LocalLog(16384);

    // hold mLock before accessing sCmdId
    private static int sCmdId;

    public LocalLog getLocalLog() {
        return mLocalLog;
    }

    private static int getNewCmdIdLocked() {
        return sCmdId++;
    }

    private void localLog(String s) {
        if (mLocalLog != null)
            mLocalLog.log(mInterfaceName + ": " + s);
    }

    public boolean loadDriver() {
        if (!mSuppTunnelHal.setConfigOption("driver_load_only", "1")) {
            Log.e(mTAG, "driver_load_only not supported");
            // proceed as if driver is already loaded
            return true;
        }
        return mSuppTunnelHal.startSupplicant();
    }

    public boolean unloadDriver() {
        if (!mSuppTunnelHal.setConfigOption("driver_load_only", "1")) {
            Log.e(mTAG, "driver_load_only not supported");
            // proceed as if operation succeeded; driver may remain loaded
            return true;
        }
        return mSuppTunnelHal.killSupplicant();
    }

    public boolean startSupplicant() {
        // startSupplicant also loads the driver
        mSuppTunnelHal.setConfigOption("driver_load_only", "0");
        return mSuppTunnelHal.startSupplicant();
    }

    public boolean killSupplicant() {
        // killSupplicant also unloads the driver
        mSuppTunnelHal.setConfigOption("driver_load_only", "0");
        return mSuppTunnelHal.killSupplicant();
    }

    public boolean registerCallback(ISuppTunnelCallback callback) {
        return mSuppTunnelHal.registerCallback(callback);
    }

    public boolean startHostapd() {
        // startHostapd also loads the driver
        mSuppTunnelHal.setConfigOption("driver_load_only", "0");
        return mSuppTunnelHal.startHostapd();
    }

    public boolean killHostapd() {
        // killHostapd also unloads the driver
        mSuppTunnelHal.setConfigOption("driver_load_only", "0");
        return mSuppTunnelHal.killHostapd();
    }

    private boolean doBooleanCommand(String command) {
        if (DBG) Log.d(mTAG, "doBoolean: " + command);
        String result = doStringCommand(command);
        return result.equals("OK");
    }

    private boolean doBooleanCommandWithIface(String iface, String command) {
        String result = doStringCommandWithIface(iface, command);
        return result.equals("OK");
    }

    private int doIntCommand(String command) {
        if (DBG) Log.d(mTAG, "doInt: " + command);
        String result = doStringCommand(command);
        int resultInt = -1;
        try {
            resultInt = Integer.parseInt(result);
        } catch (NumberFormatException e) {
            Log.e(mTAG, "bad number: " + result);
        }
        return resultInt;
    }

    private String doStringCommand(String command) {
        if (DBG) {
            //GET_NETWORK commands flood the logs
            if (!command.startsWith("GET_NETWORK")) {
                Log.d(mTAG, "doString: [" + command + "]");
            }
        }
        synchronized (mLock) {
            int cmdId = getNewCmdIdLocked();
            String toLog = Integer.toString(cmdId) + ":" + mInterfacePrefix + command;
            String result = mSuppTunnelHal.doCommand(mInterfacePrefix + command);
            if (result == null) {
                if (DBG) Log.d(mTAG, "doStringCommandNative no result");
                result = "";
            } else {
                if (!command.startsWith("STATUS-")) {
                    localLog(toLog + " -> " + result);
                }
                if (DBG) Log.d(mTAG, command + ": returned " + result);
            }
            return result;
        }
    }

    private String doStringCommandWithoutLogging(String command) {
        if (DBG) {
            //GET_NETWORK commands flood the logs
            if (!command.startsWith("GET_NETWORK")) {
                Log.d(mTAG, "doString: [" + command + "]");
            }
        }
        synchronized (mLock) {
            return mSuppTunnelHal.doCommand(mInterfacePrefix + command);
        }
    }

    private String doStringCommandWithIface(String iface, String command) {
        synchronized (mLock) {
            if (!TextUtils.isEmpty(iface))
                return mSuppTunnelHal.doCommand("IFNAME=" + iface + " " + command);
            else
                return mSuppTunnelHal.doCommand(command);
        }
    }

    private String doHostapdStringCommand(String command) {
        synchronized (mLock) {
            int cmdId = getNewCmdIdLocked();
            String toLog = Integer.toString(cmdId) + ":" + command;
            String result = mSuppTunnelHal.doHostapdCommand(command);
            if (result == null) {
                if (DBG) Log.d(mTAG, "doHostapdStringCommand no result");
                result = "";
            } else {
                if (!command.startsWith("STATUS-")) {
                    localLog(toLog + " -> " + result);
                }
                if (DBG) Log.d(mTAG, command + ": returned " + result);
            }
            return result;
        }
    }

    private boolean doHostapdBooleanCommand(String command) {
        if (DBG) Log.d(mTAG, "doHostapdBoolean: " + command);
        String result = doHostapdStringCommand(command);
        return result.equals("OK");
    }

    public boolean ping() {
        String pong = doStringCommand("PING");
        return (pong != null && pong.equals("PONG"));
    }

    public void setSupplicantLogLevel(String level) {
        doStringCommand("LOG_LEVEL " + level);
    }

    public boolean scan(String freqList) {
        if (freqList == null)
            return doBooleanCommand("SCAN");
        else
            return doBooleanCommand("SCAN freq=" + freqList);
    }

    /* Does a graceful shutdown of supplicant. Is a common stop function for both p2p and sta.
     *
     * Note that underneath we use a harsh-sounding "terminate" supplicant command
     * for a graceful stop and a mild-sounding "stop" interface
     * to kill the process
     */
    public boolean stopSupplicant() {
        return doBooleanCommand("TERMINATE");
    }

    public boolean stopHostapd() {
        return doHostapdBooleanCommand("TERMINATE");
    }

    public String listNetworks() {
        return doStringCommand("LIST_NETWORKS");
    }

    public String listNetworks(int last_id) {
        return doStringCommand("LIST_NETWORKS LAST_ID=" + last_id);
    }

    public int addNetwork() {
        return doIntCommand("ADD_NETWORK");
    }

    public boolean setNetworkVariable(int netId, String name, String value) {
        if (TextUtils.isEmpty(name) || TextUtils.isEmpty(value)) return false;
        return doBooleanCommand("SET_NETWORK " + netId + " " + name + " " + value);
    }

    public String getNetworkVariable(int netId, String name) {
        if (TextUtils.isEmpty(name)) return null;

        // GET_NETWORK will likely flood the logs ...
        return doStringCommandWithoutLogging("GET_NETWORK " + netId + " " + name);
    }

    public boolean removeNetwork(int netId) {
        return doBooleanCommand("REMOVE_NETWORK " + netId);
    }


    public boolean enableNetwork(int netId, boolean disableOthers) {
        if (DBG) Log.i(TAG, "enableNetwork nid=" + Integer.toString(netId)
                + " disableOthers=" + disableOthers);
        if (disableOthers) {
            return doBooleanCommand("SELECT_NETWORK " + netId);
        } else {
            return doBooleanCommand("ENABLE_NETWORK " + netId);
        }
    }

    public boolean disableNetwork(int netId) {
        if (DBG) Log.i(TAG, "disableNetwork nid=" + Integer.toString(netId));
        return doBooleanCommand("DISABLE_NETWORK " + netId);
    }

    public boolean selectNetwork(int netId) {
        if (DBG) Log.i(TAG, "selectNetwork nid=" + Integer.toString(netId));
        return doBooleanCommand("SELECT_NETWORK " + netId);
    }

    public boolean reconnect() {
        if (DBG) Log.i(TAG, "RECONNECT");
        return doBooleanCommand("RECONNECT");
    }

    public boolean disconnect() {
        if (DBG) Log.i(TAG, "DISCONNECT");
        return doBooleanCommand("DISCONNECT");
    }

    public String status() {
        return status(false);
    }

    public String status(boolean noEvents) {
        if (noEvents) {
            return doStringCommand("STATUS-NO_EVENTS");
        } else {
            return doStringCommand("STATUS");
        }
    }

    public String getMacAddress() {
        //Macaddr = XX.XX.XX.XX.XX.XX
        String ret = doStringCommand("DRIVER MACADDR");
        if (!TextUtils.isEmpty(ret)) {
            String[] tokens = ret.split(" = ");
            if (tokens.length == 2) return tokens[1];
        }
        return null;
    }

    /**
     * Format of results:
     * =================
     * id=1
     * bssid=68:7f:74:d7:1b:6e
     * freq=2412
     * level=-43
     * tsf=1344621975160944
     * ie=00000
     * age=2623
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=ssid1
     * ====
     *
     * RANGE=ALL gets all scan results
     * RANGE=ID- gets results from ID
     * MASK=<N> see wpa_supplicant/src/common/wpa_ctrl.h for details
     */
    public String scanResults(int sid) {
        return doStringCommandWithoutLogging("BSS RANGE=" + sid + "- MASK=0x21d87");
    }

    /**
     * Format of result:
     * id=1016
     * bssid=00:03:7f:40:84:10
     * freq=2462
     * beacon_int=200
     * capabilities=0x0431
     * qual=0
     * noise=0
     * level=-46
     * tsf=0000002669008476
     * age=5
     * ie=00105143412d485332302d52322d54455354010882848b960c12182403010b0706555...
     * flags=[WPA2-EAP-CCMP][ESS][P2P][HS20]
     * ssid=QCA-HS20-R2-TEST
     * p2p_device_name=
     * p2p_config_methods=0x0SET_NE
     * anqp_venue_name=02083d656e6757692d466920416c6c69616e63650a3239383920436f...
     * anqp_network_auth_type=010000
     * anqp_roaming_consortium=03506f9a05001bc504bd
     * anqp_ip_addr_type_availability=0c
     * anqp_nai_realm=0200300000246d61696c2e6578616d706c652e636f6d3b636973636f2...
     * anqp_3gpp=000600040132f465
     * anqp_domain_name=0b65786d61706c652e636f6d
     * hs20_operator_friendly_name=11656e6757692d466920416c6c69616e63650e636869...
     * hs20_wan_metrics=01c40900008001000000000a00
     * hs20_connection_capability=0100000006140001061600000650000106bb010106bb0...
     * hs20_osu_providers_list=0b5143412d4f53552d425353010901310015656e6757692d...
     */
    public String scanResult(String bssid) {
        return doStringCommand("BSS " + bssid);
    }

    public boolean startDriver() {
        return doBooleanCommand("DRIVER START");
    }

    public boolean stopDriver() {
        return doBooleanCommand("DRIVER STOP");
    }

    /**
      * Sets the bluetooth coexistence mode.
      *
      * @param mode One of {@link #BLUETOOTH_COEXISTENCE_MODE_DISABLED},
      *            {@link #BLUETOOTH_COEXISTENCE_MODE_ENABLED}, or
      *            {@link #BLUETOOTH_COEXISTENCE_MODE_SENSE}.
      * @return Whether the mode was successfully set.
      */
    public boolean setBluetoothCoexistenceMode(int mode) {
        // we don't need to do anything for 60G
        return true;
    }

    /**
     * Enable or disable Bluetooth coexistence scan mode. When this mode is on,
     * some of the low-level scan parameters used by the driver are changed to
     * reduce interference with A2DP streaming.
     *
     * @param isSet whether to enable or disable this mode
     * @return {@code true} if the command succeeded, {@code false} otherwise.
     */
    public boolean setBluetoothCoexistenceScanMode(boolean setCoexScanMode) {
        // we don't need to do anything for 60G
        return true;
    }

    public void enableSaveConfig() {
        doBooleanCommand("SET update_config 1");
    }

    public boolean saveConfig() {
        return doBooleanCommand("SAVE_CONFIG");
    }

    public boolean setCountryCode(String countryCode) {
        return doBooleanCommand("SET country " + countryCode.toUpperCase(Locale.ROOT));
    }

    /**
     * Start/Stop PNO scan.
     * @param enable boolean indicating whether PNO is being enabled or disabled.
     */
    public boolean setPnoScan(boolean enable) {
        String cmd = enable ? "SET pno 1" : "SET pno 0";
        return doBooleanCommand(cmd);
    }

    public void setScanInterval(int scanInterval) {
        doBooleanCommand("SCAN_INTERVAL " + scanInterval);
    }

    /** Example output:
     * RSSI=-65
     * LINKSPEED=48
     * NOISE=9999
     * FREQUENCY=0
     */
    public String signalPoll() {
        return doStringCommandWithoutLogging("SIGNAL_POLL");
    }

    public boolean startWpsPbc(String bssid) {
        if (TextUtils.isEmpty(bssid)) {
            return doBooleanCommand("WPS_PBC");
        } else {
            return doBooleanCommand("WPS_PBC " + bssid);
        }
    }

    public boolean startWpsPbc(String iface, String bssid) {
        if (TextUtils.isEmpty(bssid)) {
            return doBooleanCommandWithIface(iface, "WPS_PBC");
        } else {
            return doBooleanCommandWithIface(iface, "WPS_PBC " + bssid);
        }
    }

    public boolean startWpsPinKeypad(String pin) {
        if (TextUtils.isEmpty(pin)) return false;
        // note, wifi implementation has a bug. "WPS_PIN any <pin>" returns
        // the same pin, not "OK".
        String result = doStringCommand("WPS_PIN any " + pin);
        return !TextUtils.isEmpty(result);
    }

    public boolean startWpsPinKeypad(String iface, String pin) {
        if (TextUtils.isEmpty(pin)) return false;
        // note, wifi implementation has a bug. "WPS_PIN any <pin>" returns
        // the same pin, not "OK".
        String result = doStringCommandWithIface(iface, "WPS_PIN any " + pin);
        return !TextUtils.isEmpty(result);
    }


    public String startWpsPinDisplay(String bssid) {
        if (TextUtils.isEmpty(bssid)) {
            return doStringCommand("WPS_PIN any");
        } else {
            return doStringCommand("WPS_PIN " + bssid);
        }
    }

    public String startWpsPinDisplay(String iface, String bssid) {
        if (TextUtils.isEmpty(bssid)) {
            return doStringCommandWithIface(iface, " WPS_PIN any");
        } else {
            return doStringCommandWithIface(iface, " WPS_PIN " + bssid);
        }
    }

    public boolean simAuthResponse(int id, String type, String response) {
        // currently we don't support this feature
        return false;
    }

    public boolean simIdentityResponse(int id, String response) {
        synchronized (mLock) {
            return doBooleanCommand("CTRL-RSP-IDENTITY-" + id + ":" + response);
        }
    }

    /* Configures an access point connection */
    public boolean startWpsRegistrar(String bssid, String pin) {
        if (TextUtils.isEmpty(bssid) || TextUtils.isEmpty(pin)) return false;
        return doBooleanCommand("WPS_REG " + bssid + " " + pin);
    }

    public boolean cancelWps() {
        return doBooleanCommand("WPS_CANCEL");
    }

    public boolean setPersistentReconnect(boolean enabled) {
        int value = (enabled == true) ? 1 : 0;
        return doBooleanCommand("SET persistent_reconnect " + value);
    }

    public boolean setDeviceName(String name) {
        return doBooleanCommand("SET device_name " + name);
    }

    public boolean setDeviceType(String type) {
        return doBooleanCommand("SET device_type " + type);
    }

    public boolean setConfigMethods(String cfg) {
        return doBooleanCommand("SET config_methods " + cfg);
    }

    public boolean setManufacturer(String value) {
        return doBooleanCommand("SET manufacturer " + value);
    }

    public boolean setModelName(String value) {
        return doBooleanCommand("SET model_name " + value);
    }

    public boolean setModelNumber(String value) {
        return doBooleanCommand("SET model_number " + value);
    }

    public boolean setSerialNumber(String value) {
        return doBooleanCommand("SET serial_number " + value);
    }

    public boolean setP2pSsidPostfix(String postfix) {
        return doBooleanCommand("SET p2p_ssid_postfix " + postfix);
    }

    public boolean setP2pGroupIdle(String iface, int time) {
        return doBooleanCommandWithIface(iface, "SET p2p_group_idle " + time);
    }

    public boolean setP2pPowerSave(String iface, boolean enabled) {
        if (enabled) {
            return doBooleanCommandWithIface(iface, "P2P_SET ps 1");
        } else {
            return doBooleanCommandWithIface(iface, "P2P_SET ps 0");
        }
    }

    public boolean setWfdEnable(boolean enable) {
        return doBooleanCommand("SET wifi_display " + (enable ? "1" : "0"));
    }

    public boolean setWfdDeviceInfo(String hex) {
        return doBooleanCommand("WFD_SUBELEM_SET 0 " + hex);
    }

    /**
     * "sta" prioritizes STA connection over P2P and "p2p" prioritizes
     * P2P connection over STA
     */
    public boolean setConcurrencyPriority(String s) {
        return doBooleanCommand("P2P_SET conc_pref " + s);
    }

    public boolean p2pFind() {
        return doBooleanCommand("P2P_FIND");
    }

    public boolean p2pFind(int timeout) {
        if (timeout <= 0) {
            return p2pFind();
        }
        return doBooleanCommand("P2P_FIND " + timeout);
    }

    public boolean p2pStopFind() {
       return doBooleanCommand("P2P_STOP_FIND");
    }

    public boolean p2pListen() {
        return doBooleanCommand("P2P_LISTEN");
    }

    public boolean p2pListen(int timeout) {
        if (timeout <= 0) {
            return p2pListen();
        }
        return doBooleanCommand("P2P_LISTEN " + timeout);
    }

    public boolean p2pExtListen(boolean enable, int period, int interval) {
        if (enable && interval < period) {
            return false;
        }
        return doBooleanCommand("P2P_EXT_LISTEN"
                    + (enable ? (" " + period + " " + interval) : ""));
    }

    public boolean p2pSetChannel(int lc, int oc) {
        if (DBG) Log.d(mTAG, "p2pSetChannel: lc="+lc+", oc="+oc);

        if (lc >=1 && lc <= 11) {
            if (!doBooleanCommand("P2P_SET listen_channel " + lc)) {
                return false;
            }
        } else if (lc != 0) {
            return false;
        }

        if (oc >= 1 && oc <= 165 ) {
            int freq = (oc <= 14 ? 2407 : 5000) + oc * 5;
            return doBooleanCommand("P2P_SET disallow_freq 1000-"
                    + (freq - 5) + "," + (freq + 5) + "-6000");
        } else if (oc == 0) {
            /* oc==0 disables "P2P_SET disallow_freq" (enables all freqs) */
            return doBooleanCommand("P2P_SET disallow_freq \"\"");
        }

        return false;
    }

    public boolean p2pFlush() {
        return doBooleanCommand("P2P_FLUSH");
    }

    /* p2p_connect <peer device address> <pbc|pin|PIN#> [label|display|keypad]
        [persistent] [join|auth] [go_intent=<0..15>] [freq=<in MHz>] */
    public String p2pConnect(WifiP2pConfig config, boolean joinExistingGroup) {
        if (config == null) return null;
        List<String> args = new ArrayList<String>();
        WpsInfo wps = config.wps;
        args.add(config.deviceAddress);

        switch (wps.setup) {
            case WpsInfo.PBC:
                args.add("pbc");
                break;
            case WpsInfo.DISPLAY:
                if (TextUtils.isEmpty(wps.pin)) {
                    args.add("pin");
                } else {
                    args.add(wps.pin);
                }
                args.add("display");
                break;
            case WpsInfo.KEYPAD:
                args.add(wps.pin);
                args.add("keypad");
                break;
            case WpsInfo.LABEL:
                args.add(wps.pin);
                args.add("label");
            default:
                break;
        }

        if (config.netId == WifiP2pGroup.PERSISTENT_NET_ID) {
            args.add("persistent");
        }

        if (joinExistingGroup) {
            args.add("join");
        } else {
            //TODO: This can be adapted based on device plugged in state and
            //device battery state
            int groupOwnerIntent = config.groupOwnerIntent;
            if (groupOwnerIntent < 0 || groupOwnerIntent > 15) {
                groupOwnerIntent = DEFAULT_GROUP_OWNER_INTENT;
            }
            args.add("go_intent=" + groupOwnerIntent);
        }

        String command = "P2P_CONNECT ";
        for (String s : args) command += s + " ";

        return doStringCommand(command);
    }

    public boolean p2pCancelConnect() {
        return doBooleanCommand("P2P_CANCEL");
    }

    public boolean p2pProvisionDiscovery(WifiP2pConfig config) {
        if (config == null) return false;

        switch (config.wps.setup) {
            case WpsInfo.PBC:
                return doBooleanCommand("P2P_PROV_DISC " + config.deviceAddress + " pbc");
            case WpsInfo.DISPLAY:
                //We are doing display, so provision discovery is keypad
                return doBooleanCommand("P2P_PROV_DISC " + config.deviceAddress + " keypad");
            case WpsInfo.KEYPAD:
                //We are doing keypad, so provision discovery is display
                return doBooleanCommand("P2P_PROV_DISC " + config.deviceAddress + " display");
            default:
                break;
        }
        return false;
    }

    public boolean p2pGroupAdd(boolean persistent) {
        // TODO for now we start the autonomous GO on a fixed channel (2)
        if (persistent) {
            return doBooleanCommand("P2P_GROUP_ADD persistent freq=60480");
        }
        return doBooleanCommand("P2P_GROUP_ADD freq=60480");
    }

    public boolean p2pGroupAdd(int netId) {
        // TODO for now we start the autonomous GO on a fixed channel (2)
        return doBooleanCommand("P2P_GROUP_ADD freq=60480 persistent=" + netId);
    }

    public boolean p2pGroupRemove(String iface) {
        if (TextUtils.isEmpty(iface)) return false;
        return doBooleanCommandWithIface(iface, "P2P_GROUP_REMOVE " + iface);
    }

    public boolean p2pReject(String deviceAddress) {
        return doBooleanCommand("P2P_REJECT " + deviceAddress);
    }

    /* Invite a peer to a group */
    public boolean p2pInvite(WifiP2pGroup group, String deviceAddress) {
        if (TextUtils.isEmpty(deviceAddress)) return false;

        if (group == null) {
            return doBooleanCommand("P2P_INVITE peer=" + deviceAddress);
        } else {
            return doBooleanCommand("P2P_INVITE group=" + group.getInterface()
                    + " peer=" + deviceAddress + " go_dev_addr=" + group.getOwner().deviceAddress);
        }
    }

    /* Reinvoke a persistent connection */
    public boolean p2pReinvoke(int netId, String deviceAddress) {
        if (TextUtils.isEmpty(deviceAddress) || netId < 0) return false;

        return doBooleanCommand("P2P_INVITE persistent=" + netId + " peer=" + deviceAddress);
    }

    public String p2pGetSsid(String deviceAddress) {
        return p2pGetParam(deviceAddress, "oper_ssid");
    }

    public String p2pGetDeviceAddress() {

        Log.d(TAG, "p2pGetDeviceAddress");

        String status = null;

        /* Explicitly calling the API without IFNAME= prefix to take care of the devices that
        don't have p2p0 interface. Supplicant seems to be returning the correct address anyway. */

        status = doStringCommandWithIface(null, "STATUS");

        String result = "";
        if (status != null) {
            String[] tokens = status.split("\n");
            for (String token : tokens) {
                if (token.startsWith("p2p_device_address=")) {
                    String[] nameValue = token.split("=");
                    if (nameValue.length != 2)
                        break;
                    result = nameValue[1];
                }
            }
        }

        Log.d(TAG, "p2pGetDeviceAddress returning " + result);
        return result;
    }

    public int getGroupCapability(String deviceAddress) {
        int gc = 0;
        if (TextUtils.isEmpty(deviceAddress)) return gc;
        String peerInfo = p2pPeer(deviceAddress);
        if (TextUtils.isEmpty(peerInfo)) return gc;

        String[] tokens = peerInfo.split("\n");
        for (String token : tokens) {
            if (token.startsWith("group_capab=")) {
                String[] nameValue = token.split("=");
                if (nameValue.length != 2) break;
                try {
                    return Integer.decode(nameValue[1]);
                } catch(NumberFormatException e) {
                    return gc;
                }
            }
        }
        return gc;
    }

    public String p2pPeer(String deviceAddress) {
        return doStringCommand("P2P_PEER " + deviceAddress);
    }

    private String p2pGetParam(String deviceAddress, String key) {
        if (deviceAddress == null) return null;

        String peerInfo = p2pPeer(deviceAddress);
        if (peerInfo == null) return null;
        String[] tokens= peerInfo.split("\n");

        key += "=";
        for (String token : tokens) {
            if (token.startsWith(key)) {
                String[] nameValue = token.split("=");
                if (nameValue.length != 2) break;
                return nameValue[1];
            }
        }
        return null;
    }

    public boolean p2pServiceAdd(WifiP2pServiceInfo servInfo) {
        /*
         * P2P_SERVICE_ADD bonjour <query hexdump> <RDATA hexdump>
         * P2P_SERVICE_ADD upnp <version hex> <service>
         *
         * e.g)
         * [Bonjour]
         * # IP Printing over TCP (PTR) (RDATA=MyPrinter._ipp._tcp.local.)
         * P2P_SERVICE_ADD bonjour 045f697070c00c000c01 094d795072696e746572c027
         * # IP Printing over TCP (TXT) (RDATA=txtvers=1,pdl=application/postscript)
         * P2P_SERVICE_ADD bonjour 096d797072696e746572045f697070c00c001001
         *  09747874766572733d311a70646c3d6170706c69636174696f6e2f706f7374736372797074
         *
         * [UPnP]
         * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9332-123456789012
         * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9332-123456789012::upnp:rootdevice
         * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9332-123456789012::urn:schemas-upnp
         * -org:device:InternetGatewayDevice:1
         * P2P_SERVICE_ADD upnp 10 uuid:6859dede-8574-59ab-9322-123456789012::urn:schemas-upnp
         * -org:service:ContentDirectory:2
         */
        for (String s : servInfo.getSupplicantQueryList()) {
            String command = "P2P_SERVICE_ADD";
            command += (" " + s);
            if (!doBooleanCommand(command)) {
                return false;
            }
        }
        return true;
    }

    public boolean p2pServiceDel(WifiP2pServiceInfo servInfo) {
        /*
         * P2P_SERVICE_DEL bonjour <query hexdump>
         * P2P_SERVICE_DEL upnp <version hex> <service>
         */
        for (String s : servInfo.getSupplicantQueryList()) {
            String command = "P2P_SERVICE_DEL ";

            String[] data = s.split(" ");
            if (data.length < 2) {
                return false;
            }
            if ("upnp".equals(data[0])) {
                command += s;
            } else if ("bonjour".equals(data[0])) {
                command += data[0];
                command += (" " + data[1]);
            } else {
                return false;
            }
            if (!doBooleanCommand(command)) {
                return false;
            }
        }
        return true;
    }

    public boolean p2pServiceFlush() {
        return doBooleanCommand("P2P_SERVICE_FLUSH");
    }

    public String p2pServDiscReq(String addr, String query) {
        String command = "P2P_SERV_DISC_REQ";
        command += (" " + addr);
        command += (" " + query);

        return doStringCommand(command);
    }

    public boolean p2pServDiscCancelReq(String id) {
        return doBooleanCommand("P2P_SERV_DISC_CANCEL_REQ " + id);
    }

    /* Set the current mode of miracast operation.
     *  0 = disabled
     *  1 = operating as source
     *  2 = operating as sink
     */
    public void setMiracastMode(int mode) {
        // Note: optional feature on the driver. It is ok for this to fail.
        doBooleanCommand("DRIVER MIRACAST " + mode);
    }

    /*
     * NFC-related calls
     */
    public String getNfcWpsConfigurationToken(int netId) {
        return doStringCommand("WPS_NFC_CONFIG_TOKEN WPS " + netId);
    }

    public String getNfcHandoverRequest() {
        return doStringCommand("NFC_GET_HANDOVER_REQ NDEF P2P-CR");
    }

    public String getNfcHandoverSelect() {
        return doStringCommand("NFC_GET_HANDOVER_SEL NDEF P2P-CR");
    }

    public boolean initiatorReportNfcHandover(String selectMessage) {
        return doBooleanCommand("NFC_REPORT_HANDOVER INIT P2P 00 " + selectMessage);
    }

    public boolean responderReportNfcHandover(String requestMessage) {
        return doBooleanCommand("NFC_REPORT_HANDOVER RESP P2P " + requestMessage + " 00");
    }

    public boolean setSoftAp(String ifname, String ssid, boolean hidden, int channel,
                             boolean secured, String key, int maxNumSta, String countryCode) {
        String passphrase = (key != null) ? key : "";

        if (!mSuppTunnelHal.setHostapdConfigOption("ifname", ifname) ||
            !mSuppTunnelHal.setHostapdConfigOption("ssid", ssid) ||
            !mSuppTunnelHal.setHostapdConfigOption("hidden", hidden ? "1" : "0") ||
            !mSuppTunnelHal.setHostapdConfigOption("channel", Integer.toString(channel)) ||
            !mSuppTunnelHal.setHostapdConfigOption("secured", secured ? "1" : "0") ||
            !mSuppTunnelHal.setHostapdConfigOption("passphrase", passphrase) ||
            !mSuppTunnelHal.setHostapdConfigOption("max_num_sta", Integer.toString(maxNumSta)) ||
            (!TextUtils.isEmpty(countryCode) &&
                    !mSuppTunnelHal.setHostapdConfigOption("country_code", countryCode))) {
            Log.e(TAG, "failed to set hostapd config options");
            return false;
        }

        return true;
    }

    public boolean setWowlanTriggers(String triggers) {
        return doBooleanCommand("SET wowlan_triggers " + triggers);
    }

    public boolean setHostapdConfigWowlanTriggers(String triggers) {
        if (!mSuppTunnelHal.setHostapdConfigOption("wowlan_triggers", triggers)) {
            Log.e(TAG, "failed to set hostapd wowlan_triggers");
            return false;
        }

        return true;
    }
}
