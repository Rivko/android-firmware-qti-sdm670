/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import com.qualcomm.qti.sva.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;


public class Tutorial1Activity extends Activity {
    private final static String TAG = "ListenLog.Tutorial1Activity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_user_tutorial1);
    }

    public void BackOrNextClicked(View view) {
        Log.v(TAG, "BackOrNextClicked");
        if (view.getId() == R.id.bottombar_tv_back) {
            Log.v(TAG, "BackOrNextClicked- back was clicked.");
            finish();
        } else if (view.getId() == R.id.bottombar_tv_next) {
            Log.v(TAG, "BackOrNextClicked- next was clicked.");
            Intent intent = new Intent(Tutorial1Activity.this, Tutorial2Activity.class);
            startActivity(intent);
        } else {
            Log.e(TAG, "BackOrNextClicked- Unknown button called this function. " +
                    "Neither back nor next was clicked.");
        }
    }
}
