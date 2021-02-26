/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.translatescaleimageview;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.widget.ImageView;

import com.qualcomm.qti.qces.imageedit.objectclone.ObjectTransformation;
import com.qualcomm.qti.qces.imageedit.softcut.R;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * This class implements an {@link ImageView} which the user can touch to translate and/or scale the
 * contained {@link Drawable}. For ideal results, it should be sized to cover the entire area that
 * the translated/scaled image can occupy.
 *
 * @author slaver
 * @attr ref R.styleable#TranslateScaleImageView_hitRectPadding
 * @attr ref R.styleable#TranslateScaleImageView_minHitRectSize
 */
public class TranslateScaleImageView extends ImageView {
    static float totalAngle = 0;

    /**
     * Listener used to notify an interested party when a translate/scale operation starts and/or
     * ends
     */
    public interface OnTranslateScaleListener {
        /**
         * Invoked by the {@link TranslateScaleImageView} when a translate/scale begins
         *
         * @param view {@link TranslateScaleImageView} on which the translate/scale is targeted
         */
        public void onTranslateScaleStarted(TranslateScaleImageView view);

        /**
         * Invoked by the {@link TranslateScaleImageView} when a translate/scale end
         *
         * @param view {@link TranslateScaleImageView} on which the translate/scale is targeted
         */
        public void onTranslateScaleFinished(TranslateScaleImageView view);
    }

    private ArrayList<ObjectTransformation> undoObjTransformation;
    private ArrayList<ObjectTransformation> redoObjTransformation;

    /**
     * Helper {@link Runnable} which only allows a single instance to be queued for execution at a
     * time.
     * <p>
     * Note: this class is not thread-safe; it should only be used within the context of the UI
     * thread.
     */
    private abstract class SingleInstanceRunnable implements Runnable {
        private boolean mDirty;

        /**
         * {@inheritDoc}
         */
        @Override
        public void run() {
            mDirty = false;
        }

        /**
         * If not already queued, queues the {@link SingleInstanceRunnable} to run after the
         * specified duration
         *
         * @param delayMillis duration is ms after which {@link #run()} should be invoked
         */
        public final void queue(int delayMillis) {
            if (!mDirty) {
                mDirty = true;
                TranslateScaleImageView.this.postDelayed(this, delayMillis);
            }
        }

        /**
         * Tests if this {@link SingleInstanceRunnable} is currently queued for execution
         *
         * @return
         */
        public final boolean isQueued() {
            return mDirty;
        }
    }

