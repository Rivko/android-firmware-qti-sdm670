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

import android.net.NetworkInfo;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiSsid;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pGroup;
import android.net.wifi.p2p.WifiP2pProvDiscEvent;
import android.net.wifi.p2p.nsd.WifiP2pServiceResponse;
import android.os.Message;
import android.util.Log;

import com.android.internal.util.StateMachine;
import com.qualcomm.qti.server.wigig.p2p.WigigP2pServiceImpl.P2pStatus;
import com.qualcomm.qti.wigig.WigigManager;

import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import vendor.qti.hardware.wigig.supptunnel.V1_0.ISuppTunnelCallback;

public class WigigMonitor {

    private static boolean DBG = true;
    private static final String TAG = "WigigMonitor";

    /** name of the P2P device interface used for P2P operations */
    private static final String P2P_DEVICE_INTERFACE = "p2p-dev-wigig0";

    /** All events coming from the supplicant start with this prefix */
    private static final String EVENT_PREFIX_STR = "CTRL-EVENT-";
    private static final int EVENT_PREFIX_LEN_STR = EVENT_PREFIX_STR.length();

    /** All WPA events coming from the supplicant start with this prefix */
    private static final String WPA_EVENT_PREFIX_STR = "WPA:";
    private static final String PASSWORD_MAY_BE_INCORRECT_STR =
       "pre-shared key may be incorrect";

    /* WPS events */
    private static final String WPS_SUCCESS_STR = "WPS-SUCCESS";

    /* Format: WPS-FAIL msg=%d [config_error=%d] [reason=%d (%s)] */
    private static final String WPS_FAIL_STR    = "WPS-FAIL";
    private static final String WPS_FAIL_PATTERN =
            "WPS-FAIL msg=\\d+(?: config_error=(\\d+))?(?: reason=(\\d+))?";

    /* config error code values for config_error=%d */
    private static final int CONFIG_MULTIPLE_PBC_DETECTED = 12;
    private static final int CONFIG_AUTH_FAILURE = 18;

    /* reason code values for reason=%d */
    private static final int REASON_TKIP_ONLY_PROHIBITED = 1;
    private static final int REASON_WEP_PROHIBITED = 2;

    private static final String WPS_OVERLAP_STR = "WPS-OVERLAP-DETECTED";
    private static final String WPS_TIMEOUT_STR = "WPS-TIMEOUT";

    //used to debug and detect if we miss an event
    private static int eventLogCounter = 0;

    // callback for supplicant events
    private static final ISuppTunnelCallback mSuppTunnelCallback = new ISuppTunnelCallback.Stub() {
        public void onEvent(String evt) {
            if (WigigMonitorSingleton.sInstance.dispatchEvent(evt)) {
                if (DBG) Log.d(TAG, "Disconnecting from the supplicant, no more events??");
            }
        }
    };

    /**
     * Names of events from wpa_supplicant (minus the prefix). In the
     * format descriptions, * &quot;<code>x</code>&quot;
     * designates a dynamic value that needs to be parsed out from the event
     * string
     */

    /**
     * <pre>
     * CTRL-EVENT-CONNECTED - Connection to xx:xx:xx:xx:xx:xx completed
     * </pre>
     * <code>xx:xx:xx:xx:xx:xx</code> is the BSSID of the associated access point
     */
    private static final String CONNECTED_STR =    "CONNECTED";

    /**
     * <pre>
     * CTRL-EVENT-DISCONNECTED - Disconnect event - remove keys
     * </pre>
     */
    private static final String DISCONNECTED_STR = "DISCONNECTED";

    /**
     * <pre>
     * CTRL-EVENT-STATE-CHANGE x
     * </pre>
     * <code>x</code> is the numerical value of the new state.
     */
    private static final String STATE_CHANGE_STR =  "STATE-CHANGE";

    /**
     * <pre>
     * CTRL-EVENT-SCAN-RESULTS ready
     * </pre>
     */
    private static final String SCAN_RESULTS_STR =  "SCAN-RESULTS";

    /**
     * <pre>
     * CTRL-EVENT-SCAN-FAILED ret=code[ retry=1]
     * </pre>
     */
    private static final String SCAN_FAILED_STR =  "SCAN-FAILED";

    /**
     * <pre>
     * CTRL-EVENT-TERMINATING - signal x
     * </pre>
     * <code>x</code> is the signal that caused termination.
     */
    private static final String TERMINATING_STR =  "TERMINATING";

    /**
     * This indicates an assoc reject event
     */
    private static final String ASSOC_REJECT_STR = "ASSOC-REJECT";

    /**
     * This indicates auth or association failure bad enough so as network got disabled
     * - WPA_PSK auth failure suspecting shared key mismatch
     * - failed multiple Associations
     */
    private static final String TEMP_DISABLED_STR = "SSID-TEMP-DISABLED";

    /**
     * This indicates a previously disabled SSID was reenabled by supplicant
     */
    private static final String REENABLED_STR = "SSID-REENABLED";

    /**
     * Regex pattern for extracting an Ethernet-style MAC address from a string.
     * Matches a strings like the following:<pre>
     * CTRL-EVENT-CONNECTED - Connection to 00:1e:58:ec:d5:6d completed (reauth) [id=1 id_str=]</pre>
     */
    private static Pattern mConnectedEventPattern =
        Pattern.compile("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) .* \\[id=([0-9]+) ");

    /**
     * Regex pattern for extracting an Ethernet-style MAC address from a string.
     * Matches a strings like the following:<pre>
     * CTRL-EVENT-DISCONNECTED - bssid=ac:22:0b:24:70:74 reason=3 locally_generated=1
     */
    private static Pattern mDisconnectedEventPattern =
            Pattern.compile("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) +" +
                    "reason=([0-9]+) +locally_generated=([0-1])");

