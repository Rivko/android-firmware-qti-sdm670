/*============================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  @file USTACmdLineReceiver.java
  @brief
   USTACmdLineReceiver class implementation for command line services extends BroadcastReceiver
============================================================================*/

package com.qualcomm.qti.usta.core;

import android.os.Bundle;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import java.math.BigInteger;
import com.qualcomm.qti.usta.ui.SensorPayloadFragment;

import java.util.Vector;


public class USTACmdLineReceiver extends BroadcastReceiver {

  private final SensorContext sensorContext;
  private final SensorLowLatencyJNI sensorDrmContext;

  private final String DELIMITERCOLON = ":";
  private final String DELIMITERSEMICOLON = ";";
  private final String RIGHTBRACE = "}";
  private final String LEFTBRACE = "{";
  private final String NAME = "Name";
  private final String DATATYPE = "DataType";
  private final String ENUMVALUES = "EnumVaules";
  private final String COMMA = ",";
  private final static int MAX_ADB_LOGCAT_LENGTH = 4000;
  private long[] sensorSuid;
  private int[] channelDb;
  private final int MAX_CHANNELS = 32;

  public USTACmdLineReceiver() {
    sensorContext = USTACmdLineService.getSensorContext();
    sensorDrmContext = USTACmdLineService.getDrmSensorContext();
    sensorSuid = new long[]{0,0};
    channelDb = new int[MAX_CHANNELS];
    for (int idx = 0; idx < MAX_CHANNELS; idx++)
      channelDb[idx] = -1;
  }

  @Override
  public void onReceive(Context context, Intent intent) {

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.GETSENSORLIST")) {
      USTALog.i("GETSENSORLIST intent");
      dumpSensorList();
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.DISABLELOGGING")) {
      USTALog.i("DISABLELOGGING intent");
      disableLogging(intent);
    }


    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.GETSENSORATTRIBUTES")) {

