/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.service.diag;

/**
 * Created by muluhe on 4/20/2017.
 */

public class DiagCommand {

    public static final int FTM_AP_TEST_APP_STATUS = 0;
    public static final int FTM_AP_SELECT_SEQUENCE = 1;
    public static final int FTM_AP_CLEAR_RESULTS = 2;
    public static final int FTM_AP_EXECUTE_SINGLE_TEST = 3;
    public static final int FTM_AP_EXECUTE_ALL_TESTS = 4;
    public static final int FTM_AP_READ_FILE = 5;
    public static final int FTM_AP_WRITE_FILE = 6;
    public static final int FTM_AP_ERASE_ALL_FILES = 7;
    public static final int FTM_AP_TEST_LIST_TO_FILE = 8;
    public static final int FTM_AP_DIR_TO_FILE = 9;
    public static final int FTM_AP_EXIT_TEST = 10;
    public static final int FTM_AP_UTILITY = 11;
    public static final int FTM_AP_APPEND_TO_RES = 12;
    public static final int FTM_AP_READ_APP_VERSION = 13;
    public static final int FTM_AP_SET_TOKEN = 14;

    public static final int FTM_SUBCMD_START = 100;
    public static final int FTM_SUBCMD_GET_FAIL_COUNT = FTM_SUBCMD_START;
    public static final int FTM_SUBCMD_GET_SEQUENCE_COUNT = 101;
    public static final int FTM_SUBCMD_END = 200;

    /**
     * For utility
     */
    public static final int OPS_OFF = 0;
    public static final int OPS_ON = 1;

    public static final int UTILITY_ID_AUTOSTART = 0;
    public static final int UTILITY_ID_WLAN = 1;
    public static final int UTILITY_ID_BT = 2;
    public static final int UTILITY_ID_CHARGER = 3;
    public static final int UTILITY_ID_NFC = 4;
    public static final int UTILITY_ID_SHIPMODE = 5;
}