    /**
     * Regex pattern for extracting an Ethernet-style MAC address from a string.
     * Matches a strings like the following:<pre>
     * CTRL-EVENT-ASSOC-REJECT - bssid=ac:22:0b:24:70:74 status_code=1
     */
    private static Pattern mAssocRejectEventPattern =
            Pattern.compile("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) +" +
                    "status_code=([0-9]+)");

    /**
     * Regex pattern for extracting an Ethernet-style MAC address from a string.
     * Matches a strings like the following:<pre>
     * IFNAME=wlan0 Associated with 6c:f3:7f:ae:87:71
     */
    private static final String ASSOCIATED_WITH_STR =  "Associated with ";

    private static Pattern mAssociatedPattern =
            Pattern.compile("Associated with ((?:[0-9a-f]{2}:){5}[0-9a-f]{2}).*");

    /** P2P events */
    private static final String P2P_EVENT_PREFIX_STR = "P2P";

    /* P2P-DEVICE-FOUND fa:7b:7a:42:02:13 p2p_dev_addr=fa:7b:7a:42:02:13 pri_dev_type=1-0050F204-1
       name='p2p-TEST1' config_methods=0x188 dev_capab=0x27 group_capab=0x0 */
    private static final String P2P_DEVICE_FOUND_STR = "P2P-DEVICE-FOUND";

    /* P2P-DEVICE-LOST p2p_dev_addr=42:fc:89:e1:e2:27 */
    private static final String P2P_DEVICE_LOST_STR = "P2P-DEVICE-LOST";

    /* P2P-FIND-STOPPED */
    private static final String P2P_FIND_STOPPED_STR = "P2P-FIND-STOPPED";

    /* P2P-GO-NEG-REQUEST 42:fc:89:a8:96:09 dev_passwd_id=4 */
    private static final String P2P_GO_NEG_REQUEST_STR = "P2P-GO-NEG-REQUEST";

    private static final String P2P_GO_NEG_SUCCESS_STR = "P2P-GO-NEG-SUCCESS";

    /* P2P-GO-NEG-FAILURE status=x */
    private static final String P2P_GO_NEG_FAILURE_STR = "P2P-GO-NEG-FAILURE";

    private static final String P2P_GROUP_FORMATION_SUCCESS_STR =
            "P2P-GROUP-FORMATION-SUCCESS";

    private static final String P2P_GROUP_FORMATION_FAILURE_STR =
            "P2P-GROUP-FORMATION-FAILURE";

    /* P2P-GROUP-STARTED p2p-wlan0-0 [client|GO] ssid="DIRECT-W8" freq=2437
       [psk=2182b2e50e53f260d04f3c7b25ef33c965a3291b9b36b455a82d77fd82ca15bc|passphrase="fKG4jMe3"]
       go_dev_addr=fa:7b:7a:42:02:13 [PERSISTENT] */
    private static final String P2P_GROUP_STARTED_STR = "P2P-GROUP-STARTED";

    /* P2P-GROUP-REMOVED p2p-wlan0-0 [client|GO] reason=REQUESTED */
    private static final String P2P_GROUP_REMOVED_STR = "P2P-GROUP-REMOVED";

    /* P2P-INVITATION-RECEIVED sa=fa:7b:7a:42:02:13 go_dev_addr=f8:7b:7a:42:02:13
        bssid=fa:7b:7a:42:82:13 unknown-network */
    private static final String P2P_INVITATION_RECEIVED_STR = "P2P-INVITATION-RECEIVED";

    /* P2P-INVITATION-ACCEPTED sa=fa:7b:7a:42:02:13 go_dev_addr=f8:7b:7a:42:02:13
        bssid=fa:7b:7a:42:82:13 unknown-network
       (unknown persistent group)
       or:
       P2P-INVITATION-ACCEPTED sa=fa:7b:7a:42:02:13 persistent=0
       (matching persistent group network block in configuration) */
    private static final String P2P_INVITATION_ACCEPTED_STR = "P2P-INVITATION-ACCEPTED";

    /* P2P-INVITATION-RESULT status=1 */
    private static final String P2P_INVITATION_RESULT_STR = "P2P-INVITATION-RESULT";

    /* P2P-PROV-DISC-PBC-REQ 42:fc:89:e1:e2:27 p2p_dev_addr=42:fc:89:e1:e2:27
       pri_dev_type=1-0050F204-1 name='p2p-TEST2' config_methods=0x188 dev_capab=0x27
       group_capab=0x0 */
    private static final String P2P_PROV_DISC_PBC_REQ_STR = "P2P-PROV-DISC-PBC-REQ";

    /* P2P-PROV-DISC-PBC-RESP 02:12:47:f2:5a:36 */
    private static final String P2P_PROV_DISC_PBC_RSP_STR = "P2P-PROV-DISC-PBC-RESP";

    /* P2P-PROV-DISC-ENTER-PIN 42:fc:89:e1:e2:27 p2p_dev_addr=42:fc:89:e1:e2:27
       pri_dev_type=1-0050F204-1 name='p2p-TEST2' config_methods=0x188 dev_capab=0x27
       group_capab=0x0 */
    private static final String P2P_PROV_DISC_ENTER_PIN_STR = "P2P-PROV-DISC-ENTER-PIN";
    /* P2P-PROV-DISC-SHOW-PIN 42:fc:89:e1:e2:27 44490607 p2p_dev_addr=42:fc:89:e1:e2:27
       pri_dev_type=1-0050F204-1 name='p2p-TEST2' config_methods=0x188 dev_capab=0x27
       group_capab=0x0 */
    private static final String P2P_PROV_DISC_SHOW_PIN_STR = "P2P-PROV-DISC-SHOW-PIN";
    /* P2P-PROV-DISC-FAILURE p2p_dev_addr=42:fc:89:e1:e2:27 */
    private static final String P2P_PROV_DISC_FAILURE_STR = "P2P-PROV-DISC-FAILURE";

