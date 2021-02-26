/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef METADATA_H_
#define METADATA_H_
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#define UUID_LENGTH (37)
#define NAME_LEN (128)
#define MD_OBJECT_MAX (256)
#define MD_OBJECT_TREE_MAX (256)
#define MD_ATOMIC_EVENT_MAX (64)
#define MD_EVENT_MAX (64)
#define MD_HEAT_MAP_MAX (1)
#define MD_EXTENSION_MAX (10)


/**
* @brief
defines the input/output media types
**/
enum VAMMediaType{
    VAM_FILE = 0,
    VAM_RTSP
};
/**
* @brief
defines the event types the VA Manager provides
**/
enum MetadataEventType{
    METADATA_EVENT_UNINITIALIZED = 0,/**< uninitialized event*/
    METADATA_EVENT_CAMERA_TAMPER_DETECTED = 1,	/**< camera tamper detected*/
    METADATA_EVENT_MOTION_DETECTED = 2,		/**< camera tamper detected*/
    METADATA_EVENT_LINECROSSED = 3,			/**< line crossed */
    METADATA_EVENT_INTRUSION_DETECTED = 4,    /**< intrusion detected */
    METADATA_EVENT_OBJECT_COUNTED = 5,		/**< object counted*/
    METADATA_EVENT_LOITERING_DETECTED = 6,	/**< loitering detected*/
    METADATA_EVENT_FACE_DETECTED = 7,			/**< face detected*/
    METADATA_EVENT_FACE_RECOGNIZED = 8,		/**< face recognized*/
    METADATA_EVENT_OBJECT_CLASSIFIED = 9,	/**< object classified*/
    METADATA_EVENT_OBJECT_DETECTED = 10,    /**< object detected*/
    METADATA_EVENT_COMPOUND_EVENT = 11,     /**<A client-defined compound event*/
    METADATA_EVENT_RESERVED_1 = 12,
    METADATA_EVENT_RESERVED_2 = 13,
    METADATA_EVENT_RESERVED_3 = 14,
    METADATA_EVENT_RESERVED_4 = 15,
    METADATA_EVENT_RESERVED_5 = 16,
    METADATA_EVENT_EXTENSION_RESERVED = 999  /**<extension reserved*/
};

/**
* @brief
defines the object categories the VA Manager provides
**/
enum MetadataCategory{
    METADATA_CATEGORY_UNKNOWN = 0,   ///< unidentified category
    METADATA_CATEGORY_PEOPLE,        ///< person
    METADATA_CATEGORY_VEHICLE,       ///< vehicle
    METADATA_CATEGORY_FACE,          ///< face
    METADATA_CATEGORY_NOTUSED,   ///< unused category
    METADATA_CATEGORY_MAX   ///< the number of categories supported
};


/**
* @brief
defines the moving directions of the objects the VA Manager provides. The moving directions defined are corresponding to the display
**/
enum MetadataDir{
    METADATA_DIR_ANY = 0,     ///< moving in any direction
    METADATA_DIR_UP,          ///< moving up
    METADATA_DIR_UP_RIGHT,    ///< moving up right
    METADATA_DIR_RIGHT,       ///< moving right
    METADATA_DIR_DOWN_RIGHT,  ///< moving down right
    METADATA_DIR_DOWN,        ///< moving down
    METADATA_DIR_DOWN_LEFT,   ///< moving down left
    METADATA_DIR_LEFT,        ///< moving left
    METADATA_DIR_UP_LEFT,     ///< moving up left
    METADATA_DIR_LINE_IN,	   ///< moving in
    METADATA_DIR_LINE_OUT,	   ///< moving out
    METADATA_DIR_STILL,	   ///< not moving
    METADATA_DIR_NUM          ///< the number of directions
};


