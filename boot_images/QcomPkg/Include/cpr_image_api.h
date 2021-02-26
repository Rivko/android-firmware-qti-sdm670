// cpr.h - interface description of cpr driver
//
// Copyright 2013 by Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidental and Proprietary.
//
// $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/cpr_image_api.h#1 $
// $Author: pwbldsvc $
// $DateTime: 2017/09/18 09:26:13 $


#ifndef CPR_IMAGE_API_H
#define CPR_IMAGE_API_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "DALStdDef.h"
#include "railway.h"

//This function returns the hash of the CPR settings that are related to Cx/Mx.
//On targets with fixed Mx voltages, this hash will factor in the static table of
//Mx voltages as well as the Cx CPR settings.
//On targets with CPR-managed Mx voltages, this hash will factor in the
//Mx CPR settings.
uint32 cpr_cx_mx_settings_checksum(void);

//Function to be called after SMEM is initialized to push out the CPR settings
//to SMEM. These settings are to be picked up by the RPM CPR driver during boot.
//Must be done before the RPM FW execution begins.
void cpr_externalize_state(void);

uint32 cpr_railway_get_voltage(int railwayRailId, railway_corner railwayCorner);
void cpr_railway_get_supported_corners(int railwayRailId, railway_corner *corners, uint32 *cornerCount );

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // CPR_IMAGE_API_H

