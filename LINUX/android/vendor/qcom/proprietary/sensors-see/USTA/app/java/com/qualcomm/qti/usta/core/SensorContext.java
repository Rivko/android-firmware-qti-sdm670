/*============================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file SensorContext.java
  @brief
  SensorContext class implementation which is common for all the sensors.
============================================================================*/
package com.qualcomm.qti.usta.core;


import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Environment;

import com.qualcomm.qti.usta.ui.SensorPayloadFragment;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

public class SensorContext {

  private Vector<Sensor> sensors;
  private Vector<SensorListInfo> sensorInfo;
  private Vector<String> sensorNames;
  private Vector<String> clientProcessors;
  private Vector<String> wakeupDeliveryTypes;
  private SensorListInfo[] sensorListFromNative;
  private ModeType USTAModeType ;
  private static SensorContext[] sensorContextArray = new SensorContext[ModeType.USTA_MODE_TYPE_COUNT.ordinal()];
  private SensorContext sensorContext;

  private void getSensorListFromNative(ModeType inModeType) {
    inModeType.setEnumValue(inModeType.ordinal());
    USTAModeType = inModeType;
    sensorListFromNative = SensorContextJNI.getSensorListNativeWrapper(USTAModeType);
  }
  public ModeType getUSTAModeType() {
    return USTAModeType;
  }
  private void removeSensor(int sensorHandle) {

    sensors.remove(sensorHandle);
    sensorInfo.remove(sensorHandle);
    sensorNames.remove(sensorHandle);
  }

  private Vector<SensorListInfo> getSensorList(ModeType inModeType) {

    if (sensorListFromNative == null) {
      getSensorListFromNative(inModeType);
    }
    if (sensorListFromNative.length == 0 ) {
      return null;
    }

    return (Vector<SensorListInfo>) new Vector(Arrays.asList(sensorListFromNative));
  }

  private void getRequestMsgs() {

    boolean[] rogueSensors = new boolean[sensors.size()];
    int rogueSensorsCount = 0;

    for (int sensorHandle = 0; sensorHandle < sensors.size(); sensorHandle++) {

      Vector<String> sensorReqMsgsName = new Vector<>();
      Vector<String> sensorReqMsgIDs = new Vector<>();
      Vector<String[]> extractedReqMsgsIDs;

      ReqMsgPayload[] reqMsgs = SensorContextJNI.getRequestMessagesNativeWrapper(USTAModeType, sensorHandle);

      if ((null == reqMsgs) || (reqMsgs.length == 0)) {

        rogueSensors[sensorHandle] = true;
        rogueSensorsCount++;
        USTALog.e("Error loading " + sensors.get(sensorHandle).getSensorName());

        continue;

      } else {

        rogueSensors[sensorHandle] = false;
      }


      for (int msgIndex = 0; msgIndex < reqMsgs.length; msgIndex++) {
        sensorReqMsgsName.add(reqMsgs[msgIndex].getMsgName());
        sensorReqMsgIDs.add(reqMsgs[msgIndex].getMsgID());
      }

      sensors.get(sensorHandle).setSensorReqMsgsNames(sensorReqMsgsName);
      sensors.get(sensorHandle).setSensorReqMsgIDs(sensorReqMsgIDs);

      int reqMsgIndex = 0;

      for (int msgIndex = 0; msgIndex < reqMsgs.length; msgIndex++) {

        Vector<SensorPayloadField> sensorPayloadField;
        sensorPayloadField = new Vector<>();
        for(int fieldindex = 0 ; fieldindex < reqMsgs[msgIndex].getFieldCount() ; fieldindex++)
        {
          sensorPayloadField.add(reqMsgs[msgIndex].getFieldAt(fieldindex));
        }
        sensors.get(sensorHandle).setSensorPayloadField(reqMsgIndex++, sensorPayloadField);
      }

    }
    USTALog.d("rogueSensorsCount " + rogueSensorsCount);

    int[] rogueSensorsHandlesArray = new int[rogueSensorsCount];
    int rogueSensorHandleIndex = 0;
    for (int index = 0; index < rogueSensors.length; index++) {
      if (rogueSensors[index]) {
        rogueSensorsHandlesArray[rogueSensorHandleIndex] = index;
        rogueSensorHandleIndex++;
      }
    }
        /*First deleting the sensors from native side and then later in the app side as coded below*/
    if (rogueSensorsCount > 0) {
      SensorContextJNI.removeSensorsNativeWrapper(USTAModeType,rogueSensorsHandlesArray);
    }

    int numRogueSensors = 0;

    for (int index = 0; index < rogueSensors.length; index++) {

      if (rogueSensors[index]) {

        removeSensor(index - numRogueSensors);
        numRogueSensors++;

      }
    }
  }

