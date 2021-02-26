/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.content.Context;
import android.support.v7.widget.StaggeredGridLayoutManager;

public class DynamicGridLayoutManager extends StaggeredGridLayoutManager {

   public DynamicGridLayoutManager(Context context, int spanCount, int orientation) {
      super(spanCount, orientation);  
   }

}
