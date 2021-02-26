/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * This file is originated from Android Open Source Project,
 * platform/packages/apps/Gallery2.git /src/com/android/gallery3d/util/GifView.java
 */

package com.oma.drm.gif;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.ContentResolver;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.ImageView;

import com.oma.drm.DrmImage;
import com.oma.drm.OmaDrmInputStream;
import com.oma.drm.util.OmaDrmUtils;

public class GIFView extends ImageView implements GifAction {

    private static final String TAG = "OmaDrm:GIFView";
    private static final float SCALE_LIMIT = 4;
    private static final long FRAME_DELAY = 200; // milliseconds

    private GifDecoder mGifDecoder = null;
    private Bitmap mCurrentImage = null;
    private DrawThread mDrawThread = null;

    private Uri mUri;
    private Context mContext;
    InputStream mInputStream = null;

    public GIFView(Context context) {
        super(context);
        mContext = context;
    }

    public boolean setDrawable(Uri uri) {
        if (null == uri) {
            return false;
        }
        mUri = uri;
        try {
            mInputStream = new OmaDrmInputStream(mContext, mUri, true);
        } catch (Exception e) {
            Log.e(TAG, "Finishing GIFView " + e);
            return false;
        }
        startDecode(mInputStream);
        return true;
    }

    private InputStream getInputStream(Uri uri) {
        ContentResolver cr = mContext.getContentResolver();
        InputStream input = null;
        try {
            input = cr.openInputStream(uri);
        } catch (IOException e) {
            Log.e(TAG, "catch exception:" + e);
        }
        return input;
    }

    private void startDecode(InputStream is) {
        freeGifDecoder();
        mGifDecoder = new GifDecoder(is, this);
        mGifDecoder.start();
    }

    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (mGifDecoder == null) {
            return;
        }

        if (mCurrentImage == null) {
            mCurrentImage = mGifDecoder.getImage();
        }
        if (mCurrentImage == null) {
            // if this gif can not be displayed, just try to show it as jpg by
            // parsing mUri
            DrmImage drmImage = OmaDrmUtils.getDrmImage(mContext, mUri, null);
            if (drmImage != null) {
                setImageBitmap(drmImage.getBitmap());
            }
            return;
        }
        setImageURI(null);
        int saveCount = canvas.getSaveCount();
        canvas.save();
        canvas.translate(getPaddingLeft(), getPaddingTop());
        Rect sRect = null;
        Rect dRect = null;

        int imageHeight = mCurrentImage.getHeight();
        int imageWidth = mCurrentImage.getWidth();

        int displayHeight = ViewGifImage.mDM.heightPixels;
        int displayWidth = ViewGifImage.mDM.widthPixels;

        int width, height;
        if (imageWidth >= displayWidth || imageHeight >= displayHeight) {
            // scale-down the image
            if (imageWidth * displayHeight > displayWidth * imageHeight) {
                width = displayWidth;
                height = (imageHeight * width) / imageWidth;
            } else {
                height = displayHeight;
                width = (imageWidth * height) / imageHeight;
            }
        } else {
            // scale-up the image
            float scale = Math.min(
                    SCALE_LIMIT,
                    Math.min(displayWidth / (float) imageWidth, displayHeight
                            / (float) imageHeight));
            width = (int) (imageWidth * scale);
            height = (int) (imageHeight * scale);
        }
        dRect = new Rect((displayWidth - width) / 2,
                (displayHeight - height) / 2, (displayWidth + width) / 2,
                (displayHeight + height) / 2);
        canvas.drawBitmap(mCurrentImage, sRect, dRect, null);
        canvas.restoreToCount(saveCount);
    }

    public void parseOk(boolean parseStatus, int frameIndex) {
        if (parseStatus) {
            // indicates the start of a new GIF
            if (mGifDecoder != null && frameIndex == -1
                    && mGifDecoder.getFrameCount() > 1) {
                if (mDrawThread != null) {
                    mDrawThread = null;
                }
                mDrawThread = new DrawThread();
                mDrawThread.start();
            }
        } else {
            Log.e(TAG, "parse error");
        }
    }

    private Handler mRedrawHandler = new Handler() {
        public void handleMessage(Message msg) {
            invalidate();
        }
    };

    private class DrawThread extends Thread {
        public void run() {
            if (mGifDecoder == null) {
                return;
            }

            while (true) {
                if (!isShown() || mRedrawHandler == null) {
                    break;
                }
                if (mGifDecoder == null) {
                    return;
                }
                GifFrame frame = mGifDecoder.next();
                mCurrentImage = frame.mImage;

                Message msg = mRedrawHandler.obtainMessage();
                mRedrawHandler.sendMessage(msg);
                try {
                    Thread.sleep(getDelay(frame));
                } catch (InterruptedException e) {
                    Log.e(TAG, "catch exception:" + e);
                }
            }
        }

    }

    private long getDelay(GifFrame frame) {
        // in milliseconds
        return frame.mDelayInMs == 0 ? FRAME_DELAY : frame.mDelayInMs;
    }

    private void freeGifDecoder() {
        if (mGifDecoder != null) {
            mGifDecoder.free();
            mGifDecoder = null;
        }
    }

    public void freeMemory() {
        if (mInputStream != null) {
            try {
                mInputStream.close();
            } catch (IOException e) {
                Log.e(TAG, e.getMessage());
            }
        }
        if (mDrawThread != null) {
            mDrawThread = null;
        }
        freeGifDecoder();
    }
}
