/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * This file is originated from Android Open Source Project,
 * platform/packages/apps/Gallery2.git /src/com/android/gallery3d/util/GifView.java
 */

package com.oma.drm.gif;

import com.oma.drm.R;
import android.app.Activity;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageView;
import android.widget.LinearLayout;

public class ViewGifImage extends Activity {
    private static final String TAG = "OmaDrm:ViewGifImage";

    public static DisplayMetrics mDM;

    private ImageView mGifView;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.view_gif_image);
        mDM = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(mDM);
        Uri gifUri = getIntent().getData();
        showGifPicture(gifUri);
    }

    @Override
    protected void onStop() {
        super.onStop();
        finish();
    }

    @Override
    protected void onDestroy() {
        if (mGifView != null && mGifView instanceof GIFView) {
            ((GIFView) mGifView).freeMemory();
            mGifView = null;
        }
        super.onDestroy();
    }

    private void showGifPicture(Uri uri) {
        mGifView = new GIFView(this);
        ((LinearLayout) findViewById(R.id.image_absoluteLayout)).addView(
                mGifView, new LayoutParams(LayoutParams.MATCH_PARENT,
                        LayoutParams.MATCH_PARENT));
        if (((GIFView) mGifView).setDrawable(uri))
            return;

        finish();

    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        getWindowManager().getDefaultDisplay().getMetrics(mDM);
        super.onConfigurationChanged(newConfig);
    }
}
