/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.providers;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.util.Log;

import java.io.ByteArrayOutputStream;

/**
 * Created by mithun on 8/21/15.
 */
public class FRQueryHelper {
    private static final String TAG = FRQueryHelper.class.getSimpleName();

    public static boolean insertIntoFaceEnrollment(Context context, byte[] frDatasetId, String name, Bitmap image) {
        boolean status = false;
        ContentValues value = new ContentValues();
        value.put(VideoSummaryContract.FaceEnrollment._FR_DATASET_ID, new String(frDatasetId));
        value.put(VideoSummaryContract.FaceEnrollment._PERSON_NAME, name);
        value.put(VideoSummaryContract.FaceEnrollment._PHOTO, BitmapUtilities.getBytes(image));
        Uri uri = context.getContentResolver().insert(
                VideoSummaryContract.FaceEnrollment.CONTENT_URI, value);
        if (uri != null) {
            status = true;
            //If successfully inserted, then clear all the scoring info because it does not have the new face detected scores.
            QueryHelper.deleteAllFromScoreKeeper(context);
        }
        return status;
    }

    public static int deleteFromFaceEnrollment(Context context, byte[] frDatasetId) {
        int count = context.getContentResolver().delete(VideoSummaryContract.FaceEnrollment.CONTENT_URI, VideoSummaryContract.FaceEnrollment._FR_DATASET_ID + "=?", new String[]{new String(frDatasetId)});
        Log.i(TAG, "deleted " + count + " rows");
        if (count > 0) {
            //If successfully deleted atleast one face, then clear all the scoring info because it does have the deleted faces detected scores.
            QueryHelper.deleteAllFromScoreKeeper(context);
        }
        return count;
    }

    public static Cursor queryAllEnrolledFaces(Context context) {
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.FaceEnrollment.CONTENT_URI, VideoSummaryContract.FaceEnrollment.PROJECTION_ALL,
                null, null, null);
        return cursor;
    }

    public static Cursor queryEnrolledFace(Context context, byte[] frDatasetId) {
        String where = VideoSummaryContract.FaceEnrollment._FR_DATASET_ID + "=?";
        String selectionArg[] = new String[]{new String(frDatasetId)};
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.FaceEnrollment.CONTENT_URI, VideoSummaryContract.FaceEnrollment.PROJECTION_ALL,
                where, selectionArg, null);

        if (cursor != null && cursor.getCount() > 0) {
            while (cursor.moveToNext()) {
                return cursor;
            }
        }
        return null;
    }

    public static String queryEnrolledFaceName(Context context, byte[] frDatasetId) {
        String resultName = null;
        String where = VideoSummaryContract.FaceEnrollment._FR_DATASET_ID + "=?";
        String selectionArg[] = new String[]{new String(frDatasetId)};
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.FaceEnrollment.CONTENT_URI, VideoSummaryContract.FaceEnrollment.PROJECTION_ALL,
                where, selectionArg, null);

        if (cursor != null && cursor.getCount() > 0) {
            while (cursor.moveToNext()) {
                resultName = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._PERSON_NAME));
                break;
            }
        }
        return resultName;
    }

    public static Bitmap queryEnrolledFaceBitmap(Context context, byte[] frDatasetId) {
        Bitmap photo = null;
        String where = VideoSummaryContract.FaceEnrollment._FR_DATASET_ID + "=?";
        String selectionArg[] = new String[]{new String(frDatasetId)};
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.FaceEnrollment.CONTENT_URI, VideoSummaryContract.FaceEnrollment.PROJECTION_ALL,
                where, selectionArg, null);

        if (cursor != null && cursor.getCount() > 0) {
            while (cursor.moveToNext()) {
                photo = FRQueryHelper.BitmapUtilities.getImage(cursor.getBlob(cursor.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._PHOTO)));
                break;
            }
        }
        return photo;
    }

    /**
     * This method will only update the row of the matching ID, with the new Image passed as argument
     *
     * @param context
     * @param frDatasetId
     * @param image
     */
    public static void updateEnrolledFace(Context context, byte[] frDatasetId, Bitmap image) {
        ContentValues value = new ContentValues();
        value.put(VideoSummaryContract.FaceEnrollment._PHOTO, BitmapUtilities.getBytes(image));
        String where = VideoSummaryContract.FaceEnrollment._FR_DATASET_ID + "=?";
        String selectionArg[] = new String[]{new String(frDatasetId)};
        int count = context.getContentResolver().update(
                VideoSummaryContract.FaceEnrollment.CONTENT_URI, value, where, selectionArg);
    }

    /**
     * This method will only update the row of the matching ID, with the new name passed as argument
     *
     * @param context
     * @param frDatasetId
     * @param name
     */
    public static void updateEnrolledFace(Context context, byte[] frDatasetId, String name) {
        ContentValues value = new ContentValues();
        value.put(VideoSummaryContract.FaceEnrollment._PERSON_NAME, name);
        String where = VideoSummaryContract.FaceEnrollment._FR_DATASET_ID + "=?";
        String selectionArg[] = new String[]{new String(frDatasetId)};
        int count = context.getContentResolver().update(
                VideoSummaryContract.FaceEnrollment.CONTENT_URI, value, where, selectionArg);
    }

    public static class BitmapUtilities {

        // convert from bitmap to byte array
        public static byte[] getBytes(Bitmap bitmap) {
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            bitmap = createScaledBitmap(bitmap);
            bitmap.compress(Bitmap.CompressFormat.PNG, 0, stream);
            return stream.toByteArray();
        }

        // convert from byte array to bitmap
        public static Bitmap getImage(byte[] image) {
            return BitmapFactory.decodeByteArray(image, 0, image.length);
        }

        private static Bitmap createScaledBitmap(Bitmap bitmap) {
            long MAX_IMAGE_SIZE = 200 * 200l;
            int newWidth, newHeight;
            int origWidth = bitmap.getWidth();
            int origHeight = bitmap.getHeight();
            long originalImageSize = origWidth * origHeight;
            Bitmap b;
            if (originalImageSize > MAX_IMAGE_SIZE) {
            /*
             * calculating new Height & width to maintain aspect ratio
             */
                newHeight = (int) Math.sqrt((MAX_IMAGE_SIZE / (double) origWidth) * origHeight);
                newWidth = (int) (MAX_IMAGE_SIZE / (double) newHeight);

                b = Bitmap.createScaledBitmap(bitmap, newWidth, newHeight, false);
                if (bitmap != null && !bitmap.isRecycled()) {
                    //bitmap.recycle();
                    //bitmap = null;
                }
            } else {
            /*
             * image is smaller than MAX_IMAGE_SIZE, so just return as is.
             */
                b = bitmap;
            }

            return b;
        }

    }
}
