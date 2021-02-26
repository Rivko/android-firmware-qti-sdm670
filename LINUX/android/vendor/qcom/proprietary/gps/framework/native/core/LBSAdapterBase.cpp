/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013, 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_LBSAdapterBase"

#include <LBSAdapterBase.h>
#include <loc_pla.h>

using namespace loc_core;

namespace lbs_core {

LBSAdapterBase :: LBSAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                                 ContextBase *context) :
        LocAdapterBase(mask, context)
{
    // Dummy LocApi, APQ scenario, no WGR on target
    if( context->getLocApi() == NULL ||
            context->getLocApi()->getSibling() == NULL){
        mLBSApi = new LBSApiBase(new LocApiProxyBase());
    }
    else{
        mLBSApi = (LBSApiBase *)context->getLocApi()->getSibling();
    }

    mLBSApi->addAdapter(this);
    LOC_LOGD("%s:%d]: LBSAdapterBase created: %p\n", __func__, __LINE__, this);
}

LBSAdapterBase :: ~LBSAdapterBase()
{
    mLBSApi->removeAdapter(this);
}

bool LBSAdapterBase::requestWps(const OdcpiRequest& /*request*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestWifiApData(const WifiApDataRequest& /*request*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestSrnData(const GnssSrnDataReq& /*gnssSrnDataReq*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestSensorData(const SensorRequest& /*request*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestPedometerData(const PedometerRequest& /*request*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestMotionData(const SensorRequest& /*request*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestTimeData(const TimeRequest& /*timeRequest*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestSPIStatus(const SensorRequest& /*request*/)
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestTimeZoneInfo()
DEFAULT_IMPL(false)

bool LBSAdapterBase::requestFdclDataEvent()
DEFAULT_IMPL(false)

bool LBSAdapterBase::reportFdclStatusEvent(
    bool success, const char* errorString, uint32_t errorLength)
DEFAULT_IMPL(false)

bool LBSAdapterBase::reportFdclInfoEvent(FdclInfo& fdclInfo)
DEFAULT_IMPL(false)

};
