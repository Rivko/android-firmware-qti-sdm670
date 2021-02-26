/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.walkthrough;

import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.os.SystemClock;
import android.widget.TextView;

import com.qualcomm.qti.qces.imageedit.softcut.R;

public class CustomTransitionDrawable extends LayerDrawable implements Drawable.Callback {

    protected enum TransitionState {
        TRANSITION_PAUSED, TRANSITION_RUNNING
    }

    protected Drawable[] drawables;

    protected int currentDrawableIndex;
    protected int alpha = 0;
    protected int fromAlpha;
    protected int toAlpha;
    protected long duration;
    protected long startTimeMillis;
    protected long pauseDuration;

    private TextView mTextView;

    protected TransitionState transitionStatus;

    public CustomTransitionDrawable(Drawable[] drawables) {
        super(drawables);
        this.drawables = drawables;
    }

    public void startTransition(int durationMillis, int pauseTimeMillis) {
        fromAlpha = 0;
        toAlpha = 255;
        duration = durationMillis;
        pauseDuration = pauseTimeMillis;
        startTimeMillis = SystemClock.uptimeMillis();
        transitionStatus = TransitionState.TRANSITION_PAUSED;
        currentDrawableIndex = 0;

        invalidateSelf();
    }


    @Override
    public void draw(Canvas canvas) {
        boolean done = true;

        switch (transitionStatus) {
            case TRANSITION_PAUSED:
                if ((SystemClock.uptimeMillis() - startTimeMillis) < pauseDuration)
                    break;
                else {
                    done = false;
                    startTimeMillis = SystemClock.uptimeMillis();
                    transitionStatus = TransitionState.TRANSITION_RUNNING;
                }

            case TRANSITION_RUNNING:
             // Determine position within the transition cycle
                if (startTimeMillis >= 0) {
                    float normalized = (float) (SystemClock.uptimeMillis() - startTimeMillis) / duration;
                    done = normalized >= 1.0f;
                    normalized = Math.min(normalized, 1.0f);
                    alpha = (int) (fromAlpha + (toAlpha - fromAlpha) * normalized);
                }
                break;
        }

        if (transitionStatus == TransitionState.TRANSITION_RUNNING) {
            // Do the fading
            int nextDrawableIndex = 0;

            if (currentDrawableIndex + 1 < drawables.length)
                nextDrawableIndex = currentDrawableIndex + 1;

            Drawable currentDrawable = getDrawable(currentDrawableIndex);
            Drawable nextDrawable = getDrawable(nextDrawableIndex);

            //keep current drawable opaque (not a true cross-fade)
            currentDrawable.draw(canvas);

            if (alpha > 0) {
                nextDrawable.setAlpha(alpha);
                nextDrawable.draw(canvas);
                nextDrawable.setAlpha(0xFF);
            }

            if (done) {
                currentDrawableIndex = nextDrawableIndex;
                startTimeMillis = SystemClock.uptimeMillis();
                transitionStatus = TransitionState.TRANSITION_PAUSED;


            }
        }
        else //PAUSED
        {
            getDrawable(currentDrawableIndex).draw(canvas);

            if (currentDrawableIndex == 0)
                mTextView.setText(R.string.walkthrough_separation_text1);
            else
                mTextView.setText(R.string.walkthrough_separation_text2);

        }

        invalidateSelf();
    }

    public void setTextView(TextView overlayTextView) {
        // TODO Auto-generated method stub
        mTextView = overlayTextView;
    }
}
