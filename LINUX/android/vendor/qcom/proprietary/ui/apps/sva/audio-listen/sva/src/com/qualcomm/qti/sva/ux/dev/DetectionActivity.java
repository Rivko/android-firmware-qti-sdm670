/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.dev;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Timer;
import java.util.TimerTask;

import android.Manifest.permission;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.listen.ListenTypes;
import com.qualcomm.listen.ListenTypes.VWUKeywordConfLevel;
import com.qualcomm.listen.ListenTypes.VWUUserKeywordPairConfLevel;
import com.qualcomm.listen.ListenTypes.VoiceWakeupDetectionDataV2;
import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.Global.DetectionContainer;
import com.qualcomm.qti.sva.Keyphrase;
import com.qualcomm.qti.sva.MessageType;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.SoundModel;
import com.qualcomm.qti.sva.User;


public class DetectionActivity extends Activity {
    private final static String TAG = "ListenLog.DetectionActivity";
    private final static int MSG_RESET_RESULT_VALUES = 1;
    private final static int SHOW_DETECTION_RESULT_LENGTH = 1500;
    public final static String EXTRA_AUTOSTART = "extra.DetectionActivity.autoStart";
    private final static String[] REQUIRE_PERMISSIONS = {
            permission.READ_EXTERNAL_STORAGE,
            permission.WRITE_EXTERNAL_STORAGE,
            permission.READ_PHONE_STATE,
            permission.RECORD_AUDIO,
            permission.ACCESS_COARSE_LOCATION,
            permission.ACCESS_FINE_LOCATION,
    };

    private Messenger sendToServiceMessenger;
    private Timer detectedInformationTimer = null;
    private int keyphraseDetectionsCounter = 0;
    private int userDetectionsCounter = 0;
    private int voiceRequestCounter = 0;
    private int detectionSuccessCounter = 0;
    private int detectionFailureCounter = 0;
    private boolean thisActivityIsShowing = true;
    private boolean uiUpdated = false;

    private ImageView uiMic;
    private TextView uiNumSessionsSelected;
    private LinearLayout uiSoundModelLayout;
    private LinearLayout uiAdvancedDetailLayout;
    private TableLayout uiAdvancedDetailTableLayout;
    private TextView uiDetectionCount;
    private TextView uiKeyphraseDetectionCount;
    private TextView uiUserDetectionCount;
    private TextView uiVoiceRequestCount;
    private LinearLayout uiParentLayout;
    private ImageView uiResetCount;
    private Map<String, View> keywordUserViews = new HashMap<String, View>();
    private boolean serviceBind = false;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        if (!PermissionActivity.checkAndRequestPermission(this, REQUIRE_PERMISSIONS)) {

            setContentView(R.layout.activity_dev_detection);

            //TODO re-enable once transitions is working.
            /*// Get extras for intent-based automation.
            Bundle extras = this.getIntent().getExtras();
            if (null != extras) {
                if (extras.containsKey(EXTRA_AUTOSTART)) {
                    if ("true".equals(extras.getString(EXTRA_AUTOSTART))) {
                        Global.getInstance().setAutoStart(true);
                    }
                    Log.v(TAG, "onCreate: intent autoStart= " + extras.getString(EXTRA_AUTOSTART));
                }
            }*/

            // Get, save, and output version number.
            try {
                String versionNumber = this.getPackageManager().getPackageInfo(
                        this.getPackageName(), 0).versionName;
                Global.getInstance().setVersionNumber(versionNumber);
                Log.v(TAG, "onCreate: version number= " + Global.getInstance().getVersionNumber());
            } catch (NameNotFoundException e) {
                Log.v(TAG, "onCreate: error getting version number: " + e.getMessage());
            }

            // Load saved settings.
            Global.getInstance().loadSettingsFromSharedPreferences(this);

            // Set up the UI.
            initializeUserInterface();

            //Intent intent = new Intent("com.qualcomm.qti.sva.REMOTE_SERVICE");
            Intent intent = new Intent(this, com.qualcomm.qti.sva.VwuService.class);
            startService(intent);

            //intent = new Intent("com.qualcomm.listen.sva.REMOTE_SERVICE");
            //intent = new Intent(this, com.qualcomm.qti.sva.VwuService.class);
            bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");
        thisActivityIsShowing = true;

        startDetectedInformationTimer();
        uiNumSessionsSelected.setText("Select A Sound Model");
        updateAdvancedDetailVisibility();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.v(TAG, "onPause");
        thisActivityIsShowing = false;
        stopDetectedInformationTimer();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        Log.v(TAG, "onWindowFocusChanged.hasFocus= " + hasFocus);
    }

