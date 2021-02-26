/**=============================================================================

*@file
simple_scene_calibration.h

Copyright (c) 2013,2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*/
#ifndef SimpleSceneCalibration_H_
#define SimpleSceneCalibration_H_
#include <cstddef>
#include <json/json.h>
#include <stdio.h>
#include "json_common_structs.h"
#include "json_common_types.h"
#include <vector>


/**
*@class 
*  SimpleSceneCalibration is the class that can recover physical height and width of
*  object from the 2D scene. 
*  If it is used independently, the process is as follows:
*  1. SimpleSceneCalibration scene_calibration;
*  2. scene_calibration.LoadCalibFile (filename);//filename is a JSON formatted VAManager configuration file with scene calibration data
*  3. scene_calibration.CalibrateLeastSquares(); //compute the paramters of calibration
*  4. scene_calibration.CalculatePhysicalSize(VARect object, VASize & size);//compute the physical size of input object 
**/
class SimpleSceneCalibration
{
public:
    SimpleSceneCalibration();
    ~SimpleSceneCalibration();

    /**
    * @brief
    *  clear the content of calibration data
    **/
    JSONVAStatus Clear();
    /**
    * @brief
    *  calibrate the scene with least squares method
    **/
    JSONVAStatus CalibrateLeastSquares();
    /**
    * @brief
    *  calcuate the physical size of the given object
    * @param object the input object location
    * @param size the output physical size of the object
    **/
    JSONVAStatus CalculatePhysicalSize(VARect object, VASize & size);
    /**
    * @brief
    *  add an object for calibration
    * @param object the input object location
    * @param physical_height the physical height of the input object
    **/
    JSONVAStatus AddObject(VARect object, float physical_height);
    /**
    * @brief
    *  add an object for calibration
    * @param calib_object the input object with location and physical height
    **/
    JSONVAStatus AddObject(CalibObject calib_object);
    /**
    * @brief
    *  get the length unit used in calibration
    * @param length_unit the length unit used in calibration
    **/
    JSONVAStatus GetLengthUnit(VALengthUnit &length_unit);
    /**
    * @brief
    *  set the length unit used in calibration
    * @param length_unit the length unit used in calibration
    **/
    JSONVAStatus SetLengthUnit(VALengthUnit length_unit);
    /**
    * @brief
    *  convert the calibration data  to JSON internal structure
    * @param node the root of the JSON internal structure
    **/
    JSONVAStatus ToJSON(Json::Value &node);
    /**
    * @brief
    *  convert the calibration data from JSON internal structure  
    * @param calib_json the root of the JSON internal structure
    **/
    JSONVAStatus FromJSON(Json::Value calib_json);
    /**
    * @brief
    *  convert the calibration data from JSON internal structure to string
    * @param calib_json the root of the JSON internal structure
    * @param output the output string of calibration in JSON format
    **/
    JSONVAStatus Serialize(Json::Value calib_json, std::string  &output);
    /**
    * @brief
    *  load and parse the calibration file
    * @param filename the calibration file
    **/
    JSONVAStatus LoadCalibFile(const char * filename);
    /**
    * @brief
    *  get the calibration objects
    * @param objects the objects for calibration
    **/
    JSONVAStatus GetCalibObjects(std::vector<CalibObject> &objects);
private:
    std::vector<CalibObject> objects_;	///<array of object with physical size for calibration
    VALengthUnit length_unit_;			///<length unit
    float fixed_height_;				///<scale up the length unit for internal calculation
    float a_;							///< coefficient of a in z=a*x+b*y+c
    float b_;							///< coefficient of b in z=a*x+b*y+c
    float c_;							///< coefficient of c in z=a*x+b*y+c
    Json::Value calibration_json_;
};//class SimpleSceneCalibration

#endif
