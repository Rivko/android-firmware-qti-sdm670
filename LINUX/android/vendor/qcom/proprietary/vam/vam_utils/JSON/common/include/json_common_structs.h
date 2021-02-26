/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef COMMON_STRUCTS_H_
#define COMMON_STRUCTS_H_
#include <stdint.h>
#include <string>
#include <vector>
/**
* @brief
defines the video format for frame input to the VA modules
**/

enum VAVideoFormat{
    VAM_YUV420 = 0, ///< planar YUV420
    VAM_YUV444,     ///< planar YUV444
    VAM_RGB,        ///< planar RGB
    VAM_BGR,        ///< planar BGR
    VAM_HSV         ///< planar HSV
};

/**
* @brief
defines the color space the VA Manager provides
**/

enum VAColorSpace{
    VA_COLORSPACE_YUV = 0,  ///< YUV color space
    VA_COLORSPACE_RGB,      ///< RGB color space
    VA_COLORSPACE_HSV       ///< HSV color space
};

/**
* @brief
* define a VAUUID
**/
struct VAUUID{
    std::string uuid; ///<a string of uuid in the  canonical format using 128-bit
                        ///<hexadecimal text with inserted hyphen characters. eg.
                        ///<de305d54-75b4-431b-adb2-eb6b9e546014
};


#endif
