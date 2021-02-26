/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.utils;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;
import android.util.Xml;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.List;

public class FileUtils {
    public final static String APP_NAME = "SmartAssistant";
    public final static String APP_PATH
            = Environment.getExternalStorageDirectory().getAbsolutePath()
            + "/" + APP_NAME;
    public final static String LOOK_AHEAD_VOICE_COMMANDS_PATH = APP_PATH +
            "/voiceCommands/" + "lookAhead";
    public final static String NORMAL_VOICE_COMMANDS_PATH = APP_PATH +
            "/voiceCommands/" + "normal";
    public final static String PATH_RECORDINGS_FILE_EXT = ".wav";
    public final static String UIM_SM_FILE_EXT_NAME = ".uim";
    public final static String UDM_SM_FILE_EXT_NAME = ".udm";
    public final static int DEFAULT_USER_CONFIDENCE_LEVEL = 69;
    public final static int DEFAULT_KEYPHRASE_CONFIDENCE_LEVEL = 69;
    public final static int DEFAULT_TRAINING_CONFIDENCE_LEVEL = 72;
    public final static int DEFAULT_ONE_SHOT_BUFFER_TIME = 8000;
    public final static boolean DEFAULT_IS_SAVE_PCM_DATA = false;

    //one-shot
    public static final short NUM_CHANNELS = 1; // mono
    public static final int BITS_PER_SAMPLE = 16; // in bits
    public static final int SAMPLE_RATE = 16000; // 16kHz

    // config file define
    public final static String CONFIG_FILE_NAME = "config.xml";
    public final static String CONFIG_FILE_PATH = APP_PATH + "/" + CONFIG_FILE_NAME;
    public final static String CONFIG_ITEM_TAG = "configItem";
    public final static String CONFIG_ITEM_ID = "id";
    public final static String CONFIG_ITEM_NAME = "name";
    public final static String CONFIG_ITEM_VALUE = "value";

    public final static int CONFIG_ITEM_KEYPHRASE_CONFIDENCE_LEVEL_ID = 1;
    public final static int CONFIG_ITEM_USER_CONFIDENCE_LEVEL_ID = 2;
    public final static int CONFIG_ITEM_TRAINING_CONFIDENCE_LEVEL_ID = 3;
    public final static int CONFIG_ITEM_ONE_SHOT_BUFFER_TIME_ID = 4;
    public final static int CONFIG_ITEM_SAVE_PCM_DATA_ID = 5;

