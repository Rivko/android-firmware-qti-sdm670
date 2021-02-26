/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.callenhancement;

import android.content.Context;
import android.content.Intent;
import android.media.MediaRecorder;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.StatFs;
import android.os.SystemClock;
import android.telecom.ParcelableCall;
import android.util.Log;
import android.widget.Toast;


import com.qualcomm.qti.qtisystemservice.ICallStateListener;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Collections;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import org.codeaurora.callenhancement.ICallRecordStateListener;

public class CallRecorder extends CallSateObserver implements IBinder.DeathRecipient{
    private static final String TAG = CallRecorder.class.getSimpleName();
    private static final long LOW_STORAGE_THRESHOLD = 50000000;
    private static final String RECORD_FILE_SUBFIX = ".amr";
    private static String sCallRecordPath = Environment.getExternalStorageDirectory()
            + File.separator + "CallRecord";
    private static final String ACTION_RECORD_STATE_CHANGED =
            "com.qualcomm.qti.CallRecorder.RECORD_STATE_CHANGED";
    private final Set<ICallRecordStateListener> mListeners = Collections.newSetFromMap(
            new ConcurrentHashMap<ICallRecordStateListener, Boolean>(8, 0.9f, 1));

    private static final int EVENT_CALL_STATE_CHANGED = 0;
    private static final int EVENT_START = 1;
    private static final int EVENT_STOP = 2;
    private static final int EVENT_PAUSE = 3;

    private static CallRecorder mInstance;
    private final Context mContext;
    private File mTarget;
    private MediaRecorder mMediaRecorder;
    private long mStartTimeMillis;

    private CallRecorder(Context context, TelecomCallServiceDelegate telCallServiceDelegate) {
        super(telCallServiceDelegate);
        mContext = context;
        if (mContext.getResources().getBoolean(R.bool.def_save_name_prefix_enabled)) {
            String callRecordSavePath = mContext.getResources()
                    .getString(R.string.def_call_record_sub_dir);
            sCallRecordPath = Environment.getExternalStorageDirectory()
                    + File.separator + callRecordSavePath;
        }
    }

    public static CallRecorder setup(Context context, TelecomCallServiceDelegate telCallServDelegate) {
        mInstance = new CallRecorder(context.getApplicationContext(), telCallServDelegate);
        return mInstance;
    }

    public static CallRecorder getInstance() {
        if(mInstance == null) throw new RuntimeException("CallRecorder is not set up");
        return mInstance;
    }