  public Vector<String> getClientProcessors() {

    if (clientProcessors == null) {
      String[] clientProcessors = SensorContextJNI.getClientProcessorsNativeWrapper(USTAModeType);
      this.clientProcessors = new Vector<>();

      for (String processor : clientProcessors) {
        processor = processor.toLowerCase();
        this.clientProcessors.add(processor);
      }
    }

    return clientProcessors;
  }

  public Vector<String> getWakeupDelivery() {

    if (wakeupDeliveryTypes == null) {
      String[] wakeupDelivery = SensorContextJNI.getWakeupDeliveryNativeWrapper(USTAModeType);
      this.wakeupDeliveryTypes = new Vector<>();

      for (String deliveryType : wakeupDelivery) {
        deliveryType = deliveryType.toLowerCase();
        this.wakeupDeliveryTypes.add(deliveryType);
      }
    }

    return wakeupDeliveryTypes;
  }

  private boolean allocSensorInstances() {

    sensorNames = new Vector<>();

    if (null == sensorInfo) {
      USTALog.e("sensorInfo is null! Invalid. Application is getting closed without Crash ");
      return false;
    }

    for (int index = 0; index < sensorInfo.size(); index++) {
      Sensor sensorInstance = new Sensor();
      String sensorName = sensorInfo.get(index).getDataType() + "-" + sensorInfo.get(index).getVendor();
      sensorInstance.setSensorName(sensorName);
      sensorNames.add(sensorName);

      sensorInstance.setSensorSUIDLow(sensorInfo.get(index).getSuidLow());
      sensorInstance.setSensorSUIDHigh(sensorInfo.get(index).getSuidHigh());

      sensors.add(sensorInstance);
    }

    return true;
  }

  private SensorContext(ModeType inModeType) {

    sensorInfo = getSensorList(inModeType);
    sensors = new Vector<>();

    boolean ret = allocSensorInstances();

    if (ret == true) {
      getRequestMsgs();
      sensorContext = this;
      if(USTALog.isLoggingDisabled == false)
        dumpSensorInfoToFile(Environment.getExternalStorageDirectory() + "/SensorsInfo.log");
    } else {
      sensorContext = null;
    }

  }

  public static SensorContext getSensorContext(ModeType inModeType) {

    switch(inModeType){
      case USTA_MODE_TYPE_COMMAND_LINE:
        if (null == sensorContextArray[inModeType.USTA_MODE_TYPE_COMMAND_LINE.ordinal()]) {
          SensorContext sensorCxt = new SensorContext(inModeType.USTA_MODE_TYPE_COMMAND_LINE);
          sensorContextArray[inModeType.USTA_MODE_TYPE_COMMAND_LINE.ordinal()] = sensorCxt;
        }
        return sensorContextArray[inModeType.USTA_MODE_TYPE_COMMAND_LINE.ordinal()];

      case USTA_MODE_TYPE_UI:
        if (null == sensorContextArray[inModeType.USTA_MODE_TYPE_UI.ordinal()]) {
          SensorContext sensorCxt = new SensorContext(inModeType.USTA_MODE_TYPE_UI);
          sensorContextArray[inModeType.USTA_MODE_TYPE_UI.ordinal()] = sensorCxt;
        }
        return sensorContextArray[inModeType.USTA_MODE_TYPE_UI.ordinal()];

      default:
        USTALog.e("Invalid / Non supported USTA Mode type");
        return null;
    }
  }

