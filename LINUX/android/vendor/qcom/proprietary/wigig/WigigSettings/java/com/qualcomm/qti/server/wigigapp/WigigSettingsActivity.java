/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.server.wigigapp;

import android.content.Intent;
import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.ContextMenu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Toast;
import android.view.Menu;
import android.view.MenuInflater;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.regex.Pattern;

import android.os.Handler;

import com.qualcomm.qti.wigig.WigigManager;
import com.qualcomm.qti.wigig.ScanResult;

/*
 WiGig settings activity
 */

public class WigigSettingsActivity extends WigigActivity {
    private static final String TAG = "WigigSettings";

    private static final int SCAN_IMMEDIATE = 0;
    private static final int SCAN_INTERVAL_MS = 10000;
    private final List<AccessPoint> accessPoints;


    private WigigDialog mDialog;
    private WigigManager.ActionListener mConnectListener;
    private WigigManager.ActionListener mSaveListener;
    private WigigManager.ActionListener mForgetListener;

    private boolean isScanStarted = false;

    private int mModeChange = MODE_NOT_CHANGED;

    public WigigSettingsActivity() {
        super(R.xml.pref_general, new String[] {WigigManager.SCAN_RESULTS_AVAILABLE_ACTION} );
        accessPoints = new ArrayList<>();
    }

