/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.clevercaptureui.utils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Locale;

import com.qualcomm.qti.clevercaptureui.CameraPreviewView.OutputFrame;

import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.os.Environment;
import android.util.Log;

/**
 * This class provide image processing related static function.
 * It is mainly used for testing purpose.
 */
public class ImageUtils {
    private static final String TAG = ImageUtils.class.getSimpleName();
    private static final File DEBUG_OUTPUT_PATH =
            new File(Environment.getExternalStorageDirectory(), "pano_debug/");
    private static final String KEY_FRAME_FILE = "keyframe_%d.jpg";
    private static final String PREVIEW_FRAME_FILE = "previewframe_%d_%d.jpg";

    /**
     * Not constructable
     */
    private ImageUtils() {
    }

    /**
     * Clear any existing debug images from the debug output path
     */
    public static void cleanDebugImagePath() {
        if (DEBUG_OUTPUT_PATH.exists()) {
            File[] files = DEBUG_OUTPUT_PATH.listFiles();
            for (File file : files) {
                file.delete();
            }
        }
    }

    /**
     * Save the provided key frame to the debug output path
     * @param id Key frame index (starting at 0 for the first key frame)
     * @param jpegData JPEG-compressed key frame data
     */
    public static void saveDebugKeyFrame(int id, byte[] jpegData) {
        if (!DEBUG_OUTPUT_PATH.exists() && !DEBUG_OUTPUT_PATH.mkdirs()) {
            Log.e(TAG, "Failed to create path [" + DEBUG_OUTPUT_PATH + "]");
            return;
        }

        File keyFrame = new File(DEBUG_OUTPUT_PATH, String.format(Locale.US, KEY_FRAME_FILE, id));
        try {
            FileOutputStream outputStream = new FileOutputStream(keyFrame);
            outputStream.write(jpegData);
            outputStream.close();
        } catch (IOException e) {
            Log.e(TAG, "Failed to save key frame to file [" + keyFrame + "]", e);
        }
    }

    public static void savePicture(String folder, String name, byte[] jpegData) {
        File keyFrameFolder = new File(folder);

        if (!keyFrameFolder.exists() && !keyFrameFolder.mkdirs()) {
            Log.d(TAG, "fail to create folder " + keyFrameFolder.getAbsolutePath());
            return;
        }

        File keyFrame = new File(keyFrameFolder, name);
        try {
            FileOutputStream outputStream = new FileOutputStream(keyFrame);
            outputStream.write(jpegData);
            outputStream.close();
        } catch (IOException e) {
            Log.e(TAG, "Failed to save picture to file [" + keyFrame + "]", e);
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
        if (!DEBUG_OUTPUT_PATH.exists() && !DEBUG_OUTPUT_PATH.mkdirs()) {
            Log.e(TAG, "Failed to create path [" + DEBUG_OUTPUT_PATH + "]");
            return;
        }

        File previewFrame = new File(DEBUG_OUTPUT_PATH, String.format(Locale.US,
                PREVIEW_FRAME_FILE, keyFrameId, id));

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
}
