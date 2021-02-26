/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva;

public final class MessageType {
	// Service-client management
	public static final int MSG_REGISTER_CLIENT = 1;
	public static final int MSG_UNREGISTER_CLIENT = 2;

    // New values
    public static final int MSG_LMCGETINSTANCE_FAILED = 0;
    public static final int MSG_REGISTER_SOUNDMODEL = 10;
    public static final int MSG_DEREGISTER_SOUNDMODEL = 11;
    public static final int MSG_EXTEND_SOUNDMODEL = 12;
    public static final int MSG_DETECT_SUCCEEDED = 13;
    public static final int MSG_DETECT_FAILED = 14;
    public static final int MSG_VERIFY_RECORDING = 15;
    public static final int MSG_RECORDING_RESULT = 16;
    public static final int MSG_LISTEN_RUNNING = 17;
    public static final int MSG_LISTEN_STOPPED = 18;
    public static final int MSG_CLOSE_VWUSESSION = 19;
    public static final int MSG_CLIENT_REGISTERED = 20;
    public static final int MSG_DELETE_FROM_SOUNDMODEL = 21;
    public static final int MSG_CREATE_MERGED_SM = 22;
    public static final int MSG_QUERY_SM = 23;
    public static final int MSG_QUERY_COMPLETED = 24;
    public static final int MSG_NEW_VOICEREQUEST = 25;
    public static final int MSG_STOP_TRAINING_RECORDING = 26;

    // ST actions
    public static final int MSG_ATTACH_MODULE = 49;
    public static final int MSG_LOAD_SM = 50;
    public static final int MSG_START_RECOGNITION = 51;
    public static final int MSG_STOP_RECOGNITION = 52;
    public static final int MSG_UNLOAD_SM = 53;
    public static final int MSG_QUERY_SM_STATE = 54;

    // For the SettingsActivity
    public static final int MSG_ENABLE = 100;
    public static final int MSG_DISABLE = 101;
    public static final int MSG_LISTEN_GET_PARAM = 102;
    public static final int MSG_LISTEN_SET_PARAM = 103;
    public static final int MSG_LISTEN_ENABLED = 104;
    public static final int MSG_LISTEN_DISABLED = 105;
    public static final int MSG_VOICEWAKEUP_GET_PARAM = 106;
    public static final int MSG_VOICEWAKEUP_SET_PARAM = 107;
    public static final int MSG_VOICEWAKEUP_ENABLED = 108;
    public static final int MSG_VOICEWAKEUP_DISABLED = 109;

    // Testing
    public static final int MSG_TEST_ONE = 1001;
    public static final int MSG_TEST_TWO = 1002;

    // COM Values
    public static final int MSG_LOAD_COM_SM = 500;
}
