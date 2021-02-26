/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

/**
 @file
 @brief Vehicle Network Service provider implementation file.

 This file defines VNS provider object implementation.
 */

#include <slim_os_log_api.h>
#include <slim_os_api.h>

#include "VNSProvider.h"

using namespace slim;

void vns_processSpeed(float speed, int64_t timestamp)
{

   VNS_Event_Struct *pzNewEvent = new VNS_Event_Struct;
   memset(pzNewEvent , 0, sizeof(VNS_Event_Struct));
   pzNewEvent->propertyType = VEHICLE_PROPERTY_PERF_VEHICLE_SPEED;
   pzNewEvent->speed        = speed;
   pzNewEvent->timeStamp    = timestamp/1000000;
   /* send message to event processing loop */
   if (!slim_IpcSendData(THREAD_ID_VNS, eIPC_MSG_NEW_VNS_EVENT, &pzNewEvent,
               sizeof(pzNewEvent)))
   {
       SLIM_LOGE("Error sending IPC message to event processing loop");
   }

}


void vns_processGear(int gear,int64_t timestamp)
{

   VNS_Event_Struct *pzNewEvent = new VNS_Event_Struct;
   memset(pzNewEvent , 0, sizeof(VNS_Event_Struct));
   pzNewEvent->propertyType = VEHICLE_PROPERTY_CURRENT_GEAR;
   pzNewEvent->gear         = gear;
   pzNewEvent->timeStamp    = timestamp/1000000;
   /* send message to event processing loop */
   if (!slim_IpcSendData(THREAD_ID_VNS, eIPC_MSG_NEW_VNS_EVENT, &pzNewEvent,
               sizeof(pzNewEvent)))
   {
       SLIM_LOGE("Error sending gear IPC message to event processing loop");
   }

}