    @Override
    protected void handleEvent(Intent intent) {
        super.handleEvent(intent);
        final String action = intent.getAction();
        if (action.equals(WigigManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
            parseAndAddAccessPoint();
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
        syncTimerRunnable(false, SCAN_IMMEDIATE);
        removeAllAccessPoints();
    }

    @Override
    protected boolean onModeChange(String mode) {
        if (mode.equals(getResources().getString(R.string.p2p_mode))) {
            if (isConnected()) {
                Log.d(TAG, "Should start P2P Activity after disconnect");
                modeSwitchDelayed = mode;
                mWigigManager.disconnect();
            }
            else {
                modeSwitchDelayed = "";
                Log.d(TAG, "Starting P2P Activity");
                switchWigigMode(WigigSettingsActivity.this, WigigWifiDirectSettings.class, false);
            }
            return false;   // // Don't update the mode menu, the new activity will do that if switched
        }
        else if (mode.equals(getResources().getString(R.string.hotspot_mode))) {
            if ((mModeChange & MODE_TO_CHANGE_HOTSPOT) == MODE_TO_CHANGE_HOTSPOT) {
                switchWigigMode(WigigSettingsActivity.this, SoftAP.class,
                        (mModeChange & MODE_ALREADY_ENABLED) != MODE_ALREADY_ENABLED);
                mModeChange = MODE_NOT_CHANGED;
            } else {
                mModeChange = mModeChange | MODE_TO_CHANGE_HOTSPOT;
                setWigigEnabled(false);
            }
        }
        else if (mode.equals(getResources().getString(R.string.ru_mode))) {
            if ((mModeChange & MODE_TO_CHANGE_RU) == MODE_TO_CHANGE_RU) {
                switchWigigMode(WigigSettingsActivity.this, WigigRateUpgradeSettings.class,
                        (mModeChange & MODE_ALREADY_ENABLED) != MODE_ALREADY_ENABLED);
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
        syncTimerRunnable(true, SCAN_IMMEDIATE);
    }

    @Override
    protected void onWigigFrameworkDisabled() {
        super.onWigigFrameworkDisabled();
        Log.d(TAG, "onWigigFrameworkDisabled()");
        syncTimerRunnable(false, SCAN_IMMEDIATE);
        removeAllAccessPoints();
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
            if (states[INITIAL_P2P_CONNECTED_STATE]) {
                // WiGig enabled and P2P peer connected, switch to P2P mode
                switchWigigModeWithAlert(WigigSettingsActivity.this, WigigWifiDirectSettings.class);
            }
        }
    }

    @Override
    protected void onNetworkStateChanged(DetailedState ds, int networkId) {
        super.onNetworkStateChanged(ds, networkId);
        if (ds != DetailedState.IDLE) {
            Log.d(TAG, "onNetworkStateChanged() ds: " + ds.toString());
        }
        if (ds == DetailedState.DISCONNECTED) {
            setConnected(false);
            if (!modeSwitchDelayed.isEmpty()) {
                onModeChange(modeSwitchDelayed);
                return;
            }
        } else if (ds.ordinal() >= DetailedState.CONNECTING.ordinal() &&
                ds.ordinal() <= DetailedState.CONNECTED.ordinal())
            setConnected(true);

        /* On activity change, prevent from accessing the supplicant before it is ready */
        if (!(ds == DetailedState.DISCONNECTED && networkId == -1))
            parseAndAddAccessPoint();
    }

    @Override
    protected void onP2PStateChanged(NetworkInfo networkInfo) {
        super.onP2PStateChanged(networkInfo);
        Log.d(TAG, "onP2PStateChanged()");
        if (mP2PManager != null) {
            Log.d(TAG, "P2P_CONNECTION_CHANGED_ACTION " + networkInfo.toString());
            if (!isInitialStateObtaining() && networkInfo.isConnected()) {
                switchWigigModeWithAlert(WigigSettingsActivity.this, WigigWifiDirectSettings.class);
            }
        }
    }

    private final Handler timerHandler = new Handler();

    private final Runnable timerRunnable = new Runnable() {
        @Override
        public void run() {
            final List<WifiConfiguration> wifiConfiguration = mWigigManager.getConfiguredNetworks();
            boolean noSavedNetworks = (wifiConfiguration == null || wifiConfiguration.isEmpty());
            Log.i(TAG, "isConnected " + isConnected() + " noSavedNetworks " + noSavedNetworks);
            if (isConnected() || noSavedNetworks) {
                Log.i(TAG, "trigger periodic scan");
                mWigigManager.startScan();
            }
            else {
                parseAndAddAccessPoint();
            }

            syncTimerRunnable(true, SCAN_INTERVAL_MS);
        }
    };

    private synchronized void syncTimerRunnable(boolean isEnable, int scanInterval) {
        if (isEnable && isWigigEnabled()) {
            /* Do not enter when the timer is enabled on consecutive calls (no disable in between)
               and do not enter when the timer is being disabled during the timer callback execution */
            if ((isScanStarted && (scanInterval > SCAN_IMMEDIATE)) ||
                (!isScanStarted && (scanInterval == SCAN_IMMEDIATE))) {
                timerHandler.postDelayed(timerRunnable, scanInterval);
                isScanStarted = true;
            }
        }
        else {
            timerHandler.removeCallbacks(timerRunnable);
            isScanStarted = false;
        }
    }

    @SuppressWarnings("deprecation") // For use getPreferenceScreen() in activity rather than a fragment
    private void removeAllAccessPoints() {
        PreferenceScreen p = getPreferenceScreen();
        int accessPointsSize = accessPoints.size();
        for (int i = 0; i < accessPointsSize; ++i){
            p.removePreference(accessPoints.get(i));
        }
        accessPoints.clear();
    }

    @SuppressWarnings("deprecation") // For use getPreferenceScreen() in activity rather than a fragment
    private AccessPoint addAccessPoint(String ssid, int rssi, int security) {
        PreferenceScreen p = getPreferenceScreen();
        AccessPoint a = new AccessPoint(this, ssid, rssi, security);
        a.setOnPreferenceClickListener(sClickPreferenceListener);
        accessPoints.add(a);
        p.addPreference(a);
        return a;
    }

    private void filterOutDuplicatedAps(List<ScanResult> scanResults) {
        int outerIndex, innerIndex;

        if (scanResults == null || scanResults.size() < 2) {
            return;
        }

        for (outerIndex = 0; outerIndex < scanResults.size() - 1; outerIndex++) {
            ScanResult outerSr = scanResults.get(outerIndex);
            boolean isOuterSrSecured = outerSr.capabilities.contains("WPA");
            for (innerIndex = outerIndex + 1; innerIndex < scanResults.size(); innerIndex++) {
                ScanResult innerSr = scanResults.get(innerIndex);
                boolean isInnerSrSecured = innerSr.capabilities.contains("WPA");
                if (outerSr.SSID.equals(innerSr.SSID) && (isOuterSrSecured == isInnerSrSecured)) {
                    if (outerSr.level < innerSr.level)
                        outerSr.level = innerSr.level;
                    scanResults.remove(innerIndex);
                    innerIndex--;
                }
            }
        }
    }

    /*
    Gets a list of access points, parse, and configure them one by one.
    */
    private void parseAndAddAccessPoint() {
        removeAllAccessPoints();
        int index, index2;
        boolean wcItemRemoved = false;
        final List<WifiConfiguration> wifiConfiguration = mWigigManager.getConfiguredNetworks();
        final List<ScanResult> scanResults = mWigigManager.getScanResults();
        WifiInfo info = mWigigManager.getConnectionInfo();
        DetailedState ds = WifiInfo.getDetailedStateOf(info.getSupplicantState());

        filterOutDuplicatedAps(scanResults);

        if (scanResults != null) {
            /* Init the seen parameter in the scan result list.
             * This parameter is used to store the network id and determine the
             * summary of the access point
             *
             * Init the autoJoinStatus in the scan result list.
             * This parameter is used to indicate if the AP has an authentication failure
             * */
            for (ScanResult sr : scanResults) {
                sr.seen = -1;
                sr.autoJoinStatus = -1;
            }
        }
        // search for connected AP and store networkId in "seen" parameter
        String connectedApSsid = null;
        int connectedApLevel = 50;
        int connectedApSecurity = AccessPoint.SECURITY_NONE;
        for (index = 0; wifiConfiguration != null && index < wifiConfiguration.size(); index++) {
            WifiConfiguration wc = wifiConfiguration.get(index);
            String currSsid = wc.SSID.replace("\"", "");
            boolean isWcSecured = wifiConfiguration.get(index).allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK);
            if (wc.status == WifiConfiguration.Status.CURRENT) {
                connectedApSsid = new String(currSsid);
                connectedApSecurity =
                        isWcSecured ? AccessPoint.SECURITY_PSK : AccessPoint.SECURITY_NONE;
                wifiConfiguration.remove(index);
                wcItemRemoved = true;
            }

            if (scanResults != null) {
                for (index2 = 0; index2 < scanResults.size(); index2++) {
                    ScanResult sc = scanResults.get(index2);
                    Pattern pattern = Pattern.compile(Pattern.quote(sc.SSID));
                    boolean isScSecured = sc.capabilities.contains("WPA");
                    if (pattern.matcher(currSsid).matches() && isScSecured == isWcSecured) {
                        if (wc.status == WifiConfiguration.Status.CURRENT) {
                            connectedApLevel = sc.level;
                            scanResults.remove(index2);
                        }
                        else {
                            sc.seen = wc.networkId;
                            if (wc.status == WifiConfiguration.Status.DISABLED) {
                                if (isAuthenticationProblem(wc)) {
                                    sc.autoJoinStatus = AccessPoint.AP_AUTHENTICATION_PROBLEM;
                                }
                            }

                            wifiConfiguration.remove(index);
                            wcItemRemoved = true;
                        }
                    }
                }
            }
            if (wcItemRemoved) {
                index--; //move to next node only if we didnt remove the current node
                wcItemRemoved = false;
            }
        }

        /* first, print the connected ap */
        if (connectedApSsid != null) {
            AccessPoint a = addAccessPoint(connectedApSsid, connectedApLevel, connectedApSecurity);
            a.setSummary(R.string.wifi_connected);
        }

        /* then, print the scan results */
        if (scanResults != null) {
            for (ScanResult sr : scanResults) {
                boolean isApSecured = sr.capabilities.contains("WPA");
                AccessPoint a = addAccessPoint(sr.SSID, sr.level,
                        isApSecured ? AccessPoint.SECURITY_PSK : AccessPoint.SECURITY_NONE);

                if (sr.seen >= 0) {
                    if (sr.seen == info.getNetworkId()) {
                        Pattern pattern = Pattern.compile(Pattern.quote(sr.SSID));
                        String infoSsid = info.getSSID().replace("\"", "");
                        if (pattern.matcher(infoSsid).matches()) {
                            if (ds == DetailedState.CONNECTING)
                                a.setSummary(R.string.wifi_connecting);
                            else if (ds == DetailedState.OBTAINING_IPADDR)
                                a.setSummary(R.string.wifi_obtaining_ipaddr);
                            else if (ds == DetailedState.AUTHENTICATING)
                                a.setSummary(R.string.wifi_authenticating);
                            else
                                a.setSummary(R.string.wifi_saved);
                        }
                    } else if (sr.autoJoinStatus == AccessPoint.AP_AUTHENTICATION_PROBLEM)
                        a.setSummary(R.string.wifi_authentication_problem);
                    else
                        a.setSummary(R.string.wifi_saved);
                }
            }
        }

        /* and then, print the saved aps */
        if (wifiConfiguration != null) {
            for (WifiConfiguration wc : wifiConfiguration) {
                boolean isApSecured = wc.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK);
                AccessPoint a = addAccessPoint(wc.SSID.replace("\"", ""), 0,
                        isApSecured ? AccessPoint.SECURITY_PSK : AccessPoint.SECURITY_NONE);
                a.setSummary(R.string.wifi_saved);
            }
        }
    }

    public static WifiConfiguration getWifiConfigurationByAp(WigigManager manager, AccessPoint accessPoint) {
        return (getWifiConfigurationByParams(manager, accessPoint.ssid,
                accessPoint.security == AccessPoint.SECURITY_PSK));
    }

    public static WifiConfiguration getWifiConfigurationByParams(WigigManager manager, String SSID, boolean isApSecured) {
        final List<WifiConfiguration> wifiConfiguration = manager.getConfiguredNetworks();
        for (WifiConfiguration wc : wifiConfiguration) {
            String r_ssid = wc.SSID.replace("\"", "");
            Pattern pattern = Pattern.compile(Pattern.quote(r_ssid));
            boolean isWcSecured = wc.allowedKeyManagement.get(WifiConfiguration.KeyMgmt.WPA_PSK);
            if (pattern.matcher(SSID).matches() && isWcSecured == isApSecured) {
                return wc;
            }
        }
        return null;
    }

    private boolean isAuthenticationProblem(WifiConfiguration wc)
    {
        try {
            // android M
            int disableReason = (int)(wc.getClass().getField("disableReason").get(wc));
            int auth_failure = (int)(wc.getClass().getField("DISABLED_AUTH_FAILURE").get(wc));
            return (disableReason == auth_failure);
        } catch (Throwable e) {
            Log.e(TAG, "Exception. Failed to get disableReason (M)!");
        }

        try {
            // android N
            Method m = wc.getClass().getMethod("getNetworkSelectionStatus");
            Object obj = m.invoke(wc);
            m = obj.getClass().getMethod("getNetworkSelectionDisableReason");
            int disableReason = (Integer)m.invoke(obj);
            int auth_failure = (int)(obj.getClass().getField("DISABLED_AUTHENTICATION_FAILURE").get(obj));
            return (disableReason == auth_failure);
        } catch (Throwable e) {
            Log.e(TAG, "Exception. Failed to get disableReason (N)!");
        }

        return false;
    }

    private WifiConfiguration getConfig(AccessPoint accessPoint){ //FIXME - unite this with getConfig at WigigDialog.java
        WifiConfiguration config;

        config = new WifiConfiguration();

        config.SSID = accessPoint.ssid;
        config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);

        return config;
    }

