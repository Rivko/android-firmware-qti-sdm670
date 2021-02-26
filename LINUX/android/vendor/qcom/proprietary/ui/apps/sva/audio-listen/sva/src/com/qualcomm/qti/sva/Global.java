/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import java.util.Arrays;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Environment;
import android.util.Log;

import com.qualcomm.listen.ListenTypes;
import com.qualcomm.listen.ListenTypes.ConfidenceData;
import com.qualcomm.listen.ListenTypes.EventData;
import com.qualcomm.listen.ListenTypes.VoiceWakeupDetectionDataV2;


/**
 * Global object for configuration management and utility functions needed by
 * more than one part of the application.
 */
public class Global {
    private final static String TAG = "ListenLog.Global";
    public final static String APP_NAME = "SVA";
    public final static String PATH_APP = Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + APP_NAME;
    public final static String PATH_TRAINING_RECORDINGS = Global.PATH_APP + "/" + "trainings";
    public final static String PATH_TRAINING_FILENAME = "training";
    public final static String PATH_RECORDINGS_FILEEXT = ".wav";
    public final static String PATH_DATA_FILEEXT = ".data";
    public final static String PATH_RECORDINGS_TEMP_FILE = Global.PATH_TRAINING_RECORDINGS + "/" + "temp_recording.wav";
    public final static String PATH_VOICE_REQUESTS = Global.PATH_APP + "/" + "voiceRequests";
    public final static int NUM_TRAINING_RECORDINGS_REQUIRED = 5;
    public final static int NUM_MAX_KEYPHRASES = 8;
    public final static String PREFERENCE_GROUP_NAME = "SVA";
    public final static String PREFERENCE_TAG_NAME = "Settings";
    public final static String NO_USERNAME = "<No User>";
    public final static String DEFAULT_USERNAME = "defaultUser";
    public final static int SUCCESS = 0;
    public final static int FAILURE = -1;
    public final static double SHORTS_PER_SECOND = 16000;
    public final static int NUM_MAX_SESSIONS = 8;

    // Extras
    public final static String EXTRA_ACTION_UPDATE_KEYPHRASEACTION = "com.qualcomm.qti.sva.Global.updateKeyphraseAction";
    public final static String EXTRA_DATA_KEYPHRASE_NAME = "com.qualcomm.qti.sva.Global.keyphraseName";
    public final static String EXTRA_DATA_KEYPHRASE_ACTION = "com.qualcomm.qti.sva.Global.keyphraseAction";
    public final static String EXTRA_TRAINING_RESULT = "com.qualcomm.qti.sva.Global.trainingResult";
    public final static String EXTRA_ACTION_DELETE_KEYPHRASE = "com.qualcomm.qti.sva.Global.deleteKeyphrase";

    // Tags
    public static final String TAG_FIRST_RUN_OF_APP = "firstRunOfApp";
    public final static String TAG_SETTING_SELECTED_SOUND_MODEL_NAME = "selectedSoundModelName";
    public final static String TAG_SOUND_MODEL_NAME = "soundModelName";
    public final static String TAG_REQUIRES_TRAINING = "isUdkSm";
    public final static String TAG_TRAINING_KEYPHRASE = "trainingKeyword";
    public final static String TAG_TRAINING_USER = "trainingUser";
    public static final String TAG_TRAINING_IS_UDK = "trainingIsUdk";
    public static final String TAG_DELETED_SOUND_MODEL = "deletedSoundModel";
    public static final String TAG_DELETED_SOUND_MODEL_NAME = "deletedSoundModelName";
    public static final String TAG_KEYPHRASE_TO_DELETE = "keyPhraseToDelete";
    public static final String TAG_USER_TO_DELETE = "userToDelete";

