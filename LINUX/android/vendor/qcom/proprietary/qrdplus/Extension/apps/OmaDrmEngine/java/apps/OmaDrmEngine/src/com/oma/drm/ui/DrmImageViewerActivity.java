/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

package com.oma.drm.ui;

import android.app.ActionBar;
import android.app.Activity;
import android.app.FragmentManager;
import android.app.WallpaperManager;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.Toolbar;

import com.oma.drm.R;
import com.oma.drm.OmaDrmInfo;
import com.oma.drm.DrmImage;
import com.oma.drm.gif.ViewGifImage;
import com.oma.drm.util.OmaDrmUtils;

public class DrmImageViewerActivity extends Activity implements OnClickListener {
    private static final String TAG = "OmaDrm:ImageViewerActivity";
    private static final boolean DEBUG = true;

    // Options menu items
    private static final int MENU_SET_AS_WALLPAPER = 1;
    private static final int MENU_DETAILS = 2;
    private static final String MIME_TYPE_GIF = "image/gif";
    private static final String WALLPAPER_CONTROLL_VISIBLE = "wallpaper_controll_visible";
    private static final String TAG_RATEINED_FRAGMENT = "tag_reatined_fragment";

    private ImageView mImageViewMain;
    private boolean mHideMenu;
    private ActionBar mActionBar;
    private static boolean sLeanBack;
    private Button mSetWallpaperButton;
    private Toolbar mToolBar;
    private Bitmap mBitmap;

    private Uri mUri;
    private String mMimeType;
    private boolean mGifImage = false;
    private DrmImage mDrmImage;
    private ImageButton mPlayImageButton;

