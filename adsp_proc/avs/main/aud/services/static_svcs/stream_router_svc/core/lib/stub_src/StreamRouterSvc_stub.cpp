/*========================================================================
 *//** @file StreamRouterSvc_stub.cpp
This file contains stub functions for Stream Router Service.

Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/stream_router_svc/core/lib/stub_src/StreamRouterSvc_stub.cpp#1 $

when            who    what, where, why
--------            ---       -------------------------------------------------------
08/07/15    KR      Created file.
========================================================================== */
#include "StreamRouterSvc.h"
#include "adsp_error_codes.h"

ADSPResult stream_router_svc_create(	streamRouterSvcInitParams_t *inputParam, void **handle)
{
	*handle = NULL;
	return ADSP_ENOTIMPL;
}
