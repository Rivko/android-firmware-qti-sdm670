/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.moreprotect;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;

import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.data.Page;
import com.qualcomm.qti.setupwizard.data.SetupDataCallbacks;

public class SetupMoreProPage extends Page{

    public SetupMoreProPage(Context context, SetupDataCallbacks callbacks, int pageResourceId) {
        super(context, callbacks, pageResourceId);
    }

    @Override
    public Fragment createFragment() {
        Bundle args = new Bundle();
        args.putString(Page.KEY_PAGE_ARGUMENT, getKey());

        SetupMoreProFragment fragment = new SetupMoreProFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public int getSetupBGResId() {
        return R.drawable.ic_more_protection_head;
    }

    @Override
    public int getSetupTitleResId() {
        return R.string.setup_more_protection_title;
    }

    @Override
    public int getNextButtonResId() {
        return R.string.setup_skip;
    }
}
