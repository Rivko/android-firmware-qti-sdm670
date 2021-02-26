/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashSet;

import com.qualcomm.listen.ListenTypes.SoundModelInfo;
import com.qualcomm.listen.ListenTypes.VWUKeywordConfLevel;
import com.qualcomm.listen.ListenTypes.KeywordInfo;
import com.qualcomm.listen.ListenTypes.SVASoundModelInfo;
import com.qualcomm.listen.ListenTypes.VWUUserKeywordPairConfLevel;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

public class SoundModelRepository {
    private static final String TAG = "SoundModelRepository";
    private final static String FILEEXT_SOUNDMODEL_DEV_KEYPHRASE = ".uim";
    private final static String FILEEXT_SOUNDMODEL_DEV_USER = ".udm";
    private final static String FILEEXT_SOUNDMODEL_COM_KEYPHRASE = ".uimc";
    private final static String FILEEXT_SOUNDMODEL_COM_USER = ".udmc";
    private final static String TAG_DELIMITER = "|";
    private final static String TAG_PREFERENCE_SELECTEDKEYPHRASES =
            "com.qualcomm.qti.sva.selectedkeyphrases";

    ArrayList<SoundModel> soundModels = new ArrayList<SoundModel>();
    private String selectedSoundModelName = null;
    private String soundModelNameToQuery = null;
    private String soundModelNameToExtend = null;
    private String soundModelNameToDeleteFrom = null;
    private String mergedSoundModelName = null;
    private ByteBuffer extendedSoundModel = null;
    private ByteBuffer soundModelToModifyByteBuffer = null;
    ArrayList<ByteBuffer> soundModelsToMergeByteBuffers = new ArrayList<ByteBuffer>();
    private ArrayList<String> loadedSmNames = null;
    private Keyphrase tempTrainingComKeyphrase = null;
    private String heySnapdragonUimcName = "HeySnapdragon.uimc";
    private String heySnapdragonUdmcName = "HeySnapdragon.udmc";
    private String heySnapdragonKeyphraseName = "Hey Snapdragon";
    private String niHaoXiaoLongUimcName = "NiHaoXiaoLong.uimc";
    private String niHaoXiaoLongUdmcName = "NiHaoXiaoLong.udmc";
    private String niHaoXiaoLongKeyphraseName = "Ni Hao Xiao Long";

    SoundModelRepository() {
        //nada
    }

    public void clearAllSoundModels() {
        if (soundModels != null && !soundModels.isEmpty()) {
            soundModels.clear();
        }
    }

