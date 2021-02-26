/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.PowerManager;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.qualcomm.qti.sva.Global;
import com.qualcomm.qti.sva.ListenAudioRecorder;
import com.qualcomm.qti.sva.MessageType;
import com.qualcomm.qti.sva.R;
import com.qualcomm.qti.sva.VwuService;


/**
 * Actions:
 * <p>
 * From UI
 * <p>
 * - when mic button is pressed, it starts training by starting speech detection for five times.
 * -audioRecord.getInstance();
 * -audioRecord.start();
 * <user speaks keyword>
 * -audioRecord.stop();
 * -audioRecord.release();
 * - when close button pressed, training is aborted and model is reloaded from file.
 * - When required number of utterances are accepted, lets user select continue or cancel this training session.
 * - When user clicks continue, updates sound model and saves it to file.
 * <p>
 * Incoming messages:
 * When speech is detected, message arrives with keyword confidence.
 * This activity makes a decision based on the value TRAINING_CONFIDENCE_LEVEL_THRESHOLD.
 * If accepted, update UI, increase counts, accumulate current utterance.
 * If rejected, notify user.
 */
public class TrainingActivity extends Activity {
    private final static String TAG = "ListenLog.TrainingActivity";
    public final static String EXTRA_NEWUSER = "extra.training.newUser";
    public final static int TRAINING_CONFIDENCE_LEVEL_THRESHOLD = 62;
    public final static int TRAINING_CONFIDENCE_LEVEL_THRESHOLD_UDK = 16;
    private final static int TRAINING_RECORDING_DURATION = 3000;
    private final static int MSG_STOP_TRAINING_RECORDING = 1;

    private RecordingCounter recordingCounter = null;
    private boolean isCurrentlyTraining = false;
    private boolean isCompletedTraining = false;
    private String userName;
    private String keyphrase;
    private boolean isUdk = false;
    private String recordingState;
    private String processingState;
    private Messenger sendToServiceMessenger;
    private ListenAudioRecorder recorder = null;
    private PowerManager.WakeLock wakeLock;

