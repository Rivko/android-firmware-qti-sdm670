/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercapture.hardware;

import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicReference;

import android.graphics.RectF;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import com.qualcomm.qti.clevercapture.data.AppError;
import com.qualcomm.qti.clevercapture.hardware.CleverCaptureActor.EventListener;
import com.qualcomm.qti.clevercaptureui.CameraPreviewView;
import com.qualcomm.qti.clevercaptureui.CameraPreviewView.OutputFrame;
import com.qualcomm.qti.clevercaptureui.MainActivity;
import com.qualcomm.qti.clevercaptureui.preferences.CameraPreferenceManager;
import com.qualcomm.qti.clevercaptureui.utils.LogFileDump;

public class CleverCaptureEngine {
    private static final String LOG_TAG = "CleverCaptureEngine";
    private final CameraActor cameraActor;
    private final CameraPreviewView cameraPreviewView;
    private final Handler mainHandler;

    private HandlerThread engineThread;
    private Handler engineHandler;

    private enum State { IDLE, CAPTURING, COMPLETING, ERROR }
    private State state;

    private CleverCaptureActor.EventListener listener;

    private RectF mRoiRect = new RectF(0f, 0f, 1f, 1f);
    private Vector<Integer> mRoiIndexs = new Vector<Integer>();
    private Vector<Integer> mFrameIdsCurRoi;
    private HashMap<Integer, Vector<Integer>> mFrameIdsPerRoiIndex = new HashMap<Integer, Vector<Integer>>();
    private Vector<Integer> mPostProcessingScores = new Vector<Integer>();
    private int mRoiIndex = -1; // invalid roi index -1

    //private Integer mGlobalFrameId = 0;
    private int mFrameId;
    private int mRoiX;
    private int mRoiY;
    private int mRoiW;
    private int mRoiH;
    private int mRoiStartId;
    private LogFileDump mLogFileDump;

    private Object mLock = new Object();

    static {
        System.loadLibrary("clevercapture");
    }

    public CleverCaptureEngine(CameraActor cameraActor, CameraPreviewView view) {
        if (cameraActor == null) {
            throw new IllegalArgumentException("cameraActor cannot be null");
        } else if (view == null) {
            throw new IllegalArgumentException("view cannot be null");
        }

        this.cameraActor = cameraActor;
        this.cameraPreviewView = view;

        mainHandler = new Handler(Looper.getMainLooper());
    }

    private final Runnable doInit = new Runnable() {
        @Override
        public void run() {
            Log.d(LOG_TAG, "doInit run");
            state = State.IDLE;
        }
    };

    private final Runnable doRelease = new Runnable() {
        @Override
        public void run() {
            Log.d(LOG_TAG, "doRelease run");
        }
    };

    private abstract static class EventListenerRunnable implements Runnable {
        final CleverCaptureActor.EventListener localListener;

        public EventListenerRunnable(CleverCaptureActor.EventListener listener) {
            localListener = listener;
        }
    }

 // N.B. May only be called in the context of the engine thread
    private void handleError(final AppError error, boolean requestTerminateCapture) {
        state = State.ERROR;
        Log.d(LOG_TAG, "handleError error=" + error);

        mainHandler.post(new EventListenerRunnable(listener) {
            @Override
            public void run() {
                localListener.onError(error);
            }
        });

        if (requestTerminateCapture) {
            if (error.isImageRecoverable()) {
                engineHandler.post(doCompleteCapture);
            } else {
                engineHandler.post(doCancelCapture);
            }
        }
    }

    // N.B. May only be called in the context of the engine thread
    private void handleError(AppError error) {
        handleError(error, true);
    }

    public void setLogObject(LogFileDump dump) {
        mLogFileDump = dump;
    }

    private class DoPreviewFrame implements Runnable {
        final AtomicReference<OutputFrame> frameRef = new AtomicReference<OutputFrame>();
      //final AtomicReference<Integer> frameIdRef = new AtomicReference<Integer>();

