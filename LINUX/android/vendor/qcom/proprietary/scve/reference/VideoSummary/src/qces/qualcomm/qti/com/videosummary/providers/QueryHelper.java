/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.providers;

import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.highlights.HighlightsModel;

/**
 * Created by mithun on 8/21/15.
 */
public class QueryHelper {
    private static final String TAG = QueryHelper.class.getSimpleName();

    /**
     * Function to insert into Scorekeeper with videoUri, List<Integers> scores (scores are in the form [m1_s1,m2_s1,m3_s1,m1_s2,m2_s2,m3_s2, .....])
     * where m1 = module1, s1 = score for frame 1
     * this can have max of Constants.MAX_SCORE_MODULES modules i.e m1, m2, m3.. m_MAX_MODE
     * <p>
     * This method will split the score string if its too long (> MAX_SCORE_SIZE, ) to not to overload sqlite cursor window size when querying.
     *
     * @param context, videoUri, scores, faceIDMappingList
     *                 faceIdMappingList is the mapping table which has list of 20byte faceIds of people starring in this video in the order of mapping index
     * @return true if insert succeedes else false;
     */
    public static boolean insertIntoScoreKeeper(Context context, Uri videoUri, List<Integer> scores, byte[] faceIdsAsBytes) {
        boolean status = false;
        String scoreString = Arrays.toString(scores.toArray());
        int scoreSplitSize = scoreString.length() / VideoSummaryContract.MAX_SCORE_SIZE;
        Log.i(TAG, "Inserting scores for URI " + videoUri.toString() + " score string size = " + scoreString.length() + " number of split needed = " + scoreSplitSize + 1);
        for (int overflowBucketIdx = 0; overflowBucketIdx <= scoreSplitSize; overflowBucketIdx++) {
            String splitScoreString = getSplitStringForOverflowBucketIdx(scoreString, overflowBucketIdx);
            status = insertIntoScoreKeeper(context, videoUri, overflowBucketIdx, scores.size() / Constants.MAX_SCORE_MODULES, splitScoreString, faceIdsAsBytes);
            if (status == false) {
                break;
            }
        }
        return status;
    }

    private static String getSplitStringForOverflowBucketIdx(String scores, int idx) {
        int startIdx = idx * VideoSummaryContract.MAX_SCORE_SIZE;
        int endIdx = startIdx + VideoSummaryContract.MAX_SCORE_SIZE;
        return scores.substring(startIdx, Math.min(endIdx, scores.length()));
    }


    /**
     * Internal Function to insert into Scorekeeper with videoUri, overflowBucketIdx, String scores but trinned to MAX_SQLITE_SIZE restriction,
     * called by insertIntoScoreKeeper(Context context, Uri videoUri, List<Integer> scores, byte[] faceIdsAsBytes)
     *
     * @param context, videoUri, scores, faceIDMappingList
     *                 faceIdMappingList is the mapping table which has list of 20byte faceIds of people starring in this video in the order of mapping index
     * @return true if insert succeedes else false;
     */
    private static boolean insertIntoScoreKeeper(Context context, Uri videoUri, int overflowBucketIdx, int numFrames, String scores, byte[] faceIdsAsBytes) {
        boolean status = false;        
        ContentValues value = new ContentValues();
        value.put(VideoSummaryContract.Scores._URI, videoUri.toString());
        value.put(VideoSummaryContract.Scores._OVERFLOW_BUCKET_IDX, overflowBucketIdx);
        value.put(VideoSummaryContract.Scores._NUM_FRAMES, numFrames);
        value.put(VideoSummaryContract.Scores._SCORES, scores);        
        value.put(VideoSummaryContract.Scores._STARRING_IDS, Arrays.toString(faceIdsAsBytes));
        Uri uri = context.getContentResolver().insert(
                VideoSummaryContract.Scores.CONTENT_URI, value);
        if (uri != null) {
            status = true;
        }
        return status;
    }

    public static byte[] getBytesFromString(String byteString) {
        String[] byteValues = byteString.substring(1, byteString.length() - 1).split(",");
        byte[] bytes = new byte[byteValues.length];
        for (int i = 0, len = bytes.length; i < len; i++) {
            bytes[i] = Byte.parseByte(byteValues[i].trim());
        }
        return bytes;
    }

