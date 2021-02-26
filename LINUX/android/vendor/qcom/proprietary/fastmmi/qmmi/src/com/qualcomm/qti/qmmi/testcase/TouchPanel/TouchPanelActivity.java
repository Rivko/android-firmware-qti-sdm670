/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.TouchPanel;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Bundle;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.bean.TestCase;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;

import java.util.ArrayList;

public class TouchPanelActivity extends BaseActivity {
    static final String TOUCH_TRACE_FILE_NAME = "touch.track";
    ArrayList<EdgePoint> mArrayList;
    int mHightPix = 0, mWidthPix = 0, mRadius = 20, mStep = 0;
    float w = 0, h = 0;
    private final int X_MAX_POINTS = 16;
    private int mButtonX = 0;
    private int mButtonY = 0;
    private String mTouchTraceFile;
    StringBuffer mFileData = new StringBuffer();
    ArrayList<FileData> mDataLists = new ArrayList<FileData>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        LogUtils.logi( "touch activity create");
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        Display mDisplay = getWindowManager().getDefaultDisplay();
        mWidthPix = mDisplay.getWidth();
        mHightPix = mDisplay.getHeight();
        super.onCreate(savedInstanceState);

    }

    @Override
    protected int getLayoutId() {
        return R.layout.case_act;
    }

    @Override
    protected void onResume() {
        super.onResume();

        mStep = getStep(mWidthPix, mHightPix);
        mRadius = mStep / 2;
        mButtonX = mWidthPix / 2 - mStep;
        mButtonY = mHightPix / 2 - mStep;
        LogUtils.logi( "touchActivity, y:" + mHightPix + ",x:" + mWidthPix + " ,Step:" + mStep);
        setContentView(new Panel(this));
        mTouchTraceFile = getApplicationContext().getFilesDir().getAbsolutePath() + "/" + TOUCH_TRACE_FILE_NAME;
        mTestCase.addTestData("track_filename", mTouchTraceFile);
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        for (int i = 0; i < mDataLists.size(); i++) {
            FileData fileData = mDataLists.get(i);
            if (mFileData.length() > FileUtils.TRACE_FILE_BUFFER_SIZE) {
                mFileData.delete(0, mFileData.length());
            }
            mFileData.append(fileData.x + "," + fileData.y + ";");
        }
        FileUtils.writeFile(mTouchTraceFile, mFileData.toString());
        super.onDestroy();
    }

    public class EdgePoint {

        int x;
        int y;
        boolean isChecked = false;

        public EdgePoint(int x, int y, boolean isCheck) {

            this.x = x;
            this.y = y;
            this.isChecked = isCheck;
        }
    }

    public class FileData {
        float x;
        float y;

        public FileData(float x, float y) {
            this.x = x;
            this.y = y;
        }
    }

    int getStep(int hightPix, int widthPix) {

        int MIN_STEP = widthPix / X_MAX_POINTS;
        int step = MIN_STEP;
        for (int i = MIN_STEP; i < widthPix / 5; i++) {
            if (hightPix % i == 0 && widthPix % i == 0)
                return i;
        }
        return step;
    }

    public ArrayList<EdgePoint> getTestPoint() {

        ArrayList<EdgePoint> list = new ArrayList<EdgePoint>();

        for (int x = mRadius; x < mWidthPix + mRadius; x += mStep) {
            for (int y = mRadius; y <= mHightPix; y += mStep) {
                if (x > mRadius && x < mWidthPix - mRadius && y > mRadius
                        && y < mHightPix - mStep)
                    continue;
                list.add(new EdgePoint(x, y, false));
            }
        }
        return list;
    }

    class Panel extends View {

        public static final int TOUCH_TRACE_NUM = 30;
        public static final int PRESSURE = 500;
        private TouchData[] mTouchData = new TouchData[TOUCH_TRACE_NUM];
        private int traceCounter = 0;
        private Paint mPaint = new Paint();

        public class TouchData {

            public float x;
            public float y;
            public float r;
        }

        public Panel(Context context) {
            super(context);
            mArrayList = getTestPoint();
            mPaint.setARGB(100, 100, 100, 100);
            for (int i = 0; i < TOUCH_TRACE_NUM; i++) {
                mTouchData[i] = new TouchData();
            }
        }

        private int getNext(int c) {
            int temp = c + 1;
            return temp < TOUCH_TRACE_NUM ? temp : 0;
        }

        public void onDraw(Canvas canvas) {
            super.onDraw(canvas);
            mPaint.setColor(Color.LTGRAY);
            mPaint.setTextSize(20);

            // paint the button
            mPaint.setColor(Color.RED);
            mPaint.setStyle(Paint.Style.FILL);
            canvas.drawRect(mButtonX, mButtonY, mButtonX + mStep * 2, mButtonY + mStep * 2, mPaint);

            //paint the text
            mPaint.setTextSize(40);
            mPaint.setColor(Color.BLACK);
            canvas.drawText("W: " + w, mButtonX + mRadius, mButtonY + mStep, mPaint);
            canvas.drawText("H: " + h, mButtonX + mRadius, mButtonY + mStep + mRadius, mPaint);
            canvas.drawText(getString(R.string.touch_fail), mButtonX + mRadius, mButtonY + mRadius, mPaint);

            //paint circle
            mPaint.setStrokeWidth(mRadius);
            for (int i = 0; i < mArrayList.size(); i++) {
                EdgePoint point = mArrayList.get(i);
                mPaint.setColor(Color.WHITE);
                canvas.drawCircle(point.x, point.y, mPaint.getStrokeWidth(),
                        mPaint);
            }

            for (int i = 0; i < TOUCH_TRACE_NUM; i++) {
                TouchData td = mTouchData[i];
                mPaint.setColor(Color.WHITE);
                if (td.r > 0) {
                    canvas.drawCircle(td.x, td.y, 2, mPaint);
                }
            }
            invalidate();
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            final int eventAction = event.getAction();
            w = event.getRawX();
            h = event.getRawY();
            if(eventAction == MotionEvent.ACTION_UP){
                mDataLists.add(new FileData(w, h));
            }
            if ((eventAction == MotionEvent.ACTION_MOVE)
                    || (eventAction == MotionEvent.ACTION_UP)) {

                for (int i = 0; i < mArrayList.size(); i++) {
                    EdgePoint point = mArrayList.get(i);
                    if (!point.isChecked
                            && ((w >= (point.x - mRadius)) && (w <= (point.x + mRadius)))
                            && ((h >= (point.y - mRadius)) && (h <= (point.y + mRadius)))) {
                        mArrayList.remove(i);
                        break;
                    }
                }
                if ((w >= mButtonX && w <= (mButtonX + mStep * 2)
                        && (h >= mButtonY && h <= (mButtonY + mStep * 2)))) {
                    feedbackResult(TestCase.STATE_FAIL);
                }

                if (mArrayList.isEmpty()) {
                    feedbackResult(TestCase.STATE_PASS);
                    finish();
                }

                TouchData tData = mTouchData[traceCounter];
                tData.x = event.getX();
                tData.y = event.getY();
                tData.r = event.getPressure() * PRESSURE;
                traceCounter = getNext(traceCounter);
                invalidate();
            }
            return true;
        }

    }
}
