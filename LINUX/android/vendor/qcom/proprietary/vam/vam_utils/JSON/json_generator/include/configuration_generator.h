/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIGURATION_GENERATOR_H_
#define CONFIGURATION_GENERATOR_H_
#include <cstddef>
//#include "simple_scene_calibration.h"
#include "json_configuration.h"
#include <json/json.h>

class ConfigurationGenerator{
public:
#if (VA_CONFIG_VERSION==0)
    //set general configuration section
    JSONVAStatus SetGeneralConfiguration(JSONGeneralConfiguration
        general_configuration);
    //get general configurations section
    JSONVAStatus GetGeneralConfiguration(JSONGeneralConfiguration &
        general_configuration);
    //add a line to the configuration
    JSONVAStatus AddLine(JSONVALine line);
    //add an extension to the configuration
    JSONVAStatus AddExtension(JSONVAConfigExtension extension);
#endif
    ConfigurationGenerator();
    ~ConfigurationGenerator();
    //load a text file to a string
    JSONVAStatus LoadFileToString(const char *filename, std::string &contents);
    //clear all the buffers
    JSONVAStatus Clear();
    //add a zone to the configuration
    JSONVAStatus AddZone(JSONVAZone zone);
    //add an atomic rule to the configuration
    JSONVAStatus AddAtomicRule(JSONVAAtomicRule atomic_rule);
    //add a rule to the configuration
    JSONVAStatus AddRule(JSONVARule rule);
    //set elements of scene calibration to the configuration
    JSONVAStatus SetSceneCalibration(JSONVASceneCalibration * scene_calibration);
    //get elements of scene calibration from the configuration
    JSONVAStatus GetSceneCalibration(JSONVASceneCalibration * scene_calibration);
    //convert configuration structure to json internal representation
    JSONVAStatus ToJSON(Json::Value &node);
    //convert configuration structure to json internal representation
    JSONVAStatus ToJSON();
    //convert the json internal representation into json format of content
    JSONVAStatus Serialize(std::string &doc);
    /**
    * convert the json internal representation of node into json format of
    * content
    **/
    JSONVAStatus Serialize(Json::Value node, std::string &doc);
    JSONVAStatus GetConfiguration(struct JSONConfiguration **configuration);
    struct JSONConfiguration configuration_;
    std::string doc_;
    char version_[24];           ///<the version number of the VA Manager
#if (VA_CONFIG_VERSION==1)
    char id_[UUID_LENGTH];	    ///<the VAUUID of the configuration
    uint64_t time_stamp_;	    ///<Linux time stamp
    int64_t reserve_[5];
    char reserveStr_[5][VACONFIG_NAME_LEN];
    struct JSONVASceneCalibration  scene_calibration_;
    std::vector<JSONVAZone> zones_;
    std::vector<JSONVAAtomicRule> atomic_rules_;
    std::vector<JSONVARule> rules_;
#endif
#if (VA_CONFIG_VERSION==0)
    struct JSONVASceneCalibration  scene_calibration_;
    struct JSONGeneralConfiguration general_configuration_;
    std::vector<JSONVAZone> zones_;
    std::vector<JSONVALine> lines_;
    std::vector<JSONVAAtomicRule> atomic_rules_;
    std::vector<JSONVARule> rules_;
    std::vector<JSONVAConfigExtension> extensions_;
#endif
private:
    Json::Value configuration_json_;

};

/**
* @brief add a zone to the configuration
* @param handle the transparent handle of configuration generator
* @param zone the zone to be added to the configuration
**/
JSONVAStatus ConfigurationGenerateAddZone(void * handle, JSONVAZone zone);
/**
* @brief add an atomic rule to the configuration
* @param handle the transparent handle of configuration generator
* @param atomic_rule the atomic_rule to be added to the configuration.
* Atomic rule is the basic element of configuration to trigger atomic event
**/
JSONVAStatus ConfigurationGenerateAddAtomicRule(void * handle,
    JSONVAAtomicRule atomic_rule);
/**
* @brief add a rule to the configuration
* @param handle the transparent handle of configuration generator
* @param rule the rule to be added to the configuration. Rule is also
* called compound rule. It is the combination of atomic rules and other
* predefined rules with a logic operator which be AND, OR, and NOT.
**/
JSONVAStatus ConfigurationGenerateAddRule(void * handle, JSONVARule rule);

/**
* @brief set the elements of scene calibration to the configuration
* @param handle the transparent handle of configuration generator
* @param scene_calibration the content of scene calibration.
**/
JSONVAStatus ConfigurationGenerateSetSceneCalibration(void * handle,
    JSONVASceneCalibration  scene_calibration);
/**
* @brief get the elements of scene calibration to the configuration
* @param handle the transparent handle of configuration generator
* @param scene_calibration the content of scene calibration.
**/
JSONVAStatus ConfigurationGenerateGetSceneCalibration(void * handle,
    JSONVASceneCalibration  &scene_calibration);
/**
* @brief convert configuration structure to json internal representation
* @param handle the transparent handle of configuration generator
* @param node the internal json representation of configuration content.
**/
JSONVAStatus ConfigurationGenerateToJSON(void * handle, Json::Value &node);
/**
* @brief convert configuration structure to json internal representation
* @param handle the transparent handle of configuration generator
**/
JSONVAStatus ConfigurationGenerateToJSON(void * handle);
/**
* @brief serialize the configuration content of a json internal
* representation to a JSON string
* @param handle the transparent handle of configuration generator
* @param node the json internal representation of the configuration
* @param doc the output JSON string
**/
JSONVAStatus ConfigurationGenerateSerialize(void * handle, Json::Value node,
    std::string &doc);

/**
*@brief clear all the buffers that configuration generator uses.
*This should be called every time before a configuration is created
@param handle the transparent handle of configuration generator
**/
JSONVAStatus ConfigurationGenerateClear(void * handle);
#if (VA_CONFIG_VERSION==0)
/**
* @brief set the general configuration section of the configuration
* @param handle the transparent handle of configuration generator
* @param general_configuration the general configuration section of the
* configuration
**/
JSONVAStatus ConfigurationGenerateSetGeneralConfiguration(void * handle,
    JSONGeneralConfiguration general_configuration);
/**
* @brief add a line to the configuration
* @param handle the transparent handle of configuration generator
* @param line the line to be added to the configuration
**/
JSONVAStatus ConfigurationGenerateAddLine(void * handle, JSONVALine line);
/**
* @brief add an extension section to the configuration
* @param handle the transparent handle of configuration generator
* @param extension the extension to be added to the configuration.
* Extension is used to pass the engine specific configuration informaton
* to the engine directly. The VA Manager will not parse the content.
**/
JSONVAStatus ConfigurationGenerateAddExtension(void * handle,
    JSONVAConfigExtension extension);
#endif
#endif //CONFIGURATION_H_
