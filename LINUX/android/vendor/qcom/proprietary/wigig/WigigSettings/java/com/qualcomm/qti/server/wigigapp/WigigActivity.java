/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.server.wigigapp;


import android.app.ActionBar;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkInfo;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.SwitchPreference;
import android.provider.Settings;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.wigig.WigigManager;
import com.qualcomm.qti.wigig.p2p.WigigP2pManager;


public class WigigActivity extends PreferenceActivity {
    private static final String TAG = "WigigActivity";
    protected static final int NO_RESOURCES_TO_LOAD = -1;
    protected WigigManager mWigigManager;
    protected WigigP2pManager mP2PManager;
    private int mPrefResourceId;
    private final IntentFilter mFilter;
    protected final BroadcastReceiver mReceiver;
    private SwitchPreference mSwitchPreference;
    private ListPreference mModePreference;
    private static String mPersistentMode = "";
    protected String mLastMode;
    private boolean mIsConnected;
    private boolean mIsInitialStateObtaining;
    protected String modeSwitchDelayed;
    private AlertDialog modeSwitchAlert;
    private boolean mInsideSwitchingWithAlert;
    protected final static String ENABLE_WIGIG_STR = "EnableWigig";
    protected final static int MODE_NOT_CHANGED = 0;
    protected final static int MODE_TO_CHANGE_HOTSPOT = 1;
    protected final static int MODE_TO_CHANGE_RU = 2;
    protected final static int MODE_ALREADY_ENABLED = 4;

    protected static final int INITIAL_WIGIG_ENABLED_STATE = 0;
    protected static final int INITIAL_NETWORK_CONNECTED_STATE = 1;
    protected static final int INITIAL_P2P_CONNECTED_STATE = 2;
    protected static final int INITIAL_STATES_COUNT = 3;
    private boolean[] mInitialStates = new boolean[INITIAL_STATES_COUNT];
    private int     mInitialStatesObtained;

