/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.ims;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Toast;

import org.codeaurora.ims.QtiImsUnsupportedImageFormatException;

// This file is responsible for fetching an absolute file path given a image Uri
public class ImageUtils {
    private static String TAG = "ImageUtils";

    private static final String MEDIA_DOCUMENT =
            "com.android.providers.media.documents";
    private static final String DOWNLOAD_DOCUMENT =
            "com.android.providers.downloads.documents";

    private static boolean isMediaDocument(Uri uri) {
        return MEDIA_DOCUMENT.equals(uri.getAuthority());
    }

    private static boolean isDownloadDocument(Uri uri) {
        return DOWNLOAD_DOCUMENT.equals(uri.getAuthority());
    }

    private static String getFilePathFromDb(Context context, Uri uri, String selection,
            String[] selectionArgs, String column) {
        Cursor cursor = null;

        try {
            cursor = context.getContentResolver().query(uri, null, selection, selectionArgs,
                    null);
            if (cursor != null && cursor.moveToFirst()) {
                final int column_index = cursor.getColumnIndexOrThrow(column);
                return cursor.getString(column_index);
            }
        } catch (IllegalArgumentException ex) {
            Log.e(TAG, "column doesn't exist ex: " + ex);
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return null;
    }

    private static String getDownloadDocumentFilePath(Uri uri, Context context) {
        /* download doc uri format is for eg.
           "content://com.android.providers.downloads.documents/document/7" */
        final String column = "_data";
        final String id = DocumentsContract.getDocumentId(uri);
        // form content uri
        final Uri contentUri = ContentUris.withAppendedId(
                Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));

        return getFilePathFromDb(context, contentUri, null, null, column);
    }

    private static String getMediaDocumentFilePath(Uri uri, Context context) {
        /* media doc uri format is for eg.
           "content://com.android.providers.media.documents/document/image%3A47" */
        final String IMAGE = "image";
        final String column = "_data";
        final String docId = DocumentsContract.getDocumentId(uri);
        Log.d(TAG, "docId : " + docId);

        if (docId == null) {
            return null;
        }

        // docId is of format eg. "image:47"
        final String[] split = docId.split(":");
        if (split != null && !IMAGE.equals(split[0])) {
            Log.w(TAG, "getMediaDocumentFilePath not an image file");
            return null;
        }

        // below contentUri is of type "//content://media/external/images/media"
        Uri contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
        final String selection = "_id=?";
        final String[] selectionArgs = new String[] {
            split[1]
        };

        return getFilePathFromDb(context, contentUri, selection, selectionArgs, column);
    }

    private static boolean isSupportedFileFormat(String filePath) {
       return (filePath != null && (filePath.endsWith(".jpg") || filePath.endsWith(".png") ||
           filePath.endsWith(".jpeg")));
    }

    private static String getFilePath(Uri uri, Context context) throws
            QtiImsUnsupportedImageFormatException {
        String filePath = getFilePathFromDb(context, uri, null, null, MediaStore.Images.Media.DATA);
        /* file manager app for eg. doesn't disable unsupported file formats so user can end up
           selecting them. So, validate such selected files against supported file formats */
        if (isSupportedFileFormat(filePath)) {
            return filePath;
        } else {
            Log.d(TAG, "getFilePath unsupported file = " + filePath);
            throw new QtiImsUnsupportedImageFormatException("unsupported image format");
        }
    }

    // returns absolute file path given a Uri
    public static String getFilePathFromUri(Uri uri, Context context) throws
            QtiImsUnsupportedImageFormatException {
        String filePath = null;
        if (uri == null) {
            Log.w(TAG, "getFilePathFromUri uri is null");
            return filePath;
        }

        if (isMediaDocument(uri)) {
            // Document that is in media eg. Image folder is selected
            filePath = getMediaDocumentFilePath(uri, context);
        } else if (isDownloadDocument(uri)) {
            // Document that is in download folder is selected
            filePath = getDownloadDocumentFilePath(uri, context);
        } else {
            // Document that is in for eg. file manager app is selected
            filePath = getFilePath(uri, context);
        }

        Log.d(TAG, "getFilePathFromUri filePath = " + filePath);
        return filePath;
    }
}
