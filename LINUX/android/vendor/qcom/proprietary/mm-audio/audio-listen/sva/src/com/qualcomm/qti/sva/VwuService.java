/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringWriter;
import java.lang.reflect.Field;
import java.nio.BufferOverflowException;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import android.annotation.SuppressLint;
import android.app.KeyguardManager;
import android.app.KeyguardManager.KeyguardLock;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.Loader.ForceLoadContentObserver;
import android.os.AsyncTask;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.PowerManager;
import android.os.RemoteException;
import android.os.PowerManager.WakeLock;
import android.speech.RecognizerIntent;
import android.util.Log;
import android.util.Xml;
import android.view.View;
import android.widget.Toast;

import android.media.AudioSystem;

import com.qualcomm.listen.ListenSoundModel;
import com.qualcomm.listen.ListenTypes;
import com.qualcomm.listen.ListenTypes.KeywordInfo;
import com.qualcomm.listen.ListenTypes.SVASoundModelInfo;
import com.qualcomm.listen.ListenTypes.SoundModelInfo;
import com.qualcomm.listen.ListenTypes.VWUKeywordConfLevel;
import com.qualcomm.listen.ListenTypes.VWUUserKeywordPairConfLevel;
import com.qualcomm.listen.ListenTypes.VoiceWakeupDetectionDataV2;
import com.qualcomm.qti.sva.EventTonePlayer;
import com.qualcomm.qti.sva.Global.SmState;
import com.qualcomm.qti.sva.ux.dev.DetectionActivity;
import com.qualcomm.qti.sva.ux.dev.SoundModelsActivity;

import android.hardware.soundtrigger.SoundTrigger;
import android.hardware.soundtrigger.SoundTrigger.ConfidenceLevel;
import android.hardware.soundtrigger.SoundTrigger.KeyphraseRecognitionExtra;
import android.hardware.soundtrigger.SoundTrigger.RecognitionConfig;
import android.hardware.soundtrigger.SoundTrigger.SoundModelEvent;
import android.hardware.soundtrigger.SoundTriggerModule;

/**
 * This is a hub class. It receives messages and distributes
 * them to registered clients.
 */
@SuppressWarnings("deprecation")
public class VwuService extends Service implements SoundTrigger.StatusListener {
    private final static String TAG = "ListenLog.VwuService";
    private final static int DISPLAY_ON_DURATION = 5000;
    private final static int VOICEWAKEUP_NOTIFICATION_ID = 1041;

    public ArrayList<Messenger> mClients = new ArrayList<Messenger>();
    private NotificationManager notificationManager;
    private EventTonePlayer tonePlayer;
    private PowerManager.WakeLock wakeLock = null;
    private Timer wakelockTimer = null;
    private KeyguardLock keyguardLock = null;
    private Context context;
    SoundTriggerModule stModule = null;
    private boolean isServiceStateEnabled = true;
    AsyncTask<String, Void, Integer> lookAheadBufferTask = null;
    protected boolean detachAfterServiceStateEnabled = false;
    boolean useStKeyphraseSmFromXml = false;
    SmSessionManager smSessionManager;

    public class LocalBinder extends Binder {
        public VwuService getService() {
            return VwuService.this;
        }
    }

    public IBinder onBind(Intent intent) {
        return receiveMessenger.getBinder();
    }

    public boolean onUnbind(Intent intent) {
        return super.onUnbind(intent);
    }

    public int onStartCommand(Intent _intent, int flags, int startId) {
        return START_STICKY;
    }

    public void onCreate() {
        Log.v(TAG, "onCreate");
        context = getApplicationContext();
        notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

        // Create smSessionManager to manage the state of SMs
        smSessionManager = new SmSessionManager();

        // Create toneplayer for success and failure event tone playback.
        Log.v(TAG, "onCreate: try creating tonePlayer");
        tonePlayer = new EventTonePlayer(this);
        Log.v(TAG, "onCreate: tonePlayer created");

        // Create a wakeLock to turn the display on when detection occurs.
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK |
                                  PowerManager.ACQUIRE_CAUSES_WAKEUP, "svaLock");

        // Create directories on internal storage for the app to use.
        Utils.createDirIfNotExists(Global.PATH_APP);
        Utils.createDirIfNotExists(Global.PATH_TRAINING_RECORDINGS);
        Utils.createDirIfNotExists(Global.PATH_VOICE_REQUESTS);

        // Copy sound models to internal storage.
        Utils.copyAssetsToStorage(this, Global.PATH_APP);