    /**
     * This method will form a big string with all the faceId's as string seperated with ","
     *
     * @param faceIdMappingList
     * @return
     */
    public static String getConcatenatedFaceIdsAsString(List<byte[]> faceIdMappingList) {
        if (faceIdMappingList == null) {
            return null;
        }
        StringBuilder faceIdList = new StringBuilder();
        int numFaceIds = faceIdMappingList.size();
        for (int i = 0; i < numFaceIds; i++) {
            byte[] array = (byte[]) faceIdMappingList.get(i);
            faceIdList.append(new String(array));

            //keep appending a "," till n-1 item
            if (i < (numFaceIds - 1)) {
                faceIdList.append(",");
            }
        }
        return faceIdList.toString();
    }


    /**
     * This method does the opposite of @link getConcatenatedFaceIdsAsString()
     *
     * @param arrayString
     * @return
     */
    public static List<byte[]> getFaceIdsAsArrayListOfByteArrayFromString(String arrayString) {
        if (arrayString == null) {
            return null;
        }
        String[] byteStringArray = arrayString.split(",");

        List<byte[]> faceIdArrayList = new ArrayList<byte[]>();
        for (int i = 0; i < byteStringArray.length; i++) {
            faceIdArrayList.add(byteStringArray[i].getBytes());
        }
        return faceIdArrayList;
    }

    public static void deleteFromScoreKeeper(Context context, Uri uri) {
        int count = context.getContentResolver().delete(VideoSummaryContract.Scores.CONTENT_URI, VideoSummaryContract.Scores._URI + "=?", new String[]{uri.toString()});
        Log.i(TAG, "deleted " + count + " rows");
    }

    public static void deleteAllFromScoreKeeper(Context context) {
        int count = context.getContentResolver().delete(VideoSummaryContract.Scores.CONTENT_URI, null, null);
        Log.i(TAG, "deleted " + count + " rows");
    }

