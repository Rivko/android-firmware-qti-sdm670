/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.Intent;
import android.os.Bundle;
import android.text.Html;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.qualcomm.qti.biometrics.voiceprint.sdk.OnEnrollListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnErrorListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.OnAudioStateChangeListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintDatabase;

import java.sql.SQLException;

/**
 * Register phrase activiting. This activity contains the training steps and final commit.
 * If this activity is successful, the user has trained their voice up to three times and
 * the voice template has been saved in trust zone.
 */
public class RegisterPhraseActivity extends VoicePrintActivity implements MicFragment.OnMicEvent {

    private Button actionButton;
    private static int currentStep = 1;

    private MicFragment micFragment;
    private boolean mInError = false;

    private final int incomplete = R.drawable.status_dot_gray;
    private final int current = R.drawable.status_dot_black;
    private final int complete = R.drawable.status_dot_green;
    private final int error = R.drawable.status_dot_red;

    private ImageView lastStepImage;
    private ImageView currentStepImage;
    private TextView lastTextImage;
    private ImageView lastStepCheckImage;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.register_phrase_step);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        setupMicFragment();

        actionButton = (Button) findViewById(R.id.button);
        // Previous step is ignored for now. Do not handle actionButton click.
        actionButton.setVisibility(View.INVISIBLE);
    }

    private void previousStep() {
        currentStepImage.setImageResource(incomplete);
        lastStepCheckImage.setVisibility(View.INVISIBLE);
        lastTextImage.setVisibility(View.VISIBLE);
        --currentStep;
        processStep();
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Reset
        currentStep = 1;
        processStep();

        // SDK
        Log.d(TAG, "OnResume...requesting enroll...");
        mManager.enroll(getVoicePrintApp().getCurrentKeyPhrase().getId(),
                getVoicePrintApp().getCurrentKeyPhrase().getPhrase(),
                getVoicePrintApp().getCurrentKeyPhrase().getThreshold(),
                new OnEnrollListener() {
                    @Override
                    public void onEnrollComplete(boolean success, Bundle extra) {
                        getVoicePrintApp().update(extra);
                        processResult(success);
                    }

                    @Override
                    public void onError(int errorCode) {
                        processResult(false);
                    }
                });
    }

    private void processResult(boolean success) {
        Log.d(TAG, "ProcessResult");

        if( micFragment.isRecording() ){
            Log.d(TAG, "Result before recording completed. Received an error.");
            micFragment.stopRecording();
        }

        if (success) {

            Toast.makeText(this, "Saving to database...", Toast.LENGTH_SHORT).show();

            try {
                VoicePrintDatabase.getInstance().insertEntry(getVoicePrintApp().getCurrentKeyPhrase());
            } catch (SQLException e) {
                e.printStackTrace();
            }
            startActivity(new Intent(this,
                    RegistrationCompleteActivity.class));

        } else {
            Log.d(TAG, "Registration has failed.");
            startActivity(new Intent(this, RegistrationFailedActivity.class));

        }
        finish();
    }

    @Override
    protected void onPause() {
        // Reset
        currentStep = 1;
        mManager.cancel();

        super.onPause();
    }

    private void setupMicFragment() {
        micFragment = (MicFragment) getFragmentManager().findFragmentById(R.id.micFragment);

        // Set correct theme
        micFragment.setStyle(MicFragment.LIGHT_BG);

        // Show bottom text
        micFragment.showTextTop(true);
        micFragment.showTextBottom(true);
        micFragment.showTextBottom2(false);

        micFragment.setOnMicEventListener(this);
        micFragment.getTextTop().setVisibility(View.INVISIBLE);
    }


    private void handleNextStep() {
        ++currentStep;
        // Clear error
        mInError = false;
        micFragment.getTextTop().setText(Html.fromHtml(String.format(getString(R.string.say_key_phrase),
                getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));

        processStep();
        micFragment.setEnabled(true);
    }

    /**
     * Error in this training step.
     */
    private void handleStepError() {
        // Setup error.
        mInError = true;

        Log.d(TAG, "onError at step " + currentStep);

        processStep();
        micFragment.setEnabled(true);
    }

    private void processStep() {
        LayoutInflater inflater = LayoutInflater.from(this);

        Log.d(TAG, "Processing step " + currentStep + " inError? " + mInError);

        // Setup the text
        if (mInError) {

            try {
                String string = String.format(getString(R.string.say_key_phrase),
                        getVoicePrintApp().getCurrentKeyPhrase().getPhrase());
                Spannable text = new SpannableString(String.format("%s %s",
                    getString(R.string.oops), Html.fromHtml(string)));
                text.setSpan(new ForegroundColorSpan(0xffe01e3d), 0,
                    getString(R.string.oops).length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);

                micFragment.getTextTop().setText(text);
                micFragment.getTextTop().setVisibility(View.VISIBLE);
            } catch (Exception e) {
                e.printStackTrace();
            }
            Log.d(TAG, "Setting the error text");

        } else {


            micFragment.getTextTop().setText(Html.fromHtml(String.format(getString(R.string.say_key_phrase),
                    getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));

            Log.d(TAG, "Setting regular text.");
        }

        Log.d(TAG, "Updating UI for step");

        // Handle this new step
        switch (currentStep) {
            case 1: {

                micFragment.setEnabled(true);

                // UI Setup
                currentStepImage = (ImageView) findViewById(R.id.step1);
                actionButton.setVisibility(View.INVISIBLE);
                currentStepImage.setImageResource(mInError ? error : current);


                break;
            }
            case 2: {
                actionButton.setText(getString(R.string.cap_redo_previous));
                actionButton.setVisibility(View.INVISIBLE);

                currentStepImage = (ImageView) findViewById(R.id.step2);
                lastStepImage = (ImageView) findViewById(R.id.step1);
                lastStepCheckImage = (ImageView) findViewById(R.id.step1Check);
                lastTextImage = (TextView) findViewById(R.id.step1Text);
                currentStepImage.setImageResource(mInError ? error : current);
                lastStepImage.setImageResource(complete);
                lastStepCheckImage.setVisibility(View.VISIBLE);
                lastTextImage.setVisibility(View.INVISIBLE);
                break;
            }
            case 3: {
                actionButton.setText(getString(R.string.cap_redo_previous));
                actionButton.setVisibility(View.INVISIBLE);

                currentStepImage = (ImageView) findViewById(R.id.step3);
                lastStepImage = (ImageView) findViewById(R.id.step2);
                lastStepCheckImage = (ImageView) findViewById(R.id.step2Check);
                lastTextImage = (TextView) findViewById(R.id.step2Text);
                currentStepImage.setImageResource(mInError ? error : current);
                lastStepImage.setImageResource(complete);
                lastStepCheckImage.setVisibility(View.VISIBLE);
                lastTextImage.setVisibility(View.INVISIBLE);
                break;


            }
            case 4:
                // Done!
                Log.d(TAG, "Completed enrollment.");

                Toast.makeText(this, "Saving the voiceprint template...", Toast.LENGTH_SHORT).show();
                mManager.completeEnroll(new OnErrorListener() {
                    @Override
                    public void onError(int errorCode) {
                        Log.d(TAG, "Error committing the template! code=" + errorCode);
                    }
                });

                micFragment.setEnabled(false);


                break;
            default:
                break;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        if (micFragment.isRecording()) {
            return false;
        }

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.register_phrase, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks. The action bar will
        // automatically handle clicks on the Home/Up button,
        // parent activity specified in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_cancel) {
            startActivity(new Intent(this, MainActivity.class));
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onMicButtonClick() {

    }

    @Override
    public void onRecordingStarted() {

        AudioRecordManager audioMgr = AudioRecordManager.getInstance();
        mManager.onStartAudioCapture(audioMgr.getAudioSampleRate(),
                audioMgr.getInputChannelConfig(),
                audioMgr.getAudioFormat(),
                new OnErrorListener() {
                    @Override
                    public void onError(int errorCode) {
                        Log.d(TAG, "Error starting the template, code=" + errorCode);
                        handleStepError();
                    }
                });

        micFragment.getTextTop().setVisibility(View.VISIBLE);


        // Hide Menu
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(false);
        invalidateOptionsMenu();
    }

    @Override
    public void onRecordingStopped() {
        micFragment.setEnabled(false);

        micFragment.getTextTop().setVisibility(View.INVISIBLE);

        mManager.onStopAudioCapture(
                new OnResultListener() {
                    @Override
                    public void onResult(int code, Bundle extras) {
                        Log.d(TAG, "onResult=" + code);
                        Log.d(TAG, "onRecordingStopped result processing..");
                        getVoicePrintApp().update(extras);
                        if (code == OnResultListener.SUCCESS) {
                            handleNextStep();
                        } else {
                            handleStepError();
                        }
                    }

                    @Override
                    public void onError(int code) {
                        handleStepError();
                    }
                });

        // Show Menu
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        invalidateOptionsMenu();

        Log.d(TAG, "Received onRecordingStopped event in RegisterPhraseActivity.");
    }

    @Override
    public void onAudioParametersChange() {

    }

    @Override
    public void onAudioFrame(int index, byte[] buffer, float energy) {
        //Log.d(TAG, "onAudioFrame=" + buffer.length);
        mManager.addFrame(buffer);
    }

    @Override
    public void onError(OnAudioStateChangeListener.AudioError error) {

    }
}
