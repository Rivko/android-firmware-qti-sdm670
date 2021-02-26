/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.providers;

import android.content.ContentResolver;
import android.net.Uri;
import android.provider.BaseColumns;

/**
 * Created by mithun on 8/10/15.
 */
public final class VideoSummaryContract {


    /**
     * The authority of the videosummary scores provider.
     */
    public static final String AUTHORITY =
            "qces.qualcomm.qti.com.videosummary.providers.SummaryProvider";

    protected static final int DB_VERSION = 1;
    protected static final String DB_NAME = "summary_db";

    public static final int MAX_SCORE_SIZE = 1*1024*1024; //1MB max size of SQLITE_MAX_LENGTH
    public static final class Scores implements BaseColumns {
        private Scores() {

        }

        public static final String TABLE_NAME = "score_keeper";
        public static final String _ID = "_id";
        public static final String _URI = "MEDIA_URI";
        public static final String _OVERFLOW_BUCKET_IDX = "OVERFLOW_BUCKET_IDX"; //this is use to save scores split into multiple rows for same URI when video is too long
        public static final String _NUM_FRAMES = "NUM_FRAMES";
        public static final String _SCORES = "SCORES";
        public static final String _STARRING_IDS = "STARRING_IDS";

        protected static final String CREATE_TABLE = "create table " + TABLE_NAME + " (" +
                _ID + " INTEGER AUTO_INCREMENT, " +
                _URI + " TEXT NOT NULL, " +
                _OVERFLOW_BUCKET_IDX + " INTEGER NOT NULL, " +
                _NUM_FRAMES + " INTEGER NOT NULL, " +
                _SCORES + " TEXT NOT NULL, " +
                _STARRING_IDS + " TEXT, " +
                "PRIMARY KEY (" + _URI + "," + _OVERFLOW_BUCKET_IDX + "))";
        /**
         * The content URI for the videosummary scores authority.
         */
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * The mime type of a directory of scores.
         */
        public static final String CONTENT_VIDEO_TYPE =
                ContentResolver.CURSOR_DIR_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_scores";
        /**
         * The mime type of a single score.
         */
        public static final String CONTENT_FRAME_TYPE =
                ContentResolver.CURSOR_ITEM_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_scores";
        /**
         * A projection of all columns in this table.
         */
        public static final String[] PROJECTION_ALL = {_URI, _OVERFLOW_BUCKET_IDX, _NUM_FRAMES, _SCORES, _STARRING_IDS};
        /**
         * The default sort order
         */
        public static final String SORT_ORDER_DEFAULT = _ID + " ASC";
    }


    public static final class AssemblyTimeline implements BaseColumns {
        private AssemblyTimeline() {

        }

        protected static final String TABLE_NAME = "assembly_timeline";
        protected static final String _ID = "_id";
        public static final String _START_TIMESTAMP = "START_TIMESTAMP";
        public static final String _END_TIMESTAMP = "END_TIMESTAMP";
        public static final String _SUMMARY_TIMESTAMP = "SUMMARY_TIMESTAMP";
        public static final String _ATTEMPTS = "ATTEMPTS";
        public static final String _TYPE = "_TYPE";
        //the _TYPE field can have one of the below 3 values
        public static final int TYPE_SCORING_ONLY = 0;
        public static final int TYPE_HIGHLIGHTS_ASSEMBLY = 1;
        public static final int TYPE_TIMELAPSE = 2;

        protected static final String CREATE_TABLE = "create table " + TABLE_NAME + " (" +
                _ID + " INTEGER AUTO_INCREMENT, " +
                _START_TIMESTAMP + " TIMESTAMP NOT NULL, " +
                _END_TIMESTAMP + " TIMESTAMP NOT NULL, " +
                _SUMMARY_TIMESTAMP + " TIMESTAMP, " +
                _ATTEMPTS + " INTEGER, " +
                _TYPE + " INTEGER, " +
                "PRIMARY KEY (" + _START_TIMESTAMP + "," + _END_TIMESTAMP + "," + _TYPE + "))";

        /**
         * The content URI for the videosummary assembly timeline  authority.
         */
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * The mime type of a directory i.e list of timestamps.
         */
        public static final String CONTENT_TIMESTAMP_SET_TYPE =
                ContentResolver.CURSOR_DIR_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_timestamp";
        /**
         * The mime type of a single row i.e timestamp.
         */
        public static final String CONTENT_TIMESTAMP_TYPE =
                ContentResolver.CURSOR_ITEM_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_timestamp";
        /**
         * A projection of all columns in this table.
         */
        public static final String[] PROJECTION_ALL =
                {_START_TIMESTAMP, _END_TIMESTAMP, _SUMMARY_TIMESTAMP, _ATTEMPTS, _TYPE};

        public static final String SORT_ORDER_DEFAULT =
                _END_TIMESTAMP + " ASC , " + _START_TIMESTAMP + " DESC";
    }

    public static final class Highlights implements BaseColumns {
        private Highlights() {

        }

        public static final String TABLE_NAME = "highlights";
        public static final String _ID = "_id";
        public static final String _CARD_TYPE = "CARD_TYPE";
        public static final String _SUMMARY_TYPE = "SUMMARY_TYPE";
        public static final String _URI = "MEDIA_URI";
        public static final String _THUMBNAIL_URI = "THUMBNAIL_URI";
        public static final String _CREATED_DATE = "CREATED_DATE";
        public static final String _TITLE = "TITLE";
        public static final String _SUB_TITLE = "SUB_TITLE";
        public static final String _LOCATION = "LOCATION";
        public static final String _COMPOSITION_MEDIA_IDS = "COMPOSITION_MEDIA_IDS";
        //Store either (speed1<<16|speed2) for Timelapse or length for Highlights
        public static final String _SPEED_LENGTH = "SPEED_LENGTH";

