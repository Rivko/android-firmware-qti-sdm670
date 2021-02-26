/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef JSON_COMMON_TYPES_H_
#define JSON_COMMON_TYPES_H_
#include <stdint.h>

/**
* @brief
*  the return type of the VAM APIs
**/
enum JSONVAStatus{
    JSON_VA_OK = 0, ///< the call is successful
    JSON_VA_INVALID, ///< the call is failed
    JSON_VA_END_OF_FILE///<end of file is detected
};


/**
* @brief
defines the length unit of the physical size
**/

enum VALengthUnit{
    VA_UNIT_MM = 0,   ///< unit in millimeter
    VA_UNIT_INCH = 1  ///< unit in inch
};

/**
* @brief
defines the object categories the VA Manager provides
**/
enum VACategory{
    VA_CATEGORY_UNKNOWN = 0,   ///< unidentified category
    VA_CATEGORY_PEOPLE,        ///< person
    VA_CATEGORY_VEHICLE,       ///< vehicle
    VA_CATEGORY_FACE,          ///< face
    VA_CATEGORY_MAX   ///< the number of categories supported
};

//
///**
//* @brief
//defines the event types the VA Manager provides
//**/
//enum VAEventType{
//    METADATA_EVENT_CAMERA_TAMPER_DETECTED = 1,	/**< camera tamper detected*/
//    METADATA_EVENT_MOTION_DETECTED,		/**< camera tamper detected*/
//    METADATA_EVENT_INTRUSION_DETECTED,    /**< intrusion detected */
//    METADATA_EVENT_LINECROSSED,			/**< line crossed */
//    METADATA_EVENT_OBJECT_COUNTED,		/**< object counted*/
//    METADATA_EVENT_LOITERING_DETECTED,	/**< loitering detected*/
//    METADATA_EVENT_FACE_DETECTED,			/**< face detected*/
//    METADATA_EVENT_FACE_RECOGNIZED,		/**< face recognized*/
//    METADATA_EVENT_OBJECT_CLASSIFIED,		/**< object classified*/
//    METADATA_EVENT_UNINITIALIZED          /**< uninitialized */
//};

/**
* @brief
defines the status of a rule or atomic rule.
**/
enum VARuleStatus{
    VA_DISABLED = 0, ///<the rule or atomic rule is disabled
    VA_ENABLED		///<the rule or atomic rule is enabled
};

/**
* @brief
defines the moving directions of the objects the VA Manager provides.
The moving directions defined are corresponding to the display
**/
enum VADir{
    VA_DIR_ANY = 0,         ///< moving in any direction
    VA_DIR_UP,          ///< moving up
    VA_DIR_UP_RIGHT,    ///< moving up right
    VA_DIR_RIGHT,       ///< moving right
    VA_DIR_DOWN_RIGHT,  ///< moving down right
    VA_DIR_DOWN,        ///< moving down
    VA_DIR_DOWN_left,   ///< moving down left
    VA_DIR_LEFT,        ///< moving left
    VA_DIR_UP_LEFT,     ///< moving up left
    VA_DIR_LINE_IN,		///< moving in
    VA_DIR_LINE_OUT,	///< moving out
    VA_DIR_STILL,		///< not moving
    VA_DIR_NUM          ///< the number of directions
};
/**
* @brief
*    defines the actons in the object tree
**/
enum ObjectTreeAction{
    OBJECT_TREE_MERGED = 0, ///< multiple objects merged into one
    OBJECT_TREE_SPLIT,		///< one object split into multiple objects
    OBJECT_TREE_CREATED,	///< one new object is created
    OBJECT_TREE_DELETED,	///< one object is deleted
    OBJECT_TREE_RENAMED		///< one object is renamed to another object
};
/**
* @brief
*    enumerate the scene types
**/
enum SceneType{
    SCENE_TYPE_INDOOR = 0, ///< indoor scene
    SCENE_TYPE_OUTDOOR	   ///< outdoor scene
};
/**
* @brief
*    enumerate the zone types
**/
enum ZoneType{
    ZONE_TYPE_INCLUDE = 0,
    ZONE_TYPE_EXCLUDE,
    LINE_TYPE_BOTH_DIR,
    LINE_TYPE_DIR,
    LINE_TYPE_IN,
    LINE_TYPE_OUT,
};

/**
* brief
* define the types of the tamper detected
**/
enum CameraTamperType{
    TAMPER_TYPE_MOVED = 0, ///< camera is moved
    TAMPER_TYPE_COVERED,   ///< camera is covered
    TAMPER_TYPE_DEFOCUSED, ///< camera is defocused
};
/**
* @brief
*    VASize defines the size of the object.
**/
struct VASize{
    int32_t width;  ///< width of an object
    int32_t height; ///< height of an object
};

/**
* brief
* define a point in the scene
**/
struct VAPoint{
    int32_t x; ///<x coordinate of the point
    int32_t y; ///<y coordinate of the point
};
/**
* brief
* define a 3D point in the world coordinate system
**/
struct VAPoint3D{
    int32_t x; ///<x coordinate of the point
    int32_t y; ///<y coordinate of the point
    int32_t z; ///<z coordinate of the point
};

/**
* @brief
* defines the location of a rectangle
**/
struct VARect{
    int32_t x;      //!< the x coordinate of  top left corner of the rectangle
    int32_t y;      //!< the y coordinate of  top left corner of the rectangle
    int32_t width;  //!< the width of the rectangle
    int32_t height; //!< the height of the rectangle
};
/**
* @brief
* defines the location of a rectangle and its associated physical height.
**/
struct CalibObject{
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    float  physical_height;
};


#endif