  private void dumpSensorInfoToFile(String inFileName) {

    try {

      FileOutputStream fileInstance = new FileOutputStream(inFileName);

      final String NEWLINE = "\n";
      final String COLON = ":";
      final String TAB = "\t";
      final String COMMA = ",";
      final String SEMICOLON = ";";
      final String RIGHTBRACE = "}";
      final String LEFTBRACE = "{";
      final String NAME = "Name";
      final String DATATYPE = "DataType";
      final String ENUMVALUES = "EnumVaules";

      for (int index = 0; index < sensors.size(); index++) {

        Sensor sensorInstance = sensors.get(index);
        fileInstance.write(NEWLINE.getBytes());
        fileInstance.write(NEWLINE.getBytes());
        fileInstance.write("Sensor Name:".getBytes());
        fileInstance.write(TAB.getBytes());
        fileInstance.write(sensorInstance.getSensorName().getBytes());
        fileInstance.write(NEWLINE.getBytes());

        fileInstance.write("SUID Low:".getBytes());
        fileInstance.write(TAB.getBytes());
        fileInstance.write(sensorInstance.getSensorSUIDLow().getBytes());
        fileInstance.write(NEWLINE.getBytes());

        fileInstance.write("SUID High:".getBytes());
        fileInstance.write(TAB.getBytes());
        fileInstance.write(sensorInstance.getSensorSUIDHigh().getBytes());

        for (int reqMsgIndex = 0; reqMsgIndex < sensorInstance.getSensorReqMsgsNames().size(); reqMsgIndex++) {

          String reqMsgName = sensorInstance.getSensorReqMsgsNames().get(reqMsgIndex);
          String reqMsgID = sensorInstance.getSensorReqMsgIDs().get(reqMsgIndex);
          String payloadMsgFieldStr;

          Vector<String> clientProcessors = sensorContext.getClientProcessors();
          String clientProcessorString = "";

          for (int clientProcIndex = 0; clientProcIndex < clientProcessors.size(); clientProcIndex++) {
            clientProcessorString = clientProcessorString + clientProcessors.get(clientProcIndex);
            if (clientProcIndex < clientProcessors.size() - 1) {
              clientProcessorString = clientProcessorString + COMMA;
            }
          }

          Vector<String> wakeDelivery = sensorContext.getWakeupDelivery();
          String wakeDeliveryString = "";

          for (int wakeDeliveryIndex = 0; wakeDeliveryIndex < wakeDelivery.size(); wakeDeliveryIndex++) {
            wakeDeliveryString = wakeDeliveryString + wakeDelivery.get(wakeDeliveryIndex);
            if (wakeDeliveryIndex < wakeDelivery.size() - 1) {
              wakeDeliveryString = wakeDeliveryString + COMMA;
            }
          }

          fileInstance.write(NEWLINE.getBytes());
          fileInstance.write("Request Messages:".getBytes());
          fileInstance.write(NEWLINE.getBytes());

          fileInstance.write(reqMsgName.getBytes());
          fileInstance.write(COLON.getBytes());
          fileInstance.write(reqMsgID.getBytes());
          fileInstance.write(NEWLINE.getBytes());

          fileInstance.write("{Name:clientprocessor;DataType:enum;EnumValues:".getBytes());
          fileInstance.write(clientProcessorString.getBytes());
          fileInstance.write(SEMICOLON.getBytes());
          fileInstance.write(RIGHTBRACE.getBytes());
          fileInstance.write(NEWLINE.getBytes());

          fileInstance.write("{Name:wakeupDelivery;DataType:enum;EnumValues:".getBytes());
          fileInstance.write(wakeDeliveryString.getBytes());
          fileInstance.write(SEMICOLON.getBytes());
          fileInstance.write(RIGHTBRACE.getBytes());
          fileInstance.write(NEWLINE.getBytes());

          fileInstance.write("{Name:batchperiod;DataType:uint32;}".getBytes());
          fileInstance.write(NEWLINE.getBytes());
          fileInstance.write("{Name:flushperiod;DataType:uint32;}".getBytes());
          fileInstance.write(NEWLINE.getBytes());

          for (int payloadIndex = 0; payloadIndex < sensorInstance.getSensorPayloadField(reqMsgIndex).size(); payloadIndex++) {

            SensorPayloadField payloadField = sensorInstance.getSensorPayloadField(reqMsgIndex).get(payloadIndex);

            payloadMsgFieldStr = "";
            payloadMsgFieldStr = payloadMsgFieldStr + LEFTBRACE + NAME + COLON + payloadField.getFieldName() + SEMICOLON +
                    DATATYPE + COLON + payloadField.getDataType().toString().toLowerCase() + SEMICOLON;

            if (payloadField.getDataType() == DataType.ENUM) {

              payloadMsgFieldStr = payloadMsgFieldStr + ENUMVALUES + COLON;

              for (String enumVal : payloadField.getEnumValues()) {
                payloadMsgFieldStr = payloadMsgFieldStr + enumVal + COMMA;
              }

            }

            payloadMsgFieldStr = payloadMsgFieldStr + RIGHTBRACE;

            fileInstance.write(payloadMsgFieldStr.getBytes());
          }

          fileInstance.write(NEWLINE.getBytes());
        }
        fileInstance.write("Attributes:".getBytes());
        fileInstance.write(SensorContextJNI.getSensorAttributesNativeWrapper(USTAModeType,index).getBytes());
        fileInstance.write(NEWLINE.getBytes());
      }

      fileInstance.close();

    } catch (IOException ex) {

      ex.printStackTrace();
    }
  }

