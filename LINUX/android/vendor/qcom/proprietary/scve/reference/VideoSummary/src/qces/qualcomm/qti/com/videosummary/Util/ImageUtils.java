/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Util;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.RectF;
import android.media.ExifInterface;
import android.net.Uri;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.util.Log;

import java.io.IOException;

/**
 * Class for the common functionalities
 */
public class ImageUtils {
    private static final String LOG_TAG = "ImageUtils";

    public static final int MAX_IMAGE_SIZE = (12 * 1024 * 1024); // 12 MB

    private static Bitmap cachedBitmap = null;


    /**
     * gets folder name from path
     * 
     * @param path
     *            path name
     * @return folder name
     */
    public static String pathToSourceName(String path) {
        String[] comp = path.split("/");
        String source = "";
        if (comp.length > 1) {
            source = comp[comp.length - 2];
        }
        return source;
    }

    /**
     * gets image name from path
     * 
     * @param path
     *            path name
     * @return image name
     */
    public static String pathToImageName(String path) {
        String[] comp = path.split("/");
        String fileName = "";
        if (comp.length > 0) {
            fileName = comp[comp.length - 1];
        }
        return fileName;
    }

    /**
     * gets the image size from the path
     * 
     * @param pathName
     *            path of the image
     * @return image size
     */
    public static Point getImageSize(String pathName) {
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(pathName, options);

        int width = options.outWidth;
        int height = options.outHeight;
        try {
            int angle = ImageUtils.getExifRotation(pathName);
            if (angle == 90 || angle == 270) {
                width = options.outHeight;
                height = options.outWidth;
            }
        } catch (IOException exp) {
            exp.printStackTrace();
        }
        return new Point(width, height);
    }

    /**
     * calculates the sample size
     * 
     * @param width
     *            image width
     * @param height
     *            image height
     * @return sample size
     */
    public static int calculateInSampleSize(int width, int height) {
        long imageSize = height * width;
        int inSampleSize = 1;

        while (imageSize > MAX_IMAGE_SIZE) {
            height /= 2;
            width /= 2;
            imageSize = height * width;

            inSampleSize *= 2;
        }

        return inSampleSize;
    }

