/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorReqMessage.java
  @brief
   SensorReqMessage class implementation
============================================================================*/
package com.qualcomm.qti.usta.core;


import java.util.Vector;

public class SensorReqMessage {

  private SensorStdFields sensorStdFields;
  private Vector<SensorPayloadField> sensorPayloadField;
  private boolean isSensorStreaming = false;
  private int maxMainPayloadFields ;
  private int totalFieldsCount;

  public int getTotalFieldsCount() {
    return totalFieldsCount;
  }

  public int getMaxMainPayloadFields() {
    return maxMainPayloadFields;
  }

  public void setTotalFieldsCount(int totalFieldsCount) {
    if(this.totalFieldsCount < totalFieldsCount)
      this.totalFieldsCount = totalFieldsCount;
  }

  public void setMaxMainPayloadFields(int maxMainPayloadFields) {
    this.maxMainPayloadFields = maxMainPayloadFields;
  }

  public SensorStdFields getSensorStdFields() {
    return sensorStdFields;
  }

  public Vector<SensorPayloadField> getSensorPayloadField() {
    return sensorPayloadField;
  }

  public void setSensorStdFields(SensorStdFields sensorStdFields) {
    this.sensorStdFields = sensorStdFields;
  }

  public void setSensorPayloadField(Vector<SensorPayloadField> sensorPayloadField) {
    this.sensorPayloadField = sensorPayloadField;
  }

  public boolean isSensorStreaming() {

    return isSensorStreaming;
  }

  public void setSensorStreaming(boolean sensorStreaming) {

    isSensorStreaming = sensorStreaming;
  }
}
