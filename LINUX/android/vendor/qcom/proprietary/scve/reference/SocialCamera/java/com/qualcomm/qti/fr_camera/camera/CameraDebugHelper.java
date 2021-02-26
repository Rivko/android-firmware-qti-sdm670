/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.media.Image;
import android.media.ImageNV21;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileFilter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.Locale;
import java.util.Objects;
import java.util.TimeZone;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

/**
 * This class is a companion of {@link CameraActor}, which allows the individual preview and picture
 * frames to be saved to disk for debug purposes
 */
/*package*/ class CameraDebugHelper {
    private static final String TAG = "SocialCamera";
    private static final File DEBUG_IMAGES_ROOT =
            new File(Environment.getExternalStorageDirectory(), "camera_debug");
    private static final int MAX_NUM_IMAGE_DIRS = 5;
    private static final String ISO_8601_DATE_FORMAT = "yyyyMMdd'T'HHmmssSSS'Z'";
    private static final String IMAGE_PATTERN = "%d_pic.jpg";
    private static final String PREVIEW_FRAME_PATTERN = "%d_%d.jpg";
    private static final int MAX_NUM_SIMULTANEOUS_PREVIEW_IMAGE_SAVES =
            Runtime.getRuntime().availableProcessors() * 2;

    private final ThreadPoolExecutor saveImagesExecutor = new ThreadPoolExecutor(
            MAX_NUM_SIMULTANEOUS_PREVIEW_IMAGE_SAVES, MAX_NUM_SIMULTANEOUS_PREVIEW_IMAGE_SAVES,
            1, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>());
    {
        saveImagesExecutor.allowCoreThreadTimeOut(true);
    }
    private final Semaphore savePreviewFrameSem;

    private final File imagesPath;
    private int imageCounter;
    private int previewFrameCounter;

    /**
     * Construct a new {@link CameraDebugHelper}
     * @param context the active {@link Context}
     * @param allowSkipPreviewFrames set whether saving preview frames may be skipped when necessary
     *                               to maintain a high preview framerate
     * @throws NullPointerException if context or cameraPreferenceManager are null
     */
    public CameraDebugHelper(Context context, boolean allowSkipPreviewFrames) {
        Objects.requireNonNull(context, "context may not be null");

        savePreviewFrameSem = (allowSkipPreviewFrames ?
                new Semaphore(saveImagesExecutor.getCorePoolSize()) : null);

        if (!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            Log.w(TAG, "External storage not mounted and writable; debug images will not be " +
                    "written to disk");
            imagesPath = null;
            return;
        }

        final String imagePathPrefix = context.getApplicationInfo().packageName;
        removeOldDirs(imagePathPrefix);

        DateFormat df = new SimpleDateFormat(ISO_8601_DATE_FORMAT, Locale.US);
        df.setTimeZone(TimeZone.getTimeZone("UTC"));
        File path = new File(DEBUG_IMAGES_ROOT, imagePathPrefix + "_" + df.format(new Date()));

        if (path.mkdirs() || path.isDirectory()) {
            Log.v(TAG, "Debug images will be written to [" + path + "]");
            imagesPath = path;
        } else {
            Log.w(TAG, "Failed to create debug images directory [" + path + "]");
            imagesPath = null;
        }
    }

    /**
     * Save the specified {@link Image} to disk asynchronously
     * @param im an {@link Image} with format {@link ImageFormat#JPEG} or
     *           {@link ImageFormat#YUV_420_888}
     * @param isPreviewFrame true if this {@link Image} is for preview, false if it is for a single
     *                       capture (ie a high-res picture)
     * @return a {@link Future} which can be used to determine when the image has been saved, or
     *         null if the picture cannot be saved (either because the SDCard is not mounted, or it
     *         represents a preview frame, preview frame skipping is enabled, and it was skipped)
     * @throws NullPointerException if im is null
     */
    public Future<?> saveImageAsync(Image im, boolean isPreviewFrame) {
        Objects.requireNonNull(im, "im may not be null");

        if (imagesPath == null) {
            im.close();
            return null;
        }

        final File f;
        synchronized (this) {
            if (isPreviewFrame) {
                f = new File(imagesPath, String.format(Locale.US, PREVIEW_FRAME_PATTERN,
                        imageCounter, previewFrameCounter));
                previewFrameCounter++;
            } else {
                f = new File(imagesPath, String.format(Locale.US, IMAGE_PATTERN, imageCounter));
                imageCounter++;
                previewFrameCounter = 0;
            }
        }

        final SaveImageRunnable saveImageRunnable;
        if (isPreviewFrame && savePreviewFrameSem != null) {
            // If we can't acquire a permit from the semaphore, skip this frame
            if (!savePreviewFrameSem.tryAcquire()) {
                try {
                    //noinspection ResultOfMethodCallIgnored
                    f.createNewFile();
                } catch (IOException e) {
                    Log.e(TAG, "Failed creating [" + f + "]", e);
                }
                im.close();
                return null;
            }

            saveImageRunnable = new SaveImageRunnable(im, f, savePreviewFrameSem);
        } else {
            saveImageRunnable = new SaveImageRunnable(im, f, null);
        }

        return saveImagesExecutor.submit(saveImageRunnable);
    }

    private static void removeOldDirs(final String imagePathPrefix) {
        File[] dirs = DEBUG_IMAGES_ROOT.listFiles(new FileFilter() {
            @Override
            public boolean accept(File pathname) {
                return (pathname.isDirectory() && pathname.getName().startsWith(imagePathPrefix));
            }
        });

        if (dirs != null) {
            Arrays.sort(dirs); // dir names include an ISO 8601 timestamp, so oldest will be first

            for (int i = 0; i < dirs.length - MAX_NUM_IMAGE_DIRS + 1; i++) {
                deleteDirectoryRecursively(dirs[i]);
            }
        }
    }

    private static boolean deleteDirectoryRecursively(File path) {
        boolean result = true;

        final File[] list = path.listFiles();
        if (list != null) {
            for (File f : list) {
                if (f.isDirectory()) {
                    result = deleteDirectoryRecursively(f);
                } else {
                    result = f.delete();
                }

                if (!result) {
                    break;
                }
            }
        }

        if (result) {
            result = path.delete();
        }

        return result;
    }

    private static class SaveImageRunnable implements Runnable {
        final Image im;
        final File file;
        final Semaphore notifyCompleteSem;

        SaveImageRunnable(Image im, File file, Semaphore notifyCompleteSem) {
            this.im = im;
            this.file = file;
            this.notifyCompleteSem = notifyCompleteSem;
        }

        @Override
        public void run() {
            final int format = im.getFormat();
            final Image.Plane[] planes = im.getPlanes();

            // N.B. each switch case should close im as soon as possible
            switch (format) {
                case ImageFormat.JPEG:
                    ByteBuffer jpegBuf = planes[0].getBuffer();
                    jpegBuf.position(0);
                    try (
                        FileOutputStream out = new FileOutputStream(file)
                    ) {
                        out.getChannel().write(jpegBuf);
                    } catch (IOException e) {
                        Log.e(TAG, "Failed to write to [" + file + "]", e);
                    }
                    im.close();
                    break;

                case ImageFormat.YUV_420_888:
                    final int yWidth = im.getWidth();
                    final int yHeight = im.getHeight();
                    final byte[] nv21Data;
                    final int nv21Stride;

                    if (im instanceof ImageNV21) {
                        ImageNV21 nv21Im = (ImageNV21)im;
                        final ByteBuffer buf = nv21Im.getNV21ImageBuffer();
                        buf.position(0);
                        nv21Data = new byte[buf.limit()];
                        buf.get(nv21Data);
                        nv21Stride = nv21Im.getNV21Stride();
                    } else {
                        final int uvWidth = (yWidth + 1) / 2;
                        final int uvHeight = (yHeight + 1) / 2;
                        nv21Stride = uvWidth * 2;
                        nv21Data = new byte[nv21Stride * (yHeight + uvHeight)];

                        final ByteBuffer yBuf = planes[0].getBuffer();
                        final int yBufRowStride = planes[0].getRowStride();
                        for (int i = 0; i < yHeight; i++) {
                            yBuf.position(i * yBufRowStride);
                            yBuf.get(nv21Data, i * nv21Stride, yWidth);
                        }

                        final ByteBuffer uBuf = planes[2].getBuffer();
                        final ByteBuffer vBuf = planes[1].getBuffer();
                        final int uBufRowStride = planes[2].getRowStride();
                        final int vBufRowStride = planes[1].getRowStride();
                        final int uBufPixelStride = planes[2].getPixelStride();
                        final int vBufPixelStride = planes[1].getPixelStride();
                        final byte[] uBufRow = new byte[uBufRowStride - 1];
                        final byte[] vBufRow = new byte[vBufRowStride - 1];
                        uBuf.position(0);
                        vBuf.position(0);
                        for (int i = 0; i < uvHeight; i++) {
                            uBuf.position(i * uBufRowStride);
                            uBuf.get(uBufRow);
                            vBuf.position(i * vBufRowStride);
                            vBuf.get(vBufRow);
                            int yuvRowBase = (yHeight + i) * nv21Stride;
                            for (int j = 0; j < uvWidth; j++) {
                                nv21Data[yuvRowBase + 2 * j] = vBufRow[j * vBufPixelStride];
                                nv21Data[yuvRowBase + 2 * j + 1] = uBufRow[j * uBufPixelStride];
                            }
                        }
                    }
                    im.close();

                    try (
                        FileOutputStream out = new FileOutputStream(file)
                    ) {
                        YuvImage yi = new YuvImage(nv21Data, ImageFormat.NV21, yWidth, yHeight,
                                new int[] { nv21Stride, nv21Stride });
                        yi.compressToJpeg(new Rect(0, 0, yWidth, yHeight), 66, out);
                    } catch (IOException e) {
                        Log.e(TAG, "Failed to write to [" + file + "]", e);
                    }
                    break;

                default:
                    Log.e(TAG, "Unknown image format [" + format + "]");
                    im.close();
                    break;
            }

            if (notifyCompleteSem != null) {
                notifyCompleteSem.release();
            }
        }
    }
}