    // System Settings Tags
    private final static String TAG_SETTING_DEFAULT_KEYWORD_CONFIDENCE_LEVEL = "keywordThreshold";
    private final static String TAG_SETTING_DEFAULT_USER_CONFIDENCE_LEVEL = "userThreshold";
    private final static String TAG_SETTING_DEFAULT_TRAINING_CONFIDENCE_LEVEL = "trainingThreshold";
    private final static String TAG_SETTING_LISTEN_ENABLED = "listenEnabled";
    private final static String TAG_SETTING_VOICEWAKEUP_ENABLED = "voicewakeupEnabled";
    private final static String TAG_SETTING_TONE_ENABLED = "toneEnabled";
    private final static String TAG_SETTING_SHOW_ADVANCED_DETAIL = "showAdvanceDetail";
    private final static String TAG_SETTING_USER_VERIFICATION_ENABLED = "userVerificationEnabled";
    private final static String TAG_SETTING_VOICE_REQUESTS_ENABLED = "voiceRequestsEnabled";
    private final static String TAG_SETTING_VOICE_REQUEST_LENGTH = "voiceRequestLength";
    private final static String TAG_SETTING_FAILURE_FEEDBACK_ENABLED = "failureFeedbackEnabled";

    //support FR44168
    public enum CTRL_OPAGUE {
        OPAGUE_TRANSFER_OFF,
        OPAGUE_TRANSFER_ON
    }

    public static final String OPAGUE_SHARED_PREFERENCE_NAME = "opague_data";
    public static final String KEY_CONFIG_OPAGUE_TRANSFER_ONOFF = "ctrl_opague_onoff";
    public static final int DEFAULT_OPAGUE_TRANSFER_OFF = CTRL_OPAGUE.OPAGUE_TRANSFER_OFF.ordinal();
    public static final String KEY_CONFIG_CTRL_HIST_BUF_TIME = "hist_buf_time";
    public static final int DEFAULT_HIST_BUF_TIME = 2000;

    // Default values
    public final static String DEFAULT_VERSION_NUMBER = "No Version Number";
    public final static boolean DEFAULT_AUTOSTART = false;
    private final static int DEFAULT_KEYWORD_CONFIDENCE_LEVEL = 69;
    private final static int DEFAULT_USER_CONFIDENCE_LEVEL = 69;
    private final static int DEFAULT_TRAINING_CONFIDENCE_LEVEL = 72;
    public final static boolean DEFAULT_ENABLE_LISTEN = true;
    public final static boolean DEFAULT_ENABLE_VOICEWAKEUP = true;
    public final static boolean DEFAULT_TONE_ENABLED = true;
    public final static boolean DEFAULT_SHOW_ADVANCED_DETAIL = true;
    public final static boolean DEFAULT_USER_VERIFICATION_ENABLED = false;
    public final static boolean DEFAULT_ENABLE_VOICEREQUESTS = false;
    public final static double DEFAULT_VOICEREQUESTLENGTH_SECONDS = 3;
    public final static boolean DEFAULT_FAILURE_FEEDBACK_ENABLED = false;
    public final static String EXTRA_SELECTACTION_MODE = "selectaction mode";
    public final static String MODE_SELECTACTION_TRAINING = "selectaction mode training";

    // Set default values
    private String versionNumber = DEFAULT_VERSION_NUMBER;
    // autoStart is only for APT, does not autoStart if a SM was registered when the app last closed
    private boolean autoStart = DEFAULT_AUTOSTART;

    private int keyPhraseConfidenceLevel = DEFAULT_KEYWORD_CONFIDENCE_LEVEL;
    private int userConfidenceLevel = DEFAULT_USER_CONFIDENCE_LEVEL;
    private int trainingConfidenceLevel = DEFAULT_TRAINING_CONFIDENCE_LEVEL;
    private boolean enableListen = DEFAULT_ENABLE_LISTEN;
    private boolean enableVoiceWakeup = DEFAULT_ENABLE_VOICEWAKEUP;
    private boolean toneEnabled = DEFAULT_TONE_ENABLED;
    private boolean showAdvancedDetail = DEFAULT_SHOW_ADVANCED_DETAIL;
    private boolean userVerificationEnabled = DEFAULT_USER_VERIFICATION_ENABLED;
    private double voiceRequestLengthSeconds = DEFAULT_VOICEREQUESTLENGTH_SECONDS;
    private boolean voiceRequestsEnabled = DEFAULT_ENABLE_VOICEREQUESTS;
    private boolean failureFeedbackEnabled = DEFAULT_FAILURE_FEEDBACK_ENABLED;

