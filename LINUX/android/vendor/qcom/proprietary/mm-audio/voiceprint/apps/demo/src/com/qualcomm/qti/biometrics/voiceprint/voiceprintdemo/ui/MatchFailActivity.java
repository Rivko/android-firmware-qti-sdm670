/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.app.ActionBar;
import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;

import com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.OnAudioStateChangeListener;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.KeyPhraseAction;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.data.VoicePrintPreferences;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultBase;

public class MatchFailActivity extends VoicePrintActivity implements MicFragment.OnMicEvent {

    private MicFragment micFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.match_failed);

        // Navigation back
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        // Recolor the action bar (error)
        actionBar.setBackgroundDrawable(new ColorDrawable(getResources().getColor(R.color.error_color)));

        setupMicFragment();
    }

    private void setupMicFragment() {
        micFragment = (MicFragment) getFragmentManager().findFragmentById(R.id.micFragment);

        micFragment.setStyle(MicFragment.ERROR_BG);

        // Show bottom text
        micFragment.showTextTop(true);
        micFragment.showTextBottom(true);
        micFragment.showTextBottom2(true);


        // Setup the text
        micFragment.setTextTop(getString(R.string.oops));
        micFragment.getTextBottom2().setText(Html.fromHtml(String.format(
                getString(R.string.say_one_more_time),
                getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));
        micFragment.setOnMicEventListener(this);
    }

    @Override
    public void onMicButtonClick() {

    }

    @Override
    public void onRecordingStarted() {
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(false);

        // micFragment.showTextTop(false);

        micFragment.showTextBottom2(false);

        micFragment.getTextTop().setText(Html.fromHtml(String.format(getString(R.string.say_key_phrase),
                                         getVoicePrintApp().getCurrentKeyPhrase().getPhrase())));

        Log.d(TAG, "MatchFailActivity: onRecordingStarted");
        invalidateOptionsMenu();

        // Start the match request
        if (getVoicePrintApp().getCurrentKeyPhrase() == null) {
            // TODO match all
            Log.d(TAG, "No current key phrase selected.");
            showDialog(this, "No Keyphrase Selected", "Please select a keyphrase first.", "OK", null);
        } else {
            AudioRecordManager audioMgr = AudioRecordManager.getInstance();
            Log.d(TAG, "Attempting to match.");
            mManager.match(getVoicePrintApp().getCurrentKeyPhrase().getId(), new OnMatchListener() {
                @Override
                public void onMatchStatus(boolean success, Bundle extra) {
                    getVoicePrintApp().update(extra);
                    success = getVoicePrintApp().processMatchScore(extra);
                    onMatch(success);
                }

                @Override
                public void onError(int errorCode) {
                    Log.d(TAG, "Match failed. Error=" + errorCode);
                    onMatch(false);
                }
            });

            mManager.onStartAudioCapture(audioMgr.getAudioSampleRate(),
                                         audioMgr.getInputChannelConfig(),
                                         audioMgr.getAudioFormat(), null);
        }
    }


    private void onMatch(boolean success) {

        if (AudioRecordManager.getInstance().isRecording()) {
            AudioRecordManager.getInstance().stopRecording();
        }

        if ( false == success ) {
            setupMicFragment();
            return;
        }


        int actionId = getVoicePrintApp().getCurrentKeyPhrase().getActionId();
        switch (actionId) {
        case KeyPhraseAction.NO_ACTION:
            startActivity(new Intent(this, MatchSuccessActivity.class));
            break;
        case KeyPhraseAction.BROWSER: {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.addCategory(Intent.CATEGORY_APP_BROWSER);
            startActivity(intent);
        }
        break;
        case KeyPhraseAction.EMAIL: {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.addCategory(Intent.CATEGORY_APP_EMAIL);
            startActivity(intent);
        }
        break;
        case KeyPhraseAction.SMS: {
            Intent intent = new Intent(Intent.ACTION_MAIN);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);
            intent.addCategory(Intent.CATEGORY_APP_MESSAGING);
            startActivity(intent);
        }
        break;
        default:
            Log.d(TAG, "Unknown action: " + actionId);
            break;
        }

    }

    @Override
    public void onRecordingStopped() {
        ActionBar actionBar = this.getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);

        mManager.onStopAudioCapture(null);

    }

    @Override
    public void onAudioParametersChange() {

    }

    @Override
    public void onAudioFrame(int index, byte[] buffer, float energy) {
        mManager.addFrame(index, buffer);

    }

    @Override
    public void onError(OnAudioStateChangeListener.AudioError error) {

    }

    @Override
    public void onResume() {
        super.onResume();

        micFragment.showTextTop(true);
        micFragment.showTextBottom2(true);
    }
}
