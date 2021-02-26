/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIGURATION_GENERATOR_APIS_H_
#define CONFIGURATION_GENERATOR_APIS_H_
#include "json_configuration.h"

/**
* @brief initialize the configuration generator and return the transparent handle.
* This call must be called before configuration_generate_serialize() is called.
* @param handle the handle of configuration generator
**/
enum JSONVAStatus configuration_generator_init(void ** handle);
/**
* @brief release memory of the configuration generator. This call must be called
* after all the configuration_generate_serialize() calls are done.
**/
enum JSONVAStatus configuration_generator_deinit(void * handle);
/**
* @brief serialize the configuration content to JSON string
* @param handle the transparent handle of configuration generator
* @param configuration the configuration struc to be serialized
* @param doc the output JSON string
**/
enum  JSONVAStatus configuration_generator_serialize(
    void * handle,
    struct JSONConfiguration * configuration,
    char **doc);

#endif