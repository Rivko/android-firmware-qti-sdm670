/*============================================================================
@file TabControlReceiver.java

@brief
Receives and processes TabControl related broadcast commands.

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.qsensortest;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import com.qualcomm.qti.sensors.core.qsensortest.QSensorContext;

public class TabControlReceiver extends BroadcastReceiver {
    public TabControlReceiver() {}

    @Override
    public void onReceive(Context context, Intent intent) {
        if(intent.getAction().contentEquals("com.qualcomm.qti.sensors.qsensortest.intent.SETTAB")) {
            CharSequence tabName = intent.getCharSequenceExtra("tab");
            for(int i = 0; i < TabControl.getActivity().getActionBar().getTabCount(); i++)
                if(0 == TabControl.getActivity().getActionBar().getTabAt(i).getText().toString().compareTo(tabName.toString()))
                    TabControl.getActivity().getActionBar().setSelectedNavigationItem(i);
        }
    }

    public IntentFilter getIntentFilter() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("com.qualcomm.qti.sensors.qsensortest.intent.SETTAB");
        Log.d(QSensorContext.TAG, "Available: com.qualcomm.qti.sensors.qsensortest.intent.SETTAB");
        intentFilter.addCategory("com.qualcomm.qti.sensors.qsensortest.intent.category.DEFAULT");
        return intentFilter;
    }
}
