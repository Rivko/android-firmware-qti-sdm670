/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.graphics.RectF;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.Toast;

import com.qualcomm.qti.panoramaui.R;
import com.qualcomm.qti.panoramaui.data.AppError;
import com.qualcomm.qti.panoramaui.tileviewer.DynamicInsetLayerDrawable;
import com.qualcomm.qti.panoramaui.tileviewer.RotatePanoramaDrawable;
import com.qualcomm.qti.panoramaui.tileviewer.TiledImageDrawable;
import com.qualcomm.qti.panoramaui.tileviewer.TranslateScaleController;
import com.qualcomm.qti.panoramaui.utils.ImageUtils;

/**
 * This class handles the viewing of panorama image
 */
public class PanoramaViewer extends Activity {
    /**
     * The invoker of the {@link PanoramaViewer} activity should include this extra in their intent,
     * specifying the full path to the temporary image file. The image file must remain valid for
     * the lifetime of this activity.
     */
    public static final String EXTRA_PANORAMA_PATH = PanoramaViewer.class.getName() + "/extra_panorama_path";

    private static final String LOG_TAG = "PanoViewer";
    private static final String PANORAMA_FILE_NAME = "panorama_%s.jpg";

    private File panoramaFile;
    private ImageView image;
    private TiledImageDrawable imageDrawable;
    private TranslateScaleController imageController;
    private ImageView minimap;
    private LayerDrawable minimapDrawable;

