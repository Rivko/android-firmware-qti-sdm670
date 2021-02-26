/** vi: tw=128 ts=3 sw=3 et
@file pd_dump.c
@brief This file contains the API details for the Protection Domain Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_mon_pd_dump_config.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#include "com_dtypes.h"
#include "pd_mon_dump.h"
#include "qurt.h"
#include "servreg_locator.h"
#include "servreg_wrapper_domain_service.h"
#include "tms_utils_msg.h"

#define PDDUMP_DISABLE_SERVREG_SERVICE_NAME             "tms/pddump_disabled"

void pd_mon_pd_dump_check_service(boolean result, sr_wrapper_cb_data_t * wrapper_data)
{
  boolean * dump_config = (boolean*)(wrapper_data)->data;
  *dump_config = PD_DUMP_ENABLED;
  
  if ( result == SERVREG_TRUE )
    *dump_config = PD_DUMP_DISABLED; 

  free(wrapper_data);
}


/**
Looks into various dump configuration and enables\disables PDR
@return
None
*/

void pd_mon_pd_dump_config(boolean * dump_config)
{
  #ifdef CHECK_ENABLE_PDRESTART
  sr_wrapper_cb_data_t * wrapper_data;
  #endif
 
  /* Enable by default */
  *dump_config = PD_DUMP_ENABLED;

  #ifdef CHECK_ENABLE_PDRESTART

  /* If PD Dump is disabled by DALCFG, return */
  if ( pd_mon_dump_dalcfg_check_is_enabled() == PD_DUMP_DISABLED)
  {
    *dump_config = PD_DUMP_DISABLED;
    return;
  }

  wrapper_data = calloc(1, sizeof(sr_wrapper_cb_data_t));

  if ( wrapper_data == NULL )
    return;

  wrapper_data->domain  = servreg_get_local_domain();
  wrapper_data->service = PDDUMP_DISABLE_SERVREG_SERVICE_NAME;
  wrapper_data->cb      = pd_mon_pd_dump_check_service;
  wrapper_data->data    = (void*)dump_config;

  servreg_wrapper_domain_service(wrapper_data);
  
  #endif
}