    /*
     * Protocol format is as follows.<br>
     * See the Table.62 in the WiFi Direct specification for the detail.
     * ______________________________________________________________
     * |           Length(2byte)     | Type(1byte) | TransId(1byte)}|
     * ______________________________________________________________
     * | status(1byte)  |            vendor specific(variable)      |
     *
     * P2P-SERV-DISC-RESP 42:fc:89:e1:e2:27 1 0300000101
     * length=3, service type=0(ALL Service), transaction id=1,
     * status=1(service protocol type not available)<br>
     *
     * P2P-SERV-DISC-RESP 42:fc:89:e1:e2:27 1 0300020201
     * length=3, service type=2(UPnP), transaction id=2,
     * status=1(service protocol type not available)
     *
     * P2P-SERV-DISC-RESP 42:fc:89:e1:e2:27 1 990002030010757569643a3131323
     * 2646534652d383537342d353961622d393332322d3333333435363738393034343a3
     * a75726e3a736368656d61732d75706e702d6f72673a736572766963653a436f6e746
     * 56e744469726563746f72793a322c757569643a36383539646564652d383537342d3
     * 53961622d393333322d3132333435363738393031323a3a75706e703a726f6f74646
     * 576696365
     * length=153,type=2(UPnP),transaction id=3,status=0
     *
     * UPnP Protocol format is as follows.
     * ______________________________________________________
     * |  Version (1)  |          USN (Variable)            |
     *
     * version=0x10(UPnP1.0) data=usn:uuid:1122de4e-8574-59ab-9322-33345678
     * 9044::urn:schemas-upnp-org:service:ContentDirectory:2,usn:uuid:6859d
     * ede-8574-59ab-9332-123456789012::upnp:rootdevice
     *
     * P2P-SERV-DISC-RESP 58:17:0c:bc:dd:ca 21 1900010200045f6970
     * 70c00c000c01094d795072696e746572c027
     * length=25, type=1(Bonjour),transaction id=2,status=0
     *
     * Bonjour Protocol format is as follows.
     * __________________________________________________________
     * |DNS Name(Variable)|DNS Type(1)|Version(1)|RDATA(Variable)|
     *
     * DNS Name=_ipp._tcp.local.,DNS type=12(PTR), Version=1,
     * RDATA=MyPrinter._ipp._tcp.local.
     *
     */
    private static final String P2P_SERV_DISC_RESP_STR = "P2P-SERV-DISC-RESP";

    /* P2P-REMOVE-AND-REFORM-GROUP */
    /* Supplicant is supposed to generate this event only when p2p
     * is connected
     */
    private static final String P2P_REMOVE_AND_REFORM_GROUP_STR = "P2P-REMOVE-AND-REFORM-GROUP";

    private static final String HOST_AP_EVENT_PREFIX_STR = "AP";
    /* AP-STA-CONNECTED 42:fc:89:a8:96:09 dev_addr=02:90:4c:a0:92:54 */
    private static final String AP_STA_CONNECTED_STR = "AP-STA-CONNECTED";
    /* AP-STA-DISCONNECTED 42:fc:89:a8:96:09 */
    private static final String AP_STA_DISCONNECTED_STR = "AP-STA-DISCONNECTED";

    /* Supplicant events reported to a state machine */
    private static final int BASE = WigigManager.BASE_WIGIG_MONITOR;

    /* Connection to supplicant established */
    public static final int SUP_CONNECTION_EVENT                 = BASE + 1;
    /* Connection to supplicant lost */
    public static final int SUP_DISCONNECTION_EVENT              = BASE + 2;
   /* Network connection completed */
    public static final int NETWORK_CONNECTION_EVENT             = BASE + 3;
    /* Network disconnection completed */
    public static final int NETWORK_DISCONNECTION_EVENT          = BASE + 4;
    /* Scan results are available */
    public static final int SCAN_RESULTS_EVENT                   = BASE + 5;
    /* Supplicate state changed */
    public static final int SUPPLICANT_STATE_CHANGE_EVENT        = BASE + 6;
    /* Password failure and EAP authentication failure */
    public static final int AUTHENTICATION_FAILURE_EVENT         = BASE + 7;
    /* WPS success detected */
    public static final int WPS_SUCCESS_EVENT                    = BASE + 8;
    /* WPS failure detected */
    public static final int WPS_FAIL_EVENT                       = BASE + 9;
     /* WPS overlap detected */
    public static final int WPS_OVERLAP_EVENT                    = BASE + 10;
     /* WPS timeout detected */
    public static final int WPS_TIMEOUT_EVENT                    = BASE + 11;

    /* SSID was disabled due to auth failure or excessive
     * connection failures */
    public static final int SSID_TEMP_DISABLED                   = BASE + 13;
    /* SSID reenabled by supplicant */
    public static final int SSID_REENABLED                       = BASE + 14;

    /* Scan failure detected */
    public static final int SCAN_FAILED_EVENT                    = BASE + 17;

