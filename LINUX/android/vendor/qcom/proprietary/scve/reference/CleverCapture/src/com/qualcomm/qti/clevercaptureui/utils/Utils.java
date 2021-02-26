/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.clevercaptureui.utils;

import java.io.IOException;

import android.content.res.Resources;
import android.graphics.PointF;
import android.media.ExifInterface;
import android.util.TypedValue;
import android.view.MotionEvent;

/**
 * Utility class
 */
public class Utils {
    public static final String INTENT_KEY_FIND_MORE_TYPE = "find_more_type";
    public static final String INTENT_KEY_TEXT = "text";
    public static final String INTENT_KEY_TEXT_INDEX = "index";
    public static final String INTENT_KEY_DATE_TIME = "date_time";

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
}