    private ConfidenceData confidenceData = new ConfidenceData();
    private ShortBuffer[] userRecordings = new ShortBuffer[5];
    private int numUserRecordings = 0;
    private boolean libsError = false;
    private SoundModelRepository smRepo = new SoundModelRepository();
    private BlockingQueue<DetectionContainer> detectionContainers = new ArrayBlockingQueue<DetectionContainer>(100);
    private int addDetectionCounter;
    private int removeDetectionCounter;
    private String lastVoiceRequestFilePath = null;
    private int numActivitiesShowing = 0;

    private int mMaxUsersPerPhrase = 1;
    private int mMaxActiveSoundModels = 1;

    public enum SmState {
        UNLOADED,
        LOADED,
        STARTED,
        STOPPED
    }

    private Global() {
    }

    private static class GlobalInstance {
        public static Global Instance = new Global();
    }

    public static Global getInstance() {
        return GlobalInstance.Instance;
    }


    // General setters and getters

    public void setVersionNumber(String versionNumber) {
        this.versionNumber = versionNumber;
    }

    public String getVersionNumber() {
        return versionNumber;
    }

    public void setAutoStart(boolean inAutoStart) {
        this.autoStart = inAutoStart;
    }

    public boolean getAutoStart() {
        return this.autoStart;
    }

    public void setLibsError(boolean libsError) {
        this.libsError = libsError;
    }

    public boolean getLibsError() {
        return libsError;
    }

    public int getDetectionMode() {
        if (userVerificationEnabled) {
            return ListenTypes.USER_KEYWORD_DETECTION_MODE;
        } else {
            return ListenTypes.KEYWORD_ONLY_DETECTION_MODE;
        }
    }

    public SoundModelRepository getSmRepo() {
        return smRepo;
    }

    public ByteBuffer getLanguageModel() {
        Log.v(TAG, "getLanguageModel");
        String filePath = Global.PATH_APP + "/" + "default.lm";
        File soundModelFile = new File(filePath);
        if (soundModelFile.exists()) {
            try {
                ByteBuffer languageModel = Utils.readFileToByteBuffer(filePath);
                return languageModel;
            } catch (IOException e) {
                Log.e(TAG, "getKeywordOnlySoundModel: failed to read language model at filePath= " +
                        filePath);
                e.printStackTrace();
                return null;
            }
        } else {
            Log.v(TAG, "getSoundModelFromName: language model with filePath: " + filePath +
                    " does not exist");
            return null;
        }
    }

    public boolean getIsFirstRunOfApp(Context context) {
        SharedPreferences sp = context.getSharedPreferences(PREFERENCE_GROUP_NAME, 0);
        // If the pref doesn't exist, it's the first run.
        boolean isFirstRunOfApp = sp.getBoolean(TAG_FIRST_RUN_OF_APP, true);
        if (isFirstRunOfApp) {
            Editor editor = sp.edit();
            editor.putBoolean(TAG_FIRST_RUN_OF_APP, false);
        }
        return isFirstRunOfApp;
    }

    public int getNumActivitiesShowing() {
        return numActivitiesShowing;
    }


    public void incrementNumActivitiesShowing() {
        this.numActivitiesShowing++;
        Log.v(TAG, "incrementNumActivitiesShowing: numActivitiesShowing now = " +
                this.numActivitiesShowing);
    }

    public void decrementNumActivitiesShowing() {
        this.numActivitiesShowing--;
        Log.v(TAG, "decrementNumActivitiesShowing: numActivitiesShowing now = " +
                this.numActivitiesShowing);
    }


    /*********************
     * Detection functions
     *********************/

