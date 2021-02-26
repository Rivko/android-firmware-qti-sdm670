/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Picture;
import android.graphics.Rect;
import android.graphics.drawable.Animatable;
import android.graphics.drawable.Drawable;
import android.media.Image;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.provider.ContactsContract;
import android.provider.MediaStore;
import android.text.SpannableStringBuilder;
import android.text.method.TransformationMethod;
import android.text.style.ImageSpan;
import android.util.Log;
import android.util.Size;
import android.util.TypedValue;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Toast;

import com.qualcomm.qti.fr_camera.coordinate.CoordinateSpace;
import com.qualcomm.qti.fr_camera.coordinate.CoordinateTranslator;
import com.qualcomm.qti.fr_camera.coordinate.Orientation;
import com.qualcomm.qti.fr_camera.fr.FacePreferences;
import com.qualcomm.qti.fr_camera.fr.UserDatabase;
import com.qualcomm.qti.fr_camera.imageutils.BitmapToYUV_420_888;
import com.qualcomm.qti.fr_camera.fr.Face;
import com.qualcomm.qti.fr_camera.fr.FaceRecognizer;
import com.qualcomm.qti.fr_camera.fr.FaceRecognizerException;
import com.qualcomm.qti.fr_camera.ui.FaceLayoutManager;
import com.qualcomm.qti.fr_camera.ui.MatchFontSizeImageSpan;

import java.io.IOException;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * This Activity displays a captured picture for review, and allows performing sharing actions on it
 */
public class PictureReviewActivity extends Activity {
    private static final String TAG = "SocialCamera";
    private static final int REQUEST_SELECT_CONTACT = 0x1000;

    private ImageView pictureImageView;
    private Button sendButton;

    private final ArrayDeque<AsyncTask> pendingTasks = new ArrayDeque<>();
    private Uri pictureUri;
    private UserDatabase userDatabase;
    private FaceLayoutManager faceLayoutManager;
    private TransformationMethod sendButtonCachedTransformationMethod;
    private Image yuvImage;
    private Face[] recognizedFaces;
    private Face[] recognizedFacesFromPreview;
    private Face enrollFace;
    private Size previewResolution;
    private Size pictureResolution;
    private boolean isPortrait;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        pictureUri = getIntent().getData();
        if (pictureUri == null) {
            Log.e(TAG, "Image URI is null, finishing picture review activity");
            finish();
            return;
        }
        new ImageConverterTask().execute(pictureUri);

        setContentView(R.layout.activity_picture_review);

        pictureImageView = (ImageView)findViewById(R.id.picture_review_image);
        pictureImageView.setImageURI(pictureUri);

        findViewById(R.id.picture_review_dismiss).setOnClickListener(dismissClickedListener);

        ArrayList<Face> faceList = getIntent().getParcelableArrayListExtra("Faces");
        if (faceList != null) {
            recognizedFacesFromPreview = faceList.toArray(new Face[faceList.size()]);
        }
        pictureResolution = new Size(
                getIntent().getIntExtra("PictureResolutionWidth", 0),
                getIntent().getIntExtra("PictureResolutionHeight", 0) );
        previewResolution = new Size(
                getIntent().getIntExtra("PreviewResolutionWidth", 0),
                getIntent().getIntExtra("PreviewResolutionHeight", 0) );
        isPortrait = getIntent().getBooleanExtra("IsPortrait", false);

        sendButton = (Button)findViewById(R.id.picture_review_send);
        sendButtonCachedTransformationMethod = sendButton.getTransformationMethod();
        sendButton.setOnClickListener(sendClickedListener);

        userDatabase = UserDatabase.getInstance(this);
        faceLayoutManager = new PictureReviewFaceLayoutManager(
                (FrameLayout)findViewById(R.id.picture_review_face_container));

        findViewById(R.id.loadingPanel).setVisibility(View.GONE);

