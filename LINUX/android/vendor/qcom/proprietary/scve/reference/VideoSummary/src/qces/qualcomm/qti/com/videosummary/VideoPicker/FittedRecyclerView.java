/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPicker;

import android.content.Context;
import android.content.res.TypedArray;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.AttributeSet;

/**
 * Created by matthewf on 9/28/15.
 */

public class FittedRecyclerView extends RecyclerView {

    private GridLayoutManager gridManager;
    private int colWidth = -1;

    public FittedRecyclerView(Context context) {
        super(context);
        initialize(context, null);
    }

    public FittedRecyclerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context, attrs);
    }

    private void initialize(Context context, AttributeSet attrs) {
        if (attrs != null){
            int[] attrsArray = {android.R.attr.columnWidth};
            TypedArray arr = context.obtainStyledAttributes(attrs,attrsArray);
            colWidth = arr.getDimensionPixelSize(0, -1);
            arr.recycle();
        }
        gridManager = new GridLayoutManager(getContext(),1);
        setLayoutManager(gridManager);
    }

    public FittedRecyclerView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initialize(context, attrs);
    }

    @Override
    protected void onMeasure(int widthSpec, int heightSpec) {
        super.onMeasure(widthSpec, heightSpec);
        if (colWidth > 0) {
            int spanCnt = Math.max(1, getMeasuredWidth() / colWidth);
            gridManager.setSpanCount(spanCnt);
        }
    }
}
