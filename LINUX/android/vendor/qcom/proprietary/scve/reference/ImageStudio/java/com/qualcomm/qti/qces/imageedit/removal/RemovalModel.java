/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.removal;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Stack;

import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Cap;
import android.graphics.Paint.Join;
import android.graphics.Paint.Style;
import android.graphics.Path;
import android.graphics.Path.FillType;
import android.graphics.Rect;
import android.graphics.RectF;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import com.qualcomm.qti.qces.imageedit.removal.RemovalBitmapManager.BitmapSet;
import com.qualcomm.qti.qces.objectcloneremoveshared.TemporaryImageUtils;
import com.qualcomm.qti.qces.util.ImageUtils;

public class RemovalModel {

    private static final int MASK_FILL_SHAPE_ALPHA = 0xFF;
    private static final int MASK_FILL_BACKGROUND_ALPHA = 0x80;

    private static final String KEY_HISTORY = "com.qualcomm.qti.qces.imageedit.RemovalModel.HISTORY";
    private static final String KEY_CURRENT_IMAGE = "com.qualcomm.qti.qces.imageedit.RemovalModel.CURRENT_IMAGE";

    private static final String TAG = "ImgStudio";
    private RemovalActivity activity;

    private Stack<HistoryImageSegment> historyImages;

    public ObjectRemover mObjectRemover;
    public StrokeSize strokeSizeSelected;
    public boolean hintActive;
    public boolean hintShown;

    protected BitmapSet bitmapSet;
    private Rect removalRect;
    private int history_index = 0;

    public void onResume() {
       if (mObjectRemover == null) {
          Log.d(TAG, "RemovalModel onResume: reinstantiate mObjectRemover");
          mObjectRemover = new ObjectRemover(false, 14);
       }
       if (bitmapSet == null) {
          Log.d(TAG, "RemovalModel onResume: bitmapSet was null");
          loadImage(activity.removalUri);
       }
    }

    public enum StrokeSize {
        SMALL, MEDIUM, LARGE
    };

    static {
        try {
            System.loadLibrary("BitmapUtils");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load.\n" + e);
        }
    }

    public RemovalModel(RemovalActivity activity) {

       Log.d(TAG, "RemovalModel constructor");

        this.activity = (RemovalActivity)activity;

        removalRect = new Rect();

        strokeSizeSelected = StrokeSize.MEDIUM; // default size at first load

        mObjectRemover = new ObjectRemover(false, 14);

        historyImages = new Stack<HistoryImageSegment>();
    }

    void loadImage(Uri imageUri) {

        if (imageUri != null) {
            bitmapSet = initializeImageBitmap(imageUri, activity.isFullMP);
        }
    }

    void saveHistoryStack(Bundle outState, Stack<HistoryImageSegment> histStack) {

        String[] history = new String[histStack.size()];
        int i = 0;
        for (HistoryImageSegment his : histStack) {
            try {
                FileOutputStream os;
                File f1 = TemporaryImageUtils.createTemporaryImageFile(
                        activity.getExternalCacheDir(), ".jpg");
                os = new FileOutputStream(f1);
                os.write(his.regionPngDataBefore);
                os.close();
                File f2 = TemporaryImageUtils.createTemporaryImageFile(
                        activity.getExternalCacheDir(), ".jpg");
                os = new FileOutputStream(f2);
                os.write(his.regionPngDataAfter);
                os.close();

                history[i++] = f1.getPath() + File.pathSeparatorChar + f2.getPath() +
                        File.pathSeparatorChar + his.originX + File.pathSeparatorChar + his.originY;

            } catch (IOException e) {
                outState.remove(KEY_HISTORY);
                throw new RuntimeException();
            }
        }
        outState.putStringArray(KEY_HISTORY, history);

    }

    void saveState(Bundle outState) {
        outState.putBoolean("hintActive", hintActive);
        outState.putBoolean("hintShown", hintShown);
        outState.putSerializable("strokeSize", strokeSizeSelected);
        outState.putInt("historyIndex", history_index);

        File f;
        try {
            f = TemporaryImageUtils.saveBitmapToTemporaryFile(activity.getExternalCacheDir(),
                    activity.displayBitmap, CompressFormat.JPEG, 90);
            outState.putParcelable(KEY_CURRENT_IMAGE, Uri.fromFile(f));
        } catch (IOException e) {
            outState.remove(KEY_CURRENT_IMAGE);
            throw new RuntimeException();
        }

        saveHistoryStack(outState, historyImages);

    }

    void restoreHistoryStack(Bundle savedInstanceState) {

        for (String history : savedInstanceState.getStringArray(KEY_HISTORY)) {
            String[] split = history.split(":");

            try {
                File f1 = new File(split[0]);
                FileInputStream is1 = new FileInputStream(f1);
                byte[] historyPngDataBefore = new byte[(int)f1.length()];
                is1.read(historyPngDataBefore);
                is1.close();
                f1.delete();

                File f2 = new File(split[1]);
                FileInputStream is2 = new FileInputStream(f2);
                byte[] historyPngDataAfter = new byte[(int)f2.length()];
                is2.read(historyPngDataAfter);
                is2.close();
                f2.delete();

                HistoryImageSegment his = new HistoryImageSegment(Integer.valueOf(split[2]),
                        Integer.valueOf(split[3]), historyPngDataBefore, historyPngDataAfter);
                historyImages.add(his);
            } catch (IOException e) {
                Log.e(TAG, "Failed loading history segment: " + split[0], e);
            }
        }
    }