    public static void writeBufferToWavFile(byte[] inBuff, int bufferSize, String filePath,
                                            boolean isAppendToFile) {

        DataOutputStream doStream = null;
        try {
            LogUtils.v("writeBufferToWavFile: stream created bufferSize " + bufferSize);
            doStream = new DataOutputStream(
                    new BufferedOutputStream(new FileOutputStream(filePath, isAppendToFile)));
            int audioDataLength = bufferSize;
            byte[] wavHeader = getWavHeader(audioDataLength, audioDataLength+44);
            // write header
            LogUtils.v("writeBufferToWavFile: write header");
            doStream.write(wavHeader);
            // write audio samples
            LogUtils.v("writeBufferToWavFile: write " + (audioDataLength<1) + " samples");
            doStream.write(inBuff, 0, audioDataLength);
            LogUtils.v("writeBufferToWavFile: complete");
        } catch (FileNotFoundException e) {
            LogUtils.e("writeShortBufferToWavFile: FileNotFound: " + e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                doStream.close();
                LogUtils.v("writeBufferToWavFile: stream close");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static byte[] getWavHeader(long audioDataLength, long audioDataAndHeaderLength)
            throws IOException {
        long blocksPerSecond = SAMPLE_RATE;
        long dataRate = BITS_PER_SAMPLE * SAMPLE_RATE * NUM_CHANNELS/8;

        byte[] wavHeader = new byte[44];
        wavHeader[0] = 'R';  // chunk ID = "RIFF"
        wavHeader[1] = 'I';
        wavHeader[2] = 'F';
        wavHeader[3] = 'F';
        wavHeader[4] = (byte) (audioDataAndHeaderLength & 0xff); // chunk size
        wavHeader[5] = (byte) ((audioDataAndHeaderLength >> 8) & 0xff);
        wavHeader[6] = (byte) ((audioDataAndHeaderLength >> 16) & 0xff);
        wavHeader[7] = (byte) ((audioDataAndHeaderLength >> 24) & 0xff);
        wavHeader[8] = 'W'; // wave ID = "WAVE"
        wavHeader[9] = 'A';
        wavHeader[10] = 'V';
        wavHeader[11] = 'E';
        wavHeader[12] = 'f'; // chunk ID = "fmt "
        wavHeader[13] = 'm';
        wavHeader[14] = 't';
        wavHeader[15] = ' ';
        wavHeader[16] = 16;  // chunk size = 16
        wavHeader[17] = 0;
        wavHeader[18] = 0;
        wavHeader[19] = 0;
        wavHeader[20] = 1; // format code (0x0001 is PCM)
        wavHeader[21] = 0;
        wavHeader[22] = (byte) NUM_CHANNELS;  // number of interleaved channels
        wavHeader[23] = 0;
        wavHeader[24] = (byte) (blocksPerSecond & 0xff); // Sampling rate (blocks/sec)
        wavHeader[25] = (byte) ((blocksPerSecond >> 8) & 0xff);
        wavHeader[26] = (byte) ((blocksPerSecond >> 16) & 0xff);
        wavHeader[27] = (byte) ((blocksPerSecond >> 24) & 0xff);
        wavHeader[28] = (byte) (dataRate & 0xff); // Data rate
        wavHeader[29] = (byte) ((dataRate >> 8) & 0xff);
        wavHeader[30] = (byte) ((dataRate >> 16) & 0xff);
        wavHeader[31] = (byte) ((dataRate >> 24) & 0xff);
        wavHeader[32] = (byte) (NUM_CHANNELS * BITS_PER_SAMPLE / 8); // Data block size
        wavHeader[33] = 0;
        wavHeader[34] = BITS_PER_SAMPLE;
        wavHeader[35] = 0;
        wavHeader[36] = 'd'; // chunk ID = "data"
        wavHeader[37] = 'a';
        wavHeader[38] = 't';
        wavHeader[39] = 'a';
        wavHeader[40] = (byte) (audioDataLength & 0xff);
        wavHeader[41] = (byte) ((audioDataLength >> 8) & 0xff);
        wavHeader[42] = (byte) ((audioDataLength >> 16) & 0xff);
        wavHeader[43] = (byte) ((audioDataLength >> 24) & 0xff);

        return wavHeader;
    }

    public static void createDirIfNotExists(String directoryToCreate) {
        File directoryFile = new File(directoryToCreate);
        if (!directoryFile.isDirectory()) {
            directoryFile.mkdirs();
        }
    }

    public static final void copyAssetsToStorageIfNotExists(Context context,
                      String destAssetDirectory, String smName) {
        if (!isFileExist(getSMFilePath(smName))){
            copyAssetsToStorage(context, destAssetDirectory);
        }else {
            LogUtils.v("copyAssetsToStorageIfNotExists: yay! a SM was saved!: " +
                    getSMFilePath(smName));
        }
    }

    public static final String[] copyAssetsToStorage(Context context, String destAssetDirectory) {
        AssetManager assetManager = context.getAssets();
        String[] assetFiles = null;
        // Get asset files.
        try {
            assetFiles = assetManager.list("");
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (0 == assetFiles.length) {
            return null;
        }
        int index = 0;
        String[] copiedFilePaths = new String[assetFiles.length];
        //copy asset files
        try {
            for (String filename : assetFiles) {
                String outputFilePath = destAssetDirectory + "/" + filename;
                LogUtils.d("copyAssetsToStorage: filename = " + filename);
                if (new File(outputFilePath).exists()) {
                    LogUtils.v("copyAssetsToStorage: yay! a SM was saved!: " + outputFilePath);
                    continue;
                }
                if (!filename.endsWith(UIM_SM_FILE_EXT_NAME)){
                    LogUtils.d("copyAssetsToStorage: not the target file ");
                    continue;
                }
                InputStream in = assetManager.open(filename);
                OutputStream out = new FileOutputStream(outputFilePath);
                byte[] buffer = new byte[1024];
                int read;
                while((read = in.read(buffer)) != -1){
                    out.write(buffer, 0, read);
                }
                in.close();
                out.flush();
                out.close();

                copiedFilePaths[index++] = outputFilePath;
            }
            return copiedFilePaths;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    public static String getSMFilePath(String smName) {
        //input param check
        if (null == smName) {
            LogUtils.e("getSMFilePath: invalid input param");
            return null;
        }

        LogUtils.d("getSMFilePath: path = " + APP_PATH + "/" + smName);
        return APP_PATH + "/" + smName;
    }

    public static boolean isFileExist(String filePath) {
        if (null == filePath) {
            LogUtils.e("isFileExist: invalid input param");
            return false;
        }
        File targetFile = new File(filePath);
        return targetFile.exists();
    }

    public static ByteBuffer readFileToByteBuffer(String filePath) throws IOException {
        if (null == filePath) {
            LogUtils.e("readFileToByteBuffer: invalid input param");
            return null;
        }

        File file = new File(filePath);
        RandomAccessFile raf = new RandomAccessFile(file, "r");
        ByteBuffer byteBuffer;
        try {
            //do the force type conversion results check
            long longLength = raf.length();
            int intLength = (int) longLength;
            if (intLength != longLength) throw new IOException("File size >= 2 GB");

            byte[] data = new byte[intLength];
            raf.readFully(data);
            byteBuffer = ByteBuffer.allocateDirect(data.length);
            byteBuffer.put(data);
        } finally {
            raf.close();
        }

        return byteBuffer;
    }

    public static void saveByteBufferToFile(ByteBuffer buffer, String filePath) {
        FileChannel channel;
        try {
            channel = new FileOutputStream(filePath, false).getChannel();
            buffer.flip();
            channel.write(buffer);
            channel.close();
        } catch (FileNotFoundException e) {
            LogUtils.e("outputExtendedSoundModel: FileNotFound: " + e.getMessage());
        } catch (IOException e) {
            LogUtils.e("outputExtendedSoundModel: unable to write sound model: " + e.getMessage());
        }
    }

    public static ByteBuffer getSMData(String smName) {
        if (null == smName) {
            LogUtils.e("getSMData: invalid input param");
            return null;
        }

        String smFilePath = getSMFilePath(smName);
        if (isFileExist(smFilePath)) {
            try {
                ByteBuffer smData = readFileToByteBuffer(smFilePath);
                return smData;
            } catch (IOException e) {
                LogUtils.e("getSMData: read SM data fail");
                e.printStackTrace();
                return null;
            }
        } else {
            LogUtils.e("getSMData: smFilePath = " + smFilePath + " does not exist");
            return null;
        }
    }

    public static boolean getIsUdmSm(String smName) {
        if (null == smName) {
            LogUtils.e("getIsUimSm: invalid input param smName = " + smName);
            return false;
        }
        String extName = smName.substring(smName.lastIndexOf('.'));
        return extName.equalsIgnoreCase(UDM_SM_FILE_EXT_NAME) ? true : false;
    }

    public static List<ConfigItem> parseConfigFile(String filePath) {
        List<ConfigItem> configItemList = null;
        ConfigItem configItem = null;

        if (!isFileExist(filePath)) {
            LogUtils.e("parseConfigFile: config file not exist in path = "
                    + filePath);
            return null;
        }

        FileInputStream fileInputStream;
        try {
            fileInputStream = new FileInputStream(new File(filePath));
            XmlPullParser xmlPullParser = Xml.newPullParser();
            xmlPullParser.setInput(fileInputStream, "UTF-8");
            int eventType = xmlPullParser.getEventType();
            while (XmlPullParser.END_DOCUMENT != eventType) {
                switch (eventType) {
                    case XmlPullParser.START_DOCUMENT:
                        configItemList = new ArrayList<>();
                        break;
                    case XmlPullParser.START_TAG:
                        if (xmlPullParser.getName().equalsIgnoreCase(CONFIG_ITEM_TAG)) {
                            configItem = new ConfigItem();
                        } else if (xmlPullParser.getName().equalsIgnoreCase(CONFIG_ITEM_ID)) {
                            eventType = xmlPullParser.next();
                            configItem.setId(Integer.parseInt(xmlPullParser.getText()));
                        } else if (xmlPullParser.getName().equalsIgnoreCase(CONFIG_ITEM_NAME)) {
                            eventType = xmlPullParser.next();
                            configItem.setName(xmlPullParser.getText());
                        } else if (xmlPullParser.getName().equalsIgnoreCase(CONFIG_ITEM_VALUE)) {
                            eventType = xmlPullParser.next();
                            configItem.setValue(xmlPullParser.getText());
                        }
                        break;
                    case XmlPullParser.TEXT:
                        break;
                    case XmlPullParser.END_TAG:
                        if (xmlPullParser.getName().equalsIgnoreCase(CONFIG_ITEM_TAG)) {
                            configItemList.add(configItem);
                            LogUtils.d("parseConfigFile: configItem = " + configItem);
                            configItem = null;
                        }
                        break;
                }

                eventType = xmlPullParser.next();
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
            return null;
        } catch (XmlPullParserException e) {
            e.printStackTrace();
            return null;
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }

        return configItemList;
    }

    public static int getUserConfidenceLevelSetting() {
        int levelSetting;

        List<ConfigItem> configItemList = parseConfigFile(CONFIG_FILE_PATH);
        if (null == configItemList) {
            LogUtils.e("getUserConfidenceLevelSetting: fail when get config from "
                    + CONFIG_FILE_PATH);
            return DEFAULT_USER_CONFIDENCE_LEVEL;
        }

        for (ConfigItem item : configItemList) {
            if (CONFIG_ITEM_USER_CONFIDENCE_LEVEL_ID == item.getId()) {
                levelSetting = Integer.parseInt(item.getValue());
                LogUtils.d("getUserConfidenceLevelSetting: level value from "
                        + CONFIG_FILE_PATH + " is " + levelSetting);
                if (levelSetting <= 0 || levelSetting > 100) {
                    LogUtils.e("getUserConfidenceLevelSetting: config value is invalid");
                    return DEFAULT_USER_CONFIDENCE_LEVEL;
                }

                return levelSetting;
            }
        }

        return DEFAULT_USER_CONFIDENCE_LEVEL;
    }

    public static int getKeyphraseConfidenceLevelSetting() {
        int levelSetting;

        List<ConfigItem> configItemList = parseConfigFile(CONFIG_FILE_PATH);
        if (null == configItemList) {
            LogUtils.e("getKeyphraseConfidenceLevelSetting: fail when get config from "
                    + CONFIG_FILE_PATH);
            return DEFAULT_KEYPHRASE_CONFIDENCE_LEVEL;
        }

        for (ConfigItem item : configItemList) {
            if (CONFIG_ITEM_KEYPHRASE_CONFIDENCE_LEVEL_ID == item.getId()) {
                levelSetting = Integer.parseInt(item.getValue());
                LogUtils.d("getKeyphraseConfidenceLevelSetting: level value from "
                        + CONFIG_FILE_PATH + " is " + levelSetting);
                if (levelSetting <= 0 || levelSetting > 100) {
                    LogUtils.e("getKeyphraseConfidenceLevelSetting: value is invalid");
                    return DEFAULT_KEYPHRASE_CONFIDENCE_LEVEL;
                }

                return levelSetting;
            }
        }

        return DEFAULT_KEYPHRASE_CONFIDENCE_LEVEL;
    }

    public static int getTrainingConfidenceLevelSetting() {
        int levelSetting;

        List<ConfigItem> configItemList = parseConfigFile(CONFIG_FILE_PATH);
        if (null == configItemList) {
            LogUtils.e("getTrainingConfidenceLevelSetting: fail when get config from "
                    + CONFIG_FILE_PATH);
            return DEFAULT_TRAINING_CONFIDENCE_LEVEL;
        }

        for (ConfigItem item : configItemList) {
            if (CONFIG_ITEM_TRAINING_CONFIDENCE_LEVEL_ID == item.getId()) {
                levelSetting = Integer.parseInt(item.getValue());
                LogUtils.d("getTrainingConfidenceLevelSetting: level value from "
                        + CONFIG_FILE_PATH + " is " + levelSetting);
                if (levelSetting <= 0 || levelSetting > 100) {
                    LogUtils.e("getTrainingConfidenceLevelSetting: value is invalid");
                    return DEFAULT_TRAINING_CONFIDENCE_LEVEL;
                }

                return levelSetting;
            }
        }

        return DEFAULT_TRAINING_CONFIDENCE_LEVEL;
    }

    public static int getOneShotBufferTimeSetting() {
        int oneshotBufferTime;

        List<ConfigItem> configItemList = parseConfigFile(CONFIG_FILE_PATH);
        if (null == configItemList) {
            LogUtils.e("getOneShotBufferTimeSetting: fail when get config from "
                    + CONFIG_FILE_PATH);
            return DEFAULT_ONE_SHOT_BUFFER_TIME;
        }

        for (ConfigItem item : configItemList) {
            if (CONFIG_ITEM_ONE_SHOT_BUFFER_TIME_ID == item.getId()) {
                oneshotBufferTime = Integer.parseInt(item.getValue());
                LogUtils.d("getOneShotBufferTimeSetting: config value from "
                        + CONFIG_FILE_PATH + " is " + oneshotBufferTime);
                if (oneshotBufferTime <= 0) {
                    LogUtils.e("getOneShotBufferTimeSetting: value is invalid");
                    return DEFAULT_ONE_SHOT_BUFFER_TIME;
                }

                return oneshotBufferTime;
            }
        }

        return DEFAULT_ONE_SHOT_BUFFER_TIME;
    }

    public static boolean getIsSavePcmDataSetting() {
        List<ConfigItem> configItemList = parseConfigFile(CONFIG_FILE_PATH);
        if (null == configItemList) {
            LogUtils.e("getIsSavePcmDataSetting: fail when get config from "
                    + CONFIG_FILE_PATH);
            return DEFAULT_IS_SAVE_PCM_DATA;
        }

        for (ConfigItem item : configItemList) {
            if (CONFIG_ITEM_SAVE_PCM_DATA_ID == item.getId()) {
                int configValue = Integer.parseInt(item.getValue());
                LogUtils.d("getIsSavePcmDataSetting: config value from "
                        + CONFIG_FILE_PATH + " is " + configValue);
                if (configValue != 0 && configValue != 1) {
                    LogUtils.e("getIsSavePcmDataSetting: value is invalid");
                    return DEFAULT_IS_SAVE_PCM_DATA;
                }

                return 0 == configValue ? false : true;
            }
        }

        return DEFAULT_IS_SAVE_PCM_DATA;
    }

    public static ShortBuffer readWavFile(String filePath) throws IOException {
        File file = new File(filePath);
        RandomAccessFile raf = new RandomAccessFile(file, "r");
        int nHdrLen = 44; // RIFF Wav file header length
        try {
            // skip file's header
            raf.skipBytes(nHdrLen);
            long longlength = raf.length();
            int length = (int) longlength;
            if (length != longlength) throw new IOException("File size >= 2 GB");
            length = (int) (longlength - nHdrLen);
            int nShorts = (length + 1) / 2;
            short[] data = new short[nShorts];

            // read bytes as a short taking endianness into account
            short tmpShrt;
            for (int i = 0; i < nShorts; i++) {
                if (ByteOrder.LITTLE_ENDIAN == ByteOrder.nativeOrder()) {
                    tmpShrt = raf.readShort();
                    // flip bytes
                    data[i] = (short) ((tmpShrt & 0x00FF) << 8);
                    data[i] += ((tmpShrt & 0xFF00) >> 8);
                } else {
                    data[i] = raf.readShort();
                }
            }

            ShortBuffer sb = ShortBuffer.allocate(data.length);
            sb.put(data);
            return sb;
        } finally {
            raf.close();
        }
    }

    public static class ConfigItem {
        private int mId;
        private String mName;
        private String mValue;

        public int getId() {
            return mId;
        }

        public void setId(int id) {
            mId = id;
        }

        public String getName() {
            return mName;
        }

        public void setName(String name) {
            mName = name;
        }

        public String getValue() {
            return mValue;
        }

        public void setValue(String value) {
            mValue = value;
        }

        @Override
        public String toString() {
            return "ConfigItem{" +
                    "mId=" + mId +
                    ", mName='" + mName + '\'' +
                    ", mValue='" + mValue + '\'' +
                    '}';
        }
    }
}
