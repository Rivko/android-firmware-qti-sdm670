/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/utils/inc/inter_module_comm_server.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/19/2014 sabdagir       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   This file contains inter module communication server side apis .

   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

#ifndef _INTER_MODULE_COMM_SERVER_H_
#define _INTER_MODULE_COMM_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "Elite_CAPI_V2_types.h"

/**
 *  function called to initialize intermodule communication server.
 *  @param [in] None.
 *  @return result of type capi_v2_err_t
 */
capi_v2_err_t imc_init(void);

/**
 *  function called to de-initialize intermodule communication server.
 *  @param [in] None.
 *  @return result of type capi_v2_err_t
 */
void imc_deinit(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _INTER_MODULE_COMM_SERVER_H_ */
