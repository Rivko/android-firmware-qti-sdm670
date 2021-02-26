/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.walkthrough;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public class SlideshowFragmentActivity extends FragmentActivity {

    private Fragment contentFragment;
    private int firstSlide = 0;

    HomeFragment homeFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Bundle bundleExtras = this.getIntent().getExtras();
        if (bundleExtras != null) {
            firstSlide = bundleExtras.getInt("first_slide");
        }

        setContentView(R.layout.layout_fragment_slideshow);

        FragmentManager fragmentManager = getSupportFragmentManager();

        if (savedInstanceState != null) {
            if (fragmentManager.findFragmentByTag(HomeFragment.ARG_ITEM_ID) != null) {
                homeFragment = (HomeFragment)fragmentManager
                        .findFragmentByTag(HomeFragment.ARG_ITEM_ID);
                contentFragment = homeFragment;
            }
        } else {
            homeFragment = new HomeFragment();
            homeFragment.setFirstSlide(firstSlide);
            switchContent(homeFragment, HomeFragment.ARG_ITEM_ID);
        }

    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {

        outState.putString("content", HomeFragment.ARG_ITEM_ID);
        super.onSaveInstanceState(outState);

    }

    public void switchContent(Fragment fragment, String tag) {

        FragmentManager fragmentManager = getSupportFragmentManager();
        while (fragmentManager.popBackStackImmediate())
            ;

        if (fragment != null) {
            FragmentTransaction transaction = fragmentManager.beginTransaction();
            transaction.replace(R.id.content_frame, fragment, tag);

            if (!(fragment instanceof HomeFragment)) {
                transaction.addToBackStack(tag);
            }

            transaction.commit();
            contentFragment = fragment;
        }
    }

    @Override
    public void onBackPressed() {
        FragmentManager fm = getSupportFragmentManager();
        if (fm.getBackStackEntryCount() > 0) {
            super.onBackPressed();
        } else if (contentFragment instanceof HomeFragment || fm.getBackStackEntryCount() == 0) {
            finish();
        }
    }

}
