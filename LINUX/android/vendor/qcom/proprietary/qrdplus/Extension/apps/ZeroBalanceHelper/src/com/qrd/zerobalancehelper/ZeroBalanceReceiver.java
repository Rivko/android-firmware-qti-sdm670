/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qrd.zerobalancehelper;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.TrafficStats;
import android.net.wifi.WifiManager;
import android.net.ZeroBalanceHelper;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.INetworkManagementService;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.provider.Settings;
import android.telephony.PreciseCallState;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

/**
 * Watches for Pre-paid SIM Zerobalance and restricts background data.
 */
public class ZeroBalanceReceiver extends BroadcastReceiver {

    private static final String TAG = "ZeroBalance";

    private TelephonyManager mTelephonyMgr;
    private AlarmManager mAlarmMgr;
    private ConnectivityManager connMgr;
    private boolean dataActivityReceived = false;
    private long mTimeout = 0;
    private long mSinglePingTimeout = 0;
    private CheckForZeroBalanceTask mCheckForZeroBalanceTask = null;
    private Context mContext;

    // Flags for checking zero balance and unblock device timer
    private final int CHECK_ZERO_BALANCE = 0;
    private final int UNBLOCK_BG = 1;

    // Flags for different network state
    private final int MOBILE_DATA = 0;
    private final int WIFI_DATA = 1;
    private final int DISCONNECTED = 2;
    private final int BADSTATE = -1;

    // firewall arguments
    private static final int FIREWALL_CHAIN_NONE = 0;
    private static final int FIREWALL_RULE_ALLOW = 1;
    private static final int FIREWALL_RULE_DENY = 2;

    // Flags for ZeroBalanceTask to identify that it should perform
    // single ping to check device is recharged or multiple ping to
    // check zero balance is reached
    private final int SINGLE_PING_UNBLOCK = 1;
    private final int MULTI_PING_BLOCK = 3;

    private final String ALARM_EXTRA_TAG = "check_zero_balance";

    //TX RX packets to check dormancy
    private static long mobileTxPackets = 0L;
    private static long mobileRxPackets = 0L;

    private static int mLastCallState = -1;

    private static final Handler mHandler = new Handler();
    //Intent value for 20 minute timer to unblock data and check zero balance
    public static final String SET_ZB_RETRY_UNBLOCK_TIMER = "operator.retry.unblock.timer";
    //Intent value for 5 minute timer to check if SIM is recharged under blocked condition
    public static final String SET_ZB_SINGLE_PING_UNBLOCK_TIMER =
            "operator.single.ping.unblock.timer";

    public static final String CALL_STATE_CHANGE = "android.intent.action.PRECISE_CALL_STATE" ;
    private static final String DNS_BLOCK_INTENT_ACTION = "org.codeaurora.restrictData" ;
    private static final String BACKGROUND_EXCEPTION_APP = "sys.background.exception.app";

    private int TIMER_FLAG = MULTI_PING_BLOCK;
    private int mNumPingTryToblock = 0; // Number of pings to find SIM reached zerobalance and block
    private int mNumPingTryToUnblock = 0;// Number of pings to find SIM is recharged and unblock
    //Interval after which network should be checked for active state
    private int mNumTxRxBlockDelay = 0;
    private int mNumTxRxUnblockDelay = 0;

    private static List<Integer> exceptionUidList = new ArrayList<Integer>();

    private enum ZeroBalanceIntentActions {
        BACKGROUND_DATA_BROADCAST(ZeroBalanceHelper.BACKGROUND_DATA_BROADCAST),
        CONNECTIVITY_CHANGE(ConnectivityManager.CONNECTIVITY_ACTION),
        WIFI_STATE_CHANGE(WifiManager.NETWORK_STATE_CHANGED_ACTION),
        SET_ZB_RETRY_UNBLOCK_TIMER("operator.retry.unblock.timer"),
        SET_ZB_SINGLE_PING_UNBLOCK_TIMER(
                "operator.single.ping.unblock.timer"),
        BOOT_COMPLETE(Intent.ACTION_BOOT_COMPLETED),
        CALL_STATE_CHANGE("android.intent.action.PRECISE_CALL_STATE");

        private String text;

        ZeroBalanceIntentActions(String text) {
            this.text = text;
        }

        private String getText() {
            return this.text;
        }

