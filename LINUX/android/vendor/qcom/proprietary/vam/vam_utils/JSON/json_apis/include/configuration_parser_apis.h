/*
 * Copyright (c) 2016, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef CONFIGURATION_PARSER_APIS_H_
#define CONFIGURATION_PARSER_APIS_H_
#include "json_configuration.h"

/**
* @brief initialize configuration parser and return a transparent handle
* @return  a transparent handle of the configuration parser
**/
enum JSONVAStatus configuration_parser_init(void ** handle);

/**
* @brief Deinitialize the configuration parser
* @param handle  a transparent handle of the configuration parser
**/
enum JSONVAStatus configuration_parser_deinit(void * handle);
/**
* @brief clear the buffers of the configuration parser, This should be
* called before parsing a new configuration string.
* @param handle  a transparent handle of the configuration parser
**/
enum JSONVAStatus configuration_parser_clear(void * handle);
/**
* @brief load a json file to a string
* @param handle  a transparent handle of the configuration parser
* @param file_name the name of JSON file to be loaded
* @param doc_str the string loaded from the JSON file
**/
enum JSONVAStatus configuration_parser_load_file(void * handle, const char *
    file_name, char ** doc_str);
/**
* @brief parse a JSON string to configuration content
* @param handle  a transparent handle of the configuration parser
* @param doc_str the JSON string to be parsed
* @param configuration the configuration parsed from
* the JSON string
**/
enum JSONVAStatus configuration_parser_parse_doc(void * handle, const char* doc_str,
    struct JSONConfiguration ** configuration);
/**
* @brief parse a JSON string to configuration content
* @param handle  a transparent handle of the configuration parser
* @param file_name the name of JSON file to be parsed
* @param configuration the configuration  parsed from
* the JSON string
**/

enum JSONVAStatus configuration_parser_parse_file(void * handle, const char* file_name,
    struct JSONConfiguration ** configuration);

#endif