/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file ReqMsgPayload.java
  @brief
   ReqMsgPayload is mapped to native data structure.
============================================================================*/

package com.qualcomm.qti.usta.core;

public class ReqMsgPayload {
  private String msgName;
  private String msgID;
  private SensorPayloadField[] fields;

  public String getMsgName() {
    return msgName;
  }

  public String getMsgID() {
    return msgID;
  }

  public SensorPayloadField[] getFields() {
    return fields;
  }

  public void setFields(SensorPayloadField[] fields) {
    this.fields = fields;
  }

  public void setMsgID(String msgID) {
    this.msgID = msgID;
  }

  public void setMsgName(String msgName) {
    this.msgName = msgName;
  }

  public int getFieldCount(){
    return fields.length;
  }
  public SensorPayloadField getFieldAt(int pos){
    return fields[pos];
  }
}