        private static ZeroBalanceIntentActions fromString(String text) {
            if (text != null) {
                for (ZeroBalanceIntentActions b : ZeroBalanceIntentActions
                        .values()) {
                    if (text.equalsIgnoreCase(b.text)) {
                        return b;
                    }
                }
            }
            return null;
        }
    }

    @Override
    public void onReceive(final Context context, Intent intent) {
        Log.d(TAG,
                "Broadcast received wth action = " + intent.getAction());
        mContext = context;
        if (!context.getResources().getBoolean(
                com.android.internal.R.bool.config_zero_balance_operator)) {
            return;
        }
        String action = intent.getAction();
        ZeroBalanceIntentActions intentAction = ZeroBalanceIntentActions
                .fromString(action);
        if (intentAction != null) {
            init();
            switch (intentAction) {
                /*
                 * Background data broadcast intent from telephony: Device
                 * needs to be unblocked for data access
                 * Background data broadcast from http stack : Device needs
                 * to be blocked from data access.
                 */
                case BACKGROUND_DATA_BROADCAST: {
                    String enabled = intent.getStringExtra("enabled");
                    Log.d(TAG,
                            "Background data broadcast intent received with block enabled = "
                            + enabled);
                    restrictBackground(Boolean.valueOf(enabled));
                    if (!ZeroBalanceService.isServiceRunning) {
                        context.startServiceAsUser(getServiceIntent(context),
                                UserHandle.CURRENT);
                    }
                    break;
                }
                /*
                 * Whenever there is connectivity change based on type of
                 * network to which device connected device should be blocked
                 * or unblocked from data access.
                 */
                case CONNECTIVITY_CHANGE: {
                    if (!ZeroBalanceService.isServiceRunning) {
                        context.startServiceAsUser(getServiceIntent(context),
                                UserHandle.CURRENT);
                    }
                    int state = getActiveNetworkInfo();
                    if (state == WIFI_DATA) { // Data should be unblocked as connected to wifi
                        Log.d(TAG, "WIFI is connected. Unblocking data and " +
                                "all RX TX threads are removed");
                        mHandler.removeCallbacksAndMessages(null);
                        if (mCheckForZeroBalanceTask != null
                                && mCheckForZeroBalanceTask.getStatus() ==
                                AsyncTask.Status.RUNNING) {
                            Log.d(TAG, "Cancelling zeroBalance task");
                            mCheckForZeroBalanceTask.cancel(true);
                        }
                        restrictBackground(false);
                    } else if (state == MOBILE_DATA) { // Device should be checked for zero balance
                        startCheckZeroBalance();
                    } else if (state == BADSTATE) {
                        NetworkInfo networkInfo = intent
                                .getParcelableExtra(ConnectivityManager.EXTRA_NETWORK_INFO);
                        if (networkInfo != null
                                && networkInfo.getState() == NetworkInfo.State.DISCONNECTED
                                && networkInfo.getType() == ConnectivityManager.TYPE_MOBILE) {
                            boolean isDataDisconnected = Settings.Global
                                    .getInt(mContext.getContentResolver(),
                                    Settings.Global.MOBILE_DATA, -1) == 0;
                            boolean isAirPlaneModeOn = Settings.Global.getInt(
                                    mContext.getContentResolver(),
                                    Settings.Global.AIRPLANE_MODE_ON, -1) == 1;
                            if (isDataDisconnected || isAirPlaneModeOn) {
                                Log.d(TAG,
                                        "Mobile Data is Disconnected or AirPlaneMode is On"
                                        +" from settings, so enable background data"
                                        +" and all RX TX threads are removed");
                                mHandler.removeCallbacksAndMessages(null);
                                restrictBackground(false);
                            }
                        }
                    }
                    break;
                }
                case WIFI_STATE_CHANGE: {
                    if (getActiveNetworkInfo() == WIFI_DATA) {
                        Log.d(TAG, "WIFI state changed, unblocking data and " +
                                "all RX TX threads are removed");
                        mHandler.removeCallbacksAndMessages(null);
                        if (mCheckForZeroBalanceTask != null
                                && mCheckForZeroBalanceTask.getStatus() ==
                                AsyncTask.Status.RUNNING) {
                            Log.d(TAG, "Cancelling zeroBalance task");
                            mCheckForZeroBalanceTask.cancel(true);
                        }
                        restrictBackground(false);
                    }
                    break;
                }
                /*
                 * When device is blocked for 20 mins every 5 min SIM recharge scenario
                 * is assessed. Network is pinged once and if successfully pings network
                 * data is unblocked.
                 */
                case SET_ZB_SINGLE_PING_UNBLOCK_TIMER: {
                    Log.d(TAG, "SET_ZB_SINGLE_PROBE_UNBLOCK_TIMER");
                    if (Boolean.valueOf(SystemProperties
                            .get(ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                            "false"))) {
                        if (getActiveNetworkInfo() == MOBILE_DATA
                                && checkDataConnectivity()) {
                            Log.d(TAG,
                                    "Checking for single ping to unblock, Start ZeroBalanceTask");
                            TIMER_FLAG = SINGLE_PING_UNBLOCK;
                            // Connectivity is present so device should attempt single ping.
                            runZeroBalanceTask(mNumPingTryToUnblock);
                        } else {
                            Log.d(TAG,
                                    "Checking for single ping to unblock"
                                    +
                                    "Mobile Data is Connected and in dormant."
                                    +
                                    " So all previous RX TX threads are removed"
                                    +
                                    " and registering new TX RX Listener");
                            // Phone is in dormant condition so network condition is
                            // rechecked after 1 minute interval.
                            mHandler.removeCallbacksAndMessages(null);
                            mobileTxPackets = TrafficStats
                                    .getMobileTxPackets();
                            mobileRxPackets = TrafficStats
                                    .getMobileRxPackets();
                            mHandler.postDelayed(mSingleProbeDataCheckRunnable,
                                    mNumTxRxUnblockDelay);
                        }
                    }
                    break;
                }
                /*
                 * Once device is blocked should be unblocked after 20 min.
                 * Once device is unblocked every 20 minutes 3 pings to the network
                 * is done to check if SIM reached zero balance scenario.
                 */
                case SET_ZB_RETRY_UNBLOCK_TIMER: {
                    Log.d(TAG, "SET_ZB_RETRY_UNBLOCK_TIMER");
                    int timerState = intent.getIntExtra(ALARM_EXTRA_TAG, -1);
                    //Timer to check if SIM reached no balance
                    if (timerState == CHECK_ZERO_BALANCE) {
                        if (getActiveNetworkInfo() == MOBILE_DATA) {
                            startCheckZeroBalance();
                        }
                    } else if (timerState == UNBLOCK_BG) { // Timer to unblock
                        Log.d(TAG, "Unblock data as timer expires." +
                                " All previous RX TX threads are removed");
                        mHandler.removeCallbacksAndMessages(null);
                        restrictBackground(false);
                    }
                    break;
                }
                /*
                 * To detect successful call connection and enable background data
                 */
                case CALL_STATE_CHANGE: {
                    if (Boolean.valueOf(SystemProperties
                            .get(ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                            "false"))) {
                        int state = intent.getIntExtra(
                                TelephonyManager.EXTRA_FOREGROUND_CALL_STATE,
                                -2);
                        Log.d(TAG, "Inside onreceive of call present state : "
                                + state + "last call state : " + mLastCallState);
                        if (mLastCallState == PreciseCallState.PRECISE_CALL_STATE_DIALING
                                && state == PreciseCallState.PRECISE_CALL_STATE_ALERTING) {
                            Log.d(TAG,
                                    "Call state changes and call got connected: "
                                    +
                                    "SIM is recharged. Enable background data ");
                            restrictBackground(false);
                            return;
                        }
                        mLastCallState = state;
                    }
                    break;
                }
                /*
                 * On boot complete service is started which registers all event and
                 * whitelist apps to be allowed to access network once device is blocked.
                 */
                case BOOT_COMPLETE: {
                    Log.d(TAG,
                            "Got ACTION_BOOT_COMPLETED1, start the service");
                    context.startServiceAsUser(getServiceIntent(context),
                            UserHandle.CURRENT);
                    break;
                }
            }
        }
    }

