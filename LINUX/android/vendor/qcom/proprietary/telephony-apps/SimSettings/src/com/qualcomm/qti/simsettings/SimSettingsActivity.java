/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.qualcomm.qti.simsettings;

import android.app.Activity;
import android.os.Bundle;

public class SimSettingsActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SimSettings simsettings = new SimSettings();
        simsettings.setArguments(getIntent().getExtras());
        getFragmentManager().beginTransaction().replace(
                android.R.id.content, simsettings, "simsettings").commit();
    }
}
