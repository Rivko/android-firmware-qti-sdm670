/*
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.biometrics.voiceprint.sdk;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Log;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.PointF;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.view.ViewGroup;
import android.view.View;
import android.view.WindowManager;
import android.graphics.PixelFormat;
import android.os.RemoteException;
import android.app.ActivityManagerNative;
import android.os.AsyncTask;

import java.security.InvalidParameterException;
import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Vector;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.qualcomm.qti.biometrics.voiceprint.sdk.OnMatchListener;
import com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener;

import vendor.qti.voiceprint.V1_0.IQtiVoicePrintService;
import vendor.qti.voiceprint.V1_0.IQtiVoicePrintServiceCallback;

import vendor.qti.voiceprint.V1_0.MatchRequest;
import vendor.qti.voiceprint.V1_0.EnrollRequest;
import vendor.qti.voiceprint.V1_0.DeleteRequest;
import vendor.qti.voiceprint.V1_0.StartAudioCaptureRequest;
import vendor.qti.voiceprint.V1_0.ProcessFrameRequest;
import vendor.qti.voiceprint.V1_0.ThresholdRequest;
import vendor.qti.voiceprint.V1_0.RenameRequest;

import vendor.qti.voiceprint.V1_0.ErrorResult;
import vendor.qti.voiceprint.V1_0.DeleteResult;
import vendor.qti.voiceprint.V1_0.EnrollResult;
import vendor.qti.voiceprint.V1_0.MatchResult;
import vendor.qti.voiceprint.V1_0.VersionInfoResult;
import vendor.qti.voiceprint.V1_0.ConfigResult;
import vendor.qti.voiceprint.V1_0.RenameResult;

import vendor.qti.voiceprint.V1_0.Status;
/**
 * The Class VoicePrintManager. The VoicePrintManager is responsible for
 * communicating with the VoicePrint feature on the device. If the VoicePrint
 * feature is not available on the device, the API calls will return
 * FEATURE_NOT_AVAILABLE.
 * <p/>
 * *
 * <p/>
 * The API will provide classes that expose the VoicePrint functionality. To use
 * this SDK,
 * <ul>
 * <li>The minimum API level supported is <b>18</b>.</li>
 * <li>The voice print feature must be enabled on the device.</li>
 * <li>All voice print components are loaded on the device.</li>
 * </ul>
 * <p/>
 * <p/>
 * <p/>
 * The Voice Print SDK is a set of API that includes support for
 * Voice Print recognition/verification. It is a set of user-friendly APIs for
 * enabling and leveraging the Voice Print technology. These features include
 * matching, enrolling, deleting, and updating voiceprint data.
 * <p/>
 * The SDK interacts and binds directly with the VoicePrintService. The SDK
 * makes direct calls to the VoicePrintService via the AIDL interface. Calls to
 * the service should not block. Non-system applications have permission to use
 * a subset of the API. If a non-system app tries to use a privileged command,
 * an error will be returned. The application may query the SDK to check the
 * command set available. System-privileged applications may use all the API.
 * <p/>
 * Applications are responsible for recording and forwarding PCM data to the
 * SDK. The application must provide the audio data including sample rate,
 * channel configuration, and audio format. Processing is done asynchronously.
 * The result of a request is posted through a result interface. Since a request
 * such as match() may require multiple samples to be processed, the final
 * result is posted separately. The app is responsible for maintaining its own
 * state data.
 * <p/>
 * When the SDK is first instantiated, it will check if the OEM has enabled the
 * voice print feature on the device and that framework components are loaded on
 * the device. If either check fails, any API calls will result in a
 * FEATURE_NOT_AVAILABLE response. To use the Voice Print SDK,
 * create a new instance by calling VoicePrintSDK.newInstance(context) or
 * VoicePrintSDK.newInstance(context, mode). The query will fail if the feature
 * is not enabled on the device, components are missing, or there are permission
 * errors. In the same note, newInstance will return null if the feature is not
 * enabled on the device, components are missing, there are permission errors.
 * Before running voiceprint, the user should query isVoicePrintAvailable().
 * <p/>
 * The application is responsible for recording resource management, including
 * if a recording session is already running. The application is responsible for
 * requesting correct permissions to record and handle phone state changes. It
 * is also responsible for handling phone call state changes. If there is an
 * incoming call, the application should tear down the current recording and
 * cancel the request.
 * <p/>
 * <pre class="language-java">
 * Insert sample code here...TODO
 * </pre>
 */
public class VoicePrintManager implements AutoCloseable {


    /**
     * The Constant Logging TAG.
     */
    private final static String TAG = "VoicePrintManager";
    private final static String TAG_AUDIO = "VoicePrintAudio";
    public static final String DEBUG = "DEBUG";
    public static final String DATA = "DATA";

