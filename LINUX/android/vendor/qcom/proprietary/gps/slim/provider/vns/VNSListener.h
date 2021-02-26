/**
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef __VEHICLE_NETWORK_SERVICES_LISTENER_H_INCLUDED__
#define __VEHICLE_NETWORK_SERVICES_LISTENER_H_INCLUDED__

#include <functional>
#include <iostream>

#include <hidl/HidlTransportSupport.h>
#include <android/hardware/automotive/vehicle/2.0/IVehicleCallback.h>
#include <android/hardware/automotive/vehicle/2.0/IVehicle.h>
#include <android/hardware/automotive/vehicle/2.0/types.h>

#include <vhal_v2_0/VehicleUtils.h>
#include "VNSUtils.h"

#define VEHICLE_PROPRT_SUBSCRIBE  2

using namespace android;
using namespace android::hardware::automotive::vehicle::V2_0;
namespace slim
{

    class VNSListener : public ::android::hardware::automotive::vehicle::V2_0::IVehicleCallback
    {
     public:
        VNSListener() {};
        virtual ~VNSListener() {};
        virtual ::android::hardware::Return<void> onPropertyEvent(const ::android::hardware::hidl_vec<VehiclePropValue>& propValues);
        virtual ::android::hardware::Return<void> onPropertySet(const VehiclePropValue& propValue);
        virtual ::android::hardware::Return<void> onPropertySetError(StatusCode errorCode, int32_t propId, int32_t areaId);
        bool initializeVehicleIntrface();
      private:
        Mutex mLock;
    };

}


const ::android::hardware::automotive::vehicle::V2_0::SubscribeOptions reqVehicleProperties[] = {

   {
       .propId = toInt(::android::hardware::automotive::vehicle::V2_0::VehicleProperty::PERF_VEHICLE_SPEED),
       .vehicleAreas = 0,
       .sampleRate = 0.0f, /* in Hz */
       .flags = SubscribeFlags::DEFAULT,
   },

   {
       .propId = toInt(VehicleProperty::CURRENT_GEAR),
       .vehicleAreas = 0,
       .sampleRate = 0.0f, /* in Hz */
       .flags = SubscribeFlags::DEFAULT
   }
};

#endif /*  __VEHICLE_NETWORK_SERVICES_LISTENER_H_INCLUDED__ */