    // Gets all sound models from the file system
    public ArrayList<SoundModel> getAllSoundModels() {
        Log.v(TAG, "getAllSoundModels");

        // If there exists sound models in the list, just return the list instead of building it
        // over again.
        if (false == soundModels.isEmpty()) {
            return soundModels;
        }

        File dir = new File(Global.PATH_APP);

        File[] files = dir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                if (filename.endsWith(FILEEXT_SOUNDMODEL_DEV_KEYPHRASE) ||
                        filename.endsWith(FILEEXT_SOUNDMODEL_DEV_USER)) {
                    return true;
                }
                return false;
            }
        });
        if (null == files) {
            Log.e(TAG, "getAllSoundModels: no sound models found");
            return null;
        }


        boolean foundSelectedSoundModel = false;
        for (File file : files) {
            String soundModelFileName = file.getName();

            if (soundModelFileName.contains(".vwu")) {
                Log.e(TAG, "getAllSoundModels: Error- v1.0 SM detected");
                continue;
            } else if (soundModels.contains(new SoundModel(soundModelFileName))) {
                Log.v(TAG, "getAllSoundModels: duplicate sound model name: " + soundModelFileName +
                        " skipping adding SM and continuing to next SM");
                continue;
            } else if (null != this.selectedSoundModelName && this.selectedSoundModelName.equals(
                    soundModelFileName)) {
                foundSelectedSoundModel = true;
            }

            Log.v(TAG, "getAllSoundModels: adding soundModelName= " + soundModelFileName);
            soundModels.add(new SoundModel(soundModelFileName));
        }

        if (false == foundSelectedSoundModel) {
            this.selectedSoundModelName = null;
        }

        Collections.sort(soundModels);
        return soundModels;
    }

    // if boolean arg is false, get all the user made sound models
    public ArrayList<String> getSoundModelNames(final boolean onlyKeyPhaseSoundModels) {
        Log.v(TAG, "getSoundModelNames");
        File dir = new File(Global.PATH_APP);
        File[] files = dir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                if (onlyKeyPhaseSoundModels) {
                    return filename.endsWith(FILEEXT_SOUNDMODEL_DEV_KEYPHRASE);
                } else {
                    return filename.endsWith(FILEEXT_SOUNDMODEL_DEV_USER) ||
                            filename.endsWith(FILEEXT_SOUNDMODEL_DEV_KEYPHRASE);
                }
            }
        });

        if (null == files) {
            Log.e(TAG, "getSoundModelNames: no sound models found");
            return null;
        }

        ArrayList<String> soundModelArrayList = new ArrayList<String>();
        String soundModelFileName;
        String keyPhraseName = "";
        for (int i = 0; i < files.length; i++) {
            soundModelFileName = files[i].getName();
            if (onlyKeyPhaseSoundModels) {
                keyPhraseName = soundModelFileName.substring(0, soundModelFileName.lastIndexOf('.'));
                Log.v(TAG, "getSoundModelNames: keyPhraseName= " + keyPhraseName);
                if (false == soundModelArrayList.contains(keyPhraseName)) {
                    soundModelArrayList.add(keyPhraseName);
                } else {
                    Log.e(TAG, "getSoundModelNames: keyPhraseName= " + keyPhraseName + " is a duplicate.");
                }
            } else {
                if (false == soundModelArrayList.contains(soundModelFileName)) {
                    soundModelArrayList.add(soundModelFileName);
                } else {
                    Log.e(TAG, "getSoundModelNames: soundModelFileName= " + soundModelFileName +
                            " is a duplicate.");
                }
            }
        }

        return soundModelArrayList;
    }


    public String getSmPrettyName(String uglyName) {
        return uglyName.substring(0, uglyName.indexOf("."));
    }

    /***
     * Can't use because we don't usually have all Sound Models queried,
     * and therefore don't usually know all users.
     *
     * @return string array of users
     */
    public String[] getAllUsers() {
        Log.v(TAG, "getAllUsers");

        ArrayList<String> userArrayList = new ArrayList<String>();
        for (SoundModel soundModel : soundModels) {
            for (Keyphrase keyPhrase : soundModel.getKeyphrases()) {
                for (User user : keyPhrase.getUsers()) {
                    if (false == userArrayList.contains(user.name)) {
                        userArrayList.add(user.name);
                    }
                }
            }
        }
        Collections.sort(userArrayList);
        String[] userArray = userArrayList.toArray(new String[userArrayList.size()]);
        return userArray;
    }


    public VWUKeywordConfLevel[] getKeyphraseConfidenceThresholdsBySmName(Context context,
                                                                          String smName) {
        Log.v(TAG, "getKeyphraseConfidenceThresholdsBySmName: smName= " + smName);
        SoundModel selectedSoundModel = getSmByName(smName);
        if (null == selectedSoundModel) {
            Log.e(TAG, "getKeyphraseConfidenceThresholdsBySmName: cannot get SM for smName= " + smName);
            return null;
        }
        ArrayList<VWUKeywordConfLevel> keywordConfLevels = new ArrayList<VWUKeywordConfLevel>();
        for (Keyphrase keyword : selectedSoundModel.getKeyphrases()) {
            if (keyword == null) {
                continue;
            }

            VWUKeywordConfLevel keywordConfLevel = new VWUKeywordConfLevel();
            keywordConfLevel.keyword = keyword.getName();
            keywordConfLevel.confLevel = (short) getConfidenceLevel(context,
                    selectedSoundModel.getName(),
                    keyword.getName(),
                    null);
            Log.v(TAG, "getKeyphraseConfidenceThresholdsBySmName: smName= " + smName +
                    ", keyword= " + keyword.getName() + ", confLevel= " + keywordConfLevel.confLevel);
            keywordConfLevels.add(keywordConfLevel);
        }
        return keywordConfLevels.toArray(new VWUKeywordConfLevel[keywordConfLevels.size()]);
    }

    public VWUUserKeywordPairConfLevel[] getUserConfidenceThresholdsBySmName(Context context,
                                                                             String smName) {
        Log.v(TAG, "getUserConfidenceThresholdsBySmName: smName= " + smName);
        SoundModel selectedSoundModel = getSmByName(smName);

        ArrayList<VWUUserKeywordPairConfLevel> userConfLevels = new ArrayList<VWUUserKeywordPairConfLevel>();
        for (Keyphrase keyword : selectedSoundModel.getKeyphrases()) {
            for (User user : keyword.getUsers()) {
                if (user == null) {
                    continue;
                }

                VWUUserKeywordPairConfLevel userConfLevel = new VWUUserKeywordPairConfLevel();
                userConfLevel.keyword = keyword.getName();
                userConfLevel.user = user.getName();
                userConfLevel.confLevel = (short) getConfidenceLevel(context,
                        selectedSoundModel.getName(),
                        keyword.getName(),
                        user.getName());
                Log.v(TAG, "getKeyphraseConfidenceThresholdsBySmName: smName= " + smName +
                        ", keyword= " + keyword.getName() + ", userName= " + user.getName() +
                        ", confLevel= " + userConfLevel.confLevel);
                userConfLevels.add(userConfLevel);
            }
        }
        return userConfLevels.toArray(new VWUUserKeywordPairConfLevel[userConfLevels.size()]);
    }


    /*** Registered Sound Model Functions **/

    // Check if a sound model file exists by its keyword and username
    private boolean soundModelFileExists(String inSoundModelName) {
        File soundModelFile = new File(generateSoundModelFilePath(inSoundModelName));
        return soundModelFile.exists();
    }

    // Given a keyword and user name provide the name of the sound model
    private String generateSoundModelFilePath(String inSoundModelName) {
        Log.v(TAG, "generateSoundModelFilePath: path= " + Global.PATH_APP + "/" +
                inSoundModelName);
        return Global.PATH_APP + "/" + inSoundModelName;
    }

    public boolean isKeyphraseSoundModel(String inSoundModelName) {
        return inSoundModelName.contains(FILEEXT_SOUNDMODEL_DEV_KEYPHRASE);
    }

    public boolean soundModelExists(String soundModelName) {
        return soundModelFileExists(soundModelName);
    }

    public String getUserSoundModelName(String soundModelName) {
        return soundModelName + FILEEXT_SOUNDMODEL_DEV_USER;
    }

    public String getSystemSoundModelName(String soundModelName) {
        return soundModelName + FILEEXT_SOUNDMODEL_DEV_KEYPHRASE;
    }

    public String getSoundModelNameFromLongName(String soundModelName) {
        return soundModelName.substring(0, soundModelName.lastIndexOf('.'));
    }

    // Returns true if the service needs to be called to merge to SMs
    public boolean createNewSoundModel(ArrayList<String> baseSoundModelNames, String newSoundModelName) {
        if (0 == baseSoundModelNames.size() || newSoundModelName == null) {
            Log.e(TAG, "createNewSoundModel: bad params");
        }

        if (1 == baseSoundModelNames.size()) {
            String sourceFilePath = Global.PATH_APP + "/" + baseSoundModelNames.get(0);
            String destFilePath = Global.PATH_APP + "/" + newSoundModelName;
            Utils.createNewSoundModel(sourceFilePath, destFilePath);
            soundModels.add(new SoundModel(newSoundModelName));
            return false;
        } else {
            this.soundModelsToMergeByteBuffers.clear();
            for (String soundModelToAddName : baseSoundModelNames) {
                if (soundModelExists(soundModelToAddName)) {
                    this.soundModelsToMergeByteBuffers.add(
                            getSoundModelByteBufferFromName(soundModelToAddName));
                } else {
                    Log.e(TAG, "createNewSoundModel: sound model: " + soundModelToAddName +
                            " does not exist");
                }
            }
            setMergedSoundModelName(newSoundModelName);
            return true;
        }
    }

    public ArrayList<ByteBuffer> getSoundModelsToMergeByteBuffers() {
        return this.soundModelsToMergeByteBuffers;
    }

    public void setSoundModelNameToQuery(String inSoundModelName) {
        this.soundModelNameToQuery = inSoundModelName;
        Log.v(TAG, "setSoundModelNameToQuery: sound model name= " + soundModelNameToQuery);
    }

    public ByteBuffer getSoundModelByteBufferToQuery() {
        return getSoundModelByteBufferFromName(this.soundModelNameToQuery);
    }

    public SoundModel getQueriedSoundModel() {
        if (null == this.soundModelNameToQuery) {
            return null;
        }
        return getSmByName(this.soundModelNameToQuery);
    }

    public SoundModel getSmByName(String inSoundModelName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "getSoundModelByName: SM passed in is null");
            return null;
        } else if (inSoundModelName.equals(heySnapdragonUdmcName)) {
            return getComSoundModelFromKeyphrase(heySnapdragonKeyphraseName);
        } else if (inSoundModelName.equals(niHaoXiaoLongUdmcName)) {
            return getComSoundModelFromKeyphrase(niHaoXiaoLongKeyphraseName);
        }
        for (SoundModel soundModel : soundModels) {
            if (soundModel.equals(new SoundModel(inSoundModelName))) {
                return soundModel;
            }
        }
        Log.e(TAG, "getSoundModelByName: SM= " + inSoundModelName + " does not exist");
        return null;
    }

    public void addSmInfoFromQuery(SoundModelInfo smInfo, ArrayList<String> userKeyphrasePairIndices) {
        Log.v(TAG, "addSmInfoFromQuery: Adding info for SM: " + this.soundModelNameToQuery);
        SVASoundModelInfo v2SoundModelInfo = (SVASoundModelInfo) smInfo;
        KeywordInfo[] kwInfos = v2SoundModelInfo.keywordInfo;

        //temp code
        boolean fillIndices = true;
        if (userKeyphrasePairIndices == null) {
            fillIndices = false;
        }

        // Build the sound model from the KeywordInfo[] object.
        SoundModel soundModelToAdd = new SoundModel(this.soundModelNameToQuery);
        Log.v(TAG, "addSmInfoFromQuery: Adding SM: " + this.soundModelNameToQuery + " with: " +
                kwInfos.length + " keywords.");
        int keyphraseIndex = -2; // -1 is already taken for index not found.
        int userIndex = -2; // -1 is already taken for index not found.
        for (KeywordInfo kwInfo : kwInfos) {
            Keyphrase keyphraseToAdd;
            if (fillIndices) {
                keyphraseIndex = userKeyphrasePairIndices.indexOf(kwInfo.keywordPhrase);
                Log.v(TAG, "addSmInfoFromQuery: found keyphraseIndex= " + keyphraseIndex);
                // Error checking. If userKeyphrasePairIndices doesn't contain kwInfo.keywordPhrase.
                if (keyphraseIndex == -1) {
                    Log.e(TAG, "addSmInfoFromQuery: userKeyphrasePairIndices doesn't contain " +
                            "kwInfo.keywordPhrase= " + kwInfo.keywordPhrase);
                }
                keyphraseToAdd = new Keyphrase(kwInfo.keywordPhrase, keyphraseIndex);
            } else {
                keyphraseToAdd = new Keyphrase(kwInfo.keywordPhrase, -3);
            }
            Log.v(TAG, "addSmInfoFromQuery: Building SM: " + this.soundModelNameToQuery +
                    " adding keyphrase: " + kwInfo.keywordPhrase + " with ID= " + keyphraseIndex);
            for (String userName : kwInfo.activeUsers) {
                if (fillIndices) {
                    userIndex = userKeyphrasePairIndices.indexOf(userName + "|"
                            + kwInfo.keywordPhrase);
                    Log.v(TAG, "addSmInfoFromQuery: found userIndex= " + userIndex + " for " +
                            "user|keyphrase= " + userName + "|" + kwInfo.keywordPhrase);
                    // Error checking. If userKeyphrasePairIndices doesn't contain kwInfo.keywordPhrase.
                    if (userIndex == -1) {
                        Log.e(TAG, "addSmInfoFromQuery: userKeyphrasePairIndices doesn't contain " +
                                "user|keyphrase= " + userName + "|" + kwInfo.keywordPhrase);
                    }
                    keyphraseToAdd.addUser(userName, userIndex);
                } else {
                    keyphraseToAdd.addUser(userName, -3);
                }
                Log.v(TAG, "addSmInfoFromQuery: Building SM: " + this.soundModelNameToQuery +
                        " building keyword: " + kwInfo.keywordPhrase + " adding user: " + userName);
            }
            soundModelToAdd.addKeyphrase(keyphraseToAdd);
        }

        // if the sound model isn't already in the list, add it.
        if (false == soundModels.contains(new SoundModel(this.soundModelNameToQuery))) {
            soundModels.add(soundModelToAdd);
            Log.v(TAG, "addSmInfoFromQuery: Added SM: " + this.soundModelNameToQuery);
        } else {
            // the sound model is already in the list, write over existing sound model.
            Log.v(TAG, "addSmInfoFromQuery: SM: " + this.soundModelNameToQuery +
                    " already exists, writing over exiting");
            Log.v(TAG, "addSmInfoFromQuery: printSms()= " + printSms());
            for (SoundModel soundModel : soundModels) {
                Log.v(TAG, "addSmInfoFromQuery: testing SM: " + soundModel.getName());
                if (soundModel.equals(soundModelToAdd)) {
                    Log.v(TAG, "addSmInfoFromQuery: found SM: " + soundModel.getName());
                    Log.v(TAG, "addSmInfoFromQuery: " + printSms());
                    if (soundModels.remove(soundModel)) {
                        Log.v(TAG, "addSmInfoFromQuery: removed SM: " + soundModel.getName());
                        soundModels.add(soundModelToAdd);
                        Collections.sort(soundModels);
                        Log.v(TAG, "addSmInfoFromQuery: " + printSms());
                    } else {
                        Log.e(TAG, "addSmInfoFromQuery: could not be remove SM: " + soundModel.getName());
                    }
                    break;
                }
            }
        }
    }


    /*** Session-related functions ***/


    public void setLoadedSmNames(ArrayList<String> inLoadedSmNames) {
        this.loadedSmNames = inLoadedSmNames;
        for (int i = 0; i < this.loadedSmNames.size(); i++) {
            Log.v(TAG, "setLoadedSmNames: loadedSmNames.get(" + i + ")= " +
                    this.loadedSmNames.get(i));
        }
    }

    public ArrayList<String> getLoadedSmNames() {
        return this.loadedSmNames;
    }


    /*** SMDetailsActivity Functions **/

    public void toggleLaunchPreference(Context context, String inSoundModelName, String inKeywordName,
                                       String inUserName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Could not get launch preference because sound model name was null");
            return;
        }
        SoundModel launchPreferenceSoundModel = getSmByName(inSoundModelName);
        if (null == launchPreferenceSoundModel) {
            Log.e(TAG, "Could not get launch preference because sound model does not exist");
            return;
        }

        String launchPreferenceTag = generateSettingsTag(SoundModel.Setting.LaunchPreference,
                inSoundModelName, inKeywordName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        boolean launchPreference = sp.getBoolean(launchPreferenceTag, false);
        Log.v(TAG, "launchPreference for SM: " + inSoundModelName + ":" + inKeywordName +
                ":" + inUserName + " was: " + launchPreference);
        Editor editor = sp.edit();
        editor.putBoolean(launchPreferenceTag, !launchPreference);
        editor.commit();
    }

    public boolean getLaunchPreference(Context context, String inSoundModelName, String inKeywordName,
                                       String inUserName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Could not get launch preference because sound model name was null");
            return false;
        }
        SoundModel launchPreferenceSoundModel = getSmByName(inSoundModelName);
        if (null == launchPreferenceSoundModel) {
            Log.e(TAG, "Could not get launch preference because sound model does not exist");
            return false;
        }

        String launchPreferenceTag = generateSettingsTag(SoundModel.Setting.LaunchPreference,
                inSoundModelName, inKeywordName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        boolean launchPreference = sp.getBoolean(launchPreferenceTag, false);
        Log.v(TAG, "launchPreference for SM: " + inSoundModelName + ":" + inKeywordName +
                ":" + inUserName + "=" + launchPreference);
        return launchPreference;
    }

    public void setConfidenceLevel(Context context, String inSoundModelName, String inKeywordName,
                                   String inUserName, int inConfidenceLevel) {
        String confidenceLevelTag = generateSettingsTag(SoundModel.Setting.ConfidenceThreshold,
                inSoundModelName, inKeywordName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        Editor editor = sp.edit();
        editor.putInt(confidenceLevelTag, inConfidenceLevel);
        editor.commit();
    }

    // returns -1 for failure
    public int getConfidenceLevel(Context context, String inSoundModelName, String inKeyphraseName,
                                  String inUserName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Could not get confidence level because sound model name was null");
            return -1;
        }
        SoundModel confLevelSoundModel = getSmByName(inSoundModelName);
        if (null == confLevelSoundModel) {
            Log.e(TAG, "Could not get confidence level because sound model does not exist");
            return -1;
        }

        String confidenceLevelTag = generateSettingsTag(SoundModel.Setting.ConfidenceThreshold,
                inSoundModelName, inKeyphraseName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        int confidenceLevel = -1;
        if (null == inUserName) {
            confidenceLevel = sp.getInt(confidenceLevelTag, Global.getInstance().
                    getSettingKeyPhraseConfidenceLevel());
        } else {
            confidenceLevel = sp.getInt(confidenceLevelTag, Global.getInstance().
                    getSettingUserConfidenceLevel());
        }
        Log.v(TAG, "getConfidenceLevel: confidenceLevel for SM: " + inSoundModelName + ":" +
                inKeyphraseName + ":" + inUserName + "=" + confidenceLevel);
        return confidenceLevel;
    }

    // session uses: setting, and inSession
    // launch and confidenceThreshold use: setting, inSmName, inKwName, inUName
    // unused input params should be passed as null
    private String generateSettingsTag(SoundModel.Setting setting,
                                       String inSoundModelName,
                                       String inKeywordName,
                                       String inUserName,
                                       Integer inSessionNum) {
        String specificSetting = "";

        switch (setting) {
            case Session:
                specificSetting = "session";
                return Global.PREFERENCE_TAG_NAME + TAG_DELIMITER + specificSetting +
                        TAG_DELIMITER + inSessionNum;
            case LaunchPreference:
                specificSetting = "launch";
                break;
            case ConfidenceThreshold:
                specificSetting = "conf";
                break;
            default:
                break;
        }

        // for launch preference and confidence threshold
        return Global.PREFERENCE_TAG_NAME + TAG_DELIMITER + specificSetting + TAG_DELIMITER +
                inSoundModelName + TAG_DELIMITER + inKeywordName + TAG_DELIMITER + inUserName;
    }


    /*** Extend Functions ***/

    public void setSoundModelNameToExtend(String inSoundModelName) {
        this.soundModelNameToExtend = inSoundModelName;
    }

    public ByteBuffer getSoundModelByteBufferToExtend() {
        return getSoundModelByteBufferFromName(this.soundModelNameToExtend);
    }

    // Creates a temporary sound model once the extended sound model size is known.
    public void createExtendedSoundModelByteBuffer(int size) {
        this.extendedSoundModel = ByteBuffer.allocate(size);
    }

    // Gets temporary sound model byte buffer created by createExtendedSoundModelByteBuffer()
    public ByteBuffer getExtendedSoundModel() {
        if (null == this.extendedSoundModel) {
            Log.v(TAG, "getExtendedSoundModel: must call setExtendedSoundModel before get");
        }
        return this.extendedSoundModel;
    }

    // Saves the selected sound model to a file on the internal storage
    public void saveExtendedSoundModel(Context context) {
        Log.v(TAG, "saveExtendedSoundModel");

        String filePath;
        boolean isComTraining = false;
        if (this.tempTrainingComKeyphrase != null) {
            isComTraining = true;
            // Use commKeyphrase to create or extend SM.
            filePath = generateComSoundModelFilePath(this.tempTrainingComKeyphrase.getName());
        } else {
            // Use dev sound model name to create or extend SM.
            filePath = generateSoundModelFilePath(soundModelNameToExtend);
        }
        Log.v(TAG, "saveExtendedSoundModel: filePath= " + filePath);
        Utils.saveByteBufferToFile(this.extendedSoundModel, filePath);
        if (isComTraining) {
            // If Com PDK, set UIMC as trained. Will never set the UDMC version with the same
            // keyphrase accidentally because it hasn't been added to the soundModels list yet.
            if (this.tempTrainingComKeyphrase.getIsUdk() == false) {
                // No need to save trained attr in underlying data model (SharedPrefs) because the
                // attr will be set when getting all sound models from the file system.

                SoundModel sm = getComSoundModelFromKeyphrase(
                        this.tempTrainingComKeyphrase.getName());
                if (null != sm) {
                    sm.setIsTrained();
                }

            }

            // Save newly-created SM to soundModels list.
            String soundModelName = generateComSoundModelNameFromFilePath(filePath);
            Log.v(TAG, "saveExtendedSoundModel for SmName= " + soundModelName + ", keyphraseName= "
                    + this.tempTrainingComKeyphrase.getName() + ", keyphraseAction= " +
                    this.tempTrainingComKeyphrase.getActionName() + ", keyphraseActionIntent= " +
                    this.tempTrainingComKeyphrase.getActionIntent());
            SoundModel sm = new SoundModel(soundModelName);
            Keyphrase kp = new Keyphrase(this.tempTrainingComKeyphrase.getName(),
                    this.tempTrainingComKeyphrase.getActionName(),
                    this.tempTrainingComKeyphrase.getActionIntent(), true);
            sm.addKeyphrase(kp);
            soundModels.add(sm);

            // Save newly-created SM to SharedPrefs to persist SM data across boots.
            // Key: SmName
            // E.g. HeySnapdragon.uimc
            // Value: KeyphraseName|ActionName|ActionIntentUri|userVerificationEnabled
            // E.g. Hey Snapdragon|Open Camera|com.qualcomm.qti.camera|true
            saveCsmToSharedPrefs(context, soundModelName, this.tempTrainingComKeyphrase.getName(),
                    this.tempTrainingComKeyphrase.getActionName(),
                    this.tempTrainingComKeyphrase.getActionIntent(), true);

            // Reset tempTrainingComKeyphrase back to null.
            this.tempTrainingComKeyphrase = null;
        }
    }

    /*** DeleteData (delete user or keyword from sound model) ***/

    public boolean deleteSoundModel(String inSoundModelName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "deleteSoundModel: Sound model with name: " + inSoundModelName + " does not exist.");
            return false;
        }

        String filePath = generateSoundModelFilePath(inSoundModelName);
        Log.v(TAG, "deleteSoundModel: filePath= " + filePath);
        File soundModelFile = new File(filePath);
        if (soundModelFile.exists()) {
            if (soundModelFile.delete()) {
                Log.v(TAG, "deleteSoundModel: succeeded");
                return true;
            } else {
                Log.e(TAG, "deleteSoundModel: delete failed");
            }
        } else {
            Log.e(TAG, "deleteSoundModel: sound model does not exist");
        }
        return false;
    }

    public void setSoundModelNameToDeleteFrom(String inSoundModelName) {
        this.soundModelNameToDeleteFrom = inSoundModelName;
    }

    public ByteBuffer getSoundModelByteBufferToDeleteFrom() {
        return getSoundModelByteBufferFromName(this.soundModelNameToDeleteFrom);
    }

    public void createSoundModelToModifyByteBuffer(int size) {
        this.soundModelToModifyByteBuffer = ByteBuffer.allocate(size);
    }

    public ByteBuffer getSoundModelToModifyByteBuffer() {
        if (null == this.soundModelToModifyByteBuffer) {
            Log.v(TAG, "getExtendedSoundModel: must call setExtendedSoundModel before get");
        }
        return this.soundModelToModifyByteBuffer;
    }

    public void saveSoundModelToDeleteFrom() {
        Log.v(TAG, "saveSoundModelToDeleteFrom");
        String filePath = generateSoundModelFilePath(this.soundModelNameToDeleteFrom);
        Log.v(TAG, "outputSoundModelToDeleteFrom: filePath= " + filePath);
        Utils.saveByteBufferToFile(this.soundModelToModifyByteBuffer, filePath);
    }


    /*** Merge Functions ***/

    public void setMergedSoundModelName(String inSoundModelName) {
        this.mergedSoundModelName = inSoundModelName;
    }

    public String getMergedSoundModelName() {
        return this.mergedSoundModelName;
    }

    public void saveMergedSoundModel() {
        Log.v(TAG, "saveMergedSoundModel");
        String filePath = generateSoundModelFilePath(this.mergedSoundModelName);
        Log.v(TAG, "saveMergedSoundModel: filePath= " + filePath);
        Utils.saveByteBufferToFile(this.soundModelToModifyByteBuffer, filePath);
    }


    /*** Utility Functions ***/

    // returns null if no SM with specified name exists
    public ByteBuffer getSoundModelByteBufferFromName(String inSoundModelName) {
        if (soundModelFileExists(inSoundModelName)) {
            try {
                ByteBuffer soundModel = Utils.readFileToByteBuffer(
                        generateSoundModelFilePath(inSoundModelName));
                return soundModel;
            } catch (IOException e) {
                Log.e(TAG, "getKeywordOnlySoundModel: failed to read sound model: " + inSoundModelName);
                e.printStackTrace();
                return null;
            }
        } else {
            Log.v(TAG, "getSoundModelFromName: sound model with name: " + inSoundModelName +
                    " does not exist");
            return null;
        }
    }

    // prints all SMs and all info in them for debugging purposes
    private String printSms() {
        StringBuilder smsToText = new StringBuilder();
        smsToText.append("\n");
        for (SoundModel soundModel : soundModels) {
            smsToText.append("\t" + soundModel.getName() + "\n");
            for (Keyphrase keyphrase : soundModel.getKeyphrases()) {
                if (keyphrase == null) {
                    continue;
                }

                smsToText.append("\t\t" + keyphrase.getName() + "\n");
                for (User user : keyphrase.getUsers()) {
                    smsToText.append("\t\t\t" + user.getName() + "\n");
                }
            }
        }
        return smsToText.toString();
    }

    /*** Sound Trigger Functions ***/
    public String findKeyphraseOrUserById(String soundModelName, int keyphraseOrUserId,
                                          boolean bFindByUserID) {
        SoundModel soundModel = getSmByName(soundModelName);
        if (null == soundModel || null == soundModel.getKeyphrases()) {
            Log.e(TAG, "findKeyphraseOrUserById: Could not find soundModel with name= " +
                    soundModelName);
            return null;
        }

        for (Keyphrase keyphrase : soundModel.getKeyphrases()) {
            if (keyphrase == null) {
                continue;
            }

            if (!bFindByUserID) {
                //by keyphraseID
                if (-3 == keyphrase.id || keyphraseOrUserId == keyphrase.id) {
                    Log.d(TAG, "findKeyphraseOrUserById: keyphrase.name = " + keyphrase.name);
                    return keyphrase.name;
                }
            } else {
                //by userID
                if (-3 == keyphrase.id || keyphraseOrUserId == keyphrase.id) {
                    Log.e(TAG, "findKeyphraseOrUserById: keyphrase.id = " + keyphrase.id
                            + "keyphraseOrUserId = " + keyphraseOrUserId);
                    return keyphrase.name;
                }

                for (User user : keyphrase.getUsers()) {
                    if (keyphraseOrUserId == user.id) {
                        return user.name;
                    }
                }
            }
        }

        Log.e(TAG, "findKeyphraseOrUserById: Could not find keyphrase or user with id= " +
                keyphraseOrUserId);

        return null;
    }


    /*** Commercial App functions ***/


    public LinkedHashSet<Keyphrase> getAllComKeyphrases(Context context) {
        Log.v(TAG, "getAllComKeyphrases");

        //TODO CSVAA this function
        // If SMs were previously populated (either by sharedPrefs or the filesystem),
        // return the list since all modifications happen to the list.
        /*if (false == soundModels.isEmpty()) {
            return soundModels;
        }*/

        // Get SMs from the filesystem.
        File dir = new File(Global.PATH_APP);
        Log.v(TAG, "getAllComKeyphrases: dir created.");
        File[] files = dir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                if (filename.endsWith(FILEEXT_SOUNDMODEL_COM_KEYPHRASE) ||
                        filename.endsWith(FILEEXT_SOUNDMODEL_COM_USER)) {
                    Log.v(TAG, "getAllComKeyphrases: found keyphrase= " + filename);
                    return true;
                }
                return false;
            }
        });
        if (null == files) {
            Log.e(TAG, "getAllComKeyphrases: no sound models found");
            return null;
        }

        ArrayList<String> pdkSmFileNames = new ArrayList<String>();
        for (File file : files) {
            String soundModelFileName = file.getName();

            if (isUserIndependentPdkCsm(soundModelFileName)) {
                pdkSmFileNames.add(soundModelFileName);
            }

            if (soundModels.contains(new SoundModel(soundModelFileName))) {
                Log.v(TAG, "getAllComKeyphrases: duplicate sound model name: " + soundModelFileName +
                        " skipping adding SM and continuing to next SM");
                continue;
            }

            Log.v(TAG, "getAllComKeyphrases: adding soundModelName= " + soundModelFileName);
            SoundModel sm = new SoundModel(soundModelFileName);
            Keyphrase keyphrase = getKeyphraseFromSharedPrefs(context, soundModelFileName);
            if (keyphrase != null) {
                sm.addKeyphrase(keyphrase);
            }
            soundModels.add(sm);
        }

        // Remove UDMC (trained PDK SMs) from sorted list (to set the UIMC PDK to trained later).
        ArrayList<String> smNamesToSetTrained = new ArrayList<String>();
        for (String smName : pdkSmFileNames) {
            String nameToMatch = smName.substring(0, smName.lastIndexOf('.')) +
                    FILEEXT_SOUNDMODEL_COM_USER;
            Log.v(TAG, "getAllComKeyphrases: nameToRemove= " + nameToMatch);
            for (SoundModel sm : soundModels) {
                if (sm.getName().equals(nameToMatch)) {
                    // Remove UDMC SMs with a corresponding UIMC SM to maintain one SM per keyphrase.
                    soundModels.remove(sm);
                    Log.v(TAG, "getAllComKeyphrases: removed sm.getName()= " + sm.getName());

                    // Found a trained UIMC. Add to list to later set trained.
                    smNamesToSetTrained.add(smName);
                    break;
                }
            }
        }

        Collections.sort(soundModels);

        // Use a set to remove duplicates.
        LinkedHashSet<Keyphrase> keyphrases = new LinkedHashSet<Keyphrase>();
        for (SoundModel sm : soundModels) {
            //Log.v(TAG, "getAllComKeyphrases:keyphraseName=" + sm.getKeyphrases().get(0).getName()
            //      + ", keyphraseActionName= " + sm.getKeyphrases().get(0).getActionName());

            // Set the UIMC PDK to trained.
            if (smNamesToSetTrained.contains(sm.getName())) {
                sm.setIsTrained();
            }

            // Add keyphrases for all SMs in list.
            if (sm.getKeyphrases() != null && sm.getKeyphrases().size() > 0) {
                keyphrases.add(sm.getKeyphrases().get(0));
            }
        }

        return keyphrases;
    }

    private Keyphrase getKeyphraseFromSharedPrefs(Context context, String soundModelName) {
        SharedPreferences sharedPrefs = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        String keyphraseString = sharedPrefs.getString(soundModelName, null);
        // Check for pre-loaded SMs that haven't been saved before.
        if (keyphraseString == null && soundModelName.equals(heySnapdragonUimcName)) {
            // First time opening the app on this device, so no sharedPrefs.
            // Save pre-loaded SM.
            saveCsmToSharedPrefs(context, soundModelName, heySnapdragonKeyphraseName,
                    "None", null, false);
            keyphraseString = sharedPrefs.getString(soundModelName, null);
        } else if (keyphraseString == null && soundModelName.equals(niHaoXiaoLongUimcName)) {
            // First time opening the app on this device, so no sharedPrefs.
            // Save pre-loaded SM.
            saveCsmToSharedPrefs(context, soundModelName, niHaoXiaoLongKeyphraseName,
                    "None", null, false);
            keyphraseString = sharedPrefs.getString(soundModelName, null);
        }
        // Check if the keyphrase string is null after possibly populating it if it was a pre-loaded SM.
        if (keyphraseString == null) {
            Log.e(TAG, "getKeyphraseFromSharedPrefs: unknown SM with name= " + soundModelName);
            return null;
        }
        Log.v(TAG, "getKeyphraseFromSharedPrefs: keyphraseString= " + keyphraseString);
        String[] keyphraseStrings = keyphraseString.split("\\|");
        Log.v(TAG, "getKeyphraseFromSharedPrefs: keyphraseStrings= "
                + Arrays.toString(keyphraseStrings));

        String keyphraseName = keyphraseStrings[0];

        // Create intent from keyphrase string.
        Intent keyphraseIntent = null;
        try {
            keyphraseIntent = Intent.parseUri(keyphraseStrings[2], 0);
        } catch (URISyntaxException e) {
            Log.e(TAG, "getKeyphraseFromSharedPrefs: Error parsing intent URI");
            e.printStackTrace();
        }
        // Determine userVerificationEnabled from keyphrase string.
        boolean userVerificationEnabled = keyphraseStrings[3].equals("true") ? true : false;
        Keyphrase keyphrase = new Keyphrase(keyphraseName, keyphraseStrings[1],
                keyphraseIntent, userVerificationEnabled);
        Log.v(TAG, "getKeyphraseFromSharedPrefs: created keyphraseName= " + keyphrase.getName() +
                ", keyphraseAction=" + keyphrase.getActionName() + ", keyphraseActionIntent= " +
                keyphrase.getActionIntent() + ", userVerificationEnabled= " +
                keyphrase.getIsUserVerificationEnabled());
        return keyphrase;
    }

    private void saveCsmToSharedPrefs(Context context, String smName, String keyphraseName,
                                      String actionName, Intent actionIntent, boolean isUserVerificationEnabled) {
        String intentStringToSave;
        if (actionIntent == null) {
            intentStringToSave = null;
        } else {
            intentStringToSave = actionIntent.toUri(0);
        }
        String value = keyphraseName + TAG_DELIMITER + actionName + TAG_DELIMITER +
                intentStringToSave + TAG_DELIMITER + isUserVerificationEnabled;
        Log.v(TAG, "saveCsmToSharedPrefs: saving key= " + smName + ", value= " + value);
        SharedPreferences sharedPrefs =
                context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        Editor editor = sharedPrefs.edit();
        editor.putString(smName, value);
        editor.commit();
    }

    public void setKeyphraseAction(Context context, String keyphraseName, String actionName,
                                   Intent actionIntent) {
        Log.v(TAG, "setKeyphraseAction: keyphraseName= " + keyphraseName + ", actionName= " +
                actionName + ", actionIntent= " + actionIntent);
        for (SoundModel sm : soundModels) {
            if (sm.getKeyphrases() == null || sm.getKeyphrases().size() == 0) {
                continue;
            }

            if (sm.getKeyphrases().get(0).getName().equals(keyphraseName)) {
                Log.v(TAG, "setKeyphraseAction: sm.getKeyphrases().get(0).getName()= " +
                        sm.getKeyphrases().get(0).getName());
                // Save new actionName and actionIntent to soundModels list.
                sm.getKeyphrases().get(0).setActionName(actionName);
                sm.getKeyphrases().get(0).setActionIntent(actionIntent);
                // Save to shared prefs
                Keyphrase tempKeyphrase = sm.getKeyphrases().get(0);
                saveCsmToSharedPrefs(context, sm.getName(), tempKeyphrase.getName(),
                        tempKeyphrase.getActionName(), tempKeyphrase.getActionIntent(),
                        tempKeyphrase.getIsUserVerificationEnabled());
                return;
            }
        }
    }

    // Get actionName provided a keyphraseName. For now, actionName is the name of the app to open
    // (e.g. Calculator).
    // @return null if keyphrase provided isn't found.
    public String getKeyphraseActionName(String keyphraseName) {
        Log.v(TAG, "getKeyphraseActionName: keyphraseName= " + keyphraseName);
        for (SoundModel sm : soundModels) {
            if (sm.getKeyphrases() == null || sm.getKeyphrases().size() == 0) {
                continue;
            }

            Log.v(TAG, "getKeyphraseActionName: sm.getKeyphrases().get(0).getName()= " +
                    sm.getKeyphrases().get(0).getName());
            if (sm.getKeyphrases().get(0).getName().equals(keyphraseName)) {
                String actionName = sm.getKeyphrases().get(0).getActionName();
                Log.v(TAG, "getKeyphraseActionName: for keyphrase= " + keyphraseName + " found " +
                        "actionName= " + actionName);
                return actionName;
            }
        }
        return null;
    }

    // Get actionIntent provided a keyphraseName.
    // @return null if keyphrase provided isn't found.
    public Intent getKeyphraseActionIntent(Context context, String keyphraseName) {
        Log.v(TAG, "getKeyphraseActionIntent: keyphraseName= " + keyphraseName);
        if (keyphraseName == null) {
            return null;
        }

        for (SoundModel sm : soundModels) {
            for (Keyphrase keyphrase : sm.getKeyphrases()) {
                if (keyphrase == null) {
                    continue;
                }
                Log.v(TAG, "getKeyphraseActionIntent: actionName= " + keyphrase.getActionName());

                if (keyphraseName.equals(keyphrase.getName()) && (keyphrase.getActionName() != null
                        && !"None".equals(keyphrase.getActionName()))) {
                    Intent actionIntent = keyphrase.getActionIntent();
                    Log.v(TAG, "getKeyphraseActionIntent: for keyphrase= " + keyphraseName +
                            " found " + "actionIntent= " + actionIntent);
                    return actionIntent;
                }
            }
        }
        return null;
    }

    // Returns whether user verification is enabled for a CSM.
    // @return null for error
    public Boolean getIsUserVerificationEnabledForCsm(String smNameToMatch) {
        /*for (SoundModel sm : soundModels) {
            if (sm.getName().equals(smNameToMatch)) {
                return sm.getKeyphrases().get(0).getIsUserVerificationEnabled();
            }
        }

        // Didn't find the CSM passed in. Return error.
        return null;*/
        return !(isUserIndependentPdkCsm(smNameToMatch));
    }

    // Returns whether user verification is enabled for a CSM.
    // @return Boolean
    public Boolean getIsUserVerificationEnabledForKeyphrase(String keyphraseToMatch) {
        Boolean enabled = false;
        SoundModel sm = getComSoundModelFromKeyphrase(keyphraseToMatch);

        if (null != sm && sm.getKeyphrases() != null && sm.getKeyphrases().size() > 0) {
            enabled = sm.getKeyphrases().get(0).getIsUserVerificationEnabled();
        }

        return enabled;
    }

    // Returns true if the SM is a Commercial SM, false otherwise.
    public boolean isCSM(String smNameToMatch) {
        String smExtn = smNameToMatch.substring(smNameToMatch.lastIndexOf('.'));
        if (smExtn.equals(FILEEXT_SOUNDMODEL_COM_KEYPHRASE) ||
                smExtn.equals(FILEEXT_SOUNDMODEL_COM_USER)) {
            return true;
        }

        // Not a CSM
        return false;
    }

    // Returns true if the SM is a user-independent PDK CSM, false otherwise.
    public boolean isUserIndependentPdkCsm(String smNameToMatch) {
        String smExtn = smNameToMatch.substring(smNameToMatch.lastIndexOf('.'));
        return smExtn.equals(FILEEXT_SOUNDMODEL_COM_KEYPHRASE) ? true : false;
    }

    // Returns true for Com UDK and false for Com PDK.
    public Boolean isComUdk(String inKeyphraseName) {
        String smNameToCheck = getComSoundModelNameFromKeyphrase(inKeyphraseName);

        if (null == smNameToCheck) {
            Log.e(TAG, "isComUdk: getComSoundModelNameFromKeyphrase cannot find SM for keyphrase= "
                    + inKeyphraseName);
            return null;
        }

        return smNameToCheck.substring(smNameToCheck.lastIndexOf('.')).
                equals(FILEEXT_SOUNDMODEL_COM_USER);
    }

    // Returns the CSM that contains the keyphrase passed in.
    // If the keyphrase has user verification enabled, return the SM with the keyphrase trained
    // (the udmc).
    // @return null for error
    public SoundModel getComSoundModelFromKeyphrase(String keyphraseNameToMatch) {
        for (SoundModel sm : soundModels) {
            if (sm.getKeyphrases() == null || sm.getKeyphrases().size() == 0) {
                continue;
            }

            if (sm.getKeyphrases().get(0).getName().equals(keyphraseNameToMatch)) {
                return sm;
            }
        }

        // Didn't find the keyphrase in any of the CSMs.
        return null;
    }

    // Requires: a UDMC sm name.
    private String generateUdmcNameFromUimc(String uimcName) {
        int fileExtIndex = uimcName.lastIndexOf(FILEEXT_SOUNDMODEL_COM_KEYPHRASE);
        return uimcName.substring(0, fileExtIndex) + FILEEXT_SOUNDMODEL_COM_USER;
    }

    // Returns the name of the CSM that contains the keyphrase passed in.
    // @return null for error
    public String getComSoundModelNameFromKeyphrase(String keyphraseNameToMatch) {
        SoundModel sm = getComSoundModelFromKeyphrase(keyphraseNameToMatch);

        if (null == sm) {
            // Didn't find the keyphrase in any of the CSMs.
            return null;
        }

        return sm.getName();
    }

    private String generateComSoundModelFilePath(String inComKeyphraseToCreateSm) {
        String soundModelName = inComKeyphraseToCreateSm.replaceAll("\\s", "");
        //TODO for loop to check if sMName already exists on filesystem.
        String filePath = Global.PATH_APP + "/" + soundModelName + FILEEXT_SOUNDMODEL_COM_USER;
        Log.v(TAG, "generateSoundModelFilePath: filePath= " + filePath);
        return filePath;
    }

    private String generateComSmFilePathFromSmName(String inComSmName) {
        String filePath = Global.PATH_APP + "/" + inComSmName;
        Log.v(TAG, "generateSoundModelFilePath: filePath= " + filePath);
        return filePath;
    }

    private String generateComSoundModelNameFromFilePath(String inFilePath) {
        String fileExtension = null;
        if (this.tempTrainingComKeyphrase.getIsUdk()) {
            fileExtension = FILEEXT_SOUNDMODEL_COM_USER;
        } else {
            fileExtension = FILEEXT_SOUNDMODEL_COM_KEYPHRASE;
        }
        String soundModelName = inFilePath.replaceAll(Global.PATH_APP + "/", "");
        Log.v(TAG, "generateComSoundModelNameFromFilePath: soundModelName= " + soundModelName);
        return soundModelName;
    }

    // Restriction: is only called when a UDMC is not yet created for a particular UIMC.
    public void setComSoundModelNameToExtend(String inKeyphraseName) {
        String smNameToExtend = getComSoundModelNameFromKeyphrase(inKeyphraseName);
        this.soundModelNameToExtend = smNameToExtend;
    }

    public void setTempTrainingComKeyphrase(String keyphraseName, String keyphraseAction,
                                            Intent keyphraseActionIntent, boolean inIsUdk) {
        if (inIsUdk == false) {
            // If training a PDK SM, set action and action intent from existing PDK SM.
            SoundModel sm = getComSoundModelFromKeyphrase(keyphraseName);
            if (null != sm && sm.getKeyphrases() != null && sm.getKeyphrases().size() > 0) {
                keyphraseAction = sm.getKeyphrases().get(0).getActionName();
                keyphraseActionIntent = sm.getKeyphrases().get(0).getActionIntent();
            }
        }
        this.tempTrainingComKeyphrase = new Keyphrase(keyphraseName, keyphraseAction,
                keyphraseActionIntent, true); // userVerification true because training.
        this.tempTrainingComKeyphrase.setIsUdk(inIsUdk);
    }

    public void deleteComKeyphrase(String inKeyphraseName) {
        Log.v(TAG, "deleteComKeyphrase: delete SM with keyphrase= " + inKeyphraseName);

        SoundModel smToDelete = getComSoundModelFromKeyphrase(inKeyphraseName);
        if (null == smToDelete) {
            Log.e(TAG, "deleteComKeyphrase: getComSoundModelNameFromKeyphrase cannot find " +
                    "SM for keyphrase= " + inKeyphraseName);
            return;
        }

        // Delete it on the file system.
        String filePath = generateComSmFilePathFromSmName(smToDelete.getName());
        Utils.deleteFile(filePath);

        // Delete it in the SM list.
        Log.v(TAG, "deleteComKeyphrase: num SMs before delete= " + this.soundModels.size());
        this.soundModels.remove(smToDelete);
        Log.v(TAG, "deleteComKeyphrase: num SMs after delete= " + this.soundModels.size());
        return;
    }

    public boolean getPdkSmIsTrained(String keyphraseName) {
        boolean isTrained = false;

        SoundModel sm = getComSoundModelFromKeyphrase(keyphraseName);
        if (null != sm) {
            isTrained = sm.getIsTrained();
        }

        return isTrained;
    }

    public void setIsUserVerificationEnabled(Context context, String keyphraseName,
                                             boolean isUserVerificationEnabled) {

        SoundModel sm = getComSoundModelFromKeyphrase(keyphraseName);
        if (null == sm || sm.getKeyphrases() == null || sm.getKeyphrases().size() == 0) {
            return;
        }

        // Save IsUserVerificationEnabled for keyphrase in soundModels list.
        sm.getKeyphrases().get(0).setIsUserVerificationEnabled(isUserVerificationEnabled);

        // Save IsUserVerificationEnabled changes in SharedPrefs.
        Keyphrase tempKeyphrase = sm.getKeyphrases().get(0);
        saveCsmToSharedPrefs(context, sm.getName(), keyphraseName, tempKeyphrase.getActionName(),
                tempKeyphrase.getActionIntent(), tempKeyphrase.getIsUserVerificationEnabled());

        Log.v(TAG, "setIsUserVerificationEnabled: for keyphraseName= " + keyphraseName + ", set" +
                " enabled= " + tempKeyphrase.getIsUserVerificationEnabled());
    }

    // Converts UIMC names to UDMC names and leaves UDMC names unchanged.
    public String getUdmcName(String smName) {
        int fileExtIndex = smName.lastIndexOf(".");
        return smName.substring(0, fileExtIndex) + FILEEXT_SOUNDMODEL_COM_USER;
    }

    // Returns the UIMC or UDMC of the SM depending on if user verification is enabled.
    public String getComSoundModelNameToSendFromKeyphrase(String inKeyphraseName) {
        SoundModel sm = getComSoundModelFromKeyphrase(inKeyphraseName);

        if (null == sm) {
            return null;
        }

        if (sm.getKeyphrases() != null && sm.getKeyphrases().size() > 0
                && sm.getKeyphrases().get(0).getIsUserVerificationEnabled()) {
            return getUdmcName(sm.getName());
        }
        return sm.getName();
    }

    public void saveSelectedKeyphrases(Context context, ArrayList<String> checkedKeyphrases) {
        SharedPreferences sharedPrefs =
                context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        Editor editor = sharedPrefs.edit();
        StringBuilder stringBuilder = new StringBuilder();
        for (String keyphraseName : checkedKeyphrases) {
            if (stringBuilder.length() > 0) {
                stringBuilder.append("|");
            }
            stringBuilder.append(keyphraseName);
        }
        Log.v(TAG, "saveSelectedKeyphrases: saving keyphrasesString= " + stringBuilder.toString());
        editor.putString(TAG_PREFERENCE_SELECTEDKEYPHRASES, stringBuilder.toString());
        editor.commit();
    }

    public ArrayList<String> getSelectedKeyphrases(Context context) {
        SharedPreferences sharedPrefs =
                context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        String savedKeyphrasesValue = sharedPrefs.getString(TAG_PREFERENCE_SELECTEDKEYPHRASES, null);
        if (savedKeyphrasesValue == null) {
            return null;
        }
        Log.v(TAG, "getSelectedKeyphrases: keyphrasesString= " + savedKeyphrasesValue);
        String[] selectedKeyphrases = savedKeyphrasesValue.split("\\|");
        return new ArrayList(Arrays.asList(selectedKeyphrases));
    }
}