    /* P2P events */
    public static final int P2P_DEVICE_FOUND_EVENT               = BASE + 21;
    public static final int P2P_DEVICE_LOST_EVENT                = BASE + 22;
    public static final int P2P_GO_NEGOTIATION_REQUEST_EVENT     = BASE + 23;
    public static final int P2P_GO_NEGOTIATION_SUCCESS_EVENT     = BASE + 25;
    public static final int P2P_GO_NEGOTIATION_FAILURE_EVENT     = BASE + 26;
    public static final int P2P_GROUP_FORMATION_SUCCESS_EVENT    = BASE + 27;
    public static final int P2P_GROUP_FORMATION_FAILURE_EVENT    = BASE + 28;
    public static final int P2P_GROUP_STARTED_EVENT              = BASE + 29;
    public static final int P2P_GROUP_REMOVED_EVENT              = BASE + 30;
    public static final int P2P_INVITATION_RECEIVED_EVENT        = BASE + 31;
    public static final int P2P_INVITATION_RESULT_EVENT          = BASE + 32;
    public static final int P2P_PROV_DISC_PBC_REQ_EVENT          = BASE + 33;
    public static final int P2P_PROV_DISC_PBC_RSP_EVENT          = BASE + 34;
    public static final int P2P_PROV_DISC_ENTER_PIN_EVENT        = BASE + 35;
    public static final int P2P_PROV_DISC_SHOW_PIN_EVENT         = BASE + 36;
    public static final int P2P_FIND_STOPPED_EVENT               = BASE + 37;
    public static final int P2P_SERV_DISC_RESP_EVENT             = BASE + 38;
    public static final int P2P_PROV_DISC_FAILURE_EVENT          = BASE + 39;
    public static final int P2P_REMOVE_AND_REFORM_GROUP_EVENT    = BASE + 40;

    /* hostap events */
    public static final int AP_STA_DISCONNECTED_EVENT            = BASE + 41;
    public static final int AP_STA_CONNECTED_EVENT               = BASE + 42;

    /* Indicates assoc reject event */
    public static final int ASSOCIATION_REJECTION_EVENT          = BASE + 43;

    /* Additional P2P events */
    public static final int P2P_INVITATION_ACCEPTED_EVENT        = BASE + 100;

    private final String mInterfaceName;
    private final WigigNative mWigigNative;
    private final StateMachine mStateMachine;
    private boolean mMonitoring;

    public WigigMonitor(StateMachine stateMachine, WigigNative wigigNative) {
        if (DBG) Log.d(TAG, "Creating WigigMonitor");
        mWigigNative = wigigNative;
        mInterfaceName = wigigNative.mInterfaceName;
        mStateMachine = stateMachine;
        mMonitoring = false;

        WigigMonitorSingleton.sInstance.registerInterfaceMonitor(mInterfaceName, this);
    }

    public boolean startMonitoring() {
        return WigigMonitorSingleton.sInstance.startMonitoring(mInterfaceName);
    }

    public void stopMonitoring() {
        WigigMonitorSingleton.sInstance.stopMonitoring(mInterfaceName);
    }

    public void setP2pOverride(boolean override)
    {
        WigigMonitorSingleton.sInstance.setP2pOverride(override);
    }

    public void killSupplicant() {
        WigigMonitorSingleton.sInstance.killSupplicant();
    }

    private static class WigigMonitorSingleton {
        private static final WigigMonitorSingleton sInstance = new WigigMonitorSingleton();

        private final HashMap<String, WigigMonitor> mIfaceMap = new HashMap<String, WigigMonitor>();
        private boolean mCallbackRegistered = false;
        private WigigNative mWigigNative;
        private boolean mP2pOverride;

        private WigigMonitorSingleton() {
        }

        public void setP2pOverride(boolean override)
        {
            mP2pOverride = override;
        }

        public synchronized boolean startMonitoring(String iface) {
            WigigMonitor m = mIfaceMap.get(iface);
            if (m == null) {
                Log.e(TAG, "startMonitor called with unknown iface=" + iface);
                return false;
            }

            Log.d(TAG, "startMonitoring(" + iface + ") with mCallbackRegistered = " + mCallbackRegistered);

            if (!mCallbackRegistered) {
                if (!mWigigNative.registerCallback(mSuppTunnelCallback)) {
                    Log.e(TAG, "failed to register monitor callback");
                    return false;
                }
                mCallbackRegistered = true;
            }

            m.mMonitoring = true;
            m.mStateMachine.sendMessage(SUP_CONNECTION_EVENT);
            return true;
        }

        public synchronized void stopMonitoring(String iface) {
            WigigMonitor m = mIfaceMap.get(iface);
            if (DBG) Log.d(TAG, "stopMonitoring(" + iface + ") = " + m.mStateMachine);
            m.mMonitoring = false;
            m.mStateMachine.sendMessage(SUP_DISCONNECTION_EVENT);
        }

        public synchronized void registerInterfaceMonitor(String iface, WigigMonitor m) {
            mIfaceMap.put(iface, m);
            if (mWigigNative == null) {
                mWigigNative = m.mWigigNative;
            }
        }

        public synchronized void unregisterInterfaceMonitor(String iface) {
            // REVIEW: When should we call this? If this isn't called, then WigigMonitor
            // objects will remain in the mIfaceMap; and won't ever get deleted

            WigigMonitor m = mIfaceMap.remove(iface);
            if (DBG) Log.d(TAG, "unregisterInterface(" + iface + "+" + m.mStateMachine + ")");
        }

        public synchronized void killSupplicant() {
            Log.e(TAG, "killSupplicant");
            mWigigNative.killSupplicant();
            for (WigigMonitor m : mIfaceMap.values()) {
                m.mMonitoring = false;
            }
        }

