/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*
* Not a Contribution.
* Apache license notifications and license are retained
* for attribution purposes only.
 */

/*
* Copyright (C) 2014 The Android Open Source Project
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


package qces.qualcomm.qti.com.videosummary.VideoPlayer;

import android.content.Context;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

import qces.qualcomm.qti.com.videosummary.R;

public class VideoControllerView extends FrameLayout {
    private static final String TAG = "VideoControllerView";

    private MediaPlayerControl mPlayer;
    private Context mContext;
    private ViewGroup mAnchor;
    private View mRoot;
    private ProgressBar mProgress;
    private boolean mDragging;
    private ImageButton pauseBtn;
    private ImageButton fullscreenBtn;

    public VideoControllerView(Context context) {
        super(context);
        mRoot = null;
        mContext = context;

        Log.i(TAG, TAG);
    }

    @Override
    public void onFinishInflate() {
        super.onFinishInflate();
        if (mRoot != null)
            initControllerView(mRoot);
    }

    public void setMediaPlayer(MediaPlayerControl player) {
        mPlayer = player;
        updatePausePlay();
    }

    public void setAnchorView(ViewGroup view) {
        mAnchor = view;

        LayoutParams frameParams = new LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        );

        removeAllViews();
        View v = makeControllerView();
        addView(v, frameParams);
    }

    protected View makeControllerView() {
        LayoutInflater inflate = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mRoot = inflate.inflate(R.layout.media_controller, null);

        initControllerView(mRoot);

        return mRoot;
    }

    private void initControllerView(View v) {

        pauseBtn = (ImageButton) v.findViewById(R.id.pause);
        if (pauseBtn != null) {
            pauseBtn.requestFocus();
            pauseBtn.setOnClickListener(mPauseListener);
        }

        fullscreenBtn = (ImageButton) v.findViewById(R.id.fullscreen);
        if (fullscreenBtn != null) {
            fullscreenBtn.requestFocus();
            fullscreenBtn.setOnClickListener(mFullscreenListener);
        }

        mProgress = (SeekBar) v.findViewById(R.id.mediacontroller_seekbar);
        if (mProgress != null) {
            if (mProgress instanceof SeekBar) {
                SeekBar seeker = (SeekBar) mProgress;
                seeker.setOnSeekBarChangeListener(mSeekListener);
            }
            mProgress.setMax(1000);
        }

        if (mAnchor != null) {
            setProgress();
            if (pauseBtn != null) {
                pauseBtn.requestFocus();
            }

            FrameLayout.LayoutParams tlp = new FrameLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT,
                    Gravity.BOTTOM
            );
            mAnchor.removeView(this);
            mAnchor.addView(this, tlp);

        }

    }

    public int setProgress() {
        if (mPlayer == null || mDragging) {
            return 0;
        }

        int position = mPlayer.getCurrentPosition();
        int duration = mPlayer.getDuration();
        if (mProgress != null) {
            if (duration > 0) {
                long pos = 1000L * position / duration;
                mProgress.setProgress((int) pos);

                updatePausePlay();
            }
        }

        return position;
    }

    private OnClickListener mPauseListener = new OnClickListener() {
        public void onClick(View v) {
            doPauseResume();
        }
    };

    private OnClickListener mFullscreenListener = new OnClickListener() {
        public void onClick(View v) {
            doToggleFullscreen();
            updateFullScreen();
        }
    };

    public void updatePausePlay() {
        if (mRoot == null || pauseBtn == null || mPlayer == null) {
            return;
        }

        if (mPlayer.isPlaying()) {
            pauseBtn.setImageResource(R.drawable.ic_pause);
        } else {
            pauseBtn.setImageResource(R.drawable.ic_play);
        }
    }

    public void updateFullScreen() {
        if (mRoot == null || fullscreenBtn == null || mPlayer == null) {
            return;
        }

        if (mPlayer.isFullScreen()) {
            fullscreenBtn.setImageResource(R.drawable.ic_fullscreen_arrows_exit);
        } else {
            fullscreenBtn.setImageResource(R.drawable.ic_fullscreen_arrows);
        }
    }

    public void showFullScreenBtn(boolean show) {
        if (show)
            fullscreenBtn.setVisibility(View.VISIBLE);
        else
            fullscreenBtn.setVisibility(View.GONE);
    }

    private void doPauseResume() {
        if (mPlayer == null) {
            return;
        }

        if (mPlayer.isPlaying()) {
            mPlayer.pause();
        } else {
            mPlayer.start();
        }
        setProgress();
        updatePausePlay();
    }

    private void doToggleFullscreen() {
        if (mPlayer == null) {
            return;
        }

        mPlayer.toggleFullScreen();
    }

    private OnSeekBarChangeListener mSeekListener = new OnSeekBarChangeListener() {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (mPlayer == null) {
                return;
            }

            if (!fromUser) {
                return;
            }

            long duration = mPlayer.getDuration();
            long newposition = (duration * progress) / 1000L;
            mPlayer.seekTo((int) newposition);
        }

        public void onStartTrackingTouch(SeekBar bar) {

            mDragging = true;

        }

        public void onStopTrackingTouch(SeekBar bar) {
            mDragging = false;
            setProgress();
            //mPlayer.pause();
            updatePausePlay();

        }
    };

    void onStop() {
       mPlayer = null;
    }

    @Override
    public void setEnabled(boolean enabled) {
        if (pauseBtn != null) {
            pauseBtn.setEnabled(enabled);
        }
        if (mProgress != null) {
            mProgress.setEnabled(enabled);
        }
        super.setEnabled(enabled);
    }

    public interface MediaPlayerControl {
        void start();

        void pause();

        int getDuration();

        int getCurrentPosition();

        void seekTo(int pos);

        boolean isPlaying();

        boolean isFullScreen();

        void toggleFullScreen();
    }


}
