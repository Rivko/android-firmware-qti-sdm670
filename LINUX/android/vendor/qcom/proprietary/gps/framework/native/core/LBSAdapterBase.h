/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef LBS_ADAPTER_BASE_H
#define LBS_ADAPTER_BASE_H

#include <LocAdapterBase.h>
#include <LBSApiBase.h>

#ifdef __cplusplus
extern "C"
{
#endif

using namespace loc_core;
using namespace lbs_core;

namespace lbs_core {

class LBSAdapterBase : public LocAdapterBase {
protected:
    LBSApiBase *mLBSApi;
    LBSAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                   ContextBase* context);
    virtual ~LBSAdapterBase();
public:
virtual bool requestWps(const OdcpiRequest &request);
virtual bool requestWifiApData(const WifiApDataRequest &request);
virtual bool requestSrnData(const GnssSrnDataReq &gnssSrnDataReq);

virtual bool requestSensorData(const SensorRequest &request);
virtual bool requestPedometerData(const PedometerRequest &request);
virtual bool requestMotionData(const SensorRequest &request);
virtual bool requestTimeData(const TimeRequest &timeRequest);
virtual bool requestSPIStatus(const SensorRequest &request);
virtual bool requestTimeZoneInfo();
virtual bool requestFdclDataEvent();
virtual bool reportFdclStatusEvent(bool success, const char* errorString, uint32_t errorLength);
virtual bool reportFdclInfoEvent(FdclInfo& fdclInfo);
};
};

#ifdef __cplusplus
}
#endif

#endif // LBS_ADAPTER_BASE_H