    /**
     * Initialization of all service and variables from
     * strings and config xml files.
     */
    void init() {
        if (mTelephonyMgr == null) {
            mTelephonyMgr = (TelephonyManager) mContext.
                    getSystemService(Context.TELEPHONY_SERVICE);
        }
        if (mAlarmMgr == null) {
            mAlarmMgr = (AlarmManager) mContext.
                    getSystemService(Context.ALARM_SERVICE);
        }
        mTimeout = (long) mContext.getResources().getInteger(
                R.integer.operator_config_timeout);
        mSinglePingTimeout = (long) mContext.getResources().getInteger(
                R.integer.operator_config_single_ping_timeout);
        if (connMgr == null) {
            connMgr = (ConnectivityManager)
                    mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        }
        mNumPingTryToblock = mContext.getResources().getInteger(
                R.integer.num_ping_try_to_block);
        mNumPingTryToUnblock = mContext.getResources().getInteger(
                R.integer.num_ping_try_to_unblock);
        mNumTxRxBlockDelay = mContext.getResources().getInteger(
                R.integer.tx_rx_block_delay);
        mNumTxRxUnblockDelay = mContext.getResources().getInteger(
                R.integer.tx_rx_unblock_delay);
    }

    /**
     * Either blocks entire network through firewall and DNS proxy except specific
     * apps which are allowed to access network or unblock network for data access.
     *
     * @param restrict : true : for blocking network access false for unblocking
     */
    private synchronized void restrictBackground(boolean restrict) {
        Log.d(TAG,
                "In restrictBackground with restrict = "
                        + restrict
                        + "system property"
                        + SystemProperties
                        .get(ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                        "false"));
        if (Boolean.valueOf(SystemProperties.get(
                ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                "false")) == restrict
                || (restrict && getActiveNetworkInfo() == WIFI_DATA)) {
            Log.d(TAG,
                    "Background property didnt change else wifi is connected");
            return;
        }
        // Cancel all timers as device data access state will change.
        cancelZeroBalanceTimer();
        cancelSinglePingTimer();
        SystemProperties.set(ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                String.valueOf(restrict));
        // Intent is sent to framework to block DNS.
        Intent blockIntent = new Intent();
        blockIntent
                .setAction(DNS_BLOCK_INTENT_ACTION);
        blockIntent.putExtra("Restrict", restrict);
        mContext.sendBroadcastAsUser(blockIntent, new UserHandle(
                UserHandle.USER_CURRENT));
        INetworkManagementService mNetManager = INetworkManagementService.Stub.asInterface(
                ServiceManager.getService(mContext.NETWORKMANAGEMENT_SERVICE));

        //Read the whitelist app uid  list from system property.

        if (exceptionUidList == null
                ||  exceptionUidList.size() == 0) {
            String exceptionList = SystemProperties.get(
                    BACKGROUND_EXCEPTION_APP, "");
            if (exceptionList != null && exceptionList.length() != 0) {
                for (String token : exceptionList.split(",")) {
                    exceptionUidList.add(Integer.valueOf(token));
                    Log.d(TAG,
                            "Exception Uid addition to local list in receiver= "
                            + token);
                }
            }
        }
        if (restrict) {
            mLastCallState = -1;
            // All data access is blocked through firewall and specific apps
            // are allowed to access network through firewall allow rule.
            try {
                if(!mNetManager.isFirewallEnabled()) {
                    Log.d(TAG, "Firewall disabled, so enable");
                    mNetManager.setFirewallEnabled(true);
                }
                if (exceptionUidList != null && exceptionUidList.size() != 0) {
                    for (Integer uid : exceptionUidList) {
                        mNetManager.setFirewallUidRule(
                                FIREWALL_CHAIN_NONE, uid, FIREWALL_RULE_ALLOW);
                        Log.d(TAG, "added to exception firewall" + uid);
                    }
                }
            } catch (RemoteException e) {
                Log.w(TAG, "No Firewall control; leaving");
            }
            Log.d(TAG, "Background Data Disabled, Setting Timer for " +
                    mTimeout / 1000 + " secs");
            setZeroBalanceTimer(UNBLOCK_BG);
            Log.d(TAG, "Background Data Disabled, Single ping timer is set" +
                    mSinglePingTimeout / 1000 + " secs");
            setSinglePingTimer();
        } else {
            mLastCallState = -1;
            // All data access is unblocked through firewall by deleting all rule
            // written earlier. If device is still connected to mobile data
            // SIM is checked for zerobalance.
            try {
                if (exceptionUidList != null && exceptionUidList.size() != 0) {
                    for (Integer uid : exceptionUidList) {
                        mNetManager.setFirewallUidRule(FIREWALL_CHAIN_NONE,
                                uid, FIREWALL_RULE_DENY);
                        Log.d(TAG, "removed from exception firewall" + uid);
                    }
                }
                if(mNetManager.isFirewallEnabled()) {
                    Log.d(TAG, "Firewall enabled, so disable");
                    mNetManager.setFirewallEnabled(false);
                }
            } catch (RemoteException e) {
                Log.w(TAG, "No Firewall control; leaving");
            }
            Log.d(TAG, "Background Data Enabled, Removing Timer");
            if (getActiveNetworkInfo() == MOBILE_DATA) {
                startCheckZeroBalance();
            }
        }
    }

