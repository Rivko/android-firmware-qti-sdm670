/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm.util;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;

import com.oma.drm.DrmImage;
import com.oma.drm.OmaDrmInfo;
import com.oma.drm.OmaDrmInputStream;

public class OmaDrmUtils {
    private static final String TAG = "OmaDrm:DrmUtils";

    public static final boolean DEBUG = true;
    private static final int DRM_DUMP_BYTE_SIZE = 4096;

    private static boolean sIsOmaDrmEnabled = false;

    /**
     * Decode the Drm Image file and return respective bitmap object. One unit
     * of DRM license will consumed using this function.
     *
     * @param context
     * @param path
     *            of a DRM file Path
     * @param uri
     *            of a Drm file {@link Uri}
     * @param mimetype
     * @return {@link Bitmap} of a DRM File if it has valid license.
     *         <code>null</code> when no license left.
     */
    public static final DrmImage getDrmImage(Context context, Uri uri,
            String mimetype) {
        return getDrmImage(context, uri, mimetype, true);
    }

    /**
     * Decode the Drm Image file and return respective bitmap thumbnail object
     * object. DRM license will not be consumed when generating a thumbnail of a
     * DRM file
     *
     * @param context
     * @param path
     *            of a DRM file Path
     * @param uri
     *            of a Drm file {@link Uri}
     * @param mimetype
     * @return {@link Bitmap} of a DRM File if it has valid license.
     *         <code>null</code> when no license left.
     */
    public static final DrmImage getThumbnailDrmImage(Context context, Uri uri,
            String mimetype) {
        return getDrmImage(context, uri, mimetype, false);
    }

    /**
     * Decode the Drm Image file and return respective bitmap object. One unit
     * of DRM license will consumed when consumeRight is true. Basically this
     * function is useful for thumbnail generation.
     *
     * @param context
     * @param path
     *            of a DRM file Path
     * @param uri
     *            of a Drm file {@link Uri}
     * @param mimetype
     * @param consumeRights
     *            True when you ants to consume rights
     * @return {@link Bitmap} of a DRM File if it has valid license.
     *         <code>null</code> when no license left.
     */
    private static final DrmImage getDrmImage(Context context, Uri uri,
            String mimetype, boolean consumeRights) {
        Bitmap result = null;
        DrmImage drmImage = null;
        OmaDrmInputStream is = null;
        OmaDrmInfo omaDrmInfo = null;
        try {
            is = new OmaDrmInputStream(context, uri, consumeRights);
            Options options = new Options();
            options.inPreferredConfig = Config.ARGB_8888;
            result = BitmapFactory.decodeStream(is, null, options);
            omaDrmInfo = is.getOmaDrmInfo();
            drmImage = new DrmImage(result, uri, omaDrmInfo);
            drmImage.setSize(is.size());
        } catch (Exception e) {
            Log.e(TAG, "Error while getBitmap! " + e);
            // e.printStackTrace();
        } finally {
            try {
                if (is != null) {
                    is.close();
                }
            } catch (IOException e) {
                Log.e(TAG, "Unable to close drm file: " + e);
            }
        }
        return drmImage;
    }

    public void dumpDecodedData(Context context, Uri drmUri) {
        // Don't use this method, this only applicable for
        // developer mode.
        if (!DEBUG) {
            Log.i(TAG, "You can't dump drm file on non-debug build");
            return;
        }

        byte bytes[] = new byte[DRM_DUMP_BYTE_SIZE];
        int offset = 0;
        InputStream is = null;
        FileOutputStream fos = null;
        int length;
        File download = Environment
                .getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);

