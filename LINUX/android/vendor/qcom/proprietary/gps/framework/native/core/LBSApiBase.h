/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2014, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LBS_API_BASE_H
#define LBS_API_BASE_H

#include <WiperData.h>
#include <SlimSensor.h>
#include <LocApiBase.h>
#include "wwan_db_receiver.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_ADAPTERS 10

using namespace loc_core;
using namespace izat_manager;

namespace lbs_core {

typedef enum {
    DISABLED,
    BASIC,
    PREMIUM
}e_premium_mode;

class LBSAdapterBase;

class LBSApiBase {
    LBSAdapterBase *mLBSAdapters[MAX_ADAPTERS];
protected:
    LocApiProxyBase* mLocApiProxy;
public:
    LBSApiBase(LocApiProxyBase* locApiProxy);
    inline virtual ~LBSApiBase() {}

    void addAdapter(LBSAdapterBase *adapter);
    void removeAdapter(LBSAdapterBase *adapter);

    void requestWps(const OdcpiRequest &request);
    void requestWifiApData(const WifiApDataRequest &request);
    void requestTimeZoneInfo();
    void requestSrnData(const GnssSrnDataReq &gnssSrnDataReq);
    void requestFdclDataEvent();
    void reportFdclStatusEvent(bool success, const char* errorString, uint32_t errorLength);
    void reportFdclInfoEvent(FdclInfo& fdclInfo);

    virtual int cinfoInject(int cid, int lac, int mnc,
                            int mcc, bool roaming);
    virtual int oosInform();
    virtual int niSuplInit(char* supl_init, int length);
    virtual int chargerStatusInject(int status);
    virtual int wifiStatusInform();
    virtual int wifiAttachmentStatusInject(
        const WifiSupplicantInfo &wifiSupplicantInfo);
    virtual int wifiEnabledStatusInject(int status);
    virtual int injectCoarsePosition(const CoarsePositionInfo &cpInfo);
    virtual int injectWifiPosition(const WifiLocation &wifiInfo);
    virtual int injectWifiApInfo(const WifiApInfo &wifiApInfo);
    virtual int injectBtClassicDevScanData(const BtDeviceInfo &btDevInfo);
    virtual int injectBtLeDevScanData(const BtDeviceInfo &btDevInfo);
    virtual int setWifiWaitTimeoutValue(int timeout);
    virtual int shutdown();
    virtual void injectFeatureConfig(e_premium_mode sap_mode,
                                     e_premium_mode gtp_cell_mode,
                                     e_premium_mode gtp_wifi_mode);
    virtual void requestSensorData(SensorRequest &request);
    virtual void requestPedometerData(PedometerRequest &request);
    virtual void requestMotionData(SensorRequest &request);
    virtual void requestTimeData(TimeRequest &timeRequest);
    virtual void requestSPIStatus(SensorRequest &request);
    virtual int injectSensorData(const SensorDataBundle &dataBundle);
    virtual int injectMotionData(const MotionData &motionData);
    virtual int injectPedometerData(const PedometerData &pedometerData);
    virtual int injectTimeData(const TimeData &data);
    virtual int injectSPIStatus(const SPIStatus &status);
    virtual void timeInfoInject(long curTimeMillis, int rawOffset,
                               int dstOffset);
    virtual int batteryLevelInject(int batteryPct);
    virtual bool injectFdclData(const FdclData& fdclData);
    virtual bool requestFdclData(int32_t expireInDays);
};
}; //namespace lbs_core
#ifdef __cplusplus
}
#endif
#endif //LBS_API_BASE_H
