/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.walkthrough;

import android.app.Activity;
import android.graphics.drawable.Drawable;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.PagerAdapter;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public class ImageSlideAdapter extends PagerAdapter {

    FragmentActivity activity;
    HomeFragment homeFragment;

    ImageView mImageView;
    TextView mSlideTextView;
    TextView mImageTextView;

    View view;

    public ImageSlideAdapter(FragmentActivity activity, HomeFragment homeFragment) {
        this.activity = activity;
        this.homeFragment = homeFragment;
    }

    @Override
    public int getCount() {
        return activity.getResources().getStringArray(R.array.walkthrough).length;
    }

    @Override
    public View instantiateItem(ViewGroup container, final int position) {

        LayoutInflater inflater = (LayoutInflater)activity
                .getSystemService(Activity.LAYOUT_INFLATER_SERVICE);

        view = inflater.inflate(R.layout.layout_viewpager_image, container, false);

        mImageView = (ImageView)view.findViewById(R.id.image_display);
        mSlideTextView = (TextView)view.findViewById(R.id.slideTextView);
        mImageTextView = (TextView)view.findViewById(R.id.imageTextView);

        mSlideTextView.setVisibility(View.GONE);

        setDrawableAndTextByPosition(position);

        container.addView(view);
        return view;

    }

    private void setDrawableAndTextByPosition(int position) {
        Drawable myDrawables[] = new Drawable[2];
        boolean crossfade = false;

        String[] walkthroughTexts = activity.getResources().getStringArray(R.array.walkthrough);
        mImageTextView.setText(walkthroughTexts[position]);

        switch (position) {
        case 0:
            myDrawables[0] = activity.getResources().getDrawable(R.drawable.walkthrough_01a);
            myDrawables[1] = activity.getResources().getDrawable(R.drawable.walkthrough_01b);
            mSlideTextView.setVisibility(View.VISIBLE);
            crossfade = true;
            break;
        case 1:
            myDrawables[0] = activity.getResources().getDrawable(R.drawable.walkthrough_02);
            break;
        case 2:
            myDrawables[0] = activity.getResources().getDrawable(R.drawable.walkthrough_03);
            break;
        case 3:
            myDrawables[0] = activity.getResources().getDrawable(R.drawable.walkthrough_04);
            break;

        default:
            myDrawables[0] = activity.getResources().getDrawable(R.drawable.walkthrough_01a);
            break;
        }

        if (crossfade) {
            CustomTransitionDrawable crossfader = new CustomTransitionDrawable(myDrawables);
            mImageView.setImageDrawable(crossfader);
            crossfader.setTextView(mSlideTextView);
            crossfader.startTransition(500, 1500);
        } else
            mImageView.setImageDrawable(myDrawables[0]);

    }

    @Override
    public void destroyItem(ViewGroup container, int position, Object object) {
        container.removeView((View)object);
    }

    @Override
    public boolean isViewFromObject(View view, Object object) {
        return view == object;
    }

}
