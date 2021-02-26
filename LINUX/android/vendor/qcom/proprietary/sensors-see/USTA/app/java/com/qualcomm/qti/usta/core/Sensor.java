/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file Sensor.java
  @brief
   Sensor class implementation
============================================================================*/
package com.qualcomm.qti.usta.core;


import java.util.Vector;


public class Sensor {

  private String name;
  private String SUIDLow;
  private String SUIDHigh;
  private Vector<String> reqMsgsNames;
  private Vector<String> reqMsgIDs;
  private Vector<String> reqMsgsNamesWithMsgIDs;
  private Vector<SensorReqMessage> reqMsgFields;

  public Sensor() {

    reqMsgFields = new Vector<>();
    reqMsgsNamesWithMsgIDs = new Vector<>();
  }

  public String getSensorSUIDLow() {

    return SUIDLow;
  }

  public String getSensorSUIDHigh() {

    return SUIDHigh;
  }

  public void setSensorSUIDHigh(String sensorSUIDHigh) {

    SUIDHigh = sensorSUIDHigh;
  }

  public void setSensorSUIDLow(String sensorSUIDLow) {

    SUIDLow = sensorSUIDLow;
  }

  public String getSensorName() {

    return name;
  }

  public void setSensorName(String sensorName) {

    name = sensorName;
  }

  public Vector<String> getSensorReqMsgsNames() {

    return reqMsgsNames;
  }

  public void clearSensorReqMsgNamesWithMsgID() {
    reqMsgsNamesWithMsgIDs.clear();
  }
  public void setSensorReqMsgsNamesWithMsgID() {

    for (int reqMsgIndex = 0; reqMsgIndex < reqMsgsNames.size(); reqMsgIndex++) {

      reqMsgsNamesWithMsgIDs.add(reqMsgsNames.get(reqMsgIndex) + " - "+reqMsgIDs.get(reqMsgIndex));
    }
  }

  public Vector<String> getSensorReqMsgsNamesWithMsgID() {

    return reqMsgsNamesWithMsgIDs;
  }

  public void setSensorReqMsgsNames(Vector<String> sensorReqMsgs) {

    reqMsgsNames = sensorReqMsgs;

    for (int reqMsgIndex = 0; reqMsgIndex < reqMsgsNames.size(); reqMsgIndex++) {

      SensorStdFields stdFields = new SensorStdFields();
      SensorReqMessage reqMsg = new SensorReqMessage();

      reqMsg.setSensorStdFields(stdFields);

      reqMsgFields.add(reqMsg);
    }
  }

  public SensorStdFields getSensorStdFields(int reqMsgHandle) {

    return reqMsgFields.get(reqMsgHandle).getSensorStdFields();
  }

  public Vector<SensorPayloadField> getSensorPayloadField(int reqMsgHandle) {

    return reqMsgFields.get(reqMsgHandle).getSensorPayloadField();
  }

  public void setSensorStdFields(int reqMsgHandle, SensorStdFields sensorStdFields) {

    reqMsgFields.get(reqMsgHandle).setSensorStdFields(sensorStdFields);
  }

  public void setSensorPayloadField(int reqMsgHandle, Vector<SensorPayloadField> sensorPayloadField) {

    reqMsgFields.get(reqMsgHandle).setSensorPayloadField(sensorPayloadField);
  }

  public Vector<String> getSensorReqMsgIDs() {

    return reqMsgIDs;
  }

  public void setSensorReqMsgIDs(Vector<String> sensorReqMsgIDs) {

    reqMsgIDs = sensorReqMsgIDs;
  }

  public boolean isSensorStreaming(int reqMsgHandle) {

    return reqMsgFields.get(reqMsgHandle).isSensorStreaming();
  }

  public void setSensorStreaming(int reqMsgHandle, boolean enabled) {


    if (!enabled) {

      for (SensorReqMessage sensorReqMsg : reqMsgFields) {

        sensorReqMsg.setSensorStreaming(enabled);
      }
    } else {

      reqMsgFields.get(reqMsgHandle).setSensorStreaming(enabled);
    }
  }

  public int getMaxMainPayloadFields(int reqMsgHandle) {
    return reqMsgFields.get(reqMsgHandle).getMaxMainPayloadFields();
  }
  public void setMaxMainPayloadFields(int reqMsgHandle, int count) {
    reqMsgFields.get(reqMsgHandle).setMaxMainPayloadFields(count);
  }

  public int getTotalPayloadFieldCount(int reqMsgHandle) {
    return reqMsgFields.get(reqMsgHandle).getTotalFieldsCount();
  }
  public void setTotalPayloadFieldCount(int reqMsgHandle, int count) {
    reqMsgFields.get(reqMsgHandle).setTotalFieldsCount(count);
  }

}