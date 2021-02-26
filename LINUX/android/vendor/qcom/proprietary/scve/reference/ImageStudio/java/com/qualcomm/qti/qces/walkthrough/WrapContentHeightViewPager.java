/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Original code snippet is located at
 * https://github.com/genestream/MonthView/blob/gh-pages/monthview/src/main/java/jp/co/genestream/monthview/MonthViewPager.java
 *
 * Copyright (C) 2015 Genestream
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qualcomm.qti.qces.walkthrough;

import com.qualcomm.qti.qces.imageedit.segmentation.Util;
import com.qualcomm.qti.qces.imageedit.softcut.R;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.view.View;

public class WrapContentHeightViewPager extends ViewPager {

    Context mContext;

    /**
     * Constructor
     * @param context the context
     */
    public WrapContentHeightViewPager(Context context) {
        super(context);
    }

    /**
     * Constructor
     * @param context the context
     * @param attrs the attribute set
     */
    public WrapContentHeightViewPager(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.mContext = context;

    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        // find the first child view
        View view = getChildAt(0);
        if (view != null) {
            // measure the first child view with the specified measure spec
            view.measure(widthMeasureSpec, heightMeasureSpec);
        }

        int width = measureWidth(widthMeasureSpec, view);

        float heightToWidthRatio = Util.getHeightToWidthRatioForDrawable(mContext.getResources()
                .getDrawable(R.drawable.walkthrough_01a));

        int textHeightDp = this.getResources().getInteger(R.integer.walkthrough_textview_height_dp);;
        int textHeightPx = (int)(getResources().getDisplayMetrics().density * textHeightDp);

        int height = (int)((float)width * heightToWidthRatio) + textHeightPx;

        setMeasuredDimension(width, height);

    }

    private int measureWidth(int measureSpec, View view) {
        int result = 0;
        int specMode = MeasureSpec.getMode(measureSpec);
        int specSize = MeasureSpec.getSize(measureSpec);

        if (specMode == MeasureSpec.EXACTLY) {
            result = specSize;
        } else {
            // set the height from the base view if available
            if (view != null) {
                result = view.getMeasuredWidth();
            }
            if (specMode == MeasureSpec.AT_MOST) {
                result = Math.min(result, specSize);
            }
        }
        return result;
    }

}
