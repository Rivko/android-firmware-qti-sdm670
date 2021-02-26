/*=============================================================================
@file CoreIni.h
  
This file provides methods for creating an INI config
object, in order to access ini files through the
CoreConfig subsystem.

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/inc/CoreIni.h#1 $
=============================================================================*/
#ifndef CORE_INI_H
#define CORE_INI_H

#include "CoreConfig.h"

/**
 * CoreIni_ConfigCreate
 *
 * @brief Creates a CoreIniConfig object.  This function will 
 *        attempt to open the file passed in, and will fill in
 *        the fileHandle if successful.
 *  
 * @param filename: Name of the ini file to open.
 * 
 * @return NULL if an error occurred.  Otherwise, a valid 
 *         CoreConfigHandle is returned.
 */ 
CoreConfigHandle CoreIni_ConfigCreate( const char *filename );

/**
 * CoreIni_ConfigDestroy
 *
 * @brief Destroys a CoreIniConfig object.  Closes the file 
 *        associated with fileHandle, and frees the memory used
 *        by the object.
 *  
 * @param config: Handle to the CoreConfig object.
 * 
 * @return CORE_CONFIG_SUCCESS on successful destruction of the 
 *         object.
 */ 
unsigned int CoreIni_ConfigDestroy( CoreConfigHandle config );

#endif /* CORE_INI_H */
