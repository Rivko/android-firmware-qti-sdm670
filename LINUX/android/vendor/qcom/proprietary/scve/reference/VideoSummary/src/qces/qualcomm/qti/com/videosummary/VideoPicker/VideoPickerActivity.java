/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPicker;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import java.util.ArrayList;

import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.Constants;

public class VideoPickerActivity extends AppCompatActivity {

    private static final String TAG = VideoPickerActivity.class.getSimpleName();
    public static final String MULTI_SELECT = "multi_select";
    protected boolean multiSelect = true;
    private VideoGridRecyclerFragment pickerFragment;
    private Menu menu;
    public ArrayList<MediaInfo> mediaInfoArrayList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_videopicker);

        multiSelect = getIntent().getBooleanExtra(VideoPickerActivity.MULTI_SELECT, true);
        mediaInfoArrayList = getIntent().getParcelableArrayListExtra(Constants.EXTRA_MEDIAINFO_ARRAYLIST);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        if (savedInstanceState == null) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            pickerFragment = new VideoGridRecyclerFragment();
            transaction.replace(R.id.content_fragment, pickerFragment);
            transaction.commit();
        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_picker, menu);
        this.menu = menu;
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        int id = item.getItemId();
        if (id == R.id.action_done) {

            ArrayList<MediaInfo> selectedVideoInfo = new ArrayList<>();

            for (int i = pickerFragment.mAdapter.getItemCount() - 1; i >= 0; i--) {
                VideoItem video = pickerFragment.mAdapter.getVideoItem(i);
                if (video.selected)
                    selectedVideoInfo.add(video.info);
            }

            Intent data = new Intent();
            data.putParcelableArrayListExtra(Constants.EXTRA_MEDIAINFO_ARRAYLIST, selectedVideoInfo);
            setResult(RESULT_OK, data);
            finish();
            return true;
        } else if (id == android.R.id.home) {
            onBackPressed();
        }
        return super.onOptionsItemSelected(item);

    }

    public void performDone(MediaInfo info) {
        Intent data = new Intent();
        data.putExtra(Constants.EXTRA_MEDIAINFO, info);
        setResult(RESULT_OK, data);
        finish();
    }

    public void enableDoneBtn(boolean visible) {
        MenuItem item = menu.findItem(R.id.action_done);
        item.setVisible(visible);
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();//back will call finish internally
    }

    @Override
    public void finish() {
        super.finish();
        Log.i(TAG, "Status - finish, setting isActivityVisible to false");
        pickerFragment.isActivityVisible = false;
    }
}
