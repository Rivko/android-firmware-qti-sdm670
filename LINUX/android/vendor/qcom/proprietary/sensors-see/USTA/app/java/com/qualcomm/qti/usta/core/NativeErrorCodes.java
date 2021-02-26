/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file NativeErrorCodes.java
  @brief
   NativeErrorCodes will contains the enum values mapping to error codes returned by native.
============================================================================*/

package com.qualcomm.qti.usta.core;

public enum NativeErrorCodes {
  USTA_MEMORY_ERROR,
  USTA_PROTOFILES_NOT_FOUND,
  USTA_REQUIRED_FIELDS_MISSING,
  USTA_NO_ERROR;

  private int enumValue;

  public int getEnumValue() {
    return enumValue;
  }

  public void setEnumValue(int enumValue) {
    this.enumValue = enumValue;
  }
}