        Log.v(TAG, "user sound model output location= " + Global.PATH_APP);
    }

    public void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "ondestroy");
        notificationManager.cancelAll();
        tonePlayer.release();
    }

    public Handler receiveHandler = new Handler() {
        UUID uuid = null;
        UUID vendorUuid = null;
        byte[] opaqueData = null;

        public void handleMessage(Message msg) {
            int returnStatus = ListenTypes.STATUS_EFAILURE;
            switch (msg.what) {
                // Called by the activities that need to communicate with VwuService.java before
                // initiating communication.
                case MessageType.MSG_REGISTER_CLIENT:
                    Log.v(TAG, "MSG_REGISTER_CLIENT in handler");
                    if (false == mClients.contains(msg.replyTo)) {
                        Log.v(TAG, "MSG_REGISTER_CLIENT: adding client");
                        mClients.add(msg.replyTo);
                        Log.v(TAG, "MSG_REGISTER_CLIENT: mClients size now= " + mClients.size());
                    } else {
                        Log.v(TAG, "MSG_REGISTER_CLIENT: client already registered");
                    }
                    Log.v(TAG, "MSG_REGISTER_CLIENT: before mClients.size() == 1");
                    if(mClients.size() == 1) {
                        Log.v(TAG, "MSG_REGISTER_CLIENT: inside mClients.size() == 1");
                        // attach the SoundTrigger module once
                        ArrayList<SoundTrigger.ModuleProperties> modules = new
                                ArrayList<SoundTrigger.ModuleProperties>();
                        Log.v(TAG, "MSG_REGISTER_CLIENT: modules newed");
                        SoundTrigger.listModules(modules);
                        Log.v(TAG, "MSG_REGISTER_CLIENT: listModules");
                        if (modules.size() < 1) {
                            Log.e(TAG, "MSG_REGISTER_CLIENT: no available modules.");
                        } else {
                            Log.v(TAG, "MSG_REGISTER_CLIENT: attach module id = " + modules.get(0).id);
                            Log.v(TAG, "MSG_REGISTER_CLIENT: attach module UUID = " + modules.get(0).uuid);
                            Log.v(TAG, "MSG_REGISTER_CLIENT: attach module UUID most sig long = " + modules.get(0).uuid.getMostSignificantBits());
                            Log.v(TAG, "MSG_REGISTER_CLIENT: attach module UUID least sig long = " + modules.get(0).uuid.getLeastSignificantBits());
                            vendorUuid = modules.get(0).uuid;
                            stModule = SoundTrigger.attachModule(modules.get(0).id, VwuService.this, null);
                        }
                    }

                    sendMessageDataAll(MessageType.MSG_CLIENT_REGISTERED, Global.SUCCESS, null);
                    break;

                // Called by the activities that need to communicate with VwuService.java after
                // communication is no longer needed.
                case MessageType.MSG_UNREGISTER_CLIENT:
                    Log.v(TAG, "MSG_UNREGISTER_CLIENT in handler");

                    if ((mClients.size() == 1) && (stModule != null)) {
                        Log.v(TAG, "MSG_UNREGISTER_CLIENT: detach sound trigger module");
                        if (smSessionManager.getIsASessionBuffering()) {
                            Log.v(TAG, "MSG_UNREGISTER_CLIENT: isASessionBuffering is true");
                            boolean isLabCanceled = lookAheadBufferTask.cancel(true);
                            if (false == isLabCanceled) {
                                Log.v(TAG, "MSG_UNREGISTER_CLIENT: could not stop " +
                                        "lookAheadBufferTask. This is most likely because it has" +
                                        " already completed.");
                                // Even though LAB has already completed, it's safer to detach
                                // the module after serviceStateEnabled is sent to avoid a race
                                // condition.
                            } else {
                                //TODO Remove this temporary workaround when Google fixes
                                // StatusListener and introduces SoundModuleListener.
                                //
                                // LAB was cancelled, so wait until that completes,
                                // serviceStateEnabled is sent, and then detach module.
                                Log.v(TAG, "MSG_UNREGISTER_CLIENT: LAB was cancelled, so wait " +
                                        "until that completes, serviceStateEnabled is sent " +
                                        "and then detach module.");
                            }
                            detachAfterServiceStateEnabled = true;
                            Log.v(TAG, "MSG_UNREGISTER_CLIENT: detachAfterServiceStateEnabled set to true.");
                        } else {
                            Log.v(TAG, "MSG_UNREGISTER_CLIENT: isASessionBuffering is false");
                            stModule.detach();
                            smSessionManager.updateStateServiceDied();
                            sendMessageDataAll(MessageType.MSG_LISTEN_STOPPED, Global.SUCCESS, null);
                        }
                    }

                    if (mClients.remove(msg.replyTo)) {
                        Log.v(TAG, "MSG_UNREGISTER_CLIENT: client removed from mClients successfully");
                    } else {
                        Log.e(TAG, "MSG_UNREGISTER_CLIENT: client cannot be removed from mClients");
                    }
                    Log.v(TAG, "MSG_UNREGISTER_CLIENT: mClients size now= " + mClients.size());
                    break;

                // Called by the HomeActivity when starting as long as the setting wasn't
                // disabled when the app was last closed.
                // Called by the Settings Activity when the Enable/Disable Listen checkbox is
                // checked.
                case MessageType.MSG_LISTEN_SET_PARAM:
                    Log.v(TAG, "MSG_LISTEN_SET_PARAM in handler");
                    if (msg.arg1 == MessageType.MSG_ENABLE) {
                            sendMessageDataAll(MessageType.MSG_LISTEN_ENABLED, Global.SUCCESS, null);
                            Log.v(TAG, "MSG_LISTEN_SET_PARAM: listen enabled skippped");
                    } else if (msg.arg1 == MessageType.MSG_DISABLE) {
                            sendMessageDataAll(MessageType.MSG_LISTEN_DISABLED, Global.SUCCESS, null);
                            Log.v(TAG, "MSG_LISTEN_SET_PARAM: listen disabled skippped");
                    }

                    break;

                // Called by the Settings Activity when starting up to determine if the Listen
                // feature is enabled or disabled.
                case MessageType.MSG_LISTEN_GET_PARAM:
                    Log.v(TAG, "MSG_LISTEN_GET_PARAM in handler");
                    Log.v(TAG, "MSG_LISTEN_GET_PARAM: listen is assumed to be ENABLED for ST");
                    sendMessageDataAll(MessageType.MSG_LISTEN_ENABLED, MessageType.MSG_LISTEN_GET_PARAM, null);
                    break;

                // Called by the HomeActivity when starting as long as the setting wasn't
                // disabled when the app was last closed.
                // Called by the Settings Activity when the Enable/Disable VoiceWakeup checkbox is
                // checked.
                case MessageType.MSG_VOICEWAKEUP_SET_PARAM:
                    Log.v(TAG, "MSG_VOICEWAKEUP_SET_PARAM in handler");

                    // force return value as set
                    if (msg.arg1 == MessageType.MSG_ENABLE) {
                            Log.v(TAG, "MSG_VOICEWAKEUP_SET_PARAM: enable voicewakeup skipped");
                            sendMessageDataAll(MessageType.MSG_VOICEWAKEUP_ENABLED, Global.SUCCESS, null);
                    } else if (msg.arg1 == MessageType.MSG_DISABLE) {
                            Log.v(TAG, "MSG_VOICEWAKEUP_SET_PARAM: disable voicewakeup skipped");
                            sendMessageDataAll(MessageType.MSG_VOICEWAKEUP_DISABLED, Global.SUCCESS, null);
                    }
                    break;

                // Called by the Settings Activity when starting up to determine if the VoiceWakeup
                // feature is enabled or disabled.
                case MessageType.MSG_VOICEWAKEUP_GET_PARAM:
                    Log.v(TAG, "MSG_VOICEWAKEUP_GET_PARAM in handler");
                    Log.v(TAG, "MSG_VOICEWAKEUP_GET_PARAM: voicewakeup is force to return ENABLED for ST");
                    sendMessageDataAll(MessageType.MSG_VOICEWAKEUP_ENABLED,
                                           MessageType.MSG_VOICEWAKEUP_GET_PARAM,
                                           null);
                    break;

                // Called during training to verify the user's recorded training matched the keyword.
                case MessageType.MSG_VERIFY_RECORDING:
                    Log.v(TAG, "MSG_VERIFY_RECORDING in handler");
                    String trainingKeyword = null;
                    boolean isUdk = (msg.arg1 == 1);
                    Log.v(TAG, "MSG_VERIFY_RECORDING: isUdk= " + String.valueOf(isUdk));
                    if (null != msg.obj) {
                        trainingKeyword = (String) msg.obj;
                    }
                    Log.v(TAG, "MSG_VERIFY_RECORDING: trainingKeyword= " + trainingKeyword);
                    Log.v(TAG, "MSG_VERIFY_RECORDING: size of last training recording= " +
                            Global.getInstance().getLastUserRecording().position());
                    int confidenceLevel = -1;
                    if (isUdk) {
                        Log.v(TAG, "MSG_VERIFY_RECORDING: calling verifyUdkRecording for UDK.");
                        confidenceLevel = ListenSoundModel.verifyUdkRecording(
                                Global.getInstance().getLanguageModel(),
                                Global.getInstance().getLastUserRecording());
                    } else {
                        Log.v(TAG, "MSG_VERIFY_RECORDING: calling verifyUserRecording for PDK.");
                        confidenceLevel = ListenSoundModel.verifyUserRecording(
                                Global.getInstance().getSmRepo().getSoundModelByteBufferToExtend(),
                                trainingKeyword,
                                Global.getInstance().getLastUserRecording());
                    }
                    if (confidenceLevel <= 0) {
                        Log.e(TAG, "MSG_VERIFY_RECORDING: verifyUserRecording failed with return " +
                                "status= " + confidenceLevel);
                        sendMessageDataAll(MessageType.MSG_RECORDING_RESULT, confidenceLevel, null);
                        break;
                    }
                    // debugging: use this to stop training from continuing past the first recording.
                    /*else if (confidenceLevel < 100){
                        break;
                    }*/
                    Log.v(TAG, "MSG_VERIFY_RECORDING: mClients size now= " + mClients.size());
                    sendMessageDataAll(MessageType.MSG_RECORDING_RESULT, confidenceLevel, null);
                    break;

                // Called after a user completes training to create a sound model with user data
                case MessageType.MSG_EXTEND_SOUNDMODEL:
                    Log.v(TAG, "MSG_EXTEND_SOUNDMODEL in handler");
                    String[] keyphraseUserIsudk = (String[]) msg.obj;
                    Log.v(TAG, "MSG_EXTEND_SOUNDMODEL: " +
                            "keyphraseUserIsudk[0]=" + keyphraseUserIsudk[0] +
                            ", keyphraseUserIsudk[1]=" + keyphraseUserIsudk[1] +
                            ", keyphraseUserIsudk[2]=" + keyphraseUserIsudk[2]);
                    if (null != Global.getInstance().getSmRepo().getSoundModelByteBufferToExtend()) {
                        Log.v(TAG, "MSG_EXTEND_SOUNDMODEL: getSoundModelByteBufferToExtend() is not null");
                    }
                    new CreateOrExtendSmTask().execute(keyphraseUserIsudk);
                    break;

                case MessageType.MSG_QUERY_SM:
                    Log.v(TAG, "MSG_QUERY_SM in handler");
                    ByteBuffer soundModelToQuery = Global.getInstance().getSmRepo().
                            getSoundModelByteBufferToQuery();
                    if (null == soundModelToQuery) {
                        Log.e(TAG, "MSG_QUERY_SM: no sound model by that name found");
                        break;
                    }
                    ListenTypes.SoundModelInfo smInfo = ListenSoundModel.query(soundModelToQuery);
                    if (null == smInfo) {
                        Log.e(TAG, "MSG_QUERY_SM: query SM failed");
                        break;
                    } else {
                        Global.getInstance().getSmRepo().addSmInfoFromQuery(smInfo, null); //temp code
                    }

                    sendMessageDataAll(MessageType.MSG_QUERY_COMPLETED, Global.SUCCESS, null);
                    break;

                case MessageType.MSG_DELETE_FROM_SOUNDMODEL:
                    Log.v(TAG, "MSG_DELETE_FROM_SOUNDMODEL in handler");

                    ByteBuffer soundModelByteBuffer = Global.getInstance().getSmRepo().
                            getSoundModelByteBufferToDeleteFrom();
                    if (null == soundModelByteBuffer) {
                        Log.e(TAG, "sound model does not exist");
                        sendMessageDataAll(MessageType.MSG_DELETE_FROM_SOUNDMODEL, Global.FAILURE, null);
                        break;
                    }

                    String keyPhrase = msg.getData().getString(Global.TAG_KEYPHRASE_TO_DELETE);
                    String user = msg.getData().getString(Global.TAG_USER_TO_DELETE);

                    if (keyPhrase != null && user == null) {
                        Log.v(TAG, "MSG_DELETE_FROM_SOUNDMODEL: delete keyPhrase: " + keyPhrase);
                    } else if (keyPhrase != null && user != null){
                        Log.v(TAG, "MSG_DELETE_FROM_SOUNDMODEL: delete keyPhrase: " + keyPhrase +
                                ",  user: " + user);
                    } else if (keyPhrase == null && user != null){
                        Log.v(TAG, "MSG_DELETE_FROM_SOUNDMODEL: delete user: " + user);
                    }

                    int sizeAfterDeletion = ListenSoundModel.getSizeAfterDelete(soundModelByteBuffer,
                            keyPhrase, user);
                    if (sizeAfterDeletion < 0) {
                        sendMessageDataAll(MessageType.MSG_DELETE_FROM_SOUNDMODEL, Global.FAILURE, null);
                        break;
                    }
                    Global.getInstance().getSmRepo().createSoundModelToModifyByteBuffer(
                            sizeAfterDeletion);

                    if (ListenTypes.STATUS_SUCCESS == ListenSoundModel.deleteData(
                            soundModelByteBuffer,
                            keyPhrase,
                            user,
                            Global.getInstance().getSmRepo().getSoundModelToModifyByteBuffer())) {
                        Log.v(TAG, "MSG_DELETE_FROM_SOUNDMODEL: deletion successful");
                        Global.getInstance().getSmRepo().saveSoundModelToDeleteFrom();
                        sendMessageDataAll(MessageType.MSG_DELETE_FROM_SOUNDMODEL, Global.SUCCESS, null);
                    } else {
                        sendMessageDataAll(MessageType.MSG_DELETE_FROM_SOUNDMODEL, Global.FAILURE, null);
                    }
                    break;

                case MessageType.MSG_CREATE_MERGED_SM:
                    Log.v(TAG, "MSG_CREATE_MERGED_SM in handler");
                    ArrayList<ByteBuffer> smToMergeByteBuffers =
                            Global.getInstance().getSmRepo().getSoundModelsToMergeByteBuffers();
                    ByteBuffer[] soundModelsToMergeByteBuffers =
                            smToMergeByteBuffers.toArray(new ByteBuffer[smToMergeByteBuffers.size()]);
                    if (null == soundModelsToMergeByteBuffers) {
                        Log.e(TAG, "MSG_CREATE_MERGED_SM: sound models do not exist");
                        break;
                    }

                    int sizeAfterMerge = ListenSoundModel.getSizeWhenMerged(
                            soundModelsToMergeByteBuffers);
                    if (sizeAfterMerge < 0) {
                        sendMessageDataAll(MessageType.MSG_CREATE_MERGED_SM, Global.FAILURE, null);
                        break;
                    }
                    Global.getInstance().getSmRepo().createSoundModelToModifyByteBuffer(sizeAfterMerge);

                    if (ListenTypes.STATUS_SUCCESS == ListenSoundModel.merge(
                            soundModelsToMergeByteBuffers,
                            Global.getInstance().getSmRepo().getSoundModelToModifyByteBuffer())) {
                        Log.v(TAG, "MSG_CREATE_MERGED_SM: merge successful");
                        Global.getInstance().getSmRepo().saveMergedSoundModel();
                        sendMessageDataAll(MessageType.MSG_CREATE_MERGED_SM, Global.SUCCESS, null);
                    } else {
                        sendMessageDataAll(MessageType.MSG_CREATE_MERGED_SM, Global.FAILURE, null);
                    }
                    break;

                case MessageType.MSG_LOAD_SM:
                    Log.v(TAG, "MSG_LOAD_SM in handler");

                    if (isServiceStateEnabled == false) {
                        Log.e(TAG, "MSG_LOAD_SM: ST Service is not enabled. Cannot continue.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }
                    if (stModule == null) {
                        Log.e(TAG, "MSG_LOAD_SM: Module was NOT attached at start up.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }
                    String smNameToLoad = (String) msg.obj;
                    if (null == smNameToLoad) {
                        Log.e(TAG, "MSG_LOAD_SM: loadSoundModel failed because no smName was passed.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }

                    // Determine recognition mode from settings.
                    int stRecognitionMode = -1;
                    int detectionMode = Global.getInstance().getDetectionMode();
                    if (detectionMode == ListenTypes.KEYWORD_ONLY_DETECTION_MODE) {
                        stRecognitionMode = SoundTrigger.RECOGNITION_MODE_VOICE_TRIGGER;
                    } else if (detectionMode == ListenTypes.USER_KEYWORD_DETECTION_MODE) {
                        stRecognitionMode = SoundTrigger.RECOGNITION_MODE_VOICE_TRIGGER +
                                SoundTrigger.RECOGNITION_MODE_USER_IDENTIFICATION;
                    }
                    Log.v(TAG, "MSG_LOAD_SM: stRecognitionMode= " + stRecognitionMode);


                    // inform HAL of number of sessions that will be used
                    AudioSystem.setParameters("SVA_NUM_SESSIONS=1");

                    ByteBuffer smBbToLoad = Global.getInstance().getSmRepo().
                                                    getSoundModelByteBufferFromName(smNameToLoad);
                    if (null != smBbToLoad) {
                        //for debugging
                        StringBuilder sb1 = new StringBuilder();
                        for (int j=0; j<16; j++) {
                            sb1.append(String.format("%02X ", smBbToLoad.get(j)));
                        }
                        Log.v(TAG, "bytes #1 (after getting smBbToLoad= " + sb1.toString());
                    } else {
                        Log.e(TAG, "MSG_LOAD_SM: loadSoundModel failed because no ByteBuffer" +
                        		"is available for SM= " + smNameToLoad);
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }
                    //get info for that SM
                    smInfo = ListenSoundModel.query(smBbToLoad);
                    if (null == smInfo) {
                        useStKeyphraseSmFromXml = true; // if smInfo isn't null, use it.
                    }

                    ArrayList<String> userKeyphrasePairIndices = new ArrayList<String>();
                    KeywordInfo[] kwInfos;
                    SoundTrigger.Keyphrase[] keyphrases;
                    SoundTrigger.KeyphraseRecognitionExtra[] kpRecExtras;

                    if (null != smInfo) {
                    Global.getInstance().getSmRepo().setSoundModelNameToQuery(smNameToLoad);

                    // Create ST SM to pass to loadSoundModel().
                    SVASoundModelInfo v2SoundModelInfo = (SVASoundModelInfo) smInfo;
                    kwInfos = v2SoundModelInfo.keywordInfo;
                    keyphrases =
                            new SoundTrigger.Keyphrase[kwInfos.length];
                    kpRecExtras =
                            new SoundTrigger.KeyphraseRecognitionExtra[kwInfos.length];

                    // Create ArrayList of userKeyphrasePairs where the index in the ArrayList
                    // is the index into the array that is used to create the userIDs list.

                    // First add keyphrase indices to list.
                    for (KeywordInfo keyphraseInfo : v2SoundModelInfo.keywordInfo) {
                        userKeyphrasePairIndices.add(keyphraseInfo.keywordPhrase);
                    }
                    // After keyphrase indices Add user|keyphrase pairs indices to list.
                    for (String activeUser : v2SoundModelInfo.userNames) {
                        for (KeywordInfo keyphraseInfo : v2SoundModelInfo.keywordInfo) {
                            for (String keyphraseUser : keyphraseInfo.activeUsers) {
                                Log.v(TAG, "MSG_LOAD_SM: for activeUser= " +
                                        " activeUser= " + activeUser + " check " +
                                        keyphraseInfo.keywordPhrase + "." + keyphraseUser);
                                if (activeUser.equals(keyphraseUser)) {
                                    userKeyphrasePairIndices.add(activeUser + "|" +
                                            keyphraseInfo.keywordPhrase);
                                    Log.v(TAG, "MSG_LOAD_SM: added " + activeUser +
                                            "|" + keyphraseInfo.keywordPhrase + " with array "
                                            + "index= " + (userKeyphrasePairIndices.size()-1));
                                }
                            }
                        }
                    }
                    Global.getInstance().getSmRepo().addSmInfoFromQuery(smInfo,
                            userKeyphrasePairIndices);

                    // For all keyphrases
                    for (int j=0; j<kwInfos.length; j++) {
                        // Create ST keyphrase objects.
                        KeywordInfo keyphraseInfo = kwInfos[j];
                        // Add all userIDs from userKeyphrasePairs.
                        int[] userIDs = new int[keyphraseInfo.activeUsers.length];
                        int userIDsIndex = 0;
                        int uKPairIndex = -1;
                        for (String keyphraseUser : keyphraseInfo.activeUsers) {
                            String userKeyphraseTokenized = keyphraseUser + "|" +
                                    keyphraseInfo.keywordPhrase;
                            uKPairIndex = userKeyphrasePairIndices.indexOf(userKeyphraseTokenized);
                            Log.v(TAG, "MSG_LOAD_SM: found uKPairIndex= " +
                                    uKPairIndex + " for userKeyphraseTokenized= " +
                                    userKeyphraseTokenized);
                            if (uKPairIndex != -1) {
                                userIDs[userIDsIndex++] = uKPairIndex;
                                Log.v(TAG, "MSG_LOAD_SM: adding uKPairIndex= " +
                                        uKPairIndex + " for userKeyphraseTokenized= " +
                                        userKeyphraseTokenized);
                            } else {
                                Log.e(TAG, "MSG_LOAD_SM: The userKeyphrasePair= " +
                                        userKeyphraseTokenized + " was not found.");
                                sendMessageDataAll(MessageType.MSG_LOAD_SM,
                                        Global.FAILURE, null);
                                return;
                            }
                        }
                        SoundTrigger.Keyphrase stKeyphrase = new SoundTrigger.Keyphrase(j,
                                stRecognitionMode, "en_US", keyphraseInfo.keywordPhrase, userIDs);
                        keyphrases[j] = stKeyphrase;

                        // Create ST KeyphraseRecognitionExtra objects.
                        SoundTrigger.ConfidenceLevel[] confLevels =
                                new SoundTrigger.ConfidenceLevel[keyphraseInfo.activeUsers.length];
                        if (keyphraseInfo.activeUsers.length != stKeyphrase.users.length) {
                            Log.e(TAG, "MSG_LOAD_SM: " +
                                    "keyphraseInfo.activeUsers.length != " +
                                    "stKeyphrase.users.length.");
                            sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                            return;
                        }
                        // For all users.
                        for (int k=0; k<keyphraseInfo.activeUsers.length; k++) {
                            String userName = keyphraseInfo.activeUsers[k];
                            int userConfLevel = Global.getInstance().getSmRepo().
                                    getConfidenceLevel(getApplicationContext(), smNameToLoad,
                                            stKeyphrase.text, userName);
                            Log.v(TAG, "MSG_LOAD_SM: added confLevel= " +
                                    userConfLevel + " for keyphrase= " + stKeyphrase.text +
                                    ", user= " + userName);
                            SoundTrigger.ConfidenceLevel confLevel =
                                    new SoundTrigger.ConfidenceLevel(stKeyphrase.users[k],
                                            userConfLevel);
                            confLevels[k] = confLevel;
                        }
                        int keyphraseConfLevel = Global.getInstance().getSmRepo().
                                getConfidenceLevel(getApplicationContext(), smNameToLoad,
                                        keyphrases[j].text, null);
                        Log.v(TAG, "MSG_LOAD_SM: added confLevel= " +
                                        keyphraseConfLevel + " for keyphrase= " +
                                        keyphrases[j].text);
                        SoundTrigger.KeyphraseRecognitionExtra kre =
                                new SoundTrigger.KeyphraseRecognitionExtra(
                                        j, 0, keyphraseConfLevel, confLevels);
                        kpRecExtras[j] = kre;
                    }
                    uuid = new UUID(0, 0);
                    } else{
                        Log.v(TAG, "MSG_LOAD_SM: useStKeyphraseSmFromXml is true");
                        userKeyphrasePairIndices.add("OK Google");
                        createStksmInfoFromXml(smNameToLoad);
                        try {
                            smBbToLoad = Utils.readFileToByteBuffer(Global.PATH_APP + "/" + smNameToLoad);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        int[] userIDs = new int[0];
                        SoundTrigger.Keyphrase stKeyphrase =
                                new SoundTrigger.Keyphrase(0, stRecognitionMode, "en_US", "OK Google", userIDs);
                        keyphrases = new SoundTrigger.Keyphrase[1];
                        keyphrases[0] = stKeyphrase;

                        SoundTrigger.ConfidenceLevel[] confLevels =
                                new SoundTrigger.ConfidenceLevel[0];
                        SoundTrigger.KeyphraseRecognitionExtra krextra =
                                new SoundTrigger.KeyphraseRecognitionExtra(0, 1, 69, confLevels);
                        kpRecExtras =
                                new SoundTrigger.KeyphraseRecognitionExtra[1];
                        kpRecExtras[0] = krextra;
                    }

                    StringBuilder sb2 = new StringBuilder();
                    for (int j=0; j<16; j++) {
                        sb2.append(String.format("%02X ", smBbToLoad.get(j)));
                    }
                    Log.v(TAG, "bytes #2 (before creating keyphraseSm)= " + sb2.toString());
                    Log.v(TAG, "bytes position before rewind= " + smBbToLoad.position());
                    smBbToLoad.rewind(); // in case any reading was done and position moved
                    Log.v(TAG, "bytes position after rewind= " + smBbToLoad.position());
                    Log.v(TAG, "bytes remaining= " + smBbToLoad.remaining());
                    byte[] smByteArray = new byte[smBbToLoad.remaining()];
                    smBbToLoad.get(smByteArray);

                    SoundTrigger.KeyphraseSoundModel stKeyphraseSoundModel =
                            new SoundTrigger.KeyphraseSoundModel(uuid, vendorUuid, smByteArray, keyphrases);
                    int[] soundModelHandles = new int[20];
                    StringBuilder sb3 = new StringBuilder();
                    for (int j=0; j<16; j++) {
                        sb3.append(String.format("%02X ", stKeyphraseSoundModel.data[j]));
                    }
                    Log.v(TAG, "bytes #3 (from keyphraseSm before calling loadSoundModel)= " +
                            sb3.toString());
                    Log.v(TAG, "stKeyphraseSoundModel= " + stKeyphraseSoundModel.toString());
                    StringBuilder sb = new StringBuilder();
                    for (SoundTrigger.Keyphrase keyphrase : stKeyphraseSoundModel.keyphrases) {
                        sb.append("\tkeyphrase.id= " + keyphrase.id + "\n");
                        sb.append("\tkeyphrase.recognitionModes= " + keyphrase.recognitionModes + "\n");
                        sb.append("\tkeyphrase.locale= " + keyphrase.locale + "\n");
                        sb.append("\tkeyphrase.text= " + keyphrase.text + "\n");
                        for (int user1 : keyphrase.users) {
                            sb.append("\t\tuser= " + user1 + "\n");
                        }
                    }
                    Log.v(TAG, "recogConfig= " + sb.toString());

                    /****************
                     * loadSoundModel
                     ***************/
                    Log.v(TAG, "MSG_LOAD_SM: calling loadSoundModel");
                    returnStatus = stModule.loadSoundModel(stKeyphraseSoundModel,
                            soundModelHandles);
                    if (returnStatus != SoundTrigger.STATUS_OK) {
                        Log.e(TAG, "MSG_LOAD_SM: loadSoundModel failed with " +
                                "returnStatus= " + returnStatus);
                        sendMessageDataAll(MessageType.MSG_REGISTER_SOUNDMODEL, Global.FAILURE, null);
                        break;
                    }
                    Log.v(TAG, "MSG_LOAD_SM: loadSoundModel returned.");
                    for (int j=0; j<2; j++) {
                        Log.v(TAG, "MSG_LOAD_SM: soundModelHandle[" + j + "]: " +
                                soundModelHandles[j]);
                    }
                    int soundModelHandle = soundModelHandles[0];
                    smSessionManager.addLoadedSmSession(smNameToLoad, soundModelHandle, kpRecExtras);

                    sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.SUCCESS, null);
                    break;

                // TODO CSVAA combine with MSG_LOAD_SM and check for CSM by file extn (.uimc vs .uim).
                case MessageType.MSG_LOAD_COM_SM:
                    Log.v(TAG, "MSG_LOAD_COM_SM in handler");

                    // Error checks.
                    if (isServiceStateEnabled == false) {
                        Log.e(TAG, "MSG_LOAD_COM_SM: ST Service is not enabled. Cannot continue.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }
                    if (stModule == null) {
                        Log.e(TAG, "MSG_LOAD_COM_SM: Module was NOT attached at start up.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }
                    smNameToLoad = (String) msg.obj;
                    if (null == smNameToLoad) {
                        Log.e(TAG, "MSG_LOAD_COM_SM: loadSoundModel failed because no smName was passed.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }

                    // Determine recognition mode from settings.
                    stRecognitionMode = -1;
                    Boolean isUserVerificationEnabled = Global.getInstance().getSmRepo().
                            getIsUserVerificationEnabled(smNameToLoad);
                    // Error check.
                    if (null == isUserVerificationEnabled) {
                        Log.e(TAG, "MSG_LOAD_COM_SM: loadSoundModel failed because smNameToLoad= " +
                                smNameToLoad + "could not be found.");
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }
                    // Set recognitionMode and userIds.
                    int[] userIDs = null;
                    if (isUserVerificationEnabled) {
                        stRecognitionMode = SoundTrigger.RECOGNITION_MODE_VOICE_TRIGGER +
                                SoundTrigger.RECOGNITION_MODE_USER_IDENTIFICATION;
                        userIDs = new int[] {1};
                    } else {
                        stRecognitionMode = SoundTrigger.RECOGNITION_MODE_VOICE_TRIGGER;
                        userIDs = new int[0];
                    }
                    Log.v(TAG, "MSG_LOAD_COM_SM: stRecognitionMode= " + stRecognitionMode);

                    // Inform HAL of number of sessions that will be used.
                    AudioSystem.setParameters("SVA_NUM_SESSIONS=1");

                    // Get SM byteBuffer.
                    smBbToLoad = Global.getInstance().getSmRepo().
                                                    getSoundModelByteBufferFromName(smNameToLoad);
                    // Error check.
                    if (null != smBbToLoad) {
                        //for debugging
                        StringBuilder sb1 = new StringBuilder();
                        for (int j=0; j<16; j++) {
                            sb1.append(String.format("%02X ", smBbToLoad.get(j)));
                        }
                        Log.v(TAG, "bytes #1 (after getting smBbToLoad= " + sb1.toString());
                    } else {
                        Log.e(TAG, "MSG_LOAD_COM_SM: loadSoundModel failed because no ByteBuffer" +
                                "is available for SM= " + smNameToLoad);
                        sendMessageDataAll(MessageType.MSG_LOAD_SM, Global.FAILURE, null);
                        break;
                    }

                    // Create ST SM to pass to loadSoundModel().
                    keyphrases = new SoundTrigger.Keyphrase[1];
                    keyphrases[0] = new SoundTrigger.Keyphrase(0, stRecognitionMode, "en_US", "", userIDs);

                    // Create ST KeyphraseRecognitionExtra objects.
                    kpRecExtras =
                            new SoundTrigger.KeyphraseRecognitionExtra[1];
                    SoundTrigger.ConfidenceLevel[] confLevels;
                    if (isUserVerificationEnabled) {
                        confLevels = new SoundTrigger.ConfidenceLevel[1];
                        confLevels[0] = new SoundTrigger.ConfidenceLevel(1,
                                Global.getInstance().getSettingUserConfidenceLevel());
                    } else {
                        confLevels = new SoundTrigger.ConfidenceLevel[0];
                    }
                    kpRecExtras[0] = new SoundTrigger.KeyphraseRecognitionExtra(0, 0,
                            Global.getInstance().getSettingKeyPhraseConfidenceLevel(), confLevels);

                    uuid = new UUID(0, 0);

                    // Convert ByteBuffer to ByteArray
                    sb2 = new StringBuilder();
                    for (int j=0; j<16; j++) {
                        sb2.append(String.format("%02X ", smBbToLoad.get(j)));
                    }
                    Log.v(TAG, "bytes #2 (before creating keyphraseSm)= " + sb2.toString());
                    Log.v(TAG, "bytes position before rewind= " + smBbToLoad.position());
                    smBbToLoad.rewind(); // In case any reading was done and position moved.
                    Log.v(TAG, "bytes position after rewind= " + smBbToLoad.position());
                    Log.v(TAG, "bytes remaining= " + smBbToLoad.remaining());
                    smByteArray = new byte[smBbToLoad.remaining()];
                    smBbToLoad.get(smByteArray);

                    stKeyphraseSoundModel =
                            new SoundTrigger.KeyphraseSoundModel(uuid, vendorUuid, smByteArray, keyphrases);
                    soundModelHandles = new int[20];
                    sb3 = new StringBuilder();
                    for (int j=0; j<16; j++) {
                        sb3.append(String.format("%02X ", stKeyphraseSoundModel.data[j]));
                    }
                    Log.v(TAG, "bytes #3 (from keyphraseSm before calling loadSoundModel)= " +
                            sb3.toString());
                    Log.v(TAG, "MSG_LOAD_COM_SM: stKeyphraseSoundModel= " + stKeyphraseSoundModel.toString());
                    sb = new StringBuilder();
                    for (SoundTrigger.Keyphrase keyphrase : stKeyphraseSoundModel.keyphrases) {
                        sb.append("\tkeyphrase.id= " + keyphrase.id + "\n");
                        sb.append("\tkeyphrase.recognitionModes= " + keyphrase.recognitionModes + "\n");
                        sb.append("\tkeyphrase.locale= " + keyphrase.locale + "\n");
                        sb.append("\tkeyphrase.text= " + keyphrase.text + "\n");
                        for (int user1 : keyphrase.users) {
                            sb.append("\t\tuser= " + user1 + "\n");
                        }
                    }
                    Log.v(TAG, "MSG_LOAD_COM_SM: recogConfig= " + sb.toString());

                    /****************
                     * loadSoundModel
                     ***************/
                    Log.v(TAG, "MSG_LOAD_COM_SM: calling loadSoundModel.");
                    returnStatus = stModule.loadSoundModel(stKeyphraseSoundModel,
                            soundModelHandles);
                    if (returnStatus != SoundTrigger.STATUS_OK) {
                        Log.e(TAG, "MSG_LOAD_COM_SM: loadSoundModel failed with " +
                                "returnStatus= " + returnStatus);
                        sendMessageDataAll(MessageType.MSG_REGISTER_SOUNDMODEL, Global.FAILURE, null);
                        break;
                    }
                    Log.v(TAG, "MSG_LOAD_COM_SM: loadSoundModel returned.");
                    for (int j=0; j<2; j++) {
                        Log.v(TAG, "MSG_LOAD_COM_SM: soundModelHandle[" + j + "]: " +
                                soundModelHandles[j]);
                    }
                    soundModelHandle = soundModelHandles[0];
                    smSessionManager.addLoadedSmSession(smNameToLoad, soundModelHandle, kpRecExtras);

                    sendMessageDataAll(MessageType.MSG_LOAD_COM_SM, Global.SUCCESS, null);
                    // No break because for CSVAA, selecting the SM does a load and a start.

                case MessageType.MSG_START_RECOGNITION:
                    Log.v(TAG, "MSG_START_RECOGNITION in handler");
                    String smNameToStart = (String) msg.obj;
                    if (null == smNameToStart) {
                        Log.e(TAG, "MSG_START_RECOGNITION: startRecognition failed because no " +
                        		"smName was passed.");
                        sendMessageDataAll(MessageType.MSG_START_RECOGNITION, Global.FAILURE, null);
                        break;
                    }

                    int smHandleToStart = smSessionManager.getSmHandle(smNameToStart, SmState.STARTED);
                    if (smHandleToStart == -1) {
                        Log.e(TAG, "MSG_START_RECOGNITION: startRecognition failed because no " +
                                "SM with smName= " + smNameToStart + " was loaded first.");
                        sendMessageDataAll(MessageType.MSG_START_RECOGNITION, Global.FAILURE, null);
                        break;
                    } else if (smHandleToStart == -2) {
                        Log.e(TAG, "MSG_START_RECOGNITION: startRecognition failed because " +
                                "SM with smName= " + smNameToStart + " is in the wrong state.");
                        sendMessageDataAll(MessageType.MSG_START_RECOGNITION, Global.FAILURE, null);
                        break;
                    }

                    SoundTrigger.KeyphraseRecognitionExtra[] kpRecogExtras =
                            smSessionManager.getLoadedSmSessionKpRecogExtras(smHandleToStart);
                    if (null == kpRecogExtras) {
                        Log.e(TAG, "MSG_START_RECOGNITION: startRecognition failed because no " +
                                "kpRecogExtras were found for smHandle= " + smHandleToStart);
                        sendMessageDataAll(MessageType.MSG_START_RECOGNITION, Global.FAILURE, null);
                        break;
                    }

                    boolean captureRequested;
                    if (Global.getInstance().getSettingVoiceRequestsEnabled()) {
                        Log.v(TAG, "MSG_START_RECOGNITION: recogConfig capture requested");
                        captureRequested = true;
                    } else {
                        Log.v(TAG, "MSG_START_RECOGNITION: recogConfig capture NOT requested");
                        captureRequested = false;
                    }
                    if (null != opaqueData) {
                        for (byte oneByte : opaqueData) {
                            Log.v(TAG, "MSG_START_RECOGNITION: opaqueData byte= " + oneByte);
                        }
                    } else if (useStKeyphraseSmFromXml){
                        Log.e(TAG, "MSG_START_RECOGNITION: Error, opaque data wasn't filled.");
                    } else {
                        Log.v(TAG, "MSG_START_RECOGNITION: opaque data not filled because " +
                        		"useStKeyphraseSmfromXML is false");
                    }
                    SoundTrigger.RecognitionConfig recogConfig =
                            new SoundTrigger.RecognitionConfig(captureRequested, false,
                                    kpRecogExtras, null);
                    // For debugging purposes.
                    Log.v(TAG, "recogConfig.toString()= " + recogConfig.toString());
                    sb = new StringBuilder();
                    for (SoundTrigger.KeyphraseRecognitionExtra kre : recogConfig.keyphrases) {
                        sb.append("\tkre.id= " + kre.id + "\n");
                        sb.append("\tkre.recognitionModes= " + kre.recognitionModes + "\n");
                    }
                    Log.v(TAG, "recogConfig= " + sb.toString());
                    /******************
                     * startRecognition
                     *****************/
                    returnStatus = stModule.startRecognition(smHandleToStart, recogConfig);
                    if (returnStatus == SoundTrigger.STATUS_OK) {
                        Log.v(TAG, "MSG_START_RECOGNITION: startRecognition succeeded " +
                            " for smName= " + smNameToStart);
                        smSessionManager.addStartedSmSession(smHandleToStart, recogConfig);
                        sendMessageDataAll(MessageType.MSG_START_RECOGNITION, Global.SUCCESS, null);
                    } else {
                        Log.e(TAG, "MSG_START_RECOGNITION: startRecognition failed for smName= " +
                                smNameToStart + " with retrunStatus= " + returnStatus);
                        sendMessageDataAll(MessageType.MSG_REGISTER_SOUNDMODEL, Global.FAILURE, null);
                    }
                    break;

                case MessageType.MSG_STOP_RECOGNITION:
                    Log.v(TAG, "MSG_STOP_RECOGNITION in handler");
                    String smNameToStop = (String) msg.obj;

                    // Error check.
                    if (null == smNameToStop) {
                        Log.e(TAG, "MSG_STOP_RECOGNITION: stopRecognition failed because no " +
                                "smName was passed.");
                        sendMessageDataAll(MessageType.MSG_STOP_RECOGNITION, Global.FAILURE, null);
                        break;
                    }

                    int smHandleToStop = smSessionManager.getSmHandle(smNameToStop, SmState.STOPPED);
                    // Error checks.
                    if (smHandleToStop == -1) {
                        Log.e(TAG, "MSG_STOP_RECOGNITION: stopRecognition failed because no " +
                                "SM with smName= " + smHandleToStop + " was found.");
                        sendMessageDataAll(MessageType.MSG_STOP_RECOGNITION, Global.FAILURE, null);
                        break;
                    } else if (smHandleToStop == -2) {
                        Log.e(TAG, "MSG_STOP_RECOGNITION: stopRecognition failed because " +
                                "SM with smName= " + smHandleToStop + " is in the wrong state.");
                        sendMessageDataAll(MessageType.MSG_STOP_RECOGNITION, Global.FAILURE, null);
                        break;
                    }

                    // Call StopRecognition.
                    Log.v(TAG, "MSG_STOP_RECOGNITION: calling stopRecognition");
                    returnStatus = stModule.stopRecognition(smHandleToStop);
                    if (returnStatus == SoundTrigger.STATUS_OK) {
                        Log.v(TAG, "MSG_STOP_RECOGNITION: stopRecognition succeeded " +
                            "for smName= " + smNameToStop);
                        smSessionManager.addStoppedSmSession(smHandleToStop);

                        Integer numSessionsAlive = smSessionManager.numStartedSessions;
                        sendMessageDataAll(MessageType.MSG_STOP_RECOGNITION, Global.SUCCESS, numSessionsAlive);
                    } else {
                        Log.e(TAG, "MSG_STOP_RECOGNITION: stopRecognition failed for smName= " +
                                smNameToStop + " with retrunStatus= " + returnStatus);
                        sendMessageDataAll(MessageType.MSG_STOP_RECOGNITION, Global.FAILURE, null);
                    }

                    // Continue to unload SM for Commercial SMs.
                    if (false == Global.getInstance().getSmRepo().isCSM(smNameToStop)) {
                        break;
                    }

                case MessageType.MSG_UNLOAD_SM:
                    Log.v(TAG, "MSG_UNLOAD_SM in handler");
                    String smNameToUnload = (String) msg.obj;
                    if (null == smNameToUnload) {
                        Log.e(TAG, "MSG_UNLOAD_SM: unloadSoundModel failed because no " +
                                "smName was passed.");
                        sendMessageDataAll(MessageType.MSG_UNLOAD_SM, Global.FAILURE, null);
                        break;
                    }

                    int smHandleToUnload = smSessionManager.getSmHandle(smNameToUnload, SmState.UNLOADED);
                    if (smHandleToUnload == -1) {
                        Log.e(TAG, "MSG_UNLOAD_SM: unloadSoundModel failed because no " +
                                "SM with smHandle= " + smHandleToUnload + " and smName= " +
                                smNameToUnload + " was found.");
                        sendMessageDataAll(MessageType.MSG_UNLOAD_SM, Global.FAILURE, null);
                        break;
                    } else if (smHandleToUnload == -2) {
                        Log.e(TAG, "MSG_UNLOAD_SM: unloadSoundModel failed because " +
                                "SM with smName= " + smNameToUnload + " is in the wrong state.");
                        sendMessageDataAll(MessageType.MSG_UNLOAD_SM, Global.FAILURE, null);
                        break;
                    }

                    Log.v(TAG, "MSG_UNLOAD_SM: calling unloadSoundModel on smHandle= " +
                            smHandleToUnload);
                    returnStatus = stModule.unloadSoundModel(smHandleToUnload);
                    if (returnStatus == SoundTrigger.STATUS_OK) {
                        Log.v(TAG, "MSG_UNLOAD_SM: unloadSoundModel succeeded for smHandle= " +
                                smHandleToUnload);

                        smSessionManager.addUnloadedSmSession(smHandleToUnload);

                        sendMessageDataAll(MessageType.MSG_UNLOAD_SM, Global.SUCCESS, null);
                    } else {
                        Log.e(TAG, "MSG_UNLOAD_SM: unloadSoundModel failed for smHandle= " +
                                smHandleToUnload);
                        sendMessageDataAll(MessageType.MSG_UNLOAD_SM, Global.FAILURE, null);
                    }
                    break;

                case MessageType.MSG_QUERY_SM_STATE:
                    Log.v(TAG, "MSG_QUERY_SM_STATE in handler");
                    String smNameToQueryState = (String) msg.obj;
                    if (null == smNameToQueryState) {
                        Log.e(TAG, "MSG_QUERY_SM_STATE: querying the SM state failed because no " +
                                "smName was passed.");
                        sendMessageDataAll(MessageType.MSG_QUERY_SM_STATE, Global.FAILURE, null);
                        break;
                    }

                    int smHandleToQuery = smSessionManager.getSmHandle(smNameToQueryState, null);
                    if (smHandleToQuery == -1) {
                        Log.v(TAG, "MSG_QUERY_SM_STATE: no SM with smHandle= " + smHandleToQuery +
                                " and smName= " + smNameToQueryState + " was found.");
                        sendMessageDataAll(MessageType.MSG_QUERY_SM_STATE, Global.SUCCESS, -1);
                        break;
                    }

                    SmState smState = smSessionManager.getState(smHandleToQuery);
                    if (null == smState) {
                        Log.e(TAG, "MSG_QUERY_SM_STATE: querying the SM state failed because no " +
                                "SM with smHandle= " + smHandleToQuery + " and smName= " +
                                smNameToQueryState + " was found.");
                        sendMessageDataAll(MessageType.MSG_QUERY_SM_STATE, Global.FAILURE, null);
                        break;
                    }

                    sendMessageDataAll(MessageType.MSG_QUERY_SM_STATE, Global.SUCCESS, smState.ordinal());
                    break;

                case MessageType.MSG_TEST_ONE:
                    Log.v(TAG, "MSG_TEST_ONE in handler");

                    useStKeyphraseSmFromXml = !useStKeyphraseSmFromXml;
                    if (useStKeyphraseSmFromXml) {
                        sendMessageDataAll(MessageType.MSG_TEST_ONE, Global.SUCCESS, null);
                    } else {
                        sendMessageDataAll(MessageType.MSG_TEST_ONE, Global.FAILURE, null);
                    }
                    break;

                default:
                    Log.e(TAG, "There is no such Message: " + msg.what);
                    break;
            }
        }

        private void createXmlFromStksm(SoundTrigger.KeyphraseSoundModel stksm, String smName) {
            Log.v(TAG, "createXmlFromStksm");
            final String xmlFilePath = Global.PATH_APP + "/" + smName + ".xml";

            try {
                FileOutputStream fileOutputStream = new FileOutputStream(new File(xmlFilePath));
                XmlSerializer xmlSerializer = Xml.newSerializer();
                xmlSerializer.setFeature("http://xmlpull.org/v1/doc/features.html#indent-output", true);
                StringWriter stringWriter = new StringWriter();
                xmlSerializer.setOutput(stringWriter);
                xmlSerializer.startDocument("UTF-8", true);
                xmlSerializer.startTag(null, "KeyphraseSoundModel");

                xmlSerializer.startTag(null, "Uuid");
                xmlSerializer.text(stksm.uuid.toString());
                xmlSerializer.endTag(null, "Uuid");

                xmlSerializer.startTag(null, "VendorUuid");
                xmlSerializer.text(stksm.vendorUuid.toString());
                xmlSerializer.endTag(null, "VendorUuid");

                xmlSerializer.startTag(null, "Data");
                xmlSerializer.text(smName);
                xmlSerializer.endTag(null, "Data");

                for (SoundTrigger.Keyphrase stKeyphrase : stksm.keyphrases) {
                    xmlSerializer.startTag(null, "Keyphrase");

                    xmlSerializer.startTag(null, "KeyphraseId");
                    xmlSerializer.text(Integer.toString(stKeyphrase.id));
                    xmlSerializer.endTag(null, "KeyphraseId");

                    xmlSerializer.startTag(null, "RecognitionModes");
                    xmlSerializer.text(Integer.toString(stKeyphrase.recognitionModes));
                    xmlSerializer.endTag(null, "RecognitionModes");

                    xmlSerializer.startTag(null, "Locale");
                    xmlSerializer.text(stKeyphrase.locale);
                    xmlSerializer.endTag(null, "Locale");

                    xmlSerializer.startTag(null, "Text");
                    xmlSerializer.text(stKeyphrase.text);
                    xmlSerializer.endTag(null, "Text");

                    for (int user : stKeyphrase.users) {
                        xmlSerializer.startTag(null, "UserId");
                        xmlSerializer.text(Integer.toString(user));
                        xmlSerializer.endTag(null, "UserId");
                    }

                    xmlSerializer.endTag(null, "Keyphrase");
                }

                xmlSerializer.endTag(null, "KeyphraseSoundModel");
                xmlSerializer.endDocument();
                xmlSerializer.flush();
                fileOutputStream.write(stringWriter.toString().getBytes());
                fileOutputStream.close();
                Log.v(TAG, "createXmlFromStksm: wrote xml file to file system");
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
            } catch (IllegalStateException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        // This function requires a valid XML file exists on the device's file system named the same
        // name as the SM to be used.
        private SoundTrigger.KeyphraseSoundModel createStksmInfoFromXml(String smName) {
            Log.v(TAG, "createStksmInfoFromXml");
            XmlPullParserFactory xmlPullParserFactory;
            XmlPullParser parser;
            String keyphrase;
            final String xmlFilePath = Global.PATH_APP + "/" + smName + ".xml";
            Log.v(TAG, "createStksmInfoFromXml: smName= " + smName + ", xmlFilePath= " + xmlFilePath);
            FileInputStream fileInputStream = null;
            try {
                fileInputStream = new FileInputStream(new File(xmlFilePath));
                xmlPullParserFactory = XmlPullParserFactory.newInstance();
                parser = xmlPullParserFactory.newPullParser();
                parser.setInput(fileInputStream, null);
                int eventType = parser.getEventType();
                String startTag;
                while (eventType != XmlPullParser.END_DOCUMENT) {
                    switch (eventType) {
                        case XmlPullParser.START_DOCUMENT:
                            Log.v(TAG, "createStksmInfoFromXml: start_document found");
                            Log.v(TAG, "createStksmInfoFromXml: start_document parser.getName()= "
                                    + parser.getName());
                            break;
                        case XmlPullParser.START_TAG:
                            startTag = parser.getName();
                            Log.v(TAG, "createStksmInfoFromXml: start_tag= " + startTag);
                            if (startTag.equals("KeyphraseSoundModel")) {
                                // This can be ignored because it's only to signal the beginning of
                                //the XML file.
                                break;
                            } else if (startTag.equals("Uuid")) {
                                parser.next();
                                Log.v(TAG, "createStksmInfoFromXml: UUID= " + parser.getText());
                                uuid = UUID.fromString(parser.getText());
                            } else if (startTag.equals("VendorUuid")) {
                                parser.next();
                                Log.v(TAG, "createStksmInfoFromXml: vendor UUID= " + parser.getText());
                                vendorUuid = UUID.fromString(parser.getText());
                            } else if (startTag.equals("Data")) {
                                parser.next();
                                Log.v(TAG, "createStksmInfoFromXml: Data= " + parser.getText());
                                if (parser.getText().equals("true")) {
                                    String filePath = Global.PATH_APP + "/" + smName +
                                            Global.PATH_DATA_FILEEXT;
                                    Log.v(TAG, "createStksmInfoFromXml: opaqueData filepath= " + filePath);
                                    opaqueData = Utils.readFileToByteArray(filePath);
                                }
                            } else if (startTag.equals("Keyphrase")) {
                                parser.next();
                                Log.v(TAG, "createStksmInfoFromXml: Keyphrase= " + parser.getText());
                                keyphrase = parser.getText();
                            } else {
                                Log.e(TAG, "createStksmInfoFromXml: startTag= " + startTag +
                                        " not recognized.");
                                return null;
                            }
                            break;
                        case XmlPullParser.END_TAG:
                            Log.v(TAG, "createStksmInfoFromXml: end_tag= " + parser.getName());
                            break;
                    }
                    eventType = parser.next();
                    Log.v(TAG, "createStksmInfoFromXml: parser.next()= " + parser.getName());
                }
                if (eventType == XmlPullParser.END_DOCUMENT) {
                    Log.v(TAG, "createStksmInfoFromXml: END_DOCUMENT");
                    Log.v(TAG, "createStksmInfoFromXml: END_DOCUMENT parser.getName()= " + parser.getName());
                } else {
                    Log.e(TAG, "createStksmInfoFromXml: Could not reach end of document.");
                }
            } catch (FileNotFoundException e1) {
                e1.printStackTrace();
            } catch (XmlPullParserException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return null;
        }
    };

    // A reference for remote Service message receiving
    public final Messenger receiveMessenger = new Messenger(receiveHandler);

    // Send a message with int data to all registered clients
    void sendMessageDataAll(int what, int msgArg1, Integer msgArg2) {
        Log.v(TAG, "sendMessageDataAll");
        for (int i = mClients.size() - 1; i >= 0; i--) {
            try {
                Log.v(TAG, "sendMessageDataAll: to client= " + i);
                Message msg = Message.obtain(null, what);
                msg.arg1 = msgArg1;
                if (null != msgArg2) {
                    msg.arg2 = msgArg2.intValue();
                }
                mClients.get(i).send(msg);
                Log.v(TAG, "sendMessageDataAll: after send");
            } catch (RemoteException e) {
                Log.v(TAG, "sendMessageDataAll: removing client= " + i);
                mClients.remove(i);
            }
        }
    }

    // Manages SMs' state and the associated attributes in each of those states.
    private class SmSessionManager {
        private boolean IS_A_SESSION_STARTED_DEFAULT = false;
        private boolean IS_A_SESSION_BUFFERING_DEFAULT = false;
        private boolean IS_SERVICE_ENABLED_DEFAULT = false;
        private Map<Integer, SmSession> smSessions = new HashMap<Integer, SmSession>();
        private boolean isASessionStarted = IS_A_SESSION_STARTED_DEFAULT;
        private boolean isASessionBuffering = IS_A_SESSION_BUFFERING_DEFAULT;
        private boolean isServiceEnabled = IS_SERVICE_ENABLED_DEFAULT;
        private int numStartedSessions = 0;
        private ArrayList<String> loadedSmNames = new ArrayList<String>();

        private SmSessionManager() {
            for (int i=0; i<Global.NUM_MAX_SESSIONS; i++) {
                loadedSmNames.add(null);
            }
        }

        /*** Setters and Getters ***/

        /*
         * Returns the keyphraseRecogExtras associated with a particular smHandle or null if not found.
         */
        public KeyphraseRecognitionExtra[] getLoadedSmSessionKpRecogExtras(int inSmHandle) {
            return smSessions.containsKey(inSmHandle) ? smSessions.get(inSmHandle).kpRecogExtra : null;
        }

        /*
         * Returns the smName associated with a particular smHandle or null if not found.
         */
        public String getSmName(int inSmHandle) {
            return this.smSessions.containsKey(inSmHandle) ? this.smSessions.get(inSmHandle).smName : null;
        }

        /*
         * Returns the state associated with a particular smName or null if not found.
         */
        public SmState getState(int inSmHandle) {
            return this.smSessions.containsKey(inSmHandle) ? this.smSessions.get(inSmHandle).state : null;
        }

        /*
         * Returns the recogConfig associated with a particular smName or null if not found.
         */
        public RecognitionConfig getRecogConfig(int inSmHandle) {
            return this.smSessions.containsKey(inSmHandle) ?
                    this.smSessions.get(inSmHandle).recogConfig : null;
        }

        /*
         * Returns the sessionNum associated with a particular smName or -1 if not found.
         */
        public int getSessionNum(int inSmHandle) {
            return this.smSessions.containsKey(inSmHandle) ?
                    this.smSessions.get(inSmHandle).sessionNum : -1;
        }

        /*
         * Returns the smHandle associated with a particular smName.
         * Checks the smName's session is in a state that can transition into the supplied state.
         *
         * @return
         *      -1 for missing smName
         *      -2 for wrong state
         */
        public int getSmHandle(String inSmName, SmState inState) {
            int smHandle = -1;

            // Iterate through map to find smName and save its smHandle.
            for (Map.Entry<Integer, SmSession> entry : smSessions.entrySet()) {
                Log.v(TAG, "getSmHandle: inSmName= " + inSmName + ", checking against smName= "
                        + entry.getValue().smName);
                if (entry.getValue().smName.equals(inSmName)) {
                    smHandle = entry.getKey();
                }
            }

            // If no smHandle was found for the smName, return error.
            if (smHandle == -1) {
                return smHandle;
            }

            if (inState == null || isStatesTransitionable(smSessions.get(smHandle).state, inState)) {
                return smHandle;
            } else {
                Log.e(TAG, "getSmHandle: expected state= " + inState.name() + ", actual state= "
                        + smSessions.get(smHandle).state.name());
                return -2;
            }
        }

        /*
         * Returns smHandles for all SMs in the STARTED state or empty arraylist if none exist.
         */
        public ArrayList<Integer> getStartedSessionsSmHandles() {
            ArrayList<Integer> startSmHandles = new ArrayList<Integer>();

            for (Map.Entry<Integer, SmSession> entry : smSessions.entrySet()) {
                Log.v(TAG, "getStartedSessionsSmHandles: checking smHandle= " + entry.getKey());
                if (entry.getValue().state.equals(SmState.STARTED)) {
                    startSmHandles.add(entry.getKey());
                    Log.v(TAG, "getStartedSessionsSmHandles: added smHandle= " + entry.getKey());
                }
            }

            return startSmHandles;
        }

        /*
         * Returns true only from when a LookAheadBufferTask is started to when the
         *  buffering finishes.
         */
        public boolean getIsASessionBuffering() {
            return this.isASessionBuffering;
        }

        /*
         * Set to true when a LookAheadBufferTask is started.
         * Set to false when the LookAheadBufferTask finishes buffering.
         */
        public void setIsASessionBuffering(boolean inIsBuffering) {
            this.isASessionBuffering = inIsBuffering;
        }

        /*
         * Returns true when the service_enabled event has been received without a corresponding
         *  service_disabled event.
         * service_enabled first happens after attaching the module (in the initialization of the
         *  app). Then it happens again whenever LAB or another concurrency situation stops.
         * service_disabled happens whenever LAB or another concurrency situation starts.
         */
        public boolean getIsServiceEnabled() {
            return isServiceEnabled;
        }

        // See getIsServiceEnabled().
        public void setServiceEnabled(boolean isServiceEnabled) {
            this.isServiceEnabled = isServiceEnabled;
        }


        /*** LOAD Functions ***/

        private void addLoadedSmSession(String inSmName, int inSmHandle,
                SoundTrigger.KeyphraseRecognitionExtra[] inKre) {
            Log.v(TAG, "addLoadedSmSession");
            smSessions.put(inSmHandle, new SmSession(inSmName, inKre, SmState.LOADED));
            if (smSessions.get(inSmHandle).sessionNum == -1) {
                Log.e(TAG, "addLoadedSmSession: failed to add loaded SM with smHandle= " +
                        inSmHandle + ", smName= " + smSessions.get(inSmHandle).smName +
                        ". NumSessions is too large.");
            }
            Log.v(TAG, "addLoadedSmSession: successfully added loaded SM with smHandle= " +
                    inSmHandle + ", smName= " + smSessions.get(inSmHandle).smName);
            Global.getInstance().getSmRepo().setLoadedSmNames(this.loadedSmNames);
        }

        /*
         * Adds smName to loadedSmNames list in the index equal to the assigned session number.
         * @return
         *      success: the session number assigned
         *      failure: -1
         */
        private int addSmNameToLoadedSmNames(String smName) {
            for (int i=0; i<loadedSmNames.size(); i++) {
                if (null == loadedSmNames.get(i)) {
                    loadedSmNames.set(i, smName);
                    return i;
                }
            }
            return -1;
        }

        /*
         * Removes smName from loadedSmNames list
         * @return
         *      success: true
         *      failure: false
         */
        private boolean removeSmNameFromLoadedSmNames(String smName) {
            for (int i=0; i<loadedSmNames.size(); i++) {
                if (null != loadedSmNames.get(i) && loadedSmNames.get(i).equals(smName)) {
                    loadedSmNames.set(i, null);
                    return true;
                }
            }
            return false;
        }


        /*** START Functions ***/

        public void addStartedSmSession(int inSmHandle, RecognitionConfig recogConfig) {
            this.isASessionStarted = true;
            this.smSessions.get(inSmHandle).setRecogConfig(recogConfig);
            this.smSessions.get(inSmHandle).state = SmState.STARTED;
            this.numStartedSessions++;
        }

        public boolean isASessionStarted() {
            return isASessionStarted;
        }

        /*** STOP Functions ***/

        public void addStoppedSmSession(int inSmHandle) {
            this.smSessions.get(inSmHandle).state = SmState.STOPPED;
            this.numStartedSessions--;
            this.smSessions.get(inSmHandle).removeRecogConfig();

            if (this.numStartedSessions == 0) {
                Log.v(TAG, "addStoppedSmSession: last remaining started session has been stopped");
                this.isASessionStarted = false;
            }
        }

        /*** UNLOAD Functions ***/

        public void addUnloadedSmSession(int inSmHandle) {
            SmSession smSession = this.smSessions.remove(inSmHandle);
            if (smSession == null) {
                Log.e(TAG, "addUnloadedSmSession: could not remove SM with smHandle= " + inSmHandle +
                        ", could not be found");
            }

            boolean removedFromLoadedSmNames = removeSmNameFromLoadedSmNames(smSession.smName);
            if(false == removedFromLoadedSmNames) {
                Log.e(TAG, "addUnloadedSmSession: could not remove SM with smName= " +
                        smSession.smName + " from loadedSmNames. SM could not be found.");
            }
        }

        public void updateStateServiceDied() {
            // Reset all values.
            this.smSessions = new HashMap<Integer, SmSession>();
            this.isASessionStarted = IS_A_SESSION_STARTED_DEFAULT;
            this.isASessionBuffering = IS_A_SESSION_BUFFERING_DEFAULT;
            this.isServiceEnabled = IS_SERVICE_ENABLED_DEFAULT;
            this.numStartedSessions = 0;
            this.loadedSmNames = new ArrayList<String>();
            for (int i=0; i<Global.NUM_MAX_SESSIONS; i++) {
                loadedSmNames.add(null);
            }
            Global.getInstance().getSmRepo().setLoadedSmNames(this.loadedSmNames);
        }

        /*
         * Check if start state can transition to end state
         * e.g. LOADED -> STARTED is ok, but LOADED -> STOPPED is invalid.
         */
        public boolean isStatesTransitionable(SmState startState, SmState endState) {
            switch (endState) {

                case UNLOADED:
                    if (startState == SmState.LOADED || startState == SmState.STOPPED) return true;
                case LOADED:
                    if (startState == SmState.UNLOADED) return true;
                case STARTED:
                    if (startState == SmState.LOADED || startState == SmState.STOPPED) return true;
                case STOPPED:
                    if (startState == SmState.STARTED) return true;

                default:
                    Log.e(TAG, "isStatesTransitionable: unrecognized state= " + endState.name());
                    break;
            }

            return false;
        }

        private class SmSession {
            public final String smName;
            private final SoundTrigger.KeyphraseRecognitionExtra[] kpRecogExtra;
            public SmState state = SmState.UNLOADED;
            public SoundTrigger.RecognitionConfig recogConfig = null;
            public int sessionNum = -1;

            public SmSession(String inSmName,
                    SoundTrigger.KeyphraseRecognitionExtra[] inKpRecognitionExtra,
                    SmState inSmState) {
                this.smName = inSmName;
                this.kpRecogExtra = inKpRecognitionExtra;
                this.state = inSmState;
                this.sessionNum = addSmNameToLoadedSmNames(smName);
            }

            public void setRecogConfig(SoundTrigger.RecognitionConfig inRecognitionConfig) {
                this.recogConfig = inRecognitionConfig;
            }

            public void removeRecogConfig() {
                this.recogConfig = null;
            }
        }
    }



    public class CreateOrExtendSmTask extends AsyncTask<String, Void, Integer> {
        @Override
        protected Integer doInBackground(String... strings) {
            Log.v(TAG, "CreateOrExtendSmTask.doInBackground");
            String keyphrase = strings[0];
            String username = strings[1];
            boolean isUdk = strings[2].equals("true");
            Log.v(TAG, "CreateOrExtendSmTask.doInBackground: keyphrase= " + keyphrase +
                    ", username= " + username + ", isUdk= " + strings[2]);

            int size = -1;
            if (isUdk) {
                size = ListenSoundModel.getUdkSmSize(
                        keyphrase,
                        username,
                        Global.getInstance().getUserRecordings(),
                        Global.getInstance().getLanguageModel());
            } else {
                size = ListenSoundModel.getSizeWhenExtended(
                        Global.getInstance().getSmRepo().getSoundModelByteBufferToExtend(),
                        keyphrase,
                        username);
            }

            if (size <= 0) {
                Log.v(TAG, "CreateOrExtendSmTask.doInBackground: getSizeWhenExtended returned error= " + size);
                return ListenTypes.STATUS_EFAILURE;
            }
            Global.getInstance().getSmRepo().createExtendedSoundModelByteBuffer(size);

            Log.v(TAG, "CreateOrExtendSmTask.doInBackground: calling extend with key phrase= " + strings[0]
                    + ", user= " + strings[1]);
            int returnStatus = -1;
            if (isUdk) {
                returnStatus = ListenSoundModel.createUdkSm(
                        keyphrase,
                        username,
                        Global.NUM_TRAINING_RECORDINGS_REQUIRED,
                        Global.getInstance().getUserRecordings(),
                        Global.getInstance().getLanguageModel(),
                        Global.getInstance().getSmRepo().getExtendedSoundModel(),
                        Global.getInstance().getConfidenceData());
            } else {
                returnStatus = ListenSoundModel.extend(
                        Global.getInstance().getSmRepo().getSoundModelByteBufferToExtend(),
                        keyphrase,
                        username,
                        Global.NUM_TRAINING_RECORDINGS_REQUIRED,
                        Global.getInstance().getUserRecordings(),
                        Global.getInstance().getSmRepo().getExtendedSoundModel(),
                        Global.getInstance().getConfidenceData());
            }

            return returnStatus;
        }

        @Override
        protected void onPostExecute(Integer returnStatus) {
            Log.v(TAG, "CreateOrExtendSmTask.onPostExecute");
            if (returnStatus == ListenTypes.STATUS_SUCCESS) {
                Log.v(TAG, "CreateOrExtendSmTask.onPostExecute: task succeeded");
                Global.getInstance().getSmRepo().saveExtendedSoundModel(getApplicationContext());
                Global.getInstance().removeUserRecordings();
                sendMessageDataAll(MessageType.MSG_EXTEND_SOUNDMODEL, Global.SUCCESS, null);
            } else {
                Log.e(TAG, "CreateOrExtendSmTask.onPostExecute: task failed");
                sendMessageDataAll(MessageType.MSG_EXTEND_SOUNDMODEL, Global.FAILURE, null);
            }
        };
    }

public class LookAheadBufferTask extends AsyncTask<String, Void, Integer> {
        @Override
        protected Integer doInBackground(String... strings) {
            Log.v(TAG, "LookAheadBufferTask.doInBackground");
            // Needs to be set at the very beginning to ensure LABTask is cancelled if running.
            smSessionManager.setIsASessionBuffering(true);
            // SUCCESS if there's data to be saved, FAILURE if not. So far... not, so failure.
            int returnStatus = Global.FAILURE;
            String filePath = strings[0];
            int audioSessionNum = Integer.parseInt(strings[1]);
            Log.v(TAG, "LookAheadBufferTask.doInBackground: sessionNum = " + audioSessionNum);
// %%% why double??
            double shortsToAllocate = Global.getInstance().getSettingVoiceRequestLength() *
                    Global.SHORTS_PER_SECOND;
            int totalBytesToRead = (int)shortsToAllocate * 2;
            Log.v(TAG, "LookAheadBufferTask.doInBackground: total bytesToRead = " + totalBytesToRead);

            /* Open AudioRecord Session using sessionId returned by detection event */
            AudioAttributes.Builder audAttBldr = new AudioAttributes.Builder();
            audAttBldr.setUsage(AudioAttributes.USAGE_MEDIA);
            audAttBldr.setContentType(AudioAttributes.CONTENT_TYPE_SPEECH);
            //audAttBldr.setInternalCapturePreset(1999); // %%% MediaRecorder.HOTWORD
            // %%% HOTWORD is current private so hardcored value is used
            AudioAttributes audAtt = audAttBldr.build();

            int minBuffSize = AudioRecord.getMinBufferSize(16000, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
            Log.v(TAG, "LookAheadBufferTask.doInBackground: getMinBuffSize returns " + minBuffSize);

            AudioFormat.Builder audFormatBldr = new AudioFormat.Builder();
            audFormatBldr.setEncoding(AudioFormat.ENCODING_PCM_16BIT);
            audFormatBldr.setSampleRate(16000);
            audFormatBldr.setChannelMask(AudioFormat.CHANNEL_IN_MONO);
            AudioFormat audFormat = audFormatBldr.build();

            // Set desired buffer size to be read per AudioRecord.read call in loop
            // make this larger than 125ms (4000 bytes)
            int incBuffSize = 16000; // use 500ms as a default buffer size

            int streamBufferSizeInBytes;
            if (minBuffSize > incBuffSize)
                streamBufferSizeInBytes = minBuffSize;
            else
                streamBufferSizeInBytes = incBuffSize;
            Log.v(TAG, "LookAheadBufferTask.doInBackground: create AudioRec stream with " +
            		"sessionId = " + audioSessionNum);
            Log.v(TAG, "LookAheadBufferTask.doInBackground:      buffSize = " + streamBufferSizeInBytes);

            AudioRecord audioRecord =
                    new AudioRecord(audAtt, audFormat, streamBufferSizeInBytes, audioSessionNum);
            byte[] buffer = new byte[totalBytesToRead];
            audioRecord.startRecording();

            int curBytesRead = 0;
            while (curBytesRead < totalBytesToRead) {
                Log.v(TAG, "LookAheadBufferTask.doInBackground: call audioRecord.read curBytesRead " +
                        curBytesRead + ", incBuffSize " + incBuffSize);
                if (isCancelled()) {
                    Log.v(TAG, "LookAheadBufferTask.doInBackground: cancelled from detaching sound" +
                            "module");
                    break;
                }
                int bufferReadResult = audioRecord.read(buffer, curBytesRead, incBuffSize);
                if (bufferReadResult < 0) {
                    Log.v(TAG, "LookAheadBufferTask.doInBackground: audioRecord.read returned error "
                           +  bufferReadResult + ", serviceState " + isServiceStateEnabled);
                    break;
                } else {
                    Log.v(TAG, "LookAheadBufferTask.doInBackground: audioRecord.read successfully read "
                               + bufferReadResult + " bytes");
                    curBytesRead += bufferReadResult;
                }
            }

            audioRecord.stop();
            audioRecord.release();

            if (curBytesRead > 0) {
                Log.v(TAG, "LookAheadBufferTask.doInBackground: write wav file to " + filePath);
                Global.getInstance().saveVoiceRequest(buffer, curBytesRead, filePath);
                returnStatus = Global.SUCCESS;
            } else {
                returnStatus = Global.FAILURE;
            }

            smSessionManager.setIsASessionBuffering(false);
            return returnStatus;
        }

        @Override
        protected void onPostExecute(Integer result) {
            Log.v(TAG, "LookAheadBufferTask.onPostExecute");
            if (result == Global.SUCCESS) {
                Log.v(TAG, "LookAheadBufferTask.onPostExecute: task succeeded");
                sendMessageDataAll(MessageType.MSG_NEW_VOICEREQUEST, Global.SUCCESS, null);
            } else {
                Log.v(TAG, "LookAheadBufferTask.onPostExecute: task failed");
                sendMessageDataAll(MessageType.MSG_NEW_VOICEREQUEST, Global.FAILURE, null);
            }
        }

        @Override
        protected void onCancelled(Integer result) {
            if (result == Global.SUCCESS) {
                sendMessageDataAll(MessageType.MSG_NEW_VOICEREQUEST, Global.SUCCESS, null);
            } else {
                sendMessageDataAll(MessageType.MSG_NEW_VOICEREQUEST, Global.FAILURE, null);
            }
        }
    }

    public class AttachModuleTask extends AsyncTask<Void, Void, Integer> {
        @Override
        protected Integer doInBackground(Void... Void) {
            Log.v(TAG, "AttachModuleTask.doInBackground");
            ArrayList<SoundTrigger.ModuleProperties> modules = new
                    ArrayList<SoundTrigger.ModuleProperties>();
            SoundTrigger.listModules(modules);
            if (modules.size() < 1) {
                Log.e(TAG, "AttachModuleTask.doInBackground: no available modules.");
                return Global.FAILURE;
            } else {
                Log.v(TAG, "AttachModuleTask.doInBackground: attach module id = " + modules.get(0).id);
                Log.v(TAG, "AttachModuleTask.doInBackground: attach module UUID = " + modules.get(0).uuid);
                Log.v(TAG, "AttachModuleTask.doInBackground: attach module UUID most sig long = " + modules.get(0).uuid.getMostSignificantBits());
                Log.v(TAG, "AttachModuleTask.doInBackground: attach module UUID least sig long = " + modules.get(0).uuid.getLeastSignificantBits());
                stModule = SoundTrigger.attachModule(modules.get(0).id, VwuService.this, null);
            }

            if (null != stModule) {
                Log.v(TAG, "AttachModuleTask.doInBackground: success");
                return Global.SUCCESS;
            }

            return Global.FAILURE;
        }

        @Override
        protected void onPostExecute(Integer returnStatus) {
            Log.v(TAG, "AttachModuleTask.onPostExecute");
            if (returnStatus != Global.SUCCESS) {
                Log.e(TAG, "MSG_EXTEND_SOUNDMODEL: failed");
                sendMessageDataAll(MessageType.MSG_ATTACH_MODULE, Global.FAILURE, null);
            }
        };
    }

    private void launchVoiceQna() {
        Log.v(TAG, "launchVoiceQna");
        int defaultFlag = Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP
                              | Intent.FLAG_ACTIVITY_NEW_TASK;
        Log.v(TAG, "launchVoiceQna: inside if statement");
        Intent intent = new Intent(RecognizerIntent.ACTION_WEB_SEARCH);
        Log.v(TAG, "launchVoiceQna: intent created");
        intent.setFlags(defaultFlag);
        Log.v(TAG, "launchVoiceQna: flags set");
        try {
            startActivity(intent);
        } catch (ActivityNotFoundException e) {
            Log.e(TAG, "launchVoiceQna: No voice recognition app installed");
            return;
        }
        Log.v(TAG, "launchVoiceQna: intent launched");
    }

    // Show the icon in the notification bar that Listen is enabled
    private void showNotification(int icon, String status) {
        long when = System.currentTimeMillis();

        Notification notification = new Notification(icon, null, when);
        Intent intent = new Intent(this, DetectionActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP);

        PendingIntent contentIntent = PendingIntent.getActivity(this, 0, intent, 0);

        CharSequence contentTitle = "Voice Activation";
        notification.setLatestEventInfo(context, contentTitle, status, contentIntent);

        notificationManager.notify(VOICEWAKEUP_NOTIFICATION_ID, notification);
    }

    /**
     * Calls the SM library function parseDetectionEventData when a detection event
     * is received.
     *
     * @param eventType
     * @param eventData
     */
    private void processDetectionEvent(SoundTrigger.RecognitionEvent recognitionEvent) {

        // Print content of the received KeyphraseRecognitionEvent.
        Log.v(TAG, "processDetectionEvent: recognitionEvent.status= " + recognitionEvent.status);
        Log.v(TAG, "processDetectionEvent: recognitionEvent.soundModelHandle= " +
                recognitionEvent.soundModelHandle);
        SoundTrigger.KeyphraseRecognitionEvent stKpRecogEvent =
                (SoundTrigger.KeyphraseRecognitionEvent) recognitionEvent;
        for (SoundTrigger.KeyphraseRecognitionExtra kRecogExtra : stKpRecogEvent.keyphraseExtras) {
            Log.v(TAG, "processDetectionEvent: kRecogExtra.id= " + kRecogExtra.id);
            Log.v(TAG, "processDetectionEvent: kRecogExtra.confidenceLevels.length= " +
                    kRecogExtra.confidenceLevels.length);
            for (SoundTrigger.ConfidenceLevel confLevel : kRecogExtra.confidenceLevels) {
                Log.v(TAG, "processDetectionEvent: kRecogExtra.id= " + kRecogExtra.id + ", userID= "
                        + confLevel.userId + ", confLevel= " + confLevel.confidenceLevel);
            }
        }

        // Error checking: if recognitionEvent.status is unknown, return error.
        if ((recognitionEvent.status != SoundTrigger.RECOGNITION_STATUS_SUCCESS) &&
                (recognitionEvent.status != SoundTrigger.RECOGNITION_STATUS_FAILURE) &&
                (recognitionEvent.status != SoundTrigger.RECOGNITION_STATUS_ABORT)) {
            Log.e(TAG, "processDetectionEvent: recognitionEvent.status= " + recognitionEvent.status +
                    " is not a valid status.");
            return;
        }

        // If recognitionEvent.status is ABORT, discard the recognition because capture preempted
        // by another audio usecase.
        if (recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_ABORT) {
            Log.v(TAG, "processDetectionEvent: recognitionEvent.status= RECOGNITION_STATUS_ABORT " +
                    "discarding recognition.");
            return;
        }

        // LAB
        Log.v(TAG, "processDetectionEvent: recognitionEvent.status = " + recognitionEvent.status);
        Log.v(TAG, "processDetectionEvent: voiceRequestEnabled = " + Global.getInstance().getSettingVoiceRequestsEnabled());
        if ( (recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_SUCCESS) &&
             (recognitionEvent.captureAvailable == true) &&
             (Global.getInstance().getSettingVoiceRequestsEnabled()) ) {
            String time = String.valueOf(System.currentTimeMillis());
            // Filepath for voice requests is approximately /sdcard/sva/voiceRequests/<time>.wav.
            String filePath = Global.PATH_VOICE_REQUESTS + "/" +
                        time + Global.PATH_RECORDINGS_FILEEXT;
            Log.v(TAG, "processDetectionEvent: filePath= " + filePath);

            String labCaptureAudioSessionNum = String.valueOf(recognitionEvent.captureSession);
            String[] filePathAndAudioSessionNum = {filePath, labCaptureAudioSessionNum};

            lookAheadBufferTask = new LookAheadBufferTask();
            lookAheadBufferTask.execute(filePathAndAudioSessionNum);
        }

        String smName = smSessionManager.getSmName(recognitionEvent.soundModelHandle);
        VoiceWakeupDetectionDataV2 vwuDetectionData = new VoiceWakeupDetectionDataV2();
        // Loop through KeyphraseRecognitionExtras to find out how many non-zero confLevels so
        // arrays can be initialized to the correct size.
        int keyphraseConfLevels = 0; //temp code
        int userConfLevels = 0; //temp code
        for (SoundTrigger.KeyphraseRecognitionExtra kRecogExtra : stKpRecogEvent.keyphraseExtras) {
            if (kRecogExtra.coarseConfidenceLevel > 0) {
                keyphraseConfLevels++;
            }
            for (SoundTrigger.ConfidenceLevel confLevel : kRecogExtra.confidenceLevels) {
                if (confLevel.confidenceLevel > 0) {
                    userConfLevels++;
                }
            }
        }
        vwuDetectionData.nonzeroKWConfLevels = new VWUKeywordConfLevel[keyphraseConfLevels];
        vwuDetectionData.nonzeroUserKWPairConfLevels =
                new VWUUserKeywordPairConfLevel[userConfLevels];
        SoundTrigger.KeyphraseRecognitionExtra kRecogExtra;
        SoundTrigger.ConfidenceLevel confLevel;
        String keyphraseName = null;
        String userName;
        int nonZeroKwIndex = 0;
        int nonZeroPairIndex = 0;
        int keyphraseConfLevel = -2;
        boolean isVoiceQnaEnabled = false;
        for (int i=0; i<stKpRecogEvent.keyphraseExtras.length; i++) {
            kRecogExtra = stKpRecogEvent.keyphraseExtras[i];
            keyphraseName = Global.getInstance().getSmRepo().findKeyphraseOrUserById(smName,
                    kRecogExtra.id);
            keyphraseConfLevel = kRecogExtra.coarseConfidenceLevel;
            Log.v(TAG, "processDetectionEvent: keyphrase with id= " + kRecogExtra.id + ", name= "
                    + keyphraseName + " detected with confidence level= " + keyphraseConfLevel);
            // Ensure valid detection (>0) before adding it.
            if (keyphraseConfLevel > 0) {
                vwuDetectionData.nonzeroKWConfLevels[nonZeroKwIndex] = new VWUKeywordConfLevel();
                vwuDetectionData.nonzeroKWConfLevels[nonZeroKwIndex].keyword = keyphraseName;
                vwuDetectionData.nonzeroKWConfLevels[nonZeroKwIndex].confLevel =
                        (short)keyphraseConfLevel;
                nonZeroKwIndex++;
                // Only change isVoiceQnaEnabled if it hasn't been set to true in a previous iteration.
                if (false == isVoiceQnaEnabled) {
                    isVoiceQnaEnabled = Global.getInstance().getSmRepo().getLaunchPreference(
                            getApplicationContext(), smName, keyphraseName, null);
                    Log.v(TAG, "processDetectionEvent: isVoiceQnaEnabled set to= " + isVoiceQnaEnabled);
                }
            }
            Log.v(TAG, "processDetectionEvent: kRecogExtra.confidenceLevels.length= " +
                    kRecogExtra.confidenceLevels.length);
            for (int j=0; j<kRecogExtra.confidenceLevels.length; j++) {
                confLevel = kRecogExtra.confidenceLevels[j];
                Log.v(TAG, "processDetectionEvent: kRecogExtra.id= " + kRecogExtra.id +
                        ", keyphrase= " + keyphraseName + ", userID= "
                        + confLevel.userId + ", confLevel= " + confLevel.confidenceLevel);
                // If user confLevel == 0, it's not a valid detection so don't add it. Continue to
                // next detection.
                if (confLevel.confidenceLevel == 0) {
                    continue;
                }
                userName = Global.getInstance().getSmRepo().findKeyphraseOrUserById(smName,
                        confLevel.userId);
                vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex] =
                        new VWUUserKeywordPairConfLevel();
                vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex].keyword = keyphraseName;
                vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex].user = userName;
                vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex].confLevel =
                        (short)confLevel.confidenceLevel;
                Log.v(TAG, "processDetectionEvent: added keyword= " +
                        vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex].keyword +
                        ", user= " +
                        vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex].user +
                        ", confLevel= " +
                        vwuDetectionData.nonzeroUserKWPairConfLevels[nonZeroPairIndex].confLevel);
                nonZeroPairIndex++;
                // Only change isVoiceQnaEnabled if it hasn't been set to true
                // in a previous iteration or during key phrase only checking.
                if (false == isVoiceQnaEnabled) {
                    isVoiceQnaEnabled = Global.getInstance().getSmRepo().getLaunchPreference(
                            getApplicationContext(), smName, keyphraseName, userName);
                    Log.v(TAG, "isVoiceQnaEnabled set to= " + isVoiceQnaEnabled);
                }
            }
        }
        for (VWUKeywordConfLevel nonzeroKWConfLevel : vwuDetectionData.nonzeroKWConfLevels) {
            Log.v(TAG, "nonzeroKWConfLevel.keyword= " + nonzeroKWConfLevel.keyword);
            Log.v(TAG, "\tnonzeroKWConfLevel.confLevel= " + nonzeroKWConfLevel.confLevel);
        }
        for (VWUUserKeywordPairConfLevel nonzeroUserKWPairConfLevel : vwuDetectionData.nonzeroUserKWPairConfLevels) {
            Log.v(TAG, "nonzeroUserKWPairConfLevel.keyword= " +
                    nonzeroUserKWPairConfLevel.keyword);
            Log.v(TAG, "\tnonzeroUserKWPairConfLevel.user= " +
                    nonzeroUserKWPairConfLevel.user);
            Log.v(TAG, "\tnonzeroUserKWPairConfLevel.confLevel= " +
                    nonzeroUserKWPairConfLevel.confLevel);
        }

        int sessionNum = smSessionManager.getSessionNum(recognitionEvent.soundModelHandle);
        if (sessionNum == -1) {
            Log.e(TAG, "processDetectionEvent: cannot continue because could not find sessionNum " +
            		"for smHandle= " + recognitionEvent.soundModelHandle);
            return;
        }

        // Save detection for DetectionActivity
        Global.getInstance().setDetectionContainer(ListenTypes.STATUS_SUCCESS, null,
                vwuDetectionData, sessionNum, smName);

        // Launch VRE.
        // If one of the detected keyphrase-user combinations set launch voice recognition engine
        // to true, the launch it.
        if (isVoiceQnaEnabled && recognitionEvent.status != SoundTrigger.RECOGNITION_STATUS_SUCCESS) {
            launchVoiceQna();
        }

        // Play detection tone.
        if (Global.getInstance().getSettingDetectionTone() &&
                !Global.getInstance().getSettingVoiceRequestsEnabled()) {
            try {
                if ((recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_SUCCESS) ||
                    (recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_FAILURE)) {
                    Log.v(TAG, "processDetectionEvent: playing " +
                            (recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_SUCCESS ? "success" : "failure") +
                            " tone");
                    tonePlayer.play(recognitionEvent.status);
                } else {
                    Log.e(TAG, "processDetectionEvent: unknown recognition status= " +
                            recognitionEvent.status);
                }
             } catch (NullPointerException e) {
                 Log.e(TAG, "Player was null e= " + e.getMessage());
             }
        }

        if (Global.getInstance().getNumActivitiesShowing() == 0) {
            if (null != Global.getInstance().getSmRepo().getKeyphraseActionIntent(keyphraseName)) {
                Intent intentToLaunch = Global.getInstance().getSmRepo().
                        getKeyphraseActionIntent(keyphraseName);
                startActivity(intentToLaunch);
            } else {
                Log.v(TAG, "processDetectionEvent: Global getKeyphraseActionIntent returned null." +
                		" Either no action was selected or the action was not found.");
            }
        } else if (Global.getInstance().getNumActivitiesShowing() < 0) {
            Log.e(TAG, "processDetectionEvent: Global getNumActivitiesShowing < 0. " +
            		"getNumActivitiesShowing= " + Global.getInstance().getNumActivitiesShowing());
        }

        // Send interested parties notice of a detection.
        if (recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_SUCCESS) {
            Log.v(TAG, "DETECT_SUCCESS_EVENT: sending event");
            sendMessageDataAll(MessageType.MSG_DETECT_SUCCEEDED, Global.SUCCESS, null);
        } else if (recognitionEvent.status == SoundTrigger.RECOGNITION_STATUS_FAILURE){
            sendMessageDataAll(MessageType.MSG_DETECT_FAILED, Global.SUCCESS, null);
            return;
        }

        turnOnDisplay();

        // Get the keyguardLock so no lock screen is shown when a detection occurs
        if (null != keyguardLock) {
            keyguardLock.reenableKeyguard();
            keyguardLock = null;
        }
        KeyguardManager keyguardManager = (KeyguardManager) getApplicationContext().getSystemService(Context.KEYGUARD_SERVICE);
        keyguardLock =  keyguardManager.newKeyguardLock(TAG);
        keyguardLock.disableKeyguard();
    }

    private synchronized void turnOnDisplay() {
        Log.v(TAG, "turnOnDisplay");

        // If the wakeLock isn't held, that means no other detections
        // have happened in the last DISPLAY_ON_DURATION milliseconds.
        // It's safe to acquire the wakeLock.
        if(false == wakeLock.isHeld()) {
            wakeLock.acquire();
        }

        // If no previous detection within 3 seconds, starts a new
        // timerTask and releases the wakeLock after 3 seconds.
        // If a timerTask already exists, cancels that task and
        // creates a new one to release the wakeLock after 3 seconds.
        startWakelockTimer();
    }

    // Turns off the display after DISPLAY_ON_DURATION milliseconds if no user interaction.
    private void startWakelockTimer() {
        Log.v(TAG, "startWakelockTimer");
        // To stop existing wakelockTimers
        stopWakelockTimer();
        wakelockTimer = new Timer();
        wakelockTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                Log.v(TAG, "startWakelockTimer: run()- release wakelock");
                // To stop this wakelockTimer from repeating
                stopWakelockTimer();
                try {
                    if (wakeLock.isHeld()) {
                        wakeLock.release();
                    }
                } catch (RuntimeException re) {
                    Log.e(TAG, "WakeLock under-locked");
                }
            }
        }, DISPLAY_ON_DURATION);
    }

    private void stopWakelockTimer() {
        if (null != wakelockTimer) {
            // Cancel all scheduled tasks for the timer.
            wakelockTimer.cancel();
            // Remove all canceled tasks from the queue so they can be garbaged collected.
            int numPurgedTasks = wakelockTimer.purge();
            //wakelockTimer = null;
            Log.v(TAG, "stopWakelockTimer: numPurgedTasks= " + numPurgedTasks);
        }
    }

    /*
     * Restart recognition on a previously started sm
     */
    public class RestartRecognitionTask extends AsyncTask<Integer, Void, Integer> {
        @Override
        protected Integer doInBackground(Integer... integers) {
            Log.v(TAG, "StartRecognitionTask.doInBackground");
            int startRecogReturnStatus = -1;
            Integer recognizedSmHandle = integers[0];

            SmState recognizedSmState = smSessionManager.getState(recognizedSmHandle);
            if (null == recognizedSmState) {
                Log.e(TAG, "StartRecognitionTask.doInBackground: cannot continue because "
                        + "SM with soundModelHandle= " + recognizedSmHandle + " was " +
                		        "never loaded");
                return Global.FAILURE;
            } else if (SmState.STARTED != recognizedSmState) {
                Log.e(TAG, "StartRecognitionTask.doInBackground: cannot continue because "
                        + "SM with soundModelHandle= " + recognizedSmHandle + " has incorrect" +
                                " state= " + recognizedSmState);
                return Global.FAILURE;
            }

            RecognitionConfig recogConfig = smSessionManager.getRecogConfig(recognizedSmHandle);
            if (null == recogConfig) {
                Log.e(TAG, "StartRecognitionTask.doInBackground: cannot continue because "
                        + "SM with soundModelHandle= " + recognizedSmHandle + " could not be " +
                        		"found or has no recogConfig");
                return Global.FAILURE;
            }

            startRecogReturnStatus = stModule.startRecognition(recognizedSmHandle, recogConfig);
            if (startRecogReturnStatus == SoundTrigger.STATUS_OK) {
                Log.v(TAG, "StartRecognitionTask.doInBackground: startRecognition succeeded " +
                        " for soundModelHandle= " + recognizedSmHandle);
            } else {
                Log.e(TAG, "StartRecognitionTask.doInBackground: startRecognition failed for "
                        + "soundModelHandle= " + recognizedSmHandle + " with retrunStatus= " +
                        startRecogReturnStatus);
                return Global.FAILURE;
            }

            return Global.SUCCESS;
        }

        @Override
        protected void onPostExecute(Integer result) {
            Log.v(TAG, "StartRecognitionTask.onPostExecute");
            if (result != Global.SUCCESS) {
                sendMessageDataAll(MessageType.MSG_REGISTER_SOUNDMODEL, Global.FAILURE, null);
            }
        }
    }

    /**
     * Implements SoundTrigger.StatusListener to receive events.
     */
    @Override
    public void onRecognition(SoundTrigger.RecognitionEvent recognitionEvent) {
        Log.v(TAG, "onRecognition: entered");
        processDetectionEvent(recognitionEvent);
        if (isServiceStateEnabled && (recognitionEvent.captureAvailable == false)) {
            Log.v(TAG, "onRecognition: serviceStateEnabled so restarting recognition");
            Integer[] recognizedSmHandle = new Integer[]{recognitionEvent.soundModelHandle};
            new RestartRecognitionTask().execute(recognizedSmHandle);
        } else {
            Log.v(TAG, "onRecognition: ST Service is not enabled. Cannot restart recognition.");
        }
        Log.v(TAG, "onRecognition: exiting");
    }

    @Override
    public void onServiceDied() {
        Log.v(TAG, "onServiceDied");
        if (smSessionManager.getIsASessionBuffering()) {
            Log.v(TAG, "onServiceDied: isASessionBuffering is true");
            boolean isLabCanceled = lookAheadBufferTask.cancel(true);
            if (false == isLabCanceled) {
                Log.v(TAG, "onServiceDied: could not stop " +
                        "lookAheadBufferTask. This is most likely because it has" +
                        " already completed.");
                // Even though LAB has already completed, it's safer to detach
                // the module after serviceStateEnabled is sent to avoid a race
                // condition.
            } else {
                //TODO Remove this temporary workaround when Google fixes
                // StatusListener and introduces SoundModuleListener.
                //
                // LAB was cancelled, so wait until that completes,
                // serviceStateEnabled is sent, and then detach module.
                Log.v(TAG, "onServiceDied: LAB was cancelled, so wait " +
                        "until that completes, serviceStateEnabled is sent " +
                        "and then detach module.");
            }
        }
        stModule.detach(); //needed otherwise app hangs on listModules when restarting
        detachAfterServiceStateEnabled = false;
        Log.v(TAG, "onServiceDied: detachAfterServiceStateEnabled set back to false.");

        smSessionManager.updateStateServiceDied();
        notificationManager.cancelAll();
        sendMessageDataAll(MessageType.MSG_LISTEN_STOPPED, Global.SUCCESS, null);

        isServiceStateEnabled = true;
        Log.v(TAG, "onServiceDied: isServiceStateEnabled= " + isServiceStateEnabled);
    }

    /*
     * (non-Javadoc)
     * @see android.hardware.soundtrigger.SoundTrigger.StatusListener#onServiceStateChange(int)
     */
    @Override
    public void onServiceStateChange(int state) {
        Log.v(TAG, "onServiceStateChange: state= " + state);
        if (state == SoundTrigger.SERVICE_STATE_ENABLED) {
            Log.v(TAG, "onServiceStateChange: SoundTrigger.SERVICE_STATE_ENABLED");
            isServiceStateEnabled = true;

            //TODO Remove this temporary workaround when Google fixes
            // StatusListener and introduces SoundModuleListener.
            //
            // If detachAfterServiceStateEnabled is true, LAB just finished and the module can be
            // safely detached. We don't need to start recognition again if this is the case.
            if (detachAfterServiceStateEnabled) {
                Log.v(TAG, "onServiceStateChange: detachAfterServiceStateEnabled is true. Detaching.");
                stModule.detach();
                detachAfterServiceStateEnabled = false;
                Log.v(TAG, "onServiceStateChange: detachAfterServiceStateEnabled set back to false.");
                smSessionManager.updateStateServiceDied();
                sendMessageDataAll(MessageType.MSG_LISTEN_STOPPED, Global.SUCCESS, null);
                return;
            } else {
                Log.v(TAG, "onServiceStateChange: detachAfterServiceStateEnabled is false.");
            }

            // Now that the service is enabled, restart all previously started SMs.
            ArrayList<Integer> startedSessionsSmHandles = smSessionManager.getStartedSessionsSmHandles();
            for (int smHandle : startedSessionsSmHandles) {
                Log.v(TAG, "onRecognition: serviceStateEnabled so restarting recognition for " +
                        "smHandle= " + smHandle);
                new RestartRecognitionTask().execute(smHandle);
            }
            // If registration was successfull, add Voice Activation icon to the notification bar
            //  and let interested parties know listening is running.
            if (startedSessionsSmHandles.size() > 0) {
                // Display the Voice Activation icon in notification bar
                showNotification(R.drawable.logo_status, "Voice Activation is running");
                sendMessageDataAll(MessageType.MSG_LISTEN_RUNNING, Global.SUCCESS, null);
            }
        } else if (state == SoundTrigger.SERVICE_STATE_DISABLED) {
            Log.v(TAG, "onServiceStateChange: SoundTrigger.SERVICE_STATE_DISABLED");
            isServiceStateEnabled = false;

            // Remove the Voice Activation icon from the notification bar if it was showing.
            if (smSessionManager.isASessionStarted()) {
                notificationManager.cancelAll();
                sendMessageDataAll(MessageType.MSG_LISTEN_STOPPED, Global.SUCCESS, null);
            }
        } else {
            Log.e(TAG, "onServiceStateChange: unrecognized state= " + state);
        }
    }

    @Override
    public void onSoundModelUpdate(SoundModelEvent arg0) {
        // not in use.
    }
}
