/******************************************************************************
 * ---------------------------------------------------------------------------
 *   Copyright (c) 2015 Qualcomm Technologies, Inc.
 *   All Rights Reserved.
 *   Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *******************************************************************************/

package com.qualcomm.qti.qs;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class QSBroadcastReceiver extends BroadcastReceiver {
    private Context mContext;

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        Intent i = new Intent(mContext, QSService.class);
        mContext.startService(i);
    }
};
