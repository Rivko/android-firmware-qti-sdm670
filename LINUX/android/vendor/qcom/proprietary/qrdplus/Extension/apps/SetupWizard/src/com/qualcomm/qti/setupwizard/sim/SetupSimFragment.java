/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.sim;

import android.content.Context;
import android.os.Bundle;
import android.app.Fragment;
import android.app.FragmentManager;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.qualcomm.qti.setupwizard.R;

public class SetupSimFragment extends Fragment {
    private FragmentManager mFragmentManager;
    private SimSettingFragment mSimSettingFragment;
    private NoSimCardFragment mNoSimCardFragment;
    private View mView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mFragmentManager = getActivity().getFragmentManager();
        initSimFragment();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        if (mView != null) {
            ViewGroup parent = (ViewGroup) mView.getParent();
            if (parent != null) {
                parent.removeView(mView);
            }
        } else {
            mView = inflater.inflate(R.layout.setup_sim, container, false);
        }
        return mView;
    }

    public void initSimFragment() {
        if (getActiveSimCount() != 0) {
            initSimSettingFragment();
        } else {
            initNoSimFragment();
        }
    }

    public int getActiveSimCount() {
        SubscriptionManager mSubscriptionManager =
                SubscriptionManager.from(getActivity());
        TelephonyManager tm =
                (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
        int simCount = tm.getSimCount();
        int activeSimCount;
        activeSimCount = simCount;
        for (int i = 0; i < simCount; ++i) {
            final SubscriptionInfo sir = mSubscriptionManager
                    .getActiveSubscriptionInfoForSimSlotIndex(i);
            if (sir == null) {
                activeSimCount --;
            }
        }
        return activeSimCount;
    }

    public void initSimSettingFragment() {
        if (mSimSettingFragment == null) {
            mSimSettingFragment = new SimSettingFragment();
        }
        mFragmentManager.beginTransaction()
                .replace(R.id.base_sim, mSimSettingFragment)
                .commit();
    }

    public void initNoSimFragment() {
        if (mNoSimCardFragment == null) {
            mNoSimCardFragment = new NoSimCardFragment();
        }
        mFragmentManager.beginTransaction()
                .replace(R.id.base_sim, mNoSimCardFragment)
                .commit();
    }
}
