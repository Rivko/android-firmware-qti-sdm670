/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import com.qualcomm.listen.ListenTypes;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

public class SoundModelHelper {
    private ISoundModel mSoundModelImpl;

    public SoundModelHelper() {
        mSoundModelImpl = SoundModelImpl.getInstance();
    }

    public ListenTypes.SoundModelInfo querySoundModelInfo(String smName) {
        return mSoundModelImpl.querySoundModelInfo(smName);
    }

    public int getSizeofExtendedPdkModel(ByteBuffer soundModel,
                                         String keywordPhrase, String userName) {
        return mSoundModelImpl.getSizeofExtendedPdkModel(soundModel, keywordPhrase, userName);
    }

    public int verifyUserRecording(ByteBuffer soundModel,
                                   String keywordPhrase, ShortBuffer userRecording) {
        return mSoundModelImpl.verifyUserRecording(soundModel, keywordPhrase, userRecording);
    }

    public int createPdkUserModel(ByteBuffer soundModel, String keywordPhrase,
                                  String userName, int numUserRecordings,
                                  ShortBuffer userRecordings[], ByteBuffer extendedSoundModel,
                                  ListenTypes.ConfidenceData confidenceData) {
        return mSoundModelImpl.createPdkUserModel(soundModel, keywordPhrase,
                userName, numUserRecordings, userRecordings, extendedSoundModel, confidenceData);
    }

    public int getSizeAfterDelete(ByteBuffer soundModel, String keywordPhrase, String userName) {
        return mSoundModelImpl.getSizeAfterDelete(soundModel, keywordPhrase, userName);
    }

    public int deleteUserData(ByteBuffer soundModel, String keywordPhrase,
                              String userName, ByteBuffer outputSoundModel) {
        return mSoundModelImpl.deleteUserData(soundModel, keywordPhrase,
                userName, outputSoundModel);
    }
}
