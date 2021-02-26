/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.utils;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.List;
import java.util.Locale;

import com.qualcomm.qti.panoramaui.CameraPreviewView.OutputFrame;

import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.os.Environment;
import android.util.Log;
import android.util.Pair;

/**
 * This class provide image processing related static function.
 * It is mainly used for testing purpose.
 */
public class ImageUtils {
    private static final String TAG = ImageUtils.class.getSimpleName();
    private static final String DIR_NAME_PARENT = "pano_debug";
    private static final String DIR_NAME_DATA = DIR_NAME_PARENT + "/data";
    private static final String FILE_NAME_KEYFRAME = "keyframe_%d.jpg";
    private static final String FILE_NAME_KEYFRAME_POS = DIR_NAME_DATA + "/keyframe_position_log.txt";
    private static final String FILE_NAME_PREVIEWFRAME = "previewframe_%d_%d.jpg";
    private static final String FILE_NAME_PREVIEWFRAME_POS = DIR_NAME_DATA + "/previewframe_position_log.txt";
    private static final String FILE_NAME_APP_SETTINGS = DIR_NAME_DATA + "/app_settings_log.txt";
    private static final String GENERAL_TEXT_FILE_FORMAT= "%s, %s";
    private static final File DIR_DATA = 
            new File(Environment.getExternalStorageDirectory(), DIR_NAME_DATA);
    private static final File FILE_KEYFRAME_POS =
            new File(Environment.getExternalStorageDirectory(), FILE_NAME_KEYFRAME_POS);
    private static final File FILE_PREVIEWFRAME_POS =
            new File(Environment.getExternalStorageDirectory(), FILE_NAME_PREVIEWFRAME_POS);
    private static final File FILE_APP_SETTINGS =
            new File(Environment.getExternalStorageDirectory(), FILE_NAME_APP_SETTINGS);

    /**
     * Not constructable
     */
    private ImageUtils() {
    }

    /**
     * Clear any existing debug images from the debug output path
     */
    public static void cleanDebugImagePath() {
        if (DIR_DATA.exists()) {
            File[] files = DIR_DATA.listFiles();
            for (File file : files) {
                file.delete();
            }
        }
    }

    public static void renameKeyFrameCacheFolderName(String fileName) {
        File newFile = new File(Environment.getExternalStorageDirectory() + "/" + DIR_NAME_PARENT + "/" + fileName);
        if(DIR_DATA.renameTo(newFile) ) {
            Log.d(TAG, "Renamed: " + DIR_DATA.getAbsolutePath() + " => " + newFile.getAbsolutePath());
        } else {
            Log.d(TAG, "Failed renaming: " + DIR_DATA.getAbsolutePath() + " => " + newFile.getAbsolutePath());
        }
    }
    /**
     * Save the provided key frame to the debug output path
     * @param id Key frame index (starting at 0 for the first key frame)
     * @param jpegData JPEG-compressed key frame data
     */
    public static void saveDebugKeyFrame(int id, byte[] jpegData) {
        if (!DIR_DATA.exists() && !DIR_DATA.mkdirs()) {
            Log.e(TAG, "Failed to create path [" + DIR_DATA + "]");
            return;
        }

        File keyFrame = new File(DIR_DATA, String.format(Locale.US, FILE_NAME_KEYFRAME, id));
        try {
            FileOutputStream outputStream = new FileOutputStream(keyFrame);
            outputStream.write(jpegData);
            outputStream.close();
        } catch (IOException e) {
            Log.e(TAG, "Failed to save key frame to file [" + keyFrame + "]", e);
        }
    }