    void restoreState(Bundle savedInstanceState) {
       Log.d(TAG, "entered RemovalModel restoreState()");
       hintActive = savedInstanceState.getBoolean("hintActive");
       hintShown = savedInstanceState.getBoolean("hintShown");
       strokeSizeSelected = (StrokeSize)savedInstanceState.getSerializable("strokeSize");
       history_index = savedInstanceState.getInt("historyIndex");
       historyImages = new Stack<HistoryImageSegment>();

       activity.removalUri = savedInstanceState.getParcelable(KEY_CURRENT_IMAGE);
       restoreHistoryStack(savedInstanceState);


    }

    protected BitmapSet initializeImageBitmap(Uri imgUri, Boolean isFullMP) {
        bitmapSet = RemovalBitmapManager.getInstance(activity).acquire(imgUri, isFullMP);
        bitmapSet.mask.eraseColor(Color.TRANSPARENT);
        return bitmapSet;
    }

    Bitmap getImageBitmap() {
        return (bitmapSet != null ? bitmapSet.source : null);
    }

    Bitmap getMaskAlphaBitmap() {
        return (bitmapSet != null ? bitmapSet.mask : null);
    }

    public void releaseImage() {

        if (bitmapSet != null) {
            bitmapSet.release();
            bitmapSet = null;
        }
    }

    /**
     * Holds a single item of undo history
     */
    private static class HistoryImageSegment {
        public final int originX;
        public final int originY;
        public final byte[] regionPngDataBefore;
        public final byte[] regionPngDataAfter;

        public HistoryImageSegment(int x, int y, Bitmap regionBefore, Bitmap regionAfter) {
            originX = x;
            originY = y;

            ByteArrayOutputStream os1 = new ByteArrayOutputStream();
            ByteArrayOutputStream os2 = new ByteArrayOutputStream();

            regionBefore.compress(CompressFormat.JPEG, 90, os1);
            regionAfter.compress(CompressFormat.JPEG, 90, os2);

            regionPngDataBefore = os1.toByteArray();
            regionPngDataAfter = os2.toByteArray();

        }

        public HistoryImageSegment(int x, int y, byte[] regionPngDataBefore,
                byte[] regionPngDataAfter) {
            originX = x;
            originY = y;
            this.regionPngDataBefore = regionPngDataBefore;
            this.regionPngDataAfter = regionPngDataAfter;

        }

        public Bitmap getBitmapBefore() {
            return BitmapFactory
                    .decodeByteArray(regionPngDataBefore, 0, regionPngDataBefore.length);
        }

        public Bitmap getBitmapAfter() {
            return BitmapFactory.decodeByteArray(regionPngDataAfter, 0, regionPngDataAfter.length);
        }
    }

    private HistoryImageSegment createHistorySegment(Bitmap beforeBitmap, Bitmap afterBitmap,
            Rect boundingRect) {

        Bitmap beforeRegion = Bitmap.createBitmap(beforeBitmap, boundingRect.left,
                boundingRect.top, boundingRect.width(), boundingRect.height());
        Bitmap afterRegion = Bitmap.createBitmap(afterBitmap, boundingRect.left, boundingRect.top,
                boundingRect.width(), boundingRect.height());

        return new HistoryImageSegment(boundingRect.left, boundingRect.top, beforeRegion,
                afterRegion);

    }

    public void processImageRemoval(RemovalStroke stroke) {

        generateMask(stroke); // bitmapSet.mask is now rendered from stroke

        RectF boundingRect = ImageUtils.generateMaskBoundingRect(bitmapSet.mask);
        boundingRect.roundOut(removalRect);

        if (removalRect.isEmpty()) {
            // if the bounding rect was empty then this was an invalid stroke that should be
            // removed. This can happen if the stroke was done entirely off screen from the image in
            // the letterbox areas
            activity.deleteLastStroke();
            activity.hideSpinner();
        } else {
            activity.enableTouchMaskImageView(false);
            mObjectRemover.asyncRemove(new Handler(), activity.displayBitmap, bitmapSet.mask,
                    new ObjectRemover.OnProgressListener() {

                        @Override
                        public void onProgress(Bitmap progress) {
                            // this never seems to fire -mf
                            activity.setBitmapToImageView(progress);
                        }

                        @Override
                        public void onFinished(Bitmap finished) {
                            activity.clearStroke();
                            // first shorten the history stack for anything past this history index
                            while (historyImages.size() > history_index)
                                historyImages.pop();

                            // then add the new before and after history images to the stack
                            historyImages.add(createHistorySegment(activity.displayBitmap,
                                    finished, removalRect));
                            history_index++;

                            activity.setBitmapToImageView(finished);
                            bitmapSet.mask.eraseColor(Color.TRANSPARENT); // reset the mask
                            activity.hideSpinner();
                            refreshUndoRedoBtns();
                            activity.enableTouchMaskImageView(true);
                        }
                    });

        }
    }