        private synchronized boolean dispatchEvent(String eventStr) {
            String iface;
            if (eventStr.startsWith("IFNAME=")) {
                int space = eventStr.indexOf(' ');
                if (space != -1) {
                    iface = eventStr.substring(7, space);
                    eventStr = eventStr.substring(space + 1);
                } else {
                    // No point dispatching this event to any interface, the dispatched
                    // event string will begin with "IFNAME=" which dispatchEvent can't really
                    // do anything about.
                    Log.e(TAG, "Dropping malformed event (unparsable iface): " + eventStr);
                    return false;
                }
            } else {
                // events without prefix belong to p2p-dev-wigig0 monitor
                iface = P2P_DEVICE_INTERFACE;
            }

            if (DBG) Log.d(TAG, "Dispatching event to interface: " + iface);

            if (mP2pOverride) {
                iface = P2P_DEVICE_INTERFACE;
            }

            WigigMonitor m = mIfaceMap.get(iface);
            if (m != null) {
                if (m.mMonitoring) {
                    if (m.dispatchEvent(eventStr, iface)) {
                        return true;
                    }

                    return false;
                } else {
                    if (DBG) Log.d(TAG, "Dropping event because (" + iface + ") is stopped");
                    return false;
                }
            }

            if (DBG) Log.d(TAG, "Monitor not found for iface (" + iface + ")");
            return false;
        }
    }

