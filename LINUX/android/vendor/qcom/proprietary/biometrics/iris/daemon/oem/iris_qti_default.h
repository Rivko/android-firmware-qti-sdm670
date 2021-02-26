/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __IRIS_QTI_DEFAULT_H__
#define __IRIS_QTI_DEFAULT_H__

#include "iris_oem.h"

class iris_qti_led : public iris_oem_led {
public:
	iris_qti_led();
	virtual ~iris_qti_led();

	virtual int get_version(int &version);
	virtual int get_mode(int &led_mode);
	virtual int get_intensity_range(int &min, int &max, int &step);
	virtual int set_intensity(int intensity);
};

class iris_qti_cam : public iris_oem_cam {
public:
	iris_qti_cam();
	virtual ~iris_qti_cam();

	virtual int get_version(int &version);
	virtual int get_camera_id(int type, uint32_t &camera_id);
	virtual int get_camera_preview_setting(uint32_t &width, uint32_t &height, Rect &crop);
};


#endif