    public WigigActivity(int prefid, String[] actions) {
        mLastMode = "";
        mPrefResourceId = prefid;
        mFilter = new IntentFilter();
        if (actions != null && actions.length > 0) {
            for (String a : actions) {
                if (!(a.equals(WigigManager.WIGIG_STATE_CHANGED_ACTION) ||
                        a.equals(WigigManager.NETWORK_STATE_CHANGED_ACTION) ||
                        a.equals(WigigP2pManager.WIGIG_P2P_CONNECTION_CHANGED_ACTION))) {
                    mFilter.addAction(a);
                }
            }
        }
        mFilter.addAction(WigigManager.WIGIG_STATE_CHANGED_ACTION);
        mFilter.addAction(WigigManager.NETWORK_STATE_CHANGED_ACTION);
        mFilter.addAction(WigigP2pManager.WIGIG_P2P_CONNECTION_CHANGED_ACTION);

        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                handleEvent(intent);
            }
        };
    }

    protected void handleEvent(Intent intent) {
        final String action = intent.getAction();

        if (action.equals(WigigManager.WIGIG_STATE_CHANGED_ACTION)) {
            int newState = intent.getIntExtra(WigigManager.EXTRA_WIGIG_STATE,
                    WigigManager.WIGIG_STATE_UNKNOWN);
            int prevState = intent.getIntExtra(WigigManager.EXTRA_PREVIOUS_WIGIG_STATE,
                    WigigManager.WIGIG_STATE_UNKNOWN);
            Log.i(TAG, "WIGIG_STATE_CHANGED_ACTION: " + prevState + "=>" + newState);

            if (isInitialStateObtaining()) {
                mInitialStates[INITIAL_WIGIG_ENABLED_STATE] = (newState == WigigManager.WIGIG_STATE_ENABLED);
                mInitialStatesObtained++;
            }
            if (newState == WigigManager.WIGIG_STATE_ENABLED) {
                onWigigFrameworkEnabled();
            } else if (newState == WigigManager.WIGIG_STATE_RATE_UPGRADE) {
                onWigigRUFrameworkEnabled();
            } else if ((newState == WigigManager.WIGIG_STATE_DISABLED) ||
                       (newState == WigigManager.WIGIG_STATE_FAILED)) {
                onWigigFrameworkDisabled();
            }
        }
        else if(action.equals(WigigManager.NETWORK_STATE_CHANGED_ACTION)) {
            Log.i(TAG, "NETWORK_STATE_CHANGED_ACTION");
            NetworkInfo networkInfo = intent.getParcelableExtra(WigigManager.EXTRA_NETWORK_INFO);
            int networkId = intent.getIntExtra(WigigManager.EXTRA_NETWORK_ID, -1);
            NetworkInfo.DetailedState ds = networkInfo.getDetailedState();
            if (isInitialStateObtaining()) {
                mInitialStates[INITIAL_NETWORK_CONNECTED_STATE] = (ds == NetworkInfo.DetailedState.CONNECTED);
                mInitialStatesObtained++;
            }
            onNetworkStateChanged(ds, networkId);
        }
        else if(action.equals(WigigP2pManager.WIGIG_P2P_CONNECTION_CHANGED_ACTION)) {
            NetworkInfo networkInfo = intent.getParcelableExtra(WigigP2pManager.EXTRA_NETWORK_INFO);
            if (isInitialStateObtaining()) {
                mInitialStates[INITIAL_P2P_CONNECTED_STATE] = networkInfo.isConnected();
                mInitialStatesObtained++;
            }
            onP2PStateChanged(networkInfo);
        }
        if (mInitialStatesObtained == INITIAL_STATES_COUNT) {
            mInitialStatesObtained = 0;
            mIsInitialStateObtaining = false;
            onInitialStatesObtained(mInitialStates);
        }
    }

    protected void onWigigSwitchedOn() {
        Log.d(TAG, "onWigigSwitchedOn()");
        mModePreference.setEnabled(true);
    }

    protected void onWigigSwitchedOff() {
        Log.d(TAG, "onWigigSwitchedOff()");
        mModePreference.setEnabled(false);
        mIsConnected = false;
    }

    protected boolean onModeChange(String mode) {
        return true;
    }

    protected void onWigigFrameworkEnabled() {
        Log.d(TAG, "onWigigFrameworkEnabled()");
        setWigigSwitch(true);
    }

    protected void onWigigFrameworkDisabled() {
        Log.d(TAG, "onWigigFrameworkDisabled()");
        setWigigSwitch(false);
        mIsConnected = false;
    }

    protected void onWigigRUFrameworkEnabled() {
        Log.d(TAG, "onWigigRUFrameworkEnabled");
        setWigigSwitch(true);
    }

    protected void onInitialStatesObtained(boolean[] states) {
    }

    protected void onNetworkStateChanged(NetworkInfo.DetailedState ds, int networkId) {
    }

    protected void onP2PStateChanged(NetworkInfo netinfo) {
    }

    protected boolean isWigigEnabled() {
        return mWigigManager != null && mWigigManager.isWigigEnabled();
    }

    protected boolean isWigigApEnabled() {
        return mWigigManager != null && mWigigManager.isWigigApEnabled();
    }

    protected boolean isWigigRUEnabled() {
        return mWigigManager != null && mWigigManager.isRateUpgradeEnabled();
    }

    public boolean setWigigEnabled(boolean enabled) {
        if (enabled && isWigigApEnabled())
            setWigigApEnabled(null, false);
        else if (enabled && isWigigRUEnabled())
            setWigigRUEnabled(false);

        if (mWigigManager != null) {
            boolean ret = mWigigManager.setWigigEnabled(enabled);
            if (!ret) {
                Toast.makeText(this, "Could not" + (enabled ? "enable" : "disable") + "wigig!", Toast.LENGTH_SHORT).show();
                Log.e(TAG, "Could not" + (enabled ? "enable" : "disable") + "wigig!");
            }
            return ret;
        }
        return false;
    }

    public boolean setWigigApEnabled(WifiConfiguration config, boolean enabled) {
        if (enabled && isWigigEnabled())
            setWigigEnabled(false);
        else if (enabled && isWigigRUEnabled())
            setWigigRUEnabled(false);

        boolean ret = mWigigManager.setWigigApEnabled(config, enabled);
        if (!ret) {
            Toast.makeText(this, "Could not" + (enabled ? "enable" : "disable") + "wigig AP!", Toast.LENGTH_SHORT).show();
            Log.e(TAG, "Could not" + (enabled ? "enable" : "disable") + "wigig AP!");
        }
        return ret;
    }

    public boolean setWigigRUEnabled(boolean enabled) {
        if (enabled && isWigigEnabled())
            setWigigEnabled(false);
        else if(enabled && isWigigApEnabled())
            setWigigApEnabled(null, false);

        boolean ret = mWigigManager.setRateUpgradeEnabled(enabled);
        if (!ret) {
            Toast.makeText(this, "Could not " + (enabled ? "enable" : "disable") + "Rate Upgrade!", Toast.LENGTH_SHORT).show();
            Log.e(TAG, "Could not " + (enabled ? "enable" : "disable") + "Rate Upgrade!");
        }
        else {
            Log.d(TAG, "Rate Upgrade mode successfully " + (enabled? "enabled" : "disabled"));
        }
        return ret;
    }

    protected boolean isConnected() {
        return mIsConnected;
    }

    protected void setConnected(boolean state) {
        mIsConnected = state;
    }

    protected boolean isInitialStateObtaining() {
        return mIsInitialStateObtaining;
    }

    protected void switchWigigMode(Context context, Class cls, boolean enableWigig) {
        Log.d(TAG, "switchWigigMode(), enabled:" + enableWigig);
        writeLastMode("");
        Intent i = new Intent(context, cls);
        i.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        i.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
        Bundle b = new Bundle();
        b.putBoolean(ENABLE_WIGIG_STR, enableWigig);
        i.putExtras(b);
        startActivity(i);
        overridePendingTransition(0,0);
        mInsideSwitchingWithAlert = false;
        finish();
    }

    protected synchronized void switchWigigModeWithAlert(final Context context, final Class cls) {
        Log.d(TAG, "switchWigigModeWithAlert()");
        if (mInsideSwitchingWithAlert)
            return;
        mInsideSwitchingWithAlert = true;
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        modeSwitchAlert = null;
        builder.setTitle(R.string.title_mode_changed)
                .setMessage(R.string.message_mode_changed)
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setNeutralButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        try {
                            if (modeSwitchAlert != null && modeSwitchAlert.isShowing()) {
                                Log.d(TAG, "Dismissing mode switch alert");
                                modeSwitchAlert.dismiss();
                            }
                        } catch (Exception e) {
                            Log.e(TAG, "Error dismissing the mode change alert");
                        }
                        switchWigigMode(context, cls, false);
                    }
                });
        builder.setCancelable(false);
        modeSwitchAlert = builder.create();
        modeSwitchAlert.setCanceledOnTouchOutside(false);
        modeSwitchAlert.show();
    }

    protected void setWigigSwitch(boolean state) {
        Log.d(TAG, "setWigigSwitch() " + state);
        mSwitchPreference.setChecked(state);
        mModePreference.setEnabled(state);
    }

    private void quitApplicationWithAlert(int title, int message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(title)
                .setMessage(message)
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setCancelable(false)
                .setNeutralButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which)  {
                        // Exit the application.
                        finish();
                    }
                });
        AlertDialog alert = builder.create();
        alert.setCanceledOnTouchOutside(false);
        alert.show();
    }

    protected void writeLastMode(String mode) {
        if (mode.equals(getResources().getString(R.string.ru_mode)) ||
                mode.equals(getResources().getString(R.string.hotspot_mode)))
                mPersistentMode = "";
        else
            mPersistentMode = mode;
    }

    protected String readLastMode() {
        if (isWigigRUEnabled())
            return getResources().getString(R.string.ru_mode);
        else if (isWigigApEnabled())
            return getResources().getString(R.string.hotspot_mode);

        return mPersistentMode;
    }


    private Preference.OnPreferenceChangeListener sSwitchListener = new Preference.OnPreferenceChangeListener() {
        @Override
        public boolean onPreferenceChange(Preference preference, Object value) {
            String stringValue = value.toString();

            if (preference instanceof SwitchPreference) {
                Log.d(TAG, "Switch changed to " + stringValue);
                if (mWigigManager != null) {
                    if (stringValue.equals("true")) {
                        onWigigSwitchedOn();
                    } else {
                        onWigigSwitchedOff();
                    }
                }
            }
            return true;
        }
    };

    private Preference.OnPreferenceChangeListener sModeListener = new Preference.OnPreferenceChangeListener() {
        @Override
        public boolean onPreferenceChange(Preference preference, Object value) {
            String stringValue = value.toString();
            if (stringValue.equals(getString(R.string.hotspot_mode))) {
                if (isAirplaneMode()) {
                    Toast.makeText(WigigActivity.this, R.string.hotspot_no_switch_on_airplane,
                            Toast.LENGTH_LONG).show();
                    return false;
                }
            }

            Log.d(TAG, "Mode preference changed to " + stringValue);
            if (mWigigManager != null)
                onModeChange(stringValue);
            return false;
        }
    };

    @SuppressWarnings("deprecation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /*
         Enable the action bar. Ensure that the application style supports the Action bar,
         e.g. the AppTheme parent in styles.xml is @android:style/Theme.Holo.Light
        */
        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            // Disable the Up button in the action bar.
            actionBar.setDisplayHomeAsUpEnabled(false);
        }

        mWigigManager = WigigManager.getInstance(this);
        if (mWigigManager == null) {
            /*
              Wigig Manager not present (or not loaded). The application can not run. Display
              the error dialog and exit the application
            */
            Log.e(TAG, "Failed to get WigigManager instance");
            mP2PManager = null;
            quitApplicationWithAlert(R.string.title_wigig_notloaded, R.string.message_wigig_notloaded);
            return;
        }

        mP2PManager = WigigP2pManager.getInstance();
        if (mP2PManager == null) {
            Log.e(TAG, "Failed to get WigigP2PManager instance");
            quitApplicationWithAlert(R.string.title_wigig_notloaded, R.string.message_wigig_notloaded);
            return;
        }

        mLastMode = readLastMode();
        Log.d(TAG, "Persistent mode read: " + mLastMode);
        mIsConnected = false;   // Init state is disconnected, sticky broadcasts would set the real value

        if (mPrefResourceId != NO_RESOURCES_TO_LOAD) {
            boolean isEnabled = isWigigEnabled() || isWigigApEnabled();

            addPreferencesFromResource(mPrefResourceId);
            mSwitchPreference = (SwitchPreference) findPreference("enable_wigig_switch");
            mModePreference = (ListPreference) findPreference("wigig_mode");
            if (mSwitchPreference == null || mModePreference == null) {
                Log.e(TAG, "The resource XML is missing required preferences!");
                return;
            }
            setWigigSwitch(isEnabled);
            mSwitchPreference.setOnPreferenceChangeListener(sSwitchListener);
            mModePreference.setOnPreferenceChangeListener(sModeListener);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        mIsInitialStateObtaining = true;
        for(int i = 0; i < INITIAL_STATES_COUNT; i++) {
            mInitialStates[i] = false;
        }
        mInitialStatesObtained = 0;
        modeSwitchDelayed = "";
        if (mWigigManager != null && mFilter != null) {
            registerReceiver(mReceiver, mFilter);
        }
        if (mPrefResourceId != NO_RESOURCES_TO_LOAD) {
            registerForContextMenu(getListView());
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        mIsInitialStateObtaining = false;
        if (mPrefResourceId != NO_RESOURCES_TO_LOAD) {
            unregisterForContextMenu(getListView());
        }
        if (mWigigManager != null && mFilter != null) {
            unregisterReceiver(mReceiver);
        }
    }

    protected boolean isAirplaneMode() {
        return (Settings.Global.getInt(this.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) != 0);
    }
}