    private final Preference.OnPreferenceClickListener sClickPreferenceListener = new Preference.OnPreferenceClickListener() {
        @Override
        public boolean onPreferenceClick(Preference preference) {

            if (preference instanceof AccessPoint) {
                AccessPoint pref = (AccessPoint)preference;

                WifiConfiguration config = getWifiConfigurationByAp(mWigigManager, pref);
                if (config != null) {
                    switch (pref.GetApStatus()) {
                        case AccessPoint.AP_SAVED_AWAY:
                        case AccessPoint.AP_SAVED:
                        case AccessPoint.AP_AUTHENTICATION_PROBLEM:
                            if (!mDialog.isShowing())
                                mDialog.CreateDialog(pref, false);
                            break;
                        default:
                            break;
                    }
                }
                else {
                    if (pref.security == AccessPoint.SECURITY_PSK) {
                        if (!mDialog.isShowing())
                            mDialog.CreateDialog(pref, true);
                    } else {
                        config = getConfig(pref);
                        connect(config);
                    }
                }
            }
            return true;
        }
    };

    public void save(final WifiConfiguration config) {
        mWigigManager.save(config, mSaveListener);
        parseAndAddAccessPoint();
    }

    public void connect(final int networkId) {
        mWigigManager.connect(networkId, mConnectListener);
    }

