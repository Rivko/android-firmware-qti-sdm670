/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.biometrics.voiceprint.sdk;

import android.os.Bundle;

/**
 * The listener interface for receiving onResult events.
 * The class that is interested in processing a onResult
 * event implements this interface, and the object created
 * with that class is registered with a component using the
 * component's methods that support OnResult. When
 * the onResult event occurs, that object's appropriate
 * method is invoked.
 * <p>
 * Use this method for extending future messages. Messages
 * may have extra data in the bundle.
 */
public interface OnResultListener extends OnResultBase {

    public static final int SUCCESS = 0;

    /**
     * On result.
     *
     * @param code   the code
     * @param extras the extras
     */
    public void onResult(int code, Bundle extras);



}
