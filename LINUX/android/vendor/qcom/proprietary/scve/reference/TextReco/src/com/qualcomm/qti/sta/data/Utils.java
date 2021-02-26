/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.data;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import android.content.Context;
import android.content.ContextWrapper;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.ExifInterface;
import android.media.MediaScannerConnection;
import android.os.Environment;
import android.util.Log;
import android.util.TypedValue;
import android.view.MotionEvent;

/**
 * Utility class
 */
public class Utils {
    private static final String LOG_TAG = "Utils";

    public static final String INTENT_KEY_FIND_MORE_TYPE = "find_more_type";
    public static final String INTENT_KEY_TEXT = "text";
    public static final String INTENT_KEY_TEXT_INDEX = "index";
    public static final String INTENT_KEY_DATETIME_AT = "datetime_at";
    public static final String INTENT_KEY_DATETIME_INDEX = "datetime_index";
    public static final String INTENT_KEY_LOCALE = "locale";
    public static final String INTERNAL_STORAGE_DIRECTORY = "imageDir";

    private static final String JPG_FILE_NAME = "snapshot.jpg";
    private static final String IC_JPG_FILE_NAME = "Rectified";

    /**
     * Enum for data category
     */
    public enum CategoryType {
        NONE, CONTACT, EVENT, ALL_RESULTS;
    }

    /**
     * Enum for category type
     */
    public enum CategoryLevel {
        NONE, PRIMARY, SECONDARY;
    }

    /**
     * Get the float value from dimension float variable
     *
     * @param resources
     *            application resource reference
     * @param resourceId
     *            resource id
     * @return float value
     */
    public static float getFloatFromDimen(Resources resources, int resourceId) {
        TypedValue typedValue = new TypedValue();
        resources.getValue(resourceId, typedValue, true);
        return typedValue.getFloat();
    }

    /**
     * gets the exif rotation from the jpg file
     *
     * @param jpgFilePath
     *            absolute jpg file path
     * @return jpg rotation
     * @throws IOException
     *             exception while parsing out jpg information
     */
    public static int getExifRotation(String jpgFilePath) throws IOException {
        ExifInterface exif = new ExifInterface(jpgFilePath);
        int jpgRotation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                ExifInterface.ORIENTATION_UNDEFINED);

        // TODO: do i need to handle other orientation cases
        int angle = 0;
        switch (jpgRotation) {
        case ExifInterface.ORIENTATION_ROTATE_90:
            angle = 90;
            break;
        case ExifInterface.ORIENTATION_ROTATE_180:
            angle = 180;
            break;
        case ExifInterface.ORIENTATION_ROTATE_270:
            angle = 270;
            break;
        }
        return angle;
    }

    /**
     * Converts nv21 bytes to bitmap
     *
     * @param data
     *            nv21 bytes
     * @param width
     *            image width
     * @param height
     *            image height
     * @return bitmap
     */
    public static Bitmap getBitmapFromYUV(byte[] data, int width, int height) {
        YuvImage yuvimage = new YuvImage(data, ImageFormat.NV21, width, height, null);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        yuvimage.compressToJpeg(new Rect(0, 0, width, height), 100, baos);
        byte[] jdata = baos.toByteArray();
        BitmapFactory.Options bitmapFatoryOptions = new BitmapFactory.Options();
        bitmapFatoryOptions.inPreferredConfig = Bitmap.Config.ARGB_8888;
        Bitmap bmp = BitmapFactory.decodeByteArray(jdata, 0, jdata.length, bitmapFatoryOptions);
        return bmp;
    }

    /**
     * converts a phone string to USA phone formats
     *
     * @param phoneText
     *            raw phone text
     * @return formatted text
     */
    public static String toUsaFormattedPhone(String phoneText) {
        StringBuffer rawNumber = new StringBuffer(phoneText.replaceAll("[\\D]", ""));
        int digits = rawNumber.length();

        if (digits > 4) {// 5+
            // 0123456 => 012-3456
            rawNumber.insert(digits - 4, "-");
        } else { // less than 5
            return new String(phoneText);
        }

        int newLength = rawNumber.length();
        if (digits > 7) {// 7+
            rawNumber.insert(newLength - 8, "-");
            newLength++;
        }
        if (digits > 10) {// 10+
            rawNumber.insert(newLength - 12, "-");
        }
        return rawNumber.toString();
    }

    /**
     * gets the midpoint from motion events
     *
     * @param midPoint
     *            out parameter for the mid point
     * @param event
     *            motion event from the user touch
     */
    public static void getMidPoint(PointF midPoint, MotionEvent event) {
        float xMid = (event.getX(0) + event.getX(1)) / 2;
        float yMid = (event.getY(0) + event.getY(1)) / 2;
        midPoint.set(xMid, yMid);
    }

    /**
     * Gets the spacing between the user fingers
     *
     * @param event
     *            motion event
     * @return spacing
     */
    public static float getSpacing(MotionEvent event) {
        float xValue = event.getX(0) - event.getX(1);
        float yValue = event.getY(0) - event.getY(1);
        return (float) Math.hypot(xValue, yValue);
    }

    /**
     * gets the local jpg file object
     *
     * @param context
     *            application context
     * @return File object
     */
    public static String getLocalImageFile(Context context) {
        ContextWrapper contextWrapper = new ContextWrapper(context);
        File location = contextWrapper.getDir(Utils.INTERNAL_STORAGE_DIRECTORY,
                Context.MODE_PRIVATE);
        File jpgFile = new File(location, JPG_FILE_NAME);

        if (!location.exists()) {
            location.mkdirs();
        }
        return jpgFile.toString();
    }

    /**
     * Save the bitmap to gallery
     *
     * @param context
     *            application context
     * @param bitmap
     *            bitmap to be saved
     */
    public static void saveBitmap2Gallery(Context context, Bitmap bitmap) {
        File publicStoragePath = Environment
                .getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
        String fullImagePath = publicStoragePath + "/" + IC_JPG_FILE_NAME + "_"
                + System.currentTimeMillis() + ".jpg";

        if (publicStoragePath.isDirectory() == false) {
            if (publicStoragePath.mkdirs() == false) {
                Log.d(LOG_TAG, "Unable to make directory for saving images");
            }
        }
        if (saveBitmap(fullImagePath, bitmap)) {
            MediaScannerConnection.scanFile(context, new String[] { fullImagePath }, null, null);
            Log.d(LOG_TAG, "Image saved successfully: " + fullImagePath);
        }
    }

    /**
     * Saves a bitmap with file name
     *
     * @param fileName
     *            file name for the image
     * @param bitmap
     *            image
     * @return true if saved successfully, false otherwise
     */
    public static boolean saveBitmap(String fileName, Bitmap bitmap) {
        try {
            FileOutputStream fileOutputStream = new FileOutputStream(fileName);
            bitmap.compress(Bitmap.CompressFormat.JPEG, 100, fileOutputStream);
            fileOutputStream.flush();
            fileOutputStream.close();
            return true;

        } catch (FileNotFoundException e) {
            Log.d(LOG_TAG, "Image saved Error: File Not Found");

        } catch (IOException e) {
            Log.d(LOG_TAG, "Image saved Error: Input output Exception");
        }
        return false;
    }
}
