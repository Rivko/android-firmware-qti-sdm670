/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant;

import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.qualcomm.qti.smartassistant.training.TrainingAudioRecorder;
import com.qualcomm.qti.smartassistant.training.SoundModelTrainer;
import com.qualcomm.qti.smartassistant.training.TrainingAssistant;
import com.qualcomm.qti.smartassistant.utils.FileUtils;

import java.util.Timer;
import java.util.TimerTask;

public class TrainingActivity extends ServiceActivity {
    private final static String TAG = "TrainingActivity";
    public final static String RESULT_CONTENT = "traning_name";
    private final static int TRAINING_RECORDING_DURATION = 3000;
    private final static int MSG_STOP_TRAINING_RECORDING = 1;


    private RecordingCounter recordingCounter = null;
    private boolean isCurrentlyTraining = false;
    private String userName;
    private String keyphrase;
    private boolean isUdk = false;
    private String recordingState;
    private String processingState;
    private TrainingAudioRecorder recorder = null;

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
    private AlertDialog mConfirmDialog;
    private int mConfidenceLevel;

    private SoundModelTrainer mTrainer;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_training);

        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        Intent passedIntent = getIntent();
        keyphrase = passedIntent.getExtras().getString(TrainingAssistant.TAG_TRAINING_KEYPHRASE);
        userName = passedIntent.getExtras().getString(TrainingAssistant.TAG_TRAINING_USER);
        isUdk = passedIntent.getExtras().getBoolean(TrainingAssistant.TAG_TRAINING_IS_UDK, false);

        if (null == userName || userName.isEmpty()) {
            Log.e(TAG, "onCreate: no user name. Calling finish().");
            finish();
        }

        TrainingAssistant.getInstance().removeExistingRecordingFiles();
        TrainingAssistant.getInstance().removeUserRecordings();
        initalizeUserInterface();

        mTrainer = new SoundModelTrainer(keyphrase);
        mConfidenceLevel = FileUtils.getTrainingConfidenceLevelSetting();

        try {
            imageViews = new ImageView[]{
                    findViewById(R.id.training_record1),
                    findViewById(R.id.training_record2),
                    findViewById(R.id.training_record3),
                    findViewById(R.id.training_record4),
                    findViewById(R.id.training_record5)
            };
            recordingCounter = new RecordingCounter(this, imageViews);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    // Send back to AssistantSettingsActivity whether training completed successfully.
    @Override
    protected void onPause() {
        super.onPause();
        if (isCurrentlyTraining) {
            changeStateToNotRecording();
        }
    }

    @Override
    public void onBackPressed() {
        setResult(RESULT_CANCELED);
        super.onBackPressed();
    }

    private void initalizeUserInterface() {
        uiWelcomeLayout = findViewById(R.id.layout_training_welcome);
        uiDisableLayout = findViewById(R.id.layout_training_disableview);
        uiUserName = findViewById(R.id.training_username);
        uiRecordLayout = findViewById(R.id.layout_training_record);
        uiRecordingState = findViewById(R.id.training_recording_state);
        uiClose = findViewById(R.id.training_recording_close);
        uiKeyPhrase = findViewById(R.id.training_keyphrase);
        uiNotice = findViewById(R.id.training_recording_notice);
        uiMicLayout = findViewById(R.id.training_layoutMic);
        uiMic = findViewById(R.id.training_mic);
        uiContinue = findViewById(R.id.training_recording_continue);

        recordingState = getString(R.string.training_recording_recording);
        processingState = getString(R.string.training_recording_processing);

        uiMic.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(TAG, "uiMic.onClick");
                changeStateToRecording();
            }
        });

        uiClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(TrainingActivity.this);
                View view = View.inflate(TrainingActivity.this,
                        R.layout.dialog_training_close_confirm, null);
                builder.setView(view);
                builder.setCancelable(false);
                TextView title = view.findViewById(R.id.close_alert_title);
                title.setText(R.string.training_dialog_close_title);
                Button pBtn = view.findViewById(R.id.close_positiveButton);
                Button nBtn = view.findViewById(R.id.close_negativeButton);
                pBtn.setText(R.string.dialog_ok);
                nBtn.setText(R.string.dialog_cancel);
                pBtn.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        changeStateToNotRecording();
                        mConfirmDialog.dismiss();
                    }
                });
                nBtn.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        mConfirmDialog.dismiss();
                    }
                });
                if (false == ((Activity) TrainingActivity.this).isFinishing()) {
                    mConfirmDialog = builder.create();
                    mConfirmDialog.show();
                }
            }
        });

        uiUserName.setText(userName);
        uiKeyPhrase.setText(keyphrase);

        uiContinue.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.v(TAG, "uiContinue.onClick");
                String pleaseWait = getString(R.string.home_dialog_pleasewait);
                String creatingSoundModel = getString(R.string.training_dialog_createsoundmodel);

                progressDialog = ProgressDialog.show(TrainingActivity.this,
                        creatingSoundModel, pleaseWait);
                Log.v(TAG, "uiContinue.onClick: progress dialog started");
                String[] keyphraseUserIsudk = new String[3];
                keyphraseUserIsudk[0] = userName;

                new ExtendSoundTask().execute(keyphraseUserIsudk);

            }
        });
    }

    private void changeStateToRecording() {
        isCurrentlyTraining = true;
        //TODO figure out if I need to reset trainings here and on stopRecording.
        resetTrainings();
        setVisibleRecordingArea();
        startRecording();
    }

    private void changeStateToNotRecording() {
        isCurrentlyTraining = false;
        //TODO figure out if I need to reset trainings here and on startRecording.
        resetTrainings();
        setVisibleTrainingMainArea();
        stopRecording();
        stopRecordingTimer();
    }

    private void resetTrainings() {
        TrainingAssistant.getInstance().removeExistingRecordingFiles();
        TrainingAssistant.getInstance().removeUserRecordings();
        try {
            recordingCounter = new RecordingCounter(TrainingActivity.this, imageViews);
        } catch (Exception e) {
            e.printStackTrace();
        }
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
            if (MSG_STOP_TRAINING_RECORDING == msg.what) {
                stopRecording();
                new VerfifyRecordingTask().execute();
            }
        }
    };

    private class VerfifyRecordingTask extends AsyncTask<String, Void, Integer> {

        @Override
        protected Integer doInBackground(String...strings) {
            return mTrainer.verifyUserRecording();
        }

        @Override
        protected void onPostExecute(Integer result) {
            if (isCurrentlyTraining) {
                updateTraining(checkTrainingSuccess(result));
            }
        }
    }

    private class ExtendSoundTask extends AsyncTask<String, Void, Integer> {

        @Override
        protected Integer doInBackground(String...strings) {
            //TODO:
            String username = strings[0];
            if(mTrainer.extendSoundModel(username)) {
                return 1;
            } else {
                return 0;
            }
        }

        @Override
        protected void onPostExecute(Integer result) {
            if (progressDialog != null && progressDialog.isShowing()) {
                progressDialog.dismiss();
            }
            if (result == 1) {
                Intent data = new Intent();
                data.putExtra(RESULT_CONTENT, userName);
                setResult(RESULT_OK, data);
                finish();
            }
        }
    }

    private Boolean checkTrainingSuccess(int confidenceLevel) {
        return (confidenceLevel >= mConfidenceLevel) ? true : false;
    }

    private void updateTraining(Boolean trainingSuccessful) {
        uiNotice.setVisibility(View.VISIBLE);

        recordingCounter.updateRecordingResult(trainingSuccessful);

        if (trainingSuccessful) {
            // start new recording
            uiNotice.setTextColor(getResources().getColor(R.color.bg_recording_good));
            uiNotice.setText(R.string.training_recording_notice_good);
        } else {
            Log.v(TAG, "train speech is REJECTED\n");
            uiNotice.setTextColor(getResources().getColor(R.color.bg_recording_bad));
            uiNotice.setText(R.string.training_recording_notice_bad);
        }

        if (recordingCounter.isFinished()) {
            finishTraining();
        } else {
            startRecording();
        }
    }

    // Stops recording after TRAINING_RECORDING_DURATION milliseconds
    private void startRecordingTimer() {
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
        if (null != trainingTimer) {
            trainingTimer.cancel();
            trainingTimer = null;
        }
    }

    private void startRecording() {
        recorder = TrainingAudioRecorder.getInstance();
        recorder.start();
        startRecordingTimer();
        uiRecordingState.setText(recordingState);
        uiMic.setImageResource(R.drawable.speaker_on);
        Log.v(TAG, "startTraining: recorder.start()");
    }

    private void stopRecording() {
        recorder.stop(TrainingAssistant.getInstance().getLastUserRecordingFilePath());
        uiRecordingState.setText(processingState);
        uiMic.setImageResource(R.drawable.speaker_off);
        Log.v(TAG, "stopTraining: recorder.stop");
    }

    // Updates UI and sends a message to the Service to extend the sound model.
    private void finishTraining() {
        isCurrentlyTraining = false;
        uiMicLayout.setVisibility(View.INVISIBLE);
        uiNotice.setVisibility(View.INVISIBLE);
        uiContinue.setVisibility(View.VISIBLE);
        String recordingState = getString(R.string.training_recording_complete);
        uiRecordingState.setText(recordingState);
    }

    public class RecordingCounter {
        private static final String TAG = "RecordingCounter";
        private final static int MAX_ITEM_COUNT = 5;
        private ImageView[] views;
        private int recordingCounter = 0;

        public RecordingCounter(Context context, ImageView[] views) throws Exception {
            this.views = views;

            if (MAX_ITEM_COUNT != views.length) {
                throw new Exception("Invalid argument");
            }

            initCounter();
        }

        private void initCounter() {
            recordingCounter = 0;
            views[0].setBackgroundResource(R.drawable.recording_step_off);
            views[0].setImageResource(R.drawable.recording_1_gray);
            views[1].setBackgroundResource(R.drawable.recording_step_off);
            views[1].setImageResource(R.drawable.recording_2_gray);
            views[2].setBackgroundResource(R.drawable.recording_step_off);
            views[2].setImageResource(R.drawable.recording_3_gray);
            views[3].setBackgroundResource(R.drawable.recording_step_off);
            views[3].setImageResource(R.drawable.recording_4_gray);
            views[4].setBackgroundResource(R.drawable.recording_step_off);
            views[4].setImageResource(R.drawable.recording_5_gray);

            setImageFocusChange();
        }

        // Sets the current training image to blue when training
        private void setImageFocusChange() {
            if (MAX_ITEM_COUNT <= recordingCounter) {
                return;
            }

            int resourceId = 0;
            switch (recordingCounter) {
                case 0:
                    resourceId = R.drawable.recording_1_blue;
                    break;
                case 1:
                    resourceId = R.drawable.recording_2_blue;
                    break;
                case 2:
                    resourceId = R.drawable.recording_3_blue;
                    break;
                case 3:
                    resourceId = R.drawable.recording_4_blue;
                    break;
                case 4:
                    resourceId = R.drawable.recording_5_blue;
                    break;
            }

            views[recordingCounter].setBackgroundResource(R.drawable.recording_step_focused);
            views[recordingCounter].setImageResource(resourceId);
        }

        // Sets the current training image to white when done training
        private void setImageDone() {
            if (MAX_ITEM_COUNT <= recordingCounter) {
                return;
            }

            int resourceId = 0;
            switch (recordingCounter) {
                case 0:
                    resourceId = R.drawable.recording_1_white;
                    break;
                case 1:
                    resourceId = R.drawable.recording_2_white;
                    break;
                case 2:
                    resourceId = R.drawable.recording_3_white;
                    break;
                case 3:
                    resourceId = R.drawable.recording_4_white;
                    break;
                case 4:
                    resourceId = R.drawable.recording_5_white;
                    break;
            }

            views[recordingCounter].setBackgroundResource(R.drawable.recording_step_done);
            views[recordingCounter].setImageResource(resourceId);
        }

        // Sets the current training image to white when training fails
        private void setImageFailed() {
            if (MAX_ITEM_COUNT <= recordingCounter) {
                return;
            }

            int resourceId = 0;
            switch (recordingCounter) {
                case 0:
                    resourceId = R.drawable.recording_1_white;
                    break;
                case 1:
                    resourceId = R.drawable.recording_2_white;
                    break;
                case 2:
                    resourceId = R.drawable.recording_3_white;
                    break;
                case 3:
                    resourceId = R.drawable.recording_4_white;
                    break;
                case 4:
                    resourceId = R.drawable.recording_5_white;
                    break;
            }

            views[recordingCounter].setBackgroundResource(R.drawable.recording_step_fail);
            views[recordingCounter].setImageResource(resourceId);
        }

        // Updates the UI if training was successful
        public void updateRecordingResult(boolean isGoodRecording) {
            if (isGoodRecording) {
                setImageDone();
                recordingCounter++;
                setImageFocusChange();
            } else {
                TrainingAssistant.getInstance().discardLastUserRecording();
                setImageFailed();
            }
        }

        public boolean isFinished() {
            Log.v(TAG, "isFinished: recordingCounter= " + recordingCounter);
            return MAX_ITEM_COUNT <= recordingCounter;
        }
    }

}
