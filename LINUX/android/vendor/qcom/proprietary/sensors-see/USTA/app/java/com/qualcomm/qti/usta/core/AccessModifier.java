/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file AccessModifier.java
  @brief
   AccessModifier will contains the enum values mapping to access modifiers of proto files.
============================================================================*/
package com.qualcomm.qti.usta.core;


public enum AccessModifier {
  REQUIRED,
  OPTIONAL,
  REPEATED;

  private int accessModifierValue;


  public int getAccessModifierValue() {
    return accessModifierValue;
  }

  public void setAccessModifierValue(int accessModifierValue) {
    this.accessModifierValue = accessModifierValue;
  }
}
