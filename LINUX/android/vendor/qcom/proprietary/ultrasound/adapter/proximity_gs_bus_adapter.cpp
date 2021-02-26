/*===========================================================================
                           proximity_gs_bus_adapter.cpp

DESCRIPTION: Implements a gesture bus adapter for proximity daemon

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "proximity_gs_bus_adapter.h"
#include "usf_log.h"
#include <ProximityExports.h>
/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
#define NAME "usf_proximity"

/*-----------------------------------------------------------------------------
  ProximityGsBusAdapter Function Implementation
-----------------------------------------------------------------------------*/
/*============================================================================
  ProximityGsBusAdapter CONSTRUCTOR
============================================================================*/
ProximityGsBusAdapter::ProximityGsBusAdapter(char *gesture_fw_lib_path)
:GsBusAdapter((char *)NAME,
              sizeof(NAME),
              kGsModeUltrasoundProximity,
              gesture_fw_lib_path)
{
}

/*============================================================================
  FUNCTION:  ProximityGsBusAdapter::get_event_mapping
============================================================================*/
event_mapping_t ProximityGsBusAdapter::get_event_mapping()
{
  return NOT_MAPPED;
}

/*============================================================================
  FUNCTION:  ProximityGsBusAdapter::map_event
============================================================================*/
GsOutcome ProximityGsBusAdapter::map_event(int event)
{
  GsOutcome outcome;
  switch (event)
  {
  case QC_US_PROXIMITY_LIB_RESULT_IDLE:
    outcome.type_ = GsOutcome::kTypeProx;
    outcome.subtype_ = GsOutcome::kSubtypeProxIdle;
    break;
  case QC_US_PROXIMITY_LIB_RESULT_DETECTED:
    outcome.type_ = GsOutcome::kTypeProx;
    outcome.subtype_ = GsOutcome::kSubtypeProxDetect;
    break;
  default:
    // Gesture is not mapped
    outcome.subtype_ = GsOutcome::kSubtypeNone;
  }
  return outcome;
}

/*============================================================================
  FUNCTION:  get_adapter
============================================================================*/
/**
 * This is the implementation of the function found in gs_bus_adapter.h
 */
FrameworkAdapter *get_adapter(char *gesture_fw_lib_path)
{
  LOGD("%s: Proximity gs bus adapter instance created",
       __FUNCTION__);
  static ProximityGsBusAdapter adapter(gesture_fw_lib_path);
  return &adapter;
}

