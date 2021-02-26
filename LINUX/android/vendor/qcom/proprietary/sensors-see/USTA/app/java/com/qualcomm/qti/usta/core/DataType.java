/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file DataType.java
  @brief
   DataType will contains the enum values for DataType.
============================================================================*/
package com.qualcomm.qti.usta.core;


public enum DataType {
  STRING,
  SIGNED_INTEGER32,
  SIGNED_INTEGER64,
  UNSIGNED_INTEGER32,
  UNSIGNED_INTEGER64,
  FLOAT,
  BOOLEAN,
  ENUM,
  USER_DEFINED;

  private int enumValue;

  public int getEnumValue() {
    return enumValue;
  }

  public void setEnumValue(int enumValue) {
    this.enumValue = enumValue;
  }

}
