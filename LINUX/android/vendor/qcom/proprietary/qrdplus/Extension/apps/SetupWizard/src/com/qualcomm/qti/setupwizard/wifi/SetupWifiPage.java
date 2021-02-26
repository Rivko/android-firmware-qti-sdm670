/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.wifi;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;

import com.qualcomm.qti.setupwizard.data.Page;
import com.qualcomm.qti.setupwizard.data.SetupDataCallbacks;
import com.qualcomm.qti.setupwizard.R;

public class SetupWifiPage extends Page {

    public SetupWifiPage(Context context, SetupDataCallbacks callbacks, int pageResourceId) {
        super(context, callbacks, pageResourceId);
    }

    @Override
    public Fragment createFragment() {
        Bundle args = new Bundle();
        args.putString(Page.KEY_PAGE_ARGUMENT, getKey());

        WifiSettingFragment fragment = new WifiSettingFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public int getSetupBGResId() {
        return R.drawable.ic_wifi_head;
    }

    @Override
    public int getSetupTitleResId() {
        return R.string.setup_wifi_title;
    }

    @Override
    public int getNextButtonResId() {
        return R.string.setup_skip;
    }
}
