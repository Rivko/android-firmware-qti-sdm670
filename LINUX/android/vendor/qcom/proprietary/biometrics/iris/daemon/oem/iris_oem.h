/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __IRIS_OEM_H__
#define __IRIS_OEM_H__

#include <stdint.h>
#include <ui/Rect.h>

using namespace android;

class iris_oem_led {
public:
	enum IRIS_LED_MODE {
		IRIS_LED_MODE_FLASH,
		IRIS_LED_MODE_LED,
		IRIS_LED_MODE_MAX,
	};
public:
	virtual ~iris_oem_led() {};

	virtual int get_version(int &version) = 0;
	virtual int get_mode(int &led_mode) = 0;
	virtual int get_intensity_range(int &min, int &max, int &step) = 0;
	virtual int set_intensity(int intensity) = 0;
public:
	static const int IRIS_OEM_LED_VERSION = 1;
};

class iris_oem_cam {
public:
	/* note: must match with iris_frame_source_type_t */
	enum IRIS_CAM_TYPE {
		IRIS_CAM_TYPE_IRIS,/* iris camera */
		IRIS_CAM_TYPE_PREVIEW,/* preview camera if not same as iris camera */
		IRIS_CAM_TYPE_MAX,
	};

public:
	virtual ~iris_oem_cam() {};

	virtual int get_version(int &version) = 0;
	virtual int get_camera_id(int type, uint32_t &camera_id) = 0;
	/*
	 * transform: combination of NATIVE_WINDOW_TRANSFORM_ in <windows.h>
	 * crop: crop region in preview camera frame to be displayed
	 */
	virtual int get_camera_preview_setting(uint32_t &width, uint32_t &height, Rect &crop);
public:
	static const int IRIS_OEM_CAM_VERSION = 1;
};

extern "C"  int create_iris_oem_obj(iris_oem_led **led, iris_oem_cam **cam);

#endif

