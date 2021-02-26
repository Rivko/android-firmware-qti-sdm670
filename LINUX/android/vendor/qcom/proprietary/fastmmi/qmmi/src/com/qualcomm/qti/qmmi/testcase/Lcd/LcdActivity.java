/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qmmi.testcase.Lcd;

import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.qualcomm.qti.qmmi.R;
import com.qualcomm.qti.qmmi.framework.BaseActivity;
import com.qualcomm.qti.qmmi.utils.FileUtils;
import com.qualcomm.qti.qmmi.utils.LogUtils;
import com.qualcomm.qti.qmmi.utils.Utils;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class LcdActivity extends BaseActivity {
    List<Integer> mTestImg = new ArrayList<>();
    WindowManager.LayoutParams mLayoutParams;
    private int mImgSeq = 0;
    private RelativeLayout mRelativeLayout;
    private final String CASE_LCD_IMAGE = "LCD_IMAGE";
    private final String CASE_LCD = "LCD";
    boolean forLCDImage = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        LogUtils.logi("lcd activity create");
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        mLayoutParams = getWindow().getAttributes();
        mLayoutParams.screenBrightness = 1;
        getWindow().setAttributes(mLayoutParams);
        super.onCreate(savedInstanceState);
    }

    @Override
    protected int getLayoutId() {
        return R.layout.case_act;
    }

    @Override
    protected void onResume() {
        super.onResume();

        mTextView.setVisibility(View.GONE);
        mBtPass.setVisibility(View.GONE);
        mBtFail.setVisibility(View.GONE);
        mRelativeLayout = (RelativeLayout) findViewById(R.id.full_layout);

        String color = mTestCase.getParameter().get("color");
        String filePath = mTestCase.getParameter().get("file");
        String caseName = mTestCase.getName();

        //For LCD Image case
        if (caseName.equalsIgnoreCase(CASE_LCD_IMAGE)) {
            mTestImg.clear();

            //Check if defined file exists, If not, copy raw default png file into /sdcard/ and show
            File pngFile = new File(filePath);
            if ( !pngFile.exists() ){
                //copy the raw default png file into sdcard/
                LogUtils.logi("prepare copy");
                filePath = Utils.SDCARD_DIR.trim() + "qmmi.png";
                FileUtils.copy(getResources().openRawResourceFd(R.raw.qmmi), filePath);
            }

            Drawable image = Drawable.createFromPath(filePath);
            LogUtils.logd("Image file:" + filePath);
            if ( image != null){
                mRelativeLayout.setBackground(image);
                forLCDImage = true;
            } else {
                showNoImage();
            }

        } else if (color != null) {
            mTestImg.clear();
            mTestImg.add(Color.parseColor(color));
            forLCDImage = false;
            setBackgroud(0);
        } else if (caseName.equalsIgnoreCase(CASE_LCD)) {
            mTestImg.clear();
            mTestImg.add(Color.RED);
            mTestImg.add(Color.GREEN);
            mTestImg.add(Color.WHITE);
            mTestImg.add(Color.BLUE);
            mTestImg.add(Color.BLACK);
            setBackgroud(0);
            forLCDImage = false;
        }
    }

    private void showFullScreen(){
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        mLayoutParams = getWindow().getAttributes();
        mLayoutParams.screenBrightness = 1;
        getWindow().setAttributes(mLayoutParams);
    }

    private void showNoImage(){
        //requestWindowFeature(Window.);
        mTextView.setVisibility(View.VISIBLE);
        mBtPass.setVisibility(View.VISIBLE);
        mBtFail.setVisibility(View.VISIBLE);
        mTextView.setText(getString(R.string.lcd_no_image));
        forLCDImage = false;
    }

    @Override
    public void updateView(Bundle bundle) {
    }

    private void setBackgroud(int index) {
        if (index >= mTestImg.size())
            return;
        mRelativeLayout.setBackgroundColor(mTestImg.get(index));
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        final int mAction = event.getAction();
        if ( !forLCDImage ) {
            if ((mAction == MotionEvent.ACTION_UP)) {
                if (mImgSeq < mTestImg.size()) {
                    mImgSeq++;
                    setBackgroud(mImgSeq);
                }
            }
            if (mImgSeq == mTestImg.size()) {
                goFinishPage();
            }
        } else {
             if ((mAction == MotionEvent.ACTION_UP)) {
                 goFinishPage();
             }
        }
        return true;
    }

    private void goFinishPage(){
        mRelativeLayout.setBackgroundDrawable(null);
        mTextView.setVisibility(View.VISIBLE);
        mBtPass.setVisibility(View.VISIBLE);
        mBtFail.setVisibility(View.VISIBLE);
        mTextView.setText(getString(R.string.lcd_finish_test));
    }
}
