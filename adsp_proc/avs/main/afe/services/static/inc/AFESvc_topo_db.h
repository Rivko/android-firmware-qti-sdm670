/** @file AFESvc_db_if.h

 @brief This file contains AFE Service database for modules 
    	supported

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFESvc_topo_db.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 4/18/2014 mk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2014 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

#ifndef _AFESVC_TOPO_DB_H_
#define _AFESVC_TOPO_DB_H_
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "qurt_elite_types.h"
#include "Elite_CAPI_V2.h"
#include "EliteTopology_db_if.h"
#include "EliteAprIf.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


typedef struct afe_svc_capi_v2_info_t
{
   uint32_t 						module_id;
									/**< Module ID. */
   capi_v2_get_static_properties_f 	get_static_prop_f;   
									/**< function pointer to get memory requirements of library. */
   capi_v2_init_f 					init_f;
									/**< function pointer to init the appi module. */
} afe_svc_capi_v2_info_t;


ADSPResult afe_add_topologies_handler(elite_apr_packet_t* pkt_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_AFESVC_TOPO_DB_H_



