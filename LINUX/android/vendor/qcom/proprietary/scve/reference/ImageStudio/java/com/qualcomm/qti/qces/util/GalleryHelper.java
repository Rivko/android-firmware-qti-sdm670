/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.util;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.util.Log;

public class GalleryHelper {

    public static void printImageInfo(Context context, String tag, Uri imageUri) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = 1;
        Bitmap bm;
        if (imageUri == null) {
            Log.i(tag, "Error: file path is " + imageUri);
            return;
        }
        String filePath = getImagePathFromUri(context, imageUri);
        bm = BitmapFactory.decodeFile(filePath, options);
        Log.i(tag, "img path:" + filePath);
        if (bm != null) {
            Log.i(tag, "after decode, ht:" + bm.getHeight() + " wd:" + bm.getWidth() + " density" +
                    bm.getDensity());
        }
    }

    public static String getImagePathFromUri(final Context context, final Uri uri) {

        String scheme = uri.getScheme();
        String path = null;
        Cursor cursor = null;

        if (scheme.equals("file")) {
            path = uri.getPath();
        } else if (scheme.equals("content")) {
            String id = DocumentsContract.getDocumentId(uri);

            if(uri.toString().contains("downloads")){
                final Uri contentUri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));
                cursor = context.getContentResolver().query(contentUri, new String[]{"_data"}, null, null,
                            null);
            }else {
                String[] selectionArgs = new String[]{DocumentsContract.getDocumentId(uri)
                        .split(":")[1]};
                cursor = context.getContentResolver().query(
                        MediaStore.Images.Media.EXTERNAL_CONTENT_URI, new String[]{"_data"},
                        "_id=?", selectionArgs, null);
            }
            if (cursor != null && cursor.moveToFirst()) {
                path = cursor.getString(0);// idx 0 is the _data = file path
                cursor.close();
            }

        } else {
            Log.d("ERROR", "error: unknown uri scheme: " + scheme);
        }
        return path;

    }

}