  public Vector<Sensor> getSensors() {

    return sensors;
  }

  public Vector<String> getSensorNames() {

    return sensorNames;
  }

  private void updateFiledValuesFromMappingtable(NestedMsgPayload[] nestedMsg)
  {

    for(int nestedMsgIndex = 0 ; nestedMsgIndex < nestedMsg.length ; nestedMsgIndex++){
      SensorPayloadField[] payloadFields =  nestedMsg[nestedMsgIndex].getFields();
      for(int fieldIndex =0 ; fieldIndex < payloadFields.length ; fieldIndex++){
        if(payloadFields[fieldIndex].isUserDefined() == true) {
          updateFiledValuesFromMappingtable(payloadFields[fieldIndex].getNestedMsgs());
        }
        else{
          Map widgetIDValueTable = payloadFields[fieldIndex].getWidgetIDValueTable();
          Iterator it = widgetIDValueTable.entrySet().iterator();
          while(it.hasNext()){
            Map.Entry currentMapEntry = (Map.Entry)it.next();
            String value = currentMapEntry.getValue().toString();
            payloadFields[fieldIndex].addValue(value);
          }
          payloadFields[fieldIndex].setValuesForNative(payloadFields[fieldIndex].getValue().toArray(new String[payloadFields[fieldIndex].getValue().size()]));
        }
      }
    }

  }
  public int sendRequest(int sensorHandle, int sensorReqHandle, Context context, String logFileName) {

    final String DELIMITER = ":";

    Sensor sensor = getSensors().get(sensorHandle);
    SensorStdFields stdFields = sensor.getSensorStdFields(sensorReqHandle);
    Vector<SensorPayloadField> payloadFields = sensor.getSensorPayloadField(sensorReqHandle);

    for(int fieldIndex = 0 ; fieldIndex < payloadFields.size() ; fieldIndex++){
      if(payloadFields.get(fieldIndex).isUserDefined() == true) {
        updateFiledValuesFromMappingtable(payloadFields.get(fieldIndex).getNestedMsgs());
      }
      else{
        Map widgetIDValueTable = payloadFields.get(fieldIndex).getWidgetIDValueTable();
        Iterator it = widgetIDValueTable.entrySet().iterator();
        while(it.hasNext()){
          Map.Entry currentMapEntry = (Map.Entry)it.next();
          if(currentMapEntry.getValue() != null) {
            String value = currentMapEntry.getValue().toString();
            payloadFields.get(fieldIndex).addValue(value);
          }
        }
        payloadFields.get(fieldIndex).setValuesForNative(payloadFields.get(fieldIndex).getValue().toArray(new String[payloadFields.get(fieldIndex).getValue().size()]));
      }
    }
    ReqMsgPayload reqMsg = new ReqMsgPayload();
    reqMsg.setMsgName(sensor.getSensorReqMsgsNames().get(sensorReqHandle));
    reqMsg.setMsgID(sensor.getSensorReqMsgIDs().get(sensorReqHandle));

    SensorPayloadField[] payLoadFiledArray = payloadFields.toArray(new SensorPayloadField[payloadFields.size()]);
    reqMsg.setFields(payLoadFiledArray);

    /*reset value content for next send request - corner case where repeated filed occurs */
    for(int fieldIndex = 0 ; fieldIndex < payloadFields.size() ; fieldIndex++){
      if (payloadFields.get(fieldIndex).getAccessModifier() == AccessModifier.REPEATED){
        payloadFields.get(fieldIndex).clearValue();
      }
    }

    String batchPeriod  = stdFields.getBatchPeriod();
    String flushPeriod  = stdFields.getFlushPeriod();
    boolean flushOnly   = stdFields.isFlushOnly();
    boolean maxBatch    = stdFields.isMaxBatch();

    SendReqStdFields sendReqStdFieldObj = new SendReqStdFields();
    sendReqStdFieldObj.setBatchPeriod(batchPeriod);
    sendReqStdFieldObj.setFlushPeriod(flushPeriod);
    if(flushOnly == true){
      String sF = new String("1");
      sendReqStdFieldObj.setFlushOnly(sF);
    }
    if(maxBatch == true){
      String sB = new String("1");
      sendReqStdFieldObj.setMaxBatch(sB);
    }
    sendReqStdFieldObj.setClientType(getClientProcessors().get(stdFields.getClientProcessor()).toUpperCase());
    sendReqStdFieldObj.setWakeupType(getWakeupDelivery().get(stdFields.getWakeupDevilvery()).toUpperCase());

    NativeErrorCodes errorCodeInfo = SensorContextJNI.sendRequestNativeWrapper(USTAModeType,sensorHandle, reqMsg,sendReqStdFieldObj, logFileName);
    if(errorCodeInfo == NativeErrorCodes.USTA_REQUIRED_FIELDS_MISSING){
      throwErrorDialog("Please enter required fields information" , context);
      return -1;
    }
    else if(errorCodeInfo != NativeErrorCodes.USTA_NO_ERROR ){
      throwErrorDialog("Error while Streaming the sensor " , context);
      return -1;
    }
    else {
      sensor.setSensorStreaming(sensorReqHandle, true);
      return 0;
    }

  }

