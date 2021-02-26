/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.FacesInbox;

import android.content.Intent;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import java.io.File;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFacesListActivity;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.FaceDetails.EditFaceInfoActivity;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils.FRConstants;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;

public class FacesInboxActivity extends AppCompatActivity {

    private static final String TAG = FacesInboxActivity.class.getSimpleName();
    public static LinearLayoutManager llm;
    public static FacesListViewAdapter mAdapter;
    RecyclerView mRecyclerView;
    DefaultItemAnimator animator;

    private static final int SELECT_PICTURE = 1;
    private static final int REQUEST_CAMERA = 2;

    ContentObserver facesEnrollementObserver = new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange, Uri uri) {
            Log.i(TAG, "onChange called by FR observer");
            mAdapter.loadFacesEnrollementInfo();
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
        setContentView(R.layout.activity_faces_inbox);
        setTitle("Faces Inbox");
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);
        mRecyclerView.setHasFixedSize(true);
        llm = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(llm);
        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mAdapter = new FacesListViewAdapter(FacesInboxActivity.this);
        mRecyclerView.setAdapter(mAdapter);
        mRecyclerView.addOnItemTouchListener(
                new RecyclerItemClickListener(this, new RecyclerItemClickListener.OnItemClickListener() {
                    @Override
                    public void onItemClick(View view, int pos) {
                        Log.i(TAG, "Recycler item clicked at pos " + pos);
                        DetectedFaceModel faceItem = mAdapter.getItem(pos);
                        Intent editFaceInfoActivityIntent = new Intent(getApplicationContext(), EditFaceInfoActivity.class);
                        editFaceInfoActivityIntent.putExtra(FRConstants.EXTRA_FACE_INFO_ITEM, faceItem);
                        startActivityForResult(editFaceInfoActivityIntent, 0);
                    }
                })
        );

        animator = new DefaultItemAnimator();
        animator.setAddDuration(1000);
        animator.setChangeDuration(0);
        //animator.setRemoveDuration(1000);
        mRecyclerView.setItemAnimator(animator);
    }

    @Override
    protected void onResume() {
        super.onResume();

        //Load Faces info in a thread to populate the adapters ArrayList
        new Thread(new Runnable() {
            @Override
            public void run() {
                mAdapter.loadFacesEnrollementInfo();
            }
        }).start();
        getContentResolver().registerContentObserver(VideoSummaryContract.FaceEnrollment.CONTENT_URI, true, facesEnrollementObserver);
    }

    @Override
    protected void onPause() {
        super.onPause();

        //TODO:there could be a possibility when this activity is paused (for some pop up window)
        //and at this time an insert or delete might happen into the database which wont reflect
        // into the adapter as we are unregistering the observer
        getContentResolver().unregisterContentObserver(facesEnrollementObserver);

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_face_inbox, menu);
        return true;
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
            case SELECT_PICTURE:
                if (data == null) {
                    return;
                }
                imageUri = data.getData();
                Log.i(TAG, "HomeController: image uri:" + imageUri);
                break;

            case REQUEST_CAMERA:
                File f = new File(getExternalCacheDir().toString());
                for (File temp : f.listFiles()) {
                    if (temp.getName().equals("tempimg_home.jpg")) {
                        f = temp;
                        imageUri = Uri.fromFile(new File(f.getAbsolutePath()));
                        break;
                    }
                }
                break;
        }
        if (imageUri != null) {
            Intent detectedFacesIntent = new Intent(this, DetectedFacesListActivity.class);
            detectedFacesIntent.putExtra(FRConstants.EXTRA_TARGET_IMAGE_URI, imageUri.toString());
            startActivity(detectedFacesIntent);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        if (id == R.id.action_take_a_picture) {
            cameraClicked();
            return true;
        }
        if (id == R.id.action_choose_from_gallery) {
            galleryClicked();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    void galleryClicked() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("image/*");
        startActivityForResult(Intent.createChooser(intent, "Select Picture"),
                SELECT_PICTURE);
    }

    void cameraClicked() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
        File f = new File(getExternalCacheDir(), "tempimg_home.jpg");
        intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(f));
        startActivityForResult(intent, REQUEST_CAMERA);

    }

}
