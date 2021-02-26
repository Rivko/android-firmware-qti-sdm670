
/*
* Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution, Apache license notifications and license are retained
* for attribution purposes only.
*/

/*
* Copyright (C) 2011 The Android Open Source Project
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

package com.qualcomm.wfd.client;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TableLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdEnums.AVPlaybackMode;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;
import com.qualcomm.wfd.WfdStatus;
import com.qualcomm.wfd.client.DeviceListFragment.DeviceActionListener;
import com.qualcomm.wfd.client.ServiceUtil.ServiceFailedToBindException;
import com.qualcomm.wfd.client.WfdClientUtils.WfdOperation;
import com.qualcomm.wfd.client.WfdClientUtils.WfdOperationTask;
import com.qualcomm.wfd.client.net.CombinedNetManager;
import com.qualcomm.wfd.client.net.ConnectionInfoIF;
import com.qualcomm.wfd.client.net.LanDevice;
import com.qualcomm.wfd.client.net.NetDeviceIF;
import com.qualcomm.wfd.client.net.NetManagerIF;
import com.qualcomm.wfd.client.net.ResultListener;
import com.qualcomm.wfd.service.IWfdActionListener;
import com.qualcomm.wfd.service.IWfdSession;

import java.lang.ref.WeakReference;
import java.util.Collection;
import java.util.Map;
import java.util.Stack;

import static com.qualcomm.wfd.client.WfdClientUtils.*;

public class WFDClientActivity extends Activity implements DeviceActionListener, SharedPreferences.OnSharedPreferenceChangeListener {
    public static final String TAG = "C.WFDClientActivity";

    private TextView mynameTextView;
    private TextView mymacaddressTextView;
    private TextView mystatusTextView;
    private TextView mynameTextViewWigig;
    private TextView mymacaddressTextViewWigig;
    private TextView mystatusTextViewWigig;

    private TableLayout controlbuttonsTableLayout;
    private ToggleButton playpauseButton;
    private ToggleButton standbyresumeButton;
    private Button teardownButton;
    private ToggleButton enableDisableUibcButton;
    private ToggleButton selectTransportButton;
    private ToggleButton flushButton;
    private ToggleButton toggleDSButton;

    private Whisperer whisperer;
    private ClientEventHandler mEventHandler;

    private ProgressDialog startSessionProgressDialog = null;

    IWfdActionListener mActionListener;
    private final int START_SURFACE = 1;
    private final Stack<IWfdSession> mSessionStack = new Stack<IWfdSession>();
    private boolean mSurfaceStared;
    private int localExtSupport;
    private int peerExtSupprt;
    private WeakReference<StartSessionTask> mCurrentStartSessionTask = new WeakReference<StartSessionTask>(null);

    /**
     * @param isWifiP2pEnabled
     *            the isWifiP2pEnabled to set
     */
    public void setIsWifiP2pEnabled(boolean isWifiP2pEnabled) {
        whisperer.setWifiP2pEnabled(isWifiP2pEnabled);
        if (!whisperer.isWifiP2pEnabled()) {
            Log.d(TAG, "Find selected from action bar while p2p off");
            Toast.makeText(WFDClientActivity.this,
                    R.string.p2p_off_warning, Toast.LENGTH_SHORT)
                    .show();
        }
        if (whisperer.isMultiSinkMode()) {
            onOptionCreateGroup(null);
        } else {
            onOptionRemoveGroup(null);
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        whisperer = Whisperer.getInstance();
        whisperer.setActivity(this);

        setContentView(R.layout.main);

        mEventHandler = new ClientEventHandler();

        try {
            ServiceUtil.bindService(getApplicationContext(), mEventHandler);
        } catch (ServiceFailedToBindException e) {
            Log.e(TAG, "ServiceFailedToBindException received");
        }

        mynameTextView = (TextView) this.findViewById(R.id.tv_my_name);
        mymacaddressTextView = (TextView) this.findViewById(R.id.tv_my_mac_address);
        mystatusTextView = (TextView) this.findViewById(R.id.tv_my_status);
        mynameTextViewWigig = (TextView) this.findViewById(R.id.tv_my_name_wigig);
        mymacaddressTextViewWigig = (TextView) this.findViewById(R.id.tv_my_mac_address_wigig);
        mystatusTextViewWigig = (TextView) this.findViewById(R.id.tv_my_status_wigig);

        controlbuttonsTableLayout = (TableLayout) this.findViewById(R.id.tl_control_buttons);
        playpauseButton = (ToggleButton) this.findViewById(R.id.btn_play_pause);
        standbyresumeButton = (ToggleButton) this.findViewById(R.id.btn_standby_resume);
        teardownButton = (Button) this.findViewById(R.id.btn_teardown);
        enableDisableUibcButton = (ToggleButton) this.findViewById(R.id.btn_start_stop_uibc);
        selectTransportButton = (ToggleButton) this.findViewById(R.id.btn_transport_udp_tcp);
        flushButton = (ToggleButton) this.findViewById(R.id.btn_flush);
        toggleDSButton = (ToggleButton) this.findViewById(R.id.btn_enable_disable_ds);
        whisperer.setSharedPrefs(PreferenceManager.getDefaultSharedPreferences(this));
        PreferenceManager.setDefaultValues(this, R.xml.preferences, false);
        whisperer.getSharedPrefs().registerOnSharedPreferenceChangeListener(this);
        whisperer.setMultiSessionMode(whisperer.getSharedPrefs().getBoolean(this.getResources().getString(R.string.multi_session_mode),false));
        whisperer.setDecoderLatency(Integer.parseInt(whisperer.getSharedPrefs().getString("udp_decoder_latency_value", "5")));
        Whisperer.getInstance().registerHandler(mEventHandler, WFDClientActivity.class.getName());
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (whisperer.getSharedPrefs() != null)
        { whisperer.getSharedPrefs().unregisterOnSharedPreferenceChangeListener(this);
            whisperer.setSharedPrefs(null);
        }

        if(!mSessionStack.empty()) {
            IWfdSession currSesion = mSessionStack.pop();
            if(currSesion != null)
            {
                try {
                    currSesion.deinit();
                } catch (RemoteException e) {
                    Log.e(TAG, "RemoteException in deInit");
                }
            }
        }
        if (ServiceUtil.getmServiceAlreadyBound())
        {
            try {
                ServiceUtil.getInstance().deinit();
            } catch (RemoteException e) {
                Log.e(TAG, "RemoteException in deInit");
            }
        }
        try {
            unbindWfdService();
        } catch (IllegalArgumentException e) {
            Log.e(TAG,"Illegal Argument Exception ",e);
        }
        mSessionStack.clear();
        Whisperer.getInstance().unRegisterHandler(WFDClientActivity.class.getName());

        whisperer.clear();
        whisperer = null;
        Log.d(TAG, "onDestroy() called");
    }

    /** To prevent dynamic configuration changes from destroying activity */
    @Override
    public void onConfigurationChanged (Configuration newConfig) {
        Log.e(TAG, "onConfigurationChanged called due to" + newConfig.toString());
        super.onConfigurationChanged(newConfig);
    }

    /** register the BroadcastReceiver with the intent values to be matched */
    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume() called");
        getNetManager().onMainActivityResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        getNetManager().onMainActivityPause();
        Log.d(TAG, "onPause() called");
    }

    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,String key){

        Log.d(TAG, "onSharedPreferenceChanged Listener");

        if (whisperer.getSharedPrefs() == null) {
            Log.wtf(TAG, "Null objects!");
            return;
        }

        if (key.equals("udp_decoder_latency_value")){

            Log.d(TAG, "decoder latency preference changed");
            int decoderLatency = Integer.parseInt(whisperer.getSharedPrefs().getString("udp_decoder_latency_value", "5"));

            if (getCurrentSession() != null
                    && decoderLatency != whisperer.getDecoderLatency() && whisperer.isInSession()) {
                Log.d(TAG, "setting decoder latency");
                whisperer.setDecoderLatency(decoderLatency);
                startWfdOperationTask(this, WfdOperation.SET_DECODER_LATENCY, getCurrentSession());
            }
        }

        if(key.equalsIgnoreCase(this.getResources().getString(R.string.multi_session_mode))){
            Log.d(TAG, "Multi session mode preference changed");
            whisperer.setMultiSessionMode(whisperer.getSharedPrefs().getBoolean(key,false));
            if (whisperer.isMultiSinkMode()) {
                onOptionCreateGroup(null);
            } else {
                onOptionRemoveGroup(null);
            }
        }
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        super.onPrepareOptionsMenu(menu);
        whisperer.setModeDS(false);
        return true;
    }

    // ui event callback
    public void playPauseWfdOperation(View view) {
        Log.d(TAG, "playPauseControlOperation() called");
        if (!playpauseButton.isChecked()) {
            Log.d(TAG, "playPauseWfdOperation: Play button clicked");
            playpauseButton.toggle();
            startWfdOperationTask(this, WfdOperation.PLAY, getCurrentSession());
        } else {
            Log.d(TAG, "playPauseWfdOperation: Pause button clicked");
            playpauseButton.toggle();
            startWfdOperationTask(this, WfdOperation.PAUSE, getCurrentSession());
        }
    }

    // ui event callback
    public void standbyResumeWfdOperation(View view) {
        Log.d(TAG, "standbyResumeWfdOperation() called");
        if (!standbyresumeButton.isChecked()) {
            Log.d(TAG, "standbyResumeWfdOperation: Standby button clicked");
            standbyresumeButton.toggle();
            startWfdOperationTask(this, WfdOperation.STANDBY, getCurrentSession());
        } else {
            Log.d(TAG, "standbyResumeWfdOperation: Resume button clicked");
            standbyresumeButton.toggle();
            startWfdOperationTask(this, WfdOperation.RESUME, getCurrentSession());
        }
    }

    // ui event callback
    public void startStopUibcWfdOperation(View view) {
        Log.d(TAG, "startStopUibcControlOperation() called");
        enableDisableUibcButton.toggle();
        if (!whisperer.isUIBCM14()) {
            if (!enableDisableUibcButton.isChecked()) {
                Log.d(TAG, "Start UIBC button clicked");
                startWfdOperationTask(this, WfdOperation.START_UIBC, getCurrentSession());
            } else {
                Log.d(TAG, "Stop UIBC button clicked");
                startWfdOperationTask(this, WfdOperation.STOP_UIBC, getCurrentSession());
            }
        } else {
            int ret = -1;
            try {
                ret = getCurrentSession().setUIBC();
            } catch (RemoteException e) {
                e.printStackTrace();
            }
            Toast.makeText(getApplicationContext(), "UIBC set: " + ret,
                    Toast.LENGTH_SHORT).show();
            if (0 == ret) {
                enableDisableUibcButton.setText("Start UIBC");
                whisperer.setUIBCM14(false);
            } else {
                enableDisableUibcButton.setText("Send M14");
                whisperer.setUIBCM14(true);
            }
        }
    }

    // ui event callback
    public void toggleDSMode(View view) {
        Log.d(TAG, "toggleDSMode() called");
        toggleDSButton.toggle();
        if(!ServiceUtil.getmServiceAlreadyBound()) {
            Log.e(TAG, "toggleDSMode() called when not in WFD Session");
            Toast.makeText(getApplicationContext(), "Not in WFD Session!",
                    Toast.LENGTH_SHORT).show();
            return;
        }

        try {
            WfdStatus wfdStatus = ServiceUtil.getInstance().getStatus();
            if (wfdStatus.state != WfdEnums.SessionState.PLAY.ordinal()) {
                Log.d(TAG, "Toggle DS Not allowed in non-PLAY state");
                Toast.makeText(getApplicationContext(),
                        "Not honouring DirStr toggle in non-PLAY state",
                        Toast.LENGTH_SHORT).show();
                return;
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        /*Toggle DS Mode*/
        int ret = -1;
        try {
            ret = ServiceUtil.getInstance().toggleDSMode(!whisperer.isModeDS());
        } catch(RemoteException e) {
            e.printStackTrace();
        }

        /*Update UI Button*/
        if(ret == 1) {
            toggleDSButton.toggle();
            whisperer.setModeDS(!whisperer.isModeDS());
            if(toggleDSButton.isChecked()) {
                Log.d(TAG, "Enabling DS");
                toggleDSButton.setText("Disable DS");
            } else {
                Log.d(TAG, "Disabling DS");
                toggleDSButton.setText("Enable DS");
            }
        }
        else {
            Log.d(TAG, "DS toggle is not allowed");
            Toast.makeText(getApplicationContext(),
                        "DS toggle is not allowed",
                        Toast.LENGTH_SHORT).show();
        }
    }

    // ui event callback
    public void toggleRTPTransport(View view) {
        Log.d(TAG, "toggleRTPTransport() called");
        if (ServiceUtil.getmServiceAlreadyBound()) {
            try {
                WfdStatus wfdStatus = getCurrentSession().getStatus();
                if (wfdStatus.state != WfdEnums.SessionState.PLAY.ordinal()) {
                    Log.d(TAG, "Not allowed in non-PLAY state");
                    selectTransportButton.toggle();
                    Toast.makeText(getApplicationContext(),
                            "Not honouring TCP/UDP switch in non-PLAY state",
                            Toast.LENGTH_SHORT).show();
                    return;
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        if (selectTransportButton.isChecked()) {
            Log.d(TAG, "Start TCP Transport");
            selectTransportButton.toggle();
            startWfdOperationTask(this, WfdOperation.SELECT_TCP, getCurrentSession());
        } else {
            Log.d(TAG, "Start UDP Transport");
            selectTransportButton.toggle();
            startWfdOperationTask(this, WfdOperation.SELECT_UDP, getCurrentSession());
        }
    }

    // ui event callback
    public void flushOperation(View view) {
        Log.d(TAG, "flushOperation() called");
        startWfdOperationTask(this, WfdOperation.FLUSH, getCurrentSession());
    }

    // ui event callback
    public void teardownWfdOperation(View view) {
        if (mSessionStack.empty()) {
            Log.d(TAG, "teardownWfdOperation: no sessions");
            return;
        }
        if(view == null) {
            if(whisperer.getLocalDeviceType() != WFDDeviceType.SOURCE) {
                /*
                 * This call has been triggered after receiving network disconnect
                 * event. In case of sink, we should trigger a teardown from the
                 * application level.
                 */
                if(ServiceUtil.getmServiceAlreadyBound()) {
                    Log.d(TAG,"Teardown on network disconnect");
                    try {
                        mSessionStack.peek().teardown();
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            }
            return;
        }
        Log.d(TAG, "Teardown button clicked");
        WfdOperationTask task = new WfdOperationTask(this, "Tearing Down WFD Session", WfdOperation.TEARDOWN, mSessionStack.peek());
        task.execute();
    }

    private NetManagerIF getNetManager() {
        if (whisperer.getNetManager() == null) {
            recreateNetManagerIfNecessary();
        }
        return whisperer.getNetManager();
    }

    private void recreateNetManagerIfNecessary() {
        NetManagerIF netManager = whisperer.getNetManager();
        boolean needsResume = false;
        if (netManager != null && netManager instanceof CombinedNetManager) {
            CombinedNetManager cnetmanager = (CombinedNetManager)netManager;
            if (localExtSupport == 0 &&
                    cnetmanager.getNetTypes().contains(WfdEnums.NetType.LAN)) {
                netManager.destroy();
                netManager = null;
                needsResume = true;
            } else if (localExtSupport == 1 &&
                    !cnetmanager.getNetTypes().contains(WfdEnums.NetType.LAN)) {
                netManager.destroy();
                netManager = null;
                needsResume = true;
            }
        }
        if (netManager == null) {
            if (localExtSupport == 1) {
                Log.v(TAG, "create NetManager including LAN");
                netManager = new CombinedNetManager(mEventHandler,
                        WfdEnums.NetType.WIFI_P2P,
                        WfdEnums.NetType.WIGIG_P2P,
                        WfdEnums.NetType.LAN);
            } else {
                Log.v(TAG, "create NetManager");
                netManager = new CombinedNetManager(mEventHandler,
                        WfdEnums.NetType.WIFI_P2P,
                        WfdEnums.NetType.WIGIG_P2P);
            }
            if (needsResume) {
                netManager.onMainActivityResume();
            }
            netManager.send_wfd_set(true, whisperer.getLocalDeviceType());
            whisperer.setNetManager(netManager);
        }
    }

    private class PreTeardownTaskSessionCheck extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... params) {
            try {
                //verify session state
                WfdStatus wfdStatus = getCurrentSession().getStatus();

                Log.d(TAG, "wfdStatus.state= " + wfdStatus.state);
                if (wfdStatus.state == WfdEnums.SessionState.INVALID.ordinal() ||
                        wfdStatus.state == WfdEnums.SessionState.INITIALIZED.ordinal()) {
                    Log.d(TAG, "wfdStatus.state= " + wfdStatus.state);
                    return false;
                } else {
                    return true;
                }
            } catch (RemoteException e) {
                Log.e(TAG, "PreTeardownTaskSessionCheck- Remote exception", e);
            }
            return true;
        }

        @Override
        protected void onPostExecute(Boolean callTeardown) {
            Log.d(TAG, "PreTeardownTaskSessionCheck- onPostExecute");
            Log.d(TAG, "PreTeardownTaskSessionCheck- callTeardown: " + callTeardown);
            if (callTeardown) {
                Log.d(TAG, "PreTeardownTaskSessionCheck- now calling teardown");
            } else {
                Log.d(TAG, "PreTeardownTaskSessionCheck- not in a WFD session, not going to call teardownOperation");
            }
        }
    }

    class StartSessionTask extends AsyncTask<Void, Integer, Integer> {
        static final String TAG = "StartSessionTask";

        // events
        static final int START_SESSION_FAILED_LOCAL_DEV_NULL = 101;
        static final int START_SESSION_FAILED_TO_GET_PEER_IP = 102;
        static final int START_SESSION_SHOW_PROGRESS = 103;
        static final int START_SESSION_FAILED = 104;
        static final int START_SESSION_TIMED_OUT = 105;
        static final int START_SESSION_SUCCESS = 106;

        private final IWfdSession wfdSession;
        private final Handler mHandler;
        private final NetDeviceIF mLocalDev;
        private final NetDeviceIF mPeerDev;
        private final WfdEnums.WFDDeviceType mLocalDeviceType;
        private final WfdEnums.AVPlaybackMode mAVMode;

        public StartSessionTask(IWfdSession session, Handler handler,
                                NetDeviceIF peerDev, WfdEnums.WFDDeviceType deviceType,
                                WfdEnums.AVPlaybackMode avMode) {
            wfdSession = session;
            mHandler = handler;
            mLocalDev = peerDev.getNetTypeManager().getLocalDevice();
            mPeerDev = peerDev;
            mLocalDeviceType = deviceType;
            mAVMode = avMode;
        }

        @Override
        protected Integer doInBackground(Void... devices) {
            Log.d(TAG, "StartSessionTask: doInBackground called");
            int ret = -1;

            if (!mPeerDev.supportWfd()) {
                Log.e(TAG, "StartSessionTask: doInBackground- Device is missing wfdInfo");
                Message messageInit = mHandler.obtainMessage(WfdClientUtils.INVALID_WFD_DEVICE);
                mHandler.sendMessage(messageInit);
                return ret;
            }

            try {
                Log.d(TAG, "StartSessionTask- doInBackground- Setting surface to a surface");
                if (mLocalDev == null) {
                    mHandler.obtainMessage(START_SESSION_FAILED_LOCAL_DEV_NULL).sendToTarget();
                    return ret;
                }

                WfdDevice localWfdDev = mLocalDev.convertToWfdDevice();
                WfdDevice peerWfdDev = null;

                int timeout = 0;
                while (timeout++ < 60) { // try 60 seconds for IP to get populated else
                    peerWfdDev = mPeerDev.convertToWfdDevice();
                    if (peerWfdDev != null && peerWfdDev.ipAddress != null) {
                        break;
                    }
                    if (isCancelled()) {
                        Log.e(TAG, "getIP Task was cancelled");
                        return -1;
                    }
                    publishProgress(timeout);
                    Log.e(TAG, "Failed to get IP from lease file in attempt " + timeout);

                    if (peerWfdDev == null || (mLocalDeviceType != WfdEnums.WFDDeviceType.SOURCE && peerWfdDev.ipAddress == null)) {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            Log.e(TAG, "Background thread of async task interrupted");
                        }
                        continue;
                    } else {
                        break;
                    }
                }

                Log.v(TAG, "Local device " + localWfdDev.ipAddress + ":" + localWfdDev.rtspPort
                        + ", type: " + localWfdDev.deviceType + ", net: " + localWfdDev.netType);
                Log.v(TAG, "Peer device " + peerWfdDev.ipAddress + ":" + peerWfdDev.rtspPort
                        + ", type: " + peerWfdDev.deviceType + ", net: " + peerWfdDev.netType);

                peerWfdDev.decoderLatency = Whisperer.getInstance().getDecoderLatency();
                localWfdDev.extSupport = localExtSupport;
                int initReturn = wfdSession.init(mActionListener, localWfdDev);
                if (!(initReturn == 0 || initReturn == WfdEnums.ErrorType.ALREADY_INITIALIZED.getCode())) {
                    Log.e(TAG, "StartSessionTask- doInBackground: init failed with error- " + initReturn);
                }
                if (peerWfdDev.ipAddress == null && mLocalDeviceType != WfdEnums.WFDDeviceType.SOURCE) {
                    Log.e(TAG, "StartSessionTask- doInBackground: ipaddress null pop up");
                    return ret;
                }
                peerWfdDev.extSupport = peerExtSupprt;
                Log.d(TAG,"Starting session between devices with local R2 support: " + localWfdDev.extSupport + " peer R2 support: " + peerWfdDev.extSupport);
                ret = wfdSession.startWfdSession(peerWfdDev);
                Log.v(TAG, "StartSessionTask session:" + wfdSession + ", decoderLatency: " + peerWfdDev.decoderLatency);
            } catch (RemoteException e) {
                Log.e(TAG, "Remote exception", e);
            }
            return ret;
        }

        @Override
        protected void onPreExecute() {
            if( mLocalDeviceType == WfdEnums.WFDDeviceType.SOURCE) {
                Whisperer.getInstance().getNetManager().setMiracastMode(NetManagerIF.MIRACAST_SOURCE);
            }
            if (mAVMode != null) {
                try {
                    wfdSession.setAvPlaybackMode(mAVMode.ordinal());
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            try {
                wfdSession.setDeviceType(mLocalDeviceType.getCode());
            } catch (RemoteException e) {
                e.printStackTrace();
            }

            if (mLocalDeviceType != WfdEnums.WFDDeviceType.SOURCE) {
                showHideWfdSurface(wfdSession);
                Log.d(TAG, "StartSessionTask: onPreExecute- device is multi sink, showHideWfdSurface");
            }
            Log.d(TAG, "StartSessionTask- onPreExecute() called");
            mHandler.obtainMessage(START_SESSION_SHOW_PROGRESS).sendToTarget();
            Log.d(TAG, "StartSessionTask: onPreExecute- end");
        };

        @Override
        protected void onCancelled() {
            Log.d(TAG, "StartSessionTask: onCancelled called because" +
                    " startSessionTask returned");
        }

        @Override
        protected void onPostExecute(Integer sessionId) {
            Log.d(TAG, "StartSessionTask: onPostExecute() called");

            if (sessionId == WfdEnums.ErrorType.OPERATION_TIMED_OUT.getCode()) {
                Log.d(TAG, "StartSessionTask: onPostExecute- Start session is taking longer than expected");
                mHandler.obtainMessage(START_SESSION_TIMED_OUT).sendToTarget();
            } else if (sessionId < 0) {
                Log.d(TAG, "StartSessionTask: onPostExecute- sessionId < 0, Failed to start session with error: " + sessionId);
                mHandler.obtainMessage(START_SESSION_FAILED, sessionId, 0).sendToTarget();
                return;
            } else {
                Log.d(TAG, "StartSessionTask: completed successfully.");
                Whisperer.getInstance().setInSession(true);
                mHandler.obtainMessage(START_SESSION_SUCCESS, sessionId, 0).sendToTarget();
            }
        }
    }

    // ui event callback
    public void connectClickHandler(View v) {
        Log.d(TAG, "connectClickHandler received click");
        DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
                .findFragmentById(R.id.frag_list);
        fragmentList.handleConnectClick(v);
    }

    // ui event callback
    public void connectOptionsClickHandler(View v) {
        Log.d(TAG, "connectOptionsClickHandler received click");
        DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
                .findFragmentById(R.id.frag_list);
        fragmentList.handleConnectOptionsClick(v);
    }

    // ui event callback
    public void disconnectClickHandler(View v) {
        Log.d(TAG, "disconnectClickHandler received click");
        disconnect();
    }

    // ui event callback
    public void startClientClickHandler(View v) {
        Log.d(TAG, "startClientClickHandler received click");
        DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
                .findFragmentById(R.id.frag_list);
        fragmentList.handleStartClientClick(v);
    }

    // ui event callback
    public void filterClickHandler(View V) {
        Log.d(TAG, "filterClickHandler received click");
        DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
                .findFragmentById(R.id.frag_list);
        fragmentList.handleFilterClick();
    }

    /**
     * Remove all peers and clear all fields. This is called on
     * BroadcastReceiver receiving a state change event.
     */
    public void resetData() {
        Log.d(TAG, "resetData() called");
        DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
                .findFragmentById(R.id.frag_list);
        if (fragmentList != null) {
            fragmentList.clearPeers();
        }
    }

    public void disconnect() {
        Log.d(TAG, "disconnect() called");
        getNetManager().disconnect(ResultListener.NullListener);
    }

    @Override
    public void cancelDisconnect() {
		/*
		 * A cancel abort request by user. Disconnect i.e. removeGroup if
		 * already connected. Else, request WifiP2pManager to abort the ongoing
		 * request
		 */
        getNetManager().cancelConnect(new ResultListener() {
            @Override
            public void onSuccess(Object result) {
                Toast.makeText(WFDClientActivity.this,
                        "Aborting connection",
                        Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFailure(Object err) {
                Toast.makeText(
                        WFDClientActivity.this,
                        "Connect abort request failed. Reason Code: "
                                + err,
                        Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle item selection
        switch (item.getItemId()) {
		/*case R.id.menu_advanced_settings:
			startActivity(new Intent(this, AdvancedPreferences.class));
			break;*/
            case R.id.menu_preferences:
                Log.d(TAG, "prefences clicked");
                startActivity(new Intent(this, Preferences.class));
                break;
            default:
                Log.d(TAG, "No menu item selected");
        }
        return super.onOptionsItemSelected(item);
    }

    public void onOptionSelectSourceOrSink(MenuItem i) {
        Log.d(TAG, "Source or Sink selected from action bar");
        if (i.getTitle().toString().contains("ource")) {
            Log.d(TAG, "Source selected from action bar");
            whisperer.setLocalDeviceType(WFDDeviceType.SOURCE);
            if (ServiceUtil.getmServiceAlreadyBound()) {
                try {
                    for (IWfdSession session: mSessionStack) {
                        session.setDeviceType(whisperer.getLocalDeviceType().getCode());
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "Remote exception when setting device type", e);
                }
            } else {
                try {
                    ServiceUtil.bindService(getApplicationContext(), mEventHandler);
                } catch (ServiceFailedToBindException e) {}
            }
            getNetManager().send_wfd_set(true, WFDDeviceType.SOURCE);
            Toast.makeText(WFDClientActivity.this,
                    "This device is now Source",
                    Toast.LENGTH_SHORT).show();

        } else {
            Log.d(TAG, "Sink selected from action bar");
            whisperer.setLocalDeviceType(WFDDeviceType.PRIMARY_SINK);
            if (ServiceUtil.getmServiceAlreadyBound()) {
                try {
                    for (IWfdSession session: mSessionStack) {
                        session.setDeviceType(whisperer.getLocalDeviceType().getCode());
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "Remote exception when setting device type", e);
                }
            } else {
                try {
                    ServiceUtil.bindService(getApplicationContext(), mEventHandler);
                } catch (ServiceFailedToBindException e) {}
            }
            getNetManager().send_wfd_set(true, WFDDeviceType.PRIMARY_SINK);
            Toast.makeText(WFDClientActivity.this,
                    "This device is now Sink",
                    Toast.LENGTH_SHORT).show();
        }
        switch(i.getItemId()) {
            case R.id.menu_r1sr1k:
            case R.id.menu_r1kr1s:
                localExtSupport = 0;
                peerExtSupprt = 0;
                break;
            case R.id.menu_r1sr2k:
            case R.id.menu_r1kr2s:
                localExtSupport = 0;
                peerExtSupprt = 1;
                break;
            case R.id.menu_r2sr1k:
            case R.id.menu_r2kr1s:
                localExtSupport = 1;
                peerExtSupprt = 0;
                break;
            case R.id.menu_r2sr2k:
            case R.id.menu_r2kr2s:
                localExtSupport = 1;
                peerExtSupprt = 1;
                break;
        }
        i.setChecked(true);
    }

    public void onOptionFind(MenuItem i) {
        try {
            ServiceUtil.bindService(getApplicationContext(), mEventHandler);
        } catch (ServiceFailedToBindException e) {
            Log.e(TAG, "ServiceFailedToBindException received");
        }
        if (!whisperer.isWifiP2pEnabled()) {
            Log.d(TAG, "Find selected from action bar while p2p off");
            Toast.makeText(WFDClientActivity.this,
                    R.string.p2p_off_warning, Toast.LENGTH_SHORT)
                    .show();
        } else {
            Log.d(TAG, "Find selected from action bar");
            recreateNetManagerIfNecessary();
            whisperer.setDiscoveryRequested(true);
            final DeviceListFragment fragment = (DeviceListFragment) getFragmentManager()
                    .findFragmentById(R.id.frag_list);
            fragment.onInitiateDiscovery();
            getNetManager().discoveryPeers(new ResultListener() {
                @Override
                public void onSuccess(Object result) {
                    Toast.makeText(WFDClientActivity.this,
                            "Discovery Initiated",
                            Toast.LENGTH_SHORT).show();
                }

                @Override
                public void onFailure(Object err) {
                    Toast.makeText(WFDClientActivity.this,
                            "Discovery Failed : " + err,
                            Toast.LENGTH_SHORT).show();
                }
            });
        }
    }

    public void onOptionCreateGroup(MenuItem i) {
        if (!whisperer.isWifiP2pEnabled()) {
            Log.d(TAG, "Create Group selected from menu while p2p off");
            Toast.makeText(WFDClientActivity.this, R.string.p2p_off_warning,
                    Toast.LENGTH_SHORT).show();
        } else {
            if (!whisperer.isGroupCreated()) {
                Log.d(TAG, "Create Group selected from menu");
                getNetManager().createGroup(new ResultListener() {
                    @Override
                    public void onSuccess(Object result) {
                        Toast.makeText(WFDClientActivity.this, "Group created",
                                Toast.LENGTH_SHORT).show();
                    }

                    @Override
                    public void onFailure(Object err) {
                        Toast.makeText(WFDClientActivity.this,
                                "Group create failed : " + err,
                                Toast.LENGTH_SHORT).show();
                    }
                });
                whisperer.setGroupCreated(true);
            }
        }
    }

    public void onOptionRemoveGroup(MenuItem i) {
        if (!whisperer.isWifiP2pEnabled()) {
            Log.d(TAG, "Remove Group selected from menu while p2p off");
            Toast.makeText(WFDClientActivity.this, R.string.p2p_off_warning,
                    Toast.LENGTH_SHORT).show();
        } else {
            if (whisperer.isGroupCreated()) {
                Log.d(TAG, "Remove Group selected from menu");
                getNetManager().removeGroup(new ResultListener() {
                    @Override
                    public void onSuccess(Object result) {
                        Toast.makeText(WFDClientActivity.this, "Group removed",
                                Toast.LENGTH_SHORT).show();
                    }

                    @Override
                    public void onFailure(Object err) {
                        Toast.makeText(WFDClientActivity.this,
                                "Group remove failed : " + err,
                                Toast.LENGTH_SHORT).show();
                    }
                });
                whisperer.setGroupCreated(false);
            }
        }
    }

    public void onOptionPreferences(MenuItem i) {
        Log.d(TAG, "Preferences selected from menu");
        startActivity(new Intent(this, Preferences.class));
    }

    public void onOptionVersion(MenuItem i) {
        Log.d(TAG, "Version selected from menu");
        try {
            String version = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
            AlertDialog alertDialog = new AlertDialog.Builder(this).create();
            alertDialog.setTitle("Version #");
            alertDialog.setMessage(version);
            alertDialog.show();
        } catch (NameNotFoundException e) {
            Log.e("tag", e.getMessage());
        }
    }

    public void onOptionShowHideWfdOperationButtons(MenuItem i) {
        setVisibleControlButtons();
    }

    void unbindWfdService() {
        Log.d(TAG, "unbindWfdService() called");
        if(ServiceUtil.getmServiceAlreadyBound()) {
            ServiceUtil.unbindService(getApplicationContext());
        }
    }

    @Override
    public void startSession(NetDeviceIF inDevice) {
        if(!ServiceUtil.getmServiceAlreadyBound()){
				/* This is not a normal scenario as we always bind service in onOptionFind()
				* Its almost impossible for user to start session without pressing find.
				* Just addressing the corner case here
				*/
            Log.e(TAG, "Service is not already bound, do it now");
            try {
                ServiceUtil.bindService(getApplicationContext(), mEventHandler);
            } catch (ServiceUtil.ServiceFailedToBindException e) {
                Log.e(TAG, "ServiceFailedToBindException received");
            }
        }

        if (inDevice == null && whisperer.isMultiSinkMode()) {
            throw new IllegalArgumentException("InDevice is null in multi sink mode");
        }

        Log.d(TAG, "startSession() called for " + (inDevice != null? inDevice.getName() : "unknown"));
        //final DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
        //.findFragmentById(R.id.frag_list);

        NetDeviceIF localDevInfo = inDevice.getNetTypeManager().getLocalDevice();
        if (!localDevInfo.hasDeviceInfo()) {
            Log.e(TAG, "Something amiss!! local device is null");
            return;
        }

        if (inDevice.getNetType() == WfdEnums.NetType.LAN) {
            ((LanDevice)inDevice).sendConnectionRequestIfNecessary();
        }
        final IWfdSession session = createNewSessionInstance();
        mSessionStack.push(session);

        AVPlaybackMode av = AVPlaybackMode.AUDIO_VIDEO;
        if(whisperer.isSourceMode() && ServiceUtil.getmServiceAlreadyBound()) {
            String avMode = whisperer.getSharedPrefs().getString("av_mode_type", "Audio_Video");
            av = AVPlaybackMode.AUDIO_VIDEO;
            if(avMode.equals("Audio_Only")) {
                av = AVPlaybackMode.AUDIO_ONLY;
            } else if(avMode.equals("Video_Only")) {
                av = AVPlaybackMode.VIDEO_ONLY;
            }
            Log.d(TAG, "AV Mode = " + av);
        }

        StartSessionTask task = new StartSessionTask(session, mEventHandler,inDevice, whisperer.getLocalDeviceType(),av);
        mCurrentStartSessionTask = new WeakReference<StartSessionTask>(task);
        Log.d(TAG, "Decoder Latency " + whisperer.getDecoderLatency());
        Log.d(TAG, "Start session with " + inDevice.getAddress());
        task.execute();
    }

    @Override
    public void stopSession(int sessionId) {
        Log.d(TAG, "stopSession called");
        try {
            getCurrentSession().stopWfdSession();
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception", e);
        }
        setGoneControlButtons();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && whisperer.isInSession()) {
            Log.d(TAG, "Back button pressed while starting session");
            teardownWfdOperation(null);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_BACK) {
            Log.d(TAG, "Back button pressed");
            AlertDialog.Builder builder = new AlertDialog.Builder(WFDClientActivity.this);
            builder.setMessage("Would you like to exit the application?")
                    .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            WFDClientActivity.this.finish();
                        }
                    })
                    .setNegativeButton("No", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            dialog.cancel();
                        }
                    });
            AlertDialog alert = builder.create();
            alert.show();
        }
        return super.onKeyDown(keyCode, event);
    }

    /**
     * Update UI for this device.
     *
     * @param device WifiP2pNetDevice object
     */
    public void updateThisDevice(NetDeviceIF device) {
        Log.d(TAG, "updateThisDevice called");

        if (getNetManager() instanceof CombinedNetManager) {
            Map<WfdEnums.NetType, NetDeviceIF> localDevices = ((CombinedNetManager) getNetManager()).getLocalDevices();
            for (WfdEnums.NetType type: localDevices.keySet()) {
                NetDeviceIF dev = localDevices.get(type);
                if (!dev.hasDeviceInfo()) {
                    continue;
                }
                if (type == WfdEnums.NetType.WIFI_P2P) {
                    mynameTextView.setText(dev.getName());
                    mymacaddressTextView.setText(dev.getAddress());
                    mystatusTextView.setText(dev.getStatusString());
                } else if (type == WfdEnums.NetType.WIGIG_P2P) {
                    mynameTextViewWigig.setText(dev.getName());
                    mymacaddressTextViewWigig.setText(dev.getAddress());
                    mystatusTextViewWigig.setText(dev.getStatusString());
                } else {
                    Log.e(TAG, "unknown net type: " + type);
                }
            }
        }
    }

    private void handleConnectionRequest(final NetDeviceIF device) {
        if (whisperer.isMultiSinkMode()) {
            startSession(device);
            return;
        }

        if (!mSessionStack.empty()) {
            Log.w(TAG, "Session exists, ignore connection request");
            return;
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(WFDClientActivity.this);
        builder.setMessage("Accept request connection command from " + device.getIp() + "?")
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        startSession(device);
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });
        AlertDialog alert = builder.create();
        alert.show();
    }

    @Override
    public void peersReceived() {
        Log.d(TAG, "peersReceived() called");
        whisperer.setDiscoveryRequested(false);
        if (getNetManager() instanceof CombinedNetManager) {
            if (((CombinedNetManager)getNetManager()).allSubNetPeersReceived()) {
                Log.d(TAG, "all nets have peers now");
                whisperer.setDiscoveryRequested(false);
            }
        } else {
            whisperer.setDiscoveryRequested(false);
        }
    }

    @Override
    public WFDDeviceType getLocalDeviceType() {
        Log.d(TAG, "getLocalDeviceType() called");
        return whisperer.getLocalDeviceType();
    }

    private void setUIBCButton() {
        Bundle cfgItem = new Bundle();
        int ret = 0;
        try {
            ret = getCurrentSession().getConfigItems(cfgItem);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        int[] configArr = cfgItem.getIntArray(WfdEnums.CONFIG_BUNDLE_KEY);
        if (configArr!=null && 1 == configArr[WfdEnums.ConfigKeys.UIBC_M14.ordinal()]) {
            whisperer.setUIBCM14(true);
            enableDisableUibcButton.setText("Send M14");
        } else {
            whisperer.setUIBCM14(false);
        }
        Log.d(TAG, "setUIBCButton() M14 support is " + whisperer.isUIBCM14());
    }

    public void setVisibleControlButtons() {
        Log.d(TAG, "setVisibleControlButtons() called");
        controlbuttonsTableLayout.setVisibility(View.VISIBLE);
        setUIBCButton();
    }

    public void setGoneControlButtons() {
        Log.d(TAG, "setGoneControlButtons() called");
        controlbuttonsTableLayout.setVisibility(View.GONE);
    }

    public void teardownWFDService() {
        Log.d(TAG, "teardownWFDService() called");
        PreTeardownTaskSessionCheck task = new PreTeardownTaskSessionCheck();
        task.execute();
    }

    public void deinitWFDService() {
        try {
            Log.e(TAG, "WiFi Direct deinit do in background started");
            getCurrentSession().deinit(); // XXX
            Log.e(TAG, "WiFi Direct deinit do in background finished");
        } catch (RemoteException e) {
            Log.e(TAG, "Remote exception", e);
        }
    }

    /**
     * Class for internal event handling in WFDClientActivity. Must run on UI thread.
     */
    class ClientEventHandler extends Handler {

        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "Event handler received: " + msg.what);

            if (WfdClientUtils.wfdOperationProgressDialog != null) {
                Log.d(TAG, "clientProgressDialog != null");
                if (WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                    Log.d(TAG, "clientProgressDialog isShowing");
                } else {
                    Log.d(TAG, "clientProgressDialog not isShowing");
                }
            } else {
                Log.d(TAG, "clientProgressDialog == null");
            }

            final DeviceListFragment fragmentList = (DeviceListFragment) getFragmentManager()
                    .findFragmentById(R.id.frag_list);

            switch (msg.what) {

                case PLAY_CALLBACK: {
                    playpauseButton.setChecked(false);
                    standbyresumeButton.setChecked(true);
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                        Log.d(TAG, "clientProgressDialog dismissed");
                    }
                }
                break;
                case PAUSE_CALLBACK: {
                    playpauseButton.setChecked(true);
                    standbyresumeButton.setChecked(true);
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                        Log.d(TAG, "clientProgressDialog dismissed");
                    }
                }
                break;
                case STANDBY_CALLBACK: {
                    standbyresumeButton.setChecked(false);
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                        Log.d(TAG, "clientProgressDialog dismissed");
                    }
                }
                break;
                case SET_WFD_FINISHED: {
                    if (msg.arg1 == -1) {
                        AlertDialog.Builder builder = new AlertDialog.Builder(
                                WFDClientActivity.this);
                        builder.setTitle("WFD failed to turn on")
                                .setMessage("Would you like to exit the application?")
                                .setCancelable(false)
                                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int id) {
                                        finish();
                                    }
                                })
                                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog, int id) {
                                        dialog.cancel();
                                    }
                                });
                        AlertDialog alert = builder.create();
                        alert.show();
                    } else if (msg.arg1 == 0) {
                        //Toast.makeText(WFDClientActivity.this, "WFD turned on", Toast.LENGTH_SHORT).show();
                    }
                }
                break;
                case INVALID_WFD_DEVICE: {
                    AlertDialog.Builder builder = new AlertDialog.Builder(WFDClientActivity.this);
                    builder.setTitle("Selected device does not support WFD")
                            .setMessage("Please select another device to start a WFD session")
                            .setCancelable(false)
                            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    dialog.cancel();
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                }
                break;
                case SERVICE_BOUND: {
                    mActionListener = WfdClientUtils.createActionListener(mEventHandler);
                    setGoneControlButtons();
                    Toast.makeText(getApplicationContext(), "WFD service connected",
                            Toast.LENGTH_SHORT).show();
                }
                break;
                case CLEAR_UI: {
                    if (startSessionProgressDialog != null) {
                        startSessionProgressDialog.dismiss();
                    }
                    if(!whisperer.isSourceMode()) {
                        Log.e(TAG, "Calling finish on SurfaceActivity");
                        try {
                            finishActivity(START_SURFACE);
                        } catch (ActivityNotFoundException e) {
                            Log.e(TAG, "Surface Activity not yet started/already finished");
                        }
                    }
                }
                //fall through to TEARDOWN_CALLBACK case handling for cleanup
                case TEARDOWN_CALLBACK: {
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        Log.d(TAG, "EventHandler: teardownCallback- dismiss clientProgressDialog");
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                    }

                    if (startSessionProgressDialog != null) {
                        startSessionProgressDialog.dismiss();
                    }

                    cancelDisconnect();
                    if (fragmentList != null) {
                        Log.d(TAG, "EventHandler: teardownCallback- teardown fragment");
                        fragmentList.doTeardown();
                    }

                    Log.d(TAG, "EventHandler: teardownCallback- setGoneControlButtons");
                    setGoneControlButtons();
                    enableDisableUibcButton.setChecked(false);
                    selectTransportButton.setChecked(false);
                    toggleDSButton.setChecked(false);

                    Log.d(TAG, "EventHandler: teardownCallback-  control buttons gone");
                    getNetManager().send_wfd_set(true, whisperer.getLocalDeviceType());

                    getNetManager().setMiracastMode(NetManagerIF.MIRACAST_DISABLED);
                    Toast.makeText(WFDClientActivity.this, "WiFi Direct Teardown",
                            Toast.LENGTH_SHORT).show();
                    if (!whisperer.isMultiSinkMode() ||
                            whisperer.getLocalDeviceType() == WfdEnums.WFDDeviceType.SOURCE) {
                        if (ServiceUtil.getmServiceAlreadyBound()) {
                            try {
                                ServiceUtil.getInstance().unregisterListener(
                                        mActionListener);
                            } catch (RemoteException e) {
                                Log.e(TAG, "RemoteException in un-registering listener" + e);
                            }
                        }
                        Log.d(TAG,"unbinding from service. Source or Not in multi-session mode");

                        if(!mSessionStack.empty()) {
                            IWfdSession currSesion = mSessionStack.pop();
                            if(currSesion != null)
                            {
                                try {
                                    currSesion.deinit();
                                } catch (RemoteException e) {
                                    Log.e(TAG, "RemoteException in deInit");
                                }
                            }
                        }

                        if (ServiceUtil.getmServiceAlreadyBound())
                        {
                            try {
                                ServiceUtil.getInstance().deinit();
                            } catch (RemoteException e) {
                                Log.e(TAG, "RemoteException in deInit");
                            }
                        }
                        mSessionStack.clear();
                        unbindWfdService();
                    } else {
                        Log.d(TAG,"Not unbinding from service in multi-session mode");
                    }
                    Log.d(TAG, "EventHandler: teardownCallback- completed");
                    whisperer.setInSession(false);
                    whisperer.setModeDS(false);
                }
                break;

                case START_SESSION_ON_UI: {
                    if (startSessionProgressDialog != null
                            && startSessionProgressDialog.isShowing()) {
                        startSessionProgressDialog.dismiss();
                        Log.d(TAG, "EventHandler: startSessionOnUI- progress dialog closed");
                    }
                    if (whisperer.isSourceMode()) {
                        Log.d(TAG, "EventHandler: startSessionOnUI- device is source, setVisibleControlButtons");
                        setVisibleControlButtons();
                    }
                    if (!whisperer.isMultiSinkMode()) {
                        getNetManager().send_wfd_set(false, whisperer.getLocalDeviceType());
                    } else {
                        Log.e(TAG,"Letting device stay available for another WFD session");
                    }
                    Log.d(TAG, "EventHandler: startSessionOnUI- completed");
                }
                break;
                case UIBC_ACTION_COMPLETED: {
                    enableDisableUibcButton.toggle();
                    if (enableDisableUibcButton.isChecked()) {
                        if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                            WfdClientUtils.wfdOperationProgressDialog.dismiss();
                            Log.d(TAG,"clientProgressDialog dismissed on start UIBC successful");
                        }
                    } else {
                        if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                            WfdClientUtils.wfdOperationProgressDialog.dismiss();
                            Log.d(TAG,"clientProgressDialog dismissed on stop UIBC successful");
                        }
                    }
                    Log.d(TAG, "EventHandler: uibcActionCompleted- completed");
                }
                break;
                case REMOVE_WFD_SESSION:
                    IWfdSession wfdSession= IWfdSession.Stub.asInterface((android.os.IBinder) msg.obj);
                    if(mSessionStack.remove(wfdSession)) {
                        Log.d(TAG,"Removed WFD session");
                    } else {
                        Log.wtf(TAG,"Unable to find session in stack!");
                    }
                    break;
                case RTP_TRANSPORT_NEGOTIATED: {
                    Log.d(TAG,"EventHandler : RTP Port Negotiation Successful");
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        Log.d(TAG, "EventHandler: TCP/ UDP Success - dismiss clientProgressDialog");
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                    }
                    selectTransportButton.toggle();
                }
                break;

                case RTP_TRANSPORT_NEGOTIATED_FAIL: {
                    Log.d(TAG, "EventHandler : RTP Port Negotiation Failed");
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        Log.d(TAG, "EventHandler: TCP/ UDP Failure - dismiss clientProgressDialog");
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                    }
                }
                break;

                case TCP_PLAYBACK_CONTROL_DONE: {
                    Log.d(TAG,"EventHandler : Playback control successful");
                    if (WfdClientUtils.wfdOperationProgressDialog != null && WfdClientUtils.wfdOperationProgressDialog.isShowing()) {
                        Log.d(TAG, "EventHandler: Control Success - dismiss clientProgressDialog");
                        WfdClientUtils.wfdOperationProgressDialog.dismiss();
                    }
                }
                break;

                case StartSessionTask.START_SESSION_FAILED_LOCAL_DEV_NULL: {
                    AlertDialog.Builder builder = new AlertDialog.Builder(WFDClientActivity.this);
                    builder.setTitle("Start session cannot proceed")
                            .setMessage("This device is null")
                            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    return;
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                    Message messageFailSession = mEventHandler.obtainMessage(WfdClientUtils.CLEAR_UI);
                    mEventHandler.sendMessage(messageFailSession);
                }
                break;
                case StartSessionTask.START_SESSION_FAILED_TO_GET_PEER_IP: {
                    AlertDialog.Builder builder = new AlertDialog.Builder(WFDClientActivity.this);
                    builder.setTitle("Start session cannot proceed")
                            .setMessage("Unable to obtain peer ip address")
                            .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    return;
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                    Message messageFailSession = mEventHandler.obtainMessage(WfdClientUtils.CLEAR_UI);
                    mEventHandler.sendMessage(messageFailSession);
                }
                break;
                case StartSessionTask.START_SESSION_SHOW_PROGRESS: {
                    startSessionProgressDialog = ProgressDialog.show(WFDClientActivity.this,
                            "Starting WFD Session", "Press back to cancel", true, true);

                    startSessionProgressDialog.setCancelable(true);
                    startSessionProgressDialog.setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface dialog) {
                            Log.d(TAG, "StartSessionTask- onCancel called before");
                            StartSessionTask task = mCurrentStartSessionTask.get();
                            if (task != null) {
                                task.cancel(true);
                            }
                            try {
                                if (ServiceUtil.getmServiceAlreadyBound()) {
                                    //wfdSession.teardown();
                                    mSessionStack.pop().teardown();
                                }
                            } catch (RemoteException e) {
                                Log.e(TAG, "RemoteException in teardown");
                            }
                            Log.d(TAG, "StartSessionTask- onCancel called after");
                        }
                    });

                }
                break;
                case StartSessionTask.START_SESSION_FAILED: {
                    Toast.makeText(WFDClientActivity.this,
                            "Failed to start session with error: " + msg.arg1,
                            1500).show();
                }
                break;
                case StartSessionTask.START_SESSION_TIMED_OUT: {
                    AlertDialog.Builder builder = new AlertDialog.Builder(WFDClientActivity.this);
                    builder.setTitle("Start session is taking longer than expected")
                            .setMessage("Would you like to continue waiting?")
                            .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    Log.d(TAG, "StartSessionTask: onPostExecute- User clicked yes (would like to continue waiting)");
                                    return;
                                }
                            })
                            .setNegativeButton("No", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    Log.d(TAG, "StartSessionTask: onPostExecute- User clicked no (would not like to continue waiting)");
                                    teardownWfdOperation(null);
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.show();
                }
                break;
                case StartSessionTask.START_SESSION_SUCCESS: {
                    if (!whisperer.isMultiSinkMode()) {
                        getNetManager().stopPeerDiscovery(ResultListener.NullListener);
                    }
                }
                break;
                case NetManagerIF.WIFI_UTIL_RETRY_CHANNEL: {
                    Toast.makeText(WFDClientActivity.this, "Channel lost. Trying again",
                            Toast.LENGTH_LONG).show();
                    resetData();
                }
                break;
                case NetManagerIF.WIFI_UTIL_CHANNAL_LOST_PERMANENTLY: {
                    Toast.makeText(
                            WFDClientActivity.this,
                            "Severe! Channel is probably lost permanently. Try Disable/Re-Enable P2P.",
                            Toast.LENGTH_LONG).show();
                }
                break;
                case NetManagerIF.WIFI_UTIL_P2P_STATE_CHANGED: {
                    setIsWifiP2pEnabled(getNetManager().isEnabled());
                }
                break;
                case NetManagerIF.WIFI_UTIL_DISCONNECTED: {
                    resetData();
                }
                break;
                case NetManagerIF.WIFI_UTIL_CONNECTION_INFO: {
                    WfdEnums.NetType netType = WfdEnums.NetType.values()[msg.arg1];
                    if (fragmentList != null) {
                        fragmentList.onConnectionInfoAvailable(netType, (ConnectionInfoIF) msg.obj);
                    }
                }
                break;
                case NetManagerIF.WIFI_UTIL_PEERS_CHANGED: {
                    Collection<NetDeviceIF> peers = (Collection<NetDeviceIF>)msg.obj;
                    if (fragmentList != null) {
                        fragmentList.onPeersChanged(peers);
                    }
                }
                break;
                case NetManagerIF.WIFI_UTIL_CONNECTED_PEERS_CHANGED: {
                    Collection<NetDeviceIF> connectedPeers = (Collection<NetDeviceIF>)msg.obj;
                    if (fragmentList != null) {
                        fragmentList.onConnectedPeersChanged(connectedPeers);
                    }
                }
                break;
                case NetManagerIF.WIFI_UTIL_LOCAL_DEVICE_CHANGED: {
                    updateThisDevice((NetDeviceIF)msg.obj);
                }
                break;
                case NetManagerIF.WIFI_UTIL_CONNECTION_REQUEST: {
                    handleConnectionRequest((NetDeviceIF) msg.obj);
                }
                break;
                default:
                    Log.e(TAG, "Unknown event received: " + msg.what);
            }
        }
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult() called with "+ resultCode +"because of activity"
                + "started with " + requestCode);
        mSurfaceStared = false;
        if(requestCode == START_SURFACE && resultCode == RESULT_CANCELED) {
            Log.e(TAG, "Surface Activity has been destroyed , clear up UI");
            Message messageClearUI = mEventHandler.obtainMessage(CLEAR_UI);
            mEventHandler.sendMessage(messageClearUI);
        }
    }

    public void showHideWfdSurface(final IWfdSession wfdsession) {
        if (!mSurfaceStared) {
            Intent i = new Intent(this, SurfaceActivity.class);
            i.setFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            //The below method is listed as deprecated, but its usable for now
            i.putExtra(WFD_SESSION_BINDER_KEY, wfdsession.asBinder());
            startActivityForResult(i, START_SURFACE);
            mSurfaceStared = true;
        } else {
            //SurfaceActivity is already running, so we need another session
            Whisperer.getInstance().getHandler(SurfaceActivity.class.getName()).obtainMessage(42,wfdsession).sendToTarget();
        }
    }

    public IWfdSession getCurrentSession() {
        if (mSessionStack.empty()) {
            return null;
        } else {
            return mSessionStack.peek();
        }
    }

    private IWfdSession createNewSessionInstance() {
        Log.v(TAG, "createNewSessionInstance");
        try {
            IWfdSession session = IWfdSession.Stub.asInterface(ServiceUtil.getInstance().getWiFiDisplaySession());
            //set local device type
            int setDeviceTypeRet = session.setDeviceType(whisperer.getLocalDeviceType().getCode());
            if (setDeviceTypeRet == 0) {
                Log.d(TAG, "mWfdService.setDeviceType called.");
            } else {
                Log.d(TAG, "mWfdService.setDeviceType failed with error code: "
                        + setDeviceTypeRet);
            }
            return session;
        } catch (RemoteException e) {
            Log.e(TAG, "WfdService init() failed", e);
            return null;
        }
    }
}
