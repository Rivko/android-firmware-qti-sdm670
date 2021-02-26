/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorReqMessage.java
  @brief
   SensorStdFields class implementation
============================================================================*/
package com.qualcomm.qti.usta.core;


import java.util.Vector;

public class SensorStdFields {

  private String msgID;
  private int clientProcessor = 0;
  private int wakeupDevilvery = 0;
  private String batchPeriod;
  private String flushPeriod;
  private boolean flushOnly;
  private boolean maxBatch;

  private Vector<String> stdFieldsNames;
  private DataType[] stdFieldsDataType;

  public SensorStdFields() {

    stdFieldsNames = new Vector<>();

    stdFieldsNames.add("Client Processor");
    stdFieldsNames.add("Wakeup Devilvery");
    stdFieldsNames.add("Batch Period");
    stdFieldsNames.add("Flush Period");
    stdFieldsNames.add("flushOnly");
    stdFieldsNames.add("maxBatch");

    stdFieldsDataType = new DataType[stdFieldsNames.size()];
    stdFieldsDataType[0] = DataType.ENUM;
    stdFieldsDataType[1] = DataType.ENUM;
    stdFieldsDataType[2] = DataType.UNSIGNED_INTEGER32;
    stdFieldsDataType[3] = DataType.UNSIGNED_INTEGER32;
    stdFieldsDataType[4] = DataType.BOOLEAN;
    stdFieldsDataType[5] = DataType.BOOLEAN;
  }

  public Vector<String> getStdFieldsNames() {

    return stdFieldsNames;
  }

  public DataType[] getStdFieldsDataType() {

    return stdFieldsDataType;
  }

  public String getMsgID() {

    return msgID;
  }

  public int getClientProcessor() {

    return clientProcessor;
  }

  public int getWakeupDevilvery() {

    return wakeupDevilvery;
  }

  public String getBatchPeriod() {

    return batchPeriod;
  }

  public String getFlushPeriod() {

    return flushPeriod;
  }
  public boolean isMaxBatch() {
    return maxBatch;
  }

  public boolean isFlushOnly() {
    return flushOnly;
  }
  public void setMsgID(String msgID) {

    this.msgID = msgID;
  }

  public void setClientProcessor(int clientProcessor) {

    this.clientProcessor = clientProcessor;
  }

  public void setWakeupDevilvery(int wakeupDevilvery) {

    this.wakeupDevilvery = wakeupDevilvery;
  }

  public void setBatchPeriod(String batchPeriod) {

    this.batchPeriod = batchPeriod;
  }

  public void setFlushPeriod(String flushPeriod) {

    this.flushPeriod = flushPeriod;
  }
  public void setMaxBatch(boolean maxBatch) {
    this.maxBatch = maxBatch;
  }

  public void setFlushOnly(boolean flushOnly) {
    this.flushOnly = flushOnly;
  }
}
