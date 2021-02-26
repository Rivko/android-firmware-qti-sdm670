/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.objectcloneremoveshared;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;

import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Path;

/**
 * This class holds helper functions for working with temporary image files.
 * @author slaver
 */
public class TemporaryImageUtils {

    private static final String PREFIX = "tempimg";
    private static final String SUFFIX_PNG = ".png";
    private static final String SUFFIX_JPG = ".jpg";
    private static final String SUFFIX_TMP = ".tmp";

    /**
     * Prevent {@link TemporaryImageUtils} from being instantiated. It only contains static methods
     */
    private TemporaryImageUtils() {
    }

    /**
     * This function will create a temporary image {@link File} at the specified path, with the
     * specified extension. The filename will start with a {@link #PREFIX}, then contain a random
     * string, before ending with the specified suffix.
     * @param path {@link Path} at which the file should be created. If the path does not exist, it
     *        will be created
     * @param suffix Extension for the file
     * @return a {@link File} object pointing to an empty file on disk
     * @throws IOException if the path or temporary file could not be created
     */
    public static File createTemporaryImageFile(File path, String suffix) throws IOException {
        path.mkdirs();
        if (!path.exists()) {
            throw new IOException("Failed to create temporary file path " + path.getPath());
        }
        File f = File.createTempFile(PREFIX, suffix, path);
        if (!f.exists()) {
            throw new IOException("Failed to create temporary file " + f.getPath());
        }
        return f;
    }

    /**
     * Saves the provided {@link Bitmap} to the specified path, using specified format and quality
     * @param path {@link File} object containing the path to which to write the compressed image
     *        data. A new file with a temporary file name will be created to hold the compressed
     *        image data. This path does not need to exist beforehand
     * @param b {@link Bitmap} to compress
     * @param c {@link CompressFormat} in which to compress the image (ie JPG, PNG, etc)
     * @param quality see {@link Bitmap#compress(CompressFormat, int, java.io.OutputStream)}
     * @return a {@link File} pointing to the newly created image file
     * @throws IOException if the path or temporary file could not be created or written to
     */
    public static File saveBitmapToTemporaryFile(File path, Bitmap b, CompressFormat c, int quality)
            throws IOException {
        String suffix;
        switch (c) {
        case JPEG:
            suffix = SUFFIX_JPG;
            break;

        case PNG:
            suffix = SUFFIX_PNG;
            break;

        default:
            suffix = SUFFIX_TMP;
            break;
        }

        File f = TemporaryImageUtils.createTemporaryImageFile(path, suffix);
        FileOutputStream fos = new FileOutputStream(f);
        if (!b.compress(c, quality, fos)) {
            throw new IOException("Failed to compress bitmap to temporary file " + f.getPath());
        }
        fos.close();

        return f;
    }

    /**
     * Clean up all temporary files at the given path. For this function to be useful, the path
     * should be the same one used for {@link #createTemporaryImageFile(File, String)} and
     * {@link #saveBitmapToTemporaryFile(File, Bitmap, CompressFormat, int)}
     * @param path Path at which to clean up all temporary files (anything created by this class)
     */
    public static void deleteAllTemporaryFiles(File path) {
        if (path == null) {
            return;
        }

        File[] files = path.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                return filename.startsWith(PREFIX);
            }
        });

        for (File file : files) {
            file.delete();
        }
    }
}