    // Add detection information to a DetectionContainer and add
    // that DetectionContainer to a queue. The VwuService adds to this queue
    // and the DetectionActivity removes from the queue to display detection
    // events in the order in which they're received.
    public void setDetectionContainer(int inEventType,
                                      EventData inEventData,
                                      VoiceWakeupDetectionDataV2 inVwuDetectionData,
                                      int inSessionNum,
                                      String inSmName) {
        DetectionContainer detectionContainer = new DetectionContainer(inEventType, inEventData,
                inVwuDetectionData, inSessionNum, inSmName);
        boolean result = this.detectionContainers.offer(detectionContainer);
        if (false == result) {
            Log.e(TAG, "setDetectionContainer: not able to add detection container");
            return;
        }
        this.addDetectionCounter++;
        Log.v(TAG, "setDetectionContainer: addDetectionCounter= " + this.addDetectionCounter);
        Log.v(TAG, "setDetectionContainer: this.detectionContainers.size()= " +
                this.detectionContainers.size());
    }

    public DetectionContainer getDetectionContainer() {
        this.removeDetectionCounter++;
        Log.v(TAG, "getDetectionContainer: removeDetectionCounter= " + this.removeDetectionCounter);
        int newDetectionContainerSize = this.detectionContainers.size() - 1;
        Log.v(TAG, "getDetectionContainer: this.detectionContainers.size() after removal= " +
                newDetectionContainerSize);
        return this.detectionContainers.poll();
    }

    // Container for detection information
    public class DetectionContainer {
        public int eventType;
        public EventData eventData;
        public VoiceWakeupDetectionDataV2 vwuDetectionData;
        public int sessionNum;
        public String smName;

        public DetectionContainer(int inEventType,
                                  EventData inEventData,
                                  VoiceWakeupDetectionDataV2 inVwuDetectionData,
                                  int inSessionNum,
                                  String inSmName) {
            Log.v(TAG, "DetectionContainer: constructor- inSessionNum= " + inSessionNum);
            this.eventType = inEventType;
            this.eventData = inEventData;
            this.vwuDetectionData = inVwuDetectionData;
            this.sessionNum = inSessionNum;
            this.smName = inSmName;
        }
    }


    /********************
     * Training functions
     ********************/

    public ShortBuffer[] getUserRecordings() {
        return this.userRecordings;
    }

    public ConfidenceData getConfidenceData() {
        return this.confidenceData;
    }

    // Returns a shortBuffer of the last user training recorded
    public void addUserRecording() {
        Log.v(TAG, "addUserRecording: numUserRecordings before insert= " + this.numUserRecordings);
        try {
            String filePath = getLastUserRecordingFilePath();
            this.userRecordings[this.numUserRecordings++] = Utils.readWavFile(filePath);
        } catch (FileNotFoundException f) {
            Log.e(TAG, "addUserRecording: File cannot be opened or created based on mode. Error= " +
                    f.getMessage());
        } catch (IOException e) {
            Log.e(TAG, "addUserRecording: Unable to readWaveFile. File is closed or another I/O " +
                    "error has occurred. Error= " + e.getMessage());
        }
        Log.v(TAG, "addUserRecording: this.numUserRecordings after insert= " + this.numUserRecordings);
    }

    // Returns a shortBuffer of the last user training recorded
    public ShortBuffer getLastUserRecording() {
        Log.v(TAG, "getLastUserRecording: getNumUserRecordings()= " + getNumUserRecordings());
        int num = getNumUserRecordings();
        if (num > NUM_TRAINING_RECORDINGS_REQUIRED) {
            num = NUM_TRAINING_RECORDINGS_REQUIRED;
        }

        if (num > 0) {
            num = num - 1;
        }

        return this.userRecordings[num];
    }

    // Returns a filepath for the last user training recorded
    public String getLastUserRecordingFilePath() {
        String recordingNumber = Integer.toString(1 + this.numUserRecordings);
        String filePath = Global.PATH_TRAINING_RECORDINGS + "/" + PATH_TRAINING_FILENAME + recordingNumber +
                PATH_RECORDINGS_FILEEXT;
        Log.v(TAG, "getLastUserRecordingFilePath: filePath= " + filePath);
        return filePath;
    }

