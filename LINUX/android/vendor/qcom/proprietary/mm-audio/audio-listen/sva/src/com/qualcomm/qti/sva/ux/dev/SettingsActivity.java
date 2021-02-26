/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.MessageType;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.VwuService;


public class SettingsActivity extends Activity {
	private final static String TAG = "ListenLog.Settings";
	public final static String EXTRA_SETUSERVERIFICATION = "extra.SettingsActivity.setUserVerification";
	public final static String EXTRA_SETVOICEREQUESTS = "extra.SettingsActivity.setVoiceRequests";
	public final static String EXTRA_SETDETECTIONTONE = "extra.SettingsActivity.setDetectionTone";

    private Messenger sendToServiceMessenger;

    private Spinner uiKeyPhraseThreshold;
    private Spinner uiUserThreshold;
    private Spinner uiTrainingThreshold;
	private Switch uiListen;
    private Switch uiVoicewakeup;
    private Switch uiUserVerification;
    private Switch uiFailureFeedback;
    private Switch uiDetectionTone;
    private Switch uiVoiceRequests;
    private Spinner uiVoiceRequestLength;
	private Switch uiAdvancedDetail;
	private RelativeLayout uiLayoutListen;
    private RelativeLayout uiLayoutVoicewakeup;
    private RelativeLayout uiLayoutUserVerification;
    private RelativeLayout uiLayoutFailureFeedback;
	private RelativeLayout uiLayoutTone;
	private RelativeLayout uiLayoutVoiceRequests;
	private RelativeLayout uiLayoutDetail;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    setContentView(R.layout.activity_dev_settings);

	    // Bind to SVA app service.
	    Intent intent = new Intent(this, VwuService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);

