/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_
#include "json_metadata.h"
#include "json_event.h"
#include "json_common_types.h"
#include <VAM/vaapi_config.h>
#define VACONFIG_NAME_LEN (128)
#define ZONES_IN_ATOMIC_RULE_MAX (5)
#define ZONE_VERTICES_MAX (16)
#define ZONE_MAX (16)
#define LINES_MAX (16)
#define ATOMIC_RULE_MAX (16)
#define RULE_MAX (16)
#define EXTENSION_MAX (16)
#define CALIB_OBJECT_MAX (32)
#define FACE_RECOGNITION_GROUP_MAX (16)
#define CALIB_OBJECT_MAX (32)
#define EXCLUDE_ZONE_MAX (8)

#define VA_CONFIG_VERSION 1
#if (VA_CONFIG_VERSION == 1)

/**
* @brief
* defines a zone
**/
struct JSONVAZone
{
    char id[UUID_LENGTH];     ///<    the uuid of the zone
    enum ZoneType type;			    ///<    the type of the zone
                                    ///<    0: include_zone, 1: exclude_zone, 2: line, 3: line_in, 4: line_out
    struct VAPoint points[ZONE_VERTICES_MAX]; ///<  the vertices of the zone
    uint32_t cnt_points;            ///<    the number of vertices
};

/**
* @brief
* defines the structure of atomic rule
**/
struct JSONVAAtomicRule
{
    char id[UUID_LENGTH];	        ///< the atomic rule id
    enum JSONEventType event_type;  ///< atomic event type of the atomic rule
    char zone_ids[5][UUID_LENGTH];      ///< zone ids
    uint32_t cnt_zones;                      ///<the number of zones
    uint32_t sensitivity;             ///<sensitivity fo the atomic rule
    uint32_t min_size;                ///<min_size of the object for the rule
    vaapi_scene_type scene_type;      ///< scene_type for the rule
    char name[VACONFIG_NAME_LEN];    ///< the name of atomic rule
    uint32_t status;           ///< if the atomic rule is active or not. 0:disabled, 1:enabled
    int64_t reserve[5];                 ///<reserved fields
    char reserve_str[5][VACONFIG_NAME_LEN];///< reserved strings
};

/**
* @brief
* defines the temporal relationship in subrules
**/
struct VARelationship
{
    float relative_start_time;///<the relative start time from rule_id2 relative to the event from rule_id1
    float relative_end_time;  ///<the relative end time from rule_id2 relative to the event from rule_id1
};

/**
* @brief
* defines the structure of a compound rule
**/
struct JSONVARule
{
    char id[UUID_LENGTH];	///<rule id
    char name[128];        ///<rule name
    uint32_t status;   ///< if the atomic rule is active or not. 0:disabled, 1:enabled
    uint32_t operation;         ///< operation include 0:AND, 1:OR, 2:NOT
    char sub_rule_id1[UUID_LENGTH];	///<rule id of a sub rule1
    char sub_rule_id2[UUID_LENGTH];    ///<rule id of a sub rule2
    struct VARelationship relationship;     ///<the temporal relationship in subrules
    int64_t reserve[5];
    char reserve_str[5][VACONFIG_NAME_LEN];
};

struct JSONVASceneCalibration{
    struct CalibObject objects[CALIB_OBJECT_MAX];
    uint32_t cnt_objects;
    enum VALengthUnit length_unit;
};


/**
* @brief
* defines the structure of a complete configuration
**/
struct JSONConfiguration
{
    uint32_t cnt_zones;         //
    uint32_t cnt_atomic_rules;
    uint32_t cnt_rules;
    char version[24];           ///<the version number of the VA Manager
    char id[UUID_LENGTH];	    ///<the VAUUID of the configuration
    uint64_t time_stamp;	    ///<Linux time stamp
    int64_t reserve[5];
    char reserve_str[5][VACONFIG_NAME_LEN];
    struct JSONVAZone zones[ZONE_MAX];	///<
    struct JSONVAAtomicRule atomic_rules[ATOMIC_RULE_MAX];			///<
    struct JSONVARule rules[RULE_MAX];						///<
    struct JSONVASceneCalibration  scene_calibration;   ///<
};


#endif //VA_CONFIG_VERSION 1
#endif