    // Removes the recording files
    public void removeExistingRecordingFiles() {
        for (int i = 0; i < NUM_TRAINING_RECORDINGS_REQUIRED; i++) {
            String recordingNumber = Integer.toString(1 + i);
            String filePath = Global.PATH_TRAINING_RECORDINGS + "/" + PATH_TRAINING_FILENAME + recordingNumber +
                    PATH_RECORDINGS_FILEEXT;
            Log.v(TAG, "removeExistingRecordingFiles: filePath= " + filePath);
            File recordingFile = new File(filePath);
            if (recordingFile.exists()) {
                recordingFile.delete();
                Log.v(TAG, "removeExistingRecordingFiles: file deleted= " + filePath);
            }
        }
    }

    // Resets the user recordings for the next training session
    public void removeUserRecordings() {
        Log.v(TAG, "removeUserRecordings: getNumUserRecordings() before remove= " + getNumUserRecordings());
        Arrays.fill(this.userRecordings, null);
        this.numUserRecordings = 0;
        Log.v(TAG, "removeUserRecordings: getNumUserRecordings() after remove= " + getNumUserRecordings());
    }

    public int getNumUserRecordings() {
        return this.numUserRecordings;
    }

    // Removes the last recording trained by a user when it's unsuccessful
    public void discardLastUserRecording() {
        Log.v(TAG, "discardLastUserRecording: getNumUserRecordings() before discard= " +
                getNumUserRecordings());
        this.numUserRecordings--;
        Log.v(TAG, "discardLastUserRecording: getNumUserRecordings() after discard= " +
                getNumUserRecordings());
    }


    /*************************
     * Voice Request Functions
     *************************/

    //public void saveVoiceRequest(byte[] voiceRequest, String filePath) {
    public void saveVoiceRequest(byte[] voiceRequest, int bufferSize, String filePath) {
        Log.v(TAG, "saveVoiceRequest");
        Utils.writeBufferToWavFile(voiceRequest, bufferSize, filePath, false);
        this.lastVoiceRequestFilePath = filePath;
    }

    public String getLastVoiceRequestFilePath() {
        return this.lastVoiceRequestFilePath;
    }


    /********************
     * Settings Functions
     ********************/

    // Save settings to Share Preferences
    public void saveSettingsToSharedPreferences(Context context) {
        SharedPreferences sp = context.getSharedPreferences(PREFERENCE_GROUP_NAME, 0);

        Editor editor = sp.edit();

        editor.putInt(TAG_SETTING_DEFAULT_KEYWORD_CONFIDENCE_LEVEL, this.keyPhraseConfidenceLevel);
        editor.putInt(TAG_SETTING_DEFAULT_USER_CONFIDENCE_LEVEL, this.userConfidenceLevel);
        editor.putInt(TAG_SETTING_DEFAULT_TRAINING_CONFIDENCE_LEVEL, this.trainingConfidenceLevel);
        editor.putBoolean(TAG_SETTING_LISTEN_ENABLED, this.enableListen);
        editor.putBoolean(TAG_SETTING_VOICEWAKEUP_ENABLED, this.enableVoiceWakeup);
        editor.putBoolean(TAG_SETTING_TONE_ENABLED, this.toneEnabled);
        editor.putBoolean(TAG_SETTING_SHOW_ADVANCED_DETAIL, this.showAdvancedDetail);
        editor.putBoolean(TAG_SETTING_USER_VERIFICATION_ENABLED, this.userVerificationEnabled);
        editor.putBoolean(TAG_SETTING_VOICE_REQUESTS_ENABLED, this.voiceRequestsEnabled);
        editor.putString(TAG_SETTING_VOICE_REQUEST_LENGTH, String.valueOf(this.voiceRequestLengthSeconds));
        editor.putBoolean(TAG_SETTING_FAILURE_FEEDBACK_ENABLED, this.failureFeedbackEnabled);

        editor.commit();
    }

