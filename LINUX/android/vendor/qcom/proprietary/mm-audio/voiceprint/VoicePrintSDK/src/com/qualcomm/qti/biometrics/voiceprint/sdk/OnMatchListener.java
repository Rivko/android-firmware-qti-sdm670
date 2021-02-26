/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.sdk;

import android.os.Bundle;

/**
 * The listener interface for receiving OnMatchListener events.
 * The class that is interested in processing a OnMatchListener
 * event implements this interface, and the object created
 * with that class is registered with a component using the
 * component's <code>match<code> or <code>matchAny<code> method. When
 * the OnMatchListener event occurs, that object's appropriate
 * method is invoked.
 */
public interface OnMatchListener extends OnResultBase {

    /**
     * The Constant DATA_USER_ID.
     */
    public static final String DATA_USER_ID = "DATA_USER_ID";

    /**
     * The Constant DATA_KEYPHRASE_ID.
     */
    public static final String DATA_KEYPHRASE_ID = "DATA_KEYPHRASE_ID";

    /**
     * The Constant FAILURE_CODE.
     */
    public static final String FAILURE_CODE = "FAILURE_CODE";

    /**
     * @param success Match Success
     * @param extra   Any extra information including error code if not successful.
     *                This can be null.
     * @brief Invoked when Match has completed and result processed.
     * @details This method is invoked the Match has completed.
     */
    public void onMatchStatus(boolean success, Bundle extra);

}