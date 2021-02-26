/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIGURATION_TYPES_H_
#define CONFIGURATION_TYPES_H_
#include <stdint.h>
#include <vector>
extern "C"{
#include "json_common_types.h"
#include "json_configuration.h"
}

/**
* @brief
* defines the structure of a complete configuration
**/
struct JSONConfigurationContent
{
#if (VA_CONFIG_VERSION==0)
    JSONGeneralConfiguration general_configuration;		///<
    std::vector<JSONVAZone> zones;						///<
    std::vector<JSONVALine> lines;						///<
    std::vector<JSONVAAtomicRule> atomic_rules;			///<
    std::vector<JSONVARule> rules;						///<
	std::vector<JSONVAConfigExtension> extension;				///<
    JSONVASceneCalibration  scene_calibration;   ///<
#endif
#if (VA_CONFIG_VERSION==1)
    char version[24];           ///<the version number of the VA Manager
    char id[UUID_LENGTH];	    ///<the VAUUID of the configuration
    uint64_t time_stamp;	    ///<Linux time stamp
    int64_t reserve[5];
    char reserve_str[5][VACONFIG_NAME_LEN];
    std::vector<JSONVAZone> zones;				///<
    std::vector<JSONVAAtomicRule> atomic_rules;			///<
    std::vector<JSONVARule> rules;						///<
    JSONVASceneCalibration  scene_calibration;   ///<
#endif
};

#endif
