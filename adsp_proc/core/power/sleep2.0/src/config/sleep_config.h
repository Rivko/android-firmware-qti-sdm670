#ifndef _SLEEP_CONFIG_H_
#define _SLEEP_CONFIG_H_
/*==============================================================================
  FILE:         sleep_config.h
  
  OVERVIEW:     This file provides methods for reading sleep configuration
                data.
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/config/sleep_config.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/*==============================================================================
              EXTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepConfig_readBool
 *
 * @brief Reads the value associated with the parameter and LPRM 
 *        passed in.  This function will return the value as a boolean.
 *  
 * @param lpr_name:  LPR name to read the data for.
 * @param lprm_name: LPRM name to read the data for.
 * @param param:     Parameter to read for the specified 
 *                   LPR/LPRM pair.
 * @param result:    Output buffer to write the boolean value 
 *                   read from the config data.
 * 
 * @return FALSE if an error occurred or either the LPR/LPRM 
 *         pair or parameter were not found. TRUE if the
 *         parameter was found, and the value was returned in
 *         the buffer.
 */ 
uint32 sleepConfig_readBool(char    *lpr_name,
                            char    *lprm_name,
                            char    *param,
                            uint8   *result);

#ifdef __cplusplus
}
#endif

#endif /* _SLEEP_CONFIG_H_ */

