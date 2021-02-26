/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.telephonyservice;

import android.content.Context;
import android.media.AudioSystem;
import android.media.AudioManager;
import android.media.AudioPort;
import android.media.AudioPatch;
import android.os.IBinder;
import android.os.HwBinder;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import java.util.concurrent.atomic.AtomicLong;
import vendor.qti.hardware.radio.am.V1_0.AudioError;
import vendor.qti.hardware.radio.am.V1_0.IQcRilAudio;
import vendor.qti.hardware.radio.am.V1_0.IQcRilAudioCallback;

import android.hidl.manager.V1_0.IServiceManager;
import android.hidl.manager.V1_0.IServiceNotification;

public class QcRilAudio {
    final static String TAG = "QcRilAudio";

    private final String mInstanceName;
    private final AtomicLong mRilAudioCookie = new AtomicLong(0);

    private final AudioProxyDeathRecipient mRilAudioDeathRecipient = new AudioProxyDeathRecipient();
    private final ServiceNotification mServiceNotification = new ServiceNotification();
    private final IQcRilAudioCallback mQcRilAudioCallback = new QcRilAudioCallback();
    private IQcRilAudio mRilAudio;
    private boolean mIsDisposed = false;
    private Context mContext = null;
    private AudioManager mAudioManager = null;
    private AudioPortUpdateListener mListener = null;
    private int mError = AudioError.AUDIO_STATUS_OK;

    private static final int EVENT_AUDIO_SERVICE_DIED = 0;
    private static final int mDelay = 1000;

    private class QcRilAudioCallback extends IQcRilAudioCallback.Stub {
        @Override
        public int setParameters(String keyValuePairs) {
            Log.d(TAG, "setParameters with: " + keyValuePairs);
            int result = AudioSystem.setParameters(keyValuePairs);
            Log.d(TAG, "setParameters returned with: " + result);
            return result;
        }

        @Override
        public String getParameters(String key) {
            return AudioSystem.getParameters(key);
        }
    }

    private class AudioPortUpdateListener implements AudioManager.OnAudioPortUpdateListener {
        @Override
        public void onAudioPortListUpdate(AudioPort[] portList) {}

        @Override
        public void onAudioPatchListUpdate(AudioPatch[] patchList) {}

        @Override
        public void onServiceDied() {
            if (isDisposed()) {
                Log.d(TAG, "AudioPortUpdateListener onServiceDied: Ignoring.");
                dump("AudioPortUpdateListener onServiceDied");
                return;
            }
            Log.d(TAG, "Reporting AudioServer died to mInstance: " + mInstanceName);
            onAudioServiceDied();
        }
    }


    private void onAudioServiceDied() {
        int error = convertToAudioErrorType(AudioSystem.checkAudioFlinger());

        if (error != mError) {
            mError = error;
            setError(mError);
        }

        if (error != AudioError.AUDIO_STATUS_OK) {
            Message msg = mQcRilAudioHandler.obtainMessage(EVENT_AUDIO_SERVICE_DIED);
            mQcRilAudioHandler.sendMessageDelayed(msg, mDelay);
            return;
        }

        Log.d(TAG, "Reporting AudioServer started to mInstance: " + mInstanceName);
        unregisterAudioPortListener();
        registerAudioPortListener();
    }

