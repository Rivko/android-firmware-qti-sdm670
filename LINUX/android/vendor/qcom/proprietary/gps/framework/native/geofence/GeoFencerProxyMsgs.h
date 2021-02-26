/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       GeoFence service module

GENERAL DESCRIPTION
  GeoFence service module

Copyright (c) 2012,2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
=============================================================================*/
#ifndef GEOFENCEPROXY_MSGS_H
#define GEOFENCEPROXY_MSGS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "gps_extended_c.h"
#include "log_util.h"

#ifndef DEBUG_X86
extern "C" const LocGpsGeofencingInterface* loc_gps_geofence_get_interface_ext ();
#else
const LocGpsGeofencingInterface* loc_gps_geofence_get_interface_ext ();
#endif // DEBUG_X86

#ifdef __cplusplus
}
#endif

#endif /* GEOFENCEPROXY_MSGS_H */
