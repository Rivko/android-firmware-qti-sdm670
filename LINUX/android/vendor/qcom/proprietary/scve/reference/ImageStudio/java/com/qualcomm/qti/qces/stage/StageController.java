/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.stage;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Environment;
import android.support.v4.content.FileProvider;
import android.util.Log;
import android.widget.Toast;

import com.qualcomm.qti.qces.imageedit.effects.EffectsActivity;
import com.qualcomm.qti.qces.imageedit.objectclone.CloneActivity;
import com.qualcomm.qti.qces.imageedit.objectclone.CloneIntroFragmentActivity;
import com.qualcomm.qti.qces.imageedit.removal.RemovalActivity;
import com.qualcomm.qti.qces.imageedit.segmentation.HardCutActivity;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;
import com.qualcomm.qti.qces.util.ImageUtils;

public class StageController {

    private static final String TAG = StageController.class.getSimpleName();
    private static final String FILE_NAME = "segmentation_%s.jpg";

    private StageActivity activity;
    private StageView view;

    SharedPreferences prefs;
    private Intent hardCutActivityIntent;

    public StageController(StageActivity activity, StageView view) {
        this.activity = activity;
        this.view = view;

        prefs = activity.getSharedPreferences(CloneIntroFragmentActivity.prefName,
                Context.MODE_PRIVATE);

    }

    void initializeByImageUri(Uri stageImageUri) {
        view.showSpinner();
        new LoadInputBitmapTask().execute(stageImageUri);
    }

    private class LoadInputBitmapTask extends AsyncTask<Uri, Void, Void> {
        private Bitmap stageBitmap;

        @Override
        protected Void doInBackground(Uri... params) {

            try {
                stageBitmap = ImageUtils.loadImage(activity, params[0], activity.isFullMP);
            } catch (FileNotFoundException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            if (stageBitmap != null) {
                view.setStageBitmap(stageBitmap);
                view.hideSpinner();

                /*
                 * After loading image need to initialize content size of image for scaling and
                 * translation
                 */
                view.updateContentSize();
            } else {
                Log.e(TAG, "Failed to load stage Bitmaps");
                // TODO: handle failure gracefully
            }
        }
    }

    void shareClicked(Uri stageUri) {
        new SaveTempImageTask().execute(view.getStageBitmap());
    }

    void saveClicked() {
        new SaveImageTask().execute(view.getStageBitmap());
    }

    void cloneDialogFinished(Uri stageUri) {
        startHardCutActivity(stageUri);
    }

    void startHardCutActivity(Uri stageUri) {
        hardCutActivityIntent = new Intent(activity.getApplicationContext(), HardCutActivity.class);
        hardCutActivityIntent.putExtra(ImageSharingIntentExtras.NEXT_ACTIVITY,
                CloneActivity.class.getSimpleName());
        hardCutActivityIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                stageUri.toString());
        hardCutActivityIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);