    private class CheckForZeroBalanceTask extends
            AsyncTask<Integer, Void, Boolean> {
        protected Boolean doInBackground(Integer... params) {
            Log.d(TAG, "Checking for Zero Balance");

            boolean zeroBalance = false;
            int redirectCounter;
            int retryCounter = 1;

            final int MAX_RETRY = 2;
            final int HTTP_REDIRECT = 302;

            final String ping_url = mContext.getResources().getString(
                    com.android.internal.R.string.operator_ping_url);
            final String redirectUrl = mContext.getResources().getString(
                    com.android.internal.R.string.operator_config_url);

            // num_try will decide whether device will ping for block or unblock
            int num_try = params[0];

            Log.d(TAG, "ping_url = " + ping_url
                    + " : redirectUrl = " + redirectUrl);

            for (redirectCounter = 0; redirectCounter < num_try; redirectCounter++) {
                if (isCancelled() || retryCounter > MAX_RETRY) {
                    return false;
                }
                Log.d(TAG, "checking for Zero Balance through ping no "
                        + redirectCounter);
                if (num_try == mNumPingTryToblock && Boolean.valueOf(SystemProperties.get(
                        ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                        "false"))) {
                    Log.d(TAG, "Already reached Zero Balance , hence break");
                    zeroBalance = true;
                    break;
                }
                try {
                    HttpURLConnection conn = (HttpURLConnection)
                    new URL(ping_url).openConnection();
                    String userAgent = mContext.getResources().
                            getString(R.string.user_agent_string);
                    conn.setRequestProperty("User-Agent", userAgent);
                    conn.setInstanceFollowRedirects(false);
                    conn.connect();
                    int responseCode = conn.getResponseCode();
                    String location = conn.getHeaderField("Location");
                    Log.d(TAG, "Retrieved response = " + responseCode
                         + " : location = " + location);
                    if (responseCode == HTTP_REDIRECT &&
                            location.contains(redirectUrl)) {
                        Log.d(TAG, "Operator Redirect URL detected");
                    } else {
                        Log.d(TAG, "Operator Redirect URL NOT detected");
                        break;
                    }

                   conn.disconnect();

                } catch (MalformedURLException ex) {
                    Log.e(TAG, "Unexpected MalformedURLException" + ex);
                    break;
                } catch (IOException ex) {
                    Log.e(TAG, "Unexpected IOException" + ex);
                    redirectCounter = 0;
                    retryCounter++;
                }
            }

            if (redirectCounter == num_try) {
                zeroBalance = true;
            }
            return zeroBalance;
        }

        @Override
        protected void onPostExecute(Boolean zeroBalance) {
            if (isCancelled())
                return;
            Log.d(TAG, "Pings Completed and hasZeroBalance = "
                    + zeroBalance);
            if (TIMER_FLAG == MULTI_PING_BLOCK) {
                Log.d(TAG, "Pings Completed and numTryPing = " + TIMER_FLAG);
                // Network is pinged 3 times and zero balance is detected
                if (zeroBalance) {
                    Log.d(TAG,
                            "Zero Balance Detected due to 3 redirect urls or Already restricted");
                    restrictBackground(true);
                } else {
                    // Network is pinged 3 times and zero balance is not detected.
                    // Device should be checked again after 20 minutes.
                    // timer is set.
                    Log.d(TAG,
                        "ZeroBalance not reached or zero balance check failed, Setting Timer for "
                        + mTimeout / 1000 + " secs");
                    setZeroBalanceTimer(CHECK_ZERO_BALANCE);
                }
            } else if (TIMER_FLAG == SINGLE_PING_UNBLOCK
                    && Boolean.valueOf(SystemProperties.get(
                    ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY, "false"))) {
                // Under blocked state network is pinged once to check SIM is recharged.
                Log.d(TAG,
                        " All previous RX TX threads are removed inside postexecute");
                mHandler.removeCallbacksAndMessages(null);
                if (zeroBalance) {
                    Log.d(TAG,
                            "Phone is not recharged.Setting timer for next" +
                            mSinglePingTimeout / 1000 + " secs");
                    setSinglePingTimer();
                } else {
                    Log.d(TAG, "Phone is recharged.Unblock everything");
                    restrictBackground(false);
                }
            }
        }
    }
    /**
     * Cancel zero balance check and unblock timer
     */
    private void cancelZeroBalanceTimer() {
        Intent i = new Intent(mContext, ZeroBalanceReceiver.class);
        i.setAction(SET_ZB_RETRY_UNBLOCK_TIMER);
        mAlarmMgr.cancel(PendingIntent.getBroadcast(mContext, 0, i, 0));
        Log.d(TAG, "ZeroBalance timer is cancelled");
    }