/**
* @brief
*    defines the actons in the object tree
**/
enum MetadataObjectTreeAction{
    METADATA_OBJECT_TREE_MERGED = 0, ///< multiple objects merged into one
    METADATA_OBJECT_TREE_SPLIT,		///< one object split into multiple objects
    METADATA_OBJECT_TREE_CREATED,	///< one new object is created
    METADATA_OBJECT_TREE_DELETED,	///< one object is deleted
    METADATA_OBJECT_TREE_RENAMED		///< one object is renamed to another object
};

/**
* brief
* define the types of the tamper detected
**/
enum MetadataCameraTamperType{
    METADATA_TAMPER_TYPE_MOVED = 0, ///< camera is moved
    METADATA_TAMPER_TYPE_COVERED,   ///< camera is covered
    METADATA_TAMPER_TYPE_DEFOCUSED, ///< camera is defocused
    METADATA_TAMPER_TYPE_UNKNOWN, ///< camera is defocused
};

/**
* @brief
* define a UUID
**/
struct metadata_uuid_t{
    char uuid[UUID_LENGTH];
};
/**
* @brief
* defines the location of a rectangle
**/
struct metadata_rect_t{
    int32_t x;      //!< the x coordinate of  top left corner of the rectangle
    int32_t y;      //!< the y coordinate of  top left corner of the rectangle
    int32_t width;  //!< the width of the rectangle
    int32_t height; //!< the height of the rectangle
};

/**
* @brief
* defines the object appearance whihc is from object tracker
**/
struct metadata_tracker_appearance_t{
    uint32_t physical_width;	///< the physical width of object
    uint32_t physical_height;	///< the physical height of object
    enum MetadataDir moving_dir;	///< the moving direction of the object
    float moving_speed;			///< the moving speed of the object
};

/**
* @brief
* defines the object appearance which can be used for objet search
**/
struct metadata_appearance_t{
    struct metadata_rect_t location;			///< location of the object
    enum MetadataCategory object_type;		///< object category
    uint8_t object_type_confidence; ///< the confidence of the object classification
    struct metadata_tracker_appearance_t object_tracker_appearance; ///<the object appearance whihc is from object tracker
};


/**
* @brief
*  defines the on event content used in va_engine_object_t
**/
struct metadata_on_event
{
    uint32_t status;	    ///< status of the OnEvent, 1 means the object is associated with some events
    uint32_t num_events;    ///< the number of events that the object is associated with
    struct metadata_uuid_t  event_ids[MD_EVENT_MAX]; ///< the event_ids of the events
};


/**
* @brief
*  defines the object  used in VA Manager
**/
struct metadata_object_t{
    uint32_t object_id;     ///< the object identifier
    char engine_type[64];	///< the module type of the object
    struct metadata_appearance_t appearance_descriptor; ///< the appearance description of the object
    struct metadata_on_event on_event;       ///< if the object is associated with events
    int64_t reserve[5];
    char reserve_str[5][128];
};

/**
* @brief
*  defines the object tree used in VA Manager.
**/
struct metadata_object_tree_t{
    enum MetadataObjectTreeAction action; ///< the action of the current object tree
    uint32_t from[MD_OBJECT_TREE_MAX];   ///< the objects before the action.
    uint32_t num_from;
    uint32_t to[MD_OBJECT_TREE_MAX];	 ///< the objects after the action.
    uint32_t num_to;
};

/**
* @brief
*  defines the event details for camera_tamper_detected
**/
struct metadata_event_details_camera_tamper_t{
    enum MetadataCameraTamperType tamper_type; ///< the type of camera tamper event
};
/**
* @brief
*  defines the event details for motion_detected
**/
struct metadata_event_details_motion_detected_t{
    int motion_activity; ///< the amount of motion detected
    uint32_t object_id; ///< the object ids involved in the event
};
/**
* @brief
*  defines the event details for motion_detected
**/
struct metadata_event_details_object_detected_t{
    uint32_t object_id; ///< the object ids involved in the event
};
/**
* @brief
*  defines the event details for object tracker related events
**/
struct metadata_event_details_tracker_t
{
    uint32_t object_id; ///< the object id involved in the event
};

