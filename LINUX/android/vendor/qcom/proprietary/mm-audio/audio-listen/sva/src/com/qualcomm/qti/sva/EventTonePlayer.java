/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import android.content.Context;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.Log;


public class EventTonePlayer implements SoundPool.OnLoadCompleteListener {
    private static final String TAG = "ListenLog.EventTonePlayer";
    private Context mContext;
    private SoundPool mSoundPool;
    private static final int MAX_STREAMS = 8;
    private static final int[]  RES_IDs = {
        R.raw.succeed,
        R.raw.fail_m
    };
    private final int[] mSoundIDs;
    private final boolean[] mSoundIDReady;

    public EventTonePlayer(Context context) {
        mContext = context;
        mSoundPool = new SoundPool(MAX_STREAMS, AudioManager.STREAM_NOTIFICATION, 0);
        mSoundPool.setOnLoadCompleteListener(this);
        mSoundIDs = new int[RES_IDs.length];
        mSoundIDReady = new boolean[RES_IDs.length];
        for (int i = 0; i < RES_IDs.length; i++) {
            mSoundIDs[i] = mSoundPool.load(mContext, RES_IDs[i], 1);
            mSoundIDReady[i] = false;
        }
    }

    @Override
    public void onLoadComplete(SoundPool pool, int soundID, int status) {
        if (status != 0) {
            Log.e(TAG, "loading sound tracks failed for soundID" + soundID + "status= " + status );
            return;
        }
        for (int i = 0; i < mSoundIDs.length; i++ ) {
            if (mSoundIDs[i] == soundID) {
                mSoundIDReady[i] = true;
                break;
            }
        }
    }

    public void play(int event_status) {
        if (event_status < 0 || event_status >= RES_IDs.length) {
            Log.e(TAG, "Resource ID not found for event_status:" + event_status);
            return;
        }
        if (!mSoundIDReady[event_status]) {
            Log.e(TAG,"SoundID: " + mSoundIDs[event_status] + "not loaded yet");
            return;
        }

        mSoundPool.play(mSoundIDs[event_status], 0.5f, 0.5f, 0, 0, 1f);
    }

    public void release() {
        if (mSoundPool != null) {
            mSoundPool.release();
            mSoundPool = null;
        }
    }
}
