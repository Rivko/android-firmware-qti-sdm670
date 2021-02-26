/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
package com.qualcomm.qti.smartassistant.service;

public final class Messages {
    // Service-client management
    public static final int REGISTER_CLIENT_REQUEST = 0x1001;
    public static final int UNREGISTER_CLIENT_REQUEST = 0x1002;
    public static final int START_RECOGNITION_REQUEST = 0x1003;
    public static final int STOP_RECOGNITION_REQUEST = 0x1004;

    public static final int START_VOICE_INPUT_RESPONSE = 0x2001;
    public static final int STOP_VOICE_INPUT_RESPONSE  = 0x2002;
    public static final int LOAD_HTML_URL_RESPONSE  = 0x2003;
    public static final int RENDER_VOICE_INPUT_TEXT_RESPONSE  = 0x2004;
    public static final int CALL_BY_CONTACTS_RESPONSE  = 0x2005;
    public static final int SEND_SMS_BY_CONTACTS_RESPONSE  = 0x2006;
    public static final int START_RECOGNITION_RESPONSE = 0x2007;
    public static final int STOP_RECOGNITION_RESPONSE = 0x2008;
}