    private static String SHARE_BUTTON_TEXT;

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle mybundle) {
        super.onCreate(mybundle);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_panorama_viewer);

        String panoramaPath = getIntent().getStringExtra(EXTRA_PANORAMA_PATH);
        new LoadPanoramaTask().execute(panoramaPath);

        SHARE_BUTTON_TEXT = getResources().getString(R.string.share_button_text);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void onBackPressed() {
        super.onBackPressed();
        panoramaFile.delete();
    }

    /**
     * This method handle the custom back button
     * @param view View
     */
    public void onBackPressed(View view) {
        super.onBackPressed();
        panoramaFile.delete();
    }

    /**
     * This method handles the share button
     * @param view View
     */
    public void onSharePressed(View view) {
        Intent shareIntent = new Intent(Intent.ACTION_SEND);
        shareIntent.setType("image/*");

        Uri uri = Uri.fromFile(panoramaFile);
        shareIntent.putExtra(Intent.EXTRA_STREAM, uri);
        startActivity(Intent.createChooser(shareIntent, SHARE_BUTTON_TEXT));
    }

    /**
     * This method handles the Save button
     * @param view the Save button
     */
    public void onSaveClick(View view) {
        File picturesPath = Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_PICTURES);
        SimpleDateFormat dateFormatter = new SimpleDateFormat("yyyy-MM-dd_HH-mm", Locale.US);
        String date = dateFormatter.format(new Date());
        String dateStr = date;
        File outputFile = new File(picturesPath, String.format(Locale.US, PANORAMA_FILE_NAME, date));
        if (outputFile.exists()) {
            for (int i = 0; i < 100 && outputFile.exists(); i++) {
                dateStr = date + "_" + i;
                outputFile = new File(picturesPath, String.format(Locale.US, PANORAMA_FILE_NAME,
                        dateStr));
            }
        }

        if (outputFile.exists()) {
            Log.e(LOG_TAG, "Failed to find an unused output filename");
            // TODO: handle error gracefully
            return;
        }

        try {
            ByteBuffer panoramaData = ByteBuffer.allocateDirect((int)panoramaFile.length());
            FileInputStream inputStream = new FileInputStream(panoramaFile);
            while (panoramaData.hasRemaining()) {
                inputStream.getChannel().read(panoramaData);
            }
            inputStream.close();

            FileOutputStream outputStream = new FileOutputStream(outputFile);
            panoramaData.position(0);
            while (panoramaData.hasRemaining()) {
                outputStream.getChannel().write(panoramaData);
            }
            outputStream.close();

            panoramaFile.delete();
        } catch (IOException e) {
            Log.e(LOG_TAG, "Failed to copy [" + panoramaFile + "] to [" + outputFile + "]");
            // TODO: handle error gracefully
            return;
        }

        Intent intent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.fromFile(outputFile));
        sendBroadcast(intent);

        Toast.makeText(this, R.string.panorama_saved, Toast.LENGTH_SHORT).show();
        ImageUtils.renameKeyFrameCacheFolderName(dateStr);
    }

    private Bitmap loadMinimapImage(ByteBuffer buf, int orientation) {
        if (!buf.hasArray()) {
            throw new RuntimeException("Must use non-direct ByteBuffer");
        }

        Point displaySize = new Point();
        getWindowManager().getDefaultDisplay().getSize(displaySize);

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeByteArray(buf.array(), buf.arrayOffset(), buf.limit(), options);
        final int width = (orientation == 0 || orientation == 180 ?
                options.outWidth : options.outHeight);
        options.inSampleSize = width / displaySize.x;
        options.inJustDecodeBounds = false;
        return BitmapFactory.decodeByteArray(buf.array(), buf.arrayOffset(), buf.limit(), options);
    }

    private final TranslateScaleController.OnTranslateScaleListener translateScaleListener =
            new TranslateScaleController.OnTranslateScaleListener() {
        @Override
        public void onTranslateScale(RectF region) {
            final int minimapWidth = minimapDrawable.getBounds().width();
            final int minimapHeight = minimapDrawable.getBounds().height();
            final float scale;
            if (minimapWidth > 0) {
                scale = (float)minimapWidth / imageDrawable.getIntrinsicWidth();
            } else {
                scale = 0f;
            }
            minimapDrawable.setLayerInset(1, (int)(region.left * scale),
                    (int)(region.top * scale),
                    minimapWidth - (int)(region.right * scale),
                    minimapHeight - (int)(region.bottom * scale));
        }
    };

    private class LoadPanoramaTask extends AsyncTask<String, Void, AppError> {
        int orientation;
        ByteBuffer panoramaData;

        @Override
        protected AppError doInBackground(String... params) {
            try {
                if (params[0] == null || params[0].isEmpty()) {
                    Log.e(LOG_TAG, "No path provided for panorama image");
                    return AppError.PANORAMA_LOADING_FAILED;
                }

                panoramaFile = new File(params[0]);
                if (!panoramaFile.exists() || !panoramaFile.canRead()) {
                    Log.e(LOG_TAG, "Provided path does not exist or is not readable");
                    return AppError.PANORAMA_LOADING_FAILED;
                }

                panoramaData = ByteBuffer.allocateDirect((int)panoramaFile.length());
                FileInputStream inputStream = new FileInputStream(params[0]);
                while (panoramaData.hasRemaining()) {
                    inputStream.getChannel().read(panoramaData);
                }
                inputStream.close();

                ExifInterface exif = new ExifInterface(params[0]);
                switch (exif.getAttributeInt(ExifInterface.TAG_ORIENTATION,
                        ExifInterface.ORIENTATION_UNDEFINED)) {
                case ExifInterface.ORIENTATION_ROTATE_90:
                    orientation = 90;
                    break;

                case ExifInterface.ORIENTATION_ROTATE_180:
                    orientation = 180;
                    break;

                case ExifInterface.ORIENTATION_ROTATE_270:
                    orientation = 270;
                    break;
                }
            } catch (IOException e) {
                Log.e(LOG_TAG, "Error reading panorama file", e);
                return AppError.PANORAMA_LOADING_FAILED;
            }

            return null;
        }

        @Override
        protected void onPostExecute(AppError result) {
            if (result != null) {
                // TODO: handle error gracefully
                return;
            }

            image = (ImageView)findViewById(R.id.image_view);
            imageDrawable = new TiledImageDrawable(panoramaData, 2f);
            image.setImageDrawable(new RotatePanoramaDrawable(imageDrawable, orientation));

            minimap = (ImageView)findViewById(R.id.minimap_image);
            minimapDrawable = new DynamicInsetLayerDrawable(new Drawable[] {
                    new BitmapDrawable(getResources(), loadMinimapImage(panoramaData, orientation)),
                    getResources().getDrawable(R.drawable.visible_area) });
            minimap.setImageDrawable(new RotatePanoramaDrawable(minimapDrawable, orientation));

            imageController = new TranslateScaleController(image, orientation);
            imageController.setOnTranslateScaleListener(translateScaleListener);
        }
    }
}
