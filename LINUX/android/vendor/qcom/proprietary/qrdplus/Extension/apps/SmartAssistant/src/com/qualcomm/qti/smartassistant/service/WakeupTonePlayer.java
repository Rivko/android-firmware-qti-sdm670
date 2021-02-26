/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.service;

import android.content.Context;
import android.media.MediaPlayer;
import android.os.Message;
import android.util.Log;
import android.os.Handler;
import com.qualcomm.qti.smartassistant.R;
import com.qualcomm.qti.smartassistant.utils.LogUtils;

public class WakeupTonePlayer {

    private static final String TAG = "WakeupTonePlayer";
    private final Context mContext;
    private static final int WARNING_TONE_RES_ID = R.raw.succeed;
    private static final int MSG_PLAY_TONE_FNISH = 1000;

    private MediaPlayer mMediaPlayer;
    private OnCompletionListener mComplistener;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case MSG_PLAY_TONE_FNISH:
                    if(mMediaPlayer != null){
                        mMediaPlayer.stop();
                        mMediaPlayer.release();
                    }
                    if(mComplistener != null){
                        mComplistener.onCompletion();
                    }
            }
        }
    };

    public WakeupTonePlayer(Context context){
        mContext = context;
        mMediaPlayer = MediaPlayer.create(mContext, WARNING_TONE_RES_ID);
    }

   public void play( final OnCompletionListener listener) {
        mComplistener = listener;
        mMediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
            @Override
            public boolean onError(MediaPlayer mp, int what, int extra) {
                LogUtils.d(TAG, "onError ");
                mHandler.removeMessages(MSG_PLAY_TONE_FNISH);
                mHandler.sendEmptyMessage(MSG_PLAY_TONE_FNISH);
                return false;
            }
        });
        mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                LogUtils.d(TAG, "onCompletion ");
                mHandler.removeMessages(MSG_PLAY_TONE_FNISH);
                mHandler.sendEmptyMessage(MSG_PLAY_TONE_FNISH);
            }
        });
        mMediaPlayer.start();
        mHandler.removeMessages(MSG_PLAY_TONE_FNISH);
        mHandler.sendEmptyMessageDelayed(MSG_PLAY_TONE_FNISH, 500);
    }

    public interface OnCompletionListener{
        void onCompletion();
    }
}
