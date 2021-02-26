/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;

/**
 * Miscellaneous utility functions that may be required by the renderer package
 */
/*package*/ class Util {
    /**
     * Supported media file types
     */
    public enum MediaFileTypes {
        JPEG("jpg"),
        MPEG_4("mp4");

        private String extension;

        private MediaFileTypes(String extension) {
            this.extension = extension;
        }
    }

    private static final String TAG = "Util";
    private static final String FILENAME_PATTERN = "ObjectTrackerDemo_%s_%d.%s";

    /**
     * Not constructable
     */
    private Util() {
    }

    /**
     * Create a new file for storage of media
     * @param fileType One of the {@link MediaFileTypes} values
     * @return a {@link File} with a unique file name appropriate to the provided media file type
     *      pointing to a writable location, or null if an appropriate location could not be
     *      identified
     * @throws IllegalArgumentException if fileType is null
     */
    public static File generateMediaFile(MediaFileTypes fileType) {
        if (fileType == null) {
            throw new IllegalArgumentException("fileType cannot be null");
        }

        if (!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            Log.e(TAG, "External storage not mounted. State=" +
                    Environment.getExternalStorageState());
            return null;
        }

        File path = new File(Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_DCIM), "/Camera/");
        path.mkdirs();

        File outputFile = null;
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmm", Locale.US);
        String dateTime = dateFormat.format(new Date(System.currentTimeMillis()));
        for (int i = 0; i < 100; i++) {
            File f = new File(path, String.format(Locale.US, FILENAME_PATTERN, dateTime, i,
                    fileType.extension));
            if (!f.exists()) {
                outputFile = f;
                break;
            }
        }

        if (outputFile == null) {
            Log.e(TAG, "Unable to find a free file name to save to");
            return null;
        }

        return outputFile;
    }

    /**
     * Inform the system media scanner of new content to index
     * @param context the application {@link Context}
     * @param uri the filesystem {@link Uri} pointing to the content
     */
    public static void notifyMediaScannerOfContent(Context context, Uri uri) {
        Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
        intent.setData(uri);
        context.sendBroadcast(intent);
    }
}