    /**
     * Combination {@link GestureDetector.OnGestureListener}/
     * {@link ScaleGestureDetector.OnScaleGestureListener}. The two operations are related for this
     * View, and scaling overrides translation. It uses a {@link SingleInstanceRunnable} to queue
     * notifications to prevent flooding the UI thread with frequent translate/scales (and thus
     * invalidating and repainting the view repeatedly)
     */
    private final class TranslateScaleGestureListener implements GestureDetector.OnGestureListener,
            ScaleGestureDetector.OnScaleGestureListener {

        // Can't use enums in inner classes :/
        private static final int INACTIVE = 0;
        private static final int TRANSLATE = 1;
        private static final int SCALE_ROTATE = 2;

        private int mode;
        private float prevFocusX, prevFocusY;
        float prevAngle = 0f; // angle with respect to X axis
        float prevSpan = 0f;

        /**
         * {@inheritDoc}
         */
        @Override
        public boolean onDown(MotionEvent e) {
            if (mode == INACTIVE && mAdjustedHitRect.contains(e.getX(), e.getY())) {
                mode = TRANSLATE;
            }
            return (mode != INACTIVE);
        }

        public void onPointerUp(MotionEvent e) {
            /*
             * the onPointerUp call back was for the 2nd pointer, i.e when 2nd finger was lifted up
             * (now only one finger touched on screen), e.getPointerCount will be 2 and NOT 1
             */
            if (e.getPointerCount() == 2) {
                mode = TRANSLATE;
            }
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            /**
             * This condition checks to see if any of the 1st or 2nd finger is outside the viewport
             * then do not preform the scroll, if 2 fingers are touched then check if both fingers
             * are within the viewport else assuming only one finger and that should be within
             * the viewport
             */
            if ((e2.getPointerCount() == 2 &&
                    (!isWithinViewPortBounds(e2.getX(0), e2.getY(0)) ||
                            !isWithinViewPortBounds(e2.getX(1), e2.getY(1))))
                    || (!isWithinViewPortBounds(e2.getX(0), e2.getY(0)))) {
                return false;
            }

            if (mode == TRANSLATE && e2.getPointerCount() == 1) {
                mTransformationMatrix.postTranslate(-distanceX, -distanceY);
                mUpdateMatrix.queue(10);
            }

            /*
             * Two fingers are touched on the screen
             */
            if (e2.getPointerCount() == 2) {
                float midX = ((e2.getX(0) + e2.getX(1)) / 2);
                float midY = ((e2.getY(0) + e2.getY(1)) / 2);

                /*
                 * Only start a scale and/or rotate if the focus point falls inside the hit rect, or
                 * if a translate is already active (the case where a second finger is laid down
                 * during a translate, too far to be in the hit rect, but the object should be
                 * considered "active")
                 */

                if (mode != SCALE_ROTATE &&
                        (mode == TRANSLATE || mAdjustedHitRect.contains(prevFocusX, prevFocusY))) {
                    /*
                     * Just about to enter SCALE+ROTATION mode, this is similar to onScaleBegin(),
                     * calculate the focusXY, span & rotate when both the fingers were touched for
                     * the first time
                     */
                    prevAngle = rotation(e2);
                    prevFocusX = midX;
                    prevFocusY = midY;
                    prevSpan = getSpan(e2);
                    if (mode == TRANSLATE) {
                        mode = SCALE_ROTATE;
                    }

                } else if (mode == SCALE_ROTATE) {
                    // Scaling
                    float sf = getScaleFactor(e2, prevSpan);
                    mTransformationMatrix.postScale(sf, sf);
                    prevSpan = getSpan(e2);

                    // Rotation
                    float currAngle = rotation(e2);
                    mTransformationMatrix.postRotate((currAngle - prevAngle), midX, midY);

                    totalAngle += (currAngle - prevAngle);
                    //Log.i("test", "stored total Angle = " + totalAngle + " calculated = " + calculateDegreeOfRotation(mTransformationMatrix));
                    prevAngle = currAngle;

                    // Translation
                    mTransformationMatrix.postTranslate(-(prevFocusX * sf - midX), -(prevFocusY *
                            sf - midY));
                    prevFocusX = midX;
                    prevFocusY = midY;

                    mUpdateMatrix.queue(10);
                }
            }
            return true;
        }

        /**
         * {@inheritDoc}
         */
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            return false;
        }

        /**
         * {@inheritDoc}
         */
        public boolean onScale(ScaleGestureDetector detector) {
            return false;
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
        }

        float getScaleFactor(MotionEvent e2, float prevSpan) {
            float currSpan = getSpan(e2);
            return (prevSpan != 0) ? currSpan / prevSpan : 1;
        }

        /**
         * calculates the distance b/w the 2 points, should be called only if the second pointer is
         * touched on the screen else would hit a null pointer exception when accessing e2.getY(1).
         *
         * @param e2 The move motion event that triggered the current onScroll
         * @return distance between the 2 points
         */
        float getSpan(MotionEvent e2) {
            float dx = Math.abs(e2.getX(0) - e2.getX(1));
            float dy = Math.abs(e2.getY(0) - e2.getY(1));
            float distance = (float) Math.sqrt(dx * dx + dy * dy);
            return distance;
        }

        /**
         * Calculates the angle between the line made by the 2 points (in e2) and X axis
         *
         * @param e2 The move motion event that triggered the current onScroll
         * @return angle in Degrees
         */
        private float rotation(MotionEvent e2) {
            double theta = Math.PI;
            float thetaDegrees;
            theta = Math.atan2(e2.getY(0) - e2.getY(1), e2.getX(0) - e2.getX(1));
            thetaDegrees = (float) Math.toDegrees(theta) % 360;
            return thetaDegrees;
        }

        /**
         * Tests if either a translate or a scale is currently active.
         *
         * @return true if either operation is active, false otherwise
         */
        public boolean isActive() {
            return (mode != INACTIVE);
        }

        /**
         * Flags that neither a translate nor a scale is currently active. This is necessary because
         * the {@link GestureDetector.OnGestureListener} does not provide a function that is called
         * when the gesture is complete.
         */
        public void setInactive() {
            mode = INACTIVE;
        }

