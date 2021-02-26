/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BlurMaskFilter;
import android.graphics.BlurMaskFilter.Blur;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Matrix.ScaleToFit;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.RectF;
import android.net.Uri;
import android.os.Bundle;
import android.renderscript.Allocation;
import android.renderscript.RenderScript;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;

import com.qualcomm.qti.qces.customui.AnimatedImageView;
import com.qualcomm.qti.qces.imageedit.fastcv.FastCVJNILib;
import com.qualcomm.qti.qces.imageedit.segmentation.ScriptC_CeilTransparency;
import com.qualcomm.qti.qces.imageedit.softcut.R;
import com.qualcomm.qti.qces.objectcloneremoveshared.BitmapCache;
import com.qualcomm.qti.qces.translatescaleimageview.TranslateScaleImageView;
import com.qualcomm.qti.qces.util.ImageSharingIntentExtras;
import com.qualcomm.qti.qces.util.ImageUtils;

/**
 * This {@link DemoActivity} takes the masked object (from {@link CloneMaskActivity}) and blends it
 * into the background image (using {@link ObjectCloner}). The object can be translated and scaled
 * over the background, and the resulting image can be saved or shared (by way of
 * {@link ResultToolbarFragment})
 *
 * @author slaver
 */
public class CloneActivity extends Activity {

    private static final float BLUR_RADIUS = 40f;
    private static final int BLUR_COLOR = Color.BLACK;

    public ImageView mBackgroundImageView;
    public TranslateScaleImageView mClippedImageView;

    // Generated images and metadata
    private boolean mSetInitialClippedLocation;
    private boolean mInitialLayoutComplete;
    private Bitmap mClippedGlowing;
    public int[] mClippedGlowingInflation = new int[2];

    private ImageButton doneButton;
    private ImageButton helpButton;
    private ImageButton cancelButton;
    private ImageButton replaceBgButton;
    private ImageButton undoButton;
    private ImageButton redoButton;
    private ImageButton horizontalFlipButton;
    private ImageButton verticalFlipButton;


    private SeekBar cloneBlendSeekbar;

    private CloneController controller;
    private CloneModel model;
    private AnimatedImageView spinner;
    private View spinnerBackground;
    protected boolean isFullMP;


    /*
     * global variable to save the current layout(screen) size and will be updated every time layout
     * size changes
     */
    private RectF currentLayoutBounds;

    TranslateScaleImageView.OnTranslateScaleListener mOnTranslateScaleListener = new TranslateScaleImageView.OnTranslateScaleListener() {
        @Override
        public void onTranslateScaleStarted(TranslateScaleImageView view) {
            mClippedImageView.pushUndoLastObjTransformation();
            controller.refreshUndoRedoEnables();

            mBackgroundImageView.setImageBitmap(model.mBackground);
            mClippedImageView.clearAnimation();
            mClippedImageView.setAlpha(1.0f);
            mClippedImageView.invalidate(); // need to invalidate after setAlpha

        }

        @Override
        public void onTranslateScaleFinished(TranslateScaleImageView view) {

            model.blendImages();
            mBackgroundImageView.setImageBitmap(model.finalMergeFull);

            // Animate the glowing rough clipped image into the newly blended background
            Animation a = AnimationUtils.loadAnimation(CloneActivity.this,
                    R.anim.clone_result_hide_rough_clip);
            mClippedImageView.startAnimation(a);
        }
    };

    @Override
    protected void onSaveInstanceState(Bundle savedInstanceState) {
        super.onSaveInstanceState(savedInstanceState);
        controller.onSaveInstanceState(savedInstanceState);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        model = new CloneModel(this);
        controller = new CloneController(this, model);

        setContentView(R.layout.clone_layout);

        if (savedInstanceState != null) {
            controller.onRestoreInstanceState(savedInstanceState);
        }

        mClippedImageView = (TranslateScaleImageView) this.findViewById(R.id.cloneResultClipped);

        mBackgroundImageView = (ImageView) this.findViewById(R.id.cloneResultBackground);
        mBackgroundImageView.addOnLayoutChangeListener(new View.OnLayoutChangeListener() {
            @Override
            public void onLayoutChange(View v, int left, int top, int right, int bottom,
                                       int oldLeft, int oldTop, int oldRight, int oldBottom) {
                currentLayoutBounds = new RectF(left, top, right, bottom);
                if (mInitialLayoutComplete &&
                        (left != oldLeft || top != oldTop || right != oldRight || bottom != oldBottom)) {

                    model.blendImages();
                    mBackgroundImageView.setImageBitmap(model.finalMergeFull);
                }
            }
        });

        mSetInitialClippedLocation = true;// (savedInstanceState == null);

        initializeViews();
    }

