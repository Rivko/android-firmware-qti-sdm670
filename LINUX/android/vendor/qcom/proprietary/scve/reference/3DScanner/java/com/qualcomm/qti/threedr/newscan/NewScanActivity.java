/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.newscan;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;

import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;
import com.qualcomm.qti.threedr.modelbrowser.SettingsActivity;
import com.qualcomm.qti.threedr.tutorial.TutorialScreenActivity;

public class NewScanActivity extends AppCompatActivity {

    private static final String TAG = NewScanActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

         /* TODO:As per artie, since we have only human head scan and not the obj scan,
          * so New scan screen not required and hence we dont inflate its layout (commented below)
          * and instead directly start the tutorial screen if enabled or launch the 3d scanner
          * and call finish on this activity so that back press will not come back to NewScan
          * and instead wil go to Gallery
          */
      /*
        setContentView(R.layout.activity_new_scan);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        */
        /**
         * TODO: This is just for dev purpose and to be deleted in release
         * isDeveloperMode flag is set in the GalleryViewActivity.java by reading the meta-data
         * which is present in the AndroidManifest.xml, this is just to skip the redudant touches
         * by selecting the Head icon from NewScan Activity
         */
        /*
         if (GalleryViewActivity.isDeveloperMode) {
            scanHumanHeadClicked(null);
         }
         */
        scanHumanHeadClicked(null);
        finish();//when back is pressed in tutorial screen donot come back to NewScan activity

    }

    public void scanHumanHeadClicked(View view) {
        Log.i(TAG, "human head scan");
        //if dont show tutorial check box enabled then skip tutorial activity
        if (SettingsActivity.getPrefDontShowTutuorialCheckbox(this)) {
            BrowserUtils.Launch3DR(this);
        } else {
            Intent intent = new Intent(this, TutorialScreenActivity.class);
            startActivity(intent);
        }
    }

    public void scanObjectClicked(View view) {

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                break;
        }
        return true;
    }

}