        @Override
        public void onShowPress(MotionEvent e) {
            // TODO Auto-generated method stub

        }

        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public void onLongPress(MotionEvent e) {
            // TODO Auto-generated method stub

        }

        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            // TODO Auto-generated method stub
            return false;
        }
    }


    /**
     * Flip the object horizontally or vertically
     */
    public void horizontalFlipObject() {

        if (mOnTranslateScaleListener != null) {
            mOnTranslateScaleListener.onTranslateScaleStarted(this);
        }

        float[] currentTransformation = calculateDegreeOfRotation(mTransformationMatrix);
        float currentScaleX = currentTransformation[0];
        float currentRotatedAngle = currentTransformation[2];

        //Rotate back to 0 degree angle to do the flip & after flipping apply this rotation again
        mTransformationMatrix.postRotate(currentRotatedAngle, mAdjustedHitRect.centerX(), mAdjustedHitRect.centerY());
        applyTranslateScaleMatrix();

        mTransformationMatrix.setHorizontalFlipState(!mTransformationMatrix.getHorizontalFlipState());

        //preScale -1 will flip the object w.r.t that axis
        mTransformationMatrix.preScale(-1.0f, 1.0f);

        float paddingX = mHitRectPadding * currentScaleX;
        if (mTransformationMatrix.getHorizontalFlipState()) {
            mTransformationMatrix.postTranslate((mAdjustedHitRect.width() - (2 * paddingX)), 0);
        } else {
            mTransformationMatrix.postTranslate(-(mAdjustedHitRect.width() - (2 * paddingX)), 0);
        }
        applyTranslateScaleMatrix();
        mTransformationMatrix.postRotate(currentRotatedAngle, mAdjustedHitRect.centerX(), mAdjustedHitRect.centerY());
        applyTranslateScaleMatrix();
        mOnTranslateScaleListener.onTranslateScaleFinished(this);
    }

    public void verticalFlipObject() {

        if (mOnTranslateScaleListener != null) {
            mOnTranslateScaleListener.onTranslateScaleStarted(this);
        }

        float[] currentTransformation = calculateDegreeOfRotation(mTransformationMatrix);
        float currentScaleY = currentTransformation[1];
        float currentRotatedAngle = currentTransformation[2];

        //Rotate back to 0 degree angle to do the flip & after flipping apply this rotation again
        mTransformationMatrix.postRotate(currentRotatedAngle, mAdjustedHitRect.centerX(), mAdjustedHitRect.centerY());
        applyTranslateScaleMatrix();

        mTransformationMatrix.setVerticalFlipState(!mTransformationMatrix.getVerticalFlipState());
        //preScale -1 will flip the object w.r.t that axis
        mTransformationMatrix.preScale(1.0f, -1.0f);

        float paddingY = mHitRectPadding * currentScaleY;
        if (mTransformationMatrix.getVerticalFlipState()) {
            mTransformationMatrix.postTranslate(0, (mAdjustedHitRect.height() - (2 * paddingY)));
        } else {
            mTransformationMatrix.postTranslate(0, -(mAdjustedHitRect.height() - (2 * paddingY)));
        }

        applyTranslateScaleMatrix();
        mTransformationMatrix.postRotate(currentRotatedAngle, mAdjustedHitRect.centerX(), mAdjustedHitRect.centerY());
        applyTranslateScaleMatrix();
        mOnTranslateScaleListener.onTranslateScaleFinished(this);
    }

    /**
     * This class is used to parcel/unparcel the state of this view, to preserve it across the
     * Android app lifecycle
     */
    private static class SavedState extends View.BaseSavedState {
        ObjectTransformation transformation;

        ArrayList<ObjectTransformation> undo;
        ArrayList<ObjectTransformation> redo;

        /**
         * Create a new instance of the saved state
         *
         * @param wrappedParcelable The saved state of the superclass
         */
        public SavedState(Parcelable wrappedParcelable) {
            super(wrappedParcelable);
        }

        /**
         * Private constructor to recreate the saved state from a {@link Parcel}
         *
         * @param parcel {@link Parcel} from which to recreate saved state
         */
        private SavedState(Parcel parcel) {
            super(parcel);
            undo = new ArrayList<ObjectTransformation>();
            redo = new ArrayList<ObjectTransformation>();
            float[] values = new float[9];

            float[] stackValues = parcel.createFloatArray();

            int stackValuesIndex = stackValues.length - 1;

            // Last item in the parcel was size of undo stack.
            int undoSize = (int) stackValues[stackValuesIndex--];

            transformation = new ObjectTransformation();

            //the first 2(which was stored lastly while saving), first one is  horizontalFlip state and then vertical flip state
            transformation.setHorizontalFlipState(stackValues[stackValuesIndex--] > 0 ? true : false);
            transformation.setVerticalFlipState(stackValues[stackValuesIndex--] > 0 ? true : false);
            /*
             * restore the transformation matrix (the next 9 elements from last is the current
             * transformation matrix)
             */
            for (int i = 8; i >= 0; i--) {
                values[i] = stackValues[stackValuesIndex--];
            }
            //9 values are for matrix
            transformation.setValues(values);

            /*
             * Restore the redoStack (next (multiples of (9+2)) in reverse order is the transformation
             * matrix in the redo stack until undoSize * (9+2 flip state) elements remaining in the parcel
             */
            int matrixIndex = 0;
            boolean verticalFlipState = false, horizontalFlipState = false;
            for (; stackValuesIndex >= (undoSize * (9 + 2)); stackValuesIndex--) {

                if (matrixIndex == 0) {
                    verticalFlipState = stackValues[stackValuesIndex--] > 0;
                    horizontalFlipState = stackValues[stackValuesIndex--] > 0;
                }

                values[8 - matrixIndex] = stackValues[stackValuesIndex];
                matrixIndex++;

                if (matrixIndex == 9) {
                    ObjectTransformation m = new ObjectTransformation();
                    m.setValues(values);
                    m.setHorizontalFlipState(horizontalFlipState);
                    m.setVerticalFlipState(verticalFlipState);
                    redo.add(0, m); // adding at index 0 into the arrayList always will make sure we
                    // restore the stack in reverse order
                    matrixIndex = 0;
                }
            }

            // restore the undoStack
            matrixIndex = 0;
            for (int parcelIndex = (undoSize * (9 + 2) - 1); parcelIndex >= 0; parcelIndex--) {
                if (matrixIndex == 0) {
                    verticalFlipState = stackValues[parcelIndex--] > 0;
                    horizontalFlipState = stackValues[parcelIndex--] > 0;
                }

                values[8 - matrixIndex] = stackValues[parcelIndex];
                matrixIndex++;

                if (matrixIndex == 9) {
                    ObjectTransformation m = new ObjectTransformation();
                    m.setValues(values);
                    m.setHorizontalFlipState(horizontalFlipState);
                    m.setVerticalFlipState(verticalFlipState);
                    undo.add(0, m);
                    matrixIndex = 0;
                }
            }
        }

        /**
         * {@inheritDoc}
         */
        @Override
        public void writeToParcel(Parcel dest, int flags) {
            super.writeToParcel(dest, flags);

            /**
             * multiply by (9 matrix elements + 2 flip states) as every matrix in stack
             * has 11 elements
             */
            int count = (undo.size() + redo.size()) * (9 + 2);
            /**
             * +9 for transformationMatrix + 2 flip states and +1 to store the undoStackSize
             */

            float[] stackValues = new float[count + 9 + 2 + 1];
            float[] values = new float[9];

            // parcel the undo stack first ((9+2) x undoStack size elements)
            Iterator<ObjectTransformation> it = undo.iterator();
            int stackValuesCounter = 0;
            ObjectTransformation objTrans;
            while (it.hasNext()) {
                objTrans = it.next();
                objTrans.getValues(values);
                for (int i = 0; i < 9; i++) {
                    stackValues[stackValuesCounter++] = values[i];
                }
                //store 1.0f if flip state is true else 0.0f
                stackValues[stackValuesCounter++] = objTrans.getHorizontalFlipState() ? 1f : 0f;
                stackValues[stackValuesCounter++] = objTrans.getVerticalFlipState() ? 1f : 0f;
            }

            // parcel redo stack 2nd
            it = redo.iterator();
            while (it.hasNext()) {
                objTrans = it.next();
                objTrans.getValues(values);
                for (int i = 0; i < 9; i++) {
                    stackValues[stackValuesCounter++] = values[i];
                }
                stackValues[stackValuesCounter++] = objTrans.getHorizontalFlipState() ? 1f : 0f;
                stackValues[stackValuesCounter++] = objTrans.getVerticalFlipState() ? 1f : 0f;
            }

            // parcel the transformation matrix 3rd
            transformation.getValues(values);
            for (int i = 0; i < 9; i++) {
                stackValues[stackValuesCounter++] = values[i];
            }
            stackValues[stackValuesCounter++] = transformation.getHorizontalFlipState() ? 1f : 0f;
            stackValues[stackValuesCounter++] = transformation.getVerticalFlipState() ? 1f : 0f;

            // last element is size of undoStack
            stackValues[stackValuesCounter] = undo.size();
            dest.writeFloatArray(stackValues);
        }

        /**
         * @see {@link Parcelable}
         */
        @SuppressWarnings("unused")
        public static final Parcelable.Creator<SavedState> CREATOR = new Parcelable.Creator<SavedState>() {
            public SavedState createFromParcel(Parcel source) {
                return new SavedState(source);
            }

            public SavedState[] newArray(int size) {
                return new SavedState[size];
            }
        };
    }

    // View attributes
    private RectF mUserHitRect;
    private int mMinHitRectSize;
    private int mHitRectPadding;
    private OnTranslateScaleListener mOnTranslateScaleListener;

    // Touch motion parameters
    private RectF mReferenceHitRect;
    private ObjectTransformation mTransformationMatrix;

    /*
     * Matrix which has info about scaling and translation to be applied for 1st time when screen
     * size changes to convert from bitmap size to current screen size
     */
    private Matrix mViewPortAdjustMatrix;

    /**
     * mViewPortBoundingBox is the Rectangle box for the current viewport
     */
    private RectF mViewPortBoundingBox;

    GestureDetector mTranslateGesture;
    ScaleGestureDetector mScaleGesture;
    TranslateScaleGestureListener mGestureListener = new TranslateScaleGestureListener();

    private RectF mCachedDrawableDimensions;
    private RectF mAdjustedHitRect;

    SingleInstanceRunnable mUpdateMatrix = new SingleInstanceRunnable() {
        @Override
        public void run() {
            super.run();
            TranslateScaleImageView.this.applyTranslateScaleMatrix();
        }
    };

    /**
     * Invokes {@link #TranslateScaleImageView(Context, AttributeSet, int) with no attributes
     * or default style
     *
     * @param context
     */
    public TranslateScaleImageView(Context context) {
        this(context, null, 0);
    }

    /**
     * Invokes {@link #TranslateScaleImageView(Context, AttributeSet, int) with provided
     * attributes but no default style
     *
     * @param context
     * @param attrs
     */
    public TranslateScaleImageView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Creates a new instance of {@link TranslateScaleImageView} with the provided attributes and
     * style
     *
     * @param context
     * @param attrs
     * @param defStyle
     */
    public TranslateScaleImageView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.TranslateScaleImageView,
                0, R.style.TranslateScaleImageViewDefaultStyle);
        mMinHitRectSize = a.getDimensionPixelSize(
                R.styleable.TranslateScaleImageView_minHitRectSize, 0);
        mHitRectPadding = a.getDimensionPixelSize(
                R.styleable.TranslateScaleImageView_hitRectPadding, 0);
        a.recycle();

        mTranslateGesture = new GestureDetector(context, mGestureListener);
        mTranslateGesture.setIsLongpressEnabled(false);
        mScaleGesture = new ScaleGestureDetector(context, mGestureListener);

        this.setScaleType(ScaleType.MATRIX);
        this.resetDrawableState();

        /* initialize undo and redo stack */
        undoObjTransformation = new ArrayList<ObjectTransformation>();
        redoObjTransformation = new ArrayList<ObjectTransformation>();
    }

    /**
     * Helper function to reset the state of the encompassed {@link Drawable}
     */
    private void resetDrawableState() {
        RectF oldDimensions = mCachedDrawableDimensions;

        mCachedDrawableDimensions = new RectF();
        Drawable d = this.getDrawable();
        if (d != null) {
            mCachedDrawableDimensions.right = d.getIntrinsicWidth();
            mCachedDrawableDimensions.bottom = d.getIntrinsicHeight();
            if (mCachedDrawableDimensions.right <= 0 || mCachedDrawableDimensions.bottom <= 0) {
                // Drawable has no intrinsic size, use the bounds specified by ImageView instead
                Rect bounds = d.getBounds();
                mCachedDrawableDimensions.right = bounds.right;
                mCachedDrawableDimensions.bottom = bounds.bottom;
            }
        }

        if (oldDimensions == null || !oldDimensions.equals(mCachedDrawableDimensions)) {
            this.resetTranslateScaleState();
        }
    }

    /**
     * Helper function to reset the state of the translate/scale operation applied to the
     * encompassed {@link Drawable}
     */
    private void resetTranslateScaleState() {
        mTransformationMatrix = new ObjectTransformation();
        mViewPortAdjustMatrix = new Matrix();
        mReferenceHitRect = null;
        mAdjustedHitRect = new RectF();
        this.applyTranslateScaleMatrix();
    }

    /**
     * Helper function to update the {@link ImageView}'s {@link Drawable} {@link Matrix} with the
     * current translate/scale parameters
     */
    public void applyTranslateScaleMatrix() {
        this.setImageMatrix(mTransformationMatrix);
        mTransformationMatrix.mapRect(mAdjustedHitRect, this.getReferenceHitRect());
        float w = Math.max(0, mMinHitRectSize - mAdjustedHitRect.width());
        float h = Math.max(0, mMinHitRectSize - mAdjustedHitRect.height());
        mAdjustedHitRect.inset(-w / 2, -h / 2);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setImageBitmap(Bitmap bm) {
        super.setImageBitmap(bm);
        this.resetDrawableState();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setImageDrawable(Drawable drawable) {
        super.setImageDrawable(drawable);
        this.resetDrawableState();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setImageLevel(int level) {
        super.setImageLevel(level);
        this.resetDrawableState();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setImageResource(int resId) {
        super.setImageResource(resId);
        this.resetDrawableState();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void setImageURI(Uri uri) {
        super.setImageURI(uri);
        this.resetDrawableState();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);

        // If both oldw and oldh are 0, we are just being attached to the view hierarchy. Don't
        // discard
        // our translate/scale state, since it may have been restored in onRestoreInstanceState.
        if (oldw != 0 || oldh != 0) {
            this.resetTranslateScaleState();
        }
    }

    public void setObjTransformation(ObjectTransformation trans) {
        mTransformationMatrix.set(trans);
    }

    public ObjectTransformation getObjTransformation() {
        return mTransformationMatrix;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        super.onTouchEvent(event);

        boolean wasTouchActive = mGestureListener.isActive();
        int actionMasked = event.getActionMasked();

        // Do scale first, since it will override a translate
        mScaleGesture.onTouchEvent(event);
        mTranslateGesture.onTouchEvent(event);

        switch (actionMasked) {
            case MotionEvent.ACTION_POINTER_UP:
                mGestureListener.onPointerUp(event);
                break;

            case MotionEvent.ACTION_CANCEL:
                // !!! FALL-THROUGH !!!

            case MotionEvent.ACTION_UP:
                if (mGestureListener.isActive()) {
                    mGestureListener.setInactive();
                    if (mOnTranslateScaleListener != null) {
                        // If there is a delayed apply matrix scheduled, remove it and process
                        // immediately.
                        // This ensures matrices are up-to-date before we tell our listener we are done.
                        if (mUpdateMatrix.isQueued()) {
                            this.getHandler().removeCallbacks(mUpdateMatrix);
                            mUpdateMatrix.run();
                        }
                        mOnTranslateScaleListener.onTranslateScaleFinished(this);
                    }
                }
                break;

            default:
                // N.B. A scale can begin on a ACTION_MOVE where we are not in translate mode, if
                // the first touch missed the hit rect. Check all events that aren't a guaranteed
                // end to figure out whether or not to hit the start listener.
                if (!wasTouchActive && mGestureListener.isActive() && mOnTranslateScaleListener != null) {
                    mOnTranslateScaleListener.onTranslateScaleStarted(this);
                }
                break;
        }

        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected Parcelable onSaveInstanceState() {
        SavedState ss = new SavedState(super.onSaveInstanceState());
        ss.transformation = getCurrentOrientationToBitmapTransformedMatrix(mTransformationMatrix);
        ss.undo = undoObjTransformation;
        ss.redo = redoObjTransformation;
        return ss;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void onRestoreInstanceState(Parcelable state) {
        if (!(state instanceof SavedState)) {
            throw new IllegalArgumentException("Unexpected type for state. Expected SavedState.");
        }

        SavedState ss = (SavedState) state;
        super.onRestoreInstanceState(ss.getSuperState());
        mTransformationMatrix = getBitMapToCurrentOrientationTransformedMatrix(ss.transformation);
        undoObjTransformation = ss.undo;
        redoObjTransformation = ss.redo;
    }

    /**
     * Set the hit rect for the given {@link Drawable}. The hit rect may be larger than the drawable
     * itself
     *
     * @param hitRect
     */
    public void setDrawableHitRect(RectF hitRect) {
        if (hitRect != null) {
            mUserHitRect = new RectF(hitRect);
        } else {
            mUserHitRect = null;
        }
        mReferenceHitRect = null;
    }

    /**
     * Set the initial translate/scale parameters to position the object within the specified
     * {@link RectF}
     *
     * @param location location at which to initially position the {@link Drawable}
     */
    public void translateScaleTo(float dx, float dy, float scale) {
        if (mTransformationMatrix.isIdentity()) {
            mTransformationMatrix.postScale(scale, scale);
            mTransformationMatrix.postRotate(0);
            mTransformationMatrix.postTranslate(dx, dy);
        }
        mTransformationMatrix = getBitMapToCurrentOrientationTransformedMatrix(mTransformationMatrix);
        this.applyTranslateScaleMatrix();
    }

    /**
     * Helper function to get the hit rect, after applying any additional hit rect padding as
     * specified by attributes
     *
     * @return a {@link RectF} representing the modified hit rect. Note that this is a shared
     * instance - do not modify it.
     */
    private RectF getReferenceHitRect() {
        if (mReferenceHitRect == null) {
            mReferenceHitRect = (mUserHitRect == null ? new RectF(this.getDrawableDimensions())
                    : mUserHitRect);
            mReferenceHitRect.inset(-mHitRectPadding, -mHitRectPadding);
        }
        return mReferenceHitRect;
    }

    /**
     * Helper function to get the dimensions of the {@link Drawable}. See
     * {@link #resetDrawableState()} for the calculation of dimensions
     *
     * @return a {@link RectF} containing the dimensions of the {@link Drawable}. Note that this is
     * a shared instance - do not modify it.
     */
    private RectF getDrawableDimensions() {
        return mCachedDrawableDimensions;
    }

    /**
     * Register a {@link OnTranslateScaleListener}. If there was a previous listener registered, it
     * will be discared
     *
     * @param listener {@link OnTranslateScaleListener} to invoke when translate/ scale events occur
     */
    public void setOnTranslateScaleListener(OnTranslateScaleListener listener) {
        mOnTranslateScaleListener = listener;
    }

    private void pushLastTransformation(ArrayList<ObjectTransformation> stack, ObjectTransformation stackObj) {
        stackObj = getCurrentOrientationToBitmapTransformedMatrix(stackObj);
        stack.add(stackObj);
    }

    private ObjectTransformation popLastTransformation(ArrayList<ObjectTransformation> stack) {
        ObjectTransformation lastTransformation = null;
        if (stack.size() > 0) {
            lastTransformation = stack.get(stack.size() - 1);
            lastTransformation = getBitMapToCurrentOrientationTransformedMatrix(lastTransformation);
            stack.remove(stack.size() - 1);
        }
        return lastTransformation;
    }

    public void undoObjTransformation() {
        updateObjTransformation(undoObjTransformation, redoObjTransformation);
    }

    public void redoObjTransformation() {
        updateObjTransformation(redoObjTransformation, undoObjTransformation);
    }

    private void updateObjTransformation(ArrayList<ObjectTransformation> fromStack, ArrayList<ObjectTransformation> toStack) {
        ObjectTransformation lastTransformation = popLastTransformation(fromStack);
        if (lastTransformation != null) {
            pushLastTransformation(toStack, new ObjectTransformation(mTransformationMatrix));
            setObjTransformation(lastTransformation);
            applyTranslateScaleMatrix();
            mOnTranslateScaleListener.onTranslateScaleFinished(this);
        }
    }

    public void pushUndoLastObjTransformation() {
        pushLastTransformation(undoObjTransformation, new ObjectTransformation(mTransformationMatrix));
        /* clear redostack if any new operation is performed */
        redoObjTransformation.clear();
    }

    public int getUndoObjTransformationCount() {
        return undoObjTransformation.size();
    }

    public int getRedoObjTransformationCount() {
        return redoObjTransformation.size();
    }

    public void clearUndoRedoStack() {
        undoObjTransformation.clear();
        redoObjTransformation.clear();
    }

    public void setViewPortOffsetAdjustMatrix(Matrix m) {
        mViewPortAdjustMatrix = new Matrix();
        mViewPortAdjustMatrix.set(m);
    }

    /**
     * this setter method will set the current viewport bounding box, which is used to control the
     * object to not to be scrolled out side of the viewport
     *
     * @param boundingBox
     */
    public void setViewPortBoundingBox(RectF boundingBox) {
        mViewPortBoundingBox = new RectF(boundingBox);
    }

    private boolean isWithinViewPortBounds(float x, float y) {
        return mViewPortBoundingBox.contains(x, y);
    }

    /**
     * @Input is transformation matrix in Bitmap size co-ordinate system, which will be converted to
     * current screen size co-ordinate system.
     * @Output: is the transformation matrix which can be applied on current screen size
     */
    public ObjectTransformation getBitMapToCurrentOrientationTransformedMatrix(ObjectTransformation m) {
        ObjectTransformation translatedMatrix = new ObjectTransformation(m);
        translatedMatrix.postConcat(mViewPortAdjustMatrix);
        return translatedMatrix;
    }

    /**
     * This is opposite (or inverse ) of @function getBitMapToCurrentOrientationTransformedMatrix()
     *
     * @Input is transformation matrix in current screen size co-ordinate system, which will be
     * converted to bitmap size co-ordinate system.
     * @Output: is the transformation matrix which can be applied on bitmap size
     */
    public ObjectTransformation getCurrentOrientationToBitmapTransformedMatrix(ObjectTransformation m) {
        ObjectTransformation translatedMatrix = new ObjectTransformation(m);
        Matrix mAdjustInverse = new Matrix();
        mViewPortAdjustMatrix.invert(mAdjustInverse);
        translatedMatrix.postConcat(mAdjustInverse);
        return translatedMatrix;
    }


    float[] calculateDegreeOfRotation(ObjectTransformation matrix) {
        float[] v = new float[9];

        int horizontalFlipComponsation = 1; //1 means not yet flipped, -1 means its flipped;
        int verticalFlipComponsation = 1;

        if (mTransformationMatrix.getVerticalFlipState()) {
            verticalFlipComponsation = -1;
        }
        if (mTransformationMatrix.getHorizontalFlipState()) {
            horizontalFlipComponsation = -1;
        }

        /**if  the image is already flipped the calculating angle of rotation will have a +/-180
         * offset, so best way is to get it to original(non flipped) state and then calculate angle
         * and we can flip it back.*/
        matrix.preScale(horizontalFlipComponsation * 1.0f, verticalFlipComponsation * 1.0f);
        matrix.getValues(v);

        //flippling it back
        matrix.preScale(horizontalFlipComponsation * 1.0f, verticalFlipComponsation * 1.0f);
        float tx = v[Matrix.MTRANS_X];
        float ty = v[Matrix.MTRANS_Y];
        float scalex = v[Matrix.MSCALE_X];
        float skewy = v[Matrix.MSKEW_Y];
        float skewx = v[Matrix.MSKEW_X];

        float rScale = (float) Math.sqrt(scalex * scalex + skewy * skewy);

        // calculate the degree of rotation
        float rAngle = (float) (Math.atan2(v[Matrix.MSKEW_X], v[Matrix.MSCALE_X]) * (180 / Math.PI));
        float retVal[] = new float[5];
        retVal[0] = v[Matrix.MSCALE_X]; //[0] is Scale in X
        retVal[1] = v[Matrix.MSCALE_Y]; //[1] is Scale in Y
        retVal[2] = (rAngle % 360); //[2] is  the angle of rotation in degrees
        retVal[3] = tx;
        retVal[4] = ty;
        return retVal;
    }

}


