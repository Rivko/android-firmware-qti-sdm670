/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import com.qualcomm.listen.ListenSoundModel;
import com.qualcomm.listen.ListenTypes;
import com.qualcomm.qti.smartassistant.utils.LogUtils;
import com.qualcomm.qti.smartassistant.utils.FileUtils;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

public class SoundModelImpl implements ISoundModel {
    @Override
    public ListenTypes.SoundModelInfo querySoundModelInfo(String smName) {
        String filePath = FileUtils.getSMFilePath(smName);
        if (!FileUtils.isFileExist(filePath)) {
            LogUtils.e("querySoundModelInfo: filePath = " + filePath + " not exist");
            return null;
        }

        ByteBuffer smBuffer = FileUtils.getSMData(smName);
        if (null == smBuffer) {
            LogUtils.e("querySoundModelInfo: getSMData fail");
            return null;
        }

        return ListenSoundModel.query(smBuffer);
    }

    @Override
    public int getSizeofExtendedPdkModel(ByteBuffer soundModel,
                                         String keywordPhrase,
                                         String userName) {
        return ListenSoundModel.getSizeWhenExtended(soundModel, keywordPhrase, userName);
    }

    @Override
    public int verifyUserRecording(ByteBuffer soundModel,
                                   String keywordPhrase,
                                   ShortBuffer userRecording) {
        return ListenSoundModel.verifyUserRecording(soundModel, keywordPhrase, userRecording);
    }

    @Override
    public int createPdkUserModel(ByteBuffer soundModel,
                                  String keywordPhrase,
                                  String userName,
                                  int numUserRecordings,
                                  ShortBuffer[] userRecordings,
                                  ByteBuffer extendedSoundModel,
                                  ListenTypes.ConfidenceData confidenceData) {
        return ListenSoundModel.extend(soundModel, keywordPhrase, userName, numUserRecordings,
                userRecordings, extendedSoundModel, confidenceData);
    }

    @Override
    public int getSizeAfterDelete(ByteBuffer soundModel,
                                  String keywordPhrase,
                                  String userName) {
        return ListenSoundModel.getSizeAfterDelete(soundModel, keywordPhrase, userName);
    }

    @Override
    public int deleteUserData(ByteBuffer soundModel,
                              String keywordPhrase,
                              String userName,
                              ByteBuffer outputSoundModel) {
        return ListenSoundModel.deleteData(soundModel, keywordPhrase,
                userName, outputSoundModel);
    }

    //implement SoundModelImpl as singleton
    private static final class SingletonHolder {
        private static final SoundModelImpl INSTANCE = new SoundModelImpl();

        private SingletonHolder() {

        }
    }

    private SoundModelImpl() {

    }

    public static SoundModelImpl getInstance() {
        return SingletonHolder.INSTANCE;
    }
}