    public void connect(final WifiConfiguration config) {
        mWigigManager.connect(config, mConnectListener);
    }

    void forget(final int networkId) {
        mWigigManager.forget(networkId, mForgetListener);
        parseAndAddAccessPoint();
    }

    @SuppressWarnings("deprecation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "WigigSettingsActivity onCreate()");
        super.onCreate(savedInstanceState);

        if (!mLastMode.isEmpty()) {
            if (!mLastMode.equals(getResources().getString(R.string.network_mode))) {
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

        mDialog = new WigigDialog(this, mWigigManager);

        if (isWigigEnabled()) {
            mConnectListener = new WigigManager.ActionListener() {
                @Override
                public void onSuccess() {
                    Log.d(TAG, "mConnectListener onSuccess");
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            parseAndAddAccessPoint();
                        }
                    });
                }

                @Override
                public void onFailure(int reason) {
                    Toast.makeText(WigigSettingsActivity.this, R.string.Connect_failure, Toast.LENGTH_SHORT).show();
                    Log.e(TAG, "mConnectListener onFailure");
                }
            };
        }

        mSaveListener = new WigigManager.ActionListener() {
            @Override
            public void onSuccess() {
                Log.d(TAG, "mSaveListener onSuccess");
            }
            @Override
            public void onFailure(int reason) {
                Toast.makeText(getApplicationContext(), R.string.Save_failure, Toast.LENGTH_SHORT).show();
            }
        };