    /**
     * Save the provided preview frame to the debug output path.
     * <p><i>NOTE: this method may be destructive to the preview frame; make sure there are no other
     * consumers before invoking</i>
     * @param keyFrameId ID of the key frame for which these preview frames are being recorded
     * @param id ID of the preview frame
     * @param frame an {@link OutputFrame} containing the preview frame data. This method may be
     *      destructive to the contents of {@link OutputFrame#buffer}.
     */
    public static void saveDebugPreviewFrame(int keyFrameId, int id, OutputFrame frame) {
        if (!DIR_DATA.exists() && !DIR_DATA.mkdirs()) {
            Log.e(TAG, "Failed to create path [" + DIR_DATA + "]");
            return;
        }

        File previewFrame = new File(DIR_DATA, String.format(Locale.US, 
                FILE_NAME_PREVIEWFRAME, keyFrameId, id));

        try {
            OutputStream outputStream = new FileOutputStream(previewFrame);

            switch (OutputFrame.colorFormat) {
            case NV21:
                byte[] data = new byte[frame.buffer.limit()];
                frame.buffer.position(0);
                frame.buffer.get(data);
                YuvImage im = new YuvImage(data, ImageFormat.NV21, frame.width, frame.height,
                        new int[] { frame.stride, frame.stride });
                im.compressToJpeg(new Rect(0, 0, frame.width, frame.height), 80, outputStream);
                break;

            default:
                Log.e(TAG, "Unsupported color format [" + OutputFrame.colorFormat +
                        "] when saving debug preview frame");
                previewFrame.delete();
                break;
            }

            outputStream.close();
        } catch (IOException e) {
            Log.e(TAG, "Failed to save key frame to file [" + previewFrame + "]", e);
        }
    }
    public static void saveDebugAppSettings(List<Pair<String, String>> appSettings) {
        FileOutputStream outputStream = null;
        BufferedWriter bufferedWriter = null;
        if (DIR_DATA.exists()) {
            try {
                outputStream = new FileOutputStream(FILE_APP_SETTINGS);
                bufferedWriter = new BufferedWriter(new OutputStreamWriter(outputStream));
                try {
                    for (Pair<String,String> setting : appSettings) {
                        bufferedWriter.write(String.format(Locale.US,  GENERAL_TEXT_FILE_FORMAT, setting.first, setting.second));
                        bufferedWriter.newLine();
                    }
                } finally {
                    bufferedWriter.close();
                    outputStream.close();
                }
            } catch (IOException e) {
                Log.e(TAG, "Failed to save app settings to file [" + FILE_APP_SETTINGS + "]", e);
            }
            return;
        }
    }
    public static void saveDebugKeyFramePosition(List<Pair<Integer, Integer>> positions) {
        FileOutputStream outputStream = null;
        BufferedWriter bufferedWriter = null;
        if (DIR_DATA.exists()) {
            try {
                outputStream = new FileOutputStream(FILE_KEYFRAME_POS);
                bufferedWriter = new BufferedWriter(new OutputStreamWriter(outputStream));
                try {
                    for (Pair<Integer,Integer> position : positions) {
                        bufferedWriter.write(String.format(Locale.US,  GENERAL_TEXT_FILE_FORMAT, position.first, position.second));
                        bufferedWriter.newLine();
                    }
                } finally {
                    bufferedWriter.close();
                    outputStream.close();
                }
            } catch (IOException e) {
                Log.e(TAG, "Failed to save file [" + FILE_KEYFRAME_POS + "]", e);
            }
            return;
        }
    }
    public static void saveDebugPreviewFramePosition(List<Pair<Integer, Integer>> positions) {
        FileOutputStream outputStream = null;
        BufferedWriter bufferedWriter = null;
        if (DIR_DATA.exists()) {
            try {
                outputStream = new FileOutputStream(FILE_PREVIEWFRAME_POS);
                bufferedWriter = new BufferedWriter(new OutputStreamWriter(outputStream));
                try {
                    for (Pair<Integer,Integer> position : positions) {
                        bufferedWriter.write(String.format(Locale.US,  GENERAL_TEXT_FILE_FORMAT, position.first, position.second));
                        bufferedWriter.newLine();
                    }
                } finally {
                    bufferedWriter.close();
                    outputStream.close();
                }
            } catch (IOException e) {
                Log.e(TAG, "Failed to save preview frame position to file [" + FILE_PREVIEWFRAME_POS + "]", e);
            }
            return;
        }
    }
}
