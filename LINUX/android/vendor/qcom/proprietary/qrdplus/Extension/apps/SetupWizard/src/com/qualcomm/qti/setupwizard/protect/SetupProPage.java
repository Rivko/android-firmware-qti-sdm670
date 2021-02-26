/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.protect;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;

import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.data.Page;
import com.qualcomm.qti.setupwizard.data.SetupDataCallbacks;

public class SetupProPage extends Page {

    public SetupProPage(Context context, SetupDataCallbacks callbacks, int pageResourceId) {
        super(context, callbacks, pageResourceId);
    }

    @Override
    public Fragment createFragment() {
        Bundle args = new Bundle();
        args.putString(Page.KEY_PAGE_ARGUMENT, getKey());

        SetupProFragment fragment = new SetupProFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public int getSetupBGResId() {
        return R.drawable.ic_protect_head;
    }

    @Override
    public int getSetupTitleResId() {
        return R.string.setup_protect_title;
    }

    @Override
    public int getNextButtonResId() {
        return R.string.setup_skip;
    }
}
