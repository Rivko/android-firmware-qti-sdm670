/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Util;

/**
 * Created by mithun on 7/27/15.
 */
public class Constants {

    public static final int BATTERY_LEVEL_TO_START_VIDEO_SUMMARY = 15;
    public static final boolean PERFORM_DAILY_SCORING = true;

    public static final String EXTRA_URIS = "qces.qualcomm.qti.com.videosummary.EXTRA_URIS";
    public static final String EXTRA_URI = "qces.qualcomm.qti.com.videosummary.EXTRA_URI";
    public static final String EXTRA_MEDIAINFO_ARRAYLIST = "qces.qualcomm.qti.com.videosummary.EXTRA_MEDIAINFO_ARRAYLIST";
    public static final String EXTRA_MEDIAINFO = "qces.qualcomm.qti.com.videosummary.EXTRA_MEDIAINFO";
    public static final String EXTRA_TIME_STAMP_MILLIS = "qces.qualcomm.qti.com.videosummary.EXTRA_TIME_STAMP_MILLIS";
    public static final String EXTRA_HIGHLIGHTS_MODEL = "qces.qualcomm.qti.com.videosummary.EXTRA_HIGHLIGHTS_MODEL";

    public static final String ACTION_PROGRESS_UPDATE_RECEIVED = "qces.qualcomm.qti.com.videosummary.ACTION_PROGRESS_UPDATE_RECEIVED";
    public static final String EXTRA_HIGHLIGHTS_PROGRESS = "qces.qualcomm.qti.com.videosummary.EXTRA_HIGHLIGHTS_PROGRESS";
    public static final String EXTRA_NUM_URIS = "qces.qualcomm.qti.com.videosummary.EXTRA_NUM_URIS";
    public static final String EXTRA_CURRENT_URI_INDEX = "qces.qualcomm.qti.com.videosummary.EXTRA_CURRENT_URI_INDEX";

    public static final String ACTION_SCORE = "qces.qualcomm.qti.com.videosummary.VideoProcessing.action.SCORE_VIDEO";
    public static final String ACTION_ASSEMBLE = "qces.qualcomm.qti.com.videosummary.VideoProcessing.action.ASSEMBLE_VIDEO";
    public static final String ACTION_CHANGE_MUSIC = "qces.qualcomm.qti.com.videosummary.VideoProcessing.action.CHANGE_MUSIC";
    public static final String EXTRA_VIDEO_KEEP_FRAMES = "qces.qualcomm.qti.com.videosummary.VideoProcessing.extra.VIDEO_KEEP_FRAMES";
    public static final String EXTRA_NUM_KEEP_FRAMES = "qces.qualcomm.qti.com.videosummary.VideoProcessing.extra.NUM_KEEP_FRAMES";


    /***
     * Theme related audio assets and default theme
     */
    public static final String[] THEME = {
            "music_files/action.m4a",
            "music_files/calm.m4a",
            "music_files/dramatic.m4a",
            "music_files/happy.m4a",
            "music_files/urban.m4a"};

    public static final String[] HAPPY_ASD_CATEGORY_LIST = {
            "ASDCV_INDOOR",
            "ASDCV_AMUSEMENT_PARK",
            "ASDCV_PLAYGROUND",
            "ASDCV_STREET",
            "ASDCV_SWIMMING_POOL",
            "ASDCV_GARDEN"};

    public static final String[] URBAN_ASD_CATEGORY_LIST = {
            "ASDCV_INDUSTRIAL",
            "ASDCV_BRIDGE",
            "ASDCV_CHURCH",
            "ASDCV_SKYSCRAPER",
            "ASDCV_PARKING_LOT_STRUCTURE",
            "ASDCV_ARCHITECTURE_BUILDING"};

    public static final String[] ACTION_ASD_CATEGORY_LIST = {
            "ASDCV_SPORTS"};

    public static final String[] CALM_ASD_CATEGORY_LIST = {
            "ASDCV_WATER",
            "ASDCV_COAST",
            "ASDCV_POND",
            "ASDCV_RIVER",
            "ASDCV_OCEAN",
            "ASDCV_FOREST"};

    public static final String[] DRAMATIC_ASD_CATEGORY_LIST = {
            "ASDCV_ICE",
            "ASDCV_CANYON",
            "ASDCV_DESERT",
            "ASDCV_MOUNTAIN",
            "ASDCV_SKY"};


    public static final String FR_HEADER = "$#FR";
    public static final int FR_ID_LENGTH = 20;

    public enum THEME_TYPE {ACTION, CALM, DRAMATIC, HAPPY, URBAN}

    public static final int DEFAULT_THEME = THEME_TYPE.HAPPY.ordinal();
    //End of Theme constants


    public static final int MODE_BASIC_INDEX = 0;
    public static final int MODE_MOTION_INDEX = 1;
    public static final int MODE_FACE_INDEX = 2;
    public static final int MODE_ASD_INDEX = 3;
    public static final int MAX_SCORE_MODULES = 4;

    public static final int AFTER_HOURS_START = 21;//after 9 pm
    public static final int AFTER_HOURS_END = 9;//to 9 am

    public static final int SUMMARY_ERROR_UNKNOWN = 0;
    public static final int SUMMARY_ERROR_LENGTH_INSUFFICIENT = 1;
    public static final int SUMMARY_ERROR_DISK_SPACE_EXCEEDED = 2;
    public static final int SUMMARY_ERROR_INTERRUPTED = 3;
}