        activity.startActivity(hardCutActivityIntent);
    }

    void cloneClicked(Uri stageUri) {

        boolean dontShowAutomatically = prefs.getBoolean(
                CloneIntroFragmentActivity.dontShowAutomatically, true);

        Intent receivedIntent = activity.getIntent();
        String maskUriString = receivedIntent.getStringExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI);

        if (maskUriString != null) {
            //maskUri exists so we've already done segmentation+clone, therefore launch Clone again
            String objectUriString = receivedIntent.getStringExtra(ImageSharingIntentExtras.EXTRA_OBJECT_IMAGE_URI);
            String backgroundUriString = receivedIntent.getStringExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI);

            Intent cloneIntent = new Intent(activity.getApplicationContext(), CloneActivity.class);
            cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI, maskUriString);
            cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI, objectUriString);
            cloneIntent.putExtra(ImageSharingIntentExtras.EXTRA_OBJECT_IMAGE_URI, objectUriString);
            cloneIntent.putExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI, backgroundUriString);
            cloneIntent.putExtra(ImageSharingIntentExtras.CLONE_MATRIX, receivedIntent.getFloatArrayExtra(ImageSharingIntentExtras.CLONE_MATRIX));

            cloneIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
            activity.startActivity(cloneIntent);
        }else if (!dontShowAutomatically) {
            Intent intent = new Intent(activity, CloneIntroFragmentActivity.class);
            activity.startActivityForResult(intent, R.integer.clone_intro_code);
        } else {
            startHardCutActivity(stageUri);
        }

    }

    void removeClicked(Uri stageUri) {
        Intent removeIntent = new Intent(activity.getApplicationContext(), RemovalActivity.class);
        removeIntent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI, stageUri.toString());
        removeIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
        activity.startActivity(removeIntent);
    }

    void effectsClicked(Uri stageUri) {
        Intent effectsIntent = new Intent(activity.getApplicationContext(), EffectsActivity.class);

        String backgroundUriString = activity.getIntent().getStringExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI);

        if (backgroundUriString != null) //if we have a background already from doing clone
        {
            effectsIntent.putExtra(ImageSharingIntentExtras.FOREGROUND_IMAGE_URI, activity.getIntent().getStringExtra(ImageSharingIntentExtras.FOREGROUND_IMAGE_URI));
            effectsIntent.putExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI, backgroundUriString);
        } else {
            effectsIntent.putExtra(ImageSharingIntentExtras.FOREGROUND_IMAGE_URI, stageUri.toString());
        }

        effectsIntent.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
        effectsIntent.putExtra("enable_help_popup", true);
        activity.startActivity(effectsIntent);
    }

    private class SaveImageTask extends AsyncTask<Bitmap, Void, File> {
        @Override
        protected File doInBackground(Bitmap... params) {
            File picturesPath = Environment
                    .getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
            SimpleDateFormat dateFormatter = new SimpleDateFormat("yyyy-MM-dd_HH-mm", Locale.US);
            String date = dateFormatter.format(new Date());
            File output = new File(picturesPath, String.format(Locale.US, FILE_NAME, date));
            if (output.exists()) {
                for (int i = 0; i < 100 && output.exists(); i++) {
                    output = new File(picturesPath, String.format(Locale.US, FILE_NAME, date + "_" +
                            i));
                }
            }

            if (output.exists()) {
                Log.e(TAG, "Failed to find an unused output filename");
                return null;
            }

            try {
                FileOutputStream outputStream = new FileOutputStream(output);
                params[0].compress(CompressFormat.JPEG, 80, outputStream);
                outputStream.close();
            } catch (IOException e) {
                Log.e(TAG, "Failed writing image to gallery", e);
                return null;
            }

            return output;
        }

        @Override
        protected void onPostExecute(File result) {
            if (result == null) {
                Log.e(TAG, "Failed to save effects image to gallery");
                // TODO: handle failure gracefully
                return;
            }

            Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.fromFile(result));
            activity.sendBroadcast(intent);

            Toast.makeText(activity, R.string.effects_image_saved, Toast.LENGTH_SHORT).show();

        }

    }

    private class SaveTempImageTask extends AsyncTask<Bitmap, Void, File> {

        @Override
        protected File doInBackground(Bitmap... params) {

            // use internal app file directory for this temporary file
            File output = new File(activity.getFilesDir(), "segmentation.jpg");

            try {
                FileOutputStream outputStream = new FileOutputStream(output);
                params[0].compress(CompressFormat.JPEG, 80, outputStream);
                outputStream.close();
            } catch (IOException e1) {
                // TODO Auto-generated catch block
                e1.printStackTrace();
            }

            return output;
        }

        @Override
        protected void onPostExecute(File result) {
            if (result == null) {
                Log.e(TAG, "Failed to save temp image for sharing");
                return;
            }

            Uri contentUri = null;

            try {
                contentUri = FileProvider.getUriForFile(activity,
                        "com.qualcomm.qti.qces.imageedit.softcut.fileprovider", result);
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Failed to generate content uri for sharing");
            }

            Intent shareIntent = new Intent();
            shareIntent.setAction(Intent.ACTION_SEND);
            shareIntent.putExtra(Intent.EXTRA_STREAM, contentUri);
            shareIntent.setType("image/jpeg");

            activity.startActivity(Intent.createChooser(shareIntent, "Share Image"));

        }

    }

}