    /* @return true if the event was supplicant disconnection */
    private boolean dispatchEvent(String eventStr, String iface) {

        if (DBG) {
            if (eventStr != null)
                Log.d(TAG, iface + " dispatchEvent: " + eventStr);
        }

        if (!eventStr.startsWith(EVENT_PREFIX_STR)) {
            if (eventStr.startsWith(WPA_EVENT_PREFIX_STR) &&
                    0 < eventStr.indexOf(PASSWORD_MAY_BE_INCORRECT_STR)) {
               mStateMachine.sendMessage(AUTHENTICATION_FAILURE_EVENT, eventLogCounter);
            } else if (eventStr.startsWith(WPS_SUCCESS_STR)) {
                mStateMachine.sendMessage(WPS_SUCCESS_EVENT);
            } else if (eventStr.startsWith(WPS_FAIL_STR)) {
                handleWpsFailEvent(eventStr);
            } else if (eventStr.startsWith(WPS_OVERLAP_STR)) {
                mStateMachine.sendMessage(WPS_OVERLAP_EVENT);
            } else if (eventStr.startsWith(WPS_TIMEOUT_STR)) {
                mStateMachine.sendMessage(WPS_TIMEOUT_EVENT);
            } else if (eventStr.startsWith(P2P_EVENT_PREFIX_STR)) {
                handleP2pEvents(eventStr);
            } else if (eventStr.startsWith(HOST_AP_EVENT_PREFIX_STR)) {
                handleHostApEvents(eventStr);
            } else if (eventStr.startsWith(ASSOCIATED_WITH_STR)) {
                handleAssociatedBSSIDEvent(eventStr);
            } else {
                if (DBG) Log.w(TAG, "couldn't identify event type - " + eventStr);
            }
            eventLogCounter++;
            return false;
        }

        String eventName = eventStr.substring(EVENT_PREFIX_LEN_STR);
        int nameEnd = eventName.indexOf(' ');
        if (nameEnd != -1)
            eventName = eventName.substring(0, nameEnd);
        if (eventName.length() == 0) {
            if (DBG) Log.i(TAG, "Received wpa_supplicant event with empty event name");
            eventLogCounter++;
            return false;
        }

        String eventData = eventStr;
        int ind = eventStr.indexOf(" - ");
        if (ind != -1) {
            eventData = eventStr.substring(ind + 3);
        }

        if (eventName.equals(CONNECTED_STR)) {
            handleNetworkStateChange(NetworkInfo.DetailedState.CONNECTED, eventData);
        }
        else if (eventName.equals(DISCONNECTED_STR)) {
            handleNetworkStateChange(NetworkInfo.DetailedState.DISCONNECTED, eventData);
        }
        else if (eventName.equals(STATE_CHANGE_STR)) {
            ind = eventStr.indexOf(" ");
            if (ind != -1) {
                eventData = eventStr.substring(ind + 1);
            }

            handleSupplicantStateChange(eventData);
        }
        else if (eventName.equals(SCAN_RESULTS_STR)) {
            mStateMachine.sendMessage(SCAN_RESULTS_EVENT);
        }
        else if (eventName.equals(SCAN_FAILED_STR)) {
            mStateMachine.sendMessage(SCAN_FAILED_EVENT);
        }
        else if (eventName.equals(TERMINATING_STR)) {
            mStateMachine.sendMessage(SUP_DISCONNECTION_EVENT);
            return true;
        }
        else if (eventName.equals(ASSOC_REJECT_STR)) {
            Matcher match = mAssocRejectEventPattern.matcher(eventData);
            String BSSID = "";
            int status = -1;
            if (!match.find()) {
                if (DBG) Log.d(TAG, "Assoc Reject: Could not parse assoc reject string");
            } else {
                BSSID = match.group(1);
                try {
                    status = Integer.parseInt(match.group(2));
                } catch (NumberFormatException e) {
                    status = -1;
                }
            }
            mStateMachine.sendMessage(ASSOCIATION_REJECTION_EVENT, eventLogCounter, status, BSSID);
        }
        else if (eventName.equals(TEMP_DISABLED_STR) || eventName.equals(REENABLED_STR)) {
            String substr = null;
            int netId = -1;
            ind = eventStr.indexOf(" ");
            if (ind != -1) {
                substr = eventStr.substring(ind + 1);
            }
            if (substr != null) {
                String status[] = substr.split(" ");
                for (String key : status) {
                    if (key.regionMatches(0, "id=", 0, 3)) {
                        int idx = 3;
                        netId = 0;
                        while (idx < key.length()) {
                            char c = key.charAt(idx);
                            if ((c >= 0x30) && (c <= 0x39)) {
                                netId *= 10;
                                netId += c - 0x30;
                                idx++;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
            mStateMachine.sendMessage(eventName.equals(TEMP_DISABLED_STR) ?
                    SSID_TEMP_DISABLED : SSID_REENABLED, netId, 0, substr);
        }
        else {
            if (DBG) Log.d(TAG, "event ignored");
        }

        eventLogCounter++;
        return false;
    }

    private void handleAssociatedBSSIDEvent(String eventStr) {
        String BSSID = null;
        Matcher match = mAssociatedPattern.matcher(eventStr);
        if (match.find()) {
            BSSID = match.group(1);
        }
        mStateMachine.sendMessage(WigigStateMachine.CMD_ASSOCIATED_BSSID, eventLogCounter, 0, BSSID);
    }

    private void handleWpsFailEvent(String dataString) {
        final Pattern p = Pattern.compile(WPS_FAIL_PATTERN);
        Matcher match = p.matcher(dataString);
        int reason = 0;
        if (match.find()) {
            String cfgErrStr = match.group(1);
            String reasonStr = match.group(2);

            if (reasonStr != null) {
                int reasonInt = Integer.parseInt(reasonStr);
                switch(reasonInt) {
                    case REASON_TKIP_ONLY_PROHIBITED:
                        mStateMachine.sendMessage(mStateMachine.obtainMessage(WPS_FAIL_EVENT,
                                WigigManager.WPS_TKIP_ONLY_PROHIBITED, 0));
                        return;
                    case REASON_WEP_PROHIBITED:
                        mStateMachine.sendMessage(mStateMachine.obtainMessage(WPS_FAIL_EVENT,
                                WigigManager.WPS_WEP_PROHIBITED, 0));
                        return;
                    default:
                        reason = reasonInt;
                        break;
                }
            }
            if (cfgErrStr != null) {
                int cfgErrInt = Integer.parseInt(cfgErrStr);
                switch(cfgErrInt) {
                    case CONFIG_AUTH_FAILURE:
                        mStateMachine.sendMessage(mStateMachine.obtainMessage(WPS_FAIL_EVENT,
                                WigigManager.WPS_AUTH_FAILURE, 0));
                        return;
                    case CONFIG_MULTIPLE_PBC_DETECTED:
                        mStateMachine.sendMessage(mStateMachine.obtainMessage(WPS_FAIL_EVENT,
                                WigigManager.WPS_OVERLAP_ERROR, 0));
                        return;
                    default:
                        if (reason == 0) reason = cfgErrInt;
                        break;
                }
            }
        }
        //For all other errors, return a generic internal error
        mStateMachine.sendMessage(mStateMachine.obtainMessage(WPS_FAIL_EVENT,
                WigigManager.ERROR, reason));
    }

    /* <event> status=<err> and the special case of <event> reason=FREQ_CONFLICT */
    private P2pStatus p2pError(String dataString) {
        P2pStatus err = P2pStatus.UNKNOWN;
        String[] tokens = dataString.split(" ");
        if (tokens.length < 2) return err;
        String[] nameValue = tokens[1].split("=");
        if (nameValue.length != 2) return err;

        /* Handle the special case of reason=FREQ+CONFLICT */
        if (nameValue[1].equals("FREQ_CONFLICT")) {
            return P2pStatus.NO_COMMON_CHANNEL;
        }
        try {
            err = P2pStatus.valueOf(Integer.parseInt(nameValue[1]));
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        return err;
    }

    WifiP2pDevice getWifiP2pDevice(String dataString) {
        try {
            WifiP2pDevice device = new WifiP2pDevice(dataString);
            return device;
        } catch (IllegalArgumentException e) {
            return null;
        }
    }

    WifiP2pGroup getWifiP2pGroup(String dataString) {
        try {
            WifiP2pGroup group = new WifiP2pGroup(dataString);
            return group;
        } catch (IllegalArgumentException e) {
            return null;
        }
    }

    /**
     * Converts hex string to byte array.
     *
     * @param hex hex string. if invalid, return null.
     * @return binary data.
     */
    private static byte[] hexStr2Bin(String hex) {
        int sz = hex.length()/2;
        byte[] b = new byte[hex.length()/2];

        for (int i=0;i<sz;i++) {
            try {
                b[i] = (byte)Integer.parseInt(hex.substring(i*2, i*2+2), 16);
            } catch (Exception e) {
                e.printStackTrace();
                return null;
            }
        }
        return b;
    }

    /**
     * Handle p2p events
     */
    private void handleP2pEvents(String dataString) {
        if (dataString.startsWith(P2P_DEVICE_FOUND_STR)) {
            WifiP2pDevice device = getWifiP2pDevice(dataString);
            if (device != null) mStateMachine.sendMessage(P2P_DEVICE_FOUND_EVENT, device);
        } else if (dataString.startsWith(P2P_DEVICE_LOST_STR)) {
            WifiP2pDevice device = getWifiP2pDevice(dataString);
            if (device != null) mStateMachine.sendMessage(P2P_DEVICE_LOST_EVENT, device);
        } else if (dataString.startsWith(P2P_FIND_STOPPED_STR)) {
            mStateMachine.sendMessage(P2P_FIND_STOPPED_EVENT);
        } else if (dataString.startsWith(P2P_GO_NEG_REQUEST_STR)) {
            mStateMachine.sendMessage(P2P_GO_NEGOTIATION_REQUEST_EVENT,
                    new WifiP2pConfig(dataString));
        } else if (dataString.startsWith(P2P_GO_NEG_SUCCESS_STR)) {
            mStateMachine.sendMessage(P2P_GO_NEGOTIATION_SUCCESS_EVENT);
        } else if (dataString.startsWith(P2P_GO_NEG_FAILURE_STR)) {
            mStateMachine.sendMessage(P2P_GO_NEGOTIATION_FAILURE_EVENT, p2pError(dataString));
        } else if (dataString.startsWith(P2P_GROUP_FORMATION_SUCCESS_STR)) {
            mStateMachine.sendMessage(P2P_GROUP_FORMATION_SUCCESS_EVENT);
        } else if (dataString.startsWith(P2P_GROUP_FORMATION_FAILURE_STR)) {
            mStateMachine.sendMessage(P2P_GROUP_FORMATION_FAILURE_EVENT, p2pError(dataString));
        } else if (dataString.startsWith(P2P_GROUP_STARTED_STR)) {
            WifiP2pGroup group = getWifiP2pGroup(dataString);
            if (group != null) mStateMachine.sendMessage(P2P_GROUP_STARTED_EVENT, group);
        } else if (dataString.startsWith(P2P_GROUP_REMOVED_STR)) {
            WifiP2pGroup group = getWifiP2pGroup(dataString);
            if (group != null) mStateMachine.sendMessage(P2P_GROUP_REMOVED_EVENT, group);
        } else if (dataString.startsWith(P2P_INVITATION_RECEIVED_STR)) {
            mStateMachine.sendMessage(P2P_INVITATION_RECEIVED_EVENT,
                    new WifiP2pGroup(dataString));
        } else if (dataString.startsWith(P2P_INVITATION_ACCEPTED_STR)) {
            mStateMachine.sendMessage(P2P_INVITATION_ACCEPTED_EVENT,
                    getWifiP2pGroupFromInvitationAcceptedEvent(dataString));
        } else if (dataString.startsWith(P2P_INVITATION_RESULT_STR)) {
            mStateMachine.sendMessage(P2P_INVITATION_RESULT_EVENT, p2pError(dataString));
        } else if (dataString.startsWith(P2P_PROV_DISC_PBC_REQ_STR)) {
            mStateMachine.sendMessage(P2P_PROV_DISC_PBC_REQ_EVENT,
                    new WifiP2pProvDiscEvent(dataString));
        } else if (dataString.startsWith(P2P_PROV_DISC_PBC_RSP_STR)) {
            mStateMachine.sendMessage(P2P_PROV_DISC_PBC_RSP_EVENT,
                    new WifiP2pProvDiscEvent(dataString));
        } else if (dataString.startsWith(P2P_PROV_DISC_ENTER_PIN_STR)) {
            mStateMachine.sendMessage(P2P_PROV_DISC_ENTER_PIN_EVENT,
                    new WifiP2pProvDiscEvent(dataString));
        } else if (dataString.startsWith(P2P_PROV_DISC_SHOW_PIN_STR)) {
            mStateMachine.sendMessage(P2P_PROV_DISC_SHOW_PIN_EVENT,
                    new WifiP2pProvDiscEvent(dataString));
        } else if (dataString.startsWith(P2P_PROV_DISC_FAILURE_STR)) {
            mStateMachine.sendMessage(P2P_PROV_DISC_FAILURE_EVENT);
        } else if (dataString.startsWith(P2P_SERV_DISC_RESP_STR)) {
            String[] args = dataString.split(" ");
            if (args.length != 4) {
                Log.e(TAG, "service response invalid event string: " + dataString);
            } else {
                String srcAddr = args[1];
                //String updateIndicator = args[2];//not used.
                byte[] tlvs = hexStr2Bin(args[3]);
                if (tlvs != null) {
                    List<WifiP2pServiceResponse> list = WifiP2pServiceResponse.newInstance(srcAddr, tlvs);
                    if (list != null) {
                        mStateMachine.sendMessage(P2P_SERV_DISC_RESP_EVENT, list);
                    } else {
                        Log.e(TAG, "Null service resp " + dataString);
                    }
                }
            }
        } else if (dataString.startsWith(P2P_REMOVE_AND_REFORM_GROUP_STR)) {
            Log.d(TAG, "Received event= " + dataString);
            mStateMachine.sendMessage(P2P_REMOVE_AND_REFORM_GROUP_EVENT);
        }
    }

    /**
     * Handle hostap events
     */
    private void handleHostApEvents(String dataString) {
        String[] tokens = dataString.split(" ");
        /* AP-STA-CONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=02:90:4c:a0:92:54 */
        if (tokens[0].equals(AP_STA_CONNECTED_STR)) {
            mStateMachine.sendMessage(AP_STA_CONNECTED_EVENT, new WifiP2pDevice(dataString));
            /* AP-STA-DISCONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=02:90:4c:a0:92:54 */
        } else if (tokens[0].equals(AP_STA_DISCONNECTED_STR)) {
            mStateMachine.sendMessage(AP_STA_DISCONNECTED_EVENT, new WifiP2pDevice(dataString));
        }
    }

    /**
     * Handle the supplicant STATE-CHANGE event
     * @param dataString New supplicant state string in the format:
     * id=network-id state=new-state
     */
    private void handleSupplicantStateChange(String dataString) {
        WifiSsid wifiSsid = null;
        int index = dataString.lastIndexOf("SSID=");
        if (index != -1) {
            wifiSsid = WifiSsid.createFromAsciiEncoded(
                    dataString.substring(index + 5));
        }
        String[] dataTokens = dataString.split(" ");

        String BSSID = null;
        int networkId = -1;
        int newState  = -1;
        for (String token : dataTokens) {
            String[] nameValue = token.split("=");
            if (nameValue.length != 2) {
                continue;
            }

            if (nameValue[0].equals("BSSID")) {
                BSSID = nameValue[1];
                continue;
            }

            int value;
            try {
                value = Integer.parseInt(nameValue[1]);
            } catch (NumberFormatException e) {
                continue;
            }

            if (nameValue[0].equals("id")) {
                networkId = value;
            } else if (nameValue[0].equals("state")) {
                newState = value;
            }
        }

        if (newState == -1) return;

        SupplicantState newSupplicantState = SupplicantState.INVALID;
        for (SupplicantState state : SupplicantState.values()) {
            if (state.ordinal() == newState) {
                newSupplicantState = state;
                break;
            }
        }
        if (newSupplicantState == SupplicantState.INVALID) {
            Log.w(TAG, "Invalid supplicant state: " + newState);
        }
        notifySupplicantStateChange(networkId, wifiSsid, BSSID, newSupplicantState);
    }

    private void handleNetworkStateChange(NetworkInfo.DetailedState newState, String data) {
        String BSSID = null;
        int networkId = -1;
        int reason = 0;
        int ind = -1;
        int local = 0;
        Matcher match;
        if (newState == NetworkInfo.DetailedState.CONNECTED) {
            match = mConnectedEventPattern.matcher(data);
            if (!match.find()) {
               if (DBG) Log.d(TAG, "handleNetworkStateChange: Couldnt find BSSID in event string");
            } else {
                BSSID = match.group(1);
                try {
                    networkId = Integer.parseInt(match.group(2));
                } catch (NumberFormatException e) {
                    networkId = -1;
                }
            }
            notifyNetworkStateChange(newState, BSSID, networkId, reason);
        } else if (newState == NetworkInfo.DetailedState.DISCONNECTED) {
            match = mDisconnectedEventPattern.matcher(data);
            if (!match.find()) {
               if (DBG) Log.d(TAG, "handleNetworkStateChange: Could not parse disconnect string");
            } else {
                BSSID = match.group(1);
                try {
                    reason = Integer.parseInt(match.group(2));
                } catch (NumberFormatException e) {
                    reason = -1;
                }
                try {
                    local = Integer.parseInt(match.group(3));
                } catch (NumberFormatException e) {
                    local = -1;
                }
            }
            notifyNetworkStateChange(newState, BSSID, local, reason);
        }
    }

    /**
     * Send the state machine a notification that the state of Wigig connectivity
     * has changed.
     * @param newState the new network state
     * @param BSSID when the new state is {@link NetworkInfo.DetailedState#CONNECTED},
     * this is the MAC address of the access point. Otherwise, it
     * is {@code null}.
     * @param netId the configured network on which the state change occurred
     */
    void notifyNetworkStateChange(NetworkInfo.DetailedState newState,
                                  String BSSID, int netId, int reason) {
        boolean connected = (newState == NetworkInfo.DetailedState.CONNECTED);
        if (DBG) Log.i(TAG, "notify network " + (connected ? "connect: " : "disconnect: ")
                + BSSID
                + " netId=" + Integer.toString(netId)
                + " reason=" + Integer.toString(reason));

        if (newState == NetworkInfo.DetailedState.CONNECTED) {
            Message m = mStateMachine.obtainMessage(NETWORK_CONNECTION_EVENT,
                    netId, reason, BSSID);
            mStateMachine.sendMessage(m);
        }
        else {
            Message m = mStateMachine.obtainMessage(NETWORK_DISCONNECTION_EVENT,
                    netId, reason, BSSID);
            mStateMachine.sendMessage(m);
        }
    }

    /**
     * Send the state machine a notification that the state of the supplicant
     * has changed.
     * @param networkId the configured network on which the state change occurred
     * @param wifiSsid network name
     * @param BSSID network address
     * @param newState the new {@code SupplicantState}
     */
    void notifySupplicantStateChange(int networkId, WifiSsid wifiSsid, String BSSID,
            SupplicantState newState) {
        mStateMachine.sendMessage(mStateMachine.obtainMessage(SUPPLICANT_STATE_CHANGE_EVENT,
                eventLogCounter, 0,
                new StateChangeResult(networkId, wifiSsid, BSSID, newState)));
    }

    /**
     * TODO WifiP2pGroup constructor does not recognize newly added
     * invitation accepted event. This code parses this
     * event to create a WifiP2pGroup. Remove this code once
     * WifiP2pGroup class is fixed (needs framework/base change)
     *
     * @param supplicantEvent the supplicant event string
     *
     * @return WifiP2pGroup
     */
    private WifiP2pGroup getWifiP2pGroupFromInvitationAcceptedEvent(String supplicantEvent)
    {
        String[] tokens = supplicantEvent.split(" ");

        if (tokens.length < 3) {
            throw new IllegalArgumentException("Malformed supplicant event");
        }

        WifiP2pGroup group = new WifiP2pGroup();
        String sa = null;
        group.setNetworkId(WifiP2pGroup.PERSISTENT_NET_ID);

        for (String token : tokens) {
            String[] nameValue = token.split("=");
            if (nameValue.length != 2) continue;

            if (nameValue[0].equals("sa")) {
                sa = nameValue[1];

                // set source address into the client list.
                WifiP2pDevice dev = new WifiP2pDevice();
                dev.deviceAddress = nameValue[1];
                group.addClient(dev);
                continue;
            }

            if (nameValue[0].equals("go_dev_addr")) {
                WifiP2pDevice owner = new WifiP2pDevice(nameValue[1]);
                group.setOwner(owner);
                continue;
            }

            if (nameValue[0].equals("persistent")) {
                WifiP2pDevice owner = new WifiP2pDevice(sa);
                int netId = Integer.parseInt(nameValue[1]);
                group.setOwner(owner);
                group.setNetworkId(netId);
                continue;
            }
        }

        return group;
    }
}