    @Override
    protected void onDestroy() {
        Log.v(TAG, "onDestroy");

        if (serviceBind) {
            unregisterService();
            unbindService(mConnection);
        }

        super.onDestroy();
    }

    private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");
        uiMic = (ImageView) findViewById(R.id.home_mic);
        //TODO transitions remove when working as well as in XML
        //uiStartStopDetection = (ToggleButton)findViewById(R.id.home_start_voice_wakeup);
        uiSoundModelLayout = (LinearLayout) findViewById(R.id.layout_home_soundmodel);
        uiAdvancedDetailLayout = (LinearLayout) findViewById(R.id.home_advancedlayout);
        uiAdvancedDetailTableLayout = (TableLayout) findViewById(R.id.detection_advanceddetail_tablelayout);
        uiDetectionCount = (TextView) findViewById(R.id.detection_advanceddetail_numtotaldetections);
        uiKeyphraseDetectionCount = (TextView) findViewById(R.id.detection_advanceddetail_numkeyphrasedetections);
        uiUserDetectionCount = (TextView) findViewById(R.id.detection_advanceddetail_numuserdetections);
        uiVoiceRequestCount = (TextView) findViewById(R.id.detection_advanceddetail_numvoicerequests);
        uiParentLayout = (LinearLayout) findViewById(R.id.home_parent_layout);
        uiNumSessionsSelected = (TextView) findViewById(R.id.home_keyword);
        uiResetCount = (ImageView) findViewById(R.id.detection_resetdetections);

