/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.sim;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

import com.qualcomm.qti.setupwizard.data.Page;
import com.qualcomm.qti.setupwizard.data.SetupDataCallbacks;
import com.qualcomm.qti.setupwizard.R;

public class SetupSimPage extends Page {

    private Context mContext;

    public SetupSimPage(Context context, SetupDataCallbacks callbacks, int pageResourceId) {
        super(context, callbacks, pageResourceId);
        mContext = context;
    }

    @Override
    public Fragment createFragment() {
        Bundle args = new Bundle();
        args.putString(Page.KEY_PAGE_ARGUMENT, getKey());

        SetupSimFragment fragment = new SetupSimFragment();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public int getSetupBGResId() {
        if (getActiveSimCount() != 0) {
            return R.drawable.ic_sim_head;
        } else {
            return R.drawable.ic_no_sim_head;
        }
    }

    @Override
    public int getSetupTitleResId() {
        if (getActiveSimCount() != 0) {
            return R.string.setup_sim_title;
        } else {
            return R.string.setup_no_sim_title;
        }
    }

    @Override
    public int getNextButtonResId() {
        if (getActiveSimCount() != 0) {
            return R.string.setup_next;
        } else {
            return R.string.setup_skip;
        }
    }

    public int getActiveSimCount() {
        SubscriptionManager mSubscriptionManager =
                SubscriptionManager.from(mContext);
        TelephonyManager tm =
                (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
        int simCount = tm.getSimCount();
        int activeSimCount = 2;
        for (int i = 0; i < simCount; ++i) {
            final SubscriptionInfo sir = mSubscriptionManager
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            if (sir == null) {
                activeSimCount--;
            }
        }
        return activeSimCount;
    }
}
