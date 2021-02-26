/* ==============================================================================
 * IWfdActionListener.aidl
 *
 * Copyright (c) 2012 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ============================================================================== */

package com.qualcomm.wfd.service;

import android.os.Bundle;

oneway interface IWfdActionListener {

	void onStateUpdate(int newState, int sessionId);

	void notifyEvent(int event, int sessionId);

	void notify(in Bundle b, int sessionId);

}
