/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.qualcomm.qti.qmmi.testcase.Camera;

import com.qualcomm.qti.qmmi.utils.LogUtils;

import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.Face;
import android.util.AttributeSet;
import android.view.Surface;
import android.widget.ImageView;

public class FaceView extends ImageView {
    private Paint mLinePaint;
    private Face[] mFaces;
    private Matrix mMatrix = new Matrix();
    private RectF mRect = new RectF();
    private String mCameraId;
    private Context mContext;
    private int mUncroppedWidth;
    private int mUncroppedHeight;

    public FaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
        initPaint();
    }

    public void setFaces(Face[] faces, String cameraId) {
        this.mFaces = faces;
        this.mCameraId = cameraId;
        invalidate();
    }

    public void clearFaces() {
        mFaces = null;
        invalidate();
    }

    public void setSurfaceTextureSize(int uncroppedWidth, int uncroppedHeight) {
        mUncroppedWidth = uncroppedWidth;
        mUncroppedHeight = uncroppedHeight;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (mFaces == null || mFaces.length < 1) {
            return;
        }

        int rw, rh;
        rw = mUncroppedWidth;
        rh = mUncroppedHeight;

        int displayOrientation = getDisplayOrientation();
        if (((rh > rw) && ((displayOrientation == 0) || (displayOrientation == 180)))
                || ((rw > rh) && ((displayOrientation == 90) || (displayOrientation == 270)))) {
            int temp = rw;
            rw = rh;
            rh = temp;
        }

        CameraCharacteristics characteristics = null;
        try {
            CameraManager cm = (CameraManager)mContext.getSystemService(Context.CAMERA_SERVICE);
            characteristics = cm.getCameraCharacteristics(mCameraId);
            Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
            if (facing == CameraCharacteristics.LENS_FACING_FRONT) {
                //Need mirror for front camera.
                prepareMatrix(mMatrix, true, displayOrientation, rw, rh);
            } else {
                prepareMatrix(mMatrix, false, displayOrientation, rw, rh);
            }
        } catch (CameraAccessException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        Rect cameraBounds = null;
        if (characteristics != null){
            cameraBounds = characteristics.get(
                    CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        }

        Matrix translateMatrix = new Matrix();
        translateMatrix.preTranslate(-cameraBounds.width() / 2f, -cameraBounds.height() / 2f);
        translateMatrix.postScale(2000f / cameraBounds.width(), 2000f / cameraBounds.height());

        int dx = (getWidth() - rw) / 2;
        int dy = (getHeight() - rh) / 2;

        // Focus indicator is directional. Rotate the matrix and the canvas
        // so it looks correctly in all orientations.
        canvas.save();
        //mMatrix.postRotate(mOrientation); // postRotate is clockwise
        //canvas.rotate(-mOrientation); // rotate is counter-clockwise (for canvas)

        float rectWidth;
        float rectHeight;
        float diameter;

        for (int i = 0; i < mFaces.length; i++) {
            Rect faceBound = mFaces[i].getBounds();
            faceBound.offset(-cameraBounds.left, -cameraBounds.top);
            mRect.set(faceBound);

            translateMatrix.mapRect(mRect);
            mMatrix.mapRect(mRect);
            mRect.offset(dx, dy);

            rectHeight = mRect.bottom-mRect.top;
            rectWidth = mRect.right - mRect.left;
            diameter = rectHeight > rectWidth ? rectWidth : rectHeight;

            canvas.drawCircle(mRect.centerX(), mRect.centerY(), diameter/2, mLinePaint);
        }

        canvas.restore();
        super.onDraw(canvas);
    }

    private int getDisplayOrientation(){
        int result = 0;

        Activity activity = (Activity)mContext;
        int rotation = activity.getWindowManager().getDefaultDisplay()
                .getRotation();
        int displayRotation = 0;

        switch (rotation) {
            case Surface.ROTATION_0: displayRotation = 0; break;
            case Surface.ROTATION_90: displayRotation = 90; break;
            case Surface.ROTATION_180: displayRotation = 180; break;
            case Surface.ROTATION_270: displayRotation = 270; break;
        }

        LogUtils.logd("displayRotation" + displayRotation);

        CameraCharacteristics characteristics = null;
        try {
            CameraManager cm = (CameraManager)mContext.getSystemService(Context.CAMERA_SERVICE);
            characteristics = cm.getCameraCharacteristics(mCameraId);
        } catch (CameraAccessException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        int orientation_offset = 0;
        if (characteristics != null){
            orientation_offset = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
        }

        Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
        //For front camera
        if (facing == CameraCharacteristics.LENS_FACING_FRONT) {
            result = (orientation_offset + displayRotation) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {
            result = (orientation_offset - displayRotation + 360) % 360;
        }

        LogUtils.logd("result" + result);

        return result;
    }

    private void initPaint() {
        mLinePaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mLinePaint.setColor(Color.RED);
        mLinePaint.setStyle(Style.STROKE);
        mLinePaint.setStrokeCap(Paint.Cap.ROUND);
        mLinePaint.setStrokeWidth(6f);
        mLinePaint.setAlpha(180);
    }

    private void prepareMatrix(Matrix matrix, boolean mirror, int displayOrientation, int viewWidth, int viewHeight) {
        matrix.setScale(mirror ? -1 : 1, 1);
        matrix.postRotate(displayOrientation);
        matrix.postScale(viewWidth / 2000f, viewHeight / 2000f);
        matrix.postTranslate(viewWidth / 2f, viewHeight / 2f);
    }
}