  public void RegistryReadSendRequest(ModeType USTAModeType,  int sensorHandle, String regReadGroupName , Context context){

    USTALog.i(regReadGroupName);
    ReqMsgPayload reqMsg = new ReqMsgPayload();
    reqMsg.setMsgName("sns_registry_read_req");
    reqMsg.setMsgID("512");

    SensorPayloadField field_groupName = new SensorPayloadField();
    field_groupName.setAccessModifier(AccessModifier.REQUIRED);
    field_groupName.setDataType(DataType.STRING);
    field_groupName.setUserDefined(false);
    field_groupName.setFieldName("name");
    field_groupName.setEnumerated(false);
    field_groupName.setHasDefaultValue(false);
    String[] groupNameForNative = {regReadGroupName};
    field_groupName.setValuesForNative(groupNameForNative);

    SensorPayloadField[] payLoadFiledArray = {field_groupName};
    reqMsg.setFields(payLoadFiledArray);

    SendReqStdFields sendReqStdFieldObj = new SendReqStdFields();
    sendReqStdFieldObj.setClientType("SNS_STD_CLIENT_PROCESSOR_APSS");
    sendReqStdFieldObj.setWakeupType("SNS_CLIENT_DELIVERY_WAKEUP");

    NativeErrorCodes errorCodeInfo = SensorContextJNI.sendRequestNativeWrapper(USTAModeType,sensorHandle, reqMsg,sendReqStdFieldObj, "regsitry_read_info.log");

    if(errorCodeInfo == NativeErrorCodes.USTA_REQUIRED_FIELDS_MISSING){
      throwErrorDialog("Please enter group name" , context);
      return ;
    }
    else if(errorCodeInfo != NativeErrorCodes.USTA_NO_ERROR ){
      throwErrorDialog("Error while getting response from native / ssc  " , context);
      return ;
    }

  }

