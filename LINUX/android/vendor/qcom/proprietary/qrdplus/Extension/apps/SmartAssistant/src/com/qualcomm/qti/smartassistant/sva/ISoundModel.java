/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.sva;

import com.qualcomm.listen.ListenTypes;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

public interface ISoundModel {
    public ListenTypes.SoundModelInfo querySoundModelInfo(String smName);

    public int getSizeofExtendedPdkModel(ByteBuffer soundModel,
                                         String keywordPhrase, String userName);

    public int verifyUserRecording(ByteBuffer soundModel,
                                   String keywordPhrase, ShortBuffer userRecording);

    public int createPdkUserModel(ByteBuffer soundModel, String keywordPhrase,
                                  String userName, int numUserRecordings,
                                  ShortBuffer userRecordings[], ByteBuffer extendedSoundModel,
                                  ListenTypes.ConfidenceData confidenceData);

    public int getSizeAfterDelete(ByteBuffer soundModel, String keywordPhrase, String userName);

    public int deleteUserData(ByteBuffer soundModel, String keywordPhrase,
                              String userName, ByteBuffer outputSoundModel);
}
