#ifndef SERVREG_LOCALDB_H
#define SERVREG_LOCALDB_H
/*
#============================================================================
#  Name:                                                                     
#    servreg_localdb.h 
#
#  Description:                                                              
#    None 
#                                                                            
# Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.        
#============================================================================
*/
#include "stdlib.h"
#include "servreg_common.h"
#include "servreg_internal.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define SERVREG_PREFIX_NAME "sr:"

struct servreg_entry_s
{
   char * service_name;
   unsigned int service_data_valid;
   unsigned int service_data;
};
typedef struct servreg_entry_s servreg_entry_t, * servreg_entry_p;

/** =====================================================================
 * Function:
 *     servreg_get_local_instance_id
 *
 * Description:
 *     Function to get the local qmi instance id
 *     Queries the local database
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     qmi_instance_id for the notifier module
 * =====================================================================  */
uint32_t servreg_get_local_instance_id(void);

/** =====================================================================
 * Function:
 *     servreg_get_service_list_count
 *
 * Description:
 *     Function to get the local qmi instance id
 *     Queries the local database
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     qmi_instance_id for the notifier module
 * =====================================================================  */
uint32_t servreg_get_service_list_count(void);

/** =====================================================================
 * Function:
 *     servreg_name_check
 *
 * Description:
 *     This function checks if the given name is a valid service name or not.
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     SERVREG_SUCCESS       : If name is valid
 *     SERVREG_INVALID_PARAM : If name is invalid
 * =====================================================================  */
SERVREG_RESULT servreg_name_check(SERVREG_NAME domain, SERVREG_NAME service);

/** =====================================================================
 * Function:
 *     servreg_create_rcesn_name
 *
 * Description:
 *     This function takes in the service name and returns the rcesn sync name
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     rcecb sync name : "sr:soc/domain/subdomain/provider/service" or just "sr:soc/domain/subdomain" 
 * =====================================================================  */
SERVREG_NAME servreg_create_rcesn_name(SERVREG_NAME name);

/** =====================================================================
 * Function:
 *     servreg_create_sync_name
 *
 * Description:
 *     This function takes in the service name and returns the rcecb sync name
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     rcecb sync name : "sr_xx:soc/domain/subdomain/provider/service" or just "sr_xx:soc/domain/subdomain" 
 *                        sr_xx : sr_up or sr_down supported for now
 * =====================================================================  */
SERVREG_NAME servreg_create_sync_name(SERVREG_NAME name, SERVREG_SERVICE_STATE state);

/** =====================================================================
 * Function:
 *     servreg_create_ssr_name
 *
 * Description:
 *     This function takes in the service name and returns the before shutdown ssr message name
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *     state : state of the service. UP or DOWN
 *
 * Returns:
 *     ssr name : "ssr:####:before_shutdown" or "ssr:####:after_powerup" #### will be the name extracted from the input param domain
 * =====================================================================  */
SERVREG_NAME servreg_create_ssr_name(SERVREG_NAME domain, SERVREG_SERVICE_STATE state);

/** =====================================================================
 * Function:
 *     servreg_is_domain_local
 *
 * Description:
 *     This function checks if the domain is local or remote for root-pd vs user-pd
 *     if root-pd : name is local if "soc/domain" matches with that of the root-pd's "soc/domain"
 *     if user-pd : local if "soc/domian/subdomain" scopes matches with the local "soc/domian/subdomain" string
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     TRUE  : If domain is local
 *     FALSE : If domain is remote
 * =====================================================================  */
SERVREG_BOOL servreg_is_domain_local(SERVREG_NAME name);

/** =====================================================================
 * Function:
 *     servreg_is_service_local
 *
 * Description:
 *     This function checks if the service is local or remote for root-pd and user-pd
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     TRUE  : If service is local
 *     FALSE : If service is remote
 * =====================================================================  */
SERVREG_BOOL servreg_is_service_local(SERVREG_NAME name);

/** =====================================================================
 * Function:
 *     servreg_concat
 *
 * Description:
 *     This functions concatenates "soc/domain/subdomain" and "provider/service"
 *     string
 *
 * Parameters:
 *     domain : "soc/domain/subdomain" info
 *     service : "provider/service" info
 *
 * Returns:
 *     concatenated name : "soc/domain/subdomain/provider/service"
 * =====================================================================  */
SERVREG_NAME servreg_concat(SERVREG_NAME domain, SERVREG_NAME service);

#if defined(__cplusplus)
}
#endif

#endif
