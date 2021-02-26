/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.providers;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteQueryBuilder;
import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;

public class SummaryProvider extends ContentProvider {
    // helper constants for use with the UriMatcher
    //for table 1: score_keeper
    private static final int SCORES = 1;

    //for table 2 : AssemblyTimeline
    private static final int ASSEMBLY_TIMELINE = 3;

    //for table 3 : Highlight
    private static final int HIGHLIGHT = 4;
    private static final int HIGHLIGHT_ID = 5;

    //for table 3 : VideoUriComposer
    private static final int SUMMARY_URI_MAPPER = 6;

    //for table 4 : FaceEnrollement
    private static final int FACE_ENROLLMENT = 7;

    private static final UriMatcher URI_MATCHER;
    private static final String TAG = SummaryProvider.class.getSimpleName();

    // prepare the UriMatcher
    static {
        URI_MATCHER = new UriMatcher(UriMatcher.NO_MATCH);

        /*To match Scoring table for a match results in set of rows*/
        URI_MATCHER.addURI(VideoSummaryContract.AUTHORITY,
                VideoSummaryContract.Scores.TABLE_NAME,
                SCORES);

        /*To match AssemblyTimeline table for a match results in set of rows*/
        URI_MATCHER.addURI(VideoSummaryContract.AUTHORITY,
                VideoSummaryContract.AssemblyTimeline.TABLE_NAME,
                ASSEMBLY_TIMELINE);

        /*To match highlight table for a match results in set of rows*/
        URI_MATCHER.addURI(VideoSummaryContract.AUTHORITY,
                VideoSummaryContract.Highlights.TABLE_NAME,
                HIGHLIGHT);
        URI_MATCHER.addURI(VideoSummaryContract.AUTHORITY,
                VideoSummaryContract.Highlights.TABLE_NAME + "/#",
                HIGHLIGHT_ID);

        /*To match VideoUriComposition table for a match results in set of rows*/
        URI_MATCHER.addURI(VideoSummaryContract.AUTHORITY,
                VideoSummaryContract.VideoUriComposition.TABLE_NAME,
                SUMMARY_URI_MAPPER);

        /*To match FaceEnrollment table for a match results in set of rows*/
        URI_MATCHER.addURI(VideoSummaryContract.AUTHORITY,
                VideoSummaryContract.FaceEnrollment.TABLE_NAME,
                FACE_ENROLLMENT);

    }

    public SummaryProvider() {
    }

    @Override
    public String getType(Uri uri) {
        //to handle requests for the MIME type of the data at the given URI.
        switch (URI_MATCHER.match(uri)) {
            case SCORES:
                return VideoSummaryContract.Scores.CONTENT_VIDEO_TYPE;
            case ASSEMBLY_TIMELINE:
                return VideoSummaryContract.AssemblyTimeline.CONTENT_TIMESTAMP_SET_TYPE;
            case HIGHLIGHT:
                return VideoSummaryContract.Highlights.CONTENT_HIGHLIGHT_SET_TYPE;
            case SUMMARY_URI_MAPPER:
                return VideoSummaryContract.VideoUriComposition.CONTENT_COMPOSITION_URI_SET_TYPE;
            case FACE_ENROLLMENT:
                return VideoSummaryContract.FaceEnrollment.CONTENT_COMPOSITION_URI_SET_TYPE;
            default:
                throw new UnsupportedOperationException("Not yet implemented");
        }
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        // Implement this to handle requests to delete one or more rows.
        DatabaseHelper mHelper = DatabaseHelper.getInstance(getContext());
        SQLiteDatabase db = mHelper.getWritableDatabase();
        String tableName;
        int delCount = 0;
        switch (URI_MATCHER.match(uri)) {
            case SCORES:
                tableName = VideoSummaryContract.Scores.TABLE_NAME;
                break;
            case ASSEMBLY_TIMELINE:
                tableName = VideoSummaryContract.AssemblyTimeline.TABLE_NAME;
                break;
            case HIGHLIGHT:
                tableName = VideoSummaryContract.Highlights.TABLE_NAME;
                break;
            case SUMMARY_URI_MAPPER:
                tableName = VideoSummaryContract.VideoUriComposition.TABLE_NAME;
                break;
            case FACE_ENROLLMENT:
                tableName = VideoSummaryContract.FaceEnrollment.TABLE_NAME;
                break;
            default:
                throw new IllegalArgumentException("Unknown URI " + uri);
        }
        Log.i(TAG, "Deleting from table:" + tableName);
        delCount = db.delete(tableName, selection, selectionArgs);
        // notify all listeners of changes:
        if (delCount > 0) {
            getContext().getContentResolver().notifyChange(uri, null);
        }
        return delCount;
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        // TODO: Implement this to handle requests to insert a new row.
        // throw new UnsupportedOperationException("Not yet implemented");
        long id = -1;
        String tableName;

        DatabaseHelper mHelper = DatabaseHelper.getInstance(getContext());
        SQLiteDatabase db = mHelper.getWritableDatabase();

        switch (URI_MATCHER.match(uri)) {
            case SCORES:
                tableName = VideoSummaryContract.Scores.TABLE_NAME;
                break;
            case ASSEMBLY_TIMELINE:
                tableName = VideoSummaryContract.AssemblyTimeline.TABLE_NAME;
                break;
            case HIGHLIGHT:
                tableName = VideoSummaryContract.Highlights.TABLE_NAME;
                break;
            case SUMMARY_URI_MAPPER:
                tableName = VideoSummaryContract.VideoUriComposition.TABLE_NAME;
                break;
            case FACE_ENROLLMENT:
                tableName = VideoSummaryContract.FaceEnrollment.TABLE_NAME;
                break;
            default:
                throw new UnsupportedOperationException("Not yet implemented");
        }
        Log.i(TAG, "Inserting into table:" + tableName);
        id = db.insert(tableName, null, values);

        if (id > 0) {
            Uri itemUri = ContentUris.withAppendedId(uri, id);
            getContext().getContentResolver().notifyChange(itemUri, null);
            return itemUri;
        }
        return null;
    }

