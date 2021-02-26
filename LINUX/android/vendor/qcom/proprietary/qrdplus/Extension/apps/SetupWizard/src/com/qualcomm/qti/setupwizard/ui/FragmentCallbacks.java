/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.ui;

public interface FragmentCallbacks {
    void onFragmentInteraction(String currentPage);
    void onFragmentInteraction(boolean isRequirePassword);
    void skipToNextPage();
    int getActiveSimCount();
    void isWifiConnected(boolean isConnected);
    void updateSetupRes(int count);
    void updateSetupRes(boolean available);
}