        public static final String _STARRING_IDS = "STARRING_IDS";

        protected static final String CREATE_TABLE = "create table " + TABLE_NAME + " (" +
                _ID + " INTEGER AUTO_INCREMENT, " +
                _CARD_TYPE + " INTEGER NOT NULL, " +
                _SUMMARY_TYPE + " INTEGER NOT NULL, " +
                _URI + " TEXT NOT NULL, " +
                _THUMBNAIL_URI + " TEXT NOT NULL, " +
                _CREATED_DATE + " INTEGER NOT NULL, " +
                _TITLE + " TEXT NOT NULL, " +
                _SUB_TITLE + " TEXT NOT NULL, " +
                _LOCATION + " TEXT, " +
                _COMPOSITION_MEDIA_IDS + " TEXT, " +
                _SPEED_LENGTH + " INTEGER NOT NULL, " +
                _STARRING_IDS + " TEXT, " +
                "PRIMARY KEY (" + _URI + "))";

        /**
         * The content URI for the videosummary highlights  authority.
         */
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * The mime type of a directory
         */
        public static final String CONTENT_HIGHLIGHT_SET_TYPE =
                ContentResolver.CURSOR_DIR_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_highlight";
        /**
         * The mime type of a single row
         */
        public static final String CONTENT_HIGHLIGHT_TYPE =
                ContentResolver.CURSOR_ITEM_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_highlight";

        /**
         * A projection of all columns in this table.
         */
        public static final String[] PROJECTION_ALL =
                {_CARD_TYPE, _SUMMARY_TYPE, _URI, _THUMBNAIL_URI, _CREATED_DATE, _TITLE, _SUB_TITLE, _LOCATION, _COMPOSITION_MEDIA_IDS, _SPEED_LENGTH, _STARRING_IDS};

        public static final String SORT_ORDER_DEFAULT =
                _CREATED_DATE + " DESC";

    }

    public static final class VideoUriComposition implements BaseColumns {
        private VideoUriComposition() {

        }

        /*
         * This table will have {Summary uri, Uri's from which this summary was composed}
         * This table will be used by highlights table to query what media uri's were used
         * to compose this summary video.
         */
        protected static final String TABLE_NAME = "VideoUriComposition";
        protected static final String _ID = "_id";
        protected static final String _SUMMARY_URI = "SUMMARY_URI";
        protected static final String _URI = "MEDIA_URI";


        protected static final String CREATE_TABLE = "create table " + TABLE_NAME + " (" +
                _ID + " INTEGER AUTO_INCREMENT, " +
                _SUMMARY_URI + " TEXT NOT NULL, " +
                _URI + " TEXT NOT NULL, " +
                "PRIMARY KEY (" + _SUMMARY_URI + "," + _URI + "))";

        /**
         * The content URI for the videosummary's uri composition authority.
         */
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * The mime type of a directory.
         */
        public static final String CONTENT_COMPOSITION_URI_SET_TYPE =
                ContentResolver.CURSOR_DIR_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_highlight_uri_composition";
        /**
         * The mime type of a single row.
         */
        public static final String CONTENT_COMPOSITION_URI_TYPE =
                ContentResolver.CURSOR_ITEM_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_highlight_uri_composition";
        /**
         * A projection of all columns in this table.
         */
        public static final String[] PROJECTION_ALL =
                {_SUMMARY_URI, _URI};

        public static final String SORT_ORDER_DEFAULT = "";
    }


    /**
     * Below tables is used only for FR enrollment
     */
    public static final class FaceEnrollment implements BaseColumns {

        private FaceEnrollment() {

        }

        /*
         * This table will have {FR_DATASET_ID, its unique id at 20 char used while enrolling to FR_db, String name, name of the person, and BLOB photo, bitmap as byte array of the face}
         */
        protected static final String TABLE_NAME = "FaceEnrollment";
        public static final String _FR_DATASET_ID = "FR_DATASET_ID";
        public static final String _PERSON_NAME = "PERSON_NAME";
        public static final String _PHOTO = "PHOTO";


        protected static final String CREATE_TABLE = "create table " + TABLE_NAME + " (" +
                _FR_DATASET_ID + " TEXT NOT NULL, " +
                _PERSON_NAME + " TEXT NOT NULL, " +
                _PHOTO + " BLOB, " +
                "PRIMARY KEY (" + _FR_DATASET_ID + "))";

        /**
         * The content URI for the videosummary's Face enrollment composition authority.
         */
        public static final Uri CONTENT_URI =
                Uri.parse("content://" + AUTHORITY + "/" + TABLE_NAME);

        /**
         * The mime type of a directory.
         */
        public static final String CONTENT_COMPOSITION_URI_SET_TYPE =
                ContentResolver.CURSOR_DIR_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_fr_enrolled";
        /**
         * The mime type of a single row.
         */
        public static final String CONTENT_COMPOSITION_URI_TYPE =
                ContentResolver.CURSOR_ITEM_BASE_TYPE +
                        "/qces.qualcomm.qti.com.videosummary_fr_enrolled";
        /**
         * A projection of all columns in this table.
         */
        public static final String[] PROJECTION_ALL =
                {_FR_DATASET_ID, _PERSON_NAME, _PHOTO};

        public static final String SORT_ORDER_DEFAULT = _PERSON_NAME + "ASC";
    }
}
