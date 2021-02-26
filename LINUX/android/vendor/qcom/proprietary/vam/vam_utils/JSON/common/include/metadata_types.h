/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_TYPES_H_
#define METADATA_TYPES_H_
#include <stdint.h>
#include <vector>
#include "json_common_structs.h"
extern "C"
{
#include "json_common_types.h"
#include "json_metadata.h"
}


/**
* @brief
* defines the object appearance whihc is from object tracker
**/
struct VAObjectTrackerAppearance
{
    uint32_t physical_width;			///< the physical width of object
    uint32_t physical_height;		///< the physical height of object
	MetadataDir moving_dir;		///< the moving direction of the object
    float moving_speed;			///< the moving speed of the object
    /////up to 16 dominant colors can be defined in the objects
	//VADominantColor colors[16];
	/////< the number of dominant color in the object
    //uint32_t num_dominantColor;
};

/**
* @brief
* defines the object appearance whihc can be used for objet search
**/
struct VAAppearance
{
    struct metadata_rect_t location;			///< location of the object
    enum MetadataCategory object_type;		///< object category
    /// the confidence of the object classification
	uint8_t object_type_confidence;
	/// the object appearance whihc is from object tracker
    struct metadata_tracker_appearance_t  object_tracker_appearance;
};

/**
* @brief
*  defines the on event content used in VAObject
**/
struct OnEvent
{
	/// status of the OnEvent, 1 means the object is associated with events
    uint32_t status;
	std::vector<struct metadata_uuid_t> event_ids; ///< the event_ids of the events
};
/**
* @brief
*  defines the object tree used in VA Manager.
**/
struct VAObjectTree
{
	/// the action of the current object tree
    enum MetadataObjectTreeAction action;
	///the objects before the action. For METADATA_OBJECT_TREE_CREATED,
	///only one "to" element is provided
    std::vector<uint32_t> from;
	/// the objects after the action. For METADATA_OBJECT_TREE_DELETED,
	/// only one "from" element is provided
    std::vector<uint32_t> to;
};

/**
* @brief
*  defines the object  used in VA Manager
**/
struct VAObject
{
    uint32_t object_id;         ///< the object identifier
    std::string engine_type;	///< the module type of the object
	/// the appearance description of the object
	struct metadata_appearance_t appearance_descriptor;
    OnEvent on_event;         ///< if the object is associated with events
    int64_t reserve[5];
    char reserve_str[5][128];
};

/**
* @brief
*  defines the event details for camera_tamper_detected
**/
struct EventDetailsForCameraTamperDetected
{
   enum CameraTamperType tamper_type; ///< the type of camera tamper event
};
/**
* @brief
*  defines the event details for motion_detected
**/
struct EventDetailsForMotionDetected
{
    int motion_activity; ///< the amount of motion detected
    uint32_t object_id; ///< the object ids involved in the event
};
/**
* @brief
*  defines the event details for object tracker related events
**/
struct EventDetailsForTracker
{
    uint32_t object_id; ///< the object id involved in the event
};

/**
* @brief
*  defines the event details for face recognized event
**/
struct EventDetailsForFaceDetected
{
    uint32_t object_id; ///< object id of the detected face
};

/**
* @brief
*  defines the event details for face recognized event
**/
struct EventDetailsForFaceRecognized
{
    struct VAUUID person_id;			 ///< face id of the recognized face
    uint32_t object_id;		 ///< object id of the recognized face
    std::string display_name;///< display name of the recognized face
	struct VAUUID group_id;			 ///< group id of the recognized face
    std::string group_name;  ///< group name of the recognized face
};

/**
* @brief
*  defines the event details for object counted event
**/
struct EventDetailsForObjectCounted
{
    uint32_t count;  ///< the count of the object_counted event
};

/**
* @brief
*  defines the general event details
**/
struct EventDetails
{
    EventDetailsForCameraTamperDetected details_camera_tamper_detected; ///<details for camera tamper detected event
    EventDetailsForMotionDetected details_motion_detected;  ///< details for motion detected event
    EventDetailsForTracker details_object_trackor;			///<details for object trackor related events
    EventDetailsForFaceDetected details_face_detected;		///<details for face detected event
    EventDetailsForFaceRecognized details_face_recognized;  ///<details for face recognized event
    EventDetailsForObjectCounted details_object_counted;	///<details for counting event
};


/**
* @brief
*  defines the atomic event
**/
struct VAAtomicEvent
{
    uint64_t start_time;	///<time stamp of the event which shows the starting time of the event
    VAUUID event_id;			///<the event_id which is a VAUUID for this event
    VAUUID rule_id;			///<the rule_id of the event related to
    enum MetadataEventType event_type; ///< the event_type of the atomic event
    std::string rule_name;		///<the name of the rule
    EventDetails event_details;///<the content of the event
    int64_t reserve[5];
    char reserve_str[5][128];
};
struct CompositeEvent
{
    VAUUID sub_event_id1;
    VAUUID sub_event_id2;
    uint32_t num_sub_events;
};
/**
* @brief
*  defines the object  used in VA Manager
**/
struct VAEvent
{
    uint64_t start_time;	///<time stamp of the event which shows the starting time of the event
    VAUUID event_id;			///<the event_id which is a VAUUID for this event
    VAUUID rule_id;			///<the rule_id of the event related to
    std::string rule_name;	    ///<the name of the rule
    CompositeEvent composite_event;///<the content of the event
    int64_t reserve[5];
    char reserve_str[5][128];
};

/**
* @brief
*  defines the heat_map  used in VA Manager
**/
struct VAHeatMap
{
    uint64_t start_time;		///<start time  of the heat map
    uint64_t end_time;			///<end time  of the heat map
    uint32_t cells_per_row;		///<width of heatmap
    uint32_t rows_per_column;	///<height of heatmap
    uint16_t * data;			///<data of heatmap
};


/**
* brief
* define the extension of the configuration and metadata for third-party engines
**/
struct VAExtension{
	std::string data;			///<the content of extension
	std::string engine_id; ///< the name of the VA engine
};

struct MetadataFrame
{
    VAUUID	 camera_id;	///<the VAUUID of the camera
    uint64_t time_stamp;///<Linux time stamp
    std::vector<VAObject> objects;
    std::vector<VAObjectTree> object_trees;
    std::vector<VAAtomicEvent> atomic_events;
    std::vector<VAEvent> events;
    std::vector<VAHeatMap> heat_maps;
    std::vector<VAExtension> extensions;
    int64_t reserve[5];
    char reserve_str[5][128];
};


#endif
