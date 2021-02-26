/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.tutorial;

import android.content.res.Resources;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.LinearLayout;

import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.modelbrowser.BrowserUtils;

import java.util.ArrayList;
import java.util.List;

import static com.qualcomm.qti.threedr.modelbrowser.SettingsActivity.setPrefDontShowTutuorialCheckbox;

public class TutorialScreenActivity extends AppCompatActivity {
    private static final int NUM_PAGES = 2;
    private static final String TAG = TutorialScreenActivity.class.getSimpleName();

    private ViewPager mPager;
    private PagerAdapter mPagerAdapter;
    private List<ImageView> mDots;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tutorial_screen);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        // Instantiate a ViewPager and a PagerAdapter.

        mPager = (ViewPager) findViewById(R.id.pager);
        mPagerAdapter = new TutorialSlideScreenPagerAdapter(getSupportFragmentManager());
        mPager.setAdapter(mPagerAdapter);
        addPageDots();
        mPager.setOnPageChangeListener(new ViewPager.SimpleOnPageChangeListener() {
            @Override
            public void onPageSelected(int position) {
                // When changing pages, reset the action bar actions since they are dependent
                // on which page is currently active. An alternative approach is to have each
                // fragment expose actions itself (rather than the activity exposing actions),
                // but for simplicity, the activity provides the actions in this sample.
                invalidateOptionsMenu();
                highlightPageDot(position);
            }
        });
    }

    private void addPageDots() {
        mDots = new ArrayList<>();
        LinearLayout bottomDotLayout = (LinearLayout) findViewById(R.id.bottom_dot_layout);

        for (int i = 0; i < mPagerAdapter.getCount(); i++) {
            ImageView dotImage = new ImageView(this);
            dotImage.setImageResource(R.drawable.ic_black_circle_24dp);
            LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            bottomDotLayout.addView(dotImage, layoutParams);
            mDots.add(dotImage);
        }
        highlightPageDot(0);
    }

    private void highlightPageDot(int position) {
        Resources res = getResources();
        for (int i = 0; i < mPagerAdapter.getCount(); i++) {
            int drawableId = (i == position) ? (R.drawable.ic_white_circle_24dp) : (R.drawable.ic_black_circle_24dp);
            mDots.get(i).setImageResource(drawableId);
        }
    }

    public void skipTutorialClicked(View view) {
        Log.i(TAG, "skip clicked");
        BrowserUtils.Launch3DR(this);
    }

    public void dontShowAgainCheckboxClicked(View view) {
        Log.i(TAG, "checkbox clicked");
        setPrefDontShowTutuorialCheckbox(this, ((CheckBox) view).isChecked());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        getMenuInflater().inflate(R.menu.menu_tutorial_screen, menu);

        //menu.findItem(R.id.action_previous).setEnabled(mPager.getCurrentItem() > 0);

        // Add either a "next" or "scan" button to the action bar, depending on which page
        // is currently selected.
        MenuItem item = menu.add(Menu.NONE, R.id.action_next, Menu.NONE,
                (mPager.getCurrentItem() == mPagerAdapter.getCount() - 1)
                        ? R.string.action_scan
                        : R.string.action_next);
        item.setShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM | MenuItem.SHOW_AS_ACTION_WITH_TEXT);
        setToolbarTitle((String) mPagerAdapter.getPageTitle(mPager.getCurrentItem()));
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                if (mPager.getCurrentItem() == 0) {
                    onBackPressed();
                } else {
                    mPager.setCurrentItem(mPager.getCurrentItem() - 1);
                }
                return true;

            case R.id.action_next:
            case R.id.action_next_icon:
                if (mPager.getCurrentItem() == mPagerAdapter.getCount() - 1) {
                    //last page of tutorial, so next will be same as skip i.e to scan
                    skipTutorialClicked(null);
                } else {
                    mPager.setCurrentItem(mPager.getCurrentItem() + 1);
                }
                return true;
        }

        return super.onOptionsItemSelected(item);
    }

    void setToolbarTitle(String title) {
        getSupportActionBar().setTitle(title);
    }

    private class TutorialSlideScreenPagerAdapter extends FragmentStatePagerAdapter {
        public TutorialSlideScreenPagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int position) {
            return TutorialSlideScreenPageFragment.create(position);
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return ((TutorialSlideScreenPageFragment) getItem(position)).getPageTitle(getApplicationContext(), position);
        }

        @Override
        public int getCount() {
            return NUM_PAGES;
        }
    }


}