    private MediaRecorder.OnInfoListener mOnInfoListener = new MediaRecorder.OnInfoListener() {
        @Override
        public void onInfo(MediaRecorder mr, int what, int extra) {
            if (what == MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED) {
                mHandler.obtainMessage(EVENT_STOP).sendToTarget();
            }
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_CALL_STATE_CHANGED:
                    if (!mTelecomCallServiceDelegate.hasAnyActiveCall()) {
                        QLog.d(this, "EVENT_CALL_STATE_CHANGED stop");
                        stopRecordingCall(false);
                    } else if (mTelecomCallServiceDelegate.hasAnyHeldCall()) {
                        QLog.d(this, "EVENT_CALL_STATE_CHANGED pause");
                        pause();
                    } else if (mTelecomCallServiceDelegate.hasAnyActiveCall()
                            && mMediaRecorder != null) {
                        QLog.d(this, "EVENT_CALL_STATE_CHANGED resume");
                        resume();
                    }
                    break;
                case EVENT_STOP:
                    stopRecordingCall(false);
                    break;
            }
        }
    };

    private void notifyRecordStateChanged() {
        QLog.d(this, "notify record state changed, " + canCallRecord());
        mContext.sendBroadcast(new Intent(ACTION_RECORD_STATE_CHANGED));
        Iterator<ICallRecordStateListener> it = mListeners.iterator();
        while (it.hasNext()) {
            ICallRecordStateListener listener = it.next();
            try {
                listener.onCallRecordStateChanged(isCallRecording() ? 1 : 0);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }

    private boolean isStorageFull() {
        StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getAbsolutePath());
        long leftSize = (long) stat.getBlockSizeLong() * stat.getAvailableBlocksLong();
        QLog.d(this, "external storage left size:" + leftSize);
        return leftSize < LOW_STORAGE_THRESHOLD;
    }

    private long getAvailableSpace() {
        StatFs stat = new StatFs(Environment.getExternalStorageDirectory().getAbsolutePath());
        return stat.getBlockSizeLong() * stat.getAvailableBlocksLong();
    }

    private File createFile() {
        Calendar c = Calendar.getInstance();
        SimpleDateFormat df = new SimpleDateFormat("yyyyMMdd_HHmmss");
        String name = df.format(c.getTime());
        File parent = new File(sCallRecordPath);
        if (!parent.exists() && !parent.mkdirs()) {
            return null;
        }
        int i = 1;
        File f = new File(sCallRecordPath, name + RECORD_FILE_SUBFIX);
        try {
            while (!f.createNewFile()) {
                f = new File(sCallRecordPath, name + "(" + i++ + ")" + RECORD_FILE_SUBFIX);
            }
            return f;
        } catch (IOException e) {
        }
        return null;
    }

    private int previousCheck() {
        int errorId = 0;
        if (mMediaRecorder != null) {
            errorId = R.string.message_record_started;
        } else if (!canCallRecord()) {
            errorId = R.string.message_not_incall;
        } else if (!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            errorId = R.string.message_no_sdcard;
        } else if (isStorageFull()) {
            errorId = R.string.message_sdcard_full;
        } else if ((mTarget = createFile()) == null) {
            errorId = R.string.message_internal_error;
        }
        return errorId;
    }

    public void startRecordingCall() {
        int errorMsgId = previousCheck();
        if (errorMsgId != 0) {
            Toast.makeText(mContext, errorMsgId, Toast.LENGTH_SHORT).show();
            return;
        }
        long maxFileSize = getAvailableSpace() - LOW_STORAGE_THRESHOLD;
        mMediaRecorder = new MediaRecorder();
        try {
            mMediaRecorder.setMaxFileSize(maxFileSize);
            mMediaRecorder.setOnInfoListener(mOnInfoListener);
            mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.VOICE_CALL);
            mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.RAW_AMR);
            mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
            mMediaRecorder.setOutputFile(mTarget.getAbsolutePath());
            mMediaRecorder.prepare();
            mMediaRecorder.start();
            mStartTimeMillis = SystemClock.elapsedRealtime();
            notifyRecordStateChanged();
        } catch (Exception e) {
            QLog.e(this, e, "error when stop recording!");
            stopRecordingCall(true);
        }

    }

    public void stopRecordingCall(boolean deleteFile) {
        if (mMediaRecorder == null)
            return;
        try {
            mMediaRecorder.stop();
        } catch (IllegalStateException e) {
            QLog.d(this, "error when stop recording!");
        }
        mMediaRecorder.reset();
        mMediaRecorder.release();
        if (!deleteFile) {
            Toast.makeText(mContext,
                    mContext.getString(R.string.message_success, mTarget.getAbsoluteFile()),
                    Toast.LENGTH_SHORT).show();
        } else {
            mTarget.delete();
        }
        Uri uri = Uri.fromFile(mTarget);
        mContext.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
        mTarget = null;
        mMediaRecorder = null;
        notifyRecordStateChanged();
    }

    private void pause() {
        if (mMediaRecorder != null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                mMediaRecorder.pause();
            }
        }
    }

    private void resume() {
        if (mMediaRecorder != null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                mMediaRecorder.resume();
            }
        }
    }

    public long getRecordedCallDuration() {
        if (isCallRecording()) {
            return SystemClock.elapsedRealtime() - mStartTimeMillis;
        }
        return 0;
    }

    public boolean isCallRecording() {
        return mMediaRecorder != null;
    }

    public boolean canCallRecord() {
        return mTelecomCallServiceDelegate.hasAnyActiveCall();
    }

    @Override
    protected void onCallSateChanged(ParcelableCall call, int newState, int oldState) {
        mHandler.obtainMessage(EVENT_CALL_STATE_CHANGED).sendToTarget();
    }

    @Override
    public void binderDied() {
        Iterator<ICallRecordStateListener> it = mListeners.iterator();
        while (it.hasNext()) {
            ICallRecordStateListener listener = it.next();
            if (!listener.asBinder().isBinderAlive()) {
                it.remove();
                listener.asBinder().unlinkToDeath(this, 0);
            }
        }
    }

    public void registerListener(ICallRecordStateListener listener) {
        for (ICallRecordStateListener it : mListeners) {
            if (it.asBinder() == listener.asBinder()) {
                return;
            }
        }
        if (mListeners.add(listener)) {
            try {
                listener.asBinder().linkToDeath(this, 0);
            } catch (RemoteException e) {
                mListeners.remove(listener);
            }
        }

    }

    public void unregisterListener(ICallRecordStateListener listener) {
        if (listener != null) {
            for (ICallRecordStateListener it : mListeners) {
                if (it.asBinder() == listener.asBinder() && mListeners.remove(listener)) {
                    listener.asBinder().unlinkToDeath(this, 0);
                    break;
                }
            }
        }
    }
}
