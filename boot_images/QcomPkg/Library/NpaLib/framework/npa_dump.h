/*==============================================================================
@file npa_dump.h

NPA Dump Functionality

Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef NPA_DUMP_H
#define NPA_DUMP_H

#include "ULogFront.h"

/**
 * Type enumeration of commands that may be sent to 
 * npa_generate_dump_log plugin
 */
typedef enum npa_ulog_diag_plugin_command
{
  NPA_GENERATE_DUMP_LOG = 0,
#if defined(WINSIM) || defined(NPA_SELF_TEST)
  NPA_RUN_SELF_TEST,
#endif
  NUMBER_OF_COMMANDS
} npa_ulog_diag_plugin_command;

/**
   @brief npa_dump - Dump a resource or the whole resource graph

   The dump will be done to the ulog named NPA_DUMP_LOG

   Note: The log size is currently fixed.  If the dump exceeds this
   log, this may need to be changed. The intent is for this to be
   replaced with uLogSnapShot
   
   @dump_log : handle to ULog to be filled with dump data
   @param resource_name : name of the resource to be dumped.  If NULL,
   all resources will be dumped
*/
void npa_dump( ULogHandle dump_log, const char *resource_name );

/**
   @brief npa_ulog_diag_plugin - Plugin to ULog for commands given over diag
   Takes an enumerated command for the action to perform.
 
   @param command : npa_ulog_diag_plugin_command enumeration of command to
   pass to the plugin
   
   @return : DAL_SUCCESS on success, DAL_ERROR on error
 */
int npa_ulog_diag_plugin( uint32 command );

#endif /* NPA_DUMP_H */
