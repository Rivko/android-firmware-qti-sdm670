/* Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.ims.vt;

import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.telecom.VideoProfile.CameraCapabilities;
import android.util.Log;
import android.view.Surface;
import android.content.Context;

import com.qualcomm.ims.vt.ImsMedia.IMediaListener;

import org.codeaurora.ims.ImsCallSessionImpl;
import org.codeaurora.ims.ICallListListener;
import org.codeaurora.ims.QtiCallConstants;
import org.codeaurora.ims.R;
import org.codeaurora.ims.utils.QtiImsExtUtils;
import org.codeaurora.ims.QtiVideoCallDataUsage;

import java.util.ArrayList;
import android.telecom.VideoProfile;
import android.telecom.Connection;
import android.telecom.Connection.VideoProvider;

public class MediaController implements ImsMedia.CameraListener,
                                        ImsMedia.IMediaListener,
                                        ICallListListener {
    private static String TAG = "VideoCall_MediaController";
    private static MediaController sInstance;

    private Context mContext;
    private ImsMedia mMedia;
    private IMediaListener mMediaEventListener;
    private int mNumberOfImsCallSessions = 0;
    private PauseImageTask mPauseImageTask;
    private ArrayList<ImsCallSessionImpl> mImsCallSessionsArray;
    private boolean mIsPauseImageMode = false;

    private MediaController(Context context, ImsMedia media) {
        mContext = context;
        mMedia = media;
        mMedia.setMediaListener(this);
    }

    public static synchronized void init(Context context, ImsMedia media) {
        if (sInstance == null) {
            sInstance = new MediaController(context, media);
        } else {
            throw new RuntimeException("MediaController: Multiple initialization");
        }
    }

    public static synchronized MediaController getInstance() {
        if (sInstance != null) {
            return sInstance;
        } else {
            loge("getInstance sInstance= null");
        }
        return sInstance;
    }

    public void setMediaEventListener(IMediaListener listener) {
        mMediaEventListener = listener;
    }

    public void updateMergeStatus(int status) {
        log("updateMergeStatus status = " + status);
        mMedia.updateMergeStatus(status);
    }

    public void setDisplaySurface(Surface surface) {
        mMedia.setSurface(surface);
    }

    public void setDeviceOrientation(int rotation) {
        mMedia.sendCvoInfo(CvoUtil.toOrientation(rotation));
    }

    public void requestCallDataUsage(int mediaId) {
        log("requestCallDataUsage: mediaID = " + mediaId);
        mMedia.requestCallDataUsage(mediaId);
    }

    private class PauseImageTask extends AsyncTask<String, Void, Bitmap> {
        int width;
        int height;

        PauseImageTask(int w, int h) {
            width = w;
            height = h;
        }

        private Bitmap reconfigureAndDecodeImage(String uri) {
            Bitmap b = null;
            if (uri.isEmpty()) {
                // uri is empty, so fall back to default image
                b = QtiImsExtUtils.decodeImage(mContext.getResources(),
                        R.drawable.img_no_image, width, height);
            } else {
                b = QtiImsExtUtils.decodeImage(uri, width, height);
            }

            if (b != null) {
                b.reconfigure(width, height, b.getConfig());
                log("setPreviewImageAsync reconfigureAndDecodeImage bitmap b = " + b
                        + " width = " + b.getWidth()
                        + " height = "+ b.getHeight());
            }
            return b;
        }

        // Decode image in background.
        @Override
        protected Bitmap doInBackground(String... params) {
            String uri = params[0];

            log("setPreviewImageAsync: width = " + width +
                    " height = " + height + " uri = " + uri);

            if (isCancelled()) {
                log("setPreviewImageAsync: PauseImageTask is cancelled");
                return null;
            }

            return reconfigureAndDecodeImage(uri);
        }

        @Override
        protected void onPostExecute(Bitmap bitmap) {
            log("setPreviewImageAsync bitmap = " + bitmap);

            if (bitmap == null) {
                return;
            }

            setPauseImage(bitmap);
        }
    }

    public void setPauseImage(String uri) {
        if (mPauseImageTask != null &&
                mPauseImageTask.getStatus() != AsyncTask.Status.FINISHED) {
            boolean isCancelled = mPauseImageTask.cancel(true);
            log("setPauseImage cancelling old pauseImageTask isCancelled = " + isCancelled);
        }

        if (uri == null) {
            // uri is null meaning UE is in "Show Me" mode
            setPauseImage((Bitmap) null);
            return;
        }

        mPauseImageTask = new PauseImageTask(mMedia.getNegotiatedWidth(),
                mMedia.getNegotiatedHeight());
        mPauseImageTask.execute(uri);
    }

    private void setPauseImage(Bitmap bitmap) {
        synchronized(this) {
            boolean isPauseImageMode = mIsPauseImageMode;
            if (mMedia.setPreviewImage(bitmap)) {
                 isPauseImageMode = (bitmap != null);
            }

            log("setPauseImage isPauseImageMode = " + isPauseImageMode +
                    " mIsPauseImageMode = " + mIsPauseImageMode);

            if (mIsPauseImageMode == isPauseImageMode) {
                return;
            }

            mIsPauseImageMode = isPauseImageMode;
            if (isPauseImageMode) {
                mMedia.addCameraListener(this);
            } else {
                mMedia.removeCameraListener(this);
            }
        }
    }

    private boolean isPauseImageState() {
        synchronized(this) {
            return mIsPauseImageMode;
        }
    }

    @Override
    public void onCameraConfigChanged(int w, int h, int fps, Surface surface, int orientationMode) {
        if (!isPauseImageState()) {
            logw("received camera config changed when not in pause image mode");
            return;
        }

        ImsVideoCallProviderImpl provider =
                ImsVideoGlobals.getInstance().getActiveOrBackgroundCallVideoProvider();
        if (provider == null) {
            logw("onCameraConfigChanged: ImsVideoCallProvider is not available");
            return;
        }

        CameraCapabilities cc = new CameraCapabilities(w, h);
        log("onCameraConfigChanged in hide me mode New capabilities = " + cc);
        provider.changeCameraCapabilities(cc);
    }

    @Override
    public void onUpdateRecorderFrameRate(int rate) {
    }

    @Override
    public void onRecordingEnabled() {
    }


    @Override
    public void onRecordingDisabled() {
    }

    @Override
    public void onOrientationModeChanged(int orientationMode) {
        log("onOrientationModeChanged to orientation mode- " + orientationMode);
        ImsVideoCallProviderImpl videoProvider =
            ImsVideoGlobals.getInstance().getIncomingCallVideoProvider();
        if (videoProvider == null) {
            videoProvider = ImsVideoGlobals.getInstance().getActiveOrBackgroundCallVideoProvider();
        }
        if (videoProvider != null) {
            videoProvider.onUpdateOrientationMode(orientationMode);
        } else {
            logw("Call session video provider is null. "
                    + "Can't propagate onOrientationModeChanged event");
        }
    }

    /**
     * {@inheritDoc}
     */
    public void onPeerResolutionChanged(int width, int height) {
        log("onPeerResolutionChangeEvent width=" + width + " height=" + height);
        ImsVideoCallProviderImpl videoProvider =
                ImsVideoGlobals.getInstance().getActiveOrOutgoingCallVideoProvider();
        if (videoProvider != null) {
            videoProvider.onUpdatePeerDimensions(width, height);
        } else {
            logw("Active or Outgoing  call session video provider is null. "
                    + "Can't propagate OnPeerResolutionChanged event");
        }
    }

    /**
     * {@inheritDoc}
     */
    public void onPlayerStateChanged(int state) {
        log("onPlayerStateChanged state = " + state);

        final ImsVideoGlobals imsVideoGlobals = ImsVideoGlobals.getInstance();
        ImsVideoCallProviderImpl videoProvider =
                imsVideoGlobals.getActiveOrOutgoingCallVideoProvider();
        if (videoProvider == null) {
            videoProvider = imsVideoGlobals.getBackgroundCallVideoProvider();
            if (videoProvider == null) {
                videoProvider = imsVideoGlobals.getIncomingCallVideoProvider();
            }
        }

        if (videoProvider != null) {
            final int status = (state == ImsMedia.PLAYER_STATE_STARTED) ?
                    Connection.VideoProvider.SESSION_EVENT_RX_RESUME :
                    Connection.VideoProvider.SESSION_EVENT_RX_PAUSE;
            videoProvider.handleCallSessionEvent(status);
        } else {
            logw("All call session video providers are null."
                    + " Can't propagate onPlayerStateChanged event");
        }
    }

    /**
     * This method converts the video quality constants defined in the IMS VT documentation
     * to the ones defined in VideoProfile.
     */
    private int convertVideoQuality(int videoQuality) {
        switch(videoQuality) {
            case ImsMediaConstants.VIDEO_QUALITY_HIGH:
                return VideoProfile.QUALITY_HIGH;
            case ImsMediaConstants.VIDEO_QUALITY_MEDIUM:
                return VideoProfile.QUALITY_MEDIUM;
            case ImsMediaConstants.VIDEO_QUALITY_LOW:
                return VideoProfile.QUALITY_LOW;
            // Both unknown and default should map to unknown quality. Intentional fall through.
            case ImsMediaConstants.VIDEO_QUALITY_UNKNOWN:
            default:
                return VideoProfile.QUALITY_UNKNOWN;
        }
    }

    /**
     * {@inheritDoc}
     */
    public void onVideoQualityEvent(int videoQuality) {
        ImsVideoCallProviderImpl videoProvider =
                ImsVideoGlobals.getInstance().getActiveCallVideoProvider();
        if (videoProvider != null) {
            videoProvider.onUpdateVideoQuality(convertVideoQuality(videoQuality));
        } else {
            logw("Active call session video provider is null."
                    + " Can't propagate OnVideoQualityChanged event");
        }
    }

    /**
     * {@inheritDoc}
     */
    public void onDataUsageChanged(int mediaId, QtiVideoCallDataUsage dataUsage) {
        ImsVideoCallProviderImpl videoProvider =
                ImsVideoGlobals.getInstance().findVideoCallProviderbyMediaId(mediaId);
        if (videoProvider != null) {
            log("onDataUsageChanged dataUsage = " + dataUsage);
            videoProvider.onCallDataUsageChanged(dataUsage.getLteDataUsage());
            ImsCallSessionImpl callSession = videoProvider.getCallSession();
            if (callSession != null) {
                callSession.updateVideoCallDataUsageInfo(dataUsage);
            } else {
                loge("onDataUsageChanged: call session is null");
            }
        } else {
            loge("onDataUsageChanged: Call session video provider is null."
                    + " Received mediaId = " + mediaId);
        }
    }

    @Override
    public void onCallSessionAdded(ImsCallSessionImpl callSession) {
        log("onCallSessionAdded callSession=" + callSession);
        if (mNumberOfImsCallSessions == 0) {
            mMedia.init();
        }
        mNumberOfImsCallSessions++;
    }

    @Override
    public void onCallSessionRemoved(ImsCallSessionImpl callSession) {
        log("onCallSessionRemoved callSession=" + callSession);
        if (mNumberOfImsCallSessions == 0) {
            loge("onCallSessionRemoved: Unknown session has been removed, Session=" + callSession);
            return;
        }
        mNumberOfImsCallSessions--;
        if (mNumberOfImsCallSessions == 0) {
            if (QtiImsExtUtils.shallTransmitStaticImage(callSession.getPhoneId(), mContext)) {
                /* If Video call is remotely ended, deInit will be called immediately and
                   further request to setPauseImage to NULL from InCallUI will be ignored.
                   To handle such use-cases, setPauseImage to NULL here */
                setPauseImage((Bitmap) null);
            }
            mMedia.deInit();
        }
    }

    private static void log(String msg) {
        Log.d(TAG, msg);
    }

    private static void loge(String msg) {
        Log.e(TAG, msg);
    }

    private static void logw(String msg) {
        Log.w(TAG, msg);
    }
}