        @Override
        public void run() {
            final OutputFrame frame = frameRef.get();
          //final Integer frameId = frameIdRef.get();

            if (frame == null) {
                throw new RuntimeException("Expected a preview frame, but none found");
            }

            if (state != State.CAPTURING) {
                frame.sem.release();
                frameRef.set(null);
                return;
            }

            final CleverCaptureScore score = new CleverCaptureScore();
            int result = 0;
            Log.d(LOG_TAG, "keypoint before nProcess");
                synchronized(mLock) {
                    if(mRoiIndex != -1) {
                    result = nProcess(frame.buffer, frame.width, frame.height, score);
                    mFrameId = score.frameId;

                    Log.d(LOG_TAG, "keypoint nProcess result=" + result + " isInteresting="
                            + score.isInteresting + " frameId=" + score.frameId + " mRoiIndex=" + mRoiIndex);
                    if(mLogFileDump != null && score.isInteresting) {
                        mLogFileDump.getInterestingFramesId().add(score.frameId);
                    }
                }
            }


            if (AppError.shouldTerminateCapture(result)) {
                final AppError error;
                switch (result) {
                default:
                    error = AppError.CLEVERCAPTURE_API_FAILED;
                    Log.e(LOG_TAG, "Failed processing preview frame, error=" + result);
                    break;
                }

                handleError(error);
            } else {
                if(score.isInteresting) {
                    if(cameraActor.isFocusing()) {
                        Log.e(LOG_TAG, "Skip this time capture because camera is focusing...");
                        if(mLogFileDump != null) {
                            mLogFileDump.getSkipReason().add(LogFileDump.SKIP_FOCUSING);
                        }
                    } else {
                    mainHandler.post(new EventListenerRunnable(listener) {
                        @Override
                        public void run() {
                            localListener.onPicture(score.frameId, score.reserved);
                        }
                    });
                }
            }
            }

            frame.sem.release();
            frameRef.set(null);
        }
    }
    private final DoPreviewFrame doPreviewFrame = new DoPreviewFrame();

    private final Runnable doCompleteCapture = new Runnable() {
        @Override
        public void run() {
            if (state == State.IDLE) {
                Log.w(LOG_TAG, "CleverCaptureEngine is not capturing, ignoring complete capture");
                return;
            }

            if(mRoiIndex != -1) {
                unregROI(mRoiIndex);
                mRoiIndex = -1;
            }

            cameraPreviewView.setOnFrameAvailableListener(null);

            // N.B. If we get here, we either succeeded, or the error was recoverable
            nRelease();

            Log.i(LOG_TAG, "clevercapture capture completed");

            mainHandler.post(new EventListenerRunnable(listener) {
                @Override
                public void run() {
                    localListener.onCaptureCompleted();
                }
            });

            state = State.IDLE;
            listener = null;
        }
    };

    private final Runnable doCancelCapture = new Runnable() {
        @Override
        public void run() {
            if (state == State.IDLE) {
                Log.w(LOG_TAG, "CleverCaptureEngine is not capturing, ignoring cancel capture");
                return;
            }

            cameraPreviewView.setOnFrameAvailableListener(null);

            Log.v(LOG_TAG, "CleverCapture capture cancelled");

            mainHandler.post(new EventListenerRunnable(listener) {
                @Override
                public void run() {
                    localListener.onCaptureCancelled();
                }
            });

            state = State.IDLE;
            listener = null;
        }
    };

    private final CameraPreviewView.OnFrameAvailableListener previewFrameListener =
            new CameraPreviewView.OnFrameAvailableListener() {
        @Override
        public boolean onFrameAvailable(OutputFrame frame) {
            boolean consumed = doPreviewFrame.frameRef.compareAndSet(null, frame);

            //mGlobalFrameId++;
            if (consumed) {
                Log.d(LOG_TAG, "frame is not ignored");
                //doPreviewFrame.frameIdRef.set(mGlobalFrameId);
                if (!engineHandler.post(doPreviewFrame)) {
                    consumed = false;
                    doPreviewFrame.frameRef.set(null);
                }
            } else {
                //if(mLogFileDump != null) {
                //    mLogFileDump.getFrameStatus().add(mGlobalFrameId, 0);
                //}
                Log.d(LOG_TAG, "frame is ignored");
            }

            return consumed;
        }
    };

    private class DoStartCaptureRunnable extends EventListenerRunnable {
        public DoStartCaptureRunnable(CleverCaptureActor.EventListener listener) {
            super(listener);
        }

        @Override
        public void run() {
            if (state != State.IDLE) {
                Log.w(LOG_TAG, "CleverCaptureEngine is already capturing, ignoring start capture");
                return;
            }

            if (0 != nInit(cameraActor.getWidth(), cameraActor.getHeight(), cameraActor.getBurstNum(),
                    CameraPreferenceManager.getScveMode(cameraActor.parentActivity))) {
                Log.e(LOG_TAG, "nInit fail when start capture");

                handleError(AppError.CLEVERCAPTURE_API_FAILED);
            } else {
                state = State.CAPTURING;
                mRoiIndexs.clear();
                mFrameIdsPerRoiIndex.clear();
                mPostProcessingScores.clear();

                regRoi(mRoiRect);
                CleverCaptureEngine.this.listener = localListener;
                cameraPreviewView.setOnFrameAvailableListener(previewFrameListener);

                Log.i(LOG_TAG, "clevercapture capture started");
                mainHandler.post(new EventListenerRunnable(listener) {
                    @Override
                    public void run() {
                        localListener.onCaptureStarted();
                    }
                });
            }
        }
    }