    public static Cursor queryScoreKeeper(Context context) {
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.Scores.CONTENT_URI, VideoSummaryContract.Scores.PROJECTION_ALL,
                null, null, null);
        return cursor;
    }

    /**
     * Function to return an int array which contains scores of each frames in the order m1_s1, m2_s1, m3_s1, m1_s2,m2_s2, m3_s2,.....
     * where m1 = module1, s1 = score for frame 1
     * this can have max of Constants.MAX_SCORE_MODULES modules i.e m1, m2, m3.. m_MAX_SCORE_MODULES
     *
     * @param context, Uri
     * @return int[]
     */
    //TODO: rename this function as getScores after deleting the old method
    public static int[] getScores(Context context, Uri videoUri) {
        int[] scores = null;
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.Scores.CONTENT_URI, VideoSummaryContract.Scores.PROJECTION_ALL,
                VideoSummaryContract.Scores._URI + "=?", new String[]{videoUri.toString()},
                VideoSummaryContract.Scores.SORT_ORDER_DEFAULT);

        StringBuffer scoresStringBuffer = new StringBuffer("");
        Log.i(TAG, "Querying scores for URI " + videoUri);
        while (cursor != null && cursor.moveToNext()) {
            int overflowBucketIdx = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._OVERFLOW_BUCKET_IDX));
            Log.i(TAG, "Querying scores for overflow_bucket_idx " + overflowBucketIdx);
            scoresStringBuffer.append(cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._SCORES)));
        }
        scores = getScoresAsIntArrayFromString(scoresStringBuffer.toString());
        return scores;
    }

    public static byte[] getFaceIds(Context context, Uri videoUri) {
        byte[] faceIdsAsBytes = null;
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.Scores.CONTENT_URI, VideoSummaryContract.Scores.PROJECTION_ALL,
                VideoSummaryContract.Scores._URI + "=?", new String[]{videoUri.toString()},
                VideoSummaryContract.Scores.SORT_ORDER_DEFAULT);

        if (cursor != null && cursor.moveToFirst()) {
            String starringIdString = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._STARRING_IDS));
            faceIdsAsBytes = getBytesFromString(starringIdString);
        }
        return faceIdsAsBytes;
    }

    public static int[] getScoresAsIntArrayFromString(String arrayString) {
        String[] scoreStringArray = arrayString.replace("[", "").replace("]", "").split(", ");
        int[] scoreIntArray = new int[scoreStringArray.length];
        for (int count = 0; count < scoreStringArray.length; count++) {
            scoreIntArray[count] = Integer.parseInt(scoreStringArray[count].trim());
        }
        return scoreIntArray;
    }


    /**
     * Function to return number of frames in a scored video
     *
     * @param context, Uri
     * @return int
     */
    public static int getNumFrames(Context context, Uri videoUri) {
        int numFrames = 0;
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.Scores.CONTENT_URI, VideoSummaryContract.Scores.PROJECTION_ALL,
                VideoSummaryContract.Scores._URI + "=?", new String[]{videoUri.toString()},
                VideoSummaryContract.Scores.SORT_ORDER_DEFAULT);

        if (cursor != null && cursor.moveToFirst()) {
            numFrames = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Scores._NUM_FRAMES));
        }
        return numFrames;
    }

    /**
     * Function to check if a uri video is already scored or not
     *
     * @param context, Uri
     * @return boolean True if scored, false otherwise
     */
    public static boolean isScored(Context context, Uri videoUri) {
        boolean scored = false;
        Cursor cursor = context.getContentResolver().query(VideoSummaryContract.Scores.CONTENT_URI, VideoSummaryContract.Scores.PROJECTION_ALL,
                VideoSummaryContract.Scores._URI + "=?", new String[]{videoUri.toString()},
                VideoSummaryContract.Scores.SORT_ORDER_DEFAULT);

        if (cursor != null && cursor.getCount() > 0) {
            scored = true;
        }
        return scored;
    }


    public static ArrayList<MediaInfo> queryMediaStore(Context context, long startTimestampMillis, long endTimestampMillis) {

        ArrayList<MediaInfo> selectedVideoInfo = new ArrayList<>();
        final String orderBy = MediaStore.Video.Media.DATE_TAKEN;
        final String[] columns = {MediaStore.Video.Media.DATA, MediaStore.Video.Media.LATITUDE,
                MediaStore.Video.Media.LONGITUDE, MediaStore.Video.Media._ID, MediaStore.Video.Media.DURATION, orderBy};
        String where = MediaStore.Video.Media.DATA + " LIKE ? and " + orderBy + " >=? and " + orderBy + " <=? ";
        String whereArgs[] = new String[]{"%DCIM%", "" + Long.toString(startTimestampMillis), "" + Long.toString(endTimestampMillis)};

        Log.i(TAG, "Querying for Date Start:" + CalendarHelper.timeStampMillisToString(startTimestampMillis) + " end:" + CalendarHelper.timeStampMillisToString(endTimestampMillis));
        Cursor cursor = context.getContentResolver().query(
                MediaStore.Video.Media.EXTERNAL_CONTENT_URI, columns,
                where, whereArgs,
                orderBy + " ASC");

        Uri uriBase = MediaStore.Video.Media.getContentUri("external");

        int counter = 1;
        if (cursor != null && cursor.getCount() > 0) {
            while (cursor.moveToNext()) {
                MediaInfo item = new MediaInfo();
                int intID = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Video.Media._ID));
                item.contentUri = ContentUris.withAppendedId(uriBase, intID);
                item.createdDate = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATE_TAKEN));
                item.latitude = cursor.getDouble(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.LATITUDE));
                item.longitude = cursor.getDouble(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.LONGITUDE));
                item.duration = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DURATION));
                String data = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA));

                Log.i(TAG, "Status - count:" + counter + " => uri:" + item.contentUri + ", data:" + data);

                selectedVideoInfo.add(item);
                counter++;
            }
        }

        return selectedVideoInfo;
    }

    public static void insertIntoHighlights(Context context, HighlightsModel item) {
        ContentValues values = getHighlightsItemAsContentValues(item);
        Uri uri = context.getContentResolver().insert(
                VideoSummaryContract.Highlights.CONTENT_URI, values);
    }

    public static Cursor queryHighlights(Context context) {
        List<HighlightsModel> highlightsList = new ArrayList<HighlightsModel>();
        final String orderBy = VideoSummaryContract.Highlights.SORT_ORDER_DEFAULT;
        final String[] columns = VideoSummaryContract.Highlights.PROJECTION_ALL;

        Cursor cursor = context.getContentResolver().query(
                VideoSummaryContract.Highlights.CONTENT_URI, columns,
                null, null,
                orderBy);
        return cursor;
    }

    public static Cursor queryHighlights(Context context, String uriString) {
        List<HighlightsModel> highlightsList = new ArrayList<HighlightsModel>();
        final String orderBy = VideoSummaryContract.Highlights.SORT_ORDER_DEFAULT;
        final String[] columns = VideoSummaryContract.Highlights.PROJECTION_ALL;
        String selection = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{uriString};

        Cursor cursor = context.getContentResolver().query(
                VideoSummaryContract.Highlights.CONTENT_URI, columns,
                selection, selectionArg,
                orderBy);
        return cursor;
    }

    public static void deleteFromHighlights(Context context, String mp4FilePath) {
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{mp4FilePath};

        int count = context.getContentResolver().delete(VideoSummaryContract.Highlights.CONTENT_URI, where, selectionArg);
    }


    public static void deleteSavedToGalleryCards(Context context) {
        deleteFromHighlights(context, HighlightsModel.CardType.SAVED_TO_GALLERY);
    }

    public static void deleteFromHighlights(Context context, HighlightsModel.CardType type) {

        String where = VideoSummaryContract.Highlights._CARD_TYPE + "=?";
        String selectionArg[] = new String[]{(String.valueOf(type.ordinal()))};

        int count = context.getContentResolver().delete(VideoSummaryContract.Highlights.CONTENT_URI, where, selectionArg);
    }


    public static void updateHighlights(Context context, HighlightsModel item) {

        ContentValues values = getHighlightsItemAsContentValues(item);
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{item.videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);
    }

    public static void updateHighlightsTheme(Context context, HighlightsModel item) {
        
        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.Highlights._SPEED_LENGTH, item.speedOrLength);
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{item.videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);
    }

    /**
     * TO update the TITLE in the highlights table given the videoFilePath as key
     */
    public static void updateHighlights(Context context, String videoFilePath, String title, String subTitle) {

        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.Highlights._TITLE, title);
        values.put(VideoSummaryContract.Highlights._SUB_TITLE, subTitle);
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);

        if (count == 0) {
            throw new UnsupportedOperationException("Highlights Update title:" + title + ", didnt take effect, may be trying to update row which doesnot exist in db table");
        }
    }

    public static void updateHighlights(Context context, String videoFilePath, HighlightsModel.CardType type) {

        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.Highlights._CARD_TYPE, type.ordinal());

        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);
        if (count == 0) {
            throw new UnsupportedOperationException("Highlights item with cardType:" + type.toString() + " & path:" + videoFilePath + " is missing in the DB");
        }
    }

    /**
     * This method will change the card type to ERROR and will store the integer error code in the place of thumbinage_uri as String.
     * Error code is defined in constants which can be one of these values
     * Constants.SUMMARY_ERROR_UNKNOWN, SUMMARY_ERROR_LENGTH_INSUFFICIENT = 1, SUMMARY_ERROR_DISK_SPACE_EXCEEDED = 2, SUMMARY_ERROR_INTERRUPTED
     *
     * @param context
     * @param videoFilePath
     * @param errorType
     */
    public static void updateHighlightsAsError(Context context, String videoFilePath, int errorType) {

        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.Highlights._CARD_TYPE, HighlightsModel.CardType.ERROR.ordinal());
        values.put(VideoSummaryContract.Highlights._THUMBNAIL_URI, Integer.toString(errorType));//storing error type in place of thumbImgUri string (as this path is no more useful)
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{videoFilePath};
        int count = context.getContentResolver().update(VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);
        if (count == 0) {
            throw new UnsupportedOperationException("Updating Highlights item to error type failed, uri path:" + videoFilePath + " is missing in the DB");
        }
    }

    /**
     * TO update the CompositionMediaInfoArray in the highlights table given the videoFilePath as key
     */
    public static void updateHighlights(Context context, String videoFilePath, ArrayList<MediaInfo> mediaInfoArrayList) {

        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.Highlights._COMPOSITION_MEDIA_IDS, Utils.getCompositionMediaArrayListAsString(mediaInfoArrayList));
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);

        if (count == 0) {
            throw new UnsupportedOperationException("Highlights item with path:" + videoFilePath + " is missing in the DB");
        }
    }

    /**
     * TO update the faceIdList of all stars in this Summary video in the highlights table given the videoFilePath as key
     */
    public static void updateHighlights(Context context, String videoFilePath, List<byte[]> faceIdList) {

        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.Highlights._STARRING_IDS, getConcatenatedFaceIdsAsString(faceIdList));

        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);

        if (count == 0) {
            throw new UnsupportedOperationException("Highlights Update faceid list didnt take effect, may be trying to update row which doesnot exist in db table");
        }
    }


    public static void updateHighlights(Context context, HighlightsModel olditem, HighlightsModel newItem) {

        ContentValues values = getHighlightsItemAsContentValues(newItem);
        String where = VideoSummaryContract.Highlights._URI + "=?";
        String selectionArg[] = new String[]{olditem.videoFilePath};
        int count = context.getContentResolver().update(
                VideoSummaryContract.Highlights.CONTENT_URI, values, where, selectionArg);
        if (count == 0) {
            throw new UnsupportedOperationException("Highlights item with cardType:" + newItem.cardType.toString() + " & old path:" + olditem.videoFilePath + " is missing in the DB");
        }
    }

    public static ContentValues getHighlightsItemAsContentValues(HighlightsModel item) {

        ContentValues values = new ContentValues();

        values.put(VideoSummaryContract.Highlights._CARD_TYPE, item.cardType.ordinal());
        values.put(VideoSummaryContract.Highlights._SUMMARY_TYPE, item.summaryType.ordinal());
        values.put(VideoSummaryContract.Highlights._CREATED_DATE, item.timeStampMillis);
        values.put(VideoSummaryContract.Highlights._URI, item.videoFilePath);
        values.put(VideoSummaryContract.Highlights._THUMBNAIL_URI, item.thumbImgFilePath);
        values.put(VideoSummaryContract.Highlights._TITLE, item.title);
        values.put(VideoSummaryContract.Highlights._SUB_TITLE, item.subTitle);
        values.put(VideoSummaryContract.Highlights._COMPOSITION_MEDIA_IDS, Utils.getCompositionMediaArrayListAsString(item.compositonMediaList));
        values.put(VideoSummaryContract.Highlights._SPEED_LENGTH, item.speedOrLength);
        values.put(VideoSummaryContract.Highlights._STARRING_IDS, QueryHelper.getConcatenatedFaceIdsAsString(item.faceIdList));
        return values;
    }

    public static Cursor queryAssemblyTimeline(Context context, int type) {

        String where = VideoSummaryContract.AssemblyTimeline._TYPE + "=?";
        String selectionArg[] = new String[]{Integer.toString(type)};
        Cursor cursor = context.getContentResolver().query(
                VideoSummaryContract.AssemblyTimeline.CONTENT_URI,
                VideoSummaryContract.AssemblyTimeline.PROJECTION_ALL,
                where, selectionArg,
                VideoSummaryContract.AssemblyTimeline.SORT_ORDER_DEFAULT
        );
        return cursor;
    }

    public static void updateAssemblyTimeline(Context context, long startTimestamp, long endTimestamp, long summaryTimestamp, int attempts, int type) {

        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP, summaryTimestamp);
        values.put(VideoSummaryContract.AssemblyTimeline._ATTEMPTS, attempts);
        String where = VideoSummaryContract.AssemblyTimeline._START_TIMESTAMP + " =? AND " + VideoSummaryContract.AssemblyTimeline._END_TIMESTAMP + " =? AND " + VideoSummaryContract.AssemblyTimeline._TYPE + " =? ";
        String selectionArg[] = new String[]{Long.toString(startTimestamp), Long.toString(endTimestamp), Integer.toString(type)};
        int count = context.getContentResolver().update(
                VideoSummaryContract.AssemblyTimeline.CONTENT_URI, values, where, selectionArg);
    }

    public static int deleteAssemblyTimeline(Context context, long summaryTimestamp) {
        String where = VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP + " =? ";
        String selectionArg[] = new String[]{Long.toString(summaryTimestamp)};
        int numRowsDeleted = context.getContentResolver().delete(VideoSummaryContract.AssemblyTimeline.CONTENT_URI, where, selectionArg);
        return numRowsDeleted;
    }

    /**
     * Insert a row into the AssemblyTimeline
     *
     * @param context
     * @param startTimestampMillis
     * @param endTimestampMillis
     * @param type                 this can have one of these type values (@value VideoSummaryContract.AssemblyTimeline.TYPE_SCORING_ONLY,
     *                             VideoSummaryContract.AssemblyTimeline.TYPE_HIGHLIGHTS_ASSEMBLY, VideoSummaryContract.AssemblyTimeline.TYPE_TIMELAPSE)
     */
    public static void insertIntoAssemblyTimeline(Context context, long startTimestampMillis, long endTimestampMillis, int type) {
        Utils.sendMessage(context, TAG, "insert into AssemblyTimeline: start=" + CalendarHelper.timeStampMillisToString(startTimestampMillis) + ", end:" + CalendarHelper.timeStampMillisToString(endTimestampMillis));
        ContentValues values = new ContentValues();
        values.put(VideoSummaryContract.AssemblyTimeline._START_TIMESTAMP, startTimestampMillis);
        values.put(VideoSummaryContract.AssemblyTimeline._END_TIMESTAMP, endTimestampMillis);
        values.put(VideoSummaryContract.AssemblyTimeline._SUMMARY_TIMESTAMP, 0);
        values.put(VideoSummaryContract.AssemblyTimeline._ATTEMPTS, 0);
        values.put(VideoSummaryContract.AssemblyTimeline._TYPE, type);
        Uri uri = context.getContentResolver().insert(
                VideoSummaryContract.AssemblyTimeline.CONTENT_URI, values);
    }

}
