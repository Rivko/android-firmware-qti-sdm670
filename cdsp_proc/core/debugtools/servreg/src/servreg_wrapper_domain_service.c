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
Copyright (c) 2016 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/servreg/src/servreg_wrapper_domain_service.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "string.h"
#include "servreg_common.h"
#include "servreg_monitor_qurt.h"
#include "servreg_locator.h"
#include "servreg_wrapper_domain_service.h"
#include "err.h"
#include "tms_utils_msg.h"
#include "gen_cb_ctxt.h"

void servreg_wrapper_check_enabled_services(void * data)
{
  SERVREG_DL_HANDLE dlhandle     = SERVREG_NULL;
  SERVREG_NAME      domain       = SERVREG_NULL;
  SERVREG_RESULT    ret          = SERVREG_FAILURE;
  uint32_t          domain_count = 0;
  uint32_t          iter;
 
  sr_wrapper_cb_data_t * wrapper_data = data;
 
  dlhandle = servreg_alloc_domainlist_handle();
  
  if(SERVREG_NULL == dlhandle)
  {
    wrapper_data->cb( SERVREG_FALSE, wrapper_data);
    return;
  }

  ret = servreg_get_domainlist(wrapper_data->service, dlhandle);
  
  if(SERVREG_SUCCESS != ret)
  {
    wrapper_data->cb( SERVREG_FALSE, wrapper_data);
    servreg_free_domainlist_handle(dlhandle);
    return;
  }

  domain_count = servreg_get_num_entries(dlhandle);
  
  if(domain_count <= 0)
  {
    wrapper_data->cb( SERVREG_FALSE, wrapper_data);
    servreg_free_domainlist_handle(dlhandle);
    return;
  }

  for (iter = 0; iter<domain_count; iter++)
  {
    ret = servreg_get_entry(dlhandle, iter + 1, &domain);
    
    if(SERVREG_SUCCESS != ret)
    {
      wrapper_data->cb( SERVREG_FALSE, wrapper_data);
      servreg_free_domainlist_handle(dlhandle);
      return;
    }

    if(strncmp(domain, wrapper_data->domain, strlen(wrapper_data->domain) ) == 0)
    {
      wrapper_data->cb( SERVREG_TRUE, wrapper_data);
      servreg_free_domainlist_handle(dlhandle);
      return;
    }
  }

  wrapper_data->cb( SERVREG_FALSE, wrapper_data);
  servreg_free_domainlist_handle(dlhandle);
}

void server_wrapper_handler( SERVREG_CB_PARAM data, SERVREG_CB_PARAM ignore )
{
  gen_cb_ctxt_invoke_cb( servreg_wrapper_check_enabled_services, (void*)data);
}

/**
Looks into for a particular service and domain once service locator becomes available
@return
None
*/

void servreg_wrapper_domain_service(sr_wrapper_cb_data_t * wrapper_data )
{
  if (SERVREG_SERVICE_STATE_UP == servreg_get_servloc_availability())
  {
    gen_cb_ctxt_invoke_cb(servreg_wrapper_check_enabled_services, (void*)wrapper_data);
  }
  else
  {
     if (SERVREG_SUCCESS != servreg_register_servloc_availability_cb( server_wrapper_handler, 
                                                                    (SERVREG_CB_PARAM)wrapper_data, NULL))
     {
       TMS_MSG_HIGH( "Failed to register for locator availability");
     }
  }
}