  public void RegistryWriteSendRequest(ModeType USTAModeType, int sensorHandle, String registryWriteMainGroupName , String registryWriteGroupName, String registryWriteVersionNumber, int position, String oneOfString, Context context ) {


    ReqMsgPayload reqMsg = new ReqMsgPayload();
    reqMsg.setMsgName("sns_registry_write_req");
    reqMsg.setMsgID("513");

    SensorPayloadField field_groupName = new SensorPayloadField();
    field_groupName.setAccessModifier(AccessModifier.REQUIRED);
    field_groupName.setDataType(DataType.STRING);
    field_groupName.setUserDefined(false);
    field_groupName.setFieldName("name");
    field_groupName.setEnumerated(false);
    field_groupName.setHasDefaultValue(false);
    String[] groupNameForNative = {registryWriteMainGroupName};
    field_groupName.setValuesForNative(groupNameForNative);



    SensorPayloadField field_registry_data = new SensorPayloadField();
    field_registry_data.setAccessModifier(AccessModifier.REQUIRED);
    field_registry_data.setDataType(DataType.USER_DEFINED);
    field_registry_data.setUserDefined(true);
    field_registry_data.setFieldName("data");


    NestedMsgPayload nestedMsgs_data = new NestedMsgPayload();
    nestedMsgs_data.setNestedMsgName("sns_registry_data");

    SensorPayloadField nested_fields_item = new SensorPayloadField();

    nested_fields_item.setAccessModifier(AccessModifier.REPEATED);
    nested_fields_item.setDataType(DataType.USER_DEFINED);
    nested_fields_item.setUserDefined(true);
    nested_fields_item.setFieldName("items");

    NestedMsgPayload nestedMsgs_item = new NestedMsgPayload();
    nestedMsgs_item.setNestedMsgName("item");

    SensorPayloadField item_fields_0 = new SensorPayloadField();
    item_fields_0.setAccessModifier(AccessModifier.REQUIRED);
    item_fields_0.setDataType(DataType.STRING);
    item_fields_0.setUserDefined(false);
    item_fields_0.setFieldName("name");
    item_fields_0.setEnumerated(false);
    item_fields_0.setHasDefaultValue(false);
    String[] filed_0 = {registryWriteGroupName};
    item_fields_0.setValuesForNative(filed_0);


    SensorPayloadField item_fields_1 = new SensorPayloadField();
    item_fields_1.setAccessModifier(AccessModifier.OPTIONAL);
    item_fields_1.setDataType(DataType.UNSIGNED_INTEGER32);
    item_fields_1.setUserDefined(false);
    item_fields_1.setFieldName("version");
    item_fields_1.setEnumerated(false);
    item_fields_1.setHasDefaultValue(false);
    String[] filed_1 = {registryWriteVersionNumber};
    item_fields_1.setValuesForNative(filed_1);


    SensorPayloadField item_fields_2 = new SensorPayloadField();

    item_fields_2.setAccessModifier(AccessModifier.REQUIRED);
    item_fields_2.setUserDefined(false);
    switch(position){
      case 0:
        item_fields_2.setDataType(DataType.STRING);
        item_fields_2.setFieldName("str");
        break;
      case 1:
        item_fields_2.setDataType(DataType.FLOAT);
        item_fields_2.setFieldName("flt");
        break;
      case 2:
        item_fields_2.setDataType(DataType.SIGNED_INTEGER64);
        item_fields_2.setFieldName("sint");
        break;
    }
    item_fields_2.setEnumerated(false);
    item_fields_2.setHasDefaultValue(false);
    String[] filed_2 = {oneOfString};
    item_fields_2.setValuesForNative(filed_2);


    SensorPayloadField[] itemFieldArray = {item_fields_0, item_fields_1, item_fields_2};
    nestedMsgs_item.setFields(itemFieldArray);

    NestedMsgPayload[] itemNestedMsg = {nestedMsgs_item};
    nested_fields_item.setNestedMsgs(itemNestedMsg);

    SensorPayloadField[] nested_fields_item_Array = {nested_fields_item} ;
    nestedMsgs_data.setFields(nested_fields_item_Array);

    NestedMsgPayload[] nestedMsgs = {nestedMsgs_data};
    field_registry_data.setNestedMsgs(nestedMsgs);

    SensorPayloadField[] payLoadFiledArray = {field_groupName, field_registry_data};
    reqMsg.setFields(payLoadFiledArray);

    SendReqStdFields sendReqStdFieldObj = new SendReqStdFields();
    sendReqStdFieldObj.setClientType("SNS_STD_CLIENT_PROCESSOR_APSS");
    sendReqStdFieldObj.setWakeupType("SNS_CLIENT_DELIVERY_WAKEUP");

    NativeErrorCodes errorCodeInfo = SensorContextJNI.sendRequestNativeWrapper(USTAModeType,sensorHandle, reqMsg,sendReqStdFieldObj, "regsitry_write_info.log");

    if(errorCodeInfo == NativeErrorCodes.USTA_REQUIRED_FIELDS_MISSING){
      throwErrorDialog("Please enter All required fields " , context);
      return ;
    }
    else if(errorCodeInfo != NativeErrorCodes.USTA_NO_ERROR ){
      throwErrorDialog("Error while getting response from native / ssc  " , context);
      return ;
    }

  }