        updateUi();
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (isFinishing()) {
            for (AsyncTask task : pendingTasks) {
                task.cancel(true);
            }
            pendingTasks.clear();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_SELECT_CONTACT:
                if (resultCode == Activity.RESULT_OK) {
                    List<String> uriSegments = data.getData().getPathSegments();
                    if (uriSegments.size() < 2) {
                        Log.e(TAG, "Expected at least 2 Uri path segments for contact");
                        break;
                    }
                    String contactLookupKey = uriSegments.get(uriSegments.size() - 2) + "/" +
                            uriSegments.get(uriSegments.size() - 1);
                    new EnrollFaceTask(contactLookupKey).execute(enrollFace);
                }
                break;

            default:
                super.onActivityResult(requestCode, resultCode, data);
                break;
        }
    }

    private void updateUi() {
        Resources res = getResources();

        Drawable dr = pictureImageView.getDrawable();
        final float scale = res.getDisplayMetrics().density;
        faceLayoutManager.setCoordinateTranslator(new CoordinateTranslator(
                new CoordinateSpace((int)(dr.getIntrinsicWidth() * scale),
                        (int)(dr.getIntrinsicHeight() * scale), Orientation.O_0),
                new CoordinateSpace(pictureImageView.getWidth(), pictureImageView.getHeight(),
                        Orientation.O_0)));
        faceLayoutManager.update(recognizedFaces);

        if (recognizedFaces == null) {
            Drawable progress = createIndeterminateProgressDrawable();
            progress.setCallback(indeterminateProgressDrawableAnimationCallback);
            MatchFontSizeImageSpan progressSpan = new MatchFontSizeImageSpan(progress,
                    ImageSpan.ALIGN_BOTTOM);
            ((Animatable)progress).start();

            sendButton.setTransformationMethod(null); // N.B. Necessary for spans to work
            sendButton.setEnabled(false);
            sendButton.setText(new SpannableStringBuilder().append("X", progressSpan, 0));
        } else {
            final int numSelectedUsers = faceLayoutManager.getNumSelectedUsers();
            sendButton.setTransformationMethod(sendButtonCachedTransformationMethod);
            sendButton.setEnabled(numSelectedUsers > 0);
            sendButton.setText(res.getString(R.string.picture_review_send_faces, numSelectedUsers));
        }
    }

    private Drawable createIndeterminateProgressDrawable() {
        TypedArray a = obtainStyledAttributes(new int[] { android.R.attr.progressBarStyleSmall });
        final int progressBarStyleId = a.getResourceId(0, 0);
        a.recycle();

        a = obtainStyledAttributes(progressBarStyleId, new int[] {
                android.R.attr.indeterminateDrawable });
        final int indeterminateProgressDrawableResourceId = a.getResourceId(0, 0);
        a.recycle();

        return getDrawable(indeterminateProgressDrawableResourceId);
    }

    private Drawable.Callback indeterminateProgressDrawableAnimationCallback =
            new Drawable.Callback() {
        private final Handler handler = new Handler(Looper.getMainLooper());

        @Override
        public void invalidateDrawable(Drawable who) {
            sendButton.invalidate();
        }

        @Override
        public void scheduleDrawable(Drawable who, Runnable what, long when) {
            if (who != null && what != null) {
                handler.postAtTime(what, when);
            }
        }

        @Override
        public void unscheduleDrawable(Drawable who, Runnable what) {
            if (who != null && what != null) {
                handler.removeCallbacks(what);
            }
        }
    };

    private View.OnClickListener dismissClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            finish();
        }
    };

    private View.OnClickListener sendClickedListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            SharePicture.send(PictureReviewActivity.this, pictureUri,
                    faceLayoutManager.getSelectedUsers());
            finish();
        }
    };

    private class ImageConverterTask extends AsyncTask<Uri, Void, Image> {
        @Override
        protected void onPreExecute() {
            pendingTasks.add(this);
            Log.v(TAG, "Queued ImageConverterTask");
        }

        @Override
        protected Image doInBackground(Uri... params) {
            if (params == null || params.length != 1 || params[0] == null) {
                Log.e(TAG, "No URI specified for face recognizer task");
                return null;
            }

            if (isCancelled()) {
                return null;
            }

            Bitmap b;
            try {
                b = MediaStore.Images.Media.getBitmap(getContentResolver(), params[0]);
            } catch (IOException e) {
                Log.e(TAG, "Failed loading bitmap from Uri [" + params[0] + "]", e);
                return null;
            }

            if (isCancelled()) {
                return null;
            }

            BitmapToYUV_420_888 converter = new BitmapToYUV_420_888(PictureReviewActivity.this);
            return converter.convert(b);
        }

        @Override
        protected void onPostExecute(Image image) {
            pendingTasks.remove(this);

            if (isCancelled()) {
                Log.w(TAG, "ImageConverterTask cancelled");
                return;
            }

            Log.v(TAG, "ImageConverterTask completed");

            if (image == null) {
                Toast.makeText(PictureReviewActivity.this,
                        R.string.picture_review_error_face_recognition, Toast.LENGTH_SHORT).show();
            } else {
                yuvImage = image;
                new FaceRecognizerTask().execute();
            }
        }
    }

    private class FaceRecognizerTask extends AsyncTask<Void, Void, Face[]> {
        @Override
        protected void onPreExecute() {
            pendingTasks.add(this);
            Log.v(TAG, "Queued FaceRecognizerTask");
            findViewById(R.id.loadingPanel).setVisibility(View.VISIBLE);
        }

        @Override
        protected Face[] doInBackground(Void... params) {
            Image.Plane y = yuvImage.getPlanes()[0];
            Face[] faces;
            FaceRecognizer.ScveMode scveMode = FaceRecognizer.ScveMode.DEFAULT;
            if (FacePreferences.isCPUOffloadEnabled(PictureReviewActivity.this)) {
                scveMode = FaceRecognizer.ScveMode.CPU_OFF_LOAD;
                Log.d(TAG, "CPU_OFF_LOAD mode");
            }
            try (
                FaceRecognizer recognizer = new FaceRecognizer(PictureReviewActivity.this,
                        scveMode, FaceRecognizer.FaceRecoMode.STILL_IMAGE,
                        FaceRecognizer.FrameFormat.GRAYSCALE,
                        yuvImage.getWidth(), yuvImage.getHeight(),
                        new int[] { y.getRowStride() }, 16)
            ) {
                faces = recognizer.detectFaces(y.getBuffer(), true);
            } catch (FaceRecognizerException e) {
                Log.e(TAG, "Error while recognizing faces", e);
                return null;
            }

            return faces;
        }

        @Override
        protected void onPostExecute(Face[] faces) {
            pendingTasks.remove(this);

            if (isCancelled()) {
                Log.w(TAG, "FaceRecognizerTask cancelled");
                return;
            }

            Log.v(TAG, "FaceRecognizerTask completed");

            if (faces == null) {
                Toast.makeText(PictureReviewActivity.this,
                        R.string.picture_review_error_face_recognition, Toast.LENGTH_SHORT).show();
            } else {
                Log.v(TAG, "Recognized " + faces.length + " faces");
                PictureReviewActivity.this.recognizedFaces = faces;
                if (faces.length == 0) {
                    addFacesFromPreview();
                }

                if (PictureReviewActivity.this.recognizedFaces.length == 0) {
                    Toast.makeText(PictureReviewActivity.this, R.string.picture_review_no_faces,
                            Toast.LENGTH_SHORT).show();
                }
            }

            findViewById(R.id.loadingPanel).setVisibility(View.GONE);
            updateUi();
        }
    }

    private void addFacesFromPreview() {
        if (recognizedFacesFromPreview == null) {
            return;
        }
        Log.d(TAG, "Recognized " + recognizedFacesFromPreview.length + " from preview");

        // translate faces from preview to picture review resolution
        translateFacesFromPreviewToPictureRes();

        for (int i = 0; i < recognizedFacesFromPreview.length; i++) {
            Face faceFromPreview = recognizedFacesFromPreview[i];

            if (!isFaceRecognizedInPicture(faceFromPreview)) {
                // there was a recognized face from preview that was not detected in picture mode

                // determine if faceFromPreview was also recognized in picture mode but with different ids
                // if there isn't a match based on coords then add faceFromPreview to recognized faces list
                if (!swapFaceBasedOnCoords(faceFromPreview)) {
                    Log.d(TAG, "Face was not recognized in picture but was in preview with id " + faceFromPreview.id);
                    int size = recognizedFaces.length;
                    recognizedFaces = Arrays.copyOf(recognizedFaces, size + 1);
                    recognizedFaces[size] = faceFromPreview;
                }
            }
        }
    }

    private final static int FACE_BOUNDS_RANGE = 450;

    private boolean swapFaceBasedOnCoords(Face faceFromPreview) {
        for (int i = 0; i < recognizedFaces.length; i++) {
            Face faceFromPicture = recognizedFaces[i];
            if (isFaceSameBasedOnCoords(faceFromPicture, faceFromPreview)) {
                // only swap the face if it is already recognized from preview
                if (!getHighestScoringId(faceFromPreview).isEmpty()) {
                    recognizedFaces[i] = faceFromPreview;
                    Log.d(TAG, "Face from picture with id " + faceFromPicture.id + " was swapped " +
                            "for face from preview with id " + faceFromPreview.id);
                }
                return true;
            }
        }
        return false;
    }

    private boolean isFaceSameBasedOnCoords(Face faceFromPicture, Face faceFromPreview) {
        Log.d(TAG, "faceFromPicture bounds: " + faceFromPicture.bounds.toString());
        Log.d(TAG, "faceFromPreview bounds: " + faceFromPreview.bounds.toString());

        int screenWidth = getResources().getDisplayMetrics().widthPixels;
        int screenHeight = getResources().getDisplayMetrics().heightPixels;

        float rangeWidthRatio = (float)yuvImage.getWidth()/(float)screenWidth;
        float rangeHeightRatio = (float)yuvImage.getHeight()/(float)screenHeight;

        Log.d(TAG, "screen res: " + screenWidth + " x " + screenHeight);
        Log.d(TAG, "width range: " + rangeWidthRatio * FACE_BOUNDS_RANGE + " height range: " + rangeHeightRatio * FACE_BOUNDS_RANGE);

        return ( (Math.abs(faceFromPicture.bounds.centerX() - faceFromPreview.bounds.centerX()) <= rangeWidthRatio * FACE_BOUNDS_RANGE) &&
                (Math.abs(faceFromPicture.bounds.centerY() - faceFromPreview.bounds.centerY()) <= rangeHeightRatio * FACE_BOUNDS_RANGE) );
    }

    private void translateFacesFromPreviewToPictureRes() {
        if (recognizedFacesFromPreview == null) {
            return;
        }

        int pictureReviewWidth = yuvImage.getWidth();//pictureResolution.getWidth();
        int pictureReviewHeight = yuvImage.getHeight();//pictureResolution.getHeight();

        int previewWidth = previewResolution.getWidth();
        int previewHeight = previewResolution.getHeight();
        if (isPortrait) {
            // swap them if in portrait
            previewWidth = previewResolution.getHeight();
            previewHeight = previewResolution.getWidth();
        }

        Log.d(TAG, "picture res " + pictureReviewWidth + " x " + pictureReviewHeight);
        Log.d(TAG, "preview res " + previewWidth + " x " + previewHeight);

        float widthRatio = (float)pictureReviewWidth/(float)previewWidth;
        float heightRatio = (float)pictureReviewHeight/(float)previewHeight;

        Log.d(TAG, "width ratio: " + widthRatio + "height ratio: " + heightRatio);

        for (int i = 0; i < recognizedFacesFromPreview.length; i++) {
            // left, right x widthRatio
            // top, bottom x heightRatio
            Rect rect = recognizedFacesFromPreview[i].bounds;
            Log.d(TAG, "bounds before " + rect.toString());
            rect.set((int)(rect.left * widthRatio),
                    (int)(rect.top * heightRatio),
                    (int)(rect.right * widthRatio),
                    (int)(rect.bottom * heightRatio));

            Log.d(TAG, "bounds after" + recognizedFacesFromPreview[i].bounds.toString());
        }
    }

    private boolean isFaceRecognizedInPicture(Face faceFromPreview) {
        String faceFromPreviewId = getHighestScoringId(faceFromPreview);
        if (faceFromPreviewId == null || faceFromPreviewId.isEmpty()) {
            return false;
        }
        for (int i = 0; i < recognizedFaces.length; i++) {
            Face faceFromPicture = recognizedFaces[i];
            if (getHighestScoringId(faceFromPicture).equals(faceFromPreviewId)) {
                return true;
            }
        }
        return false;
    }

    private String getHighestScoringId(Face face) {
        float highestScore = 0.0f;
        String id = "";
        for (int i = 0; i < face.recognitionResults.length; i++) {
            if (face.recognitionResults[i].score > highestScore) {
                id = face.recognitionResults[i].persistentId;
                highestScore = face.recognitionResults[i].score;
            }
        }
        return id;
    }

    private class EnrollFaceTask extends AsyncTask<Face, Void, Void> {
        private final String contactLookupId;
        private String id;
        private Face enrolledFace;

        private EnrollFaceTask(String contactLookupId) {
            this.contactLookupId = contactLookupId;
            id = userDatabase.findUserByContactLookupKey(contactLookupId);
        }

        @Override
        protected void onPreExecute() {
            pendingTasks.add(this);
            Log.v(TAG, "Queued EnrollFaceTask");
        }

        @Override
        protected Void doInBackground(Face... params) {
            if (params == null || params.length != 1 || params[0] == null) {
                Log.e(TAG, "No face to enroll");
                return null;
            }

            enrolledFace = params[0];

            if (isCancelled()) {
                return null;
            }

            Image.Plane y = yuvImage.getPlanes()[0];
            try (
                FaceRecognizer recognizer = new FaceRecognizer(PictureReviewActivity.this,
                        FaceRecognizer.ScveMode.DEFAULT, FaceRecognizer.FaceRecoMode.STILL_IMAGE,
                        FaceRecognizer.FrameFormat.GRAYSCALE,
                        yuvImage.getWidth(), yuvImage.getHeight(),
                        new int[] { y.getRowStride() }, 16)
            ) {
                if (id != null) {
                    recognizer.enrollFace(y.getBuffer(), params[0], id);
                } else {
                    id = recognizer.enrollFace(y.getBuffer(), params[0]);
                }
            } catch (FaceRecognizerException e) {
                Log.e(TAG, "Error while recognizing faces", e);
                return null;
            }

            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            pendingTasks.remove(this);

            if (isCancelled()) {
                Log.w(TAG, "EnrollFaceTask cancelled");
                return;
            }

            Log.v(TAG, "EnrollFaceTask completed");

            if (id == null || id.isEmpty()) {
                Toast.makeText(PictureReviewActivity.this,
                        R.string.picture_review_error_face_enroll, Toast.LENGTH_SHORT).show();
            } else {
                userDatabase.createUser(id, contactLookupId);
                new FaceRecognizerTask().execute(); // rerun recognition
            }
        }
    }

    private class PictureReviewFaceLayoutManager extends FaceLayoutManager {
        public PictureReviewFaceLayoutManager(FrameLayout layout) {
            super(layout, Mode.PICTURE_REVIEW, userDatabase);
        }

        @Override
        protected void enrollFace(Face face) {
            if (yuvImage == null) {
                throw new IllegalStateException("Attempting to enroll a face without a source " +
                        "image");
            }

            enrollFace = face;

            Intent intent = new Intent(Intent.ACTION_PICK, ContactsContract.Contacts.CONTENT_URI);
            intent.setType(ContactsContract.Contacts.CONTENT_TYPE);
            startActivityForResult(intent, REQUEST_SELECT_CONTACT);
        }

        @Override
        protected void onSelectedFacesChanged() {
            // calling this method caused an infinite loop and is not necessary for PictureReviewActivity
            //updateUi();
        }
    }
}