    /**
     * Sets zero balance check and unblock timer
     * @param timerFlag : to check zero balance or unblock data
     */
    private void setZeroBalanceTimer(int timerFlag) {
        // Cancel the previous alarm
        cancelZeroBalanceTimer();
        cancelSinglePingTimer();
        if (timerFlag == UNBLOCK_BG || timerFlag == CHECK_ZERO_BALANCE) {
            Intent i = new Intent(mContext, ZeroBalanceReceiver.class);
            i.setAction(SET_ZB_RETRY_UNBLOCK_TIMER);
            i.putExtra(ALARM_EXTRA_TAG, timerFlag);
            PendingIntent pi = PendingIntent.getBroadcast(mContext, 0, i,
                    PendingIntent.FLAG_CANCEL_CURRENT);
            mAlarmMgr.setExact(AlarmManager.RTC_WAKEUP,
                    System.currentTimeMillis() + mTimeout, pi);
            Log.d(TAG, "ZeroBalance timer is set for next"+ mTimeout / 1000 + " secs"
                    + " flag is"+timerFlag );
        }
    }

    /**
     * Cancel  SIM recharge check   timer
     */
    private void cancelSinglePingTimer() {
        Intent i = new Intent(mContext, ZeroBalanceReceiver.class);
        i.setAction(SET_ZB_SINGLE_PING_UNBLOCK_TIMER);
        mAlarmMgr.cancel(PendingIntent.getBroadcast(mContext, 0, i, 0));
        Log.d(TAG, "SinglePing timer to unblock data is cancelled");
    }