    private RetainedFragment mRetainedFragment;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_image_viewer);

        mPlayImageButton = (ImageButton) findViewById(R.id.play_button);
        mImageViewMain = (ImageView) findViewById(R.id.image_view_main);
        mSetWallpaperButton = (Button) findViewById(R.id.set_wallpaper_button);
        mSetWallpaperButton.setOnClickListener(this);
        mToolBar = (Toolbar) findViewById(R.id.toolbar);

        setActionBar(mToolBar);
        mActionBar = getActionBar();

        mUri = getIntent().getData();
        mMimeType = getIntent().getType();

        if (mMimeType != null && mMimeType.endsWith(MIME_TYPE_GIF)) {
            mGifImage = true;
            mPlayImageButton.setVisibility(View.VISIBLE);
        } else {
            mPlayImageButton.setVisibility(View.GONE);
        }

        if (mUri == null) {
            Log.e(TAG,
                    "No Uri specified in the action Intent! finishing ImageViewerActivity.");
            finish();
            return;
        }

        mPlayImageButton.setOnClickListener(this);
        loadImage();
        if (savedInstanceState != null) {
            if (savedInstanceState.getBoolean(WALLPAPER_CONTROLL_VISIBLE)) {
                mSetWallpaperButton.setVisibility(View.VISIBLE);
                mActionBar.hide();
            }
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putBoolean(WALLPAPER_CONTROLL_VISIBLE,
                (mSetWallpaperButton.getVisibility() == View.VISIBLE));
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        if (mDrmImage != null && mDrmImage.getDrmInfo().isForwardLock()) {
            menu.add(0, MENU_SET_AS_WALLPAPER, 1,
                    R.string.menu_set_as_wallpaper);
        }
        menu.add(0, MENU_DETAILS, 3, R.string.menu_details);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case MENU_SET_AS_WALLPAPER:
            SetWallPaperControl();
            return true;

        case MENU_DETAILS:
            showDrmInfo(mUri);
            return true;

        default:
            return super.onOptionsItemSelected(item);
        }
    }

    private void showDrmInfo(Uri uri) {
        OmaDrmInfo info = mDrmImage.getDrmInfo();
        Intent intent = new Intent(this, DrmDetailsDialogActivity.class);
        intent.setData(uri);
        // This will not work under show from multiple places
        // intent.putExtra(DrmDetailsDialogActivity.KEY_DRM_INFO, info);
        startActivity(intent);
    }

    public void SetWallPaperControl() {
        mSetWallpaperButton.setVisibility(View.VISIBLE);
        mActionBar.hide();
    }

    private void setWallpaper() {
        WallpaperManager wallPaperManager = WallpaperManager
                .getInstance(getApplicationContext());
        try {
            Bitmap resized = OmaDrmUtils
                    .resizeImage(wallPaperManager.getDesiredMinimumHeight(),
                            wallPaperManager.getDesiredMinimumWidth(),
                            ((BitmapDrawable) mImageViewMain.getDrawable())
                                    .getBitmap(), this);
            wallPaperManager.setBitmap(resized);
        } catch (Exception e) {
            Log.d(TAG, "Not able to set wallpaper");
        }
    }

    @Override
    public void onBackPressed() {
        if (mSetWallpaperButton.getVisibility() == View.VISIBLE) {
            mSetWallpaperButton.setVisibility(View.GONE);
            mActionBar.show();
            return;
        }
        super.onBackPressed();
    }

    private void defaultView() {
        mActionBar.show();
        mSetWallpaperButton.setVisibility(View.GONE);
    }

    public void updateUI() {
        sLeanBack = !sLeanBack;
        View decorView = getWindow().getDecorView();
        ActionBar actionBar = getActionBar();
        if (sLeanBack) {
            hideSystemUi(decorView, actionBar);
        } else {
            ShowSystemUi(decorView, actionBar);
        }
    }

    private void ShowSystemUi(View decorView, ActionBar actionBar) {
        actionBar.show();
    }

    private void hideSystemUi(View decorView, ActionBar actionBar) {
        actionBar.hide();
    }

    private void loadImage() {
        if (mUri == null) {
            Log.e(TAG, "Image Uri not found! ");
            return;
        }
        // remove this on production, it is for test
        // dumpDecodedData();

        // Get data holder fragment and data
        FragmentManager fm = getFragmentManager();
        mRetainedFragment = (RetainedFragment) fm
                .findFragmentByTag(TAG_RATEINED_FRAGMENT);
        if (mRetainedFragment != null) {
            mDrmImage = mRetainedFragment.getDrmImage();
            if (mDrmImage == null) {
                if (mGifImage) {
                    mDrmImage = OmaDrmUtils.getThumbnailDrmImage(this, mUri,
                            mMimeType);
                } else {
                    mDrmImage = OmaDrmUtils.getDrmImage(this, mUri, mMimeType);
                }
            }
        } else {
            if (mGifImage) {
                mDrmImage = OmaDrmUtils.getThumbnailDrmImage(this, mUri,
                        mMimeType);
            } else {
                mDrmImage = OmaDrmUtils.getDrmImage(this, mUri, mMimeType);
            }
        }

        if (mDrmImage == null) {
            Toast.makeText(this, R.string.error_unable_to_view,
                    Toast.LENGTH_SHORT).show();
            Log.e(TAG, "Unable to fetch DRM image! Finishing " + TAG);
            finish();
            return;
        }

        mBitmap = mDrmImage.getBitmap();

        if (mBitmap != null) {
            mDrmImage.setWidth(mBitmap.getWidth());
            mDrmImage.setHeight(mBitmap.getHeight());
            // Bitmap bitmap = BitmapFactory.decodeFile(mDumpFilePath);
        }

        // Create data holder fragment and set data
        if (mRetainedFragment == null) {
            mRetainedFragment = new RetainedFragment();
            fm.beginTransaction().add(mRetainedFragment, TAG_RATEINED_FRAGMENT)
                    .commit();
            mRetainedFragment.setDrmImage(mDrmImage);
        }

        if (mBitmap == null) {
            Toast.makeText(this, R.string.error_unable_to_view,
                    Toast.LENGTH_SHORT).show();
            finish();
        }
        mImageViewMain.setImageBitmap(mBitmap);
    }

    private void viewAnimateGif(Uri uri) {
        Intent intent = new Intent(this, ViewGifImage.class);
        intent.setDataAndNormalize(uri);
        startActivity(intent);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
        case R.id.set_wallpaper_button:
            defaultView();
            new Thread(new Runnable() {

                @Override
                public void run() {
                    setWallpaper();
                }
            }).start();
            break;

        case R.id.play_button:
            viewAnimateGif(mUri);
            break;

        }
    }

}
