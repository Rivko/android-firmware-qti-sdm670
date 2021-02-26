/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.tutorial;


import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.qualcomm.qti.threedr.R;

public class TutorialSlideScreenPageFragment extends Fragment {
    public static final String ARG_PAGE = "page_number";
    private int mPageNumber;


    public static TutorialSlideScreenPageFragment create(int pageNumber) {
        TutorialSlideScreenPageFragment fragment = new TutorialSlideScreenPageFragment();
        Bundle args = new Bundle();
        args.putInt(ARG_PAGE, pageNumber);
        fragment.setArguments(args);
        return fragment;
    }

    public TutorialSlideScreenPageFragment() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mPageNumber = getArguments().getInt(ARG_PAGE);
    }

    public String getPageTitle(Context context, int position) {
        String title = null;
        switch (position) {
            case 0:
                title = context.getResources().getString(R.string.tutorial_title_step1);
                break;
            case 1:
                title = context.getResources().getString(R.string.tutorial_title_step2);
                break;
        }
        return title;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ViewGroup rootView = null;
        switch (mPageNumber) {
            case 0:
                rootView = (ViewGroup) inflater
                        .inflate(R.layout.fragment_tutorial_step_1, container, false);
                break;
            case 1:
                rootView = (ViewGroup) inflater
                        .inflate(R.layout.fragment_tutorial_step_2, container, false);
                break;
        }
        return rootView;
    }

    /**
     * Returns the page number represented by this fragment object.
     */
    public int getPageNumber() {
        return mPageNumber;
    }
}