    // Load settings from Shared Preferences.
    // If the item is not in the saved context, use the value set by default.
    public void loadSettingsFromSharedPreferences(Context context) {
        SharedPreferences sp = context.getSharedPreferences(PREFERENCE_GROUP_NAME, 0);

        this.keyPhraseConfidenceLevel = sp.getInt(TAG_SETTING_DEFAULT_KEYWORD_CONFIDENCE_LEVEL,
                DEFAULT_KEYWORD_CONFIDENCE_LEVEL);
        this.userConfidenceLevel = sp.getInt(TAG_SETTING_DEFAULT_USER_CONFIDENCE_LEVEL,
                DEFAULT_USER_CONFIDENCE_LEVEL);
        this.trainingConfidenceLevel = sp.getInt(TAG_SETTING_DEFAULT_TRAINING_CONFIDENCE_LEVEL,
                DEFAULT_TRAINING_CONFIDENCE_LEVEL);
        this.enableListen = sp.getBoolean(TAG_SETTING_LISTEN_ENABLED, this.enableListen);
        this.enableVoiceWakeup = sp.getBoolean(TAG_SETTING_VOICEWAKEUP_ENABLED, this.enableVoiceWakeup);
        this.toneEnabled = sp.getBoolean(TAG_SETTING_TONE_ENABLED, this.toneEnabled);
        this.showAdvancedDetail = sp.getBoolean(TAG_SETTING_SHOW_ADVANCED_DETAIL, this.showAdvancedDetail);
        this.userVerificationEnabled = sp.getBoolean(TAG_SETTING_USER_VERIFICATION_ENABLED,
                this.userVerificationEnabled);
        this.voiceRequestsEnabled = sp.getBoolean(TAG_SETTING_VOICE_REQUESTS_ENABLED,
                this.voiceRequestsEnabled);
        this.voiceRequestLengthSeconds = Double.parseDouble(sp.getString(
                TAG_SETTING_VOICE_REQUEST_LENGTH,
                String.valueOf(this.voiceRequestLengthSeconds)));
        this.failureFeedbackEnabled = sp.getBoolean(TAG_SETTING_FAILURE_FEEDBACK_ENABLED,
                this.failureFeedbackEnabled);

        Log.v(TAG, "loadSettingsFromSharedPreferences: defaultKeywordConfidenceLevel= " +
                this.keyPhraseConfidenceLevel);
        Log.v(TAG, "loadSettingsFromSharedPreferences: defaultUserConfidenceLevel= " +
                this.userConfidenceLevel);
        Log.v(TAG, "loadSettingsFromSharedPreferences: defaultTrainingConfidenceLevel= " +
                this.trainingConfidenceLevel);
        Log.v(TAG, "loadSettingsFromSharedPreferences: enableListen= " +
                this.enableListen);
        Log.v(TAG, "loadSettingsFromSharedPreferences: enableVoiceWakeup= " +
                this.enableVoiceWakeup);
        Log.v(TAG, "loadSettingsFromSharedPreferences: toneEnabled= " +
                this.toneEnabled);
        Log.v(TAG, "loadSettingsFromSharedPreferences: showAdvancedDetail= " +
                this.showAdvancedDetail);
        Log.v(TAG, "loadSettingsFromSharedPreferences: userVerificationEnabled= " +
                this.userVerificationEnabled);
        Log.v(TAG, "loadSettingsFromSharedPreferences: voiceRequestsEnabled= " +
                this.voiceRequestsEnabled);
        Log.v(TAG, "loadSettingsFromSharedPreferences: voiceRequestLength= " +
                this.voiceRequestLengthSeconds);
        Log.v(TAG, "loadSettingsFromSharedPreferences: failureFeedbackEnabled= " +
                this.failureFeedbackEnabled);
    }

    // Settings setters and getters

