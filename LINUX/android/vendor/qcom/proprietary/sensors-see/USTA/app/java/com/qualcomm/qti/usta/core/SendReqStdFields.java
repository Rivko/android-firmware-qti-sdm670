/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SendReqStdFields.java
  @brief
   SendReqStdFields is mapped to native data structure.
============================================================================*/
package com.qualcomm.qti.usta.core;

public class SendReqStdFields {
  String batchPeriod;
  String flushPeriod;
  String clientType;
  String wakeupType;
  String flushOnly;
  String maxBatch;

  public void setMaxBatch(String maxBatch) {
    this.maxBatch = maxBatch;
  }

  public void setFlushOnly(String flushOnly) {
    this.flushOnly = flushOnly;
  }

  public void setBatchPeriod(String batchPeriod) {
    this.batchPeriod = batchPeriod;
  }

  public void setFlushPeriod(String flushPeriod) {
    this.flushPeriod = flushPeriod;
  }

  public void setClientType(String clientType) {
    this.clientType = clientType;
  }

  public void setWakeupType(String wakeupType) {
    this.wakeupType = wakeupType;
  }

}
