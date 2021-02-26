/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces;

import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.LinearLayout;

import java.util.List;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.AddFace.AddFaceActivity;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils.FRConstants;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.face.VideoSummaryFR;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;

public class DetectedFacesListActivity extends AppCompatActivity {

    private static final String TAG = DetectedFacesListActivity.class.getSimpleName();
    public static DetectedFacesListViewAdapter mAdapter;
    public static LinearLayoutManager llm;
    RecyclerView mRecyclerView;
    public static LinearLayout progressBarLinearLayout;
    DefaultItemAnimator animator;
    public final int ENROLL_FACE = 1;
    VideoSummaryFR videoSummaryFRObj = null;

    List<DetectedFaceModel> detectedFaceList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_detected_faces_list);
        setTitle("Faces Found");
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        progressBarLinearLayout = (LinearLayout) findViewById(R.id.progress_bar_linear_layout);

        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);
        mRecyclerView.setHasFixedSize(true);
        llm = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(llm);

        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mAdapter = new DetectedFacesListViewAdapter(DetectedFacesListActivity.this);
        mRecyclerView.setAdapter(mAdapter);

        animator = new DefaultItemAnimator();
        animator.setAddDuration(1000);
        animator.setChangeDuration(0);
        //animator.setRemoveDuration(1000);
        mRecyclerView.setItemAnimator(animator);

        doFaceRecognition();

    }

    private void detectFacesAndList() {
        String uriString = getIntent()
                .getStringExtra(FRConstants.EXTRA_TARGET_IMAGE_URI);
        if (uriString != null) {
            videoSummaryFRObj = new VideoSummaryFR(this, Uri.parse(uriString));
            detectedFaceList = videoSummaryFRObj.doFR();
            if (detectedFaceList == null || detectedFaceList.size() == 0) {
                // Toast.makeText(this, "No faces found!!", Toast.LENGTH_SHORT).show();
            }
            //Load Faces info in a thread to populate the adapters ArrayList
            //loadDetectedFaceList();
        } else {
            finish();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        //Load Faces info in a thread to populate the adapters ArrayList
        loadDetectedFaceList();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != RESULT_OK) {
            //user hit cancel, so dont do anything
            return;
        }

        Uri imageUri = null;
        switch (requestCode) {
            case ENROLL_FACE:
                //TODO: reload the faces list
                int pos = data.getIntExtra(FRConstants.EXTRA_TARGET_DETECTED_FACE_INDEX, -1);
                DetectedFaceModel item = data.getParcelableExtra(FRConstants.EXTRA_FACE_INFO_ITEM);
                boolean isNewEnroll = data.getBooleanExtra(FRConstants.EXTRA_FACE_NEW_ENROLL, false);
                if (pos != -1) {
                    enrollThisFace(pos, item, isNewEnroll);
                    //loadDetectedFaceList();
                }
                Log.i(TAG, "HomeController: image uri:" + imageUri);
                break;
        }

    }

    void loadDetectedFaceList() {
        //Load Faces info in a thread to populate the adapters ArrayList
        new Thread(new Runnable() {
            @Override
            public void run() {
                mAdapter.loadDetectedFaces(detectedFaceList);
            }
        }).start();
    }

    void enrollThisFace(int pos, DetectedFaceModel item, boolean isNewEnroll) {
        //if its a replacement(Assigne to selected) then first delete the entry in FRDB and in app db.
        if (!isNewEnroll) {
            int removeFaceRes = videoSummaryFRObj.removeFaceFromEnrollment(item.frDatasetId);
            if (removeFaceRes != 0) {
                Log.e(TAG, "removing enrolled face failed as the API returned result code " + removeFaceRes);
            }
            FRQueryHelper.deleteFromFaceEnrollment(this, item.frDatasetId);
        }

        //Adding the face to both the db's
        int result = videoSummaryFRObj.enrollFace(pos, item.frDatasetId);
        if (result != 0) {
            //Enroll api returned
            Log.e(TAG, "Enrollment failed as the enrollAPI returned result code " + result);
        } else {
            Log.i(TAG, "Enrollment success by the API");
            FRQueryHelper.insertIntoFaceEnrollment(this, item.frDatasetId, item.name, item.photo);
            detectedFaceList.remove(pos);

            detectedFaceList.add(pos, item);
            //loadDetectedFaceList();
        }
    }


    void addFace(int pos, DetectedFaceModel item) {
        Log.i(TAG, "enroll clicked on item " + item.frDatasetId);
        Intent intent = new Intent(getApplicationContext(), AddFaceActivity.class);
        intent.putExtra(FRConstants.EXTRA_FACE_INFO_ITEM, item);
        intent.putExtra(FRConstants.EXTRA_TARGET_DETECTED_FACE_INDEX, pos);
        startActivityForResult(intent, ENROLL_FACE);
    }


    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {
        if (progressBarLinearLayout.getVisibility() == View.VISIBLE) {
            //to consume all touch events when spinner is visible
            return true;
        }
        return super.dispatchTouchEvent(ev);
    }

    void showSpinner() {
        progressBarLinearLayout.setVisibility(View.VISIBLE);
    }

    public static void hideSpinner() {
        progressBarLinearLayout.setVisibility(View.GONE);
    }


    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Log.i(TAG, "onBackpressed");
        cleanup();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "onDestroy called");
        cleanup();
    }

    void cleanup() {
        if (videoSummaryFRObj != null) {
            videoSummaryFRObj.deinitFR();
        }
    }


    public void doFaceRecognition() {
        new FaceRecognitionBackgroundTask().execute();
    }

    private class FaceRecognitionBackgroundTask extends AsyncTask<Void, Void, Void> {

        public FaceRecognitionBackgroundTask() {

        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            showSpinner();

        }

        @Override
        protected Void doInBackground(Void... params) {
            detectFacesAndList();
            return null;
        }


        @Override
        protected void onPostExecute(Void result) {
            hideSpinner();
            loadDetectedFaceList();
        }
    }
}