    /**
     * Sets SIM recharge check timer.
     */
    private void setSinglePingTimer() {
        // Cancel the previous alarm
        cancelSinglePingTimer();
        Intent i = new Intent(mContext, ZeroBalanceReceiver.class);
        i.setAction(SET_ZB_SINGLE_PING_UNBLOCK_TIMER);
        PendingIntent pi = PendingIntent.getBroadcast(mContext, 0, i,
                PendingIntent.FLAG_CANCEL_CURRENT);
        mAlarmMgr.setExact(AlarmManager.RTC_WAKEUP,
                System.currentTimeMillis() + mSinglePingTimeout, pi);
        Log.d(TAG, "Single ping timer to unblock data is set for next" +
                mSinglePingTimeout / 1000+" secs");
    }

    private boolean checkDataConnectivity() {
        Log.d(TAG, "checkDataConnectivity");
        int direction = mTelephonyMgr.getDataActivity();
        Log.d(TAG, "checkDataConnectivity direction = " + direction);
        if ((mTelephonyMgr.getDataState() == TelephonyManager.DATA_CONNECTED) &&
                (direction == TelephonyManager.DATA_ACTIVITY_IN ||
                direction == TelephonyManager.DATA_ACTIVITY_OUT ||
                direction == TelephonyManager.DATA_ACTIVITY_INOUT)) {
            Log.d(TAG, "checkDataConnectivity return true");
            return true;
        }
        Log.d(TAG, "Phone is in either DORMANT or NONE = " + direction);
        return false;
    }

    private Intent getServiceIntent(Context context) {
        Intent serviceIntent = new Intent();
        serviceIntent.setClass(context, ZeroBalanceService.class);
        return serviceIntent;
    }

    private boolean isDataActive() {
        Log.i(TAG,
                "TrafficStats.getMobileTxPackets()"
                + TrafficStats.getMobileTxPackets()
                + " mobileTxPackets" + mobileTxPackets
                + " mobileRxPackets" + mobileRxPackets);
        if (((TrafficStats.getMobileTxPackets() - mobileTxPackets) > 0) ||
                ((TrafficStats.getMobileRxPackets() - mobileRxPackets) > 0)) {
            Log.d(TAG, "Data Actitivity is there");
            return true;
        } else {
            mobileTxPackets = TrafficStats.getMobileTxPackets();
            mobileRxPackets = TrafficStats.getMobileRxPackets();
            Log.d(TAG, "Data Actitivity is not there");
        }
        return false;
    }

