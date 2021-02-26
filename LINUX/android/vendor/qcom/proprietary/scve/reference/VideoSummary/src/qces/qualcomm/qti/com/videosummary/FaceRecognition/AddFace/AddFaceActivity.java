/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.AddFace;

import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.FacesInbox.FacesListViewAdapter;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.FacesInbox.RecyclerItemClickListener;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils.FRConstants;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils.FRUtils;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;

public class AddFaceActivity extends AppCompatActivity {

    private static final String TAG = AddFaceActivity.class.getSimpleName();
    public static LinearLayoutManager llm;
    public static FacesListViewAdapter mAdapter;
    RecyclerView mRecyclerView;
    DefaultItemAnimator animator;
    ImageView photoView;
    EditText editTextName;

    DetectedFaceModel item = null;

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
        setContentView(R.layout.activity_add_face);
        setTitle("Add Face");
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);


        photoView = (ImageView) findViewById(R.id.face_imageview);
        editTextName = (EditText) findViewById(R.id.name_edit_text);
        item = getIntent().getParcelableExtra(FRConstants.EXTRA_FACE_INFO_ITEM);

        photoView.setImageBitmap(item.photo);

        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);
        mRecyclerView.setHasFixedSize(true);

        llm = new LinearLayoutManager(this);
        mRecyclerView.setLayoutManager(llm);
        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mAdapter = new FacesListViewAdapter(AddFaceActivity.this);
        mRecyclerView.setAdapter(mAdapter);
        mRecyclerView.addOnItemTouchListener(
                new RecyclerItemClickListener(this, new RecyclerItemClickListener.OnItemClickListener() {
                    @Override
                    public void onItemClick(View view, int pos) {
                        Log.i(TAG, "assign to Recycler item clicked at pos " + pos);
                        DetectedFaceModel faceItem = mAdapter.getItem(pos);
                       /* Intent editFaceInfoActivityIntent = new Intent(getApplicationContext(), EditFaceInfoActivity.class);
                        editFaceInfoActivityIntent.putExtra(FRConstants.EXTRA_FACE_INFO_ITEM, faceItem);
                        startActivityForResult(editFaceInfoActivityIntent, 0);*/
                        showAssignToAlertDialog(faceItem);
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
        getMenuInflater().inflate(R.menu.menu_add_face, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_done:
            enrollFaceAsNew();
                break;
            default:
                return super.onOptionsItemSelected(item);
        }
            return true;
        }

    void enrollFaceAsNew() {
        String enteredName = editTextName.getText().toString();
        item.name = enteredName;
        item.frDatasetId = FRUtils.generateFRDatasetID(this);
        item.faceType = DetectedFaceModel.FaceType.KNOWN;
        returnResult(true);//isNewEnroll is true
    }


    void showAssignToAlertDialog(final DetectedFaceModel faceItem) {
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);

        alertDialogBuilder
                .setTitle("Are you sure you want to assign this face to " + faceItem.name + "?")
                .setCancelable(false)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        updateEnrolledFace(faceItem);
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });

        // create alert dialog and show
        AlertDialog alertDialog = alertDialogBuilder.create();
        alertDialog.show();
    }

    void updateEnrolledFace(DetectedFaceModel faceItem) {
        item.name = faceItem.name;
        item.frDatasetId = faceItem.frDatasetId;
        item.faceType = DetectedFaceModel.FaceType.KNOWN;
        returnResult(false);
    }

    void returnResult(boolean isNewEnroll) {
        Intent intent = new Intent();
        intent.putExtra(FRConstants.EXTRA_FACE_INFO_ITEM, item);
        intent.putExtra(FRConstants.EXTRA_TARGET_DETECTED_FACE_INDEX, getIntent().getIntExtra(FRConstants.EXTRA_TARGET_DETECTED_FACE_INDEX, -1));
        intent.putExtra(FRConstants.EXTRA_FACE_NEW_ENROLL, isNewEnroll);
        setResult(Activity.RESULT_OK, intent);
        finish();
    }


}
