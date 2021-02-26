/*==============================================================================
@file CoreConfig.h
  
This file provides the public API of the abstract
configuration reader/writer.

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreConfig.h#1 $
==============================================================================*/
#ifndef CORECONFIG_H
#define CORECONFIG_H

/**
 * @brief CORE_CONFIG error codes 
 * 
 * These are the possible error codes that are returned by the 
 * Core Config subsystem. 
 */
#define CORE_CONFIG_SUCCESS                   0x0
#define CORE_CONFIG_ERROR_SECTION_NOT_FOUND   0x1
#define CORE_CONFIG_ERROR_KEY_NOT_FOUND       0x2
#define CORE_CONFIG_ERROR_INSUF_SIZE          0x3

/**
 * CoreConfigHandle 
 *  
 * @brief Handle that is to be passed into all CoreConfig 
 *        functions.  This handle must be obtained by calling 
 *        the create function provided by the subsystem that 
 *        implements the access functions for the specific type 
 *        of configuration that is used (i.e., ini, registry, 
 *        etc.) 
 */ 
typedef struct CoreConfig *CoreConfigHandle;

/**
 * CoreConfig_ReadUint32
 * 
 *  @brief Reads the configuration data for the section and key
 *         passed in.  Returns the data as an unsigned integer.
 * 
 * This function will use the vector table in the config 
 *  parameter to call the appropriate function.
 * 
 *  @param config:  Handle to the CoreConfig object to use. 
 *  @param section: Section to read the parameter from. 
 *  @param key:     Name of the parameter to read. 
 *  @param value:   Buffer to store the value associated with
 *                the key.
 *  @return CORE_CONFIG_SUCCESS if the key was read
 *          successfully. CORE_CONFIG_ERROR_SECTION_NOT_FOUND if
 *          the section was not found.
 *          CORE_CONFIG_ERROR_KEY_NOT_FOUND if the key was not
 *          found.  CORE_CONFIG_ERROR_INSUF_SIZE if the output
 *          buffer was not big enough for the data in the file.
 */
unsigned int CoreConfig_ReadUint32( CoreConfigHandle config, 
                                    char *section, char *key,
                                    unsigned int *value );

/**
 * CoreConfig_ReadString
 * 
 *  @brief Reads the configuration data for the section and key
 *         passed in.  Returns the data as a string.
 * 
 * This function will use the vector table in the config 
 *  parameter to call the appropriate function.
 * 
 *  @param config:  Handle to the CoreConfig object to use. 
 *  @param section: Section to read the parameter from. 
 *  @param key:     Name of the parameter to read. 
 *  @param string:  Buffer to store the string associated with
 *                  the key.
 *  @param size:    Input/Output -- Tells the function how big
 *                  the string buffer is.  The function will
 *                  output the number of characters that were
 *                  actually written to the string buffer, or if
 *                  there not enough room, will return the
 *                  number of characters that were needed.
 *  @return CORE_CONFIG_SUCCESS if the key was read
 *          successfully. CORE_CONFIG_ERROR_SECTION_NOT_FOUND
 *          if the section was not found.
 *          CORE_CONFIG_ERROR_KEY_NOT_FOUND if the key was not
 *          found. CORE_CONFIG_ERROR_INSUF_SIZE if the output
 *          buffer was not big enough for the data in the file.
 */
unsigned int CoreConfig_ReadString( CoreConfigHandle config,
                                    char *section, char *key,
                                    char *string, unsigned int *size );

/**
 * CoreConfig_ReadBool
 *
 * @brief Reads the configuration data for the section and key 
 *        passed in.  Returns the data as an unsigned char.
 *  
 * This function will call CoreConfig_ReadUint32, and then 
 * convert the output to a '1' or a '0'. 
 *
 * @param config:  Handle to the CoreConfig object to use. 
 * @param section: Section to read the parameter from.
 * @param key:     Name of the parameter to read.
 * @param value:   Buffer to store the value associated with the
 *                 key.
 *  
 * @return CORE_CONFIG_SUCCESS if the key was read successfully. 
 *         CORE_CONFIG_ERROR_SECTION_NOT_FOUND if the section
 *         was not found.
 *         CORE_CONFIG_ERROR_KEY_NOT_FOUND if the key was not
 *         found.
 *         CORE_CONFIG_ERROR_INSUF_SIZE if the output buffer was
 *         not big enough for the data in the file.
 */
unsigned int CoreConfig_ReadBool( CoreConfigHandle config, 
                                  char *section, char *key,
                                  unsigned char *value );

#endif /* CORECONFIG_H */
