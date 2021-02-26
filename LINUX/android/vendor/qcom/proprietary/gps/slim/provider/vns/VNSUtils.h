/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#ifndef __VEHICLE_NETWORK_SERVICES_UTILS_H_INCLUDED__
#define __VEHICLE_NETWORK_SERVICES_UTILS_H_INCLUDED__


#define VEHICLE_PROPERTY_PERF_VEHICLE_SPEED    (0x00000207)
#define VEHICLE_PROPERTY_CURRENT_GEAR          (0x00000401)

   typedef struct VNS_Event_Struct_t
   {
        int propertyType;
        int64_t timeStamp;
        int gear;
        float speed;
   }VNS_Event_Struct;

void vns_processSpeed(float speed, int64_t timestamp);
void vns_processGear(int gear,int64_t timestamp);

#endif /* __VEHICLE_NETWORK_SERVICES_UTILS_H_INCLUDED__ */