    @Override
    public int bulkInsert(Uri uri, ContentValues[] values) {

        long id = -1;
        int insertCount = 0;
        String tableName;

        DatabaseHelper mHelper = DatabaseHelper.getInstance(getContext());
        SQLiteDatabase db = mHelper.getWritableDatabase();

        switch (URI_MATCHER.match(uri)) {
            case SCORES:
                tableName = VideoSummaryContract.Scores.TABLE_NAME;
                break;
            case ASSEMBLY_TIMELINE:
                tableName = VideoSummaryContract.AssemblyTimeline.TABLE_NAME;
                break;
            case HIGHLIGHT:
                tableName = VideoSummaryContract.Highlights.TABLE_NAME;
                break;
            case SUMMARY_URI_MAPPER:
                tableName = VideoSummaryContract.VideoUriComposition.TABLE_NAME;
                break;
            case FACE_ENROLLMENT:
                tableName = VideoSummaryContract.FaceEnrollment.TABLE_NAME;
                break;
            default:
                throw new UnsupportedOperationException("Not yet implemented");
        }
        Log.i(TAG, "Bulk Inserting into table:" + tableName);
        db.beginTransaction();
        for (ContentValues value : values) {
            id = db.insert(tableName, null, value);
            if (id > 0)
                insertCount++;
        }
        db.setTransactionSuccessful();
        db.endTransaction();

        if (insertCount > 0) {
            getContext().getContentResolver().notifyChange(uri, null);
        }
        return insertCount;
    }

    @Override
    public boolean onCreate() {
       DatabaseHelper mHelper = DatabaseHelper.getInstance(getContext());
       return true;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection,
                        String[] selectionArgs, String sortOrder) {
        // TODO: Implement this to handle query requests from clients.
        //throw new UnsupportedOperationException("Not yet implemented");

        DatabaseHelper mHelper = DatabaseHelper.getInstance(getContext());
        SQLiteDatabase db = mHelper.getReadableDatabase();
        SQLiteQueryBuilder builder = new SQLiteQueryBuilder();
        String where;

        switch (URI_MATCHER.match(uri)) {
            case SCORES:
                builder.setTables(VideoSummaryContract.Scores.TABLE_NAME);
                if (TextUtils.isEmpty(sortOrder)) {
                    sortOrder = VideoSummaryContract.Scores.SORT_ORDER_DEFAULT;
                }
                break;
            case ASSEMBLY_TIMELINE:
                builder.setTables(VideoSummaryContract.AssemblyTimeline.TABLE_NAME);
                break;
            case HIGHLIGHT:
                builder.setTables(VideoSummaryContract.Highlights.TABLE_NAME);
                break;
            case HIGHLIGHT_ID:
                builder.setTables(VideoSummaryContract.Highlights.TABLE_NAME);
                where = VideoSummaryContract.Highlights._ID + " = " + uri.getLastPathSegment();
                if (!TextUtils.isEmpty(selection)) {
                    selection = selection + " AND " + where;
                } else {
                    selection = where;
                }
                break;
            case SUMMARY_URI_MAPPER:
                builder.setTables(VideoSummaryContract.VideoUriComposition.TABLE_NAME);
                break;
            case FACE_ENROLLMENT:
                builder.setTables(VideoSummaryContract.FaceEnrollment.TABLE_NAME);
                break;
            default:
                throw new IllegalArgumentException("Unsupported URI: " + uri);
        }
        Log.i(TAG, "Querying table:" + builder.getTables());
        Cursor cursor =
                builder.query(
                        db,
                        projection,
                        selection,
                        selectionArgs,
                        null,
                        null,
                        sortOrder);
        cursor.setNotificationUri(getContext().getContentResolver(), uri);
        return cursor;
    }

    @Override
    public int update(Uri uri, ContentValues values, String selection,
                      String[] selectionArgs) {
        // TODO: Implement this to handle requests to update one or more rows.
        String tableName;
        DatabaseHelper mHelper = DatabaseHelper.getInstance(getContext());
        SQLiteDatabase db = mHelper.getWritableDatabase();
        int updateCount = 0;
        switch (URI_MATCHER.match(uri)) {
            case SCORES:
                tableName = VideoSummaryContract.Scores.TABLE_NAME;
                break;
            case ASSEMBLY_TIMELINE:
                tableName = VideoSummaryContract.AssemblyTimeline.TABLE_NAME;
                break;
            case HIGHLIGHT:
                tableName = VideoSummaryContract.Highlights.TABLE_NAME;
                break;
            case SUMMARY_URI_MAPPER:
                tableName = VideoSummaryContract.VideoUriComposition.TABLE_NAME;
                break;
            case FACE_ENROLLMENT:
                tableName = VideoSummaryContract.FaceEnrollment.TABLE_NAME;
                break;
            default:
                // no support for updating photos or entities!
                throw new IllegalArgumentException("Unsupported URI: " + uri);
        }
        Log.i(TAG, "Updating table:" + tableName);
        updateCount = db.update(
                tableName,
                values,
                selection,
                selectionArgs);
        if (updateCount > 0) {
            getContext().getContentResolver().notifyChange(uri, null);
        }
        return updateCount;
    }
}
