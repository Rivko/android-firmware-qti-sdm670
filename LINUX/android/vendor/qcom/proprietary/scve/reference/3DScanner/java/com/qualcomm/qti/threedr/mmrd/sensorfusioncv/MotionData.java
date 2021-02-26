/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.mmrd.sensorfusioncv;

public class MotionData {
  public float[] v;
  public long    t;
  
  public MotionData() {
    v = new float[3]; //    
  }
  public MotionData(float[] _v, long _t) {
    v = _v.clone();
    t = _t;
  }
}
