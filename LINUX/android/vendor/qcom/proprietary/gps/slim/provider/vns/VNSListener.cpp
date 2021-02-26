/**
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/


#include <android/hardware/automotive/vehicle/2.0/types.h>
#include "VNSListener.h"
#include "VNSProvider.h"

using namespace slim;

using namespace android::hardware::automotive::vehicle::V2_0;
using ::android::hardware::automotive::vehicle::V2_0::VehicleProperty;
using android::hardware::hidl_vec;
using ::android::hardware::Void;

::android::hardware::Return<void> VNSListener::onPropertyEvent
         (const ::android::hardware::hidl_vec<VehiclePropValue>& propValues)

{

   int32_t rxProprtyCount = propValues.size();
   for(int i = 0; i < rxProprtyCount; i++)
   {
       SLIM_LOGD("%s:Vehicle Property Value:%d",__func__,propValues[i].prop);

       switch((propValues[i].prop))
       {
           case ((int32)VehicleProperty::PERF_VEHICLE_SPEED):
           {
               SLIM_LOGD("New Speed event received. Val:%f , Time:%u ",
                   propValues[i].value.floatValues[0],
                   (unsigned int)(propValues[i].timestamp/1000000));
               vns_processSpeed(propValues[i].value.floatValues[0],
                   propValues[i].timestamp);
           }
           break;

           case ((int32)VehicleProperty::CURRENT_GEAR):
           {
               SLIM_LOGD("New Gear event received. Val:%d, Time:%d",
                   propValues[i].value.int32Values[0],
                   (unsigned int)(propValues[i].timestamp/1000000));
               vns_processGear(propValues[i].value.int32Values[0],
                   propValues[i].timestamp);
           }
           break;
           default:
               SLIM_LOGD("%s:UnHandle Vehicle Property Value:%d",
                              __func__,propValues[i].prop);
       }

   }
   return Void();
}

::android::hardware::Return<void> VNSListener::onPropertySet
                   (const VehiclePropValue & propValue)
{
    SLIM_LOGD("%s:Vehicle Property Value:%d",__func__,propValue.prop);
    return Void();
}

::android::hardware::Return<void> VNSListener::onPropertySetError
                   (StatusCode errorCode, int32_t propId,int32_t areaId)
{
    SLIM_LOGD("%s:Error Code:%d,Property Code:%d,Area Code:%d",
                   __func__,errorCode,propId,areaId);
    return Void();
}


bool VNSListener::initializeVehicleIntrface()
{

  /* Register for new event handlers with Subscribe method */
   ::android::sp<::android::hardware::automotive::vehicle::V2_0::IVehicle> mVehicleIntf = ::android::hardware::automotive::vehicle::V2_0::IVehicle::getService();

   if(mVehicleIntf == NULL)
   {
       SLIM_LOGD("Vehicle Interface is not present ...");
       return false;
   }

   mVehicleIntf->registerAsService();
   SLIM_LOGD("Register as defualt Service...");

   hidl_vec<::android::hardware::automotive::vehicle::V2_0::SubscribeOptions> vhclePropVec;

   vhclePropVec.resize(VEHICLE_PROPRT_SUBSCRIBE);
   for( int i =0; i < VEHICLE_PROPRT_SUBSCRIBE; i++)
   {
       vhclePropVec[i] = reqVehicleProperties[i];
   }

   mVehicleIntf->subscribe(this,vhclePropVec);
   SLIM_LOGD("subscribe Vehicle Property...");

   return true;
}
