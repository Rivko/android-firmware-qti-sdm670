/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "simple_scene_calibration.h"

#include <iostream>
#include <fstream>
//#include "fastcv.h"
#include <json/json.h>
#include <stdio.h>
#include "vamath.h"
#include "json_common_types.h"


SimpleSceneCalibration::SimpleSceneCalibration() : length_unit_(VA_UNIT_MM), fixed_height_(1)
{
}

SimpleSceneCalibration::~SimpleSceneCalibration()
{
}

JSONVAStatus SimpleSceneCalibration::AddObject(VARect object, float physical_height)
{
	CalibObject calib_object;
	calib_object.x = object.x;
	calib_object.y = object.y;
	calib_object.width = object.width;
	calib_object.height = object.height;
	calib_object.physical_height = physical_height;
	objects_.push_back(calib_object);
	return JSON_VA_OK;
}

JSONVAStatus SimpleSceneCalibration::Clear()
{
	objects_.clear();
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::AddObject(CalibObject calib_object)
{
	objects_.push_back(calib_object);
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::GetCalibObjects(std::vector<CalibObject> &objects)
{
	objects = objects_;
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::CalibrateLeastSquares()
{
	float sx2 = 0;
	float sxy = 0;
	float sx = 0;
	float sy2 = 0;
	float sy = 0;
	float sxz = 0;
	float syz = 0;
	float sz = 0;
	for (unsigned int i = 0; i < objects_.size(); i++)
	{
		CalibObject object = objects_[i];
		float height = object.height * fixed_height_ / object.physical_height;
		sx2 += object.x * object.x;
		sxy += object.x * (object.y + object.height);
		sx += object.x;
		sy2 += (object.y + object.height) *(object.y + object.height);
		sy += (object.y + object.height);
		sxz += object.x * height;
		syz += (object.y + object.height) * height;
		sz += height;
	}
	float h_mat[9] = { sx2, sxy, sx, sxy, sy2, sy, sx, sy, (float)objects_.size()};
	float h_inv_mat[9];
	float p[3] = { sxz, syz, sz };
	MatrixInverse3x3((float *)h_mat, (float *)h_inv_mat);
	a_ = 0;
	b_ = 0;
	c_ = 0;
	for (int i = 0; i < 3; i++)
	{
		a_ += h_inv_mat[i] * p[i];
		b_ += h_inv_mat[3+i] * p[i];
		c_ += h_inv_mat[6+i] * p[i];
	}
#ifdef DEBUG_
	for (int i = 0; i < 9; i++)
		printf("%f   %f\n", h_mat[i], h_inv_mat[i]);
	printf("abc:%f  %f  %f\n", a_, b_, c_);
#endif
	return JSON_VA_OK;
}

JSONVAStatus SimpleSceneCalibration::CalculatePhysicalSize(VARect object, VASize & size)
{
	float xy0, xy1,xy2;
	xy0 = (float)(object.x + object.width / 2);
	xy1 = (float)(object.y + object.height - 2);
	xy2 = 1;
	float z = xy0*a_ + xy1*b_ + xy2*c_;
	if (z > 0)
	{
		size.height = (int)(object.height * fixed_height_ / z);
		size.width = (int)(size.height * object.width / object.height);
	}
	else
	{
		size.width = 0;
		size.height = 0;
	}
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::GetLengthUnit(VALengthUnit &length_unit)
{
	length_unit = length_unit_;
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::SetLengthUnit(VALengthUnit length_unit)
{
	length_unit_ = length_unit;
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::Serialize(Json::Value calib_json, std::string  &output)
{
	output = calib_json.toStyledString();
	return JSON_VA_OK;
}
JSONVAStatus SimpleSceneCalibration::ToJSON(Json::Value &calib_json)
{
	Json::Value calibration_json(Json::objectValue);
	Json::Value objects_json(Json::arrayValue);

	for (std::vector<CalibObject>::iterator it = objects_.begin(); it != objects_.end(); ++it) {
		Json::Value value(Json::objectValue);
		value["x"] = it->x;
		value["y"] = it->y;
		value["width"] = it->width;
		value["height"] = it->height;
		value["physical_height"] = it->physical_height;
		objects_json.append(value);
	}
	calibration_json["length_unit"] = length_unit_;
	calibration_json["objects"] = objects_json;
	calib_json = calibration_json;
	calibration_json_ = calibration_json;
	return JSON_VA_OK;
}
JSONVAStatus  SimpleSceneCalibration::FromJSON(Json::Value calib_json)
{
	objects_.clear();
	if (calib_json.isMember("length_unit"))
		length_unit_ = (VALengthUnit)calib_json["length_unit"].asInt();

	if (calib_json.isMember("objects"))
	{
		for (Json::Value::iterator it = calib_json["objects"].begin(); it != calib_json["objects"].end(); ++it)
		{
			CalibObject object;
			object.x = (*it)["x"].asInt();
			object.y = (*it)["y"].asInt();
			object.width = (*it)["width"].asInt();
			object.height = (*it)["height"].asInt();
			object.physical_height = (*it)["physical_height"].asFloat();
			objects_.push_back(object);
		}
	}
	return JSON_VA_OK;
}

JSONVAStatus SimpleSceneCalibration::LoadCalibFile(const char * filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	std::string doc;
	if (in)
	{
		in.seekg(0, std::ios::end);
		doc.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&doc[0], doc.size());
		in.close();
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;
		objects_.clear();
		bool parsingSuccessful = reader.parse(doc, root);
		if (!parsingSuccessful) {
		  return JSON_VA_INVALID;
		}
		if (root.isMember("scene_calibration"))
		{
			Json::Value calib_json = root["scene_calibration"];
			FromJSON(calib_json);
			return JSON_VA_OK;
		}
		else
			return JSON_VA_INVALID;
	}
	else
		return JSON_VA_INVALID;
}
