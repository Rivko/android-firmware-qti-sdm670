/*==============================================================================

FILE:      adsppm_utils_settings.h

DESCRIPTION: ADSPPM internal utils settings definitions

This is the internal utils settings for ADSPPM. This is not distributed to any clients that uses
ADSPPM.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/settings/power/qdsp_pm/interface/adsppm_utils_settings.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/15/17   jlg     Initial version
==============================================================================*/

#ifndef ADSPPM_UTILS_SETTINGS_H
#define ADSPPM_UTILS_SETTINGS_H


#define ADSPPM_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define ADSPPM_ARRAY(array) ADSPPM_ARRAY_SIZE(array), &array[0]


//Logging levels
#define ADSPPM_LOG_LEVEL_PROFILE  0
#define ADSPPM_LOG_LEVEL_ERROR    1 //!< Error log messages
#define ADSPPM_LOG_LEVEL_WARNING  2 //!< Warnings
#define ADSPPM_LOG_LEVEL_INFO     3 //!< Critical info messages
#define ADSPPM_LOG_LEVEL_INFO_EXT 4 //!< Extended info messages
#define ADSPPM_LOG_LEVEL_FN_ENTRY 5 //!< Function entry/exit messages
#define ADSPPM_LOG_LEVEL_DEBUG    6 //!< All debug messages
#define ADSPPM_LOG_LEVEL_MAX      7


#endif /* ADSPPM_UTILS_SETTINGS_H */