    public static String THRESHOLD = "THRESHOLD";
    public static String BYTE_INDEX = "BYTE_INDEX";

    public static final int THRESHOLD_LOW = 3;
    public static final int THRESHOLD_MED = 2;
    public static final int THRESHOLD_HIGH = 1;

    /**
     * The application context.
     */
    private Context mContext;

    /**
     * The Constant RECORD_AUDIO_PERMISSION.
     */
    private final static String RECORD_AUDIO_PERMISSION = "android.permission.RECORD_AUDIO";

    /** The state flags. */
    private static boolean mEnrolling = false;
    private static boolean mMatching = false;

    /**
     * The service.
     */
    private IQtiVoicePrintService mService = null;

    /**
     * The service is bound flag.
     */
    private boolean mIsBound;

    /**
     * The component.
     */
    private ComponentName mComponent;

    private static Map<Integer, Boolean> FeatureMap = new HashMap<Integer, Boolean>();

    private final static String VOP_PROPERTY = "persist.vendor.qvop";

    /**
     * Callback listeners.
     */
    private OnResultListener resultListener;
    private OnMatchListener matchListener;
    private OnEnrollListener enrollListener;
    private OnErrorListener errorListener;
    private OnVersionListener versionListener;

    // All features are disabled by default. Verify availability after
    // service has connected to verify that libraries were properly loaded.
    static {
        FeatureMap.put(VoicePrintFeature.VOICEPRINT, isVoicePrintEnabled());
        FeatureMap.put(VoicePrintFeature.MATCH, isVoicePrintEnabled());
        FeatureMap.put(VoicePrintFeature.MATCH_ANY, isSystemUid());
        FeatureMap.put(VoicePrintFeature.ENROLL, isSystemUid());
        FeatureMap.put(VoicePrintFeature.DELETE, isSystemUid());
        FeatureMap.put(VoicePrintFeature.DELETE_ALL, isSystemUid());
        FeatureMap.put(VoicePrintFeature.PHRASE_STRENGTH, isSystemUid());
    }

    /**
     * <p/>
     * Creates a new VoicePrintManager SDK instance. Returns null if feature is
     * not enabled or components are missing on the device. Will throw
     * InvalidParameterException if context is not valid.
     * <p/>
     * The VoicePrintManager is responsible for communicating with the
     * VoicePrint feature on the device. If the VoicePrint feature is not
     * available on the device, the API calls will return FEATURE_NOT_AVAILABLE.
     *
     * @param context the context
     * @return the new instance of VoicePrintManager
     */
    static public VoicePrintManager newInstance(Context context) {
        if (context == null) {
            Log.e(TAG, "Context cannot be null.");
            throw new InvalidParameterException("Context cannot be null.");
        }

        // Check components
        try {
            return new VoicePrintManager(context);
        } catch (RuntimeException e) {

            Log.d(TAG, "Error constructing VoicePrintManager");
        }

        return null;
    }

    /**
     * <p/>
     * The QVoicePrintManager is responsible for communicating with the
     * VoicePrint feature on the device. If the VoicePrint feature is not
     * available on the device, the API calls will return FEATURE_NOT_AVAILABLE.
     * <p/>
     * This constructor will throw an InvalidParameterException if the context
     * is not valid and a RuntimeException if the RECORD_AUDIO permission is not
     * set.
     *
     * @param context the context
     * @throws RuntimeException          the runtime exception
     * @throws InvalidParameterException the invalid parameter exception
     */
    private VoicePrintManager(Context context) throws RuntimeException,
        InvalidParameterException {

        if (context == null) {
            Log.e(TAG, "Context cannot be null.");
            throw new InvalidParameterException("Context cannot be null.");
        }


        if (isVoicePrintEnabled() == false) {
            Log.e(TAG, "VoicePrint is DISABLED on this device.");
            throw new RuntimeException("Permission failure: Voice Print is not enabled on this device.");
        }

        Log.d(TAG, "VoicePrint is ENABLED on this device.");

        //        if (!hasRecordPermission(context)) {
        //            Log.e(TAG, "Permission failure: android.permission.RECORD_AUDIO");
        //            throw new RuntimeException(
        //                    "Permission failure: android.permission.RECORD_AUDIO");
        //        }

        mContext = context;

        try {
            Log.d(TAG, "Getting service...");
            if (getQtiVoicePrintService() != null) {
                mIsBound = true;
            }
        } catch (Exception e) {
            Log.e(TAG, "Exception binding to service. " + e.getMessage());
            e.printStackTrace();
            mIsBound = false;
            FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
            FeatureMap.put(VoicePrintFeature.MATCH, false);
            throw new RuntimeException("VoicePrintServiceError: Cannot bind to the service.");
        } catch (Error e) {
            Log.e(TAG, "Error binding to service. " + e.getMessage());
            e.printStackTrace();
            mIsBound = false;
            FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
            FeatureMap.put(VoicePrintFeature.MATCH, false);
            throw new RuntimeException("VoicePrintServiceError: Cannot bind to the service.");
        }

        Log.d(TAG, "Bound to Service? " + mIsBound);
        Log.d(TAG, "hasSystemPrivilege? " + isSystemUid());

        if (mIsBound) {
            // Wait until connection is made to verify that features
            // are available on the device.
        } else {
            Log.d(TAG, "Cannot bind to service or insufficient privileges.");
            FeatureMap.put(VoicePrintFeature.VOICEPRINT, false);
            FeatureMap.put(VoicePrintFeature.MATCH, false);
            FeatureMap.put(VoicePrintFeature.MATCH_ANY, false);
            FeatureMap.put(VoicePrintFeature.ENROLL, false);
            FeatureMap.put(VoicePrintFeature.DELETE, false);
            FeatureMap.put(VoicePrintFeature.DELETE_ALL, false);
            FeatureMap.put(VoicePrintFeature.PHRASE_STRENGTH, false);
        }

    }