        try {
            is = new OmaDrmInputStream(context, drmUri, false);
            File file = new File(download, "drm_dump.file");
            if (file.exists()) {
                file.delete();
            }
            file.createNewFile();
            fos = new FileOutputStream(file);

            while ((length = is.read(bytes, offset, DRM_DUMP_BYTE_SIZE)) > 0) {
                fos.write(bytes, 0, length);
                offset += length;
            }

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } finally {
            try {
                if (is != null) {
                    is.close();
                }
                if (fos != null) {
                    fos.close();
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    public static final boolean isOmaDrmEnabled() {
        return sIsOmaDrmEnabled;
    }

    public static int initializeInputStream(int uniqueId, FileDescriptor fd,
            boolean consumeRights) {
        return _initializeOmaDrmInputStream(uniqueId, fd, consumeRights);
    }

    public static int releaseInputStream(int uniqueId) {
        return _closeOmaDrmInputStream(uniqueId);
    }

    public static int readFromOmaDrmInputStream(int uniqueId, byte[] buffer,
            int numBytes, int offset) {
        return _readOmaDrmInputStream(uniqueId, buffer, numBytes, offset);
    }

    public static long getFileSizeFromOmaDrmInputStream(int uniqueId) {
        return _fileSizeOmaDrmInputStream(uniqueId);
    }

    public static Object getOmaDrmInfoOmaDrmInputStream(int uniqueId) {
        return _getOmaDrmInfoOmaDrmInputStream(uniqueId);
    }

    public static Bitmap resizeImage(int desirableHeight, int desirableWidth,
            Bitmap bitmap, Context context) {
        Bitmap resizedImage;
        final double heightBm = bitmap.getHeight(); // height of Bitmap in
                                                    // memory
        final double widthBm = bitmap.getWidth(); // width of Bitmap in memory
        double width = 0;
        double height = 0;
        double factor = 1.0;
        if (heightBm >= widthBm) {
            factor = desirableHeight / heightBm;
            height = desirableHeight;
            width = Math.round(widthBm * factor);
        } else {
            factor = desirableWidth / widthBm;
            width = desirableWidth;
            height = Math.round(heightBm * factor);
        }
        resizedImage = Bitmap.createScaledBitmap(bitmap, (int) width,
                (int) height, true);
        return resizedImage;
    }

    static {
        Log.i(TAG, "Loading OmaDrmUtils native library ...");

        // Check and load Oma Drm native libraries
        File file1 = new File("/vendor/lib/drm/libomadrmengine.so");
        File file2 = new File("/vendor/lib64/drm/libomadrmengine.so");
        File file3 = new File("/system/lib/drm/libomadrmengine.so");
        File file4 = new File("/system/lib64/drm/libomadrmengine.so");
        if (file1.exists() || file2.exists() || file3.exists()
                || file4.exists()) {
            sIsOmaDrmEnabled = true;
            File fileomadrmhelper1 = new File(
                    "/system/lib/libomadrmutils_jni.so");
            File fileomadrmhelper2 = new File(
                    "/system/lib64/libomadrmutils_jni.so");
            if (fileomadrmhelper1.exists() || fileomadrmhelper2.exists()) {
                System.loadLibrary("omadrmutils_jni");
                Log.i(TAG, "OmaDrmUtils native library loaded successfully");
            }
        }
    }

    public static final String getFilePath(Context context, Uri uri) {
        String path = null;
        Cursor cursor = null;
        if (isOmaDrmEnabled()) {
            try {
                cursor = doQuery(context, uri,
                        new String[] { MediaStore.Files.FileColumns.DATA });
                if (cursor.moveToFirst()) {
                    path = cursor.getString(0);
                }
            } catch (Exception e) {
                Log.e(TAG, "Could not get drm file path : Exception : " + e);
            } finally {
                if (cursor != null && !cursor.isClosed()) {
                    cursor.close();
                }
            }
        }
        return path;
    }

    private static Cursor doQuery(Context context, Uri uri, String[] projection) {
        Cursor cursor = context.getContentResolver().query(uri, projection,
                null, null, null);
        return cursor;
    }

    public static String getFileName(String path) {
        String fileNameWithExtension, fileName;
        if (path.isEmpty()) {
            return "";
        }
        String[] splitPath = path.split("/");
        fileNameWithExtension = splitPath[splitPath.length - 1];
        if (fileNameWithExtension.isEmpty()) {
            return "";
        }
        fileName = fileNameWithExtension.substring(0,
                fileNameWithExtension.lastIndexOf('.'));
        return fileName;
    }

    public static OmaDrmInfo getOmaDrmInfo(Context context, Uri mUri) {
        // TODO Auto-generated method stub
        OmaDrmInputStream is = null;
        OmaDrmInfo omaDrmInfo = null;
        try {
            if (omaDrmInfo == null) {
                is = new OmaDrmInputStream(context, mUri, false);
                omaDrmInfo = is.getOmaDrmInfo();
            }
        } catch (Exception e) {
            Log.e(TAG, "Finishing DrmDetailsDialogActivity " + e);
        } finally {
            if (is != null) {
                try {
                    is.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
        return omaDrmInfo;
    }


    private static native int _readOmaDrmInputStream(int uniqueId,
            byte[] buffer, int numBytes, int offset);

    private static native int _initializeOmaDrmInputStream(int uniqueId,
            FileDescriptor fd, boolean consumeRights);

    private static native int _closeOmaDrmInputStream(int uniqueId);

    private static native long _fileSizeOmaDrmInputStream(int uniqueId);

    private static native Object _getOmaDrmInfoOmaDrmInputStream(int uniqueId);

}
