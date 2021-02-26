/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.ui;

import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.TextView;

import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.R;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.VoicePrintDemoApp;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.AudioRecordManager;
import com.qualcomm.qti.biometrics.voiceprint.voiceprintdemo.audio.OnAudioStateChangeListener;

import java.util.Date;

/**
 * Fragment class containing a mic control which starts and stop the
 * AudioRecord thread.
 */
public class MicFragment extends BaseFragment implements OnAudioStateChangeListener {

    private VoicePrintDemoApp mApp;

    private TextView mTextTop;
    private TextView mTextBottom;
    private TextView mTextBottom2;
    private ImageButton mMicImage;

    private int mTextTopColor;
    private int mTextBottomColor;
    private int mTextBottom2Color;

    private boolean mShowBottom = true;
    private boolean mShowBottom2 = true;
    private boolean mShowTop = true;

    private Typeface mTextTopTypeface;
    private Typeface mTextBottomTypeface;
    private Typeface mTextBottom2Typeface;

    private int mMicImageResourceId;

    private View mMainContainer;
    private int mBackgroundColor;

    private String mTextBottomString;
    private String mTextBottom2String;
    private String mTextTopString;
    private long mLastToggle = 0;
    private boolean mEnabled = true;
    private View mEffectView;
    private VoiceEffecter mVoiceEffecter = null;

    private static final long MIN_TOGGLE_TIMEOUT = 500;

    private final float MAX = 14.5f;
    private final float MIN = 7.0f;
    public final static int LIGHT_BG = 0;
    public final static int DARK_BG = 1;
    public final static int ERROR_BG = 2;

    private final static int MAX_RECORDING = 10000; // 10 seconds

    private OnMicEvent eventListener;

    public interface OnMicEvent {
        public void onMicButtonClick();

        public void onRecordingStarted();

        public void onRecordingStopped();

        public void onAudioParametersChange();

        public void onAudioFrame(int index, byte[] buffer, float energy);

        /**
         * The recording has been cancelled or could not be started.
         * This may be due to an incoming phone call or other internal system error.
         */
        public void onError(AudioError error);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.mic_fragment, container,
                                         false);

        // Access to app
        mApp = getApp();

        // UI Controls
        mTextTop = (TextView) rootView.findViewById(R.id.textTop);
        mTextBottom = (TextView) rootView.findViewById(R.id.textBottom);
        mTextBottom2 = (TextView) rootView.findViewById(R.id.textBottom2);
        mMicImage = (ImageButton) rootView.findViewById(R.id.micImage);
        mMainContainer = rootView.findViewById(R.id.micMainLayout);

        // Defaults
        mTextTopColor = getActivity().getResources().getColor(R.color.dark_text_color);
        mTextBottomColor = getActivity().getResources().getColor(R.color.dark_text_color);
        mTextTopString = mTextTop.getText().toString();
        mTextBottomString = getString(R.string.tap_mic_start);
        mTextBottom2String = mTextBottom2.getText().toString();

        // Visualizer
        mEffectView = rootView.findViewById(R.id.mic_effect);
        mVoiceEffecter = new VoiceEffecter(MIN, MAX, mEffectView, 1.0f);

