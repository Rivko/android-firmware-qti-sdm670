/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;

import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.utils.Helper;

public class BaseActivity extends AppCompatActivity {

    Toolbar toolbar;

    Toolbar getToolbar() {
        if( toolbar == null ) {
            toolbar = (Toolbar) this.findViewById(R.id.toolbar);
            if( toolbar != null ) {
                toolbar.setPadding(0, Helper.getNotificationBarHeight(this), 0, 0);
                this.setSupportActionBar(toolbar);
            }
        }

        return toolbar;
    }
}
