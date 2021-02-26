/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file NestedMsgPayload.java
  @brief
   NestedMsgPayload is mapped to native data structure.
============================================================================*/
package com.qualcomm.qti.usta.core;

public class NestedMsgPayload {
  private String nestedMsgName;
  private SensorPayloadField[] fields;

  public void setNestedMsgName(String nestedMsgName) {
    this.nestedMsgName = nestedMsgName;
  }
  public void setFields(SensorPayloadField[] fields) {
    this.fields = fields;
  }

  public String getNestedMsgName() {
    return nestedMsgName;
  }

  public SensorPayloadField[] getFields() {
    return fields;
  }

  public int getFieldCount(){
    return fields.length;
  }

  public SensorPayloadField getFiledAt(int pos){
    return fields[pos];
  }




}
