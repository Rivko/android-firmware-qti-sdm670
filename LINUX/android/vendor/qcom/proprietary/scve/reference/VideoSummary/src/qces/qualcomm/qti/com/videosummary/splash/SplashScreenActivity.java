/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.splash;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.settings.AboutActivity;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;

public class SplashScreenActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash_screen);
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
    }

    public void onOkClicked(View view) {
        SettingsActivity.setPrefShowedOneTimeSplash(this, true);

        //Turn on the settings switch to auto create highlights and timlapse
        SettingsActivity.setPrefCreateHighlightVideos(this, true);
        SettingsActivity.setPrefCreateTimelapseVideos(this, true);
        SettingsActivity.setAssemblyTimelineTableUpdateTimestamp(this, CalendarHelper.getInitialValForAssemblyTableUpdateTimestamp());

        finish();
    }

    public void onMoreInfoClicked(View view) {
        Intent intent = new Intent(this, AboutActivity.class);
        startActivity(intent);
    }

}