    public void refreshUndoRedoBtns() {

        if (history_index == 0)
            activity.disableUndoBtn();
        else
            activity.enableUndoBtn();

        if (history_index == historyImages.size())
            activity.disableRedoBtn();
        else
            activity.enableRedoBtn();

    }

    public void getHistoryImageForUndo() {

        HistoryImageSegment hist = historyImages.get(history_index - 1);
        activity.drawBitmapOntoImageView(hist.originX, hist.originY, hist.getBitmapBefore());
        history_index--;
        refreshUndoRedoBtns();

    }

    public void getHistoryImageForRedo() {
        HistoryImageSegment hist = historyImages.get(history_index);
        activity.drawBitmapOntoImageView(hist.originX, hist.originY, hist.getBitmapAfter());
        history_index++;
        refreshUndoRedoBtns();

    }

    /**
     * Gets the mask {@link Bitmap} representing the masked region drawn by the user, after masking
     * mode (Outline or Scribble), stroke widths, and/or fill have been applied. The mask is filled
     * with {@link Color#WHITE}, and it's alpha set to either 0xFF (where the mask was drawn) or 0x0
     * (where the mask was not drawn)
     * @return a new {@link Bitmap} with the same dimensions as the source image, containing the
     *         output mask.
     */
    public void generateMask(RemovalStroke stroke) {

        Paint mPaint = new Paint();
        mPaint.setStrokeCap(Cap.ROUND);
        mPaint.setStrokeJoin(Join.ROUND);

        Path mPath = stroke.getPath();
        mPath.setFillType(FillType.WINDING);

        Paint paint = new Paint(mPaint);
        paint.setColor(Color.WHITE);
        paint.setStrokeWidth(stroke.getWidth() + 10); // always draw with the expanded thickness
                                                      // here

        Canvas c = new Canvas(bitmapSet.mask);

        renderPathToCanvas(c, paint, mPath, true);

    }

    /**
     * Helper function to render the current mask {@link Path} to the provided {@link Canvas}. This
     * is used both for rendering to the display, as well as to render the final path onto the
     * masked image
     * @param canvas {@link Canvas} on which to render
     * @param paint {@link Paint} with which to render the path. Note that the stroke style will be
     *        modified by this function
     * @param path {@link Path} to draw using paint on the provided canvas
     * @param fill true if the path should be filled, false otherwise
     */
    private static void renderPathToCanvas(Canvas canvas, Paint paint, Path path, boolean fill) {
        if (canvas == null) {
            throw new IllegalArgumentException("canvas must be specified");
        } else if (paint == null) {
            throw new IllegalArgumentException("paint must be specified");
        }

        if (path == null || path.isEmpty()) {
            return;
        }

        paint.setStyle(Style.STROKE);

        // When filling, render to an intermediate Bitmap for solid fill generation
        // before drawing to output canvas. This isn't done very often, so it's OK
        // not to cache the results.
        if (fill) {
            RectF boundsF = new RectF();
            path.computeBounds(boundsF, true);

            if (!boundsF.isEmpty()) {
                // Include the paint stroke in the path bounds
                final float halfStrokeWidth = paint.getStrokeWidth() / 2;
                boundsF.inset(-halfStrokeWidth, -halfStrokeWidth);
                Rect bounds = new Rect();
                boundsF.roundOut(bounds);

                // Create a mask layer large enough to hold the path bounds, and render the path to
                // it.
                // Then, flood-fill the resulting shape so that it is solid.
                Bitmap maskLayer = Bitmap.createBitmap(bounds.width(), bounds.height(),
                        Config.ALPHA_8);
                maskLayer.eraseColor((MASK_FILL_BACKGROUND_ALPHA << 24) | 0xFFFFFF);
                Canvas maskCanvas = new Canvas(maskLayer);
                maskCanvas.save();
                maskCanvas.translate(-bounds.left, -bounds.top); // N.B. Translate canvas to local
                                                                 // coord space
                maskCanvas.drawPath(path, paint);
                maskCanvas.restore();
                nFloodFillShape(maskLayer);

                // Render the mask layer to the provided canvas
                canvas.drawBitmap(maskLayer, bounds.left, bounds.top, paint);
                maskLayer.recycle();
            }
        } else {
            canvas.drawPath(path, paint);
        }
    }

    /**
     * This method flood-fills the interior of a shape, where the interior is defined as any part
     * that does not touch an edge of the {@link Bitmap}.
     * @param b A {@link Config#ALPHA_8} {@link Bitmap} containing the shape to fill. The shape
     *        should be drawn with {@link #MASK_FILL_SHAPE_ALPHA}, and the background should be
     *        drawn with {@link #MASK_FILL_BACKGROUND_ALPHA}.
     */
    private static native void nFloodFillShape(Bitmap b);
}
