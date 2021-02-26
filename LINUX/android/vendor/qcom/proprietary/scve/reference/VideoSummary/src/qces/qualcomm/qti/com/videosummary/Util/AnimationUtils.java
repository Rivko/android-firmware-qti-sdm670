/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.Util;

import android.animation.ObjectAnimator;
import android.graphics.Rect;
import android.support.v7.widget.RecyclerView;
import android.util.Log;

import java.util.List;

/**
 * Created by mithun on 9/3/15.
 */
public class AnimationUtils {

    public static void animateViewHolder(RecyclerView.ViewHolder viewHolder, boolean scrollUp) {
        ObjectAnimator animatorTransY = ObjectAnimator.ofFloat(viewHolder.itemView, "translationY", scrollUp ? -200 : 200, 0);
        animatorTransY.setDuration(1000);
        animatorTransY.start();
    }

    /**
     * This method calculates the biggest rectangle which is the bounding box for all the input rectangles provided in the list as argument
     *
     * @param rectList
     * @return biggest rectangle which is the bounding box for all the input rectangles provided in the list as argument
     */
    public static Rect outermostBoundingBox(List<Rect> rectList) {

        if (rectList == null || rectList.size() == 0 || rectList.get(0) == null) {
            throw new NullPointerException("rectList is empty or invalid");
        }

        Rect resultRect = new Rect(rectList.get(0));

        for (Rect rect : rectList) {
            Log.i("Rect", "L:" + rect.left + " T:" + rect.top + " R:" + rect.right + " B:" + rect.bottom);
            if (rect.left < resultRect.left) {
                resultRect.left = rect.left;
            }

            if (rect.top < resultRect.top) {
                resultRect.top = rect.top;
            }
            if (rect.right > resultRect.right) {
                resultRect.right = rect.right;
            }
            if (rect.bottom > resultRect.bottom) {
                resultRect.bottom = rect.bottom;
            }
        }
        Log.i("Res: ", "L:" + resultRect.left + " T:" + resultRect.top + " R:" + resultRect.right + " B:" + resultRect.bottom);
        return resultRect;
    }
}