        // Toggling the mic control
        mMicImage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OnMicClick(v);
            }
        });

        mMicImage.setSoundEffectsEnabled(false);

        return rootView;
    }


    public void setStyle(int style) {

        switch (style) {
        case LIGHT_BG:
            setMicResource(R.drawable.mic_button);
            setBackgroundColor(Color.WHITE);
            setTextTopColor(getResources().getColor(R.color.dark_text_color));
            setTextBottomColor(getResources().getColor(R.color.dark_text_color));
            setTextBottom2Color(getResources().getColor(R.color.dark_text_color));
            mEffectView.setBackgroundResource(R.drawable.mic_effect_dark);

            break;
        case DARK_BG:
            setMicResource(R.drawable.mic_button);
            setBackgroundColor(getResources().getColor(R.color.primary_theme_color));
            setTextTopColor(getResources().getColor(R.color.light_text_color));
            setTextBottomColor(getResources().getColor(R.color.light_text_color));
            setTextBottom2Color(getResources().getColor(R.color.light_text_color));
            mEffectView.setBackgroundResource(R.drawable.mic_effect_light);
            break;

        case ERROR_BG:
            setMicResource(R.drawable.mic_error_button);
            setBackgroundColor(getResources().getColor(R.color.error_color));
            setTextTopColor(getResources().getColor(R.color.light_text_color));
            setTextBottomColor(getResources().getColor(R.color.light_text_color));
            setTextBottom2Color(getResources().getColor(R.color.light_text_color));
            mEffectView.setBackgroundResource(R.drawable.mic_effect_light);
            break;

        default:
            Log.d(TAG, "Invalid style set: " + style);
            break;
        }
    }

    public float getVoiceMin() {
        return MIN;
    }

    public float getVoiceMax() {
        return MAX;
    }


    public void setTextTopColor(int color) {
        mTextTopColor = color;

        if (mTextTop != null) {
            mTextTop.setTextColor(color);
        }
    }

    public void setTextBottomColor(int color) {
        mTextBottomColor = color;

        if (mTextBottom != null) {
            mTextBottom.setTextColor(color);
        }
    }

    public void setTextBottom2Color(int color) {
        mTextBottom2Color = color;

        if (mTextBottom2 != null) {
            mTextBottom2.setTextColor(color);
        }
    }


    public void showTextTop(boolean show) {

        mShowTop = show;

        if (mTextTop != null) {
            mTextTop.setVisibility(show ? View.VISIBLE : View.GONE);
        }
    }

    public void showTextBottom(boolean show) {

        mShowBottom = show;

        if (mTextBottom != null) {
            mTextBottom.setVisibility(show ? View.VISIBLE : View.GONE);
        }
    }

    public void showTextBottom2(boolean show) {

        mShowBottom2 = show;

        if (mTextBottom2 != null) {
            mTextBottom2.setVisibility(show ? View.VISIBLE : View.GONE);
        }
    }

    public void setTextTopTypeface(Typeface typeface) {
        mTextTopTypeface = typeface;

        if (mTextTop != null) {
            mTextTop.setTypeface(typeface);
        }
    }

    public void setTextBottomTypeface(Typeface typeface) {
        mTextBottomTypeface = typeface;

        if (mTextBottom != null) {
            mTextBottom.setTypeface(typeface);
        }
    }

    public void setTextBottom2Typeface(Typeface typeface) {
        mTextBottom2Typeface = typeface;

        if (mTextBottom2 != null) {
            mTextBottom2.setTypeface(typeface);
        }
    }


    public void setMicResource(int id) {
        mMicImageResourceId = id;

        if (mMicImage != null) {
            mMicImage.setImageResource(id);
        }
    }

    public void setBackgroundColor(int color) {
        mBackgroundColor = color;

        mMainContainer.setBackgroundColor(color);
    }

    public void setTextTop(String text) {
        if (mTextTop != null) {
            mTextTop.setText(text);
        }
        mTextTopString = text;
    }

    public void setTextBottom(String text) {
        if (mTextBottom != null) {
            mTextBottom.setText(text);
        }
        mTextBottomString = text;
    }

    public void setTextBottom2(String text) {
        if (mTextBottom2 != null) {
            mTextBottom2.setText(text);
        }
        mTextBottom2String = text;
    }

    public TextView getTextBottom2() {
        return mTextBottom2;
    }

    public TextView getTextTop() {
        return mTextTop;
    }

    public TextView getTextBottom() {
        return mTextBottom;
    }

    @Override
    public void onHiddenChanged(boolean hidden) {
        super.onHiddenChanged(hidden);

        // If visible, check the status and update text
        if (hidden == false) {
            mTextTop.setVisibility(mShowTop ? View.VISIBLE : View.INVISIBLE);
            mTextTop.setTextColor(mTextTopColor);
            mTextTop.setTypeface(mTextTopTypeface);
            mTextTop.setText(mTextTopString);

            mTextBottom.setVisibility(mShowBottom ? View.VISIBLE : View.INVISIBLE);
            mTextBottom.setTextColor(mTextBottomColor);
            mTextBottom.setTypeface(mTextBottomTypeface);
            mTextBottom.setText(mTextBottomString);

            mTextBottom2.setVisibility(mShowBottom2 ? View.VISIBLE : View.INVISIBLE);
            mTextBottom2.setTextColor(mTextBottom2Color);
            mTextBottom2.setTypeface(mTextBottom2Typeface);
            mTextBottom2.setText(mTextBottom2String);

            mMicImage.setImageResource(mMicImageResourceId);
            mMainContainer.setBackgroundColor(mBackgroundColor);

        }
    }

    public void setOnMicEventListener(OnMicEvent listener) {
        eventListener = listener;
    }


    public void OnMicClick(View v) {

        if (mEnabled == false) {
            return;
        }

        Date now = new Date();

        if (now.getTime() - mLastToggle < MIN_TOGGLE_TIMEOUT) {
            // Ignore to avoid lots of clicks
            return;
        }

        Log.d(TAG, "Tapped the MIC button");
        if (eventListener != null) {
            eventListener.onMicButtonClick();
        }

        if (AudioRecordManager.getInstance().isRecording()) {
            AudioRecordManager.getInstance().stopRecording();
        } else {
            AudioRecordManager.getInstance().startRecording(this);
        }

        mLastToggle = now.getTime();
    }

    public boolean isRecording() {
        return AudioRecordManager.getInstance().isRecording();
    }

    public void stopRecording() {
        if ( AudioRecordManager.getInstance().isRecording() ) {
            AudioRecordManager.getInstance().stopRecording();
        }
    }

    Handler handler = new Handler();

    final Runnable r = new Runnable() {
        public void run() {
            Log.d(TAG, "Time's up! Should not be recording more than " + MAX_RECORDING + " ms.");
            if (AudioRecordManager.getInstance().isRecording()) {
                AudioRecordManager.getInstance().stopRecording();
            }

            if (eventListener != null) {
                eventListener.onError(AudioError.MaxLengthReached);
            }
        }
    };

    @Override
    public void onStateChange(AudioState state) {

        try {
            if (state == AudioState.Recording) {
                Log.d(TAG, "Recording has started");
                setTextBottom(getString(R.string.tap_mic_stop));

                if (eventListener != null) {
                    eventListener.onRecordingStarted();
                }

                // Recording has started. Set timeout.


                handler.postDelayed(r, MAX_RECORDING);

            } else if (state == AudioState.Idle) {
                Log.d(TAG, "Recording has stopped");
                handler.removeCallbacksAndMessages(null);

                if (eventListener != null) {
                    eventListener.onRecordingStopped();
                }

                setTextBottom(getString(R.string.tap_mic_start));

            }

            if (mVoiceEffecter != null) {
                if (AudioRecordManager.getInstance().isRecording()) {
                    mVoiceEffecter.start();
                } else {
                    mVoiceEffecter.stop();
                }
            }
        } catch (Exception e) {

        }
    }

    @Override
    public void onError(AudioError error) {
        if (eventListener != null) {
            eventListener.onError(error);
        }
    }

    @Override
    public void onAudioCapture(int index, byte[] buffer, float energy) {
        //Log.d(TAG, "onAudioCapture: " + buffer.length +
        //    " bytes, energy=" + energy + ", index=" + index);
        try {
            if (mVoiceEffecter != null) {
                mVoiceEffecter.update(energy);
            }

            if ( (eventListener != null) && (index != -1) ) {
                eventListener.onAudioFrame(index, buffer, energy);
            }

        } catch (Exception e) {
             e.printStackTrace();
        }
    }

    /**
     * Set the enablement of the microphone view. If disabled, user will not be able
     * to start/stop the microphone.
     *
     * @param enabled
     */
    public void setEnabled(boolean enabled) {
        this.mEnabled = enabled;
        mMicImage.setClickable(enabled);
    }

}
