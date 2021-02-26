/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.service;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.hardware.soundtrigger.SoundTrigger;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;

import com.qualcomm.qti.smartassistant.SmartAssistantActivity;
import com.qualcomm.qti.smartassistant.asr.CandidateInfo;
import com.qualcomm.qti.smartassistant.asr.IVoiceSdkListener;
import com.qualcomm.qti.smartassistant.asr.VoiceSdkHelper;
import com.qualcomm.qti.smartassistant.service.device.ContactsUploader;
import com.qualcomm.qti.smartassistant.sva.IWakeupListener;
import com.qualcomm.qti.smartassistant.sva.SvaApiConst;
import com.qualcomm.qti.smartassistant.sva.WakeupSessionHelper;
import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.utils.SharedPreferenceUtils;
import com.qualcomm.qti.smartassistant.utils.FileUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class SmartAssistantService extends Service{
    public static final String STARTED_BY_ASSISTANT_SERVICE = "Started_By_Assistant_Service";
    private static final int MSG_ESTABLISH_SVA_SESSION = 0x3001;
    private static final int MSG_TERMINATE_SVA_SESSION = 0x3002;
    private static final int MSG_RESTART_SVA_RECOGNITION = 0x3005;
    private static final int MSG_INITIALIZE_STORAGE = 0x3006;
    private static final String TAG = "SmartAssistantService";
    private static final String XIAODUXIAODU_PDK_NAME = "XiaoduXiaodu.uim";
    private static final String XIAODUXIAODU_UDM_NAME = "XiaoduXiaodu.udm";

    private ArrayList<Messenger> mClients = new ArrayList<>();
    private ArrayList<Context> mClientContexts = new ArrayList<>();

    private final Handler mReceiveHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what){
                case Messages.REGISTER_CLIENT_REQUEST:
                    LogUtils.v(TAG, "MSG_REGISTER_CLIENT" );
                    if (!mClients.contains(msg.replyTo)) {
                        mClients.add(msg.replyTo);
                        Log.v(TAG, "adding client = " + msg.replyTo +" clients size = "
                                + mClients.size());
                    }

                    Context registerContext = (Context) msg.obj;
                    if (!mClientContexts.contains(registerContext)
                            && registerContext instanceof SmartAssistantActivity){
                        mClientContexts.add((Context) msg.obj);
                        Log.v(TAG, "add client context= " + msg.obj);
                    }
                    break;
                case Messages.UNREGISTER_CLIENT_REQUEST:
                    LogUtils.v(TAG, "MSG_UNREGISTER_CLIENT" );
                    if (mClients.remove(msg.replyTo)){
                        Log.v(TAG, "remove client = " + msg.replyTo);
                    }

                    Context unRegisterContext = (Context) msg.obj;
                    if (mClientContexts.contains(unRegisterContext)
                            && unRegisterContext instanceof SmartAssistantActivity){
                        Log.v(TAG, "remove client context= " + msg.obj);
                        handleReleaseVoiceSdk();
                        mClientContexts.remove(unRegisterContext);
                    }
                    break;
                case Messages.START_RECOGNITION_REQUEST:
                    Log.v(TAG, "MSG_START_RECOGNITION" );
                    mAsyncEventsHandler.sendEmptyMessage(MSG_ESTABLISH_SVA_SESSION);
                    handleReleaseVoiceSdk();
                    handleInitializeVoiceSdk();
                    break;
                case Messages.STOP_RECOGNITION_REQUEST:
                    Log.v(TAG, "MSG_STOP_RECOGNITION" );
                    mAsyncEventsHandler.sendEmptyMessage(MSG_TERMINATE_SVA_SESSION);
                    handleReleaseVoiceSdk();
                    break;
            }
        }
    };

    // A reference for remote Service message receiving
    private final Messenger mReceiveMessenger = new Messenger(mReceiveHandler);

    private boolean mIsServiceStateEnabled;
    private SoundTrigger.RecognitionEvent mLastRecognitionEvent;
    private Handler mAsyncEventsHandler;
    private HandlerThread myHandlerThread;
    private ContactsUploader mContactsUploader;
    private VoiceSdkHelper mVoiceSdk;
    private ScreenWakeupHelper mScreenWakeup;
    private boolean mVoiceSdkInitialized = false;
    private boolean mSvaSessionEstablished = false;
    private String mLastRunSmName = XIAODUXIAODU_PDK_NAME;
    private Runnable mVoicePostExecuteTask;
    private boolean mNeedRestoreSVAListening = false;

    private WakeupSessionHelper mWakeup;
    private IWakeupListener mWakeupListener = new IWakeupListener() {
        @Override
        public void onRecognition(final SoundTrigger.RecognitionEvent recognitionEvent) {
            LogUtils.d(TAG,"onRecognition received wakeup status"+ recognitionEvent.status);
            mLastRecognitionEvent = recognitionEvent;
            mNeedRestoreSVAListening = true;
            if (SoundTrigger.RECOGNITION_STATUS_SUCCESS == recognitionEvent.status) {
                if (recognitionEvent.captureAvailable){
                    startVoiceRecognition(recognitionEvent.captureAvailable,
                            recognitionEvent.captureSession, recognitionEvent.soundModelHandle);

                }else {
                    new WakeupTonePlayer(getApplicationContext()).play( new WakeupTonePlayer.OnCompletionListener() {
                        @Override
                        public void onCompletion() {
                            startVoiceRecognition(recognitionEvent.captureAvailable,
                                    recognitionEvent.captureSession, recognitionEvent.soundModelHandle);
                        }
                    });
                }
                startMainActivity();
                mScreenWakeup.wakeup();
            } else {
                LogUtils.d(TAG," wakeup failed == "+recognitionEvent.status);
            }
        }

        @Override
        public void onServiceStatusChange(int state) {
            if (state == SoundTrigger.SERVICE_STATE_ENABLED){
                if (!getIsServiceStatusEnabled()){
                    setIsServiceStatusEnabled(true);
                    if (mLastRecognitionEvent != null){
                        sendRestartRecognitionRequest();
                    }
                }
            }else if (state == SoundTrigger.SERVICE_STATE_DISABLED) {
                if (getIsServiceStatusEnabled()){
                    setIsServiceStatusEnabled(false);
                }
            }
        }

        @Override
        public void onServiceDied() {
            LogUtils.d(TAG," onServiceDied ");
        }
    };

    private IVoiceSdkListener mVoiceSdkListener = new IVoiceSdkListener() {
        @Override
        public void onVoiceStart() {
            sendMessageToClients(Messages.START_VOICE_INPUT_RESPONSE, null);
        }

        @Override
        public void onVoiceStop() {
            sendMessageToClients(Messages.STOP_VOICE_INPUT_RESPONSE, null);
            if (mLastRecognitionEvent != null && mNeedRestoreSVAListening){
                sendRestartRecognitionRequest();
            }
        }

        @Override
        public void onNetworkException() {
            if (mLastRecognitionEvent != null && mNeedRestoreSVAListening){
                sendRestartRecognitionRequest();
            }
        }

        @Override
        public void onRenderVoiceInputText(String text) {
            sendMessageToClients(Messages.RENDER_VOICE_INPUT_TEXT_RESPONSE, text);
        }

        @Override
        public void onHtmlPayload(String url) {
            sendMessageToClients(Messages.LOAD_HTML_URL_RESPONSE, url);
        }

        @Override
        public void onPhoneCallByName(List<CandidateInfo> candidateInfos) {
            sendMessageToClients(Messages.CALL_BY_CONTACTS_RESPONSE, candidateInfos);
        }

        @Override
        public void onSendSmsByName(List<CandidateInfo> candidateInfos, String message) {
            Map<String, Object> smsParams =  new HashMap<>();
            smsParams.put("candidate",candidateInfos);
            smsParams.put("message",message);
            sendMessageToClients(Messages.SEND_SMS_BY_CONTACTS_RESPONSE, smsParams);
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        LogUtils.d(TAG," onCreate ");
        initializeAsyncEventsHandler();
        mAsyncEventsHandler.sendMessage(Message.obtain(null, MSG_INITIALIZE_STORAGE));

        mWakeup = new WakeupSessionHelper();
        mWakeup.addWakeupListener(mWakeupListener);

        mVoiceSdk =  new VoiceSdkHelper(getApplicationContext());
        mVoiceSdk.addVoiceSdkListener(mVoiceSdkListener);

        if (SharedPreferenceUtils.getAssistantEnabled(getApplicationContext())){
            mAsyncEventsHandler.sendEmptyMessage(MSG_ESTABLISH_SVA_SESSION);
        }

        mScreenWakeup = new ScreenWakeupHelper(getApplicationContext());

        mContactsUploader = new ContactsUploader(getApplicationContext(), mVoiceSdk);
    }

    @Override
    public int onStartCommand(Intent _intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mReceiveMessenger.getBinder();
    }

    private void initializeAsyncEventsHandler(){
        myHandlerThread = new HandlerThread( "voice-recognition") ;
        myHandlerThread.start();
        mAsyncEventsHandler = new Handler( myHandlerThread.getLooper() ){
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what){
                    case MSG_INITIALIZE_STORAGE:
                        handleInitializeStorage();
                        break;
                    case MSG_ESTABLISH_SVA_SESSION:
                        handleEstablishSvaSession();
                        break;
                    case MSG_TERMINATE_SVA_SESSION:
                        handleTerminateSvaSession();
                        break;
                    case MSG_RESTART_SVA_RECOGNITION:
                        handleRestartRecognition((int)msg.obj);
                        break;
                }
            }
        };
    }

    @Override
    public void onDestroy(){
        if (myHandlerThread != null){
            myHandlerThread.quitSafely();
        }
        mContactsUploader.release();

        //maybe need to release in async handler.
        mWakeup.releaseAllSvaSessions();
        mWakeup.removeWakeupListener(mWakeupListener);

        mVoiceSdk.removeVoiceSdkListener(mVoiceSdkListener);
        handleReleaseVoiceSdk();

        super.onDestroy();
    }

    private void handleInitializeVoiceSdk(){
        if (!mVoiceSdkInitialized ) {
            mVoiceSdk.initializeSdk();
            mContactsUploader.updateContacts();
            mVoiceSdkInitialized = true;
            if (mVoicePostExecuteTask != null){
                mVoicePostExecuteTask.run();
                mVoicePostExecuteTask = null;
            }
        }
    }

    private void handleReleaseVoiceSdk(){
        if (mVoiceSdkInitialized) {
            mVoiceSdk.stopRecognition();
            mVoiceSdk.releaseSdk();
            mVoiceSdkInitialized = false;
            mVoicePostExecuteTask = null;
        }
    }

    private void handleInitializeStorage(){
        FileUtils.createDirIfNotExists(FileUtils.APP_PATH);
        FileUtils.createDirIfNotExists(FileUtils.LOOK_AHEAD_VOICE_COMMANDS_PATH);
        FileUtils.createDirIfNotExists(FileUtils.NORMAL_VOICE_COMMANDS_PATH);
        FileUtils.copyAssetsToStorageIfNotExists(getApplicationContext(), FileUtils.APP_PATH,
                XIAODUXIAODU_PDK_NAME);
    }

    private void handleEstablishSvaSession(){
        final int ret = establishSvaSession();
        sendMessageToClients(Messages.START_RECOGNITION_RESPONSE, ret);
    }

    private void handleTerminateSvaSession(){
        int ret = terminateSvaSession();
        sendMessageToClients(Messages.STOP_RECOGNITION_RESPONSE, ret);
    }

    private void handleRestartRecognition(int smHandle){
        mWakeup.restartRecognition(smHandle);
    }

    /**
     * must restart recognition after receive the sva recognition result.
     * 1.service state change.(system send the event to app)
     * 2.network exception
     * 3.sdk not start the audio recording.
     */
    private void sendRestartRecognitionRequest(){
        mAsyncEventsHandler.sendMessage(Message.obtain(null,
                MSG_RESTART_SVA_RECOGNITION, mLastRecognitionEvent.soundModelHandle));
        mNeedRestoreSVAListening = false;
    }

    private int terminateSvaSession(){
        mSvaSessionEstablished = false;
        return mWakeup.terminateSvaSession(mLastRunSmName);
    }

    private int establishSvaSession(){
        if (mSvaSessionEstablished){
            return SvaApiConst.RET_CODE_OK;
        }else {
            int ret = mWakeup.establishSvaSession(getTargetSmName(), getApplicationContext());
            mLastRunSmName = getTargetSmName();
            if (ret == SvaApiConst.RET_CODE_OK){
                mSvaSessionEstablished = true;
            }
            return ret;
        }
    }

    /**
     * must start voice recognition after sdk initialized
     * @param captureAvailable
     * @param captureSession
     * @param soundModelHandle
     */
    private void startVoiceRecognition(final boolean captureAvailable, final int captureSession,
                                       final int soundModelHandle){
        if (mVoiceSdkInitialized) {
            mVoiceSdk.startRecognition(captureAvailable, captureSession, soundModelHandle);
        } else {
            mVoicePostExecuteTask = new Runnable() {
                @Override
                public void run() {
                    mVoiceSdk.startRecognition(captureAvailable, captureSession, soundModelHandle);
                }
            };
        }
    }

    private String getTargetSmName(){
        boolean isUserVerificationEnabled =  SharedPreferenceUtils.getIsUserVerificationEnabled(getApplicationContext())
                && FileUtils.isFileExist(FileUtils.getSMFilePath(XIAODUXIAODU_UDM_NAME));
        return isUserVerificationEnabled ? XIAODUXIAODU_UDM_NAME : XIAODUXIAODU_PDK_NAME;
    }

    private boolean getIsServiceStatusEnabled() {
        return mIsServiceStateEnabled;
    }

    private void setIsServiceStatusEnabled(boolean enabled) {
        mIsServiceStateEnabled = enabled;
    }

    private void startMainActivity(){
        Intent intent = new Intent(getApplicationContext(), SmartAssistantActivity.class);
        intent.putExtra(STARTED_BY_ASSISTANT_SERVICE, true);
        startActivity(intent);
    }

    // Send a message with obj data to all registered clients
    private void sendMessageToClients(int what, Object obj) {
        Iterator<Messenger> iterator = mClients.iterator();
        while (iterator.hasNext()){
            Messenger messenger = iterator.next();
            try {
                Message msg = Message.obtain(null, what, obj);
                messenger.send(msg);
            }catch (RemoteException e){
                iterator.remove();
            }
        }
    }

    private Context getActivityContext() {
        final int size = mClientContexts.size();
        return size> 0 ? mClientContexts.get(size -1): null;
    }
}
