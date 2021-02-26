/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies, Inc. Confidential and Proprietary.
 */
package com.qualcomm.qti.sta.engine;

/**
 * STA constant class
 */
public class JniConstants {

    public static final int STA_RESULT_SUCCESS = 0;
    public static final int STA_RESULT_IN_PROGRESS = 1;
    public static final int STA_RESULT_EMPTY = 2;
    public static final int STA_RESULT_TIMEOUT = 3;

    public static final int DATA_TYPE_NONE = 0x00;
    public static final int DATA_TYPE_EMAIL = 0x01;
    public static final int DATA_TYPE_URL = 0x02;
    public static final int DATA_TYPE_PHONENUMBER = 0x04;
    public static final int DATA_TYPE_ADDRESS = 0x08;
    public static final int DATA_TYPE_DATETIME = 0x10;
    public static final int DATA_TYPE_GENERAL = 0x20;
    public static final int DATA_TYPE_ALL = DATA_TYPE_EMAIL | DATA_TYPE_URL | DATA_TYPE_PHONENUMBER
            | DATA_TYPE_ADDRESS | DATA_TYPE_DATETIME | DATA_TYPE_GENERAL;

    public static final int PARSER_MODE_SNAPSHOT = 0;
    public static final int PARSER_MODE_PREVIEW = 1;

    public static final int IC_SCALE_HIGH_RESOLUTION = 0;
    public static final int IC_SCALE_LOW_RESOLUTION = 1;
}