        mForgetListener = new WigigManager.ActionListener() {
            @Override
            public void onSuccess() {
                Log.d(TAG, "mForgetListener onSuccess");

            }
            @Override
            public void onFailure(int reason) {
                Toast.makeText(WigigSettingsActivity.this, R.string.Forget_failure, Toast.LENGTH_SHORT).show();
                Log.e(TAG, "mForgetListener onFailure");
            }
        };

    }

    @SuppressWarnings("deprecation")
    @Override
    protected void onResume() {
        Log.d(TAG, "Enter onResume");
        super.onResume();
        if (mWigigManager != null) {
            ListPreference modePreference = (ListPreference) findPreference("wigig_mode");
            modePreference.setValue(getResources().getString(R.string.network_mode));
            modePreference.setSummary("%s");
            writeLastMode(getResources().getString(R.string.network_mode));

            if (isWigigEnabled()) {
                syncTimerRunnable(true, SCAN_IMMEDIATE);
            }
        }
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "Enter onPause");
        super.onPause();
        if (mWigigManager != null) {
            syncTimerRunnable(false, SCAN_IMMEDIATE);
            removeAllAccessPoints();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.options_menu_network, menu);
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
            if (o instanceof AccessPoint) {
                AccessPoint a = (AccessPoint)o;
                super.onCreateContextMenu(menu, v, info);
                MenuInflater inflater = getMenuInflater();
                menu.setHeaderTitle(a.ssid);

                int apStatus = a.GetApStatus();

                switch (apStatus) {
                    case AccessPoint.AP_NOT_CONNECTED:
                        inflater.inflate(R.menu.context_menu_inactive_ap, menu);
                        break;
                    case AccessPoint.AP_SAVED_AWAY:
                        inflater.inflate(R.menu.context_menu_saved_away, menu);
                        break;
                    case AccessPoint.AP_SAVED:
                    case AccessPoint.AP_AUTHENTICATION_PROBLEM:
                        inflater.inflate(R.menu.context_menu_notconn_ap, menu);
                        break;
                    case AccessPoint.AP_CONNECTING:
                    case AccessPoint.AP_OBTAINING_IP:
                    case AccessPoint.AP_CONNECTED:
                    case AccessPoint.AP_AUTHENTICATING:
                        inflater.inflate(R.menu.context_menu_conn_ap, menu);
                        break;
                }
            }
        }
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        menu.findItem(R.id.menu_refresh).setEnabled(isWigigEnabled());
        menu.findItem(R.id.menu_add_network).setEnabled(isWigigEnabled());

        return super.onPrepareOptionsMenu(menu);
    }

    /*
     Called upon the option menu selection
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.menu_add_network:
                if (mWigigManager.isWigigEnabled()) {
                    mDialog.CreateDialog(null, true);
                }
                return true;
            case R.id.menu_refresh:
                if (mWigigManager.isWigigEnabled()) {
                    mWigigManager.startScan();
                }
                return true;
            case R.id.menu_advanced:
                Intent i = new Intent(WigigSettingsActivity.this, AdvancedWifiSettings.class);
                startActivity(i);
        }
        return (super.onOptionsItemSelected(item));
    }

    /*
    Called upon context menu selection
    */
    @Override
    public boolean onContextItemSelected(MenuItem item)
    {
        Log.d(TAG, "OnContextItemSelected()");
        AdapterView.AdapterContextMenuInfo info = (AdapterView.AdapterContextMenuInfo) item.getMenuInfo();
        Object o = getListView().getItemAtPosition(info.position);
        if (o != null) {
            switch (item.getItemId()) {
                case R.id.menu_context_connect:
                    if (o instanceof AccessPoint) {
                        AccessPoint a = (AccessPoint)o;
                        WifiConfiguration wc = getWifiConfigurationByAp(mWigigManager, a);
                        if (wc != null) {
                            connect(wc.networkId);
                        }
                        else {
                            if (a.security == AccessPoint.SECURITY_PSK) {
                                mDialog.CreateDialog(a, true);
                            } else {
                                wc = getConfig(a);
                                if (wc != null) {
                                    connect(wc);
                                }
                            }
                        }
                    }
                    break;
                case R.id.menu_context_disconnect_network:
                    if (o instanceof AccessPoint) {
                        AccessPoint a = (AccessPoint)o;
                        mWigigManager.disconnect();
                        a.setSummary(R.string.wifi_saved);
                        Collections.sort(accessPoints);
                    }
                    break;
                case R.id.menu_context_modify_network:
                    if (o instanceof AccessPoint) {
                        AccessPoint a = (AccessPoint)o;
                        mDialog.CreateDialog(a, true);
                    }
                    break;
                case R.id.menu_context_forget_network:
                    if (o instanceof AccessPoint) {
                        AccessPoint a = (AccessPoint)o;
                        WifiConfiguration wc = getWifiConfigurationByAp(mWigigManager, a);
                        if (wc != null) {
                            forget(wc.networkId);
                        }
                    }
                    break;
                default:
                    Log.d(TAG, "Invalid context menu item" + item.getTitle());
            }
        } else {
            Log.d(TAG, "Context menu object is  null");
        }

        return true;
    }
}
