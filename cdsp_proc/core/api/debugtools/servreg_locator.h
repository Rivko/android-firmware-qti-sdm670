#ifndef SERVREG_LOCATOR_H
#define SERVREG_LOCATOR_H
/*
#============================================================================
#  Name:
#    servreg_locator.h 
#
#  Description:
#    Service Registry Locator feature. Clients query APSS to get the list of 
#    domains where the service is running on.
#
# Copyright (c) 2015-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
#============================================================================
*/

#include "stdlib.h"
#include "servreg_common.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/** =====================================================================
 * Function:
 *     servreg_get_local_domain
 *
 * Description:
 *     Function to get the local domain info i.e "soc/domain/subdomain" string
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     string containing soc/domain/subdomain info
 * =====================================================================  */
SERVREG_NAME servreg_get_local_domain(void);

/** =====================================================================
 * Function:
 *     servreg_get_local_soc_scope
 *
 * Description:
 *     Function to get the local soc scope info
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     "soc" string
 * =====================================================================  */
SERVREG_NAME servreg_get_local_soc_scope(void);

/** =====================================================================
 * Function:
 *     servreg_get_local_domain_scope
 *
 * Description:
 *     Function to get the local domain scope
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     "domain" string
 * =====================================================================  */
SERVREG_NAME servreg_get_local_domain_scope(void);

/** =====================================================================
 * Function:
 *     servreg_get_local_subdomain_scope
 *
 * Description:
 *     Function to get the local subdomain scope
 *     Queries the local database
 *
 *     Do not modify string return by this API as it is read only memory.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     "subdomain" string
 * =====================================================================  */
SERVREG_NAME servreg_get_local_subdomain_scope(void);

/** =====================================================================
 * Function:
 *     servreg_is_local
 *
 * Description:
 *     This function checks if the domain is local or remote
 *     Givne domain is said to be local if "soc/domian/subdomain" scopes matches with
 *     the local "soc/domian/subdomain" string
 *
 * Parameters:
 *     name : "soc/domain/subdomain/provider/service" or just "soc/domain/subdomain" info
 *
 * Returns:
 *     TRUE  : If domain is local
 *     FALSE : If domain is remote
 * =====================================================================  */
SERVREG_BOOL servreg_is_local(SERVREG_NAME name);

/** =====================================================================
 * Function:
 *     servreg_alloc_domainlist_handle
 *
 * Description:
 *     Allocates domain list handle. 
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     SERVREG_DL_HANDLE : domain list handle
 *                         Check for return value and if it is NOT NULL
 * =====================================================================  */
SERVREG_DL_HANDLE servreg_alloc_domainlist_handle(void);

/** =====================================================================
 * Function:
 *     servreg_get_domainlist
 *
 * Description:
 *     This queries the APSS(HLOS or TN) via a QMI message to get the list of
 *     all domains where the service runs
 *
 * Parameters:
 *    service_name : Service name that the client is interested in i.e "provider/service" info
 *    dlhandle     : Allocated domain list handle.
 *
 * Returns:
 *    Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_get_domainlist(SERVREG_NAME service_name, SERVREG_DL_HANDLE dlhandle);

/** =====================================================================
 * Function:
 *     servreg_get_pdlist
 *
 * Description:
 *     Get the list of all pd's ("soc/domain/subdomain" strings) attached 
 *     to the SR framework in root-pd.
 *     If API used in user-pd, populates nothing.
 *
 * Parameters:
 *    dlhandle     : Allocated domain list handle.
 *
 * Returns:
 *    Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_get_pdlist(SERVREG_DL_HANDLE dlhandle);

/** =====================================================================
 * Function:
 *     servreg_get_num_entries
 *
 * Description:
 *      This gives the total number of domains the service runs on
 *
 * Parameters:
 *      dlhandle :  Allocated handle.
 *
 * Returns:
 *      Total number of domains the service runs on
 * =====================================================================  */
uint32_t servreg_get_num_entries(SERVREG_DL_HANDLE dlhandle);

/** =====================================================================
 * Function:
 *     servreg_get_entry
 *
 * Description:
 *     Given an offset this function returns the domain name
 *
 * Parameters:
 *     dlhandle : Allocated handle.
 *     offset   : offset at which the client wants the domain name.
 *     domain   : Result stored here. The Domain name at the offset.
 *
 * Returns:
 *    Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_get_entry(SERVREG_DL_HANDLE dlhandle, uint32_t offset, char** domain);

/** =====================================================================
 * Function:
 *     servreg_free_domainlist_handle
 *
 * Description:
 *     Deallocates the domain list handle
 *
 * Parameters:
 *     dlhandle : Allocated handle.
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_free_domainlist_handle(SERVREG_DL_HANDLE dlhandle);

/** =====================================================================
 * Function:
 *     servreg_register_servloc_availability_signal
 *
 * Description:
 *     Clients register to get notified when Service Locator feature is available
 *
 * Parameters:
 *     servreg_signal_type : NHLOS specific notification signal type information
 *     servreg_signal      : NHLOS specific notification signal and mask information
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 *
 * Note:
 *     Look at servreg_locator_xxx.h for NHLOS interface API
 * =====================================================================  */
SERVREG_RESULT servreg_register_servloc_availability_signal(SERVREG_SIGEX_TYPE servreg_signal_type, SERVREG_SIGEX servreg_signal);

/** =====================================================================
 * Function:
 *     servreg_register_servloc_availability_cb
 *
 * Description:
 *     Clients register to get notified when Service Locator feature is available
 *
 * Parameters:
 *     callback : Callback function registered
 *     cb_p1    : Callback function parameter
 *     cb_p2    : Callback function parameter 2
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_register_servloc_availability_cb(SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM const cb_p1, SERVREG_CB_PARAM const cb_p2);

/** =====================================================================
 * Function:
 *     servreg_deregister_servloc_availability_cb
 *
 * Description:
 *     Clients deregister if they do not want to get notified via  
 *     callback when Service Locator feature is available
 *
 * Parameters:
 *     callback : Callback function previously registered
 *     cb_p1    : Callback function parameter
 *     cb_p2    : Callback function parameter 2
 *
 * Returns:
 *     Refer to the enum SERVREG_RESULT for list of possible results
 * =====================================================================  */
SERVREG_RESULT servreg_deregister_servloc_availability_cb(SERVREG_FNSIG_CALLBACK const callback, SERVREG_CB_PARAM const cb_p1, SERVREG_CB_PARAM const cb_p2);

/** =====================================================================
 * Function:
 *     servreg_get_servloc_availability
 *
 * Description:
 *     Clients use this function to query the current state of Service Locator feature
 *
 * Parameters:
 *
 * Returns:
 *     Service state of Service Locator feature.
 *     Refer to the enum SERVREG_SERVICE_STATE for list of possible service states.
 * =====================================================================  */
SERVREG_SERVICE_STATE servreg_get_servloc_availability(void);

#if defined(__cplusplus)
}
#endif

#endif