    public void setSettingKeyPhraseThreshold(int inThreshold) {
        keyPhraseConfidenceLevel = inThreshold;
    }

    public int getSettingKeyPhraseConfidenceLevel() {
        return keyPhraseConfidenceLevel;
    }

    public void setSettingUserConfidenceLevel(int inThreshold) {
        userConfidenceLevel = inThreshold;
    }

    public int getSettingUserConfidenceLevel() {
        return userConfidenceLevel;
    }

    public void setSettingTrainingConfidenceLevel(int inThreshold) {
        trainingConfidenceLevel = inThreshold;
    }

    public int getSettingTrainingConfidenceLevel() {
        return trainingConfidenceLevel;
    }

    public void setSettingEnableListen(boolean checked) {
        enableListen = checked;
    }

    public boolean getSettingEnableListen() {
        return enableListen;
    }

    public void setSettingEnableVoiceWakeup(boolean checked) {
        enableVoiceWakeup = checked;
    }

    public boolean getSettingEnableVoiceWakeup() {
        return enableVoiceWakeup;
    }

    public void setSettingDetectionTone(boolean checked) {
        toneEnabled = checked;
    }

    public boolean getSettingDetectionTone() {
        return toneEnabled;
    }

    public void setSettingShowAdvancedDetail(boolean checked) {
        showAdvancedDetail = checked;
    }

    public boolean getSettingShowAdvancedDetail() {
        return showAdvancedDetail;
    }

    public void setSettingUserVerification(boolean checked) {
        userVerificationEnabled = checked;
    }

    public boolean getSettingUserVerification() {
        return userVerificationEnabled;
    }

    public void setSettingVoiceRequestsEnabled(boolean checked) {
        voiceRequestsEnabled = checked;
    }

    public boolean getSettingVoiceRequestsEnabled() {
        return voiceRequestsEnabled;
    }

    public void setSettingVoiceRequestLength(double inLength) {
        Log.v(TAG, "setSettingVoiceRequestLength: inLength= " + inLength);
        voiceRequestLengthSeconds = inLength;
    }

    public double getSettingVoiceRequestLength() {
        return voiceRequestLengthSeconds;
    }

    public void setSettingFailureFeeback(boolean checked) {
        failureFeedbackEnabled = checked;
    }

    public boolean getSettingFailureFeedback() {
        return failureFeedbackEnabled;
    }

    public void setMaxUsersPerPhrase(int num) {
        if (num <= 0) return;
        mMaxUsersPerPhrase = num;
    }

    public int getMaxUsersPerPhrase() {
        return mMaxUsersPerPhrase;
    }


    public void setMaxActiveSoundModels(int maxActiveSoundModels) {
        if (maxActiveSoundModels <= 0) return;
        mMaxActiveSoundModels = maxActiveSoundModels;
    }

    public int getMaxActiveSoundModels() {
        return mMaxActiveSoundModels;
    }


    //support FR44168
    public static boolean getIsSupportFR44168(Context context) {
        return context.getResources().getBoolean(R.bool.supportFR44168);
    }

    public SharedPreferences getOpagueSharedPreferences(Context context) {
        return context.getSharedPreferences(OPAGUE_SHARED_PREFERENCE_NAME, Context.MODE_PRIVATE);
    }

    public void setOpagueSharedPreference(Context context, String key, int value) {
        SharedPreferences sp = getOpagueSharedPreferences(context);
        sp.edit().putInt(key, value).commit();
    }

    public int getConfigOpagueTransferONOFF(Context context) {
        SharedPreferences sp = getOpagueSharedPreferences(context);
        return sp.getInt(KEY_CONFIG_OPAGUE_TRANSFER_ONOFF, DEFAULT_OPAGUE_TRANSFER_OFF);
    }

    public int getConfigHistBufTime(Context context) {
        SharedPreferences sp = getOpagueSharedPreferences(context);
        return sp.getInt(KEY_CONFIG_CTRL_HIST_BUF_TIME, DEFAULT_HIST_BUF_TIME);
    }
}