        Bundle extras = this.getIntent().getExtras();
        if (null != extras) {
            if (extras.containsKey(EXTRA_SETUSERVERIFICATION)) {
                Log.v(TAG, "onCreate: intent extra setUserVerification= " + extras.getString(
                        EXTRA_SETUSERVERIFICATION));
                String setUserVerificationFromExtra = extras.getString(EXTRA_SETUSERVERIFICATION);
                if (setUserVerificationFromExtra.toLowerCase().equals("true")) {
                    Log.v(TAG, "onCreate: intent extra setUserVerification setting to true");
                    Global.getInstance().setSettingUserVerification(true);
                } else if (setUserVerificationFromExtra.toLowerCase().equals("false")) {
                    Log.v(TAG, "onCreate: intent extra setUserVerification setting to false");
                    Global.getInstance().setSettingUserVerification(false);
                } else {
                    Log.e(TAG, "onCreate: intent extra setUserVerification cannot recognize " +
                            "parameter= " + setUserVerificationFromExtra);
                }
            }
            if (extras.containsKey(EXTRA_SETVOICEREQUESTS)) {
                Log.v(TAG, "onCreate: intent EXTRA_SETVOICEREQUESTS= " + extras.getString(
                        EXTRA_SETVOICEREQUESTS));
                String setVoiceRequestsFromExtra = extras.getString(EXTRA_SETVOICEREQUESTS);
                if (setVoiceRequestsFromExtra.toLowerCase().equals("true")) {
                    Log.v(TAG, "onCreate: intent EXTRA_SETVOICEREQUESTS setting to true");
                    Global.getInstance().setSettingVoiceRequestsEnabled(true);
                } else if (setVoiceRequestsFromExtra.toLowerCase().equals("false")) {
                    Log.v(TAG, "onCreate: intent EXTRA_SETVOICEREQUESTS setting to false");
                    Global.getInstance().setSettingVoiceRequestsEnabled(false);
                } else {
                    Log.e(TAG, "onCreate: intent EXTRA_SETVOICEREQUESTS cannot recognize " +
                            "parameter= " + setVoiceRequestsFromExtra);
                }
            }
            if (extras.containsKey(EXTRA_SETDETECTIONTONE)) {
                Log.v(TAG, "onCreate: intent EXTRA_SETDETECTIONTONE= " + extras.getString(
                        EXTRA_SETDETECTIONTONE));
                String setDetectionToneFromExtra = extras.getString(EXTRA_SETDETECTIONTONE);
                if (setDetectionToneFromExtra.toLowerCase().equals("true")) {
                    Log.v(TAG, "onCreate: intent EXTRA_SETDETECTIONTONE setting to true");
                    Global.getInstance().setSettingDetectionTone(true);
                } else if (setDetectionToneFromExtra.toLowerCase().equals("false")) {
                    Log.v(TAG, "onCreate: intent EXTRA_SETDETECTIONTONE setting to false");
                    Global.getInstance().setSettingDetectionTone(false);
                } else {
                    Log.e(TAG, "onCreate: intent EXTRA_SETDETECTIONTONE cannot recognize " +
                            "parameter= " + setDetectionToneFromExtra);
                }
            }
        }
	}

	@Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();

        Global.getInstance().setSettingKeyPhraseThreshold(uiKeyPhraseThreshold.getSelectedItemPosition()+1);
        Global.getInstance().setSettingUserConfidenceLevel(uiUserThreshold.getSelectedItemPosition()+1);
        Global.getInstance().setSettingTrainingConfidenceLevel(uiTrainingThreshold.getSelectedItemPosition()+1);
        Global.getInstance().setSettingEnableListen(uiListen.isChecked());
        Global.getInstance().setSettingEnableVoiceWakeup(uiVoicewakeup.isChecked());
        Global.getInstance().setSettingUserVerification(uiUserVerification.isChecked());
        Global.getInstance().setSettingFailureFeeback(uiFailureFeedback.isChecked());
        Global.getInstance().setSettingDetectionTone(uiDetectionTone.isChecked());
        Global.getInstance().setSettingVoiceRequestsEnabled(uiVoiceRequests.isChecked());
        Global.getInstance().setSettingVoiceRequestLength(
                ((double)uiVoiceRequestLength.getSelectedItemPosition() + 1)/2);
        Global.getInstance().setSettingShowAdvancedDetail(uiAdvancedDetail.isChecked());

        Global.getInstance().saveSettingsToSharedPreferences(getApplicationContext());
    }

	@Override
    protected void onDestroy() {
        super.onDestroy();
        sendReply(MessageType.MSG_UNREGISTER_CLIENT, null);
        if (null != sendToServiceMessenger) {
            unbindService(mConnection);
        }
    }

	// Connects to the Service
	private ServiceConnection mConnection = new ServiceConnection() {
        // Registers as a client to receive messages.
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.v(TAG, "onServiceConnected");
            sendToServiceMessenger = new Messenger(service);
            // Register clients
            sendReply(MessageType.MSG_REGISTER_CLIENT, null);
            Log.v(TAG, "connected service");
            initializeUserInterface();
        }

        // Unregisters as a client to receive messages.
        public void onServiceDisconnected(ComponentName name) {
            Log.v(TAG, "onServiceDisconnected");
            sendReply(MessageType.MSG_UNREGISTER_CLIENT, null);
            sendToServiceMessenger = null;
            Log.v(TAG, "disconnected service");
        }
    };

    // Handles incoming messages from the Service
    public Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            Log.v(TAG, "handleMessage");
            switch(msg.what) {
                case MessageType.MSG_LISTEN_ENABLED:
                    if (msg.arg1 == MessageType.MSG_LISTEN_GET_PARAM) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_ENABLED msg received- " +
                        		    "get param returned listen is enabled");
                        uiListen.setChecked(true);
                    } else if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_ENABLED msg received with status= " + msg.arg1);
                        showFeatureToast("Listen enable ", msg.arg1);
                        uiListen.setChecked(true);
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_ENABLED msg received with status= " + msg.arg1);
                        showFeatureToast("Listen enable ", msg.arg1);
                    }
                    break;
                case MessageType.MSG_LISTEN_DISABLED:
                    if (msg.arg1 == MessageType.MSG_LISTEN_GET_PARAM) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_DISABLED msg received- " +
                                    "get param returned listen is disabled");
                        uiListen.setChecked(false);
                    } else if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_DISABLED msg received with status= " + msg.arg1);
                        showFeatureToast("Listen disable ", msg.arg1);
                        uiListen.setChecked(false);
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_LISTEN_DISABLED msg received with status= " + msg.arg1);
                        showFeatureToast("Listen disable ", msg.arg1);
                    }
                    break;
                case MessageType.MSG_VOICEWAKEUP_ENABLED:
                    if (msg.arg1 == MessageType.MSG_VOICEWAKEUP_GET_PARAM) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_ENABLED msg received- " +
                                    "get param returned voicewakeup is enabled");
                        uiVoicewakeup.setChecked(true);
                    } else if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_ENABLED msg received with status= " + msg.arg1);
                        showFeatureToast("VoiceWakeup enable ", msg.arg1);
                        uiVoicewakeup.setChecked(true);
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_ENABLED msg received with status= " + msg.arg1);
                        showFeatureToast("VoiceWakeup enable ", msg.arg1);
                    }
                    break;
                case MessageType.MSG_VOICEWAKEUP_DISABLED:
                    if (msg.arg1 == MessageType.MSG_LISTEN_GET_PARAM) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_DISABLED msg received- " +
                                    "get param returned listen is disabled");
                        uiVoicewakeup.setChecked(false);
                    } else if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_DISABLED msg received with status= " + msg.arg1);
                        showFeatureToast("VoiceWakeup disable ", msg.arg1);
                        uiVoicewakeup.setChecked(false);
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_VOICEWAKEUP_DISABLED msg received with status= " + msg.arg1);
                        showFeatureToast("VoiceWakeup disable ", msg.arg1);
                    }
                    break;

                default:
                   Log.v(TAG, "handleMessage: no such case: " + msg.what);
            }
        }

        private void showFeatureToast(String inFeature, int inStatus) {
            if (Global.SUCCESS == inStatus) {
                Toast.makeText(getApplicationContext(), inFeature + "succeeded", Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(getApplicationContext(), inFeature + "failed", Toast.LENGTH_LONG).show();
            }
        }
    };

    private final Messenger mMessenger = new Messenger(mHandler);

    // Sends messages to the Service
    private void sendReply(int what, Object obj) {
        Log.v(TAG, "sendReply");
        if (null == sendToServiceMessenger) {
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

    // Sends messages with data to the Service
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

	private void initializeUserInterface() {
        Log.v(TAG, "initializeUserInterface");
	    uiKeyPhraseThreshold = (Spinner)findViewById(R.id.settings_keyphraseconfidence_spinner);
	    uiUserThreshold = (Spinner)findViewById(R.id.settings_userconfidence_spinner);
	    uiTrainingThreshold = (Spinner)findViewById(R.id.settings_trainingconfidence_spinner);

	    uiListen = (Switch) findViewById(R.id.settings_listen);
        uiVoicewakeup = (Switch) findViewById(R.id.settings_voicewakeup);
        uiUserVerification = (Switch) findViewById(R.id.settings_userverification);
        uiFailureFeedback = (Switch) findViewById(R.id.settings_failurefeedback);
        uiDetectionTone = (Switch) findViewById(R.id.settings_detectiontone);
        uiVoiceRequests = (Switch)findViewById(R.id.settings_voicerequests);
        uiVoiceRequestLength = (Spinner)findViewById(R.id.settings_voicerequestlength_spinner);
		uiAdvancedDetail = (Switch)findViewById(R.id.settings_advanceddetail);

		uiLayoutListen = (RelativeLayout)findViewById(R.id.settings_layout_listen);
        uiLayoutVoicewakeup = (RelativeLayout)findViewById(R.id.settings_layout_voicewakeup);
        uiLayoutUserVerification = (RelativeLayout)findViewById(R.id.settings_layout_userverification);
        uiLayoutFailureFeedback = (RelativeLayout)findViewById(R.id.settings_layout_failurefeedback);
		uiLayoutTone = (RelativeLayout)findViewById(R.id.settings_layout_detectiontone);
		uiLayoutVoiceRequests = (RelativeLayout)findViewById(R.id.settings_layout_voicerequests);
		uiLayoutDetail = (RelativeLayout)findViewById(R.id.settings_layout_advanced);

        Log.v(TAG, "initializeUserInterface: before any spinner setup");
		// set up spinner array adapters
		ArrayAdapter[] arrayAdapters = new ArrayAdapter[3];
        for (int i=0; i<3; i++) {
            Integer[] levels = new Integer[100];
            for (int j=0; j<100; j++) {
                levels[j] = j+1;
            }
            Log.v(TAG, "initializeUserInterface: spinner after array created");
            ArrayAdapter<Integer> spinnerAdapter = new
                    ArrayAdapter<Integer>(this, android.R.layout.simple_spinner_item, levels);
            Log.v(TAG, "initializeUserInterface: spinner after array adapter created");
            spinnerAdapter.setDropDownViewResource(android.R.layout.simple_dropdown_item_1line);
            arrayAdapters[i] = spinnerAdapter;
        }
        String[] voiceRequestLengthOptions = new String[20];
        for (int i=1; i<=20; i++) {
            voiceRequestLengthOptions[i-1] = String.valueOf((double)i/2);
        }
        ArrayAdapter<String> voiceRequestArrayAdapter = new
                ArrayAdapter<String>(this, android.R.layout.simple_spinner_item,
                        voiceRequestLengthOptions);
        voiceRequestArrayAdapter.setDropDownViewResource(android.R.layout.simple_dropdown_item_1line);

        Log.v(TAG, "initializeUserInterface: spinner after array adapter for loop");
        uiKeyPhraseThreshold.setAdapter(arrayAdapters[0]);
        uiUserThreshold.setAdapter(arrayAdapters[1]);
        uiTrainingThreshold.setAdapter(arrayAdapters[2]);
        uiVoiceRequestLength.setAdapter(voiceRequestArrayAdapter);

		uiKeyPhraseThreshold.setSelection(Global.getInstance().getSettingKeyPhraseConfidenceLevel()-1);
        uiUserThreshold.setSelection(Global.getInstance().getSettingUserConfidenceLevel()-1);
        uiTrainingThreshold.setSelection(Global.getInstance().getSettingTrainingConfidenceLevel()-1);
		sendReply(MessageType.MSG_LISTEN_GET_PARAM, null);
		sendReply(MessageType.MSG_VOICEWAKEUP_GET_PARAM, null);
        uiUserVerification.setChecked(Global.getInstance().getSettingUserVerification());
		uiFailureFeedback.setChecked(Global.getInstance().getSettingFailureFeedback());
		uiDetectionTone.setChecked(Global.getInstance().getSettingDetectionTone());
		uiVoiceRequests.setChecked(Global.getInstance().getSettingVoiceRequestsEnabled());
		uiVoiceRequestLength.setSelection((int)(Global.getInstance().getSettingVoiceRequestLength()*2)-1);
		uiAdvancedDetail.setChecked(Global.getInstance().getSettingShowAdvancedDetail());

		findViewById(R.id.settings_layout_focus).requestFocus();

		uiLayoutListen.setTag(uiListen);
        uiLayoutVoicewakeup.setTag(uiVoicewakeup);
        uiLayoutUserVerification.setTag(uiUserVerification);
        uiLayoutFailureFeedback.setTag(uiFailureFeedback);
		uiLayoutTone.setTag(uiDetectionTone);
		uiLayoutVoiceRequests.setTag(uiVoiceRequests);
		uiLayoutDetail.setTag(uiAdvancedDetail);

		uiLayoutListen.setOnClickListener(onClickListener);
        uiLayoutVoicewakeup.setOnClickListener(onClickListener);
        uiLayoutUserVerification.setOnClickListener(onClickListener);
        uiLayoutFailureFeedback.setOnClickListener(onClickListener);
        uiLayoutTone.setOnClickListener(onClickListener);
        uiLayoutVoiceRequests.setOnClickListener(onClickListener);
        uiLayoutDetail.setOnClickListener(onClickListener);
	}

    // Enables/disables the feature that the user checked/unchecked
    private OnClickListener onClickListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            Switch uiSwitch = (Switch)v.getTag();
            boolean checked = !uiSwitch.isChecked();
            if (uiSwitch == uiListen){
                Log.v(TAG, "checkbox onclick for uiListen checkbox, checked= " + checked);
                if (checked) {
                    sendDataReply(MessageType.MSG_LISTEN_SET_PARAM, MessageType.MSG_ENABLE, null);
                } else {
                    sendDataReply(MessageType.MSG_LISTEN_SET_PARAM, MessageType.MSG_DISABLE, null);
                }
            } else if (uiSwitch == uiVoicewakeup) {
                Log.v(TAG, "checkbox onclick for uiVoicewakeup checkbox, checked= " + checked);
                if (checked) {
                    sendDataReply(MessageType.MSG_VOICEWAKEUP_SET_PARAM, MessageType.MSG_ENABLE, null);
                } else {
                    sendDataReply(MessageType.MSG_VOICEWAKEUP_SET_PARAM, MessageType.MSG_DISABLE, null);
                }
            } else {
                uiSwitch.setChecked(checked);
            }
        }
    };
}
