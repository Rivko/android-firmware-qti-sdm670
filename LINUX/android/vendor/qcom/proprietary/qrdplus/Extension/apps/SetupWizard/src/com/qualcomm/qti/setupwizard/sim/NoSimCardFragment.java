/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.setupwizard.sim;

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;

import com.qualcomm.qti.setupwizard.R;
import com.qualcomm.qti.setupwizard.ui.FragmentCallbacks;
import com.qualcomm.qti.setupwizard.ui.SimProgressDialog;

public class NoSimCardFragment extends Fragment {

    private static final String TAG = "NoSimCardFragment";
    private static final String NO_SIM_CARD = "no_sim_card";
    private static final int SIM_SINGLE = 1;
    private static final int SIM_DOUBLE = 2;
    private static final int EVT_PROGRESS_DLG_TIME_OUT = 1;
    private static final int PROGRESS_DLG_TIME_OUT = 3000;
    private FragmentManager mFragmentManager;
    private FragmentCallbacks mCallbacks;
    private Button mTryAgain;
    private SimProgressDialog mSimProgressDialog = null;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.no_sim_card, container, false);
        onBackLabel(NO_SIM_CARD);
        mFragmentManager = getActivity().getFragmentManager();
        mTryAgain = (Button) view.findViewById(R.id.try_again);
        mTryAgain.setOnClickListener(tryAgainListener);
        return view;
    }

    public OnClickListener tryAgainListener = new OnClickListener() {
        @Override
        public void onClick(View v) {
            startProgressDialog();
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Log.e(TAG, "msg.what = " + msg.what);
            switch (msg.what) {
                case EVT_PROGRESS_DLG_TIME_OUT:
                    stopProgressDialog();
                    onCheckSimCount();
                    break;
                default:
                    break;
            }
        }
    };

    private void startProgressDialog() {
        if (mSimProgressDialog == null) {
            mSimProgressDialog = SimProgressDialog.createProgressDialog(getActivity());
            mSimProgressDialog.show();
        }
        sendMessage(EVT_PROGRESS_DLG_TIME_OUT, mHandler, PROGRESS_DLG_TIME_OUT);
    }

    private void stopProgressDialog() {
        if (mSimProgressDialog != null) {
            mSimProgressDialog.dismiss();
            mSimProgressDialog = null;
        }
    }

    private void sendMessage(int event, Handler handler, int delay) {
        Message message = handler.obtainMessage(event);
        handler.sendMessageDelayed(message, delay);
    }

    private void onCheckSimCount() {
        updateRes(getActiveSimCount());
        if (getActiveSimCount() == SIM_SINGLE) {
            startNextPage();
        } else if (getActiveSimCount() == SIM_DOUBLE) {
            mFragmentManager.beginTransaction()
                    .replace(R.id.base_sim, new SimSettingFragment())
                    .commit();
        }
    }

    public int getActiveSimCount() {
        SubscriptionManager mSubscriptionManager =
                SubscriptionManager.from(getActivity());
        TelephonyManager tm =
                (TelephonyManager) getActivity().getSystemService(Context.TELEPHONY_SERVICE);
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

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof FragmentCallbacks) {
            mCallbacks = (FragmentCallbacks) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mCallbacks = null;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onBackLabel(String currentPage) {
        mCallbacks.onFragmentInteraction(currentPage);
    }

    public void startNextPage() {
        mCallbacks.skipToNextPage();
    }

    public void updateRes(int count) {
        mCallbacks.updateSetupRes(count);
    }
}
