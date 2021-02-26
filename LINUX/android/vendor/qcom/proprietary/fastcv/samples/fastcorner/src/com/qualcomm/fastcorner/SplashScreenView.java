/*==============================================================================
            Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Technologies Confidential and Proprietary
==============================================================================*/

package com.qualcomm.fastcorner;

import com.qualcomm.fastcorner.R;
import com.qualcomm.fastcorner.SplashScreen;

import android.view.MotionEvent;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.DecelerateInterpolator;
import android.view.animation.OvershootInterpolator;
import android.view.animation.TranslateAnimation;
import android.widget.ImageView;

/** SplashScreenView is an ImageView for displaying the splash screen
 * when client launch FastCV sample application
 *
 *  */
public class SplashScreenView extends ImageView {
    private SplashScreen mSplashScreen;
    private boolean mFirstShow;

    /**
     * Constructor of SpalshScreenView
     * @param splashScreen the activity that shows the splash screen
     */
    public SplashScreenView(SplashScreen splashScreen) {
        super(splashScreen);
        mSplashScreen = splashScreen;
        mFirstShow = true;

        setBackgroundDrawable(getResources().getDrawable(R.drawable.splashscreen));
    }

    /**
     * Notify the SplashScreen that the App config changed
     */
    public void onSettingsChanged() {
        postInvalidate();
    }

    /**
     * Make the splash screen smoothly disappear and invoke a call-back when done.
     * @param closedCallback the call back to invoke when the animation ends
     */
    public void animateHiding(final Runnable closedCallback) {
        //stopFlickering();
        int displayWidth = SplashScreen.sDisplay.getWidth();
        TranslateAnimation anim = new TranslateAnimation(0, -displayWidth, 0, 0);
          anim.setInterpolator(new AccelerateInterpolator());
          anim.setDuration(300);
          anim.setAnimationListener(new AnimationListener() {
                public void onAnimationStart(Animation animation) {}
                public void onAnimationRepeat(Animation animation) {}
                public void onAnimationEnd(Animation animation) { closedCallback.run(); }
          });
        startAnimation(anim);
    }

    /**
     * Tell that we don't want the options pop-up at the end
     */
    private boolean mDisableOptionsPopup = false;
    void setDisableOptionsPopup(boolean disable) {
        boolean wasDisabled = mDisableOptionsPopup;
        mDisableOptionsPopup = disable;
        if (wasDisabled && !mDisableOptionsPopup && mSplashScreen != null)
            mSplashScreen.openOptionsMenu();
    }

    /**
     * Called when a touch screen motion event occurs.
     * @param event The motion event
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            if (mSplashScreen != null)
                mSplashScreen.openOptionsMenu();
        }
        return super.onTouchEvent(event);
    }

    /**
     * Called when the view is attached to a window.
     */
    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        if (mSplashScreen == null)
            return;

        // animate the appearance
        if (mFirstShow)
            animateStartup();
        else
            animateReenter();
        mFirstShow = false;
    }

    /**
     * Called when the view is detached from a window.
     */
    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
    }

    /**
     * Called when the splash screen animation done at startup.
     * It will open the option menu.
     */
    protected void onStartupAnimationEnded() {
        if (mSplashScreen == null)
            return;
        // show the options menu after 1s
        if (!mDisableOptionsPopup)
            postDelayed(new Runnable() {public void run() {if (mSplashScreen != null) mSplashScreen.openOptionsMenu();}}, 1000);
    }

    /**
     * Called when the splash screen animation done at reenter.
     * It will open the option menu.
     */
    protected void onReenterAnimationEnded() {
        if (mSplashScreen == null)
            return;
        mSplashScreen.openOptionsMenu();
    }

    /**
     * Start the animation .
     */
    private void animateStartup() {
        int displayHeight = SplashScreen.sDisplay.getHeight();
        Animation anim = new TranslateAnimation(0, 0, displayHeight, 0);
          anim.setInterpolator(new OvershootInterpolator());
          anim.setDuration(600);
          anim.setAnimationListener(new AnimationListener() {
                public void onAnimationStart(Animation animation) {}
                public void onAnimationRepeat(Animation animation) {}
                public void onAnimationEnd(Animation animation) {
                    postDelayed(new Runnable() {public void run() {onStartupAnimationEnded();}}, 1);
                }
          });
        startAnimation(anim);
    }

    /**
     * Reenter the animation .
     */
    private void animateReenter() {
        int displayWidth = SplashScreen.sDisplay.getWidth();
        Animation anim = new TranslateAnimation(displayWidth, 0, 0, 0);
          anim.setInterpolator(new DecelerateInterpolator());
          anim.setDuration(250);
          anim.setAnimationListener(new AnimationListener() {
                public void onAnimationStart(Animation animation) {}
                public void onAnimationRepeat(Animation animation) {}
                public void onAnimationEnd(Animation animation) { onReenterAnimationEnded(); }
          });
        startAnimation(anim);
    }
}