    @Override
    protected void onResume() {
        super.onResume();
        controller.onResume();
    }

    void initializeViews() {
        initializeSpinner(R.id.spinner, R.id.spinner_background);
        initializeCancelButton(R.id.clone_cancel_button);
        initializeHelpButton(R.id.clone_help_button);
        initializeDoneButton(R.id.clone_done_button);
        initializeReplaceBgButton(R.id.clone_replace_bg_button);
        initializeCloneBlendSeekbar(R.id.clone_blend_seekbar);
        initializeUndoButton(R.id.clone_undo_button);
        initializeRedoButton(R.id.clone_redo_button);
        initializeHorizontalFlipButton(R.id.clone_horizontal_flip_button);
        initializeVerticalFlipButton(R.id.clone_vertical_flip_button);
    }

    void initializeSpinner(int idSpinner, int idSpinnerBackground) {
        spinner = (AnimatedImageView) findViewById(idSpinner);
        spinnerBackground = findViewById(idSpinnerBackground);
    }

    void initializeCancelButton(int id) {
        cancelButton = (ImageButton) findViewById(id);
        cancelButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.cancelClicked();
            }
        });
    }

    void initializeHelpButton(int id) {
        helpButton = (ImageButton) findViewById(id);
        helpButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.helpClicked();
            }
        });
    }

    void initializeDoneButton(int id) {
        doneButton = (ImageButton) findViewById(id);
        doneButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.doneClicked();
            }
        });
    }

    void initializeHorizontalFlipButton(int id) {
        horizontalFlipButton = (ImageButton) findViewById(id);
        horizontalFlipButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                mClippedImageView.horizontalFlipObject();
            }
        });
    }

    void initializeVerticalFlipButton(int id) {
        verticalFlipButton = (ImageButton) findViewById(id);
        verticalFlipButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                mClippedImageView.verticalFlipObject();
            }
        });
    }

    void initializeReplaceBgButton(int id) {
        replaceBgButton = (ImageButton) findViewById(id);
        replaceBgButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.replaceBgClicked();
            }
        });
    }

    void initializeCloneBlendSeekbar(int id) {
        cloneBlendSeekbar = (SeekBar) findViewById(id);
        cloneBlendSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                //change the blend amount here between the smart blend and the dumb blend
                model.blend_factor = (float) i / 100;

                if (model.mBlend != null) {
                    model.blendAndDraw();
                    mBackgroundImageView.invalidate();
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });


    }

    void initializeUndoButton(int id) {
        undoButton = (ImageButton) findViewById(id);
        undoButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.undoClicked();
            }
        });

        disableUndoBtn(); // on init
    }

    void initializeRedoButton(int id) {
        redoButton = (ImageButton) findViewById(id);
        redoButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                controller.redoClicked();
            }
        });

        disableRedoBtn(); // on init
    }

    void enableUndoBtn() {
        undoButton.setEnabled(true);
    }

    void disableUndoBtn() {
        undoButton.setEnabled(false);
    }

    void enableRedoBtn() {
        redoButton.setEnabled(true);
    }

    void disableRedoBtn() {
        redoButton.setEnabled(false);
    }

    public void undoTransScale() {
        mClippedImageView.undoObjTransformation();
    }

    public void redoTransScale() {
        mClippedImageView.redoObjTransformation();
    }

    public int getUndoTransScaleCount() {
        return mClippedImageView.getUndoObjTransformationCount();
    }

    public int getRedoTransScaleCount() {
        return mClippedImageView.getRedoObjTransformationCount();
    }

    public void clearUndoRedoStack() {
        mClippedImageView.clearUndoRedoStack();
    }

    public void getBackgroundFromGalleryClicked(View v) {
        controller.getBackgroundFromGalleryClicked();
    }

    public void getBackgroundFromCameraClicked(View v) {
        controller.getBackgroundFromCameraClicked();
    }

    void showSpinner() {
        spinner.setVisibility(View.VISIBLE);
        spinnerBackground.setVisibility(View.VISIBLE);
    }

    void hideSpinner() {
        spinner.setVisibility(View.GONE);
        spinnerBackground.setVisibility(View.GONE);
    }

    float[] matrix_values = null;
    /**
     * {@inheritDoc}
     */
    @Override
    protected void onStart() {
        super.onStart();
        Uri imageUri = null, objectUri = null, maskUri = null;
        Intent intent = getIntent();

        matrix_values = intent.getFloatArrayExtra(ImageSharingIntentExtras.CLONE_MATRIX);
        //if intent has matrix values, then we are re-entering clone from stage after we just did a clone
        if (matrix_values != null) {
            intent.putExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI,intent.getStringExtra(ImageSharingIntentExtras.BACKGROUND_IMAGE_URI)); //overwrite Uri with original BG
            //setting this to null so that next time when onStart is called due to orientation change or replacebg it would not reset the matrix to start
            intent.putExtra(ImageSharingIntentExtras.CLONE_MATRIX,"");
            mSetInitialClippedLocation = false;
        }

        String imageUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_TARGET_IMAGE_URI);
        String objectUriString = intent
                .getStringExtra(ImageSharingIntentExtras.EXTRA_OBJECT_IMAGE_URI);
        String maskUriString = intent.getStringExtra(ImageSharingIntentExtras.EXTRA_MASK_IMAGE_URI);
        if (imageUriString != null && objectUriString != null && maskUriString != null) {
            imageUri = Uri.parse(imageUriString);
            objectUri = Uri.parse(objectUriString);
            maskUri = Uri.parse(maskUriString);
        }



        isFullMP = intent.getBooleanExtra(ImageSharingIntentExtras.FULL_MP_MODE, true);

        model.mBackground = BitmapCache.loadBitmapThroughCache(this, imageUri, isFullMP);
        Bitmap object = BitmapCache.loadBitmapThroughCache(this, objectUri, isFullMP);
        Bitmap preciseMask = BitmapCache.loadBitmapThroughCache(this, maskUri, isFullMP);

        int border = 50;
        Bitmap borderReplicationBitmap = Bitmap.createBitmap(model.mBackground.getWidth() + border * 2,
                model.mBackground.getHeight() + border * 2, Config.ARGB_8888);
        BorderReplicateJNILib.BorderReplicateRGBA32(model.mBackground, borderReplicationBitmap);

        Bitmap dilatedAlphaMask = Bitmap.createBitmap(preciseMask.getWidth(),
                preciseMask.getHeight(), Bitmap.Config.ALPHA_8);
        Bitmap alphaMask = preciseMask.extractAlpha();
        FastCVJNILib.dilateNxN(alphaMask, 15, dilatedAlphaMask); //bitmap stride must be multiple of 8
        Bitmap dilatedMask = dilatedAlphaMask.copy(Config.ARGB_8888, true);
        dilatedAlphaMask.recycle();
        alphaMask.recycle();

        //set the dilated mask alpha values to 0 if 0, and 255 if between 1<->255
        RenderScript rs = RenderScript.create(this);
        ScriptC_CeilTransparency scriptC_ceilTransparency = new ScriptC_CeilTransparency(rs);
        Allocation a = Allocation.createFromBitmap(rs, dilatedMask);
        a.syncAll(Allocation.USAGE_SCRIPT);
        scriptC_ceilTransparency.forEach_root(a);
        a.syncAll(Allocation.USAGE_SHARED);
        a.destroy();

        RectF maskBoundingRect = ImageUtils.generateMaskBoundingRect(dilatedMask);

        if (dilatedMask.getWidth() < object.getWidth() || dilatedMask.getHeight() > object.getHeight()) {
            float horScale = (float) object.getWidth() / (float) dilatedMask.getWidth();
            float verScale = (float) object.getHeight() / (float) dilatedMask.getHeight();
            maskBoundingRect = new RectF(maskBoundingRect.left = maskBoundingRect.left * horScale,
                    maskBoundingRect.top = maskBoundingRect.top * verScale,
                    maskBoundingRect.right = maskBoundingRect.right * horScale,
                    maskBoundingRect.bottom = maskBoundingRect.bottom * verScale);

            Matrix m = new Matrix();
            m.setRectToRect(new RectF(0, 0, dilatedMask.getWidth(), dilatedMask.getHeight()), new RectF(0, 0,
                    object.getWidth(), object.getHeight()), ScaleToFit.CENTER);
            dilatedMask = Bitmap.createBitmap(preciseMask, 0, 0, dilatedMask.getWidth(), dilatedMask.getHeight(), m, true);
        }

        // Set initial background state, until first blend occurs (via global layout listener below)
        mBackgroundImageView.setImageBitmap(model.mBackground);
        mInitialLayoutComplete = false;

        object = Bitmap.createBitmap(object, (int) maskBoundingRect.left, (int) maskBoundingRect.top,
                (int) maskBoundingRect.width(), (int) maskBoundingRect.height());

        preciseMask = Bitmap.createBitmap(preciseMask, (int) maskBoundingRect.left, (int) maskBoundingRect.top,
                (int) maskBoundingRect.width(), (int) maskBoundingRect.height());

        dilatedMask = Bitmap.createBitmap(dilatedMask, (int) maskBoundingRect.left, (int) maskBoundingRect.top,
                (int) maskBoundingRect.width(), (int) maskBoundingRect.height());

        model.initRSKernelBlend(preciseMask);

        // Create a version of the clipped image with a glow, for drawing while we translate/scale
        mClippedGlowing = createGlowingBitmap(object, preciseMask, mClippedGlowingInflation);
        mClippedImageView.setImageBitmap(mClippedGlowing);
        maskBoundingRect.inset(mClippedGlowingInflation[0], mClippedGlowingInflation[1]);

        mClippedImageView.getViewTreeObserver().addOnGlobalLayoutListener(
                new OnGlobalLayoutListener() {
                    @SuppressWarnings("deprecation")
                    @Override
                    public void onGlobalLayout() {
                        // Only run on first layout
                        mClippedImageView.getViewTreeObserver().removeGlobalOnLayoutListener(this);

                        mClippedImageView.setViewPortOffsetAdjustMatrix(getViewPortAdjustMatrix());
                        mClippedImageView.setViewPortBoundingBox(getViewPortImageBounds());

                        //if matrix_values are null, we're re-entering clone, so we bring back the matrix to put the object where it was
                        if (matrix_values != null) {
                            ObjectTransformation lastTranslate = new ObjectTransformation();
                            lastTranslate.setValues(matrix_values);
                            lastTranslate = mClippedImageView.getBitMapToCurrentOrientationTransformedMatrix(lastTranslate);
                            lastTranslate.setHorizontalFlipState(matrix_values[matrix_values.length - 2] > 0);
                            lastTranslate.setVerticalFlipState(matrix_values[matrix_values.length - 1] > 0);
                            mClippedImageView.setObjTransformation(lastTranslate);
                            mClippedImageView.applyTranslateScaleMatrix();
                            matrix_values = null;
                        }

                        if (mSetInitialClippedLocation) {
                            mSetInitialClippedLocation = false;
                            /*
                             * position the clipped object: Setting initial offset to (10,10) &
                             * scale value to half its original size of the clone object
                             */
                            int leftPos = getBaseContext().getResources().getInteger(
                                    R.integer.clone_object_initial_position_left);
                            int topPos = getBaseContext().getResources().getInteger(
                                    R.integer.clone_object_initial_position_top);
                            TypedValue outValue = new TypedValue();
                            getBaseContext().getResources().getValue(
                                    R.integer.clone_object_initial_scale, outValue, true);
                            float initialScale = outValue.getFloat();
                            mClippedImageView.translateScaleTo(leftPos, topPos, initialScale);
                        }
                        mOnTranslateScaleListener.onTranslateScaleFinished(mClippedImageView);
                        mInitialLayoutComplete = true;
                    }
                });

        // The hit region is in image view coordinates, not our imaginary
        // "if the whole object image was still there" coordinates
        maskBoundingRect.offsetTo(0, 0);
        mClippedImageView.setDrawableHitRect(maskBoundingRect);

        model.mObjectCloner = new ObjectCloner(this.getResources(), borderReplicationBitmap, object, dilatedMask, border,
                getUseGpu());

        // N.B. Don't install OnTranslateScaleListener in OnCreate, since it isn't safe to invoke
        // before object cloner is created
        mClippedImageView.setOnTranslateScaleListener(mOnTranslateScaleListener);
    }

    /**
     * Get's whether the GPU flag was set or not.
     *
     * @return
     */
    private boolean getUseGpu() {
        // SharedPreferences sp = getSharedPreferences(CloneGalleryActivity.SHARED_PREFERENCES_NAME,
        // Context.MODE_PRIVATE);
        // String sTargetsp.getString(CloneGalleryActivity.PREF_TARGET, "CPU");
        // TODO: currently forcing to use CPU, to be fixed later
        String sTarget = new String("CPU");
        return sTarget.equals("GPU");
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onStop() {
        super.onStop();
        stop();
    }

    protected void stop() {
        // N.B. Must clear this listener, it depends on object cloner (which we are about to
        // destroy)
        mClippedImageView.setOnTranslateScaleListener(null);

        if (model.mObjectCloner != null) {
            model.mObjectCloner.close();
            model.mObjectCloner = null;
        }

        model.mBackground = null;
        mClippedGlowing = null;
        model.mBlend = null;

        mBackgroundImageView.setImageDrawable(null);
    }

    /**
     * This helper function creates a glow around the object {@link Bitmap}, as an effect to
     * indicate to the user that the object is currently "actionable"
     *
     * @param src        Source {@link Bitmap} around which to create a glow
     * @param mask       Mask {@link Bitmap} to create the glow from
     * @param blurOffset int[2] array into which to insert the X, Y offset that creating the glow
     *                   effect has added to the size of the src/mask {@link Bitmap}s
     * @return a new {@link Bitmap}, inflated by blurOffset, containing the src Bitmap with the glow
     * effect added
     */
    private static Bitmap createGlowingBitmap(Bitmap src, Bitmap mask, int[] blurOffset) {
        if (blurOffset != null && blurOffset.length != 2) {
            throw new IllegalArgumentException("If blurOffset provided, must have a length of 2");
        }

        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setMaskFilter(new BlurMaskFilter(BLUR_RADIUS, Blur.OUTER));
        Bitmap blurMask = mask.extractAlpha(paint, blurOffset);

        // Sadly, the default BlurMaskFilter is not very dark. Draw it a bunch in PorterDuff
        // darken mode to get a heavier mask.
        Bitmap dst = Bitmap.createBitmap(blurMask.getWidth(), blurMask.getHeight(),
                Config.ARGB_8888);
        Canvas c = new Canvas(dst);
        paint.setMaskFilter(null);
        paint.setColor(BLUR_COLOR);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.DARKEN));
        for (int i = 0; i < 4; i++) {
            c.drawBitmap(blurMask, 0, 0, paint);
        }
        blurMask.recycle();

        // Now, paint the clipped image on top of the glow
        Bitmap clipped = createClippedBitmap(src, mask);
        c.drawBitmap(clipped, -blurOffset[0], -blurOffset[1], null);
        clipped.recycle();

        return dst;
    }

    /**
     * This helper function clips the object {@link Bitmap} with the mask Bitmap, and returns the
     * result as a new Bitmap
     *
     * @param object Object {@link Bitmap} to clip
     * @param mask   Mask {@link Bitmap} to clip the object with
     * @return a new {@link Bitmap} containing the clipped object
     */
    private static Bitmap createClippedBitmap(Bitmap object, Bitmap mask) {
        Bitmap clipped = Bitmap.createBitmap(object.getWidth(), object.getHeight(),
                Config.ARGB_8888);
        Canvas c = new Canvas(clipped);
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
        c.drawBitmap(mask, 0, 0, null);
        p.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.SRC_IN));
        c.drawBitmap(object, 0, 0, p);
        return clipped;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        controller.handleOnActivityResult(requestCode, resultCode, data);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        super.dispatchTouchEvent(event);

        /* Do not dismiss any hints for MotionEvent.ACTION_DOWN when helpButton is pressed */
        if (helpButton.isPressed()) {
            return false;
        }
        if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
            controller.dismissHint();
        }
        return false;
    }

    /*
     * This function will calculate the amount of scaling needed on the bg bitmap image to show it
     * full screen along with the translation needed to fit it to center of screen & maintain aspect
     * ratio of the bitmap image image
     */
    Matrix getViewPortAdjustMatrix() {
        Matrix m = new Matrix();
        RectF currImgBounds = getViewPortImageBounds();
        float scale = getViewPortImageScaling();

        m.setScale(scale, scale);
        m.postTranslate(currImgBounds.left, currImgBounds.top);
        return m;
    }

    /*
     * calculate the background image bounds when its made fit to current screen size along with
     * maintaining aspect ratio of image. Normally in Landscape image would fit height but not width
     * and an x-offset need to be provided to make it place in the center of screen along x-axis and
     * vice-versa in portrait mode
     */
    RectF getViewPortImageBounds() {
        RectF currImgBounds;
        int w = model.mBackground.getWidth();
        int h = model.mBackground.getHeight();
        float scale = getViewPortImageScaling();

        w = (int) (w * scale);
        h = (int) (h * scale);
        float left = (currentLayoutBounds.width() - w) / 2;
        float top = (currentLayoutBounds.height() - h) / 2;
        currImgBounds = new RectF(left, top, left + w, top + h);
        return currImgBounds;
    }

    /*
     * calculate and return the amount of scaling required to show the background image bitmap on to
     * current screen resolution
     */
    float getViewPortImageScaling() {
        int w = model.mBackground.getWidth();
        int h = model.mBackground.getHeight();

        float scaleX = currentLayoutBounds.width() / w;
        float scaleY = currentLayoutBounds.height() / h;
        float scale = Math.min(scaleX, scaleY);
        return scale;
    }
}