    private int getActiveNetworkInfo() {
        ConnectivityManager connMgr = (ConnectivityManager)
                mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo nwInfo = connMgr.getActiveNetworkInfo();
        if (nwInfo != null && nwInfo.isConnected()) {
            if (nwInfo.getType() == ConnectivityManager.TYPE_MOBILE) {
                Log.d(TAG, "Connectivity type is Mobile data");
                return MOBILE_DATA;
            } else if (nwInfo.getType() == ConnectivityManager.TYPE_WIFI) {
                Log.d(TAG, "Connectivity type is Wifi data");
                return WIFI_DATA;
            }
        }
        Log.d(TAG, "Connectivity type is neither mobile nor wifi");
        return BADSTATE;
    }

    private void startCheckZeroBalance() {
        Log.d(TAG, "Mobile Data is Connected");
        if (!Boolean.valueOf(SystemProperties.get(
                ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY,
                "false"))
                && ((mCheckForZeroBalanceTask == null) || mCheckForZeroBalanceTask
                .getStatus() != AsyncTask.Status.RUNNING)) {
            // Checks if device is in unblocked state and no task is running
            Log.d(TAG, "Inside startCheckZeroBalance. systemBGDataProperty: "+
                    Boolean.valueOf(SystemProperties.get(
                    ZeroBalanceHelper.BACKGROUND_DATA_PROPERTY, "false")));
            Log.d(TAG, "Inside startCheckZeroBalance. All RX TX threads are removed");
            mHandler.removeCallbacksAndMessages(null);
            if (checkDataConnectivity()) {
                // If device is connected state check for zero balance
                TIMER_FLAG = MULTI_PING_BLOCK;
                runZeroBalanceTask(mNumPingTryToblock);
            } else {
                Log.d(TAG,
                        "Mobile Data is Connected and in dormant state,"
                           +"So registering TX RX Listener");
                setZeroBalanceTimer(CHECK_ZERO_BALANCE);
                mobileTxPackets = TrafficStats
                        .getMobileTxPackets();
                mobileRxPackets = TrafficStats
                        .getMobileRxPackets();
                mHandler.postDelayed(mDataCheckRunnable, mNumTxRxBlockDelay);
            }
        }
    }

    /**
     * Runnable to assess network dormancy and start checking zero balance scenario
     */
    private final Runnable mDataCheckRunnable = new Runnable() {
        public void run() {
            if (isDataActive()) {
                Log.d(TAG, "Data is active, so check zero balance");
                TIMER_FLAG = MULTI_PING_BLOCK;
                runZeroBalanceTask(mNumPingTryToblock);
            } else {
                Log.d(TAG, "All RX TX threads are removed and set again for "+
                        mNumTxRxBlockDelay/1000+ " seconds");
                mHandler.removeCallbacksAndMessages(null);
                mHandler.postDelayed(mDataCheckRunnable, mNumTxRxBlockDelay);
            }
        }
    };

    /**
     * Runnable to assess network dormancy and start checking SIM recharge scenario
     */
    private final Runnable mSingleProbeDataCheckRunnable = new Runnable() {
        public void run() {
            if (isDataActive()) {
                Log.d(TAG, "Data is active, so check for recharge");
                TIMER_FLAG = SINGLE_PING_UNBLOCK;
                runZeroBalanceTask(mNumPingTryToUnblock);
            } else {
                Log.d(TAG,"All RX TX threads are removed and set again for " +
                        mNumTxRxUnblockDelay/1000+" seconds");
                mHandler.removeCallbacksAndMessages(null);
                mHandler.postDelayed(mSingleProbeDataCheckRunnable, mNumTxRxUnblockDelay);
            }
        }
    };

    private void runZeroBalanceTask(int retryCount) {

        if (mCheckForZeroBalanceTask == null
                || mCheckForZeroBalanceTask.getStatus() ==
                AsyncTask.Status.FINISHED) {
            Log.d(TAG, "runZeroBalanceTask: new task started running");
            mCheckForZeroBalanceTask = new CheckForZeroBalanceTask();
            mCheckForZeroBalanceTask.execute(retryCount);
        } else {
            Log.d(TAG, "runZeroBalanceTask: already running");
        }
    }
}
