/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.home;

import java.io.File;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;

public class HomeController {
    private static final String TAG = HomeController.class.getSimpleName();
    private static final int SELECT_PICTURE = 1;
    private static final int REQUEST_CAMERA = 2;

    private HomeActivity activity;

    public HomeController(HomeActivity activity) {
        this.activity = activity;
    }

    void galleryClicked() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("image/*");
        activity.startActivityForResult(Intent.createChooser(intent, "Select Picture"),
                SELECT_PICTURE);
    }

    void cameraClicked() {
        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

        File f = new File(activity.getExternalCacheDir(), "tempimg_home.jpg");
        intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(f));
        activity.startActivityForResult(intent, REQUEST_CAMERA);

    }

    void handleOnActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == Activity.RESULT_OK) {
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
                File f = new File(activity.getExternalCacheDir().toString());
                for (File temp : f.listFiles()) {
                    if (temp.getName().equals("tempimg_home.jpg")) {
                        f = temp;
                        imageUri = Uri.fromFile(new File(f.getAbsolutePath()));
                        break;
                    }
                }

                break;
            }
            Intent nextActivity = new Intent(activity, com.qualcomm.qti.qces.stage.StageActivity.class);
            nextActivity.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,
                    imageUri.toString());
            nextActivity.putExtra(ImageSharingIntentExtras.FULL_MP_MODE, activity.isFullMP);
            nextActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            activity.startActivity(nextActivity);
            activity.finish();

        }
    }
}
