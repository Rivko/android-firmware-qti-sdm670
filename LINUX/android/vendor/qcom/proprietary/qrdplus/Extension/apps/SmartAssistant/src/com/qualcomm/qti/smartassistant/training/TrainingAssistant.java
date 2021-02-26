/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.training;

import com.qualcomm.qti.smartassistant.utils.FileUtils;
import com.qualcomm.qti.smartassistant.utils.LogUtils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ShortBuffer;
import java.util.Arrays;

public class TrainingAssistant {
    public final static String PATH_TRAINING_RECORDINGS = FileUtils.APP_PATH + "/" + "trainings";
    public final static String PATH_TRAINING_FILENAME = "training";
    public final static String PATH_RECORDINGS_TEMP_FILE = PATH_TRAINING_RECORDINGS + "/" + "temp_recording.wav";
    public final static String TAG_TRAINING_KEYPHRASE = "trainingKeyword";
    public final static String TAG_TRAINING_USER = "trainingUser";
    public static final String TAG_TRAINING_IS_UDK = "trainingIsUdk";
    public static final String DEFAULT_KEY_PHRASE = "XiaoduXiaodu";
    public final static int NUM_TRAINING_RECORDINGS_REQUIRED = 5;

    public final static int NUM_MAX_USERS = 3;

    private ShortBuffer[] userRecordings = new ShortBuffer[5];
    private int numUserRecordings = 0;

    private TrainingAssistant() {
    }

    private static class GlobalInstance {
        public static TrainingAssistant Instance = new TrainingAssistant();
    }

    public static TrainingAssistant getInstance() {
        return GlobalInstance.Instance;
    }


    public void removeExistingRecordingFiles() {
        for (int i = 0; i < NUM_TRAINING_RECORDINGS_REQUIRED; i++) {
            String recordingNumber = Integer.toString(1 + i);
            String filePath = PATH_TRAINING_RECORDINGS + "/" + PATH_TRAINING_FILENAME + recordingNumber +
                    FileUtils.PATH_RECORDINGS_FILE_EXT;
            File recordingFile = new File(filePath);
            if (recordingFile.exists()) {
                recordingFile.delete();
            }
        }
    }

    public void removeUserRecordings() {
        Arrays.fill(this.userRecordings, null);
        this.numUserRecordings = 0;
    }

    public int getNumUserRecordings() {
        return this.numUserRecordings;
    }


    // Returns a shortBuffer of the last user training recorded
    public void addUserRecording() {
        try {
            String filePath = getLastUserRecordingFilePath();
            this.userRecordings[this.numUserRecordings++] = FileUtils.readWavFile(filePath);
        } catch (FileNotFoundException f) {
            LogUtils.e("File not found");
        } catch (IOException e) {
            LogUtils.e("IO exeption");
        }
    }

    // Returns a filepath for the last user training recorded
    public String getLastUserRecordingFilePath() {
        String recordingNumber = Integer.toString(1 + this.numUserRecordings);
        String filePath = PATH_TRAINING_RECORDINGS + "/" + PATH_TRAINING_FILENAME + recordingNumber +
                FileUtils.PATH_RECORDINGS_FILE_EXT;
        return filePath;
    }

    // Returns a shortBuffer of the last user training recorded
    public ShortBuffer getLastUserRecording() {
        int num = getNumUserRecordings();
        if (num > NUM_TRAINING_RECORDINGS_REQUIRED) {
            num = NUM_TRAINING_RECORDINGS_REQUIRED;
        }

        if (num > 0) {
            num = num - 1;
        }
        return this.userRecordings[num];
    }

    // Removes the last recording trained by a user when it's unsuccessful
    public void discardLastUserRecording() {
        this.numUserRecordings--;
    }

    public ShortBuffer[] getUserRecordings() {
        return this.userRecordings;
    }

}
