/**
@file AdspCoreSvc_DynamicLoader_stub.cpp***********************

@brief This file contains the implementation for the ADSP Dynamic Loader.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/dynl/stub_src/AdspCoreSvc_DynamicLoader.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/19/13   DG      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2013-2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AdspCoreSvc_DynamicLoader.h"
#include "elite_thrd_prio.h"

#include "qurt_elite_adsppm_wrapper.h"


typedef struct _adsp_dynamic_loader_t
{
	qurt_elite_thread_t thread;
	qurt_elite_signal_t thread_exit_signal;

   uint32_t adsppm_core_id;
   qurt_elite_adsppm_client_t *adsppm_wrapper;
} adsp_dynamic_loader_t;

adsp_dynamic_loader_t *adsp_dynamic_loader_create(qurt_elite_channel_t *thread_exit_notification_channel)
{
	return NULL;
}

void adsp_dynamic_loader_destroy(adsp_dynamic_loader_t *obj_ptr)
{
	return ;
}

uint32_t adsp_dynamic_loader_get_signal_mask(adsp_dynamic_loader_t *obj_ptr)
{
	return 0;
}