    private IQtiVoicePrintService getQtiVoicePrintService() throws Exception {

        IQtiVoicePrintService qtiVoicePrint = IQtiVoicePrintService.getService();
        if (qtiVoicePrint == null) {
            Log.e(TAG, "Unable to connect to qvop-service");
            return null;
        }
        return qtiVoicePrint;
    }


    /**
     * Checks for record_audio permission.
     *
     * @param context the context
     * @return true, if successful
     */
    private boolean hasRecordPermission(Context context) {
        try {
            int res = context
                      .checkCallingOrSelfPermission(RECORD_AUDIO_PERMISSION);
            return (res == PackageManager.PERMISSION_GRANTED);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.Object#finalize()
     */
    @Override
    public void finalize() {
        close();
    }

    /**
     * Check system properties if voice print feature is enabled on this device.
     *
     * @return
     */
    private static boolean isVoicePrintEnabled() {
        boolean enabled = SystemProperties.getBoolean(VOP_PROPERTY, false);
        return enabled;
    }

    /*
     * (non-Javadoc)
     *
     * @see java.lang.AutoCloseable#close()
     */
    @Override
    public synchronized void close() {
        if (mService != null) {
            try {
                mService.cancel();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * Checks if is feature available. Static method to query if a feature is
     * available. Some APIs require system privilege. For example, query if
     * UPDATE is available for the app.
     *
     * @param feature the feature
     * @return true, if is feature available
     */
    public boolean isFeatureAvailable(int feature) {
        return FeatureMap.get(feature);
    }

    /**
     * Checks if is voice print available. Checks if the feature is enabled and
     * all necessary components are on the device.
     *
     * @return true, if is voice print available
     */
    public boolean isVoicePrintAvailable() {
        return FeatureMap.get(VoicePrintFeature.VOICEPRINT);
    }

    /*
    public void setMatchListener(OnMatchListener listener) {
        matchListener = listener;
    }

    public void setEnrollListener(OnEnrollListener listener) {
        enrollListener = listener;
    }*/

    /**
     * Match. Check if the current user is a match. Result posted will be MATCH,
     * NO_MATCH, or an error. Additional data will be bundled with the result.
     *
     * @return the result constant
     */
    public int match() {
        Log.d(TAG, "match called phraseId=NULL");
        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH) == false) {
                Log.d(TAG, "feature is not available");
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            //call is to match against current user as the onlly criteria
            MatchRequest request = new MatchRequest();
            request.user_id = getCurrentUser();
            getQtiVoicePrintService().match(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match. Check if the current user is a match. Result posted will be MATCH,
     * NO_MATCH, or an error. Additional data will be bundled with the result.
     *
     * @param callback the callback
     * @return the result constant
     */
    public int match(OnMatchListener callback) {
        Log.d(TAG, "match called phraseId=NULL");
        matchListener = callback;
        return match();
    }

    /**
     * Match. Check if the current user is a match to particular identifier.
     * Result posted will be MATCH, NO_MATCH, or an error. Additional data will
     * be bundled with the result.
     *
     * @param phraseId unique phrase identifier
     * @param callback the callback
     * @return the result constant
     */
    public int match(String phraseId, OnMatchListener callback) {
        Log.d(TAG, "match called phraseId=" + phraseId);
        matchListener = callback;

        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH) == false) {
                Log.d(TAG, "feature is not available");
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            //call is to match against current user with id param
            MatchRequest request = new MatchRequest();
            request.user_id = getCurrentUser();
            request.identifier = phraseId;
            getQtiVoicePrintService().matchWithId(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @return the result constant
     */
    public int matchAny() {

        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH_ANY) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            //request data not used in this scenario
            MatchRequest request = new MatchRequest();
            getQtiVoicePrintService().matchAny(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @param keyphrase the keyphrase to search for
     * @return the result constant
     */
    public int matchAny(String keyphrase) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.MATCH_ANY) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            //matchAny does not require the current user id, just a
            //keyphrase in this scenario
            MatchRequest request = new MatchRequest();
            request.phrase = keyphrase;
            getQtiVoicePrintService().matchAnyKeyPhrase(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @param keyphrase the keyphrase to search for
     * @param callback  the listener
     * @return the result constant
     */
    public int matchAny(String keyphrase, OnMatchListener callback) {

        matchListener = callback;
        return matchAny(keyphrase);
    }

    /**
     * Match any. Check if voice is a match to any user in the device database.
     * Result posted will be MATCH, NO_MATCH, or an error. If a match is a
     * found, additional data will be bundled with the result.
     *
     * @param callback the listener
     * @return the result constant
     */
    public int matchAny(OnMatchListener callback) {

        matchListener = callback;
        return matchAny();
    }

    /**
     * On start audio capture. PCM recording information.
     *
     * @param sampleRate       the sample rate expressed in Hz (16000)
     * @param numberOfChannels the configuration of the audio channels (1 for mono, 2 for
     *                         stereo)
     * @param audioFormat      the format in which the audio data is represented -
     *                         ENCODING_PCM_16BIT, ENCODING_PCM_8BIT. (8, 16)
     * @return the result constant
     */
    public int onStartAudioCapture(int sampleRate,
                                   int numberOfChannels,
                                   int audioFormat,
                                   OnErrorListener callback) {

        errorListener = callback;
        try {
            stopAudioRequest = false;

            //basically setting audio parameters, the call does not actually
            //begin streaming audio.
            StartAudioCaptureRequest request = new StartAudioCaptureRequest();
            request.sampleRate = sampleRate;
            request.numberOfChannels = numberOfChannels;
            request.audioFormat = audioFormat;
            getQtiVoicePrintService().onStartAudioCapture(_callback, request);
            Log.d(TAG, "onStartAudioCapture: mEnrolling: " + mEnrolling);
            if (mEnrolling) {
                Log.d(TAG, "calling enrollCaptureStart");
                getQtiVoicePrintService().enrollCaptureStart(_callback);
                setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING);
            }

        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Adds the frame. Sends PCM data to the service. This may be continuous.
     * The service is responsible for buffering all the PCM data. Will return an
     * error if PCM information has not been provided. onStartAudioCapture must
     * have been called before addFrame is valid.
     *
     * @param buffer the array to which the recorded audio data is written
     * @return the result constant
     */
    public int addFrame(byte[] buffer) {

        return addFrame(0, buffer);
    }


    /**
     * Adds the frame. Sends PCM data to the service. This may be continuous.
     * The service is responsible for buffering all the PCM data. Will return an
     * error if PCM information has not been provided. onStartAudioCapture must
     * have been called before addFrame is valid.
     *
     * @param frame_id id of the particular frame. This will be used to let the
     *                 application know which frame an EoP (end of phrase) has been
     *                 detected in.
     * @param buffer   the array to which the recorded audio data is written
     * @return the result constant
     */
    public int addFrame(int frame_id, byte[] buffer) {
        try {
            Log.d(TAG_AUDIO, "frame[0]: " + buffer[0]);
            Log.d(TAG_AUDIO, "frame[" + (buffer.length - 1) + "]: " + buffer[buffer.length - 1]);

            if ( mState == VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING ||
                    mState == VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING) {

                // This task can take a long time and block the main UI thread.
                // For better usability, post on a worker thread.
                postProcessFrame(frame_id, buffer);
            } else {
                invalidStateTransition("addFrameWithId", mState.toString());
            }
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    class PostFrameParams {
        int frame_id;
        byte[] buffer;

        public PostFrameParams(int id, byte[] zBuffer) {
            buffer = new byte[zBuffer.length];
            System.arraycopy(zBuffer, 0, buffer, 0, zBuffer.length);
            frame_id = id;
        }
    }

    private int framesToProcess = 0;
    private boolean stopAudioRequest = false;

    private void processStopAudioRequest(){
       try {
            if( framesToProcess > 0 ){
                Log.d(TAG, "Still processing");
                return;
            }

            IQtiVoicePrintService svc = getQtiVoicePrintService();
            svc.onStopAudioCapture(_callback);
            // Check current state.
            // Enrollment does not complete until ready to commit.
            // Match end of audio should check for match result.
            Log.d(TAG, "onStopAudioCapture");
            if (mEnrolling) {
                Log.d(TAG, "poll for enroll train result");
                svc.enrollCaptureComplete(stopAudioCallback);
                setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING);
            } else if (mMatching ) {
                Log.d(TAG, "poll for match result");
                svc.matchCaptureComplete(stopAudioCallback);
            }
            stopAudioRequest = false;
        }catch(Exception e){
            e.printStackTrace();
        }

    }

    private void postProcessFrame(int frame_id, byte[] buffer) {
        framesToProcess++;
        new PostFrameTask().execute(new PostFrameParams(frame_id, buffer));
    }

    private void processFrame(PostFrameParams param) {
        try {
            ProcessFrameRequest request = new ProcessFrameRequest();

            //boxing primitives to objects.
            Byte[] byteObjects = new Byte[param.buffer.length];
            int index = 0;
            for (byte b : param.buffer) {
                byteObjects[index++] = b;
            }

            Log.d(TAG, "addFrame frame_id: " + param.frame_id);
            request.timestamp = System.currentTimeMillis();
            request.frameId = param.frame_id;
            request.len = param.buffer.length;
            request.buffer.addAll(Arrays.asList(byteObjects));
            getQtiVoicePrintService().processFrame(_callback, request);
        } catch(Exception e){
            e.printStackTrace();
        }
    }

    private class PostFrameTask extends AsyncTask<PostFrameParams, Void, Integer> {
        protected Integer doInBackground(PostFrameParams... param) {
            try {
                for ( int i = 0; i < param.length; ++i ) {
                    processFrame(param[i]);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }


            return VoicePrintError.NO_ERROR;
        }

        protected void onProgressUpdate(Integer... progress) {

        }

        protected void onPostExecute(Integer result) {
            framesToProcess--;

            if( stopAudioRequest ){
                processStopAudioRequest();
            }
        }
    }

    /**
     * checkPhraseStrength. Check the relative strength of the phrase. Value of
     * phrase strength will be posted in result.
     *
     * @param phrase the phrase to check against
     * @return the result constant
     */
    public int checkPhraseStrength(String phrase) {
        if (internalFeatureCheck(VoicePrintFeature.PHRASE_STRENGTH) == false) {
            return VoicePrintError.FEATURE_NOT_AVAILABLE;
        }

        return 0;
    }

    /**
     * On stop audio capture. Called when the last sample has been sent. Result
     * will not be posted until this method has been called.
     *
     * @return the result constant
     */
    public int onStopAudioCapture(com.qualcomm.qti.biometrics.voiceprint.sdk.OnResultListener callback) {

        resultListener = callback;

        try {
            stopAudioRequest = true;
            processStopAudioRequest();
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Enroll. Start an enrollment session. The enrolled user will be the
     * current user. Additional data will be bundled with the posted result.
     *
     * @param identifier unique phrase identifier
     * @param phrase     the phrase
     * @return the result constant
     */
    public int enroll(String identifier, String phrase, int threshold) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.ENROLL) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            Log.d(TAG, "enroll _callbackValid=" + (_callback != null));

            //threshold was dropped in the android service,
            //now it is dropped here.
            EnrollRequest request = new EnrollRequest();
            request.user_id = getCurrentUser();
            request.identifier = identifier;
            request.phrase = phrase;
            getQtiVoicePrintService().enroll(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Enroll. Start an enrollment session. The enrolled user will be the
     * current user . Additional data will be bundled with the posted result.
     *
     * @param identifier unique phrase identifier
     * @param phrase     the phrase
     * @param callback   the listener
     * @return the result constant
     */
    public int enroll(String identifier, String phrase, int threshold,
                      OnEnrollListener callback) {

        enrollListener = callback;
        Log.d(TAG, "Have enroll listener? " + (enrollListener != null));
        return enroll(identifier, phrase, threshold);
    }

    public int setThreshold(String identifier, int threshold) {
        try {
            Log.d(TAG, "Setting threshold " + identifier + "=" + threshold);

            ThresholdRequest request = new ThresholdRequest();
            request.user_id = getCurrentUser();
            request.threshold = threshold;
            request.identifier = identifier;
            getQtiVoicePrintService().setThreshold(_callback, request);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return VoicePrintError.NO_ERROR;
    }

    /**
     * Complete enroll. End an enrollment session.The voice model will be saved
     * for the user.
     *
     * @return the result constant
     */
    public int completeEnroll(OnErrorListener callback) {
        try {
            // Do not overwrite the enroll listener
            errorListener = callback;
            getQtiVoicePrintService().enrollCommit(_callback);
            setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
            Log.d(TAG, "Completing enrollment");
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete. Start an delete session. All of the current users enrolled data
     * will be deleted. Result will be posted.
     *
     * @return the result constant
     */
    public int delete() {
        try {
            if (internalFeatureCheck(VoicePrintFeature.DELETE) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }
            Log.d(TAG, "delete");
            //delete request requires only the current user id as a param
            DeleteRequest request = new DeleteRequest();
            request.user_id = getCurrentUser();
            getQtiVoicePrintService().deleteUser(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete. Start an delete session. The current users particular voicepint
     * template with identifier will be deleted. Result will be posted.
     *
     * @param identifier unique phrase identifier
     * @return the result constant
     */
    public int delete(String identifier) {
        try {
            if (internalFeatureCheck(VoicePrintFeature.DELETE) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            Log.d(TAG, "Delete " + identifier);
            //delete request requires only the current user id as a param
            DeleteRequest request = new DeleteRequest();
            request.user_id = getCurrentUser();
            request.identifier = identifier;
            getQtiVoicePrintService().deleteWithId(_callback, request);
            setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete. Start an delete session. All of the current users enrolled data
     * will be deleted. Result will be posted.
     *
     * @param callback the listener
     * @return the result constant
     */
    public int delete(OnResultListener callback) {

        resultListener = callback;
        return delete();
    }

    /**
     * Delete. Start an delete session. The current users particular voicepint
     * template with identifier will be deleted. Result will be posted.
     *
     * @param identifier unique phrase identifier
     * @param callback   the listener
     * @return the result constant
     */
    public int delete(String identifier, OnResultListener callback) {

        resultListener = callback;
        return delete(identifier);
    }

    /**
     * Delete all. Start an delete session. All users will be deleted from the
     * database. Result will be posted.
     *
     * @return the result constant
     */
    public int deleteAll() {
        try {
            if (internalFeatureCheck(VoicePrintFeature.DELETE_ALL) == false) {
                return VoicePrintError.FEATURE_NOT_AVAILABLE;
            }

            //deleteAll is scoped to the current user. TODO: verify scope
            DeleteRequest request = new DeleteRequest();
            request.user_id = getCurrentUser();
            getQtiVoicePrintService().deleteAll(_callback, request);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        setState(VoicePrintServiceStates.WAIT_FOR_RESULT);
        return VoicePrintError.NO_ERROR;
    }

    /**
     * Delete all. Start an delete session. All users will be deleted from the
     * database. Result will be posted.
     *
     * @param callback the listener
     * @return the result constant
     */
    public int deleteAll(OnResultListener callback) {

        resultListener = callback;

        return deleteAll();
    }

    /**
     * Cancel. Cancels current action. Action must be associated with app and
     * cannot cancel action request from another app.
     *
     * @return the result constant
     */
    public int cancel() {
        try {
            getQtiVoicePrintService().cancel();
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        setState(VoicePrintServiceStates.READY);
        return VoicePrintError.NO_ERROR;
    }

    /**
     * Rename a voiceprint template.
     *
     * @param oldKey
     * @param newKey
     * @return
     */
    public int rename(String oldKey, String newKey) {
        try {
            RenameRequest request = new RenameRequest();
            request.user_id = getCurrentUser();
            request.old_id = oldKey;
            request.new_id = newKey;
            getQtiVoicePrintService().rename(_callback, request);
        } catch (Exception e) {
            return VoicePrintError.ERROR;
        }

        setState(VoicePrintServiceStates.READY);
        return VoicePrintError.NO_ERROR;
    }

    /**
     * Rename a voiceprint template.
     *
     * @param oldKey
     * @param newKey
     * @param newKey
     * @parafm newKey
     * @param callback
     * @return
     */
    public int rename(String oldKey, String newKey, OnResultListener callback) {

        resultListener = callback;

        return rename(oldKey, newKey);
    }

    /**
     * Check is System UID.
     *
     * @return true if a system app
     */
    private static boolean isSystemUid() {
        return (android.os.Process.myUid() == android.os.Process.SYSTEM_UID);
    }

    /**
     * Check if feature is enabled.
     *
     * @param feature
     * @return true if available else false
     */
    private boolean internalFeatureCheck(int feature) {
        if (FeatureMap.get(feature)) {
            return true;
        }
        return false;
    }

    private String getCurrentUser() throws RemoteException {
        return Integer.toString(ActivityManagerNative.getDefault()
                                .getCurrentUser().id);
    }

    private void setState(VoicePrintServiceStates state) {
        Log.i(TAG, "State Change from: " + mState + " to: " + state);
        mState = state;

        mMatching = state == VoicePrintServiceStates.WAIT_FOR_AUDIO_MATCHING;
        mEnrolling = state == VoicePrintServiceStates.WAIT_FOR_AUDIO_ENROLLING;
    }

    private void invalidStateTransition(String fn, String state) {
        Log.w(TAG, "Invalid function call " + fn + " in state " + state);
    }

    /** The state. */
    private VoicePrintServiceStates mState = VoicePrintServiceStates.READY;

    enum VoicePrintServiceStates  {

        READY,
        WAIT_FOR_AUDIO_MATCHING,
        WAIT_FOR_AUDIO_ENROLLING,
        WAIT_FOR_RESULT
    }

    /**
     * Messages for AIDL callback.
     */
    private final int MSG_MATCH_RESULT = 1;
    private final int MSG_ERROR = 2;
    private final int MSG_ENROLL_RESULT = 3;
    private final int MSG_RESULT = 4;
    private final int MSG_VERSION = 5;

    /**
     * The handler.
     */
    Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {

            switch (msg.what) {
            case MSG_MATCH_RESULT:
                if (matchListener != null) {
                    Bundle bundle = null;
                    if (msg.obj instanceof Bundle) {
                        bundle = (Bundle) msg.obj;
                    }
                    matchListener.onMatchStatus((msg.arg1 == VoicePrintError.NO_ERROR), bundle);
                }
                matchListener = null;
                break;
            case MSG_ERROR:
                // Notify all listeners.
                if (errorListener != null) {
                    errorListener.onError(msg.arg1);
                }
                errorListener = null;

                if (matchListener != null) {
                    matchListener.onError(msg.arg1);
                }
                matchListener = null;

                if (enrollListener != null) {
                    enrollListener.onError(msg.arg1);
                }
                enrollListener = null;
                break;
            case MSG_ENROLL_RESULT:
                if (enrollListener != null) {
                    Bundle bundle = null;
                    if (msg.obj instanceof Bundle) {
                        bundle = (Bundle) msg.obj;
                    }
                    enrollListener.onEnrollComplete((msg.arg1 == VoicePrintError.NO_ERROR), bundle);
                }
                enrollListener = null;
                break;
            case MSG_RESULT:
                if (resultListener != null) {
                    Bundle bundle = null;
                    if (msg.obj instanceof Bundle) {
                        bundle = (Bundle) msg.obj;
                    }
                    resultListener.onResult(msg.arg1, bundle);
                }
                resultListener = null;
                break;
            case MSG_VERSION:
                if (versionListener != null) {
                    versionListener.onVersionInfo(msg.arg1, msg.arg2);
                }
                versionListener = null;
                break;
            default:
                Log.d(TAG, "Not handling msg " + msg.what);
                break;
            }
        }
    };


    private IQtiVoicePrintServiceCallback _callback = new IQtiVoicePrintServiceCallback.Stub() {
        @Override
        public void onError(ErrorResult result) {
            Log.d(TAG, "onError error=" + result.error);

            if ( mMatching ) {
                MatchResult mr = new MatchResult();
                mr.status = result.error;
                onMatch(mr);
            } else if ( mEnrolling ) {
                EnrollResult er = new EnrollResult();
                er.status = result.error;
                onEnroll(er);
            } else {
                Message msg = mHandler.obtainMessage(MSG_ERROR, result.error, 0, null);
                mHandler.sendMessage(msg);
            }

            setState(VoicePrintServiceStates.READY);
        }

        @Override
        public void onMatch(MatchResult result) {
            Log.d(TAG, "onMatchStatus status=" + result.status);

            boolean success = (result.status == Status.SUCCESS);

            // Send a response to the matchListener
            if (matchListener == null) {
                Log.d(TAG, "No match listener is set");
                return;
            }

            //There are more fields that need to be propigated up
            Bundle data = new Bundle();
            StringBuilder debug = new StringBuilder();
            String newline = System.getProperty("line.separator");
            debug.append("index=" + result.index );
            debug.append(newline);
            debug.append(String.format("sentence_score=%.3f", result.sentence_score));
            debug.append(newline);
            debug.append(String.format("user_score=%.3f", result.ver_score));
            debug.append(newline);
            debug.append(String.format("spoof_score=%.3f", result.spoofScore));
            data.putString(DEBUG, debug.toString());
            data.putInt(BYTE_INDEX, result.index);

            if (result.user_id != null) {
                data.putString(OnResultBase.DATA_USER_ID, result.user_id);
            }
            if (result.identifier != null) {
                data.putString(OnResultBase.DATA_KEYPHRASE_ID, result.identifier);
            }

            // Error Code
            data.putInt(OnResultBase.FAILURE_CODE, result.status);
            Message msg = mHandler.obtainMessage(MSG_MATCH_RESULT, result.status, 0, data);
            mHandler.sendMessage(msg);

            setState(VoicePrintServiceStates.READY);
        }

        @Override
        public void onEnroll(EnrollResult result) {
            Log.d(TAG, "onEnroll status=" + result.status);

            boolean success = (result.status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (enrollListener == null) {
                Log.d(TAG, "No enroll listener is set");
                return;
            }

            Bundle data = new Bundle();
            data.putInt(OnResultBase.FAILURE_CODE, result.status);
            Message msg = mHandler.obtainMessage(MSG_ENROLL_RESULT, result.status, 0, data);
            mHandler.sendMessage(msg);

            setState(VoicePrintServiceStates.READY);
        }

        @Override
        public void onDelete(DeleteResult result) {
            Log.d(TAG, "onDeleteStatus status=" + result.status);
            if (resultListener == null) {
                return;
            }

            boolean success = (result.status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (resultListener == null) {
                Log.d(TAG, "No result listener is set");
                return;
            }

            Bundle data = new Bundle();
            if (result.user_id != null) {
                data.putString(OnResultBase.DATA_USER_ID, result.user_id);
            }
            if (result.identifier != null) {
                data.putString(OnResultBase.DATA_KEYPHRASE_ID, result.identifier);
            }


            data.putInt(OnResultBase.FAILURE_CODE, result.status);
            Message msg = mHandler.obtainMessage(MSG_RESULT, result.status, 0, data);
            mHandler.sendMessage(msg);

            setState(VoicePrintServiceStates.READY);
        }

        @Override
        public void onVersionInfo(VersionInfoResult result) {
            Log.d(TAG, "onVersionInfo version=" + result.major + "." + result.minor);
            if (versionListener == null) {
                return;
            }

            Bundle data = new Bundle();
            data.putLong(OnVersionListener.VERSION_MAJOR, result.major);
            data.putLong(OnVersionListener.VERSION_MINOR, result.minor);
            Message msg = mHandler.obtainMessage(MSG_RESULT, (int) result.major, (int) result.minor, data);
            mHandler.sendMessage(msg);

            setState(VoicePrintServiceStates.READY);
        }

        @Override
        public void onConfigStatus(ConfigResult result) {
            Log.d(TAG, "onConfigUpdateStatus status=" + result.status);
            if (resultListener == null) {
                return;
            }

            boolean success = (result.status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (resultListener == null) {
                Log.d(TAG, "No result listener is set");
                return;
            }

            Bundle data = new Bundle();
            data.putInt(OnResultBase.FAILURE_CODE, result.status);

            Message msg = mHandler.obtainMessage(MSG_RESULT, result.status, 0, data);
            mHandler.sendMessage(msg);

            setState(VoicePrintServiceStates.READY);
        }

        @Override
        public void onRenameStatus(RenameResult result) {
            Log.d(TAG, "onRenameStatus status=" + result.status);
            if (resultListener == null) {
                return;
            }

            boolean success = (result.status == VoicePrintError.NO_ERROR);

            // Send a response to the matchListener
            if (resultListener == null) {
                Log.d(TAG, "No result listener is set");
                return;
            }

            Bundle data = new Bundle();
            data.putInt(OnResultBase.FAILURE_CODE, result.status);

            Message msg = mHandler.obtainMessage(MSG_RESULT, result.status, 0, data);
            mHandler.sendMessage(msg);

            setState(VoicePrintServiceStates.READY);
        }
    };


    private IQtiVoicePrintServiceCallback stopAudioCallback = new IQtiVoicePrintServiceCallback.Stub() {
        @Override
        public void onError(ErrorResult result) {
            try {
                _callback.onError(result);
            } catch (RemoteException re) {
                Log.e(TAG, re.getMessage());
            }
        }

        @Override
        public void onMatch(MatchResult result) {
            try {
                _callback.onMatch(result);
            } catch (RemoteException re) {
                Log.e(TAG, re.getMessage());
            }
        }

        @Override
        public void onEnroll(EnrollResult result) {
            Log.d(TAG, "onEnroll status=" + result.status);

            if ( mEnrolling ) {
                Message msg = mHandler.obtainMessage(MSG_RESULT, result.status, 0, null);
                mHandler.sendMessage(msg);
            }
            //stay in same state until enroll complete
        }

        @Override
        public void onDelete(DeleteResult result) {
            //na
        }

        @Override
        public void onVersionInfo(VersionInfoResult result) {
            //na
        }

        @Override
        public void onConfigStatus(ConfigResult result) {
            //na
        }

        @Override
        public void onRenameStatus(RenameResult result) {
            //na
        }
    };
}
