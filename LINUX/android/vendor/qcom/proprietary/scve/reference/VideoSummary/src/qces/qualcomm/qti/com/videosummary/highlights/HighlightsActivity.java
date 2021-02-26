/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

package qces.qualcomm.qti.com.videosummary.highlights;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import java.util.ArrayList;
import java.util.Calendar;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.FacesInbox.FacesInboxActivity;
import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.Notification.Notification;
import qces.qualcomm.qti.com.videosummary.PolicyManager.InitService;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.VideoPicker.VideoPickerActivity;
import qces.qualcomm.qti.com.videosummary.debug.DebugMainActivity;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;
import qces.qualcomm.qti.com.videosummary.settings.AboutActivity;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;
import qces.qualcomm.qti.com.videosummary.splash.SplashScreenActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.VidProcManagerService;


public class HighlightsActivity extends AppCompatActivity {

    private static final String TAG = HighlightsActivity.class.getSimpleName();
    public static HighlightsViewAdapter mAdapter;
    public static LinearLayoutManager llm;
    RecyclerView mRecyclerView;
    DefaultItemAnimator animator;
    static boolean inForeground = false;

    ContentObserver highlightsObserver = new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange, Uri uri) {
            Log.i(TAG, "onChange called by observer");
            mAdapter.loadHighlightInfo();
        }

        @Override
        public void onChange(boolean selfChange) {
            onChange(selfChange, null);
        }

        @Override
        public boolean deliverSelfNotifications() {
            return super.deliverSelfNotifications();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        showOneTimeSplash();

        setContentView(R.layout.activity_highlights);
        setTitle(R.string.title_activity_highlights);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        doFirstTimeOperations();

        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);
        mRecyclerView.setHasFixedSize(true);
        llm = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(llm);
        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mAdapter = new HighlightsViewAdapter(HighlightsActivity.this);
        mRecyclerView.setAdapter(mAdapter);

        animator = new DefaultItemAnimator();
        animator.setAddDuration(1000);
        animator.setChangeDuration(0);
        //animator.setRemoveDuration(1000);
        mRecyclerView.setItemAnimator(animator);

        //Swipe to dismiss
        ItemTouchHelper.Callback callback = new HighlightsViewTouchHelperCallback(mAdapter);
        ItemTouchHelper touchHelper = new ItemTouchHelper(callback);
        touchHelper.attachToRecyclerView(mRecyclerView);
    }

    public void showOneTimeSplash() {
        if (SettingsActivity.getPrefShowedOneTimeSplash(this) == false) {
            Intent intent = new Intent(this, SplashScreenActivity.class);
            startActivity(intent);
        }
    }

    void doFirstTimeOperations() {
        //start InitService which takes care of all 1st time background initializations
        InitService.startActionInit(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        inForeground = true;
        Notification.cancelNotifications(this);
        //Load summary video info in a thread to populate the adapters highlightVideos ArrayList
        new Thread(new Runnable() {
            @Override
            public void run() {
                mAdapter.loadHighlightInfo();
                mAdapter.deleteExpiredSummaryVideos();
            }
        }).start();
        getContentResolver().registerContentObserver(VideoSummaryContract.Highlights.CONTENT_URI, true, highlightsObserver);

        LocalBroadcastManager.getInstance(this).registerReceiver(mProgressReceiver,
                new IntentFilter(Constants.ACTION_PROGRESS_UPDATE_RECEIVED));
    }

    @Override
    protected void onPause() {
        super.onPause();
        inForeground = false;
        //TODO:there could be a possibility when this activity is paused (for some pop up window)
        //and at this time an insert or delete might happen into the database which wont reflect
        // into the adapter as we are unregistering the observer
        getContentResolver().unregisterContentObserver(highlightsObserver);

        // Unregister since the activity is about to be closed.
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mProgressReceiver);
    }

    @Override
    protected void onStop() {
        super.onStop();
        mAdapter.deleteSavedToGalleryCards();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_highlights, menu);
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != RESULT_OK) {
            //user hit cancel, so dont do anything
            return;
        }
        HighlightsModel.SummaryType summaryType = HighlightsModel.SummaryType.values()[requestCode];
        switch (summaryType) {
            case HIGHLIGHTS:
            case TIMELAPSE:
                ArrayList<MediaInfo> mediaInfoArrayList = data.getParcelableArrayListExtra(Constants.EXTRA_MEDIAINFO_ARRAYLIST);
                startVideoSummaryProcess(mediaInfoArrayList, summaryType);
                break;
            default:
                throw new UnsupportedOperationException("Mode " + summaryType.toString() + " not supported");
        }
    }

    public void startVideoSummaryProcess(ArrayList<MediaInfo> mediaInfoArrayList, HighlightsModel.SummaryType summaryType) {
        //Based on this timestamp, this summarized video will be uniquely identified
        long timeStampMillis = Calendar.getInstance().getTimeInMillis();
        HighlightsModel item = mAdapter.addCreatingMovieCard(summaryType, mediaInfoArrayList, timeStampMillis);
        VidProcManagerService.startActionProcess(this, item);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Intent intent;

        switch (item.getItemId()) {
            case R.id.action_create_highlights:
                intent = new Intent(this, VideoPickerActivity.class);
                startActivityForResult(intent, HighlightsModel.SummaryType.HIGHLIGHTS.ordinal());
                break;
            case R.id.action_create_timelapse:
                intent = new Intent(this, VideoPickerActivity.class);
                startActivityForResult(intent, HighlightsModel.SummaryType.TIMELAPSE.ordinal());
                break;
            case R.id.action_settings:
                intent = new Intent(this, SettingsActivity.class);
                startActivityForResult(intent, 0);
                break;
            case R.id.action_face_enroll:
                intent = new Intent(this, FacesInboxActivity.class);
                startActivityForResult(intent, 0);
                break;
            case R.id.action_about:
                intent = new Intent(this, AboutActivity.class);
                startActivityForResult(intent, 0);
                break;
            case R.id.action_debug:
                intent = new Intent(this, DebugMainActivity.class);
            startActivityForResult(intent, 0);
                break;
            default:
                return super.onOptionsItemSelected(item);
        }

        return true;
    }

    public static boolean isInForeground() {
        return inForeground;
    }


    // This receiver will receive scoring/assembly progress updates to be updates on cards
    private BroadcastReceiver mProgressReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            long timeStampMillis = intent.getLongExtra(Constants.EXTRA_TIME_STAMP_MILLIS, 0);
            int numUris = intent.getIntExtra(Constants.EXTRA_NUM_URIS, -1);
            int currentUriIndex = intent.getIntExtra(Constants.EXTRA_CURRENT_URI_INDEX, -1);
            int progress = intent.getIntExtra(Constants.EXTRA_HIGHLIGHTS_PROGRESS, 0);

            CreatingStatus status = new CreatingStatus(currentUriIndex, numUris, progress);
            mAdapter.updateProgress(timeStampMillis, status);
        }
    };


    /***
     * This static method should be called by the scoring service and/or assembly service by passing
     * the current progress which will be updated on to the CREATING card
     *
     * @param context
     * @param timeStampMillis
     * @param currentUriIndex
     * @param numUris
     * @param progress
     */
    public static void sendProgressUpdate(Context context, long timeStampMillis, int currentUriIndex, int numUris, int progress) {
        Log.i(TAG, "Status - currentIdx:" + currentUriIndex + " numUris=" + numUris + " progress:" + progress);
        if (timeStampMillis == 0) {
            //this update is for background scoring so do not send ui progress update
            return;
        }
        Intent intent = new Intent(Constants.ACTION_PROGRESS_UPDATE_RECEIVED);
        intent.putExtra(Constants.EXTRA_TIME_STAMP_MILLIS, timeStampMillis);
        intent.putExtra(Constants.EXTRA_CURRENT_URI_INDEX, currentUriIndex);
        intent.putExtra(Constants.EXTRA_NUM_URIS, numUris);
        intent.putExtra(Constants.EXTRA_HIGHLIGHTS_PROGRESS, progress);
        LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
    }
}
