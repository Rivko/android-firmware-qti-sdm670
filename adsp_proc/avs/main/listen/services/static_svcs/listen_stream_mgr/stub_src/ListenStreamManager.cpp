/**
@file ListenStreamMgr.cpp

@brief This file contains the implementation for ListenStreamMgr.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/stub_src/ListenStreamManager.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/18/2012  Sudhir      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

/*========================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "ListenStreamMgr.h"
#include "adsp_core_api.h"


/**
 * This function returns the LSM service version
 *
 * @param[out] svc_info_ptr pointer to the service info structure
 * @return error code
 */
ADSPResult lsm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
   return ADSP_ENOTIMPL;
}

/* =======================================================================
**         ListenStreamMgr Instance Create/Destroy/Workloop
** ======================================================================= */

ADSPResult aud_create_stat_lsm_svc (uint32_t dummy,void **handle)
{

   *handle = NULL;

   return ADSP_EOK;
}

ADSPResult lsm_add_static_topos_to_cmn_db(void)
{
   return ADSP_EOK;
}