    private ImageButton uiMic;
    private ImageButton uiClose;
    private TextView uiUserName;
    private Button uiContinue;
    private TextView uiNotice;
    private TextView uiRecordingState;
    private TextView uiKeyPhrase;
    private View uiWelcomeLayout;
    private View uiRecordLayout;
    private RelativeLayout uiMicLayout;
    private View uiDisableLayout;
    private Timer trainingTimer;
    private ProgressDialog progressDialog = null;
    private ImageView[] imageViews;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dev_training);

        Intent passedIntent = getIntent();
        keyphrase = passedIntent.getExtras().getString(Global.TAG_TRAINING_KEYPHRASE);
        userName = passedIntent.getExtras().getString(Global.TAG_TRAINING_USER);
        isUdk = passedIntent.getExtras().getBoolean(Global.TAG_TRAINING_IS_UDK, false);

        if (null == userName || userName.isEmpty()) {
            Log.e(TAG, "onCreate: no user name. Calling finish().");
            finish();
        }

        Global.getInstance().removeExistingRecordingFiles();
        Global.getInstance().removeUserRecordings();
        initalizeUserInterface();

        Intent intent = new Intent(this, VwuService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);

        try {
            imageViews = new ImageView[]{
                    (ImageView) findViewById(R.id.training_record1),
                    (ImageView) findViewById(R.id.training_record2),
                    (ImageView) findViewById(R.id.training_record3),
                    (ImageView) findViewById(R.id.training_record4),
                    (ImageView) findViewById(R.id.training_record5)
            };
            recordingCounter = new RecordingCounter(this, imageViews);
        } catch (Exception e) {
            e.printStackTrace();
        }

        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "trainingWakeLock");
    }

    // Send back to SoundModelsActivity whether training completed successfully.
    @Override
    protected void onPause() {
        Log.v(TAG, "onPause");
        if (isCurrentlyTraining) {
            changeStateToNotRecording();
        }
        if (isCompletedTraining) {
            setResult(RESULT_OK);
        } else {
            setResult(RESULT_CANCELED);
        }
        super.onPause();
    }

    @Override
    public void onBackPressed() {
        Log.v(TAG, "onBackPressed");
        setResult(RESULT_CANCELED);
        super.onBackPressed();
    }

    @Override
    protected void onDestroy() {
        Log.v(TAG, "onDestroy");
        super.onDestroy();
        sendReply(MessageType.MSG_UNREGISTER_CLIENT, null);
        if (null != sendToServiceMessenger) {
            unbindService(mConnection);
        }
    }

    private void initalizeUserInterface() {
        Log.v(TAG, "initalizeUserInterface");
        uiWelcomeLayout = findViewById(R.id.layout_training_welcome);
        uiDisableLayout = findViewById(R.id.layout_training_disableview);
        uiUserName = (TextView) findViewById(R.id.training_username);
        uiRecordLayout = findViewById(R.id.layout_training_record);
        uiRecordingState = (TextView) findViewById(R.id.training_recording_state);
        uiClose = (ImageButton) findViewById(R.id.training_recording_close);
        uiKeyPhrase = (TextView) findViewById(R.id.training_keyphrase);
        uiNotice = (TextView) findViewById(R.id.training_recording_notice);
        uiMicLayout = (RelativeLayout) findViewById(R.id.training_layoutMic);
        uiMic = (ImageButton) findViewById(R.id.training_mic);
        uiContinue = (Button) findViewById(R.id.training_recording_continue);

        recordingState = getString(R.string.training_recording_recording);
        processingState = getString(R.string.training_recording_processing);

        uiMic.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(TAG, "uiMic.onClick");
                changeStateToRecording();
            }
        });

        uiClose.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(TAG, "uiClose.onClick");
                /*if (completedTraining) {
                    Log.v(TAG, "uiClose.onClick: completedTraining is true");*/
                Log.v(TAG, "uiClose.onClick");
                AlertDialog.Builder builder = new AlertDialog.Builder(TrainingActivity.this);
                builder.setTitle(R.string.training_dialog_close_title)
                        .setMessage(R.string.training_dialog_close_message)
                        .setCancelable(false)
                        .setNegativeButton(R.string.dialog_cancel, null)
                        .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                changeStateToNotRecording();
                            }
                        });

                if (false == ((Activity) TrainingActivity.this).isFinishing()) {
                    builder.show();
                }
			    /*}

			    if (completedTraining == false) {
                    Log.v(TAG, "uiClose.onClick: completedTraining is false");
			        closeRecordingWindow();
			        Log.v(TAG, "uiClose.onClick: getNumUserRecordings: "
			                + Global.getInstance().getNumUserRecordings());
			    }*/
            }

        });

        uiUserName.setText(userName);
        uiKeyPhrase.setText(keyphrase);

        uiContinue.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(TAG, "uiContinue.onClick");
                String pleaseWait = getString(R.string.home_dialog_pleasewait);
                String creatingSoundModel = getString(R.string.training_dialog_createsoundmodel);

                progressDialog = ProgressDialog.show(TrainingActivity.this, creatingSoundModel, pleaseWait);
                Log.v(TAG, "uiContinue.onClick: progress dialog started");
                String[] keyphraseUserIsudk = new String[3];
                keyphraseUserIsudk[0] = keyphrase;
                keyphraseUserIsudk[1] = userName;
                keyphraseUserIsudk[2] = (isUdk) ? "true" : "false";
                sendReply(MessageType.MSG_EXTEND_SOUNDMODEL, keyphraseUserIsudk);
            }
        });
    }

    private void changeStateToRecording() {
        Log.v(TAG, "changeStateToRecording");
        isCurrentlyTraining = true;
        //TODO figure out if I need to reset trainings here and on stopRecording.
        resetTrainings();
        setVisibleRecordingArea();
        startRecording();
        // Acquire wakeLock.
        if (false == wakeLock.isHeld()) {
            wakeLock.acquire();
            Log.v(TAG, "wakeLock.acquire() completed");
        } else {
            Log.e(TAG, "cannot acquire wakeLock, wakeLock.isHeld() is already true");
        }
    }

    private void changeStateToNotRecording() {
        isCurrentlyTraining = false;
        //TODO figure out if I need to reset trainings here and on startRecording.
        resetTrainings();
        setVisibleTrainingMainArea();
        stopRecording();
        stopRecordingTimer();
        // Release wakeLock.
        try {
            if (wakeLock.isHeld()) {
                wakeLock.release();
                Log.v(TAG, "wakeLock.release() completed");
            } else {
                Log.e(TAG, "cannot release wakeLock, wakeLock.isHeld() is false");
            }
        } catch (RuntimeException re) {
            Log.e(TAG, "wakeLock under-locked");
        }
    }

    private void resetTrainings() {
        Global.getInstance().removeExistingRecordingFiles();
        Global.getInstance().removeUserRecordings();
        try {
            recordingCounter = new RecordingCounter(TrainingActivity.this, imageViews);
        } catch (Exception e) {
            e.printStackTrace();
        }
        isCompletedTraining = false;
    }

    private void setVisibleRecordingArea() {
        Log.v(TAG, "setVisibleRecording");
        uiWelcomeLayout.setEnabled(false);
        uiRecordLayout.setVisibility(View.VISIBLE);
        uiDisableLayout.setVisibility(View.VISIBLE);
        uiRecordingState.setText(recordingState);
        uiNotice.setVisibility(View.INVISIBLE);
        uiContinue.setVisibility(View.INVISIBLE);
        uiMic.setImageResource(R.drawable.speaker_on);
        uiMic.setClickable(false);
    }

    private void setVisibleTrainingMainArea() {
        Log.v(TAG, "setVisibleTrainingMain");
        uiWelcomeLayout.setEnabled(true);
        uiRecordLayout.setVisibility(View.INVISIBLE);
        uiDisableLayout.setVisibility(View.INVISIBLE);

        uiMic.setImageResource(R.drawable.speaker_off);
        uiMic.setClickable(true);
        uiMicLayout.setVisibility(View.VISIBLE);
    }

    private Handler uiHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Log.v(TAG, "uiHandler");
            if (MSG_STOP_TRAINING_RECORDING == msg.what) {
                stopRecording();
                sendDataReply(MessageType.MSG_VERIFY_RECORDING, keyphrase, (isUdk) ? 1 : 0, null);
            }
        }

        ;
    };


    // Stops recording after TRAINING_RECORDING_DURATION milliseconds
    private void startRecordingTimer() {
        Log.v(TAG, "startTrainingTimer");
        stopRecordingTimer();

        trainingTimer = new Timer();
        trainingTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                uiHandler.sendEmptyMessage(MSG_STOP_TRAINING_RECORDING);
            }
        }, TRAINING_RECORDING_DURATION);
    }

    private void stopRecordingTimer() {
        Log.v(TAG, "stopTrainingTimer");
        if (null != trainingTimer) {
            trainingTimer.cancel();
            trainingTimer = null;
        }
    }

    private void startRecording() {
        Log.v(TAG, "startTraining");
        recorder = ListenAudioRecorder.getInstance();
        recorder.start();
        startRecordingTimer();
        uiRecordingState.setText(recordingState);
        uiMic.setImageResource(R.drawable.speaker_on);
        Log.v(TAG, "startTraining: recorder.start()");
    }

    private void stopRecording() {
        Log.v(TAG, "stopTraining");
        recorder.stop(Global.getInstance().getLastUserRecordingFilePath());
        uiRecordingState.setText(processingState);
        uiMic.setImageResource(R.drawable.speaker_off);
        Log.v(TAG, "stopTraining: recorder.stop");
    }

    // Updates UI and sends a message to the Service to extend the sound model.
    private void finishTraining() {
        Log.v(TAG, "finishTraining");
        isCurrentlyTraining = false;
        uiMicLayout.setVisibility(View.INVISIBLE);
        uiNotice.setVisibility(View.INVISIBLE);
        uiContinue.setVisibility(View.VISIBLE);
        String recordingState = getString(R.string.training_recording_complete);
        uiRecordingState.setText(recordingState);
    }


    // Sets up service connection
    private ServiceConnection mConnection = new ServiceConnection() {

        // Registers as a client to receive messages from the Service
        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.v(TAG, "onServiceConnected");
            sendToServiceMessenger = new Messenger(service);

            // Registers client
            sendReply(MessageType.MSG_REGISTER_CLIENT, null);
            Log.v(TAG, "connected service");
        }

        // Unregisters as a client to receive messages from the Service
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
            switch (msg.what) {
                case MessageType.MSG_RECORDING_RESULT:
                    Log.v(TAG, "handleMessage: MSG_RECORDING_RESULT");
                    if (isCurrentlyTraining) {
                        updateTraining(checkTrainingSuccess(msg.arg1));
                    } else {
                        Log.v(TAG, "handleMessage: MSG_RECORDING_RESULT- stopped training, discarding MSG");
                    }
                    break;

                case MessageType.MSG_EXTEND_SOUNDMODEL:
                    Log.v(TAG, "handleMessage: MSG_EXTEND_SOUNDMODEL");
                    if (progressDialog != null && progressDialog.isShowing()) {
                        progressDialog.dismiss();
                        Log.v(TAG, "handleMessage: MSG_EXTEND_SOUNDMODEL: progress dialog dismissed");
                    }
                    if (msg.arg1 == Global.SUCCESS) {
                        Log.v(TAG, "handleMessage: MSG_EXTEND_SOUNDMODEL successful");
                        setResult(RESULT_OK);
                        finish();
                    } else if (msg.arg1 == Global.FAILURE) {
                        Log.v(TAG, "handleMessage: MSG_EXTEND_SOUNDMODEL failed");
                        AlertDialog.Builder builder = new AlertDialog.Builder(TrainingActivity.this);
                        builder.setTitle(R.string.training_dialog_smcreatefailed_title)
                                .setMessage(R.string.training_dialog_smcreatefailed_message)
                                .setCancelable(false)
                                .setPositiveButton(R.string.dialog_ok, new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        Log.v(TAG, "handleMessage: MSG_EXTEND_SOUNDMODEL failed: alert- user selected OK");
                                        resetTrainings();
                                        setVisibleRecordingArea();
                                        uiMicLayout.setVisibility(View.VISIBLE);
                                        startRecording();
                                    }
                                });
                        if (false == ((Activity) TrainingActivity.this).isFinishing()) {
                            builder.show();
                        }
                    } else {
                        Log.e(TAG, "handleMessage: MSG_EXTEND_SOUNDMODEL: unknown message= " + msg.arg1);
                    }
                    break;

                default:
                    Log.v(TAG, "handleMessage: no such case: " + msg.what);
            }
        }

        private Boolean checkTrainingSuccess(int confidenceLevel) {
            Log.v(TAG, "checkTrainingSuccess: confidence level = " + confidenceLevel);
            if (isUdk) {
                return (confidenceLevel >= TRAINING_CONFIDENCE_LEVEL_THRESHOLD_UDK) ? true : false;
            } else {
                return (confidenceLevel >= TRAINING_CONFIDENCE_LEVEL_THRESHOLD) ? true : false;
            }
        }

        private void updateTraining(Boolean trainingSuccessful) {
            Log.v(TAG, "updateTraining");
            Log.v(TAG, "updateTraining: trainingSuccessful = " + trainingSuccessful);
            uiNotice.setVisibility(View.VISIBLE);

            recordingCounter.updateRecordingResult(trainingSuccessful);

            if (trainingSuccessful) {
                // start new recording
                Log.v(TAG, "train speech is ACCEPTED\n");
                uiNotice.setTextColor(getResources().getColor(R.color.bg_recording_good));
                uiNotice.setText(R.string.training_recording_notice_good);
            } else {
                Log.v(TAG, "train speech is REJECTED\n");
                uiNotice.setTextColor(getResources().getColor(R.color.bg_recording_bad));
                uiNotice.setText(R.string.training_recording_notice_bad);
            }

            if (recordingCounter.isFinished()) {
                isCompletedTraining = true;
                finishTraining();
            } else {
                startRecording();
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

    // Sends message to the Service with one or two ints as data
    void sendDataReply(int what, Object obj, int msgArg1, Integer msgArg2) {
        Log.v(TAG, "sendMessageDataAll");
        if (null == sendToServiceMessenger) {
            return;
        }

        Message msg = Message.obtain(null, what, obj);
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
}
