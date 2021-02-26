/*
 * Copyright (c) 2016-2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

extern "C"{
#include "configuration_generator_apis.h"
}
#include <cstddef>
#include <string.h>
#include <json/json.h>
#include "json_configuration_types.h"
#include "configuration_generator.h"
extern "C" enum JSONVAStatus configuration_generator_init(void ** handle)
{
    ConfigurationGenerator * configuration_generagor_handle =
        new ConfigurationGenerator();
    *handle = (void*)configuration_generagor_handle;
    if (configuration_generagor_handle == NULL)
        return JSON_VA_INVALID;
    else
        return JSON_VA_OK;
}
extern "C" enum JSONVAStatus configuration_generator_deinit(void * handle)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *) handle;
    delete configuration_generagor_handle;
    return JSON_VA_OK;
}

JSONVAStatus ConfigurationGenerateAddZone(void * handle, JSONVAZone zone)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->AddZone(zone);
}
JSONVAStatus ConfigurationGenerateAddAtomicRule(void * handle,
    JSONVAAtomicRule atomic_rule)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->AddAtomicRule(atomic_rule);
}
JSONVAStatus ConfigurationGenerateAddRule(void * handle, JSONVARule rule)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->AddRule(rule);
}
JSONVAStatus ConfigurationGenerateSetSceneCalibration(void * handle,
    JSONVASceneCalibration  scene_calibration)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->SetSceneCalibration(&scene_calibration);
}
JSONVAStatus ConfigurationGenerateGetSceneCalibration(void * handle,
    JSONVASceneCalibration * scene_calibration)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->GetSceneCalibration(
        scene_calibration);
}
JSONVAStatus ConfigurationGenerateToJSON(void * handle, Json::Value &node)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->ToJSON(node);
}
JSONVAStatus ConfigurationGenerateToJSON(void * handle)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->ToJSON();
}
JSONVAStatus ConfigurationGenerateSerialize(void * handle, std::string &doc)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->Serialize(doc);
}
JSONVAStatus ConfigurationGenerateSerialize(void * handle, Json::Value node,
    std::string &doc)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->Serialize(node,doc);
}

extern "C" enum JSONVAStatus configuration_generator_serialize(void * handle, struct JSONConfiguration * configuration, char **doc)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    configuration_generagor_handle->Clear();
#if (VA_CONFIG_VERSION==0)
    configuration_generagor_handle->SetGeneralConfiguration(
        configuration->general_configuration);
    for (int i = 0; i < configuration->cnt_lines; i++){
        configuration_generagor_handle->AddLine(configuration->lines[i]);
    }
    for (int i = 0; i < configuration->cnt_extension; i++){
        configuration_generagor_handle->AddExtension(configuration->extension[i]);
    }
#endif
#if (VA_CONFIG_VERSION==1)
    strcpy(configuration_generagor_handle->id_, configuration->id);
    configuration_generagor_handle->time_stamp_ = configuration->time_stamp;
    strcpy(configuration_generagor_handle->version_, configuration->version);

#endif
    configuration_generagor_handle->SetSceneCalibration(
        &(configuration->scene_calibration));
    for (uint32_t i = 0; i < configuration->cnt_zones; i++){
        configuration_generagor_handle->AddZone(configuration->zones[i]);
    }
    for (uint32_t i = 0; i < configuration->cnt_atomic_rules; i++){
        configuration_generagor_handle->AddAtomicRule(configuration->atomic_rules[i]);
    }
    for (uint32_t i = 0; i < configuration->cnt_rules; i++){
        configuration_generagor_handle->AddRule(configuration->rules[i]);
    }
    //configuration_generagor_handle->ToJSON();
    configuration_generagor_handle->Serialize(configuration_generagor_handle->doc_);
    *doc = (char*)configuration_generagor_handle->doc_.c_str();
    return JSON_VA_OK;
}
#if (VA_CONFIG_VERSION==0)
JSONVAStatus ConfigurationGenerateSetGeneralConfiguration(void * handle,
    JSONGeneralConfiguration general_configuration)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->SetGeneralConfiguration(
        general_configuration);
}
JSONVAStatus ConfigurationGenerateAddLine(void * handle, JSONVALine line)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->AddLine(line);
}
JSONVAStatus ConfigurationGenerateAddExtension(void * handle,
    JSONVAConfigExtension extension)
{
    ConfigurationGenerator * configuration_generagor_handle =
        (ConfigurationGenerator *)handle;
    return configuration_generagor_handle->AddExtension(extension);
}
#endif
