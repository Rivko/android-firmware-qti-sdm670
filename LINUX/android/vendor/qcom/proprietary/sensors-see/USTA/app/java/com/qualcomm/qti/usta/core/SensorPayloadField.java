/*===========================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorPayloadField.java
  @brief
   SensorPayloadField class which implements payload types for messages
============================================================================*/
package com.qualcomm.qti.usta.core;


import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

public class SensorPayloadField {

  private String fieldName;
  private DataType dataType;
  private AccessModifier accessModifier;
  private boolean isEnumerated;
  private String[] enumValues;
  private boolean hasDefaultValue;
  private String defaultValue;
  private boolean isUserDefined;
  private String[] valuesForNative;
  private Vector<String> value;
  private NestedMsgPayload[] nestedMsgs;

  private Map widgetIDValueTable;

  public static final int DATATYPE_STRING = 0;
  public static final int DATATYPE_SIGNED_INTEGER32 = 1;
  public static final int DATATYPE_SIGNED_INTEGER64 = 2;
  public static final int DATATYPE_UNSIGNED_INTEGER32 = 3;
  public static final int DATATYPE_UNSIGNED_INTEGER64 = 4;
  public static final int DATATYPE_FLOAT = 5;
  public static final int DATATYPE_BOOLEAN = 6;
  public static final int DATATYPE_ENUM = 7;
  public static final int DATATYPE_USER_DEFINED = 8;

  public static final int ACESSMODIFIER_REQUIRED = 0;
  public static final int ACESSMODIFIER_OPTIONAL = 1;
  public static final int ACESSMODIFIER_REPEATED = 2;


  public String[] getValuesForNative() {
    return valuesForNative;
  }

  public void setValuesForNative(String[] valuesForNative) {
    this.valuesForNative = valuesForNative;
  }

  public SensorPayloadField() {
    value = new Vector<>();
    widgetIDValueTable = new HashMap();
  }
  public Vector<String> getValue() {
    return value;
  }

  public void setValue(Vector<String> value) {
    this.value = value;
  }

  public void clearValue() {
    this.value.removeAllElements();
  }

  public void addValueToWidgetIDValueTable(String value, int widgetID) {
    widgetIDValueTable.put(widgetID, value);
  }

  public void resetValueToWidgetIDValueTable(){
    widgetIDValueTable.clear();
  }

  public void addValue(String value) {
    if (accessModifier == AccessModifier.REPEATED) {
      if(isEnumerated){
        this.value.add(enumValues[Integer.parseInt(value)]);
      }else {
        this.value.add(value);
      }
    }
    else {
      this.value.clear();
      if (isEnumerated) {
        this.value.add(enumValues[Integer.parseInt(value)]);
      } else {
        this.value.add(value);
      }
    }
  }

  public Map getWidgetIDValueTable() {
    return widgetIDValueTable;
  }
  public String getFieldName() {

    return fieldName;
  }

  public DataType getDataType() {
    return dataType;
  }

  public AccessModifier getAccessModifier() {

    return accessModifier;
  }

  public boolean isEnumerated() {

    return isEnumerated;
  }

  public String[] getEnumValues() {

    return enumValues;
  }


  public boolean isHasDefaultValue() {
    return hasDefaultValue;
  }

  public boolean isUserDefined() {
    updateEnumValueInDataType();
    updateEnumValueInAccessModifier();
    return isUserDefined;
  }

  public NestedMsgPayload[] getNestedMsgs() {
    return nestedMsgs;
  }

  public String getDefaultValue() {

    return defaultValue;
  }

  public int getNestedMsgSize() {
    return nestedMsgs.length;
  }

  public void setFieldName(String fieldName) {
    this.fieldName = fieldName;
  }

  public void updateEnumValueInDataType() {

    if (dataType == DataType.STRING)
      this.dataType.setEnumValue(DATATYPE_STRING);

    if (dataType == DataType.SIGNED_INTEGER32)
      this.dataType.setEnumValue(DATATYPE_SIGNED_INTEGER32);

    if (dataType == DataType.SIGNED_INTEGER64)
      this.dataType.setEnumValue(DATATYPE_SIGNED_INTEGER64);

    if (dataType == DataType.UNSIGNED_INTEGER32)
      this.dataType.setEnumValue(DATATYPE_UNSIGNED_INTEGER32);

    if (dataType == DataType.UNSIGNED_INTEGER64)
      this.dataType.setEnumValue(DATATYPE_UNSIGNED_INTEGER64);

    if (dataType == DataType.FLOAT)
      this.dataType.setEnumValue(DATATYPE_FLOAT);

    if (dataType == DataType.BOOLEAN)
      this.dataType.setEnumValue(DATATYPE_BOOLEAN);

    if (dataType == DataType.ENUM)
      this.dataType.setEnumValue(DATATYPE_ENUM);

    if (dataType == DataType.USER_DEFINED)
      this.dataType.setEnumValue(DATATYPE_USER_DEFINED);

  }

  public void updateEnumValueInAccessModifier() {

    if (accessModifier == AccessModifier.REQUIRED)
      this.accessModifier.setAccessModifierValue(ACESSMODIFIER_REQUIRED);

    if (accessModifier == AccessModifier.OPTIONAL)
      this.accessModifier.setAccessModifierValue(ACESSMODIFIER_OPTIONAL);

    if (accessModifier == AccessModifier.REPEATED)
      this.accessModifier.setAccessModifierValue(ACESSMODIFIER_REPEATED);

  }

  public void setDataType(DataType dataType) {
    this.dataType = dataType;
  }

  public void setAccessModifier(AccessModifier accessModifier) {
    this.accessModifier = accessModifier;
  }

  public void setEnumerated(boolean enumerated) {
    isEnumerated = enumerated;
  }

  public void setEnumValues(String[] enumValues) {
    this.enumValues = enumValues;
  }

  public void setHasDefaultValue(boolean hasDefaultValue) {
    this.hasDefaultValue = hasDefaultValue;
  }

  public void setDefaultValue(String defaultValue) {
    this.defaultValue = defaultValue;
  }

  public void setUserDefined(boolean userDefined) {
    isUserDefined = userDefined;
  }

  public void setNestedMsgs(NestedMsgPayload[] nestedMsgs) {
    this.nestedMsgs = nestedMsgs;
  }
}