/**
* @brief
*  defines the event details for face recognized event
**/
struct metadata_event_details_face_detected_t{
    uint32_t object_id; ///< object id of the detected face
};

/**
* @brief
*  defines the event details for face recognized event
**/
struct metadata_event_details_face_recognized_t{
    uint32_t object_id;		 ///< object id of the recognized face
    struct metadata_uuid_t person_id;///< face id of the recognized face
    char display_name[NAME_LEN];///< display name of the recognized face
    struct metadata_uuid_t  group_id;///< group id of the recognized face
    char group_name[NAME_LEN];  ///< group name of the recognized face
};

/**
* @brief
*  defines the event details for object counted event
**/
struct metadata_event_details_object_counted_t{
    uint32_t count;  ///< the count of the object_counted event
};

/**
* @brief
*  defines the general event details
**/

struct metadata_event_details
{
    struct metadata_event_details_camera_tamper_t details_camera_tamper_detected; ///<details for camera tamper detected event
    struct metadata_event_details_motion_detected_t details_motion_detected;  ///< details for motion detected event
    struct metadata_event_details_tracker_t details_object_trackor;			///<details for object trackor related events
    struct metadata_event_details_face_detected_t details_face_detected;		///<details for face detected event
    struct metadata_event_details_face_recognized_t details_face_recognized;  ///<details for face recognized event
    struct metadata_event_details_object_counted_t details_object_counted;	///<details for counting event
};
/**
* @brief
*  defines the atomic event
**/
struct metadata_atomic_event_t{
    uint64_t start_time;	   ///<time stamp of the event
    char rule_name[NAME_LEN];		///<the name of the rule
    struct metadata_uuid_t event_id;  ///<the event_id which is a UUID for this event
    struct metadata_uuid_t rule_id;   ///<the rule_id of the event related to
    enum MetadataEventType event_type; ///< the event_type of the atomic event
    struct metadata_event_details event_details;///<the content of the event
    int64_t reserve[5];
    char reserve_str[5][128];
};
struct  metadata_composite_event_t
{
    struct metadata_uuid_t sub_event_id1;
    struct metadata_uuid_t sub_event_id2;
    uint32_t num_sub_events;
};

/**
* @brief
*  defines the object  used in VA Manager
**/
struct metadata_event_t
{
    uint64_t start_time;	///<time stamp of the event which shows the starting time of the event
    struct metadata_uuid_t event_id;			///<the event_id which is a VAUUID for this event
    struct metadata_uuid_t rule_id;			///<the rule_id of the event related to
    char rule_name[128];	    ///<the name of the rule
    struct metadata_composite_event_t composite_event;///<the content of the event
    int64_t reserve[5];
    char reserve_str[5][128];
};

/**
* @brief
*  defines the heat_map  used in VA Manager
**/
struct metadata_heat_map_t
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
struct metadata_extension_t{
    char engine_id[NAME_LEN];  ///< the name of the VA engine
    char * data;	   ///<the content of extension
    uint32_t size_data;///<the size of the data in bytes
};
/**
* @brief
*  defines the content of metadata frame
**/
struct metadata_content_t{
    uint64_t time_stamp;///<Linux time stamp
    struct metadata_uuid_t camera_id;	///<the UUID of the camera
    struct metadata_object_t *objects;
    struct metadata_object_tree_t *object_trees;
    struct metadata_atomic_event_t *atomic_events;
    struct metadata_event_t *events;
    struct metadata_heat_map_t *heat_maps;
    struct metadata_extension_t *extensions;
    uint32_t cnt_objects;
    uint32_t cnt_object_trees;
    uint32_t cnt_atomic_events;
    uint32_t cnt_events;
    uint32_t cnt_heat_maps;
    uint32_t cnt_extensions;
    uint32_t frame_cnt;
    int64_t reserve[5];
    char reserve_str[5][128];
};
#ifdef __cplusplus
}
#endif


#endif