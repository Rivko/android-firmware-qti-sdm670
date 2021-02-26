/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


#include "iris_qti_default.h"
#include <unistd.h>
#include <fcntl.h>


iris_qti_led::iris_qti_led()
{
}

iris_qti_led::~iris_qti_led()
{
}

int iris_qti_led::get_version(int &version)
{
	version = IRIS_OEM_LED_VERSION;
	return 0;
}

int iris_qti_led::get_mode(int &led_mode)
{
	led_mode = IRIS_LED_MODE_FLASH;
	return 0;
}

int iris_qti_led::get_intensity_range(int &min, int &max, int &step)
{
	min = 0;
	max = 1;
	step = 1;
	return 0;
}

int iris_qti_led::set_intensity(int intensity)
{
	return -1;
}




iris_qti_cam::iris_qti_cam()
{
}

iris_qti_cam::~iris_qti_cam()
{
}


int iris_qti_cam::get_version(int &version)
{
	version = IRIS_OEM_CAM_VERSION;
	return 0;
}

int iris_qti_cam::get_camera_id(int type, uint32_t &camera_id)
{
	if (type == IRIS_CAM_TYPE_IRIS)
		camera_id = 2;
	else
		camera_id = 1;

	return 0;
}

int iris_qti_cam::get_camera_preview_setting(uint32_t &width, uint32_t &height, Rect &crop)
{
	uint32_t display_width = 1920, display_height = 1080, crop_x = 912, crop_y = 220, crop_width = 417, crop_height = 834;
	FILE *cfg_file = NULL;
	char *cfg_file_name = (char *)"/data/misc/iris/display.cfg";

	cfg_file = fopen(cfg_file_name, "r");
	if (!cfg_file) {
		ALOGE("failed to open iris display cfg file %s, use default settings \n", cfg_file_name);
	} else {
		fscanf(cfg_file, "%d %d %d %d %d %d", &display_width, &display_height, &crop_x, &crop_y, &crop_width, &crop_height);
		ALOGD("Get display settings from cfg file %d %d %d %d %d %d", display_width, display_height, crop_x, crop_y, crop_width, crop_height);
	}

	//8998 mtp settings
	width = display_width;
	height = display_height;
	crop = Rect(crop_x, crop_y, crop_x + crop_height * height / width -1, crop_y + crop_height -1);

	return 0;
}

int create_iris_oem_obj(iris_oem_led **led, iris_oem_cam **cam)
{
	*led = new iris_qti_led();
	*cam = new iris_qti_cam();
	return 0;
}


