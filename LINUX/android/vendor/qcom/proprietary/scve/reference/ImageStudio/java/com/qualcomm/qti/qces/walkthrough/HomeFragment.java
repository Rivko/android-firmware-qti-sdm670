/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.walkthrough;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.CompoundButton.OnCheckedChangeListener;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public class HomeFragment extends Fragment {

    public static final String ARG_ITEM_ID = "home_fragment";

    private static final String prefName = "com.qualcomm.qti.qces.imageedit.segmentation";
    private static final String dontShowAutomatically = "DontShowHelpAutoSegmentation";

    private int currentSlide = 0;

    // UI References
    private ImageSlideAdapter mPagerAdapter;
    private WrapContentHeightViewPager mViewPager;
    ImageView mIndicator;

    FragmentActivity activity;

    SharedPreferences prefs;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        activity = getActivity();

        activity.getWindow().setFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS,
                WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);

        if (savedInstanceState != null) {
            currentSlide = savedInstanceState.getInt("current_slide");
        }

        prefs = activity.getSharedPreferences(prefName, Context.MODE_PRIVATE);

    }

    private boolean getPref() {
        return prefs.getBoolean(dontShowAutomatically, true);// Default is true/checked
    }

    private void savePref(boolean isChecked) {
        SharedPreferences.Editor editor = prefs.edit();
        editor.putBoolean(dontShowAutomatically, isChecked);
        editor.commit();
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstance) {
        Log.d("test", "HomeFragment onViewCreated called");

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.layout_fragment_slide, container, false);

        mViewPager = (WrapContentHeightViewPager)view.findViewById(R.id.wrapContentHeightViewPager);
        mIndicator = (ImageView)view.findViewById(R.id.indicator);

        mPagerAdapter = new ImageSlideAdapter(activity, HomeFragment.this);

        Button mCloseButton = (Button)view.findViewById(R.id.close_button);
        mCloseButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                activity.finish();
            }
        });

        CheckBox mCheckBox = (CheckBox)view.findViewById(R.id.close_checkbox);
        mCheckBox.setChecked(getPref());
        mCheckBox.setOnCheckedChangeListener(new OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                savePref(isChecked);
            }
        });

        mViewPager.setAdapter(mPagerAdapter);
        mViewPager.setCurrentItem(currentSlide);
        addViewPagerListeners();
        setPageIndicator(currentSlide);

        return view;
    }

    private void addViewPagerListeners() {
        mViewPager.setOnPageChangeListener(new PageChangeListener());
    }

    private void setPageIndicator(int pageNum) {
        switch (pageNum) {
        case 0:
            mIndicator.setImageDrawable(getResources().getDrawable(R.drawable.step_01));
            break;
        case 1:
            mIndicator.setImageDrawable(getResources().getDrawable(R.drawable.step_02));
            break;
        case 2:
            mIndicator.setImageDrawable(getResources().getDrawable(R.drawable.step_03));
            break;
        case 3:
            mIndicator.setImageDrawable(getResources().getDrawable(R.drawable.step_04));
            break;

        }
    }

    private class PageChangeListener implements OnPageChangeListener {

        @Override
        public void onPageScrollStateChanged(int state) {
            if (state == ViewPager.SCROLL_STATE_IDLE) {
                setPageIndicator(mViewPager.getCurrentItem());
            }
        }

        @Override
        public void onPageScrolled(int arg0, float arg1, int arg2) {
        }

        @Override
        public void onPageSelected(int arg0) {
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putInt("current_slide", mViewPager.getCurrentItem());
        super.onSaveInstanceState(outState);
    }

    public void setFirstSlide(int firstSlide) {
        currentSlide = firstSlide;
    }

}
