/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.customui;

import android.content.Context;
import android.graphics.drawable.AnimationDrawable;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;

public class AnimatedImageView extends ImageView {

   AnimationDrawable frameAnimation;

   public AnimatedImageView(Context context, AttributeSet attrs) {
      super(context, attrs);
   }

   @Override
   protected void onVisibilityChanged(View changedView, int visibility) {

      if (frameAnimation == null)
         frameAnimation = (AnimationDrawable) this.getBackground();

      if (visibility == VISIBLE)
         frameAnimation.start();
      else
         frameAnimation.stop();

      super.onVisibilityChanged(changedView, visibility);

   }

}