      USTALog.i("GETSENSORATTRIBUTES intent");
      dumpSensorAttribList(intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.GETREQUESTMSGS")) {

      USTALog.i("GETREQUESTMSGS intent");
      dumpReqMsgList(intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.ENABLE")) {

      USTALog.i("ENABLE intent");
      sendReqMsg(intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.DISABLE")) {

      USTALog.i("DISABLE intent");
      stopReqMsg(intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.CREATE_DRMCHANNEL")) {
      USTALog.i("CREATE_DRMCHANNEL intent");
      createDrmChannel(context, intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.DELETE_DRMCHANNEL")) {
      USTALog.i("DELETE_DRMCHANNEL intent");
      deleteDrmChannel(intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.CONFIGURE_DRMCHANNEL")) {
      USTALog.i("CONFIGURE_DRMCHANNEL intent");
      configureDrmChannel(intent);
    }

    if (intent.getAction().contentEquals("com.qualcomm.qti.usta.core.intent.GET_DRMATTRIBUTES")) {
      USTALog.i("GET_DRMATTRIBUTES intent");
      getDrmSupportedRate(intent);
    }

  }

  private void configureDrmChannel(Intent intent) {

    USTALog.i("configureDrmChannel called ");
    int channel_id = intent.getIntExtra("channel_id", -1);
    int samplerate = intent.getIntExtra("drmrate", -1);
    int flags = intent.getIntExtra("flags", -1);
    USTALog.i("channel_id " + channel_id + ", samplerate " + samplerate + ", flags " + flags);

    String sensorSuidLow = intent.getStringExtra("suid_low");
    String sensorSuidHigh = intent.getStringExtra("suid_high");
    USTALog.i("String suid_low " + sensorSuidLow + " & suid_high " + sensorSuidHigh);
    sensorSuid[0] = new BigInteger(sensorSuidLow,16).longValue();
    sensorSuid[1] = new BigInteger(sensorSuidHigh,16).longValue();
    USTALog.i("sensorSuid low " + sensorSuid[0] + " & sensorSuid high " + sensorSuid[1]);

    int samplePeriodUs = sensorDrmContext.lowLatChannelSampleRate(samplerate);
    if ( -1 != samplePeriodUs) {
      if ((-1 < channel_id) && (MAX_CHANNELS > channel_id)){
        if (-1 != channelDb[channel_id]) {
          USTALog.i("configure drm channel handle " + channelDb[channel_id] + ", samplePeriodUs " + samplePeriodUs);
          USTALog.i("suid_low " + sensorSuid[0] + ", suid_high " + sensorSuid[1] + ", flags " + flags);
          if (samplePeriodUs != 0)
            sensorDrmContext.lowLatStart(channelDb[channel_id], sensorSuid, samplePeriodUs, flags);
          else
            sensorDrmContext.lowLatStop(channelDb[channel_id]);
          } else {
            USTALog.e("IMPROPERINPUT: no drm channel handle for channel_id " + channel_id);
          }
        } else {
          USTALog.e("IMPROPERINPUT:  allowed channel_ids 0-" + ( MAX_CHANNELS -1 ));
        }
    } else {
      USTALog.e("IMPROPERINPUT:  improper samplerate " + samplerate);
    }
    USTALog.i("configureDrmChannel completed ");
  }

  private void createDrmChannel(Context context, Intent intent){

    USTALog.i("createDrmChannel called ");
    int bufferSize = intent.getIntExtra("buffersize", -1);
    int drm_channel = -1;
    if (-1 != bufferSize) {
      USTALog.i("buffer size " + bufferSize);
      int channel_id = intent.getIntExtra("channel_id", -1);
      USTALog.i("channel_id " + channel_id);
      if ((-1 < channel_id) && (MAX_CHANNELS > channel_id)){
        if (channelDb[channel_id] == -1) {
          USTALog.i("get drm channel handle for channel_id " + channel_id);
          drm_channel = sensorDrmContext.lowLatGetChannel(bufferSize);
          USTALog.i("drm channel " + drm_channel);
          channelDb[channel_id] = drm_channel;
        } else {
          USTALog.i("channle handle already available " + channelDb[channel_id] + " for " + channel_id);
        }
      } else {
        USTALog.e("IMPROPERINPUT:  allowed channel_ids 0-" + ( MAX_CHANNELS -1 ));
      }
    } else {
      USTALog.e("IMPROPERINPUT: buffersize improper " + bufferSize);
    }
    USTALog.i("createDrmChannel done ");

  }


   private void getDrmSupportedRate(Intent intent) {

     USTALog.i("getDrmSupportedRate called ");
     String sensorSUIDLow = intent.getStringExtra("suid_low");
     String sensorSUIDHigh = intent.getStringExtra("suid_high");

     sensorSuid[0] = new BigInteger(sensorSUIDLow,16).longValue();
     sensorSuid[1] = new BigInteger(sensorSUIDHigh,16).longValue();
     USTALog.i("sensorSuid low " + sensorSuid[0] + " & sensorSuid high " + sensorSuid[1]);

     int max_rate = sensorDrmContext.lowLatGetMaxRate(sensorSuid);
     USTALog.i("supported max rate " + max_rate);
     USTALog.i("getDrmSupportedRate done");

  }


  private void deleteDrmChannel(Intent intent){

    USTALog.i("deleteDrmChannel called ");
    int channel_id = intent.getIntExtra("channel_id", -1);
    USTALog.i("channel_id " + channel_id);

    if ((-1 < channel_id) && (MAX_CHANNELS > channel_id)){
      USTALog.i("closing drm channel " + channelDb[channel_id]);
      if ( -1 != channelDb[channel_id]) {
        sensorDrmContext.lowLatCloseChannel(channelDb[channel_id]);
        USTALog.i("closed drm channel " + channelDb[channel_id]);
        channelDb[channel_id] = -1;
      } else {
        USTALog.i("IMPROPERINPUT: drm channel not availalbe for " + channel_id);
      }
    } else {
      USTALog.e("IMPROPERINPUT:  allowed channel_ids 0-" + ( MAX_CHANNELS -1 ));
    }
    USTALog.i("deleteDrmChannel done ");

  }

  private void disableLogging(Intent intent){
    int disable_logging = intent.getIntExtra("disable_logging", -1);
    if (disable_logging == 0) {
      USTALog.isLoggingDisabled = false;
      SensorContextJNI.updateLoggingFlagWrapper();
    }
    else if (disable_logging == 1) {
      USTALog.isLoggingDisabled = true;
      SensorContextJNI.updateLoggingFlagWrapper();
    }
    else{
      USTALog.e("Wrong value from command line instruction");
    }

  }

  private void updateSensorPayloadField(Vector<SensorPayloadField> payloadFieldVector, Intent intent) {

    for (SensorPayloadField payloadField : payloadFieldVector) {

      DataType dataType = payloadField.getDataType();

      switch (dataType) {
        case ENUM:
        case SIGNED_INTEGER32:
        case UNSIGNED_INTEGER32:
        case SIGNED_INTEGER64:
        case UNSIGNED_INTEGER64:
          String suidLow = "suid_low";
          String suidHigh = "suid_high";
          if (payloadField.getFieldName().equals(suidLow) || payloadField.getFieldName().equals(suidHigh)) {
            String suidPart = intent.getStringExtra(payloadField.getFieldName());
            payloadField.addValue(suidPart);
          } else {
            int intField = intent.getIntExtra(payloadField.getFieldName(), -1);
            if(intField != -1)
              payloadField.addValueToWidgetIDValueTable(Integer.toString(intField), 999);
          }
          break;

        case FLOAT:
          float floatField = intent.getFloatExtra(payloadField.getFieldName(), (float) -1.0);
          if(floatField != -1)
            payloadField.addValueToWidgetIDValueTable(Float.toString(floatField), 999);
          break;

        case BOOLEAN:
          boolean boolField = intent.getBooleanExtra(payloadField.getFieldName(), false);
          payloadField.addValueToWidgetIDValueTable(Boolean.toString(boolField), 999);
          break;

        case USER_DEFINED:
          for (int nestedMsgIndex = 0; nestedMsgIndex < payloadField.getNestedMsgSize(); nestedMsgIndex++) {
            Vector<SensorPayloadField> sensorPayloadFields;
            sensorPayloadFields = new Vector<>();
            for (int payLoadFieldIndex = 0; payLoadFieldIndex < (payloadField.getNestedMsgs())[nestedMsgIndex].getFieldCount(); payLoadFieldIndex++) {
              sensorPayloadFields.add((payloadField.getNestedMsgs())[nestedMsgIndex].getFiledAt(payLoadFieldIndex));
            }
            updateSensorPayloadField(sensorPayloadFields, intent);
          }
          break;

        case STRING:
          String stringField = intent.getStringExtra(payloadField.getFieldName());
          if(stringField != null && stringField.length() != 0)
            payloadField.addValue(stringField);
      }

    }
  }

  private void sendReqMsg(Intent intent) {

    int sensorHandle = intent.getIntExtra("sensorhandle", -1);

    if (-1 == sensorHandle) {
      String sensorSUIDLow = intent.getStringExtra("suid_low");
      String sensorSUIDHigh = intent.getStringExtra("suid_high");
      sensorHandle = sensorContext.getSensorHandleFromSUID(sensorSUIDLow, sensorSUIDHigh);
    }

    if (sensorHandle == -1) {
      USTALog.e("Improper Sensor SUID is passed. Please check and send valid SUID !");
      return;
    }

    String logFileName = intent.getStringExtra("logfile");

    if (logFileName == null) {
      USTALog.i("LogFileName was not entered by Users. So Taking the default logFileName ");
      logFileName = "";

    }
    int reqMsgID = intent.getIntExtra("msg_id", 0);
    Sensor sensorInstance = sensorContext.getSensors().get(sensorHandle);
    boolean msgIDValid = false;
    int reqMsgHandle = -1;

    if (sensorInstance == null) {

      USTALog.e("Improper Sensor Handle passed. Please check and send valid Handle!");
      return;
    }
    USTALog.d("User MSg ID " + reqMsgID);

    for (String msgID : sensorInstance.getSensorReqMsgIDs()) {

      reqMsgHandle++;
      USTALog.d("current MSg ID " + msgID);
      if (msgID.compareTo(Integer.toString(reqMsgID)) == 0) {
        msgIDValid = true;
        USTALog.d(Integer.toString(reqMsgHandle));
        break;
      }
    }

    if (!msgIDValid) {

      USTALog.e("Incorrect Request Msg ID passed. Please check and send valid Request Msg ID!");

      return;
    }



      int clientproc = intent.getIntExtra("client", 0);
      int wakeupType = intent.getIntExtra("wakeup", 0);
      int batchperiod = intent.getIntExtra("batch_period", -1);
      int flushperiod = intent.getIntExtra("flush_period", -1);
      boolean flushOnly = intent.getBooleanExtra("flush_only", false);
      boolean maxBatch = intent.getBooleanExtra("max_batch", false);

      if(batchperiod != -1)
        sensorInstance.getSensorStdFields(reqMsgHandle).setBatchPeriod(Integer.toString(batchperiod));

      sensorInstance.getSensorStdFields(reqMsgHandle).setFlushOnly(flushOnly);
      sensorInstance.getSensorStdFields(reqMsgHandle).setMaxBatch(maxBatch);
      sensorInstance.getSensorStdFields(reqMsgHandle).setClientProcessor(clientproc);
      sensorInstance.getSensorStdFields(reqMsgHandle).setWakeupDevilvery(wakeupType);

      if(flushperiod != -1)
        sensorInstance.getSensorStdFields(reqMsgHandle).setFlushPeriod(Integer.toString(flushperiod));

    String registryName = "registry";
    if(!sensorContext.getSensorNames().get(sensorHandle).contains(registryName)) {
      updateSensorPayloadField(sensorInstance.getSensorPayloadField(reqMsgHandle), intent);
      sensorContext.sendRequest(sensorHandle, reqMsgHandle, null, logFileName);
    }
    else if(reqMsgID == 512){
      updateSensorPayloadField(sensorInstance.getSensorPayloadField(reqMsgHandle), intent);
      sensorContext.sendRequest(sensorHandle, reqMsgHandle, null, logFileName);
    }
    else if(reqMsgID == 513) {
      USTALog.i("registry write case ");
      String subMessageName = intent.getStringExtra("sub_message_name");
      if(subMessageName == null || subMessageName.length() == 0){
        USTALog.e("Incorrect subMessageName is not passed. Please check again and try");
        return;
      }

      String dataName = intent.getStringExtra("data_name");
      if(dataName == null || dataName.length() == 0){
        USTALog.e("Incorrect dataName is not passed. Please check again and try");
        return;
      }

      int version = intent.getIntExtra("version", -1);
      if(version == -1){
        USTALog.e("version number is not supplied. Please supply it to proceed further ");
        return;
      }

      String oneOfFiledName = intent.getStringExtra("oneof_name");
      if(oneOfFiledName == null || oneOfFiledName.length() == 0){
        USTALog.e("oneOfFiledName is not passed. Please check again and try");
        return;
      }
      int position = -1;
      if(oneOfFiledName.equals("sint")){
        position = 2 ;
      }
      else if(oneOfFiledName.equals("flt")){
        position = 1 ;
      }
      else if(oneOfFiledName.equals("str")){
        position = 0 ;
      }

      String oneOfValue = intent.getStringExtra("one_of_value");
      if(subMessageName == null || subMessageName.length() == 0){
        USTALog.e("Incorrect subMessageName is not passed. Please check again and try");
        return;
      }
      sensorContext.RegistryWriteSendRequest(ModeType.USTA_MODE_TYPE_COMMAND_LINE, sensorHandle, subMessageName, dataName, Integer.toString(version), position,oneOfValue, null);
    }
  }

  private void stopReqMsg(Intent intent) {

    int sensorHandle = intent.getIntExtra("sensorhandle", -1);

    if (-1 == sensorHandle) {
      String sensorSUIDLow = intent.getStringExtra("suid_low");
      String sensorSUIDHigh = intent.getStringExtra("suid_high");
      sensorHandle = sensorContext.getSensorHandleFromSUID(sensorSUIDLow, sensorSUIDHigh);
    }

    if (sensorHandle == -1) {
      USTALog.e("Improper Sensor SUID is passed. Please check and send valid SUID !");

      return;
    }

    USTALog.d("stop request from command line for sensor " + Integer.toString(sensorHandle));
    Sensor sensorInstance = sensorContext.getSensors().get(sensorHandle);
    if (sensorInstance == null) {
      USTALog.e("Improper Sensor Handle passed. Please check and send valid Handle!");
      return;
    }
    sensorContext.stopRequest(sensorHandle, -1);
    USTALog.d("after stop request for the sensorhandle " + Integer.toString(sensorHandle));
  }

  private void dumpSensorList() {

    Vector<String> sensorNames = sensorContext.getSensorNames();

    for (int index = 0; index < sensorNames.size(); index++) {

      USTALog.d("Sensor Handle" + DELIMITERCOLON + index +
              " Sensor Name" + DELIMITERCOLON + sensorNames.get(index) +
              " Sensor SUID Low" + DELIMITERCOLON + sensorContext.getSensors().get(index).getSensorSUIDLow() +
              " Sensor SUID High" + DELIMITERCOLON + sensorContext.getSensors().get(index).getSensorSUIDHigh());
    }
  }

  private void dumpSensorAttribList(Intent intent) {

    int sensorHandle = intent.getIntExtra("sensorhandle", -1);

    if (-1 == sensorHandle) {

      String sensorSUIDLow = intent.getStringExtra("suid_low");
      String sensorSUIDHigh = intent.getStringExtra("suid_high");

      sensorHandle = sensorContext.getSensorHandleFromSUID(sensorSUIDLow, sensorSUIDHigh);
    }
    if (-1 == sensorHandle) {
      USTALog.e("Improper Sensor SUID is passed. Please check and send valid SUID !");
    }

    if(SensorContextJNI.getSensorAttributesNativeWrapper(sensorContext.getUSTAModeType(), sensorHandle).length() <= MAX_ADB_LOGCAT_LENGTH) {
      USTALog.d(SensorContextJNI.getSensorAttributesNativeWrapper(sensorContext.getUSTAModeType(), sensorHandle));
    }
    else {
      int maxLoopCount = SensorContextJNI.getSensorAttributesNativeWrapper(sensorContext.getUSTAModeType(), sensorHandle).length() / MAX_ADB_LOGCAT_LENGTH + 1;
      String remainingLogBuffer = SensorContextJNI.getSensorAttributesNativeWrapper(sensorContext.getUSTAModeType(), sensorHandle);
      int loopCount = 0;
      while(loopCount < maxLoopCount) {
        int startIndex = 0;
        int endIndex = (remainingLogBuffer.length() > MAX_ADB_LOGCAT_LENGTH )?MAX_ADB_LOGCAT_LENGTH:remainingLogBuffer.length();
        String subString = remainingLogBuffer.substring(startIndex, endIndex);
        USTALog.d(subString);
        loopCount = loopCount + 1 ;
        remainingLogBuffer = remainingLogBuffer.substring(endIndex );
      }
    }

  }

  private void dumpReqMsgList(Intent intent) {

    int sensorHandle = intent.getIntExtra("sensorhandle", -1);

    if (-1 == sensorHandle) {

      String sensorSUIDLow = intent.getStringExtra("suid_low");
      String sensorSUIDHigh = intent.getStringExtra("suid_high");

      sensorHandle = sensorContext.getSensorHandleFromSUID(sensorSUIDLow, sensorSUIDHigh);
    }

    if (sensorHandle == -1) {

      USTALog.e( "Improper Sensor SUID is passed. Please check and send valid SUID !");
      return;
    }

    Sensor sensorInstance = sensorContext.getSensors().get(sensorHandle);

    if (sensorInstance == null) {

      USTALog.e("Improper Sensor Handle passed. Please check and send valid Handle!");
    } else {

      for (int reqMsgIndex = 0; reqMsgIndex < sensorInstance.getSensorReqMsgsNames().size(); reqMsgIndex++) {

        String reqMsgName = sensorInstance.getSensorReqMsgsNames().get(reqMsgIndex);
        String reqMsgID = sensorInstance.getSensorReqMsgIDs().get(reqMsgIndex);
        String payloadMsgFields;

        Vector<String> clientProcessors = sensorContext.getClientProcessors();
        String clientProcessorString = "";

        for (int clientProcIndex = 0; clientProcIndex < clientProcessors.size(); clientProcIndex++) {
          clientProcessorString = clientProcessorString + clientProcessors.get(clientProcIndex);
          if (clientProcIndex < clientProcessors.size() - 1) {
            clientProcessorString = clientProcessorString + COMMA;
          }
        }

        Vector<String> wakeupDelivery = sensorContext.getWakeupDelivery();
        String wakeupDeliveryString = "";

        for (int wakeupDeliveryIndex = 0; wakeupDeliveryIndex < wakeupDelivery.size(); wakeupDeliveryIndex++) {
          wakeupDeliveryString = wakeupDeliveryString + wakeupDelivery.get(wakeupDeliveryIndex);
          if (wakeupDeliveryIndex < wakeupDelivery.size() - 1) {
            wakeupDeliveryString = wakeupDeliveryString + COMMA;
          }
        }

        // Req Msg Name and Msg ID
        USTALog.d(reqMsgName + DELIMITERCOLON + reqMsgID);

        // Req Msg Std Fields
        USTALog.d("{Name:clientprocessor;DataType:enum;EnumValues:" + clientProcessorString + ";}");
        USTALog.d("{Name:wakeupDelivery;DataType:enum;EnumValues:" + wakeupDeliveryString + ";}");
        USTALog.d("{Name:batch_period;DataType:uint32;}");
        USTALog.d("{Name:flush_period;DataType:uint32;}");

        for (int payloadIndex = 0; payloadIndex < sensorInstance.getSensorPayloadField(reqMsgIndex).size(); payloadIndex++) {

          SensorPayloadField payloadField = sensorInstance.getSensorPayloadField(reqMsgIndex).get(payloadIndex);

          payloadMsgFields = "";
          payloadMsgFields = payloadMsgFields + LEFTBRACE + NAME + DELIMITERCOLON + payloadField.getFieldName() + DELIMITERSEMICOLON +
                  DATATYPE + DELIMITERCOLON + payloadField.getDataType().toString().toLowerCase() + DELIMITERSEMICOLON;

          if (payloadField.getDataType() == DataType.ENUM) {

            payloadMsgFields = payloadMsgFields + ENUMVALUES + DELIMITERCOLON;

            for (String enumVal : payloadField.getEnumValues()) {
              payloadMsgFields = payloadMsgFields + enumVal + COMMA;
            }

          }

          payloadMsgFields = payloadMsgFields + RIGHTBRACE;

          USTALog.d( payloadMsgFields);
        }
      }
    }
  }

  public IntentFilter getIntentFilter() {

    IntentFilter intentFilter = new IntentFilter();

    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.GETSENSORLIST");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.GETSENSORATTRIBUTES");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.GETREQUESTMSGS");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.ENABLE");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.DISABLE");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.DISABLELOGGING");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.CREATE_DRMCHANNEL");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.DELETE_DRMCHANNEL");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.CONFIGURE_DRMCHANNEL");
    intentFilter.addAction("com.qualcomm.qti.usta.core.intent.GET_DRMATTRIBUTES");
    return intentFilter;
  }
}