    public void init() {
        if (engineThread != null) {
            throw new RuntimeException("CleverCaptureEngine is already initialized");
        }

        engineThread = new HandlerThread(LOG_TAG);
        engineThread.start();

        engineHandler = new Handler(engineThread.getLooper());

        engineHandler.post(doInit);
    }
    public void cancelCapture() {
        if (engineThread == null) {
            throw new RuntimeException("CleverCaptureEngine is not initialized");
        }

        engineHandler.post(doCancelCapture);
    }
    public void release() {
        if (engineThread == null) {
            throw new IllegalArgumentException("CleverCaptureEngine is not initialized");
        }

        engineHandler.post(doRelease);
        engineThread.quitSafely();
        try {
            engineThread.join();
        } catch (InterruptedException e) {
            Log.w(LOG_TAG, "Interrupted while waiting for CleverCaptureEngine thread to finish");
            Thread.currentThread().interrupt();
        }
        engineHandler = null;
        engineThread = null;
    }
    public void startCapture(EventListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener cannot be null");
        } else if (engineThread == null) {
            throw new RuntimeException("CleverCaptureEngine is not initialized");
        }

        engineHandler.post(new DoStartCaptureRunnable(listener));
    }
    public void completeCapture() {
        if (engineThread == null) {
            throw new RuntimeException("CleverCaptureEngine is not initialized");
        }

        engineHandler.post(doCompleteCapture);
    }

    public void regRoi(RectF roi) {
        int width = cameraActor.getWidth();
        int height = cameraActor.getHeight();

        int x = (int) (roi.left * width);
        int y = (int) (roi.top * height);
        int w = (int) (roi.width() * width);
        int h = (int) (roi.height() * height);

        synchronized(mLock) {
            mRoiIndex = nRegROI(x, y, w, h);
            mRoiIndexs.add(mRoiIndex);
            mFrameIdsCurRoi = new Vector<Integer>();


            mRoiStartId = mFrameId;
            mRoiX = x;
            mRoiY = y;
            mRoiW = w;
            mRoiH = h;
        }

        Log.d(LOG_TAG, "regRoi mRoiIndex=" + mRoiIndex);
    }

    public void unregROI(int index) {
        Log.d(LOG_TAG, "unregROI index=" + index);
        synchronized(mLock) {
            nUnregROI(index);

            int frameIdsCount = mFrameIdsCurRoi.size();
            mFrameIdsPerRoiIndex.put(mRoiIndex, (Vector<Integer>) mFrameIdsCurRoi.clone());
            if(frameIdsCount > 0) {
                int[] frameIds = new int[frameIdsCount];
                int[] scores = new int[frameIdsCount * cameraActor.getBurstNum()];
                for(int i = 0; i < frameIdsCount; i++) {
                    frameIds[i] = mFrameIdsCurRoi.get(i);
                }

                int rc = nGetPostProcessResult(scores, frameIds, mRoiIndex);
                if(rc != 0) {
                    Log.i(LOG_TAG, "nGetPostProcessResult fail for roiIndex " + mRoiIndex);
                    for(int i = 0; i < scores.length; i++)
                        scores[i] = -1;
                }

                for(int i = 0; i < scores.length; i++)
                    mPostProcessingScores.add(scores[i]);

                Log.i(LOG_TAG, "roiIndex " + mRoiIndexs.toString());
                Log.i(LOG_TAG, "postprocessing result " + mPostProcessingScores.toString());
            }

            if(mLogFileDump != null) {
                Vector<Integer> rois = mLogFileDump.getRois();
                rois.add(mRoiIndex);
                rois.add(mRoiStartId);
                rois.add(mFrameId);
                rois.add(mRoiX);
                rois.add(mRoiY);
                rois.add(mRoiW);
                rois.add(mRoiH);
            }
        }
    }
    public void updateRoi(RectF roi) {
        mRoiRect.set(roi);
        if(state == State.CAPTURING) {
            if(mRoiIndex != -1) {
                unregROI(mRoiIndex);
                mRoiIndex = -1;
            }
            regRoi(mRoiRect);
        }
    }

    public void removeRoi() {
        if(state == State.CAPTURING && mRoiIndex != -1) {
            unregROI(mRoiIndex);
            mRoiIndex = -1;
        }
    }

    public void submitForEval(int frameId) {
        if(state == State.CAPTURING) {
            synchronized(mLock) {
                nSubmitForEval(frameId);
                mFrameIdsCurRoi.add(frameId);


                if(mLogFileDump != null) {
                    mLogFileDump.getEvalFramesId().add(frameId);
                }
            }
        }
    }

    public Vector<Integer> getPostProcessingResult() {
        return mPostProcessingScores;
    }

    public native int nInit(int w, int h, int burst_num, int mode);
    public native int nRegROI(int x, int y, int w, int h);
    public native int nUnregROI(int index);
    public native int nProcess(ByteBuffer buffer, int w, int h, CleverCaptureScore score);
     public native int nSubmitForEval(int frameId);
    public native int nGetPostProcessResult(int[] scores, int[] frameIds,int RoiIdx);
    public native void nRelease();
}