        // Starts the SoundModelsActivity when the user clicks the Sound Model Layout
        uiSoundModelLayout.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(getApplicationContext(), SoundModelsActivity.class);
                startActivity(intent);
            }
        });

        // Resets the number of detections shown on the screen
        uiResetCount.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resetProgressBarsAndCounters();
            }
        });
        uiResetCount.setVisibility(View.INVISIBLE);

        updateAdvancedDetailVisibility();

        detectionSuccessCounter = 0;
        detectionFailureCounter = 0;
        keyphraseDetectionsCounter = 0;
        userDetectionsCounter = 0;
        voiceRequestCounter = 0;
    }

    private void updateAdvancedDetailVisibility() {
        boolean isVisibile = Global.getInstance().getSettingShowAdvancedDetail();
        Log.v(TAG, "updateAdvancedDetailVisibility: advancedDetail isVisible= " + isVisibile);
        uiAdvancedDetailLayout.setVisibility(isVisibile ? View.VISIBLE : View.INVISIBLE);
        uiParentLayout.setBackgroundColor(
                isVisibile ? getResources().getColor(R.color.bg_home) : getResources().getColor(R.color.bg_content));
    }

    // Sets up menu options
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.v(TAG, "onCreateOptionsMenu");
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.detection, menu);
        return true;
    }

    // Sets up menu options
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.v(TAG, "onOptionsItemSelected");
        Intent intent = null;
        switch (item.getItemId()) {

            /*case R.id.detection_menu_voicerequests:
                intent = new Intent(this, VoiceRequestsActivity.class);
                startActivity(intent);
                return true;*/

            case R.id.detection_menu_settings:
                intent = new Intent(this, SettingsActivity.class);
                startActivity(intent);
                return true;

            case R.id.detection_menu_version:
                openAlertDialog("Version Number", Global.getInstance().getVersionNumber());
                return true;

            // For debugging purposes.
            case R.id.detection_menu_testone:
                sendReply(MessageType.MSG_TEST_ONE, null);
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    // Creates a callback method for once the Service is connected
    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.v(TAG, "onServiceConnected");
            serviceBind = true;
            sendToServiceMessenger = new Messenger(service);

            registerService();

            Log.v(TAG, "onServiceConnected: Global.getEnableListen()=  " + Global.getInstance().getSettingEnableListen()
                    + ", Global.getEnableVoiceWakeup= " + Global.getInstance().getSettingEnableVoiceWakeup());
            if (Global.getInstance().getSettingEnableListen()) {
                sendDataReply(MessageType.MSG_LISTEN_SET_PARAM, MessageType.MSG_ENABLE, null);
            }
            if (Global.getInstance().getSettingEnableVoiceWakeup()) {
                sendDataReply(MessageType.MSG_VOICEWAKEUP_SET_PARAM, MessageType.MSG_ENABLE, null);
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.v(TAG, "onServiceDisconnected");
            serviceBind = false;
            unregisterService();
        }
    };

    private void registerService() {
        Log.v(TAG, "registerService");
        Message msg = Message.obtain(null, MessageType.MSG_REGISTER_CLIENT);
        msg.replyTo = mMessenger;
        sendToService(msg);
    }

    private void unregisterService() {
        Log.v(TAG, "unregisterService");
        Message msg = Message.obtain(null, MessageType.MSG_UNREGISTER_CLIENT_ALL);
        msg.replyTo = mMessenger;
        sendToService(msg);
        sendToServiceMessenger = null;
    }

    // Sends messages to the Service
    private void sendReply(int what, Object obj) {
        Log.v(TAG, "sendReply");
        if (null == sendToServiceMessenger) {
            Log.v(TAG, "sendReply: sendToServiceMessenger is null right now- returning");
            return;
        }

        Message msg = Message.obtain(null, what, obj);
        msg.replyTo = mMessenger;
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    // Sends message to the Service with one or two ints as data
    void sendDataReply(int what, int msgArg1, Integer msgArg2) {
        Log.v(TAG, "sendMessageDataAll");
        if (null == sendToServiceMessenger) {
            return;
        }

        Message msg = Message.obtain(null, what);
        msg.arg1 = msgArg1;
        if (null != msgArg2) {
            msg.arg2 = msgArg2.intValue();
        }
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    //TODO transitions remove when working
    /*private void disableFeatures() {
        Log.v(TAG, "disableFeatures");
        sendDataReply(MessageType.MSG_LISTEN_SET_PARAM, MessageType.MSG_DISABLE, null);
        sendDataReply(MessageType.MSG_VOICEWAKEUP_SET_PARAM, MessageType.MSG_DISABLE, null);
    }

    private void closeSession() {
        Message msg = Message.obtain(null, MessageType.MSG_CLOSE_VWUSESSION);
        sendToService(msg);
    }*/

    // Sends a message to the service without any associated data
    private void sendToService(Message msg) {
        Log.v(TAG, "sendToService");
        try {
            sendToServiceMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // Handles messages coming from the Service
    /**
     * The msg handler.
     */
    private final Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            Log.v(TAG, "handleMessage");
            switch (msg.what) {
                case MessageType.MSG_ATTACH_MODULE:
                    Log.v(TAG, "MSG_ATTACH_MODULE");
                    if (msg.arg1 != Global.SUCCESS) {
                        openAlertDialog(null, "Attach Module Failed");
                    }
                    break;

                case MessageType.MSG_LOAD_SM:
                    Log.v(TAG, "MSG_LOAD_SM: msg.arg1= " + msg.arg1);
                    if (msg.arg1 != Global.SUCCESS) {
                        openAlertDialog(null, "Start Recognition Failed");
                    } else {
                        resetProgressBarsAndCounters();
                        updateAdvancedDetailWithQueryInfo();
                        uiUpdated = true;
                    }
                    break;

                case MessageType.MSG_START_RECOGNITION:
                    Log.v(TAG, "MSG_START_RECOGNITION: msg.arg1= " + msg.arg1);
                    if (msg.arg1 != Global.SUCCESS) {
                        openAlertDialog(null, "Start Recognition Failed");
                    } else {
                        uiMic.setImageResource(R.drawable.mic_on);
                        uiUpdated = true;
                    }
                    break;

                case MessageType.MSG_DETECT_SUCCEEDED:
                    Log.v(TAG, "MSG_DETECT_SUCCEEDED");
                    updateAdvancedDetailWithVwuDetectionData();
                    break;

                case MessageType.MSG_DETECT_FAILED:
                    Log.v(TAG, "MSG_DETECT_FAILED");
                    updateAdvancedDetailWithVwuDetectionData();
                    break;

                case MessageType.MSG_STOP_RECOGNITION:
                    Log.v(TAG, "MSG_STOP_RECOGNITION: msg.arg1= " + msg.arg1);
                    if (msg.arg1 != Global.SUCCESS) {
                        openAlertDialog(null, "Stop Recognition Failed");
                    } else if (msg.arg2 == 0) {
                        uiMic.setImageResource(R.drawable.mic_off);
                        uiUpdated = true;
                    }
                    break;

                case MessageType.MSG_UNLOAD_SM:
                    Log.v(TAG, "MSG_UNLOAD_SM: msg.arg1= " + msg.arg1);
                    if (msg.arg1 != Global.SUCCESS) {
                        openAlertDialog(null, "Start Recognition Failed");
                    } else {
                        updateAdvancedDetailWithQueryInfo();
                        uiUpdated = true;
                    }
                    break;

                //TODO transitions remove this along with settings options
                case MessageType.MSG_LISTEN_ENABLED:
                    if (msg.arg1 == MessageType.MSG_LISTEN_GET_PARAM) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_ENABLED msg received- " +
                                "get param returned listen is enabled");
                    } else if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_ENABLED msg received with status= " + msg.arg1);
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_ENABLED msg received with status= " + msg.arg1);
                    }
                    break;

                //TODO transitions remove this along with settings options
                case MessageType.MSG_VOICEWAKEUP_ENABLED:
                    if (msg.arg1 == MessageType.MSG_VOICEWAKEUP_GET_PARAM) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_ENABLED msg received- " +
                                "get param returned voicewakeup is enabled");
                    } else if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_ENABLED msg received with status= " + msg.arg1);
                        //TODO transitions re-enable when working
                        /*if (Global.getInstance().getAutoStart()) {
                            registerSoundModel();
                            Global.getInstance().setAutoStart(false);
                        }*/
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_ENABLED msg received with status= " + msg.arg1);
                    }
                    break;

                case MessageType.MSG_LISTEN_STOPPED:
                    Log.v(TAG, "handleMessage: MSG_LISTEN_STOPPED msg received");
                    uiMic.setImageResource(R.drawable.mic_off);
                    uiUpdated = false;
                    break;

                case MessageType.MSG_LISTEN_RUNNING:
                    Log.v(TAG, "handleMessage: MSG_LISTEN_RUNNING msg received");
                    if (false == uiUpdated) {
                        uiMic.setImageResource(R.drawable.mic_on);
                        updateAdvancedDetailWithQueryInfo();
                        uiUpdated = true;
                    } else {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_RUNNING- uiUpdated= " + uiUpdated);
                    }
                    break;

                case MessageType.MSG_NEW_VOICEREQUEST:
                    Log.v(TAG, "handleMessage: MSG_NEW_VOICEREQUEST msg received");
                    Log.v(TAG, "handleMessage: MSG_NEW_VOICEREQUEST: voiceRequestsCounter= " +
                            String.valueOf(++voiceRequestCounter));
                    // increments voice requests count
                    uiVoiceRequestCount.setText(String.valueOf(voiceRequestCounter));
                    break;

                case MessageType.MSG_TEST_ONE:
                    Log.v(TAG, "handleMessage: MSG_TEST_ONE msg received");
                    if (msg.arg1 == Global.SUCCESS) {
                        Toast.makeText(getApplicationContext(), "Get params from XML",
                                Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(getApplicationContext(), "Do not get params from XML",
                                Toast.LENGTH_SHORT).show();
                    }

                    break;

                default:
                    Log.v(TAG, "handleMessage: no such case: " + msg.what);
                    break;
            }
        }
    };
    private final Messenger mMessenger = new Messenger(mHandler);

    private void openAlertDialog(String title, String message) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        if (null == title) {
            title = getResources().getString(R.string.dev_app_name);
        }
        Log.v(TAG, "openAlertMessage: title= " + title);
        Log.v(TAG, "openAlertMessage: message= " + message);
        builder.setTitle(title)
                .setMessage(message)
                .setCancelable(true)
                .setNegativeButton(R.string.dialog_cancel, null);

        if (false == ((Activity) DetectionActivity.this).isFinishing()) {
            builder.show();
        }
    }


    /*** Advanced Detail Functions ***/

    private void updateAdvancedDetailWithQueryInfo() {
        Log.v(TAG, "updateAdvancedDetailWithQueryInfo");
        uiAdvancedDetailTableLayout.removeAllViews();
        keywordUserViews.clear();
        uiResetCount.setVisibility(View.VISIBLE);
        ArrayList<String> loadedSmNames = Global.getInstance().getSmRepo().getLoadedSmNames();
        Log.v(TAG, "updateAdvancedDetailWithQueryInfo: loadedSmNames.size()= " +
                loadedSmNames.size());
        for (int i = 0; i < loadedSmNames.size(); i++) {
            if (null == loadedSmNames.get(i)) {
                continue;
            }
            SoundModel soundModel = Global.getInstance().getSmRepo().getSmByName(
                    loadedSmNames.get(i));
            if (null == soundModel) {
                Log.e(TAG, "updateAdvancedDetailWithQueryInfo: no SMs for session= " + i);
                return;
            }
            Log.v(TAG, "updateAdvancedDetailWithQueryInfo: soundModel.getName()= "
                    + soundModel.getName() + " with # keywords= " + soundModel.getKeyphrases().size());

            View sessionSmView = getLayoutInflater().inflate(
                    R.layout.detection_advanceddetail_sessionsm, null);
            TextView uiSessionSm = (TextView) sessionSmView.findViewById(
                    R.id.detection_advanceddetail_sessionsm);
            uiSessionSm.setText("Session " + String.valueOf(i + 1) + ": " + soundModel.getPrettyName());
            uiSessionSm.setTypeface(null, Typeface.BOLD);
            uiAdvancedDetailTableLayout.addView(sessionSmView);

            for (Keyphrase keyPhrase : soundModel.getKeyphrases()) {
                View keywordView = getLayoutInflater().inflate(
                        R.layout.detection_advanceddetail_keyworduser, null);
                TextView uiKeyword = (TextView) keywordView.findViewById(
                        R.id.detection_advanceddetail_keyworduser);
                uiKeyword.setText(keyPhrase.getName());
                TableRow.LayoutParams layoutParams = new TableRow.LayoutParams(
                        TableRow.LayoutParams.WRAP_CONTENT, TableRow.LayoutParams.WRAP_CONTENT);
                layoutParams.setMargins(10, 0, 0, 0);
                uiKeyword.setLayoutParams(layoutParams);
                ProgressBar uiConfidenceLevelBar = (ProgressBar) keywordView.findViewById(
                        R.id.detection_confidencelevel_bar);
                uiConfidenceLevelBar.setProgressDrawable(getResources().getDrawable(
                        R.drawable.progress_score_bad));
                uiConfidenceLevelBar.setProgress(0);
                uiAdvancedDetailTableLayout.addView(keywordView);
                String keyphraseViewKey = generateViewKey(i, keyPhrase.getName(), null);
                Log.v(TAG, "updateAdvancedDetailWithQueryInfo: keyphraseViewKey= " + keyphraseViewKey);
                keywordUserViews.put(keyphraseViewKey, keywordView);
                for (User user : keyPhrase.getUsers()) {
                    View userView = getLayoutInflater().inflate(
                            R.layout.detection_advanceddetail_keyworduser, null);
                    TextView uiUser = (TextView) userView.findViewById(
                            R.id.detection_advanceddetail_keyworduser);
                    uiUser.setText(user.getName());
                    layoutParams = new TableRow.LayoutParams(
                            TableRow.LayoutParams.WRAP_CONTENT, TableRow.LayoutParams.WRAP_CONTENT);
                    layoutParams.setMargins(20, 0, 0, 0);
                    uiUser.setLayoutParams(layoutParams);
                    uiConfidenceLevelBar = (ProgressBar) userView.findViewById(
                            R.id.detection_confidencelevel_bar);
                    uiConfidenceLevelBar.setProgressDrawable(getResources().getDrawable(
                            R.drawable.progress_score_bad));
                    uiConfidenceLevelBar.setProgress(0);
                    uiAdvancedDetailTableLayout.addView(userView);
                    String userViewKey = generateViewKey(i, keyPhrase.getName(),
                            user.getName());
                    Log.v(TAG, "updateAdvancedDetailWithQueryInfo: userViewKey= " + userViewKey);
                    keywordUserViews.put(userViewKey, userView);
                }
            }
        }
    }

    private void updateAdvancedDetailWithVwuDetectionData() {
        startDetectedInformationTimer();

        // output detection data
        DetectionContainer detectionContainer = Global.getInstance().getDetectionContainer();
        if (null == detectionContainer) {
            openAlertDialog("Error", "Received event but no detection data");
        }
        int sessionNum = detectionContainer.sessionNum;
        String smName = detectionContainer.smName;
        VoiceWakeupDetectionDataV2 vwuDetectionData = detectionContainer.vwuDetectionData;
        boolean detectSuccessEvent = (ListenTypes.STATUS_SUCCESS == vwuDetectionData.status);
        Log.v(TAG, "   vwuDetectionData detection status= " + vwuDetectionData.status);
        Log.v(TAG, "   vwuDetectionData sessionNum= " + sessionNum);
        Log.v(TAG, "   vwuDetectionData SM name= " + smName);
        if (detectSuccessEvent) {
            Log.i(TAG, "updateAdvancedDetail: detectionSuccessCounter= " + ++detectionSuccessCounter);
        } else {
            Log.i(TAG, "updateAdvancedDetail: detectionFailureCounter= " + ++detectionFailureCounter);
        }

        // keyword conf levels
        Log.v(TAG, "vwuDetectionData.nonzeroKWConfLevels.length= " +
                vwuDetectionData.nonzeroKWConfLevels.length);
        for (VWUKeywordConfLevel kwConfLevel : vwuDetectionData.nonzeroKWConfLevels) {
            updateAdvancedDetailUI(detectSuccessEvent, sessionNum, kwConfLevel.keyword, null,
                    kwConfLevel.confLevel);
        }
        // user conf levels
        Log.v(TAG, "vwuDetectionData.nonzeroUserKWPairConfLevels.length= " +
                vwuDetectionData.nonzeroUserKWPairConfLevels.length);
        for (VWUUserKeywordPairConfLevel pairConfLevel : vwuDetectionData.nonzeroUserKWPairConfLevels) {
            updateAdvancedDetailUI(detectSuccessEvent, sessionNum, pairConfLevel.keyword,
                    pairConfLevel.user, pairConfLevel.confLevel);
        }

        Log.v(TAG, "updateAdvancedDetailWithVwuDetectionData: detectionSuccessCounter + " +
                "detectionFailureCounter= " +
                String.valueOf(detectionSuccessCounter + detectionFailureCounter));
        // set total detections count
        uiDetectionCount.setText(String.valueOf(detectionSuccessCounter + detectionFailureCounter));
        Log.v(TAG, "updateAdvancedDetailWithVwuDetectionData: keyphraseDetectionsCounter= " +
                String.valueOf(keyphraseDetectionsCounter));
        // set keyword detections count
        uiKeyphraseDetectionCount.setText(String.valueOf(keyphraseDetectionsCounter));
        Log.v(TAG, "updateAdvancedDetailWithVwuDetectionData: userDetectionsCounter= " +
                String.valueOf(userDetectionsCounter));
        // set user detections count
        uiUserDetectionCount.setText(String.valueOf(userDetectionsCounter));
    }

    private void updateAdvancedDetailUI(boolean detectSuccessEvent, int sessionNum,
                                        String keyword, String user, int confLevel) {
        Log.v(TAG, "updateAdvancedDetailUI detectSuccess= " + detectSuccessEvent);
        Log.v(TAG, "updateAdvancedDetailUI sessionNum= " + sessionNum);
        Log.v(TAG, "updateAdvancedDetailUI kwConfLevel.keyword= " + keyword);
        Log.v(TAG, "updateAdvancedDetailUI kwConfLevel.user= " + user);
        Log.v(TAG, "updateAdvancedDetailUI kwConfLevel.confLevel= " + confLevel);
        Drawable progressBarFill;
        if (detectSuccessEvent) {
            // to only count detections once
            if (null == user) {
                Log.i(TAG, "updateAdvancedDetailUI: keyphraseDetectionsCounter= " + ++keyphraseDetectionsCounter);
            } else {
                Log.i(TAG, "updateAdvancedDetailUI: userDetectionsCounter= " + ++userDetectionsCounter);
            }
            progressBarFill = getResources().getDrawable(R.drawable.progress_score_good);
        } else {
            progressBarFill = getResources().getDrawable(R.drawable.progress_score_bad);
        }

        /*for (View keyPhraseUserView : keywordUserViews) {
            if (keyPhraseUserView.getTag().equals(generateViewTag(keyword, user))) {*/
        String viewKey = generateViewKey(sessionNum, keyword, user);
        View keyPhraseUserView = keywordUserViews.get(viewKey);

        //if do not get according view, should return directly
        if (keyPhraseUserView == null) {
            Log.e(TAG, "updateAdvancedDetailUI: keyPhraseUserView is null, return");
            return;
        }

        // num detections
        TextView uiNumDetections = (TextView) keyPhraseUserView.
                findViewById(R.id.detection_num_detections);
        //int numDetectionsOld = Integer.getInteger(uiNumDetections.getText().toString());
        String uiDetectionsText = uiNumDetections.getText().toString();
        if (null == uiDetectionsText) {
            Log.v(TAG, "updateAdvancedDetailUI: uiNumDetections is null");
        } else {
            Log.v(TAG, "updateAdvancedDetailUI: uiNumDetections= " + uiDetectionsText);
        }
        int numDetectionsOld = Integer.parseInt(uiNumDetections.getText().toString());
        uiNumDetections.setText(Integer.toString(++numDetectionsOld));
        /*if (null == user) {
            uiNumDetections.setText(String.valueOf(++keyphraseDetectionsCounter));
        } else {
            uiNumDetections.setText(String.valueOf(++userDetectionsCounter));
        }*/
        // progress bar
        ProgressBar uiConfidenceLevelBar = (ProgressBar) keyPhraseUserView.findViewById(
                R.id.detection_confidencelevel_bar);
        uiConfidenceLevelBar.setProgressDrawable(progressBarFill);
        uiConfidenceLevelBar.setProgress(confLevel);
        // confidence level
        TextView uiConfidenceLevel = (TextView) keyPhraseUserView.
                findViewById(R.id.detection_confidencelevel);
        uiConfidenceLevel.setText(String.valueOf(confLevel));

        Log.v(TAG, "View for keyword: " + keyword + ", user: " + user +
                " found. Update with confidence level: " + confLevel);
    }

    public String generateViewKey(int inSessionNum, String inKeywordName, String inUserName) {
        return inSessionNum + ":" + inKeywordName + ":" + inUserName;
    }

    // Resets keyword and user detection bars after SHOW_DETECTION_RESULT_LENGTH milliseconds
    private void startDetectedInformationTimer() {
        Log.v(TAG, "startDetectedInformationTimer");
        stopDetectedInformationTimer();


        Log.v(TAG, "startDetectedInformationTimer: starting new timer");
        detectedInformationTimer = new Timer();
        TimerTask timerTask = new TimerTask() {
            @Override
            public void run() {
                uiHandler.sendEmptyMessage(MSG_RESET_RESULT_VALUES);
            }
        };
        detectedInformationTimer.schedule(timerTask, SHOW_DETECTION_RESULT_LENGTH);
    }

    private void stopDetectedInformationTimer() {
        Log.v(TAG, "stopDetectedInformationTimer: stopping previous timer");
        if (null != detectedInformationTimer) {
            detectedInformationTimer.cancel();
            detectedInformationTimer = null;
        }
    }

    private Handler uiHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Log.v(TAG, "uiHandler");
            if (MSG_RESET_RESULT_VALUES == msg.what) {
                Log.v(TAG, "uiHandler: MSG_RESET_RESULT_VALUES");
                /*for (View keyPhaseUserView : keywordUserViews) {
                    ProgressBar uiConfidenceLevelBar = (ProgressBar)keyPhaseUserView.findViewById(
                            R.id.detection_confidencelevel_bar);
                    uiConfidenceLevelBar.setProgress(0);
                }*/
                View keyPhraseUserView;
                for (Entry<String, View> mapEntry : keywordUserViews.entrySet()) {
                    keyPhraseUserView = mapEntry.getValue();
                    ProgressBar uiConfidenceLevelBar = (ProgressBar) keyPhraseUserView.findViewById(
                            R.id.detection_confidencelevel_bar);
                    uiConfidenceLevelBar.setProgress(0);
                }
            }
        }

        ;
    };

    private void resetProgressBarsAndCounters() {
        Log.v(TAG, "resetProgressBarsAndCounters: keyword and user detections reset");

        detectionSuccessCounter = 0;
        detectionFailureCounter = 0;
        keyphraseDetectionsCounter = 0;
        userDetectionsCounter = 0;
        voiceRequestCounter = 0;
        // set total detection count
        uiDetectionCount.setText(String.valueOf(detectionSuccessCounter));
        // set keyword detection count
        uiKeyphraseDetectionCount.setText(String.valueOf(keyphraseDetectionsCounter));
        // set user detection count
        uiUserDetectionCount.setText(String.valueOf(userDetectionsCounter));
        // set voice requests count
        uiVoiceRequestCount.setText(String.valueOf(voiceRequestCounter));
        View keyPhraseUserView;
        for (Entry<String, View> mapEntry : keywordUserViews.entrySet()) {
            keyPhraseUserView = mapEntry.getValue();
            TextView uiNumKeyPhraseDetections = (TextView) keyPhraseUserView.
                    findViewById(R.id.detection_num_detections);
            uiNumKeyPhraseDetections.setText("0");

            ProgressBar uiConfidenceLevelBar = (ProgressBar) keyPhraseUserView.findViewById(
                    R.id.detection_confidencelevel_bar);
            uiConfidenceLevelBar.setProgressDrawable(getResources().getDrawable(R.drawable.progress_score_bad));
            uiConfidenceLevelBar.setProgress(0);

            TextView uiKeyPhraseConfLevel = (TextView) keyPhraseUserView.
                    findViewById(R.id.detection_confidencelevel);
            uiKeyPhraseConfLevel.setText("0");
        }
    }

    public void playLastVoiceRequest(View view) {
        Log.v(TAG, "playLastVoiceRequest");
        String lastVoiceRequestFilePath = Global.getInstance().getLastVoiceRequestFilePath();
        if (null == lastVoiceRequestFilePath) {
            Log.v(TAG, "playLastVoiceRequest: null == lastVoiceRequestFilePath");
            Toast.makeText(getApplicationContext(), "No Voice Requests Yet", Toast.LENGTH_LONG).show();
            return;
        }
        Log.v(TAG, "playLastVoiceRequest: lastVoiceRequestFilePath= " + lastVoiceRequestFilePath);
        Uri lastVoiceRequestUri = Uri.fromFile(new File(lastVoiceRequestFilePath));
        MediaPlayer mediaPlayer = MediaPlayer.create(getApplicationContext(), lastVoiceRequestUri);
        mediaPlayer.start();
    }
}
