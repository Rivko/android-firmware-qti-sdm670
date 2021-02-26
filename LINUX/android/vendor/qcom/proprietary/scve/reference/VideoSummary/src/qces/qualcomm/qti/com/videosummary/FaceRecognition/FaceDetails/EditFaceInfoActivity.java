/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.FaceDetails;

import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.ImageView;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.Utils.FRConstants;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.face.VideoSummaryFR;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;

public class EditFaceInfoActivity extends AppCompatActivity {
    private static final String TAG = EditFaceInfoActivity.class.getSimpleName();
    ImageView facePhoto;
    EditText nameEditText;

    byte[] frDatasetId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_face);
        setTitle("Edit Face Info");
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        facePhoto = (ImageView) findViewById(R.id.face_imageview);
        nameEditText = (EditText) findViewById(R.id.name_edit_text);

        DetectedFaceModel faceItem = getIntent().getParcelableExtra(FRConstants.EXTRA_FACE_INFO_ITEM);
        loadFaceInfo(faceItem);
    }

    void loadFaceInfo(DetectedFaceModel faceItem) {
        frDatasetId = faceItem.frDatasetId;
        facePhoto.setImageBitmap(faceItem.photo);
        nameEditText.setText(faceItem.name);
        nameEditText.setSelection(faceItem.name.length());

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_edit_face, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        if (id == R.id.action_delete_face) {
            showDeleteAlertDialog();
            return true;
        }
        if (id == R.id.action_done) {
            FRQueryHelper.updateEnrolledFace(getApplicationContext(), frDatasetId, nameEditText.getText().toString());
            finish();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    void deleteFace() {
        int res = FRQueryHelper.deleteFromFaceEnrollment(getApplicationContext(), frDatasetId);
        if (res == 0) {
            Log.e(TAG, "could not delete face from app db with id = " + frDatasetId);
        }
        VideoSummaryFR videoSummaryFRObj = new VideoSummaryFR(this, null);
        res = videoSummaryFRObj.removeFaceFromEnrollment(frDatasetId);
        if (res != 0) {
            Log.e(TAG, "delete face from FRDB failed with error code = " + res + "for dsid = " + frDatasetId);
        }
        videoSummaryFRObj.deinitFR();
        finish();
    }


    void showDeleteAlertDialog() {
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);

        alertDialogBuilder
                .setTitle("Delete this face?")
                .setCancelable(false)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        deleteFace();
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

}