    /**
     * loads Bitmap with sample size
     * 
     * @param pathName
     *            image path name
     * @param sampleSize
     *            sample size
     * @return bitmap
     */
    public static Bitmap loadParsableBitmap(String pathName, int sampleSize, boolean inBitmap) {
        final BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = sampleSize;
        options.inJustDecodeBounds = false;
        options.inMutable = true;

        Bitmap rgbaBitmap = null;
        if (inBitmap && cachedBitmap != null) {
            Log.d(LOG_TAG, "Loading older image");
            options.inBitmap = cachedBitmap;
            rgbaBitmap = BitmapFactory.decodeFile(pathName, options);
        }

        if (rgbaBitmap == null) {
            options.inBitmap = null;
            Log.d(LOG_TAG, "Loading new image");
            rgbaBitmap = BitmapFactory.decodeFile(pathName, options);
            Log.d(LOG_TAG,
                    "Created new image, size " + rgbaBitmap.getWidth() + " X "
                            + rgbaBitmap.getHeight());

        } else {
            Log.d(LOG_TAG,
                    "Reused older image, size " + rgbaBitmap.getWidth() + " X "
                            + rgbaBitmap.getHeight());
        }

        if (rgbaBitmap != null) {
            try {
                int angle = getExifRotation(pathName);
                if (angle != 0) {
                    Matrix matrix = new Matrix();
                    matrix.postRotate(angle);
                    Bitmap rotatedRgbaBitmap = Bitmap.createBitmap(rgbaBitmap, 0, 0,
                            rgbaBitmap.getWidth(), rgbaBitmap.getHeight(), matrix, true);
                    if (rotatedRgbaBitmap != rgbaBitmap) {
                        Log.d(LOG_TAG, "Got rotated image, original recycled");
                        rgbaBitmap.recycle();
                        rgbaBitmap = rotatedRgbaBitmap;
                    }
                }

            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if (inBitmap && rgbaBitmap != cachedBitmap) {
            if (cachedBitmap != null) {
                Log.d(LOG_TAG, "Recycle and update");
                cachedBitmap.recycle();
            } else {
                Log.d(LOG_TAG, "only update");
            }
            cachedBitmap = rgbaBitmap;
        }

        return rgbaBitmap;
    }

    /**
     * gets the exif information of the image
     * 
     * @param jpgFilePath
     *            jpg file location
     * @return rotation angle
     * @throws IOException
     */
    public static int getExifRotation(String jpgFilePath) throws IOException {
        ExifInterface exif = new ExifInterface(jpgFilePath);
        int jpgRotation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                ExifInterface.ORIENTATION_UNDEFINED);

        int angle = 0;
        switch (jpgRotation) {
        case ExifInterface.ORIENTATION_ROTATE_90:
            angle = 90;
            break;
        case ExifInterface.ORIENTATION_ROTATE_180:
            angle = 180;
            break;
        case ExifInterface.ORIENTATION_ROTATE_270:
            angle = 270;
            break;
        }
        return angle;
    }

    /**
     * Creates a round icon
     * 
     * @param image
     *            image to be drawn in rounded image
     * @param iconSize
     *            icon size
     * @param shadowWidth
     *            shadow width
     * @return rounded image
     */
    public static Bitmap createRoundIcon(Bitmap image, int iconSize, int shadowWidth) {
        float imageCenter = iconSize / 2;

        // create mask
        Paint maskPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        maskPaint.setColor(Color.BLACK);
        Bitmap maskImage = Bitmap.createBitmap(iconSize, iconSize, Config.ARGB_8888);
        Canvas maskCanvas = new Canvas(maskImage);
        maskCanvas.drawCircle(imageCenter, imageCenter, imageCenter - shadowWidth, maskPaint);

        // create round icon image
        Bitmap roundImage = Bitmap.createBitmap(iconSize, iconSize, Config.ARGB_8888);
        Canvas canvas = new Canvas(roundImage);

        final RectF destinationRect = new RectF(0, 0, iconSize, iconSize);
        final Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);

        canvas.drawBitmap(image, null, destinationRect, paint);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DST_IN));
        canvas.drawBitmap(maskImage, 0, 0, paint);
        paint.setXfermode(null);

        Bitmap output = null;

        if (shadowWidth > 0) {// draw final image (shadow and round icon)
            output = Bitmap.createBitmap(iconSize, iconSize, Config.ARGB_8888);
            Canvas shadowCanvas = new Canvas(output);
            Paint shadowPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
            shadowPaint.setColor(Color.WHITE);
            shadowPaint.setShadowLayer(shadowWidth, 0.0f, 0.0f, Color.BLACK);
            shadowCanvas.drawCircle(imageCenter, imageCenter, imageCenter - shadowWidth,
                    shadowPaint);
            shadowCanvas.drawBitmap(roundImage, 0, 0, paint);

            roundImage.recycle();
        } else {
            output = roundImage;
        }
        maskImage.recycle();
        return output;
    }

    public static String getImagePathFromUri(final Context context, final Uri uri) {

        String scheme = uri.getScheme();
        String path = null;
        Cursor cursor = null;

        if (scheme.equals("file")) {
            path = uri.getPath();
        } else if (scheme.equals("content")) {
            String id = DocumentsContract.getDocumentId(uri);

            if(uri.toString().contains("downloads")){
                final Uri contentUri = ContentUris.withAppendedId(
                        Uri.parse("content://downloads/public_downloads"), Long.valueOf(id));
                cursor = context.getContentResolver().query(contentUri, new String[]{"_data"}, null, null,
                        null);
            }else {
                String[] selectionArgs = new String[]{DocumentsContract.getDocumentId(uri)
                        .split(":")[1]};
                cursor = context.getContentResolver().query(
                        MediaStore.Images.Media.EXTERNAL_CONTENT_URI, new String[]{"_data"},
                        "_id=?", selectionArgs, null);
            }
            if (cursor != null && cursor.moveToFirst()) {
                path = cursor.getString(0);// idx 0 is the _data = file path
                cursor.close();
            }

        } else {
            Log.d("ERROR", "error: unknown uri scheme: " + scheme);
        }
        return path;

    }

}
