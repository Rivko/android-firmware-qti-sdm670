/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.fr_camera.camera;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.util.Log;

import com.qualcomm.qti.fr_camera.misc.ByteBufferInputStream;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.Objects;
import java.util.TimeZone;

/**
 * This class asynchronously writes an image to disk, and then requests that MediaStore index it
 */
public abstract class JpegPictureWriter {
    private static final String TAG = "SocialCamera";
    private static final String FILENAME_PATTERN = "IMG_%s_%d.jpg";
    private static final String TIMESTAMP_PATTERN = "yyyyMMdd_HHmmss";

    private final Context context;
    private final Handler handler;
    private final Date timestamp;
    private final ByteBuffer jpegData;
    private boolean horizontalInvert;

    /**
     * Construct a new {@link JpegPictureWriter}
     * @param context the active {@link Context}
     * @param jpegData a {@link ByteBuffer} containing JPEG data
     * @param horizontalInvert
     * @throws NullPointerException if context or jpegData are null
     */
    public JpegPictureWriter(Context context, ByteBuffer jpegData, boolean horizontalInvert) {
        Objects.requireNonNull(context, "context may not be null");
        Objects.requireNonNull(jpegData, "jpegData may not be null");

        this.context = context;

        handler = new Handler();
        timestamp = new Date();
        this.jpegData = jpegData;
        this.horizontalInvert = horizontalInvert;

        writerTask.execute();
    }

    /**
     * Subclasses must override this method to handle any required actions after the image has been
     * successfully written to disk. This method will be invoked on the UI thread.
     * @param path the path to the image {@link File}
     * @param uri the {@link Uri} of the image in the {@link MediaStore}
     */
    public abstract void onWriteComplete(File path, Uri uri);

    /**
     * Subclasses must override this method to handle any required actions if writing or indexing
     * the image fails. This method will be invoked on the UI thread.
     */
    public abstract void onWriteFailed();

    @SuppressWarnings("FieldCanBeLocal")
    private final AsyncTask<Void, Void, Boolean> writerTask = new AsyncTask<Void, Void, Boolean>() {
        @Override
        protected Boolean doInBackground(Void... params) {
            File pic = writeToFile();
            if (pic == null) {
                return false;
            }

            Log.v(TAG, "Successfully wrote picture to '" + pic + "', requesting indexing");
            indexImage(pic);

            return true;
        }

        @Override
        protected void onPostExecute(Boolean result) {
            if (!result) {
                onWriteFailed();
            }
        }

        private File writeToFile() {
            SimpleDateFormat sdf = new SimpleDateFormat(TIMESTAMP_PATTERN, Locale.US);
            sdf.setTimeZone(TimeZone.getTimeZone("UTC"));

            File path = Environment.getExternalStoragePublicDirectory(
                    Environment.DIRECTORY_PICTURES);

            if (!path.mkdirs() && !path.isDirectory()) {
                Log.e(TAG, "Failed creating path '" + path + "' to save picture");
                return null;
            }

            File pic = null;
            for (int i = 0; i < 100; i++) {
                File f = new File(path, String.format(Locale.US, FILENAME_PATTERN,
                        sdf.format(timestamp), i));
                try {
                    if (f.createNewFile()) {
                        pic = f;
                        break;
                    }
                } catch (IOException e) {
                    Log.e(TAG, "Error creating file '" + f + "' for saving picture", e);
                    return null;
                }
            }

            if (pic == null) {
                Log.e(TAG, "Unable to find an unused filename to save picture");
                return null;
            }

            //first save the jpeg bytebuffer to a file so that we can see if it needs to be rotated
            try (
                    FileOutputStream fos = new FileOutputStream(pic);
                    FileChannel channel = fos.getChannel()
            ) {
                jpegData.position(0);
                channel.write(jpegData);
            } catch (IOException e) {
                Log.e(TAG, "Failed writing file '" + pic + "' for saving picture", e);
                //noinspection ResultOfMethodCallIgnored
                pic.delete(); // Remove the failed picture file
                return null;
            }

            //read the exif information from the file
            ExifInterface exif = null;
            try {
                exif = new ExifInterface(pic.getPath());
            } catch (IOException e) {
                e.printStackTrace();
            }

            int orientation = 0;
            if (exif != null) {
                orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                        ExifInterface.ORIENTATION_UNDEFINED);
            }
            float rotDegrees = 0;
            switch (orientation) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                    rotDegrees = 90;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_180:
                    rotDegrees = 180;
                    break;
                case ExifInterface.ORIENTATION_ROTATE_270:
                    rotDegrees = 270;
                    break;
                default:
                    break;
            }

            //if it needs to be rotated or inverted, discard the first file and write a
            //new file with the proper matrix manipulation
            if (rotDegrees == 0 && !horizontalInvert)
                return pic;
            else {
                Bitmap b = BitmapFactory.decodeStream(new ByteBufferInputStream(jpegData));
                Matrix m = new Matrix();
                if (horizontalInvert)
                    m.preScale(-1, 1);

                m.preRotate(rotDegrees);

                Bitmap bitmap = Bitmap.createBitmap(b, 0, 0, b.getWidth(), b.getHeight(),
                        m, true);
                if (b != null && !b.isRecycled()) {
                    b.recycle();
                }

                String picPath = pic.getPath();
                pic.delete();
                File rotatedJpgFile = new File(picPath);
                try {
                    FileOutputStream fos = new FileOutputStream(rotatedJpgFile);
                    BufferedOutputStream bos = new BufferedOutputStream(fos);
                    bitmap.compress(Bitmap.CompressFormat.JPEG, 80, bos);
                    bos.flush();
                    bos.close();
                    fos.close();
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                return rotatedJpgFile;
            }
        }

        private void indexImage(File jpegPath) {
            MediaScannerConnection.scanFile(context, new String[] { jpegPath.getPath() },
                    new String[] { "image/jpeg" }, scanCompletedListener);
        }
    };

    private final MediaScannerConnection.OnScanCompletedListener scanCompletedListener =
            new MediaScannerConnection.MediaScannerConnectionClient() {
        @Override
        public void onMediaScannerConnected() {
        }

        @Override
        public void onScanCompleted(final String path, final Uri uri) {
            // N.B. This is invoked in an arbitrary thread context
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (uri != null) {
                        Log.v(TAG, "Indexing completed for file '" + path + "', uri='" + uri + "'");
                        onWriteComplete(new File(path), uri);
                    } else {
                        Log.e(TAG, "Indexing failed for file '" + path + "'");
                        onWriteFailed();
                    }
                }
            });
        }
    };
}
