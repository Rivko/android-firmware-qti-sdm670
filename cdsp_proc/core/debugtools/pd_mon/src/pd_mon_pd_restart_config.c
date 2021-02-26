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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/pd_mon/src/pd_mon_pd_restart_config.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "com_dtypes.h"
#include "pd_mon_restart.h"
#include "qurt.h"
#include "servreg_locator.h"
#include "servreg_wrapper_domain_service.h"
#include "tms_utils_msg.h"

#define PDR_SERVREG_SERVICE_NAME             "tms/pdr_enabled"

void pd_mon_pd_restart_check_service(boolean result, sr_wrapper_cb_data_t * wrapper_data)
{
  boolean * restart_config = (boolean*)(wrapper_data)->data;
  *restart_config = FALSE;
  
  if ( result == SERVREG_TRUE )
    *restart_config = TRUE;
  
  free(wrapper_data);
}


/**
Looks into various restart configuration and enables\disables PDR
@return
None
*/

void pd_mon_pd_restart_config(boolean * restart_config)
{
  #ifdef CHECK_ENABLE_PDRESTART
  sr_wrapper_cb_data_t * wrapper_data;
  #endif

  /* Disable by default */
  *restart_config =0;

  #ifdef CHECK_ENABLE_PDRESTART

  /* If PD restart is enabled by DALCFG, return */
  if ( pd_mon_restart_dalcfg_check())
  {
    *restart_config = 1;
    return;
  }

  wrapper_data = calloc(1, sizeof(sr_wrapper_cb_data_t));

  if ( wrapper_data == NULL )
    return;

  wrapper_data->domain  = servreg_get_local_domain();
  wrapper_data->service = PDR_SERVREG_SERVICE_NAME;
  wrapper_data->cb      = pd_mon_pd_restart_check_service;
  wrapper_data->data    = (void*)restart_config;

  servreg_wrapper_domain_service(wrapper_data);
  
  #endif
}
