/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2014, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_LBSApiV02"
#include <LBSApiV02.h>
#include <location_service_v02.h>
#include <loc_api_sync_req.h>
#include <loc_api_v02_log.h>
#include <IzatApiV02.h>
#include <LocApiProxy.h>
#include <LBSProxy.h>
#include <loc_pla.h>
#include <inttypes.h>

#define LAT_SCALE_FACTOR 23860929.4222
#define LONG_SCALE_FACTOR 11930464.7111
#define LAT_SCALE_DOWN(x) ((double)(x / LAT_SCALE_FACTOR))
#define LONG_SCALE_DOWN(x) ((double)(x / LONG_SCALE_FACTOR))

using namespace loc_core;
using namespace izat_core;

namespace lbs_core {

LBSApiV02*
LBSApiV02 :: createLBSApiV02(const MsgTask* msgTask,
                          LOC_API_ADAPTER_EVENT_MASK_T exMask,
                          ContextBase* context)
{
    if (NULL == msgTask) {
        LOC_LOGE("%s:%d]: msgTask cannot be NULL", __func__, __LINE__);
        return NULL;
    }
    return new LBSApiV02(msgTask, exMask, context);
}

LBSApiV02 :: LBSApiV02(const MsgTask* msgTask,
                       LOC_API_ADAPTER_EVENT_MASK_T exMask,
                       ContextBase* context):
    LocApiV02(msgTask, exMask, context),
    LBSApiBase(new LocApiProxyV02(this)),
    mOpaqueId(0)
{
    LOC_LOGD("%s:%d]: LBSApiV02 created. lbsapi: %p; locApiProxy:%p\n",
             __func__, __LINE__, this, mLocApiProxy);
}

LBSApiV02 :: ~LBSApiV02()
{
    delete mLocApiProxy;
}

void LBSApiV02::eventCb(locClientHandleType clientHandle,
                         uint32_t eventId,
                         locClientEventIndUnionType eventPayload)
{
    LOC_LOGV ("%s:%d] client = %p, event id = %d, event name = %s payload = %p\n",
              __func__,  __LINE__,  clientHandle, eventId, loc_get_v02_event_name(eventId),
              &eventPayload);

    switch(eventId) {
    case  QMI_LOC_EVENT_WIFI_REQ_IND_V02:
        if (NULL != eventPayload.pWifiReqEvent) {
            WpsEvent(eventPayload.pWifiReqEvent);
        } else {
            LOC_LOGE("%s: WifiReqIndMsg is NULL when it shouldn't be", __FUNCTION__);
        }
        break;
    case QMI_LOC_EVENT_INJECT_WIFI_AP_DATA_REQ_IND_V02:
        if (NULL != eventPayload.pWifiApDataReqEvent) {
            WifiApDataRequest request = {false};
            if (eventPayload.pWifiApDataReqEvent->e911Mode_valid) {
                request.emergencyRequest = eventPayload.pWifiApDataReqEvent->e911Mode;
            }

            requestWifiApData(request);
        } else {
            LOC_LOGE("%s: WifiApDataIndMsg is NULL when it shouldn't be", __FUNCTION__);
        }
        break;
    case QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02:
        if( NULL != eventPayload.pTimeSyncReqEvent )
        {
            TimeRequest timeRequest;
            memset(&timeRequest, 0, sizeof(timeRequest));
            timeRequest.clientTxTime = eventPayload.pTimeSyncReqEvent->refCounter;
            requestTimeData(timeRequest);
        }
        else
        {
            LOC_LOGE("%s: TimeSyncIndMsg is NULL when it shouldn't be", __FUNCTION__);
        }
        break;

    case QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02:
        if( NULL != eventPayload.pSensorStreamingReadyStatusEvent )
        {

            if(eventPayload.pSensorStreamingReadyStatusEvent->accelReady_valid)
            {
                doSensorDataRequest(
                        eventPayload.pSensorStreamingReadyStatusEvent->accelReady,
                        ACCEL);
            }

            if(eventPayload.pSensorStreamingReadyStatusEvent->gyroReady_valid)
            {
                doSensorDataRequest(
                        eventPayload.pSensorStreamingReadyStatusEvent->gyroReady,
                        GYRO);
            }

            if(eventPayload.pSensorStreamingReadyStatusEvent->accelTemperatureReady_valid)
            {
                doSensorDataRequest(
                        eventPayload.pSensorStreamingReadyStatusEvent->accelTemperatureReady,
                        ACCEL_TEMPERATURE);
            }

            if(eventPayload.pSensorStreamingReadyStatusEvent->gyroTemperatureReady_valid)
            {
                doSensorDataRequest(
                        eventPayload.pSensorStreamingReadyStatusEvent->gyroTemperatureReady,
                        GYRO_TEMPERATURE);
            }

            if(eventPayload.pSensorStreamingReadyStatusEvent->calibratedMagReady_valid)
            {
                doSensorDataRequest(
                        eventPayload.pSensorStreamingReadyStatusEvent->calibratedMagReady,
                        MAG_CALIB);
            }

            if(eventPayload.pSensorStreamingReadyStatusEvent->uncalibratedMagReady_valid)
            {
                doSensorDataRequest(
                        eventPayload.pSensorStreamingReadyStatusEvent->uncalibratedMagReady,
                        MAG_UNCALIB);
            }

        }
        else
        {
            LOC_LOGE("%s: SensorStreamingReadyStatusEvent is NULL when it shouldn't be", __FUNCTION__);
        }
        break;

    case QMI_LOC_EVENT_SET_SPI_STREAMING_REPORT_IND_V02:
        if( NULL != eventPayload.pSetSpiStreamingReportEvent )
        {
            LOC_LOGV("%s:%d] received spi request ...\n", __func__, __LINE__);
            SensorRequest sensorRequest;
            memset(&sensorRequest, 0, sizeof(sensorRequest));

            sensorRequest.requestType = SPI;
            sensorRequest.requestValue = (SensorRequestType)
                eventPayload.pSetSpiStreamingReportEvent->enable;
            sensorRequest.frequencyValid = false;
            requestSPIStatus(sensorRequest);
        }
        else
        {
            LOC_LOGE("%s: SetSpiStreamingReportEvent is NULL when it shouldn't be", __FUNCTION__);
        }
        break;

    case QMI_LOC_EVENT_MOTION_DATA_CONTROL_IND_V02:
        if( NULL != eventPayload.pMotionDataControlEvent )
        {

            SensorRequest sensorRequest;
            memset(&sensorRequest, 0, sizeof(sensorRequest));

            sensorRequest.requestType = MOTION;
            sensorRequest.requestValue = (SensorRequestType)
                eventPayload.pMotionDataControlEvent->requestMotionData;
            sensorRequest.frequencyValid = false;

            LOC_LOGV("%s:%d] received motion data request with enable %d...\n",
                     __func__, __LINE__,sensorRequest.requestValue);
            requestMotionData(sensorRequest);

        }
        else
        {
            LOC_LOGE("%s: InjectMotionDataEvent is NULL when it shouldn't be", __FUNCTION__);
        }
        break;

    case QMI_LOC_EVENT_PEDOMETER_CONTROL_IND_V02:
        if( NULL != eventPayload.pPedometerControlEvent )
        {
            PedometerRequest sensorRequest;
            memset(&sensorRequest, 0, sizeof(sensorRequest));

            sensorRequest.requestValue = (SensorRequestType)
                eventPayload.pPedometerControlEvent->requestPedometerData;
            sensorRequest.resetStepCountValid =
                eventPayload.pPedometerControlEvent->resetStepCount_valid;
            sensorRequest.resetStepCount =
                eventPayload.pPedometerControlEvent->resetStepCount;
            sensorRequest.stepCountThresholdValid =
                eventPayload.pPedometerControlEvent->stepCountThreshold_valid;
            sensorRequest.stepCountThreshold =
                eventPayload.pPedometerControlEvent->stepCountThreshold;

            LOC_LOGV("%s:%d] received pedometer request with enable %d...\n",
                     __func__, __LINE__,sensorRequest.requestValue);
            requestPedometerData(sensorRequest);

        }
        else
        {
            LOC_LOGE("%s: PedometerControlEvent is NULL when it shouldn't be", __FUNCTION__);
        }
        break;

    case QMI_LOC_EVENT_GET_TIME_ZONE_INFO_IND_V02:
       requestTimeZoneInfo();
        break;

    case QMI_LOC_EVENT_INJECT_SRN_AP_DATA_REQ_IND_V02:
        if( NULL != eventPayload.pInjectSrnApDataReqEvent ) {
            const qmiLocEventInjectSrnApDataReqIndMsgT_v02 *pSrnEventDataReq =
                                            eventPayload.pInjectSrnApDataReqEvent;
            GnssSrnDataReq gnssSrnDataReq;
            memset(&gnssSrnDataReq, 0, sizeof(gnssSrnDataReq));

            if (pSrnEventDataReq->srnTechMask & QMI_LOC_SRN_AP_DATA_TECH_TYPE_BT_V02)
            {
                gnssSrnDataReq.srnTechType = SRN_AP_DATA_TECH_TYPE_BT;
            }
            else if (pSrnEventDataReq->srnTechMask & QMI_LOC_SRN_AP_DATA_TECH_TYPE_BTLE_V02)
            {
                gnssSrnDataReq.srnTechType = SRN_AP_DATA_TECH_TYPE_BTLE;
            }
            else if (pSrnEventDataReq->srnTechMask & QMI_LOC_SRN_AP_DATA_TECH_TYPE_NFC_V02)
            {
                gnssSrnDataReq.srnTechType = SRN_AP_DATA_TECH_TYPE_NFC;
            }
            else if (pSrnEventDataReq->srnTechMask & QMI_LOC_SRN_AP_DATA_TECH_TYPE_MOBILE_CODE_V02)
            {
                gnssSrnDataReq.srnTechType = SRN_AP_DATA_TECH_TYPE_MOBILE_CODE;
            }
            else if (pSrnEventDataReq->srnTechMask & QMI_LOC_SRN_AP_DATA_TECH_TYPE_OTHER_V02)
            {
                gnssSrnDataReq.srnTechType = SRN_AP_DATA_TECH_TYPE_OTHER;
            }

            gnssSrnDataReq.srnRequest = pSrnEventDataReq->srnRequest;
            if (pSrnEventDataReq->e911Mode_valid)
                gnssSrnDataReq.e911Mode = pSrnEventDataReq->e911Mode;
            if (pSrnEventDataReq->srnApMacAddrType_valid)
            {
                switch(pSrnEventDataReq->srnApMacAddrType)
                {
                case eQMI_LOC_SRN_AP_DATA_PUBLIC_MAC_ADDR_V02:
                    gnssSrnDataReq.macAddrType = SRN_AP_DATA_PUBLIC_MAC_ADDR_TYPE_PUBLIC;
                    break;
                case eQMI_LOC_SRN_AP_DATA_PRIVATE_MAC_ADDR_V02:
                    gnssSrnDataReq.macAddrType = SRN_AP_DATA_PUBLIC_MAC_ADDR_TYPE_PRIVATE;
                    break;
                case eQMI_LOC_SRN_AP_DATA_OTHER_MAC_ADDR_V02:
                    gnssSrnDataReq.macAddrType = SRN_AP_DATA_PUBLIC_MAC_ADDR_TYPE_OTHER;
                    break;
                default:
                    break;
                }
            }

            requestSrnData(gnssSrnDataReq);
        }
        else {
            LOC_LOGE("%s: InjectSrnApDataReqEvent is NULL when it shouldn't be", __FUNCTION__);
        }
        break;

    case QMI_LOC_EVENT_FDCL_SERVICE_REQ_IND_V02:
        requestFdclDataEvent();
        break;
    default:
        //Check with IzatApiV02 if the event can be handled
        if(!((LocApiProxyV02 *)mLocApiProxy)->eventCb(clientHandle, eventId, eventPayload))
            //Event handled in IzatApiV02. Break.
            break;
        //Check with LocApiV02 if event can be handled
        LocApiV02::eventCb(clientHandle, eventId, eventPayload);
    }
}

void LBSApiV02::WpsEvent(const qmiLocEventWifiReqIndMsgT_v02* request)
{
    OdcpiRequest odcpiRequest = {UNKNOWN, false};

    switch (request->requestType) {
    case eQMI_LOC_WIFI_START_PERIODIC_HI_FREQ_FIXES_V02:
        odcpiRequest.requestType = HIGH;
        break;
    case eQMI_LOC_WIFI_START_PERIODIC_KEEP_WARM_V02:
        odcpiRequest.requestType = LOW;
        break;
    case eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02:
        odcpiRequest.requestType = STOP;
        break;
    default:
        odcpiRequest.requestType = UNKNOWN;
    }

    // ignore TBF as it not set on the modem side

    if (request->e911Mode_valid) {
        odcpiRequest.emergencyRequest = request->e911Mode;
    }

    requestWps(odcpiRequest);
}

int LBSApiV02::wifiStatusInform()
{
    qmiLocNotifyWifiStatusReqMsgT_v02 wifiStatusReq;
    int status = 1;
    memset(&wifiStatusReq, 0, sizeof(wifiStatusReq));
    wifiStatusReq.wifiStatus = (qmiLocWifiStatusEnumT_v02)status;

    LOC_LOGV("%s:%d] informing wifi status ...\n", __func__, __LINE__);
    LOC_SEND_SYNC_REQ(NotifyWifiStatus, NOTIFY_WIFI_STATUS, wifiStatusReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::injectCoarsePosition(const CoarsePositionInfo &cpInfo)
{
    qmiLocInjectPositionReqMsgT_v02 injectPositionReq;
    memset(&injectPositionReq, 0, sizeof(injectPositionReq));

    injectPositionReq.latitude_valid = cpInfo.latitudeValid;
    injectPositionReq.latitude = cpInfo.latitude;

    injectPositionReq.longitude_valid = cpInfo.longitudeValid;
    injectPositionReq.longitude = cpInfo.longitude;

    injectPositionReq.horUncCircular_valid = cpInfo.horUncCircularValid;
    injectPositionReq.horUncCircular = cpInfo.horUncCircular;
    if (injectPositionReq.horUncCircular < 1000) {
        injectPositionReq.horUncCircular = 1000;
    }

    injectPositionReq.horConfidence_valid = 1;
    injectPositionReq.horConfidence = 68;

    injectPositionReq.rawHorUncCircular_valid = cpInfo.horUncCircularValid;
    injectPositionReq.rawHorUncCircular = cpInfo.horUncCircular;

    injectPositionReq.rawHorConfidence_valid = cpInfo.horConfidenceValid;
    injectPositionReq.rawHorConfidence = cpInfo.horConfidence;

    injectPositionReq.horReliability_valid = cpInfo.horReliabilityValid;
    injectPositionReq.horReliability = mapReliabilityValue(cpInfo.horReliability);

    injectPositionReq.altitudeWrtEllipsoid_valid = cpInfo.altitudeWrtEllipsoidValid;
    injectPositionReq.altitudeWrtEllipsoid = cpInfo.altitudeWrtEllipsoid;

    injectPositionReq.altitudeWrtMeanSeaLevel_valid = cpInfo.altitudeWrtMeanSeaLevelValid;
    injectPositionReq.altitudeWrtMeanSeaLevel = cpInfo.altitudeWrtMeanSeaLevel;

    injectPositionReq.vertUnc_valid = cpInfo.vertUncValid;
    injectPositionReq.vertUnc = cpInfo.vertUnc;

    injectPositionReq.vertConfidence_valid = cpInfo.vertConfidenceValid;
    injectPositionReq.vertConfidence = cpInfo.vertConfidence;

    injectPositionReq.vertReliability_valid = cpInfo.vertReliabilityValid;
    injectPositionReq.vertReliability = mapReliabilityValue(cpInfo.vertReliability);

    injectPositionReq.timestampUtc_valid = cpInfo.timestampUtcValid;
    injectPositionReq.timestampUtc = cpInfo.timestampUtc;
    if (!injectPositionReq.timestampUtc_valid) {
        struct timespec time_info_current;
        clock_gettime(CLOCK_REALTIME,&time_info_current);
        injectPositionReq.timestampUtc_valid = 1;
        injectPositionReq.timestampUtc = (time_info_current.tv_sec)*1e3 +
                                         (time_info_current.tv_nsec)/1e6;
        LOC_LOGV("%s:%d] inject timestamp from system: %" PRIu64,
                 __func__, __LINE__, injectPositionReq.timestampUtc);
    }

    injectPositionReq.timestampAge_valid = cpInfo.timestampAgeValid;
    injectPositionReq.timestampAge = cpInfo.timestampAge;

    injectPositionReq.positionSrc_valid = cpInfo.positionSrcValid;
    injectPositionReq.positionSrc = mapSourceType(cpInfo.positionSrc);

    injectPositionReq.positionSrcProvider_valid = cpInfo.positionSrcProviderValid;
    injectPositionReq.positionSrcProvider = mapSourceProviderType(cpInfo.positionSrcProvider);

    injectPositionReq.onDemandCpi_valid = cpInfo.onDemandCpiValid;
    injectPositionReq.onDemandCpi = cpInfo.onDemandCpi;

    /* Log */
    LOC_LOGD("%s:%d]: Lat=%lf, Lon=%lf, Acc=%.2lf rawAcc=%.2lf horConfidence=%d"
             "rawHorConfidence=%d onDemandCpi=%d posSrcProvider=%d horzReliability= %d",
             __func__, __LINE__,
             injectPositionReq.latitude, injectPositionReq.longitude,
             injectPositionReq.horUncCircular, injectPositionReq.rawHorUncCircular,
             injectPositionReq.horConfidence, injectPositionReq.rawHorConfidence,
             injectPositionReq.onDemandCpi, injectPositionReq.positionSrcProvider,
             injectPositionReq.horReliability);

    LOC_SEND_SYNC_REQ(InjectPosition, INJECT_POSITION, injectPositionReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::injectWifiPosition(const WifiLocation &wifiInfo)
{
    qmiLocInjectWifiPositionReqMsgT_v02 injectWifiPositionReq;
    memset(&injectWifiPositionReq, 0, sizeof(injectWifiPositionReq));

    injectWifiPositionReq.wifiFixTime_valid = 1;
    injectWifiPositionReq.wifiFixTime.wifiPositionTime= 0xFFFFFFFF;

    injectWifiPositionReq.wifiFixPosition_valid = wifiInfo.positionValid;
    injectWifiPositionReq.wifiFixPosition.lat = wifiInfo.latitude;
    injectWifiPositionReq.wifiFixPosition.lon = wifiInfo.longitude;

    injectWifiPositionReq.wifiFixPosition.hepe =(uint16_t)wifiInfo.accuracy;
    if (injectWifiPositionReq.wifiFixPosition.hepe < 1000) {
        injectWifiPositionReq.wifiFixPosition.hepe = 1000;
    }

    injectWifiPositionReq.rawHepe_valid = 1;
    injectWifiPositionReq.rawHepe = (uint16_t)wifiInfo.accuracy;

    LOC_LOGD("%s:%d]: Hepe=%u rawHepe=%u", __func__, __LINE__,
             injectWifiPositionReq.wifiFixPosition.hepe, injectWifiPositionReq.rawHepe);

    injectWifiPositionReq.wifiFixPosition.numApsUsed =(uint8_t)wifiInfo.numApsUsed;

    if(wifiInfo.positionValid)
    {
        injectWifiPositionReq.wifiFixPosition.fixErrorCode =
            (qmiLocWifiFixErrorCodeEnumT_v02)wifiInfo.fixError;
    }
    else
    {
        injectWifiPositionReq.wifiFixPosition.fixErrorCode =
            eQMI_LOC_WIFI_FIX_ERROR_LOCATION_CANNOT_BE_DETERMINED_V02;
    }

    LOC_LOGV("%s:%d] wifiInfo.apInfoValid = %d,wifiInfo.numApsUsed = %d ...\n", __func__, __LINE__,
             wifiInfo.apInfoValid,wifiInfo.numApsUsed);

    if(wifiInfo.apInfoValid){
        injectWifiPositionReq.apInfo_valid = 1;
        injectWifiPositionReq.apInfo_len = wifiInfo.apInfo.apLen;
        injectWifiPositionReq.wifiApSsidInfo_valid = wifiInfo.apInfo.wifiApSsidValid;
        injectWifiPositionReq.wifiApSsidInfo_len = wifiInfo.apInfo.wifiApSsidInfoLen;
        LOC_LOGV("%s:%d] wifiInfo.apInfo.apLen = %d, ...\n", __func__, __LINE__,wifiInfo.apInfo.apLen);

        int k = 0;
        for(int i=0;i<MAX_REPORTED_APS;i++)
        {
            for(int j=0;j<MAC_ADDRESS_LENGTH;j++,k++)
                injectWifiPositionReq.apInfo[i].macAddr[j] = wifiInfo.apInfo.mac_address[k];

            injectWifiPositionReq.apInfo[i].rssi = wifiInfo.apInfo.rssi[i];
            injectWifiPositionReq.apInfo[i].channel = wifiInfo.apInfo.channel[i];
            injectWifiPositionReq.apInfo[i].apQualifier = 0;
            LOC_LOGV("%s:%d] mac address %d is  %X:%X:%X:%X:%X:%X rssi[%d] = %d and channel[%d] = %d ...\n"
                     , __func__, __LINE__,i,
                     injectWifiPositionReq.apInfo[i].macAddr[0],
                     injectWifiPositionReq.apInfo[i].macAddr[1],
                     injectWifiPositionReq.apInfo[i].macAddr[2],
                     injectWifiPositionReq.apInfo[i].macAddr[3],
                     injectWifiPositionReq.apInfo[i].macAddr[4],
                     injectWifiPositionReq.apInfo[i].macAddr[5],
                     i, injectWifiPositionReq.apInfo[i].rssi,
                     i, injectWifiPositionReq.apInfo[i].channel);

            for(int j=0;wifiInfo.apInfo.wifiApSsidInfo[i].ssid[j] != '\0';j++)
            {
                injectWifiPositionReq.wifiApSsidInfo[i].ssid[j]
                    = wifiInfo.apInfo.wifiApSsidInfo[i].ssid[j];
            }
            LOC_LOGV("%s:%d] ssid[%d] is %s", __func__, __LINE__, i,
                     wifiInfo.apInfo.wifiApSsidInfo[i].ssid);
        }
    }

    LOC_LOGV("%s:%d] injecting wifi position ...\n", __func__, __LINE__);
    LOC_SEND_SYNC_REQ(InjectWifiPosition, INJECT_WIFI_POSITION, injectWifiPositionReq);

    EXIT_LOG(%d, rv);
    return rv;
}

/* set the Wifi timeout value */
int LBSApiV02::setWifiWaitTimeoutValue(int timeout)
{

    qmiLocSetProtocolConfigParametersReqMsgT_v02 suplWifiConfigReq;
    memset(&suplWifiConfigReq, 0, sizeof(suplWifiConfigReq));

    LOC_LOGD("%s:%d]: WIFI timeout value = %d\n",  __func__, __LINE__, timeout);

    suplWifiConfigReq.wifiScanInjectTimeout_valid = 1;
    suplWifiConfigReq.wifiScanInjectTimeout = timeout;

    LOC_LOGV("%s:%d] injecting the wifi timeout value ...\n", __func__, __LINE__);
    LOC_SEND_SYNC_REQ(SetProtocolConfigParameters, SET_PROTOCOL_CONFIG_PARAMETERS,
            suplWifiConfigReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::injectWifiApInfo(const WifiApInfo &wifiApInfo)
{
    qmiLocInjectWifiApDataReqMsgT_v02 injectWifiApDataReq;
    memset(&injectWifiApDataReq, 0, sizeof(injectWifiApDataReq));

    if (WIFI_SCAN_RESPONSE_SUCCESS == wifiApInfo.wifiScanRespType) {
        injectWifiApDataReq.wifiApInfo_len = wifiApInfo.apLen;
        injectWifiApDataReq.wifiApInfoA_valid = 1;
        injectWifiApDataReq.wifiApInfoA_len = wifiApInfo.apLen;
        LOC_LOGV("%s:%d] injectWifiApDataReq.wifiApInfo_len = %d,"
            "injectWifiApDataReq.onDemandScan = %d.\n", __func__, __LINE__,
                 injectWifiApDataReq.wifiApInfo_len, injectWifiApDataReq.onDemandScan);

        int k = 0;
        for(uint32_t i=0;i<MAX_REPORTED_APS;i++)
        {
            for(int j=0;j<MAC_ADDRESS_LENGTH;j++,k++)
                injectWifiApDataReq.wifiApInfo[i].macAddress[j] = wifiApInfo.mac_address[k];

            injectWifiApDataReq.wifiApInfo[i].apRssi = wifiApInfo.rssi[i];
            injectWifiApDataReq.wifiApInfo[i].apChannel = (uint16_t)wifiApInfo.channel[i];
            injectWifiApDataReq.wifiApInfoA[i].apHighResolutionRssi =
                wifiApInfo.apHighResolutionRssi[i];
            injectWifiApDataReq.wifiApInfoA[i].rssiTimestamp = wifiApInfo.rssi_timestamp[i];
            injectWifiApDataReq.wifiApInfoA[i].channelFrequency = wifiApInfo.frequency[i];
            injectWifiApDataReq.wifiApInfoA[i].measAge = wifiApInfo.measurement_age[i];
            injectWifiApDataReq.wifiApInfoA[i].servingAccessPoint = wifiApInfo.servingAccessPoint[i];
            injectWifiApDataReq.wifiApInfo[i].wifiApDataMask =
                WIFI_APDATA_MASK_AP_RSSI | WIFI_APDATA_MASK_AP_CHANNEL |
                WIFI_APDATA_MASK_RSSI_TIMESTAMP | WIFI_APDATA_MASK_MEASUREMENT_AGE |
                WIFI_APDATA_MASK_SERVING_AP | WIFI_APDATA_MASK_FREQUENCY |
                WIFI_APDATA_MASK_SSID;

            for(int j=0;wifiApInfo.wifiApSsidInfo[i].ssid[j] != '\0';j++)
            {
                injectWifiApDataReq.wifiApInfoA[i].ssid[j]
                    = wifiApInfo.wifiApSsidInfo[i].ssid[j];
            }
            LOC_LOGV("%s:%d] ssid[%d] is %s", __func__, __LINE__, i,
                     wifiApInfo.wifiApSsidInfo[i].ssid);

            // update the timestamps if on-demand scan
            if (wifiApInfo.onDemandScan) {
                injectWifiApDataReq.requestTimestamp_valid = 1;
                injectWifiApDataReq.requestTimestamp = wifiApInfo.requestTimestamp;
                injectWifiApDataReq.receiveTimestamp_valid = 1;
                injectWifiApDataReq.receiveTimestamp = wifiApInfo.receiveTimestamp;
            }

            LOC_LOGV("%s:%d] mac address %d is  %X:%X:%X:%X:%X:%X rssi[%d] = %d\n"
                     " apHighResolutionRssi[%d] = %d channel[%d] = %d\n"
                     " rssiTimestamp[%d] = %" PRIu64 ", frequency[%d] = %d\n"
                     " measurementAge[%d] = %d servingAccessPoint[%d] = %d\n",
                     __func__, __LINE__,i,
                     injectWifiApDataReq.wifiApInfo[i].macAddress[0],
                     injectWifiApDataReq.wifiApInfo[i].macAddress[1],
                     injectWifiApDataReq.wifiApInfo[i].macAddress[2],
                     injectWifiApDataReq.wifiApInfo[i].macAddress[3],
                     injectWifiApDataReq.wifiApInfo[i].macAddress[4],
                     injectWifiApDataReq.wifiApInfo[i].macAddress[5],
                     i, injectWifiApDataReq.wifiApInfo[i].apRssi,
                     i, injectWifiApDataReq.wifiApInfoA[i].apHighResolutionRssi,
                     i, injectWifiApDataReq.wifiApInfo[i].apChannel,
                     i, injectWifiApDataReq.wifiApInfoA[i].rssiTimestamp,
                     i, injectWifiApDataReq.wifiApInfoA[i].channelFrequency,
                     i, injectWifiApDataReq.wifiApInfoA[i].measAge,
                     i, injectWifiApDataReq.wifiApInfoA[i].servingAccessPoint);
        }
    } else {

        injectWifiApDataReq.errorCause_valid = 1;
        switch (wifiApInfo.wifiScanRespType)
        {
            case WIFI_SCAN_RESPONSE_NO_MEASUREMENTS_AVAILABLE:
                injectWifiApDataReq.errorCause = eQMI_LOC_WLAN_AP_ERR_NO_REQ_MEAS_AVAILABLE_V02;
            break;
            case WIFI_SCAN_RESPONSE_NO_WIFI_AVAILABLE:
                injectWifiApDataReq.errorCause = eQMI_LOC_WLAN_AP_ERR_WIFI_OFF_V02;
            break;
            default:
                injectWifiApDataReq.errorCause = eQMI_LOC_WLAN_AP_ERR_UNKNOWN_V02;
            break;
        }
    }

    injectWifiApDataReq.onDemandScan_valid = 1;
    injectWifiApDataReq.onDemandScan = wifiApInfo.onDemandScan;

    LOC_LOGV("%s:%d] injecting wifi ap info ...\n", __func__, __LINE__);
    LOC_SEND_SYNC_REQ(InjectWifiApData, INJECT_WIFI_AP_DATA, injectWifiApDataReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::injectBtLeDevScanData(const BtDeviceInfo &btDevInfo)
{
    qmiLocInjectSrnApDataReqMsgT_v02 injectSrnApDataReq;
    memset(&injectSrnApDataReq, 0, sizeof(injectSrnApDataReq));

    if (btDevInfo.mValidDeviceData) {

        injectSrnApDataReq.apDeviceTechType = eQMI_LOC_SRN_AP_DATA_TECH_TYPE_BTLE_V02;
        injectSrnApDataReq.srnBtleApInfo_valid = true;
        injectSrnApDataReq.srnBtleApInfo_len = btDevInfo.btAp_len;
        injectSrnApDataReq.requestTimestamp_valid = true;
        injectSrnApDataReq.requestTimestamp = btDevInfo.mScanStartTimestamp;
        injectSrnApDataReq.receiveTimestamp_valid = true;
        injectSrnApDataReq.receiveTimestamp = btDevInfo.mScanReceiveTimestamp;
        int k = 0;
        int max_ap = (injectSrnApDataReq.srnBtleApInfo_len > SRN_MAX_REPORTED_APS ) ?
                                SRN_MAX_REPORTED_APS:injectSrnApDataReq.srnBtleApInfo_len;
        LOC_LOGV("%s:%d] BTLE num devices: %d, tech type: %d, max_ap:%d,"
                 "Rqst Time: %" PRId64 ", Recv time: %" PRId64 "\n",
                __FUNCTION__, __LINE__,
                injectSrnApDataReq.srnBtleApInfo_len,injectSrnApDataReq.apDeviceTechType,max_ap,
                injectSrnApDataReq.requestTimestamp,
                injectSrnApDataReq.receiveTimestamp);
        for(int i=0;i<max_ap;i++)
        {
            injectSrnApDataReq.srnBtleApInfo[i].apSrnRssi = btDevInfo.mApBtDevRssi[i];
            // Send time diff from scan start time (device_scan_rcv_time-scan_start_time)
            injectSrnApDataReq.srnBtleApInfo[i].apSrnTimestamp =
                         btDevInfo.mBtDevScanRequestTimestamp[i] - btDevInfo.mScanStartTimestamp;
            for(int j=0;j<SRN_MAC_ADDRESS_LENGTH;j++,k++) {
                injectSrnApDataReq.srnBtleApInfo[i].macAddress[j] = btDevInfo.mApBtMacAddress[k];
            }
            LOC_LOGV("%s:%d] mac address[%d]: %02X:%02X:%02X:%02X:%02X:%02X,"
                        " rssi[%d]: %d, rssiTime[%d]: %" PRId64 "\n",
                        __FUNCTION__, __LINE__,
                        i,injectSrnApDataReq.srnBtleApInfo[i].macAddress[0],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[1],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[2],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[3],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[4],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[5],
                        i, injectSrnApDataReq.srnBtleApInfo[i].apSrnRssi,
                        i, injectSrnApDataReq.srnBtleApInfo[i].apSrnTimestamp);
        }
    }
    else {
        injectSrnApDataReq.errorCause_valid = true;
        // Map to error cause
        switch (btDevInfo.mErrorCause)
        {
            case SRN_SCAN_RESPONSE_NO_MEASUREMENTS_AVAILABLE:
                injectSrnApDataReq.errorCause = eQMI_LOC_SRN_AP_ERR_NO_REQ_MEAS_AVAILABLE_V02;
            break;
            case SRN_SCAN_RESPONSE_SRN_ADAPTER_OFF:
                injectSrnApDataReq.errorCause = eQMI_LOC_SRN_AP_ERR_SENSOR_OFF_V02;
            break;
            default:
                injectSrnApDataReq.errorCause = eQMI_LOC_SRN_AP_ERR_UNKNOWN_V02;
            break;
        }

        LOC_LOGV("%s:%d] srn error %d error cause %d\n",__FUNCTION__, __LINE__,
                            btDevInfo.mErrorCause,injectSrnApDataReq.errorCause);

    }

    LOC_LOGV("%s:%d] injecting BTLE device scan details...\n", __FUNCTION__, __LINE__);
    LOC_SEND_SYNC_REQ(InjectSrnApData, INJECT_SRN_AP_DATA, injectSrnApDataReq);
    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::injectBtClassicDevScanData(const BtDeviceInfo &btDevInfo)
{
    qmiLocInjectSrnApDataReqMsgT_v02 injectSrnApDataReq;
    memset(&injectSrnApDataReq, 0, sizeof(injectSrnApDataReq));

    if (btDevInfo.mValidDeviceData) {

        injectSrnApDataReq.apDeviceTechType = eQMI_LOC_SRN_AP_DATA_TECH_TYPE_BT_V02;
        // Note: When BT is added, this should be change accordingly to srnBtApInfo*
        injectSrnApDataReq.srnBtleApInfo_valid = true;
        injectSrnApDataReq.srnBtleApInfo_len = btDevInfo.btAp_len;
        injectSrnApDataReq.requestTimestamp_valid = true;
        injectSrnApDataReq.requestTimestamp = btDevInfo.mScanStartTimestamp;
        injectSrnApDataReq.receiveTimestamp_valid = true;
        injectSrnApDataReq.receiveTimestamp = btDevInfo.mScanReceiveTimestamp;
        int k = 0;
        int max_ap = (injectSrnApDataReq.srnBtleApInfo_len > SRN_MAX_REPORTED_APS ) ?
                                SRN_MAX_REPORTED_APS:injectSrnApDataReq.srnBtleApInfo_len;
        LOC_LOGV("%s:%d] Bt classic num devices: %d, tech type: %d, max_ap:%d, "
                 "Rqst Time: %" PRId64 ", Recv time: %" PRId64 "\n",
                __FUNCTION__, __LINE__,
                injectSrnApDataReq.srnBtleApInfo_len,injectSrnApDataReq.apDeviceTechType,max_ap,
                injectSrnApDataReq.requestTimestamp,
                injectSrnApDataReq.receiveTimestamp);
        for(int i=0;i<max_ap;i++)
        {
            // Note: When BT is added, this should be change accordingly to srnBtApInfo*
            injectSrnApDataReq.srnBtleApInfo[i].apSrnRssi = btDevInfo.mApBtDevRssi[i];
            injectSrnApDataReq.srnBtleApInfo[i].apSrnTimestamp = btDevInfo.mBtDevScanRequestTimestamp[i];
            for(int j=0;j<SRN_MAC_ADDRESS_LENGTH;j++,k++) {
                injectSrnApDataReq.srnBtleApInfo[i].macAddress[j] = btDevInfo.mApBtMacAddress[k];
            }
            LOC_LOGV("%s:%d] mac address[%d]: %02X:%02X:%02X:%02X:%02X:%02X,"
                        " rssi[%d]: %d rssiTime[%d]: %" PRId64 "\n",
                        __FUNCTION__, __LINE__,
                        i,injectSrnApDataReq.srnBtleApInfo[i].macAddress[0],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[1],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[2],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[3],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[4],
                        injectSrnApDataReq.srnBtleApInfo[i].macAddress[5],
                        i, injectSrnApDataReq.srnBtleApInfo[i].apSrnRssi,
                        i, injectSrnApDataReq.srnBtleApInfo[i].apSrnTimestamp);
        }
    }
    else {
        injectSrnApDataReq.errorCause_valid = true;
        // Map to error cause
        switch (btDevInfo.mErrorCause)
        {
            case SRN_SCAN_RESPONSE_NO_MEASUREMENTS_AVAILABLE:
                injectSrnApDataReq.errorCause = eQMI_LOC_SRN_AP_ERR_NO_REQ_MEAS_AVAILABLE_V02;
            break;
            case SRN_SCAN_RESPONSE_SRN_ADAPTER_OFF:
                injectSrnApDataReq.errorCause = eQMI_LOC_SRN_AP_ERR_SENSOR_OFF_V02;
            break;
            default:
                injectSrnApDataReq.errorCause = eQMI_LOC_SRN_AP_ERR_UNKNOWN_V02;
            break;
        }

        LOC_LOGV("%s:%d] srn error %d error cause %d\n",__FUNCTION__, __LINE__,
                            btDevInfo.mErrorCause,injectSrnApDataReq.errorCause);
    }

    LOC_LOGV("%s:%d] injecting BT Classic device scan details...\n", __func__, __LINE__);
    LOC_SEND_SYNC_REQ(InjectSrnApData, INJECT_SRN_AP_DATA, injectSrnApDataReq);
    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::cinfoInject (int cid, int lac, int mnc, int mcc, bool roaming)
{
    qmiLocInjectGSMCellInfoReqMsgT_v02 cinfoReq;
    cinfoReq.gsmCellId.MCC = mcc;
    cinfoReq.gsmCellId.MNC = mnc;
    cinfoReq.gsmCellId.LAC = lac;
    cinfoReq.gsmCellId.CID = cid;
    cinfoReq.roamingStatus = roaming;

    LOC_SEND_SYNC_REQ(InjectGSMCellInfo, INJECT_GSM_CELL_INFO, cinfoReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::oosInform()
{
    void* oosReq = NULL;

    LOC_SEND_SYNC_REQ(WWANOutOfServiceNotification, WWAN_OUT_OF_SERVICE_NOTIFICATION, oosReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::niSuplInit(char* supl_init, int length)
{
    qmiLocInjectNetworkInitiatedMessageReqMsgT_v02 suplInitReq;
    memcpy(suplInitReq.injectedNIMessage, supl_init, length);
    suplInitReq.injectedNIMessage_len = length;
    suplInitReq.injectedNIMessageType = eQMI_LOC_INJECTED_NETWORK_INITIATED_MESSAGE_TYPE_SUPL_V02;

    LOC_SEND_SYNC_REQ(InjectNetworkInitiatedMessage, INJECT_NETWORK_INITIATED_MESSAGE, suplInitReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::chargerStatusInject(int status)
{
    qmiLocSetExternalPowerConfigReqMsgT_v02 chargerStatus;

    memset(&chargerStatus, 0, sizeof(chargerStatus));
    chargerStatus.externalPowerState = (qmiLocExternalPowerConfigEnumT_v02)status;

    LOC_SEND_SYNC_REQ(SetExternalPowerConfig, SET_EXTERNAL_POWER_CONFIG, chargerStatus);

    EXIT_LOG(%d, rv);
    return rv;
}

qmiLocReliabilityEnumT_v02 LBSApiV02::mapReliabilityValue(ReliabilityValue reliabilityValue)
{
    switch (reliabilityValue) {
    case RELIABILITY_NOT_SET:
        return eQMI_LOC_RELIABILITY_NOT_SET_V02;
    case RELIABILITY_VERY_LOW:
        return eQMI_LOC_RELIABILITY_VERY_LOW_V02;
    case RELIABILITY_LOW:
        return eQMI_LOC_RELIABILITY_LOW_V02;
    case RELIABILITY_MEDIUM:
        return eQMI_LOC_RELIABILITY_MEDIUM_V02;
    case RELIABILITY_HIGH:
        return eQMI_LOC_RELIABILITY_HIGH_V02;
    default:
        return eQMI_LOC_RELIABILITY_NOT_SET_V02;
    }
}

qmiLocPositionSrcEnumT_v02 LBSApiV02::mapSourceType(PositionSourceType sourceType)
{
    switch (sourceType) {
    case GNSS:
        return eQMI_LOC_POSITION_SRC_GNSS_V02;
    case CELLID:
        return eQMI_LOC_POSITION_SRC_CELLID_V02;
    case ENH_CELLID:
        return eQMI_LOC_POSITION_SRC_ENH_CELLID_V02;
    case WIFI:
        return eQMI_LOC_POSITION_SRC_WIFI_V02;
    case TERRESTRIAL:
        return eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02;
    case GNSS_TERRESTRIAL_HYBRID:
        return eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID_V02;
    case OTHER:
        return eQMI_LOC_POSITION_SRC_OTHER_V02;
    default:
        return eQMI_LOC_POSITION_SRC_OTHER_V02;

    }
}

qmiLocPositionSrcProviderEnumT_v02 LBSApiV02::mapSourceProviderType(PositionSourceProviderType sourceProviderType)
{
    switch (sourceProviderType) {
    case EXTERNAL:
        return eQMI_LOC_POSITION_SRC_PROVIDER_EXTERNAL_V02;
    case INTERNAL:
        return eQMI_LOC_POSITION_SRC_PROVIDER_INTERNAL_V02;
    default:
        return eQMI_LOC_POSITION_SRC_PROVIDER_EXTERNAL_V02;

    }
}

int LBSApiV02::shutdown()
{
    LOC_LOGD("%s:%d]: Enter", __func__, __LINE__);
    close();
    LOC_LOGD("%s:%d]: Exit", __func__, __LINE__);
    return true;
}

void LBSApiV02::injectFeatureConfig(e_premium_mode sap_mode,
                                    e_premium_mode gtp_cell_mode,
                                    e_premium_mode gtp_wifi_mode)
{
    qmiLocSetPremiumServicesCfgReqMsgT_v02 premiumCfgReq;

    memset(&premiumCfgReq, 0, sizeof(premiumCfgReq));
    //GTP-CELL
    premiumCfgReq.premiumServiceType = eQMI_LOC_PREMIUM_SERVICE_GTP_CELL_V02;
    if(gtp_cell_mode == BASIC) {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_ENABLED_BASIC_V02;
    }
    else if(gtp_cell_mode == PREMIUM) {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_ENABLED_PREMIUM_V02;
    }
    else {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_DISABLED_V02;
    }
    //Setting block boundries to prevent re-definition of variables in macro
    {
        LOC_SEND_SYNC_REQ(SetPremiumServicesCfg, SET_PREMIUM_SERVICES_CONFIG, premiumCfgReq);
    }


    //GTP-WIFI
    premiumCfgReq.premiumServiceType = eQMI_LOC_PREMIUM_SERVICE_GTP_WIFI_V02;
    if(BASIC == gtp_wifi_mode) {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_ENABLED_BASIC_V02;
    }
    else if(PREMIUM == gtp_wifi_mode) {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_ENABLED_PREMIUM_V02;
    }
    else {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_DISABLED_V02;
    }
    //Setting block boundries to prevent re-definition of variables in macro
    {
        LOC_SEND_SYNC_REQ(SetPremiumServicesCfg, SET_PREMIUM_SERVICES_CONFIG, premiumCfgReq);
    }

    //SAP
    memset(&premiumCfgReq, 0, sizeof(premiumCfgReq));
    premiumCfgReq.premiumServiceType = eQMI_LOC_PREMIUM_SERVICE_SAP_V02;
    if(sap_mode == BASIC) {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_ENABLED_BASIC_V02;
    }
    else if(sap_mode == PREMIUM) {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_ENABLED_PREMIUM_V02;
    }
    else {
        premiumCfgReq.premiumServiceCfg = eQMI_LOC_PREMIUM_SERVICE_DISABLED_V02;
    }
    {
        LOC_SEND_SYNC_REQ(SetPremiumServicesCfg, SET_PREMIUM_SERVICES_CONFIG, premiumCfgReq);
    }
}

int LBSApiV02::wifiAttachmentStatusInject(const WifiSupplicantInfo &wifiSupplicantInfo)
{
    qmiLocNotifyWifiAttachmentStatusReqMsgT_v02 wifiAttachmentStatusReq;
    memset(&wifiAttachmentStatusReq, 0, sizeof(wifiAttachmentStatusReq));

    wifiAttachmentStatusReq.attachState =
        (qmiLocWifiAccessPointAttachStatesEnumT_v02)wifiSupplicantInfo.attachState;

    wifiAttachmentStatusReq.accessPointMacAddress_valid =
        wifiSupplicantInfo.apMacAddressValid;

    if(wifiSupplicantInfo.apMacAddressValid) {
        for(int i=0;i<MAC_ADDRESS_LENGTH;i++) {
            wifiAttachmentStatusReq.accessPointMacAddress[i] =
                wifiSupplicantInfo.apMacAddress[i];
        }
    }

    wifiAttachmentStatusReq.wifiApSsid_valid =
        wifiSupplicantInfo.wifiApSsidValid;

    if(wifiSupplicantInfo.wifiApSsidValid) {

        for(int i=0;wifiSupplicantInfo.ssid[i] != '\0';i++) {
            wifiAttachmentStatusReq.wifiApSsid[i] =
                wifiSupplicantInfo.ssid[i];
        }

        LOC_LOGV("%s:%d] ssid valid is %d and ssid is %s", __func__, __LINE__
                 , wifiSupplicantInfo.wifiApSsidValid, wifiSupplicantInfo.ssid);
    }

    LOC_LOGV("%s:%d] informing wifi attachment with status %d...\n",
             __func__, __LINE__, wifiSupplicantInfo.attachState);

    LOC_SEND_SYNC_REQ(NotifyWifiAttachmentStatus, NOTIFY_WIFI_ATTACHMENT_STATUS,
            wifiAttachmentStatusReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::wifiEnabledStatusInject(int status)
{
    qmiLocNotifyWifiEnabledStatusReqMsgT_v02 wifiEnabledStatusReq;
    memset(&wifiEnabledStatusReq, 0, sizeof(wifiEnabledStatusReq));

    wifiEnabledStatusReq.enabledStatus = (qmiLocWifiEnabledStatusEnumT_v02)status;

    LOC_LOGV("%s:%d] informing wifi enabled with status %d...\n",
             __func__, __LINE__,status);
    LOC_SEND_SYNC_REQ(NotifyWifiEnabledStatus, NOTIFY_WIFI_ENABLED_STATUS, wifiEnabledStatusReq);

    EXIT_LOG(%d, rv);
    return rv;
}



int LBSApiV02::injectSensorData(const SensorDataBundle &dataBundle)
{
    ENTRY_LOG();
    qmiLocInjectSensorDataReqMsgT_v02 sensorDataReq;
    memset(&sensorDataReq, 0, sizeof(sensorDataReq));

    switch (dataBundle.sensorType)
    {
    case ACCEL:
    {
        sensorDataReq.threeAxisAccelData_valid = true;
        sensorDataReq.threeAxisAccelDataTimeSource_valid = 1;
        sensorDataReq.threeAxisAccelDataTimeSource =
            (qmiLocSensorDataTimeSourceEnumT_v02)dataBundle.timeSource;
        convertThreeAxisData(dataBundle, sensorDataReq.threeAxisAccelData);
    }
    break;
    case ACCEL_TEMPERATURE:
    {
        sensorDataReq.accelTemperatureData_valid = true;
        convertTemperatureData(dataBundle, sensorDataReq.accelTemperatureData);
    }
    break;
    case GYRO:
    {
        sensorDataReq.threeAxisGyroData_valid = true;
        sensorDataReq.threeAxisGyroDataTimeSource_valid = 1;
        sensorDataReq.threeAxisGyroDataTimeSource =
            (qmiLocSensorDataTimeSourceEnumT_v02)dataBundle.timeSource;
        convertThreeAxisData(dataBundle, sensorDataReq.threeAxisGyroData);
    }
    break;
    case GYRO_TEMPERATURE:
    {
        sensorDataReq.gyroTemperatureData_valid = true;
        convertTemperatureData(dataBundle, sensorDataReq.gyroTemperatureData);
    }
    break;
    case MAG_CALIB:
    case MAG_UNCALIB:
    {
        sensorDataReq.threeAxisMagData_valid = true;
        sensorDataReq.threeAxisMagDataTimeSource_valid = 1;
        sensorDataReq.threeAxisMagDataTimeSource =
            (qmiLocSensorDataTimeSourceEnumT_v02)dataBundle.timeSource;
        convertThreeAxisData(dataBundle, sensorDataReq.threeAxisMagData);
    }
    break;
    default:
        break;
    }

    sensorDataReq.opaqueIdentifier_valid = true;
    sensorDataReq.opaqueIdentifier = mOpaqueId;
    mOpaqueId++;

    LOC_LOGD("%s: Injecting Sensor Data to LocApi. opaque_id = %u",
             __func__, mOpaqueId);

    /* Send Sensor data to Modem */
    LOC_SEND_SYNC_REQ(InjectSensorData, INJECT_SENSOR_DATA, sensorDataReq);

    EXIT_LOG(%d, rv);
    return rv;
}

int LBSApiV02::injectPedometerData(const PedometerData &pedometerData)
{
    ENTRY_LOG();
    qmiLocPedometerReportReqMsgT_v02 pedometerDataReq;
    memset(&pedometerDataReq, 0, sizeof(pedometerDataReq));

    pedometerDataReq.timeSource = (qmiLocSensorDataTimeSourceEnumT_v02)pedometerData.timeSource;
    pedometerDataReq.timestamp = pedometerData.timestamp;
    pedometerDataReq.timeInterval = pedometerData.timeInterval;

    pedometerDataReq.stepCount = pedometerData.stepCount;

    pedometerDataReq.stepConfidence_valid = pedometerData.stepConfidenceValid;
    pedometerDataReq.stepConfidence = pedometerData.stepConfidence;

    pedometerDataReq.stepCountUncertainty_valid = pedometerData.stepCountUncertaintyValid;
    pedometerDataReq.stepCountUncertainty = pedometerData.stepCountUncertainty;

    pedometerDataReq.stepRate_valid = pedometerData.stepRateValid;
    pedometerDataReq.stepRate = pedometerData.stepRate;

    LOC_LOGD("%s: Injecting Pedometer Data to LocApi",
             __func__);

    /* Send Sensor data to Modem */
    LOC_SEND_SYNC_REQ(PedometerReport, PEDOMETER_REPORT, pedometerDataReq);

    return rv;
}

int LBSApiV02::injectMotionData(const MotionData &motionData)
{
    ENTRY_LOG();
    qmiLocInjectMotionDataReqMsgT_v02 motionDataReq;
    memset(&motionDataReq, 0, sizeof(motionDataReq));

    motionDataReq.motion_data.motion_state = (qmiLocMotionStateEnumT_v02)motionData.state;
    motionDataReq.motion_data.motion_mode = (qmiLocMotionModeEnumT_v02)motionData.mode;

    motionDataReq.motion_data.probability_of_state = motionData.probability;
    motionDataReq.motion_data.age = motionData.age;
    motionDataReq.motion_data.timeout = motionData.timeout;

    LOC_LOGD("%s: Injecting Motion Data to LocApi",
             __func__);

    /* Send Sensor data to Modem */
    LOC_SEND_SYNC_REQ(InjectMotionData, INJECT_MOTION_DATA, motionDataReq);

    return rv;
}

int LBSApiV02::injectTimeData(const TimeData &data)
{
    ENTRY_LOG();

    locClientStatusEnumType st;
    locClientReqUnionType reqUnion;
    qmiLocInjectTimeSyncDataIndMsgT_v02 ind;

    qmiLocInjectTimeSyncDataReqMsgT_v02 timeSyncDataReq;
    memset(&timeSyncDataReq, 0, sizeof(timeSyncDataReq));
    memset(&ind, 0, sizeof(ind));

    timeSyncDataReq.refCounter = data.clientTxTime;
    timeSyncDataReq.sensorProcRxTime = data.remoteProcRxTime;
    timeSyncDataReq.sensorProcTxTime = data.remoteProcTxTime;

    LOC_LOGD("%s: Sending TimeSyncData to modem refcounter-%d and provider time-%d",
             __FUNCTION__, data.clientTxTime, data.remoteProcRxTime);

    reqUnion.pInjectTimeSyncReq = &timeSyncDataReq;

    st = locSyncSendReq(QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02,
                        reqUnion,
                        LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                        QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02,
                        &ind);

    if (st != eLOC_CLIENT_SUCCESS ||
        eQMI_LOC_SUCCESS_V02 != ind.status) {
        LOC_LOGE ("%s:%d]: Error : st = %d, ind.status = %d",
                  __func__, __LINE__,  st, ind.status);
        return -1;
    }

    return 0;
}


int LBSApiV02::injectSPIStatus(const SPIStatus &status)
{
    qmiLocSetSpiStatusReqMsgT_v02 spiStatusReq;
    memset(&spiStatusReq, 0, sizeof(spiStatusReq));

    spiStatusReq.stationary = status.stationary;
    spiStatusReq.confidenceStationary_valid = status.confidenceStationaryValid;//always false
    //spiStatusReq.confidenceStationary = status.confidenceStationary;

    LOC_LOGD("%s: Sending SPI Data to LocApi",__func__);

    /* Send Sensor data to Modem */
    LOC_SEND_SYNC_REQ(SetSpiStatus, SET_SPI_STATUS, spiStatusReq);
    return true;

}

void LBSApiV02::doSensorDataRequest(
        const qmiLocSensorReadyStatusStructT_v02 &fromRequest,
        SensorType sensorType)
{
    SensorRequest toRequest;
    memset(&toRequest, 0, sizeof(toRequest));
    toRequest.requestType = sensorType;
    toRequest.requestValue = (SensorRequestType) fromRequest.injectEnable;
    toRequest.frequencyValid = true;
    toRequest.requestRate.samplesPerBatch =
            fromRequest.dataFrequency.samplesPerBatch;
    toRequest.requestRate.batchesPerSecond =
            fromRequest.dataFrequency.batchesPerSecond;
    toRequest.requestRate.samplingRate =
            toRequest.requestRate.samplesPerBatch * toRequest.requestRate.batchesPerSecond;

    LOC_LOGV("%s:%d] received request for sensor %d with enable %d...\n",
            __func__, __LINE__, toRequest.requestType, toRequest.requestValue);
    requestSensorData(toRequest);
}

void LBSApiV02::convertThreeAxisData(
        const SensorDataBundle &fromData,
        qmiLoc3AxisSensorSampleListStructT_v02 &toData)
{
    memset(&toData, 0, sizeof(toData));

    toData.timeOfFirstSample = fromData.timeOfFirstSample;
    toData.flags = fromData.flags;
    toData.sensorData_len = fromData.sensorDataLength;

    for (uint32_t i=0; i < fromData.sensorDataLength; i++)
    {
        LOC_LOGV("%s: Injecting data (%d) with time offset-%d, data-(%f,%f,%f)",
                __func__, fromData.sensorType, fromData.samples[i].timeOffset,
                fromData.samples[i].xAxis, fromData.samples[i].yAxis,
                fromData.samples[i].zAxis);
        toData.sensorData[i].timeOffset = fromData.samples[i].timeOffset;
        toData.sensorData[i].xAxis = fromData.samples[i].xAxis;
        toData.sensorData[i].yAxis = fromData.samples[i].yAxis;
        toData.sensorData[i].zAxis = fromData.samples[i].zAxis;
    }
}

void LBSApiV02::convertTemperatureData(
        const SensorDataBundle &fromData,
        qmiLocSensorTemperatureSampleListStructT_v02 &toData)
{
    memset(&toData, 0, sizeof(toData));

    toData.timeOfFirstSample = fromData.timeOfFirstSample;
    toData.temperatureData_len = fromData.sensorDataLength;
    toData.timeSource = (qmiLocSensorDataTimeSourceEnumT_v02)fromData.timeSource;

    for (uint32_t i=0; i < fromData.sensorDataLength; i++)
    {
        LOC_LOGV("%s: Injecting data (%d) with time offset-%d, data-(%f)",
                __func__, fromData.sensorType, fromData.samples[i].timeOffset,
                fromData.samples[i].xAxis);
        toData.temperatureData[i].timeOffset = fromData.samples[i].timeOffset;
        toData.temperatureData[i].temperature = fromData.samples[i].xAxis;
    }
}


void LBSApiV02 :: timeInfoInject(long curTimeMillis, int rawOffset, int dstOffset) {
    LOC_LOGD("%s:%d]: curTime: %ld, rawOffset: %d, dstOffset: %d",
             __func__, __LINE__, curTimeMillis, rawOffset, dstOffset);

    qmiLocInjectTimeZoneInfoReqMsgT_v02 timeZoneInfoReq;
    timeZoneInfoReq.timeUtc = curTimeMillis;
    timeZoneInfoReq.timeZone.rawOffset = rawOffset;
    timeZoneInfoReq.timeZone.dstOffset = dstOffset;

    LOC_SEND_SYNC_REQ(InjectTimeZoneInfo, INJECT_TIME_ZONE_INFO, timeZoneInfoReq);
}

int LBSApiV02::batteryLevelInject(int batteryPct)
{
    qmiLocSetExternalPowerConfigReqMsgT_v02 batteryStatus;
    int i = 0;
    /* Keeps track of the last battery level sent to QMI */
    static qmiLocBatteryLevelEnumT_v02 lastBatteryLevel =
        eQMI_LOC_BATTERY_LEVEL_UNKNOWN_V02;

    if (batteryPct > 100)
    {
        return -1;
    }

    memset(&batteryStatus, 0, sizeof(batteryStatus));
    batteryStatus.externalPowerState = eQMI_LOC_EXTERNAL_POWER_UNKNOWN_V02;

    if ((batteryPct >= 0 && batteryPct <= 25)&&
        (lastBatteryLevel != eQMI_LOC_BATTERY_LEVEL_LOW_V02))
    {
        /* Battery level is LOW and the notification for this level has not been sent */
        batteryStatus.batteryLevel_valid = true;
        batteryStatus.batteryLevel = eQMI_LOC_BATTERY_LEVEL_LOW_V02;
    }
    else if ((batteryPct > 25 && batteryPct <= 50)&&
        (lastBatteryLevel != eQMI_LOC_BATTERY_LEVEL_MEDIUM_V02))
    {
        /* Battery level is MEDIUM and the notification for this level has not been sent */
        batteryStatus.batteryLevel_valid = true;
        batteryStatus.batteryLevel = eQMI_LOC_BATTERY_LEVEL_MEDIUM_V02;
    }
    else if ((batteryPct > 50 && batteryPct <= 75)&&
        (lastBatteryLevel != eQMI_LOC_BATTERY_LEVEL_HIGH_V02))
    {
        /* Battery level is HIGH and the notification for this level has not been sent */
        batteryStatus.batteryLevel_valid = true;
        batteryStatus.batteryLevel = eQMI_LOC_BATTERY_LEVEL_HIGH_V02;
    }
    else if ((batteryPct > 75 && batteryPct <= 100)&&
        (lastBatteryLevel != eQMI_LOC_BATTERY_LEVEL_VERY_HIGH_V02))
    {
        /* Battery level is VERY HIGH and the notification for this level has not been sent */
        batteryStatus.batteryLevel_valid = true;
        batteryStatus.batteryLevel = eQMI_LOC_BATTERY_LEVEL_VERY_HIGH_V02;
    }
    else
    {
        /* Battery level is UNKNOWN and the notification for this level need not be sent */
        batteryStatus.batteryLevel_valid = false;
    }

    if ((true == batteryStatus.batteryLevel_valid)&&
        (batteryStatus.batteryLevel <= eQMI_LOC_BATTERY_LEVEL_LOW_V02))
    {
        /* Send the notification about the change in battery level */
        LOC_LOGD("Sending battery level %d to QMI LOC client \n",
            batteryStatus.batteryLevel);
        /* Note the last battery level which was sent to QMI */
        lastBatteryLevel = batteryStatus.batteryLevel;
        LOC_SEND_SYNC_REQ(SetExternalPowerConfig, SET_EXTERNAL_POWER_CONFIG, batteryStatus);
    }
    else
    {
        LOC_LOGD("Not sending battery level notification to QMI LOC client !...\n");
    }
    return 0;
}

bool LBSApiV02::injectFdclData(const FdclData& fdclData)
{
    qmiLocInjectFdclDataReqMsgT_v02 injectFdclDataReq;
    memset(&injectFdclDataReq, 0, sizeof(qmiLocInjectFdclDataReqMsgT_v02));

    injectFdclDataReq.daysValid = fdclData.daysValid;

    if (fdclData.bsLocationDataList.size() > QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02) {
        injectFdclDataReq.cellPosList_len = QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02;
    } else {
        injectFdclDataReq.cellPosList_len = fdclData.bsLocationDataList.size();
    }

    for (uint32_t i = 0; i < injectFdclDataReq.cellPosList_len; ++i) {
        qmiLocFdclCellPosStructT_v02& qmiData = injectFdclDataReq.cellPosList[i];
        const BSLocationData_s& data = fdclData.bsLocationDataList[i];
        switch (data.cellType){
            case 0:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_CDMA_V02;
                break;
            case 1:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_GSM_V02;
                break;
            case 2:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_WCDMA_V02;
                break;
            case 3:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_V02;
                break;
            case 4:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_M1_V02;
                break;
            case 5:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_NB1_V02;
                break;
            case 6:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_M1_MODE_A_V02;
                break;
            case 7:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_M1_MODE_B_V02;
                break;
            default:
                qmiData.cellId.airInterfaceType = eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_UNKNOWN_V02;
                break;
        }
        qmiData.cellId.id1 = data.cellRegionID1;
        qmiData.cellId.id2 = data.cellRegionID2;
        qmiData.cellId.id3 = data.cellRegionID3;
        qmiData.cellId.id4 = data.cellRegionID4;
        qmiData.latitude = data.latitude;
        qmiData.longitude = data.longitude;
        qmiData.horCoverageRadius = data.horizontal_coverage_radius;
        qmiData.horConfidence = data.horizontal_confidence;
        switch (data.horizontal_reliability){
            case 0:
                qmiData.horReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
                break;
            case 1:
                qmiData.horReliability = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
                break;
            case 2:
                qmiData.horReliability = eQMI_LOC_RELIABILITY_LOW_V02;
                break;
            case 3:
                qmiData.horReliability = eQMI_LOC_RELIABILITY_MEDIUM_V02;
                break;
            case 4:
                qmiData.horReliability = eQMI_LOC_RELIABILITY_HIGH_V02;
                break;
            default:
                qmiData.horReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
                break;
        }
        qmiData.validMask |= QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02;
        qmiData.altitude = data.altitude;
        qmiData.validMask |= QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_UNC_V02;
        qmiData.altUnc = data.altitude_uncertainty;
        qmiData.validMask |= QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE_V02;
        qmiData.altConfidence = data.altitude_confidence;
        qmiData.validMask |= QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02;
        switch (data.altitude_reliability){
            case 0:
                qmiData.altReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
                break;
            case 1:
                qmiData.altReliability = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
                break;
            case 2:
                qmiData.altReliability = eQMI_LOC_RELIABILITY_LOW_V02;
                break;
            case 3:
                qmiData.altReliability = eQMI_LOC_RELIABILITY_MEDIUM_V02;
                break;
            case 4:
                qmiData.altReliability = eQMI_LOC_RELIABILITY_HIGH_V02;
                break;
            default:
                qmiData.altReliability = eQMI_LOC_RELIABILITY_NOT_SET_V02;
                break;
        }
    }

    struct timespec time_info_current;
    clock_gettime(CLOCK_REALTIME,&time_info_current);
    injectFdclDataReq.timestampUtc_valid = 1;
    injectFdclDataReq.timestampUtc = (time_info_current.tv_sec)*1e3 +
                                     (time_info_current.tv_nsec)/1e6;

    LOC_SEND_SYNC_REQ(InjectFdclData, INJECT_FDCL_DATA, injectFdclDataReq);
    if (rv) {
        reportFdclStatusEvent(true, "", strlen(""));
    } else {
        if (st == eLOC_CLIENT_FAILURE_UNSUPPORTED || ind.status == eQMI_LOC_UNSUPPORTED_V02) {
            reportFdclStatusEvent(false, "UNSUPPORTED", strlen("UNSUPPORTED"));
        } else {
            reportFdclStatusEvent(false, "GENERAL", strlen("GENERAL"));
        }
    }

    EXIT_LOG(%d, rv);
    return rv;
}

bool LBSApiV02::requestFdclData(int32_t expireInDays)
{
    qmiLocGetFdclBsListReqMsgT_v02 getFdclBsListReq;
    memset(&getFdclBsListReq, 0, sizeof(qmiLocGetFdclBsListReqMsgT_v02));

    if (expireInDays >= 0) {
        getFdclBsListReq.expireInDays_valid = 1;
        getFdclBsListReq.expireInDays = (uint32_t)expireInDays;
    }

    struct timespec time_info_current;
    clock_gettime(CLOCK_REALTIME,&time_info_current);
    getFdclBsListReq.timestampUtc_valid = 1;
    getFdclBsListReq.timestampUtc = (time_info_current.tv_sec)*1e3 +
                                    (time_info_current.tv_nsec)/1e6;

    LOC_SEND_SYNC_REQ(GetFdclBsList, GET_FDCL_BS_LIST, getFdclBsListReq);

    if (st != eLOC_CLIENT_SUCCESS || eQMI_LOC_SUCCESS_V02 != ind.status) {
        if (st == eLOC_CLIENT_FAILURE_UNSUPPORTED || ind.status == eQMI_LOC_UNSUPPORTED_V02) {
            reportFdclStatusEvent(false, "UNSUPPORTED", strlen("UNSUPPORTED"));
        } else {
            reportFdclStatusEvent(false, "GENERAL", strlen("GENERAL"));
        }
        return false;
    }
    reportFdclStatusEvent(true, "", strlen(""));
    FdclInfo fdclInfo = {};
    for (uint32_t i = 0; i < ind.BsList_len; ++i) {
        BSInfo_s info = {};
        info.cell_type = (uint8_t)ind.BsList[i].airInterfaceType;
        info.cell_id1 = ind.BsList[i].id1;
        info.cell_id2 = ind.BsList[i].id2;
        info.cell_id3 = ind.BsList[i].id3;
        info.cell_id4 = ind.BsList[i].id4;
        fdclInfo.bsInfoList.push_back(info);
    }
    reportFdclInfoEvent(fdclInfo);

    return true;
}

}; //namespace lbs_core