    private Handler mQcRilAudioHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_AUDIO_SERVICE_DIED:
                    onAudioServiceDied();
                    break;
                default:
                    Log.d(TAG, msg.what + " not supported.");
                    break;
            }
        }

    };

    private class ServiceNotification extends IServiceNotification.Stub {
        @Override
        public void onRegistration(String fqName, String name, boolean preexisting) {
            Log.d(TAG, "onRegistration: fqName=" + fqName + " name=" + name);

            if (!mInstanceName.equals(name) || isDisposed()) {
                Log.d(TAG, "onRegistration: Ignoring.");
                dump("onRegistration");
                return;
            }

            initHal();
            setError(convertToAudioErrorType(AudioSystem.checkAudioFlinger()));
        }
    }

    private class AudioProxyDeathRecipient implements HwBinder.DeathRecipient {
        @Override
        public void serviceDied(long cookie) {
            if (isDisposed()) {
                Log.d(TAG, "serviceDied: Ignoring.");
                dump("serviceDied");
                return;
            }

            final long current = mRilAudioCookie.get();
            if (cookie != current) {
                Log.v(TAG, "serviceDied: Ignoring. provided=" + cookie + " expected=" + current);
                return;
            }

            Log.e(TAG, "IQcRilAudio service died");
            resetService();
        }

    }

    public QcRilAudio(int slotId, Context context) {
        mContext = context;
        mInstanceName = "slot" + slotId;

        try {
            boolean ret = IServiceManager.getService()
                    .registerForNotifications("vendor.qti.hardware.radio.am@1.0::IQcRilAudio",
                    mInstanceName, mServiceNotification);
            if (!ret) {
                Log.e(TAG, "Unable to register service start notification: ret = " + ret);
            }
        } catch (RemoteException ex) {
            Log.e(TAG, "Unable to register service start notification");
        }

        registerAudioPortListener();
    }

    public synchronized void dispose() {
        if (isDisposed()) {
            return;
        }
        mIsDisposed = true;

        try {
            if (mRilAudio != null) {
                mRilAudio.unlinkToDeath(mRilAudioDeathRecipient);
                mRilAudio = null;
            }
        } catch(RemoteException e) {
            Log.d(TAG, "dispose: Exception=" + e );
        }
        unregisterAudioPortListener();
    }

    public boolean isDisposed() {
        return mIsDisposed;
    }

    private synchronized void initHal() {
        Log.d(TAG, "initHal");

        try {
            mRilAudio = IQcRilAudio.getService(mInstanceName);
            if (mRilAudio == null) {
                Log.e(TAG, "initHal: mRilAudio == null");
                return;
            }

            mRilAudio.linkToDeath(mRilAudioDeathRecipient,
                    mRilAudioCookie.incrementAndGet());
            mRilAudio.setCallback(mQcRilAudioCallback);

        } catch (Exception e) {
                Log.e(TAG, "initHal: Exception: " + e);
        }
    }

    private synchronized void setError(int errorcode) {
        if (mRilAudio == null) {
            Log.w(TAG, "setError - mRilAudio is null, returning.");
            return;
        }

        try {
            mRilAudio.setError(errorcode);
        } catch (Exception e) {
            Log.e(TAG, "setError request to IQcRilAudio. Exception: " + e);
        }
    }

    private synchronized void registerAudioPortListener() {
        mAudioManager = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        if (mAudioManager != null && mListener == null) {
            mListener = new AudioPortUpdateListener();
            mAudioManager.registerAudioPortUpdateListener(mListener);
        }
    }

    private synchronized void unregisterAudioPortListener() {
        if (mListener != null && mAudioManager != null) {
            mAudioManager.unregisterAudioPortUpdateListener(mListener);
            mListener = null;
            mAudioManager = null;
        }
    }

    private synchronized int convertToAudioErrorType(int error) {
        switch (error) {
            case AudioSystem.AUDIO_STATUS_SERVER_DIED:
                error = AudioError.AUDIO_STATUS_SERVER_DIED;
                break;
            case AudioSystem.AUDIO_STATUS_OK:
                error = AudioError.AUDIO_STATUS_OK;
                break;
            default:
                error = AudioError.AUDIO_GENERIC_FAILURE;
                break;
        }

        return error;
    }

    private synchronized void resetService() {
        mRilAudio = null;
    }

    private void dump(String fn) {
        Log.d(TAG, fn + ": InstanceName=" + mInstanceName);
        Log.d(TAG, fn + ": isDisposed=" + isDisposed());
    }
}