  public static void throwErrorDialog(String errorMsg, Context context) {

    if (null == context) {
      return;
    }

    AlertDialog.Builder errDialogBuilder = new AlertDialog.Builder(context);
    errDialogBuilder.setTitle("Error");
    errDialogBuilder.setMessage(errorMsg);
    errDialogBuilder.setCancelable(false);
    errDialogBuilder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
      @Override
      public void onClick(DialogInterface dialog, int which) {

      }
    });

    AlertDialog alertDialog = errDialogBuilder.create();
    alertDialog.show();
  }

  public void stopRequest(int sensorHandle, int sensorReqHandle) {

    SensorContextJNI.stopRequestNativeWrapper(USTAModeType,sensorHandle , SensorPayloadFragment.isClientDisableRequestEnabled);

    getSensors().get(sensorHandle).setSensorStreaming(sensorReqHandle, false);
  }

  public int getSensorHandleFromSUID(String suidLow, String suidHigh) {

    for (int sensorIndex = 0; sensorIndex < sensors.size(); sensorIndex++) {
      if (suidLow.equals(sensors.get(sensorIndex).getSensorSUIDLow()) && suidHigh.equals(sensors.get(sensorIndex).getSensorSUIDHigh())) {
        return sensorIndex;
      }
    }

    return -1;
  }

}
