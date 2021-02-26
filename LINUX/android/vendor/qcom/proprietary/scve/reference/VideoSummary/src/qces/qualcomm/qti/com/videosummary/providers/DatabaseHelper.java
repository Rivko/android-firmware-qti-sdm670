/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.providers;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

/**
 * Created by mithun on 8/3/15.
 */
public class DatabaseHelper extends SQLiteOpenHelper {

    private static final String TAG = DatabaseHelper.class.getSimpleName();

    private static DatabaseHelper mInstance = null;

    public static DatabaseHelper getInstance(Context context) {
       if (mInstance == null) {
          mInstance = new DatabaseHelper(context.getApplicationContext());
       }
       return mInstance;
    }

    private DatabaseHelper(Context context) {
       super(context, VideoSummaryContract.DB_NAME, null, VideoSummaryContract.DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        Log.d(TAG, "Creating SCORE_KEEPER_TABLE = " + VideoSummaryContract.Scores.CREATE_TABLE);
        db.execSQL(VideoSummaryContract.Scores.CREATE_TABLE);

        Log.d(TAG, "Creating ASSEMBLY_TIMELINE_TABLE = " + VideoSummaryContract.AssemblyTimeline.CREATE_TABLE);
        db.execSQL(VideoSummaryContract.AssemblyTimeline.CREATE_TABLE);

        Log.d(TAG, "Creating HIGHLIGHTS_TABLE = " + VideoSummaryContract.Highlights.CREATE_TABLE);
        db.execSQL(VideoSummaryContract.Highlights.CREATE_TABLE);

        Log.d(TAG, "Creating FACE_ENROLLMENT_TABLE = " + VideoSummaryContract.FaceEnrollment.CREATE_TABLE);
        db.execSQL(VideoSummaryContract.FaceEnrollment.CREATE_TABLE);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS " + VideoSummaryContract.Scores.TABLE_NAME);
        Log.d(TAG, " onUpgrade droppig table " + VideoSummaryContract.Scores.TABLE_NAME);

        db.execSQL("DROP TABLE IF EXISTS " + VideoSummaryContract.AssemblyTimeline.TABLE_NAME);
        Log.d(TAG, " onUpgrade droppig table " + VideoSummaryContract.AssemblyTimeline.TABLE_NAME);

        db.execSQL("DROP TABLE IF EXISTS " + VideoSummaryContract.Highlights.TABLE_NAME);
        Log.d(TAG, " onUpgrade droppig table " + VideoSummaryContract.Highlights.TABLE_NAME);

        db.execSQL("DROP TABLE IF EXISTS " + VideoSummaryContract.FaceEnrollment.TABLE_NAME);
        Log.d(TAG, " onUpgrade droppig table " + VideoSummaryContract.FaceEnrollment.TABLE_NAME);

        onCreate(db);
    }
}
