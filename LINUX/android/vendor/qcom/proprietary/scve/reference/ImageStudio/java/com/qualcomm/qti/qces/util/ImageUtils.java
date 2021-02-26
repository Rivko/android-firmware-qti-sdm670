/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.util;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.ContentResolver;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.RectF;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Environment;
import android.os.ParcelFileDescriptor;
import android.util.Log;

public class ImageUtils {
    private static final long MAX_IMAGE_SIZE = 2000000l; // 2 Megapixel max
    private static final String PREFIX = "tempimg";
    private static final String TAG = ImageUtils.class.getSimpleName();

    public static Bitmap loadImage(Context context, Uri uri, boolean isFullMP)
            throws FileNotFoundException {
        ContentResolver cr = context.getContentResolver();
        ParcelFileDescriptor parcelFileDescriptor = cr.openFileDescriptor(uri, "r");
        FileDescriptor fileDescriptor = parcelFileDescriptor.getFileDescriptor();
        Bitmap bitmap = BitmapFactory.decodeFileDescriptor(fileDescriptor);
        return createScaledBitmap(bitmap, isFullMP, GalleryHelper.getImagePathFromUri(context, uri));
    }

    public static Bitmap loadImage(Context context, Uri uri, InputStream input, boolean isFullMP) {
        Bitmap bitmap = BitmapFactory.decodeStream(input);
        return createScaledBitmap(bitmap, isFullMP, GalleryHelper.getImagePathFromUri(context, uri));
    }

    private static Bitmap createScaledBitmap(Bitmap bitmap, boolean isFullMP, String path) {
        int rotation = ImageUtils.getCameraImageOrientation(path);
        if (rotation != 0) {
            Matrix rotMatrix = new Matrix();
            rotMatrix.setRotate(rotation);
            Bitmap tempBitmap = bitmap;
            bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(),
                    rotMatrix, true);
            if (tempBitmap != null && !tempBitmap.isRecycled()) {
                tempBitmap.recycle();
                tempBitmap = null;
            }
        }
        Log.i(TAG, "createScaledBitmap : isFull MP = " + isFullMP);
        Log.i(TAG, "Bitmap size before W:" + bitmap.getWidth() + "  H:" + bitmap.getHeight());
        if (!isFullMP) {
            bitmap = createScaledBitmap(bitmap);
        }
        Log.i(TAG, "Bitmap size after W:" + bitmap.getWidth() + "  H:" + bitmap.getHeight());
        return bitmap;
    }

    private static Bitmap createScaledBitmap(Bitmap bitmap) {
        int newWidth, newHeight;
        int origWidth = bitmap.getWidth();
        int origHeight = bitmap.getHeight();
        long originalImageSize = origWidth * origHeight;
        Bitmap b;
        if (originalImageSize > MAX_IMAGE_SIZE) {
            /*
             * calculating new Height & width to maintain aspect ratio
             */
            newHeight = (int)Math.sqrt((MAX_IMAGE_SIZE / (double)origWidth) * origHeight);
            newWidth = (int)(MAX_IMAGE_SIZE / (double)newHeight);

            b = Bitmap.createScaledBitmap(bitmap, newWidth, newHeight, false);
            if (bitmap != null && !bitmap.isRecycled()) {
                bitmap.recycle();
                bitmap = null;
            }
        } else {
            /*
             * image is smaller than MAX_IMAGE_SIZE, so just return as is.
             */
            b = bitmap;
        }

        return b;
    }

    public static int getCameraImageOrientation(String path) {
        ExifInterface exif;
        int rotate = 0;
        if (path == null) {
            return rotate;
        }
        try {
            exif = new ExifInterface(path);
            int orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                    ExifInterface.ORIENTATION_NORMAL);
            switch (orientation) {
            case ExifInterface.ORIENTATION_ROTATE_270:
                rotate = 270;
                break;
            case ExifInterface.ORIENTATION_ROTATE_180:
                rotate = 180;
                break;
            case ExifInterface.ORIENTATION_ROTATE_90:
                rotate = 90;
                break;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return rotate;
    }

    /**
     * Clean up all temporary files at the given path. For this function to be useful, the path
     * should be the same one used for {@link #createTemporaryImageFile(File, String)} and
     * {@link #saveBitmapToTemporaryFile(File, Bitmap, CompressFormat, int)}
     * @param path Path at which to clean up all temporary files (anything created by this class)
     */
    public static void deleteAllTemporaryFiles(File path) {
        if (path == null) {
            return;
        }

        File[] files = path.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                return filename.startsWith(PREFIX);
            }
        });
        if (files != null && files.length > 0) {
            for (File file : files) {
                file.delete();
            }
        }
    }

    /**
     * Gets the bounding rect of the masked region within the original image.
     * @param mask A previously generated mask {@link Bitmap}
     * @return A {@link Rect} representing the minimum bounds that enclose the masked area
     * @see #getMask()
     */
    public static RectF generateMaskBoundingRect(Bitmap mask) {
        final int w = mask.getWidth();
        final int h = mask.getHeight();
        RectF boundingRect = new RectF(w, h, -1, -1); // bounds backwards and outside of image

        // Scan the image row by row, updating bounding rect as we go
        int[] row = new int[w];
        for (int y = 0; y < h; y++) {
            mask.getPixels(row, 0, w, 0, y, w, 1);

            // Scan left to find first set pixel, then scan right to find last set pixel
            int l, r;
            for (l = 0; l < w && row[l] == 0; l++)
                ;
            for (r = w - 1; r >= l && row[r] == 0; r--)
                ;

            if (l < boundingRect.left)
                boundingRect.left = l;
            if (r >= l && r > boundingRect.right)
                boundingRect.right = r;
            if (l < w) {
                if (y < boundingRect.top)
                    boundingRect.top = y;
                if (y > boundingRect.bottom)
                    boundingRect.bottom = y;
            }
        }

        // Rect currently is inclusive for right and bottom, but convention is that it
        // is exclusive. Fix up the rect
        boundingRect.right++;
        boundingRect.bottom++;

        // If rect.left == w, we did not find any points, return an empty rect
        if (boundingRect.left == w) {
            boundingRect.setEmpty();
        }

        return boundingRect;
    }

    public static void writeBitmapToPng(Bitmap bmap, String fileName) {
        String path = Environment.getExternalStorageDirectory().toString();
        OutputStream fOut = null;
        File file = new File(path, fileName);
        try {
            fOut = new FileOutputStream(file);
            bmap.compress(Bitmap.CompressFormat.PNG, 85, fOut);

            fOut.flush();
            fOut.close();
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
